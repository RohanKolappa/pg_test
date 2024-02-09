/*
 * h264slicedecoder.cpp
 *
 *  Created on: Nov 1, 2010
 *      Author: rkale
 */


#include "h264slicedecoder.hh"
#include "codecdefs.h"
#include "imagemanip.h"
#include "dct.hh"
#include "ipputils.h"
#include "svc.h"
#include "memory_tracker.h"

//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define PRINTF printf
#else
#define PRINTF(...) do {} while (0)
#endif


CH264SliceDecoder::CH264SliceDecoder() :
    CSliceDecoder(eH264)
{
    InitSVLCTable();
    create_quant_tables_4x4(m_quant4_mf, m_dequant4_mf);
}

CH264SliceDecoder::~CH264SliceDecoder()
{

}

bool CH264SliceDecoder::DecodeHeaders(int & iMQuant, int & iDCQuant) {

    if (m_pBitStream->getRemainingBytes() < 4)
        return false;

    if (IsSliceZero()) {
        if (!ParseStartFrameCode(iMQuant, iDCQuant)) {
            return false;
        }
    }
    else {
        if (!ParseStartSliceCode(iMQuant, iDCQuant)) {
            return false;
        }
    }

    m_qp = iMQuant;

    return true;
}

bool CH264SliceDecoder::GetQuantBits(int& a_mQuant, int& a_dcQuant)
{
    unsigned int tmp1;
    m_pBitStream->getBits(6, tmp1);
    a_mQuant = tmp1;
    return true;
}

bool CH264SliceDecoder::DecodeData(uint8_t * pYUV[3], int iMQuant, int iDCQuant, int a_nSliceSize)
{
    int yuvStrides[3] = { a_nSliceSize * 16, a_nSliceSize * 8,
        a_nSliceSize * 8 };
    if (!DecodeSlice(iMQuant, iDCQuant, a_nSliceSize, pYUV,
            yuvStrides)) {
        PRINTF("Unable to decode slice\n");
        return false;
    }
    return true;
}

bool CH264SliceDecoder::DecodeSlice(const int a_nMQuant,
                                const int a_nDCQuant,
                                const size_t a_nSliceSize,
                                uint8_t * a_pYUV[3],
                                int a_yuvStrides[3])
{
    uint8_t *yArrayPos;
    uint8_t *uArrayPos;
    uint8_t *vArrayPos;
    for (int rowNo = 0; rowNo < H264_SLICE_HEIGHT; rowNo++) {
        yArrayPos = a_pYUV[0] + rowNo * 16 * a_yuvStrides[0];
        uArrayPos = a_pYUV[1] + rowNo * 16 * a_yuvStrides[1];
        vArrayPos = a_pYUV[2] + rowNo * 16 * a_yuvStrides[2];


        for (size_t nBlkNo = 0; nBlkNo < a_nSliceSize; nBlkNo++) {

            LoadNewBlock(rowNo, nBlkNo);

            if (!ReadBlockHeaders(nBlkNo, rowNo)) {
                goto decodeslice_fail;
            }

            if (!DecodeLumaBlocks(nBlkNo, rowNo, a_nMQuant, a_nDCQuant, 0,
                    yArrayPos + nBlkNo * 16, a_yuvStrides[0]))
                goto decodeslice_fail;

            if (!DecodeChromaBlocks(nBlkNo, rowNo, a_nMQuant, a_nDCQuant, 4,
                    uArrayPos + nBlkNo * 8, vArrayPos + nBlkNo * 8,
                    a_yuvStrides[1]))
                goto decodeslice_fail;
        }
    }
    return true;

    decodeslice_fail: return false;
}

bool CH264SliceDecoder::ReadBlockHeaders(int aBlkNo, int aRowNo)
{
    // Read predicted modes for luma
    int current_mode;
    for (int idx = 0; idx < 16; idx++) {
        int predicted_mode = mb_predict_intra4x4_mode(idx, m_intra4x4_pred_mode[aBlkNo]);
        if (m_pBitStream->getBitsValue(1)) {
            current_mode = predicted_mode;
        }
        else {
            current_mode = m_pBitStream->getBitsValue(3);
            if (current_mode >= predicted_mode)
                current_mode++;
        }
        m_intra4x4_pred_mode[aBlkNo][x264_scan8[idx]] = current_mode; // for next blocks
    }

    // Read predicted modes for chroma
    for (int idx = 0; idx < 2; idx++) {
        current_mode = GolombReadUE(m_pBitStream);
        if (current_mode > 3) {
            printf("Warning: Bad Chroma predict mode %d!!!\n", idx);
            return false;
        }
        m_chroma_predict_mode[idx] = current_mode;
    }

    // Read coded block pattern
    m_cbp = GolombReadUE(m_pBitStream);
    if (m_cbp >= 48) {
        printf("Warning: Invalid CBP %d\n", m_cbp);
        return false;
        exit(0);
    }
    m_cbp = golomb_to_intra4x4_cbp[m_cbp];


    return true;
}

bool CH264SliceDecoder::DecodeLumaBlocks(
                                int aBlkNo, int aRowNo,
                                const int a_mquant,
                                const int a_dcquant,
                                const int a_comp,
                                uint8_t * pDest,
                                const int aStride)
{
    //printf("Starting Decode Luma\n");
    int luma4x4[16];
    int nonZero;
    int current_mode;

    int i_qscale = m_qp;


    m_nBlocksDecoded++;
    for (int idx = 0; idx < 16; idx++) {

        int x, y;
        int16_t dct4x4[4][4];
        uint8_t *dstPtr;
        x = block_idx_x[idx];
        y = block_idx_y[idx];

        current_mode = mb_pred_mode4x4_valid((aBlkNo == 0), (aRowNo == 0), idx,
                m_intra4x4_pred_mode[aBlkNo][x264_scan8[idx]]);

        // IVLC on the block
        if (m_cbp & (1 << idx/4)) {
            int nonZeroPredict = mb_predict_non_zero_code(idx, m_non_zero_count[aBlkNo]);
            DecodeBlockCavlc(0, luma4x4, 16, nonZeroPredict, nonZero);
            m_non_zero_count[aBlkNo][x264_scan8[idx]] = nonZero;

            //V2dLibImageManip::dumpBlock16(luma4x4, "Luma");
        }
        else {
            dstPtr = pDest + 4 * x + 4 * y * aStride;
            predict_4x4[current_mode](dstPtr, aStride);
            m_non_zero_count[aBlkNo][x264_scan8[idx]] = 0;
            continue;
        }
        // Predict current block
        dstPtr = pDest + 4 * x + 4 * y * aStride;
        predict_4x4[current_mode](dstPtr, aStride);

        // Reverse zig-zag
        unscan_zigzag_4x4full(dct4x4, luma4x4 );

        // De-quantize
        dequant_4x4(dct4x4, m_dequant4_mf, i_qscale);

        // Inverse dct and add back to predicted values
        add4x4_idct(dstPtr, aStride, dct4x4);

    }

    //V2dLibImageManip::dumpBuffer8x8(pDest, aStride, "Output");

    return true;
}

bool CH264SliceDecoder::DecodeChromaBlocks(
                                int aBlkNo, int aRowNo,
                                const int a_mquant,
                                const int a_dcquant,
                                const int a_comp,
                                uint8_t * pDestU,
                                uint8_t * pDestV,
                                const int aStride)
{

    //printf("Starting Decode Chroma\n");
    int chroma_dcU[4];
    int chroma_dcV[4];
    int residual_acU[4][16];
    int residual_acV[4][16];

    int16_t dct2x2U[2][2];
    int16_t dct2x2V[2][2];
    int16_t dct4x4U[4][4][4];
    int16_t dct4x4V[4][4][4];

    int i_qscale = m_qp;


    for(int idx = 0; idx < 2; idx++ ) {

        // printf("fixed mode %d : %d\n", idx, current_mode);

        // Inverse VLC the DC and AC coefficients
        int nonZero;
        int nonZeroPredict = 0;
        if ((m_cbp >> 4) != 0) { // If DC present
            DecodeBlockCavlc(BLOCK_INDEX_CHROMA_DC, chroma_dcU, 4, 0, nonZero);
            DecodeBlockCavlc(BLOCK_INDEX_CHROMA_DC, chroma_dcV, 4, 0, nonZero);
            if ((m_cbp >> 4) & 0x2) { // If AC present too
                for (int i = 0; i < 4; i++) {
                    DecodeBlockCavlc(0, residual_acU[i], 15, nonZeroPredict, nonZero);
                    DecodeBlockCavlc(0, residual_acV[i], 15, nonZeroPredict, nonZero);
                }
            }
        }


        // Predict current block
        uint8_t *dstPtrU = pDestU + idx * 8 * aStride;
        uint8_t *dstPtrV = pDestV + idx * 8 * aStride;

        int current_mode = mb_pred_chromamode_valid((aBlkNo == 0), (aRowNo == 0), idx,
                m_chroma_predict_mode[idx]);
        //printf("Read Predicting using mode %d\n", current_mode);
        predict_8x8c[current_mode](dstPtrU, aStride);
        predict_8x8c[current_mode](dstPtrV, aStride);

        // If DC and AC not present, we are done
        if ((m_cbp >> 4) == 0) {
            continue;
        }

        // Recover the AC coefficients
        for (int i = 0; i < 4; i++) {
            if (!((m_cbp >> 4) & 0x2)) {
                // AC not present
                memset(dct4x4U[i], 0x0, 16 * sizeof(uint16_t));
                memset(dct4x4V[i], 0x0, 16 * sizeof(uint16_t));
                continue;
            }
            // Reverse zig zag
            unscan_zigzag_4x4(dct4x4U[i], residual_acU[i]);
            unscan_zigzag_4x4(dct4x4V[i], residual_acV[i]);

            // De-quant
            dequant_4x4(dct4x4U[i], m_dequant4_mf, i_qscale);
            dequant_4x4(dct4x4V[i], m_dequant4_mf, i_qscale);

        }

        // Reverse zig-zag DC coefficients
        unscan_zigzag_2x2_dc(dct2x2U, chroma_dcU);
        unscan_zigzag_2x2_dc(dct2x2V, chroma_dcV);

        // Dequantize the DC coefficients
        dequant_2x2_dc(dct2x2U, m_dequant4_mf, i_qscale);
        dequant_2x2_dc(dct2x2V, m_dequant4_mf, i_qscale);

        // Inverse the DC coefficients. idct function same as dct
        dct2x2dc(dct2x2U);
        dct2x2dc(dct2x2V);

        /* Copy dc coeff into dct array */
        for(int i = 0; i < 4; i++) {
            dct4x4U[i][0][0] = dct2x2U[block_idx_y[i]][block_idx_x[i]];
            dct4x4V[i][0][0] = dct2x2V[block_idx_y[i]][block_idx_x[i]];
        }

        // Reconstruct the block
        add8x8_idct(dstPtrU, aStride, dct4x4U);
        add8x8_idct(dstPtrV, aStride, dct4x4V);


    }

    //V2dLibImageManip::dumpBuffer8x8(pDest, aStride, "Output");

    return true;
}


void CH264SliceDecoder::LoadNewBlock(int a_rowNo, int a_blkNo) {


    if (a_rowNo == 0) {
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[0] - 8] = -1;
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[1] - 8] = -1;
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[4] - 8] = -1;
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[5] - 8] = -1;

        m_non_zero_count[a_blkNo][x264_scan8[0] - 8] = 0x80;
        m_non_zero_count[a_blkNo][x264_scan8[1] - 8] = 0x80;
        m_non_zero_count[a_blkNo][x264_scan8[4] - 8] = 0x80;
        m_non_zero_count[a_blkNo][x264_scan8[5] - 8] = 0x80;
    }
    else {
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[0] - 8] = m_intra4x4_pred_mode[a_blkNo][x264_scan8[10]];
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[1] - 8] = m_intra4x4_pred_mode[a_blkNo][x264_scan8[11]];
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[4] - 8] = m_intra4x4_pred_mode[a_blkNo][x264_scan8[14]];
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[5] - 8] = m_intra4x4_pred_mode[a_blkNo][x264_scan8[15]];

        m_non_zero_count[a_blkNo][x264_scan8[0] - 8] = m_non_zero_count[a_blkNo][x264_scan8[10]];
        m_non_zero_count[a_blkNo][x264_scan8[1] - 8] = m_non_zero_count[a_blkNo][x264_scan8[11]];
        m_non_zero_count[a_blkNo][x264_scan8[4] - 8] = m_non_zero_count[a_blkNo][x264_scan8[14]];
        m_non_zero_count[a_blkNo][x264_scan8[5] - 8] = m_non_zero_count[a_blkNo][x264_scan8[15]];

    }
    if (a_blkNo > 0) {
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[0 ] - 1] = m_intra4x4_pred_mode[a_blkNo - 1][x264_scan8[5]];
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[2 ] - 1] = m_intra4x4_pred_mode[a_blkNo - 1][x264_scan8[7]];
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[8 ] - 1] = m_intra4x4_pred_mode[a_blkNo - 1][x264_scan8[13]];
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[10] - 1] = m_intra4x4_pred_mode[a_blkNo - 1][x264_scan8[15]];

        m_non_zero_count[a_blkNo][x264_scan8[0 ] - 1] = m_non_zero_count[a_blkNo - 1][x264_scan8[5]];
        m_non_zero_count[a_blkNo][x264_scan8[2 ] - 1] = m_non_zero_count[a_blkNo - 1][x264_scan8[7]];
        m_non_zero_count[a_blkNo][x264_scan8[8 ] - 1] = m_non_zero_count[a_blkNo - 1][x264_scan8[13]];
        m_non_zero_count[a_blkNo][x264_scan8[10] - 1] = m_non_zero_count[a_blkNo - 1][x264_scan8[15]];
    }
    else {
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[0 ] - 1] = -1;
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[2 ] - 1] = -1;
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[8 ] - 1] = -1;
        m_intra4x4_pred_mode[a_blkNo][x264_scan8[10] - 1] = -1;

        m_non_zero_count[a_blkNo][x264_scan8[0 ] - 1] = 0x80;
        m_non_zero_count[a_blkNo][x264_scan8[2 ] - 1] = 0x80;
        m_non_zero_count[a_blkNo][x264_scan8[8 ] - 1] = 0x80;
        m_non_zero_count[a_blkNo][x264_scan8[10] - 1] = 0x80;

    }

}

void CH264SliceDecoder::InitSVLCTable()
{
    int i;
    for( i = 0; i < 5; i++ )
    {
        /* max 2 step */
        m_coeff_token_lookup[i] = vlc_table_lookup_new( x264_coeff_token[i], 17*4, 4 );
    }
    /* max 2 step */
    m_level_prefix_lookup = vlc_table_lookup_new( x264_level_prefix, 16, 8 );

    for( i = 0; i < 15; i++ )
    {
        /* max 1 step */
        m_total_zeros_lookup[i] = vlc_table_lookup_new( x264_total_zeros[i], 16, 9 );
    }
    for( i = 0;i < 3; i++ )
    {
        /* max 1 step */
        m_total_zeros_dc_lookup[i] = vlc_table_lookup_new( x264_total_zeros_dc[i], 4, 3 );
    }
    for( i = 0;i < 7; i++ )
    {
        /* max 2 step */
        m_run_before_lookup[i] = vlc_table_lookup_new( x264_run_before[i], 15, 6 );
    }

}

int CH264SliceDecoder::ReadVLC(struct x264_vlc_table_t *table)
{
    int i_nb_bits;
    int i_value = 0;
    int i_bits;
    int i_index;
    int i_level = 0;

    //    i_index = bs_show(table->i_lookup_bits );
    i_index = m_pBitStream->showBitsValue((unsigned int) table->i_lookup_bits);
    if (i_index >= table->i_lookup) {
        return (-1);
    }
    i_value = table->lookup[i_index].i_value;
    i_bits = table->lookup[i_index].i_size;

    while (i_bits < 0) {
        i_level++;
        if (i_level > 5) {
            return (-1); // FIXME what to do ?
        }
        m_pBitStream->flushBits(table->i_lookup_bits);
        i_nb_bits = -i_bits;

        i_index = m_pBitStream->showBitsValue(i_nb_bits) + i_value;
        if (i_index >= table->i_lookup) {
            return (-1);
        }
        i_value = table->lookup[i_index].i_value;
        i_bits = table->lookup[i_index].i_size;
    }
    m_pBitStream->flushBits(i_bits);

    return (i_value);
}


int CH264SliceDecoder::DecodeBlockCavlc(int i_idx, int *l, int i_count,
        int nonZeroPredict, int &nonZero)
{
    int i;
    int level[16], run[16];
    int i_coeff;

    int i_total, i_trailing;
    int i_suffix_length;
    int i_zero_left;

    for( i = 0; i < i_count; i++ )
    {
        l[i] = 0;
    }

    //PRINTF("Coeff Token\n");
    /* total/trailing */
    if( i_idx == BLOCK_INDEX_CHROMA_DC)
    {
        int i_tt;

        if( ( i_tt = ReadVLC(m_coeff_token_lookup[4] )) < 0 )
        {
            return -1;
        }

        i_total = i_tt / 4;
        i_trailing = i_tt % 4;
    }
    else
    {
        /* x264_mb_predict_non_zero_code return 0 <-> (16+16+1)>>1 = 16 */
        static const int ct_index[17] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3 };
        int i_tt;


        if( ( i_tt = ReadVLC(m_coeff_token_lookup[ct_index[nonZeroPredict]] ) ) < 0 )
        {
            return -1;
        }

        i_total = i_tt / 4;
        i_trailing = i_tt % 4;
    }

    if( i_idx >= 0 )
    {
        //mb->block[i_idx].i_non_zero_count = i_total;
        nonZero = i_total;
    }

    if( i_total <= 0 )
    {
        return 0;
    }

    //PRINTF("Found total nz = %d Trailing %d\n", i_total, i_trailing);

    //PRINTF("Trailing Ones\n");

    i_suffix_length = i_total > 10 && i_trailing < 3 ? 1 : 0;

    for( i = 0; i < i_trailing; i++ )
    {
        level[i] = 1 - 2 * m_pBitStream->getBitsValue(1);
    }

    //PRINTF("Coefficients\n");

    for( ; i < i_total; i++ )
    {
        int i_prefix;
        int i_level_code;

        i_prefix = ReadVLC(m_level_prefix_lookup);

        if( i_prefix == -1 )
        {
            return -1;
        }
        else if( i_prefix < 14 )
        {
            if( i_suffix_length > 0 )
            {
                i_level_code = (i_prefix << i_suffix_length) + m_pBitStream->getBitsValue(i_suffix_length );
            }
            else
            {
                i_level_code = i_prefix;
            }
        }
        else if( i_prefix == 14 )
        {
            if( i_suffix_length > 0 )
            {
                i_level_code = (i_prefix << i_suffix_length) + m_pBitStream->getBitsValue(i_suffix_length );
            }
            else
            {
                i_level_code = i_prefix + m_pBitStream->getBitsValue(4 );
            }
        }
        else /* if( i_prefix == 15 ) */
        {
            i_level_code = (i_prefix << i_suffix_length) + m_pBitStream->getBitsValue(12);
            if( i_suffix_length == 0 )
            {
                i_level_code += 15;
            }
        }
        if( i == i_trailing && i_trailing < 3 )
        {
            i_level_code += 2;
        }
        /* Optimise */
        level[i] = i_level_code&0x01 ? -((i_level_code+1)/2) : (i_level_code+2)/2;

        //PRINTF("Level[%d] = %d\n", i, level[i]);
        if( i_suffix_length == 0 )
        {
            i_suffix_length++;
        }
        if( abs( level[i] ) > ( 3 << ( i_suffix_length - 1 ) ) && i_suffix_length < 6 )
        {
            i_suffix_length++;
        }
    }

    //PRINTF("Total Zeros\n");

    if( i_total < i_count )
    {
        if( i_idx == BLOCK_INDEX_CHROMA_DC )
        {
            i_zero_left = ReadVLC(m_total_zeros_dc_lookup[i_total-1] );
        }
        else
        {
            i_zero_left = ReadVLC(m_total_zeros_lookup[i_total-1] );
        }
        if( i_zero_left < 0 )
        {
            return -1;
        }
    }
    else
    {
        i_zero_left = 0;
    }

    //PRINTF("Zeros Found: %d\n", i_zero_left);

    //PRINTF("Runs Before\n");

    for( i = 0; i < i_total - 1; i++ )
    {
        if( i_zero_left <= 0 )
        {
            break;
        }
        run[i] = ReadVLC(m_run_before_lookup[SVC_MIN( i_zero_left - 1, 6 )] );

        if( run[i] < 0 )
        {
            return -1;
        }
        i_zero_left -= run[i];
    }
    if( i_zero_left < 0 )
    {
        return -1;
    }

    for( ; i < i_total - 1; i++ )
    {
        run[i] = 0;
    }
    run[i_total-1] = i_zero_left;

    i_coeff = -1;
    for( i = i_total - 1; i >= 0; i-- )
    {
        i_coeff += run[i] + 1;
        l[i_coeff] = level[i];
    }

    return 0;
}
