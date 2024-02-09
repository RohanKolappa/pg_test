/*
 * h264sliceencoder.cpp
 *
 *  Created on: Oct 29, 2010
 *      Author: rkale
 */


#include "h264sliceencoder.hh"
#include "ipputils.h"
#include "imagemanip.h"
#include "bitstreamwriter.hh"

#include "svc.h"
#include "cavlc.hh"
#include "memory_tracker.h"


// TODO: move to svc.c
static inline int array_non_zero_count(int *v, int i_count)
{
    int i;
    int i_nz;

    for( i = 0, i_nz = 0; i < i_count; i++ )
        if( v[i] )
            i_nz++;

    return i_nz;
}


CH264SliceEncoder::CH264SliceEncoder(int a_nMQuant, int a_nDCQuant) :
CSliceEncoder(a_nMQuant, a_nDCQuant, 0, H264_SLICE_WIDTH * 2, H264_SLICE_HEIGHT * 2)
{
    m_yArrayPred = (uint8_t *) malloc(m_nWidth * m_nHeight);
    m_uArrayPred = (uint8_t *) malloc(m_nWidth * m_nHeight);
    m_vArrayPred = (uint8_t *) malloc(m_nWidth * m_nHeight);
    m_qp = 24;
}

CH264SliceEncoder::~CH264SliceEncoder()
{

    if (m_yArrayPred != NULL)
        free(m_yArrayPred);
    if (m_uArrayPred != NULL)
        free(m_uArrayPred);
    if (m_vArrayPred != NULL)
        free(m_vArrayPred);

}

void CH264SliceEncoder::SetQuantizationParameter(int a_qp) {
    if ((a_qp >= 0) && (a_qp <= 52)) {
        m_qp = a_qp;
    }
}

#ifdef COLLECT_STATS
static const char * bucketStr[] = {
    "Slice Headers",
    "Zilch",
    "Luma Pred",
    "Chroma Pred",
    "Coded Blk. Pat.",
    "Coeff Block",
    "Trail. Ones",
    "Coefficients",
    "Total Zeros",
    "Run Before",
    "Zilch",
    "Zilch",
    "End Of Slice",
    "Padding",
    "Undefined",
    "Undefined",
    "Undefined",
    "Undefined",
};
#endif

void CH264SliceEncoder::PrintStats() {
#ifdef COLLECT_STATS
    uint64_t total = m_pBitStreamStats->GetTotal();
    for (int i = 0; i < NUMB_BUCKETS; i++) {
        printf("%-2d %-20s Bytes: %-10"PRIu64" %-5.2f %% %-5"PRIu64"\n",
                i, bucketStr[i],
                    m_pBitStreamStats->GetBucket(i) >> 3,
                    100.0 * m_pBitStreamStats->GetBucket(i)/total,
                    m_pBitStreamStats->GetCounter(i));
    }
    printf("Total = %"PRIu64" %"PRIu64"\n", total/8, m_pBitStreamStats->GetBucket(2));
        printf("Distribution Luma %"PRIu64" Chroma %"PRIu64"\n",
                m_pBitStreamStats->GetBucketToo(1) >> 3,
                        m_pBitStreamStats->GetBucketToo(2) >> 3);
    printf("Blocks Encoded = %u\n", m_nBlocksEncoded);
#endif
}

void CH264SliceEncoder::PutQuantBits(void)
{
    m_pBitStream->PutBits(m_qp, 6);
}


void CH264SliceEncoder::EncodeData() {
    int yStride = m_nWidth;
    int uvStride = m_nWidth >> 1;
    short mQuant = m_nMMult;

    // For each row of macroblocks in a slice
    for (int rowNo = 0; rowNo < H264_SLICE_HEIGHT; rowNo++) {
        uint8_t * yArrayPos = m_yArray + rowNo * 16 * yStride;
        uint8_t * uArrayPos = m_uArray  + rowNo * 16 * uvStride;
        uint8_t * vArrayPos = m_vArray + rowNo * 16 * uvStride;

        uint8_t * yArrayPredPos = m_yArrayPred + rowNo * 16 * yStride;
        uint8_t * uArrayPredPos = m_uArrayPred + rowNo * 16 * uvStride;
        uint8_t * vArrayPredPos = m_vArrayPred + rowNo * 16 * uvStride;

        // For each macroblock in a row
        int flag = 1;
        int firstRow = (rowNo == 0);
        for (int blkNo = 0; blkNo < H264_SLICE_WIDTH; blkNo++) {
            LoadNewBlock(rowNo, blkNo);

            // Encode Luma
            EncodeH264LumaBlocks(yArrayPos, yArrayPredPos, yStride, mQuant, blkNo, flag, firstRow);
            yArrayPos += 16;
            yArrayPredPos += 16;

            // Encode Chroma
            EncodeH264ChromaBlocks(uArrayPos, uArrayPredPos,
                    vArrayPos, vArrayPredPos, uvStride, mQuant, blkNo, flag, firstRow);
            uArrayPos += 8;
            uArrayPredPos += 8;
            vArrayPos += 8;
            vArrayPredPos += 8;

            // Write out the block
            WriteH264Blocks(blkNo);

            flag = 0;

        }
    }
}



#define COST_MAX  (1 << 24)

void CH264SliceEncoder::EncodeH264LumaBlocks(uint8_t * a_arrayPos,
        uint8_t * a_arrayPredPos,
        short a_stride, unsigned int a_iMQuant,
        int a_blkNo, int a_firstBlkFlg, int a_firstRowFlag)
{
    int idx;
    int16_t dct4x4[4][4];
    int quant4_mf[6][4][4];
    int dequant4_mf[6][4][4];

    int i_qscale = m_qp;

    create_quant_tables_4x4(quant4_mf, dequant4_mf);
    //V2dLibImageManip::dumpBuffer8x8(a_arrayPos, a_stride, "Input");

    // Determine available neighbors
    int neighbour = 0;
    int neighbour4[16];
    if (!a_firstBlkFlg)
        neighbour |= MB_LEFT;
    if (!a_firstRowFlag)
        neighbour |= MB_TOP;

    create_neighbour_list4(neighbour, neighbour4);

    m_nBlocksEncoded++;

    for( idx = 0; idx < 16; idx++ ) {
        int x, y;

        uint8_t *srcPtr;
        uint8_t *dstPtr;
        x = block_idx_x[idx];
        y = block_idx_y[idx];


        srcPtr = a_arrayPos + 4 * x + 4 * y * a_stride;
        dstPtr = a_arrayPredPos + 4 * x + 4 * y * a_stride;

        // Calculate best mode to use to encode block
        int predict_mode[9];
        int numbmodes;
        int best_cost = COST_MAX;
        int best_mode = 0;

        predict_4x4_mode_available(neighbour4[idx], predict_mode, &numbmodes);
        for(int i = 0; i < numbmodes; i++) {
            int cost;
            int mode;

            mode = predict_mode[i];
            predict_4x4[mode](dstPtr, a_stride);

            cost = pixel_sad_4x4(dstPtr, a_stride, srcPtr, a_stride);

            // printf("%d, ", cost);

            if (best_cost > cost) {
                best_mode = mode;
                best_cost = cost;
            }
        }
//        int predicted_mode = mb_predict_intra4x4_mode(idx, m_intra4x4_pred_mode[a_blkNo]);
//        printf("Index = %d Best cost = %d, best mode = %d [%s] => %d, predicted mode %d [%s]\n",
//                idx, best_cost,
//                best_mode, mode4x4_string[best_mode], x264_mb_pred_mode4x4_fix(best_mode),
//                predicted_mode, mode4x4_string[predicted_mode]);

        // Predict block using the best mode
        //V2dLibImageManip::dumpBuffer4x4(srcPtr, a_stride, "Input");
        //best_mode = I_PRED_4x4_DC_128;
//        if (best_mode != I_PRED_4x4_DC_128)
//            best_mode = I_PRED_4x4_H;
        predict_4x4[best_mode](dstPtr, a_stride);

        m_intra4x4_pred_mode[a_blkNo][x264_scan8[idx]] = best_mode;

        //V2dLibImageManip::dumpBuffer4x4(dstPtr, a_stride, "after predict");

        // Perform dct on residual
        sub4x4_dct(dct4x4, srcPtr, a_stride, dstPtr, a_stride);

        // Quantize
        quant_4x4(dct4x4, quant4_mf, i_qscale, 1);

        // Store away the dct zig-zagged
        scan_zigzag_4x4full(m_luma4x4[idx], dct4x4);
        m_non_zero[idx] = array_non_zero_count(m_luma4x4[idx], 16);
        m_non_zero_count[a_blkNo][x264_scan8[idx]] = m_non_zero[idx];
        //V2dLibImageManip::dumpBlock16(luma4x4, "Luma");

        // Reconstruct current block from transform domain for
        // prediction of further blocks

        // De-quantize
        dequant_4x4(dct4x4, dequant4_mf, i_qscale);

        // Inverse dct and add back to predicted values
        add4x4_idct(dstPtr, a_stride, dct4x4);

        //V2dLibImageManip::dumpBuffer4x4(dstPtr, a_stride, "after construct");

    }

    // Compute luma coded block pattern
    m_cbp_luma = 0;
    for( idx = 0; idx < 16; idx++ ) {
        if (m_non_zero[idx] > 0)
            m_cbp_luma |= 1 << (idx/4);
    }

}

void CH264SliceEncoder::EncodeH264ChromaBlocks(
        uint8_t * a_uArrayPos, uint8_t * a_uArrayPredPos,
        uint8_t * a_vArrayPos, uint8_t * a_vArrayPredPos,
        short a_stride, unsigned int a_iMQuant,
        int a_blkNo, int a_firstBlkFlg, int a_firstRowFlag)
{
    int idx;
    int16_t dct2x2U[2][2];
    int16_t dct2x2V[2][2];
    int16_t dct4x4U[4][4][4];
    int16_t dct4x4V[4][4][4];

    int quant4_mf[6][4][4];
    int dequant4_mf[6][4][4];
    int i_qscale = m_qp;

    create_quant_tables_4x4(quant4_mf, dequant4_mf);

    m_pBitStreamStats->FillBucket(1);

    m_cbp_chroma = 0;
    for( idx = 0; idx < 2; idx++ ) {
        int i;
        int predict_mode[7];
        int i_max;
        uint8_t *srcPtrU = a_uArrayPos + idx * 8 * a_stride;
        uint8_t *dstPtrU = a_uArrayPredPos + idx * 8 * a_stride;

        uint8_t *srcPtrV = a_vArrayPos + idx * 8 * a_stride;
        uint8_t *dstPtrV = a_vArrayPredPos + idx * 8 * a_stride;

        // Find the best prediction mode
        int neighbour = 0;
        if (a_firstRowFlag) {
            if (a_firstBlkFlg) {
                if (idx == 1)
                    neighbour |= MB_TOP;
            }
            else {
                if (idx == 1)
                    neighbour = MB_TOP | MB_LEFT | MB_TOPLEFT;
                else
                    neighbour = MB_LEFT;
            }
        }
        else {
            if (a_firstBlkFlg) {
                neighbour |= MB_TOP;
            }
            else {
                neighbour = MB_TOP | MB_LEFT | MB_TOPLEFT;
            }
        }
        predict_8x8chroma_mode_available(neighbour, predict_mode, &i_max );
        int best_sad = COST_MAX;

        int best_mode = I_PRED_CHROMA_DC_128;
        for (i = 0; i < i_max; i++) {
            int i_sad;
            int i_mode;

            i_mode = predict_mode[i];

            /* Predict  */
            predict_8x8c[i_mode](dstPtrU, a_stride);

            /* Calculate the Cost */
            i_sad = pixel_sad_8x8(dstPtrU, a_stride, srcPtrU, a_stride);

            /* Lower cost is better */
            if (best_sad > i_sad) {
                best_mode = i_mode;
                best_sad = i_sad;
            }
        }


        m_chroma_predict_mode[idx] = best_mode;
        // Predict both U and V using same best mode
        //printf("Write Predicting using mode %d\n", best_mode);
        predict_8x8c[best_mode](dstPtrU, a_stride);
        predict_8x8c[best_mode](dstPtrV, a_stride);


        // Perform dct on residuals of each 4x4 block
        sub8x8_dct(dct4x4U, srcPtrU, a_stride, dstPtrU, a_stride);
        sub8x8_dct(dct4x4V, srcPtrV, a_stride, dstPtrV, a_stride);

        /* Calculate dct coeffs */
        for( i = 0; i < 4; i++ )
        {
            /* Save off DC coeff */
            dct2x2U[block_idx_y[i]][block_idx_x[i]] = dct4x4U[i][0][0];
            dct2x2V[block_idx_y[i]][block_idx_x[i]] = dct4x4V[i][0][0];

            // Quantize
            quant_4x4(dct4x4U[i], quant4_mf, i_qscale, 1);
            quant_4x4(dct4x4V[i], quant4_mf, i_qscale, 1);

            // Save off zig-zagged
            scan_zigzag_4x4(m_chroma_acU[idx][i], dct4x4U[i]);
            scan_zigzag_4x4(m_chroma_acV[idx][i], dct4x4V[i]);

            // Compute non zero's
            int nzc = 0;
            nzc += array_non_zero_count(m_chroma_acU[idx][i], 15);
            nzc += array_non_zero_count(m_chroma_acV[idx][i], 15);
            if (nzc > 0)
                m_cbp_chroma |= 0x2;

            //int m_non_zero = array_non_zero_count(residual_ac[i], 15);
            //V2dLibImageManip::dumpBlock16(luma4x4, "Luma");
            //if (m_non_zero == 0)
              // m_nCounter[3]++;

            // De-quant
            dequant_4x4(dct4x4U[i], dequant4_mf, i_qscale);
            dequant_4x4(dct4x4V[i], dequant4_mf, i_qscale);
        }

        // Deal with the 2x2 DC matrix
        dct2x2dc(dct2x2U);
        dct2x2dc(dct2x2V);

        quant_2x2_dc(dct2x2U, quant4_mf, i_qscale, 1);
        quant_2x2_dc(dct2x2V, quant4_mf, i_qscale, 1);

        scan_zigzag_2x2_dc(m_chroma_dcU[idx], dct2x2U);
        scan_zigzag_2x2_dc(m_chroma_dcV[idx], dct2x2V);

        int nzc = 0;
        nzc += array_non_zero_count(m_chroma_dcU[idx], 4);
        nzc += array_non_zero_count(m_chroma_dcV[idx], 4);
        if (nzc > 0)
            m_cbp_chroma |= 0x1;
        dct2x2dc(dct2x2U); // idct function same as dct
        dct2x2dc(dct2x2V); // idct function same as dct
        dequant_2x2_dc(dct2x2U, dequant4_mf, i_qscale);
        dequant_2x2_dc(dct2x2V, dequant4_mf, i_qscale);

        /* Copy dc coeff */
        for( i = 0; i < 4; i++ ) {
            dct4x4U[i][0][0] = dct2x2U[block_idx_y[i]][block_idx_x[i]];
            dct4x4V[i][0][0] = dct2x2V[block_idx_y[i]][block_idx_x[i]];
        }
        /* Output samples to dest for future macroblocks */
        add8x8_idct(dstPtrU, a_stride, dct4x4U);
        add8x8_idct(dstPtrV, a_stride, dct4x4V);

    }

    if (m_cbp_chroma == 0x3) {
        m_cbp_chroma = 0x2;
    }

    //V2dLibImageManip::dumpBuffer8x8(a_arrayPredPos, a_stride, "Predict");
    //V2dLibImageManip::dumpResidual8x8(a_arrayPos, a_arrayPredPos, a_stride, "Residual Error");
    //exit(0);
}

void CH264SliceEncoder::WriteH264Blocks(int a_blkNo)
{
    m_pBitStreamStats->FillBucket(1);

    int intra4x4_pred_mode[SCAN8_SIZE];

    for (int i = 0; i < SCAN8_SIZE; i++) {
        intra4x4_pred_mode[i] = m_intra4x4_pred_mode[a_blkNo][i];
    }
    // Write Luma prediction modes
    for(int idx = 0; idx < 16; idx++ ) {

        int predicted_mode = mb_predict_intra4x4_mode(idx, intra4x4_pred_mode);
        int current_mode = x264_mb_pred_mode4x4_fix(m_intra4x4_pred_mode[a_blkNo][x264_scan8[idx]]);

        if (current_mode == predicted_mode) {
            //printf("Write luma predict mode = %d [%d]\n", best_mode, 0);
            m_pBitStream->PutBits(1, 1);
            m_pBitStreamStats->FillCounter(0);
        }
        else {
            //printf("Write luma predict mode = %d [%d]\n", best_mode, 1);
            m_pBitStream->PutBits(0, 1);
            m_pBitStreamStats->FillCounter(1);
            if (current_mode < predicted_mode) {
                m_pBitStream->PutBits(current_mode, 3);
            }
            else {
                m_pBitStream->PutBits(current_mode - 1, 3);
            }
        }
    }

    m_pBitStreamStats->FillBucket(2);

    // Write Chroma prediction modes
    for (int idx = 0; idx < 2; idx++) {
        m_pBitStream->GoulombWriteUE(x264_mb_pred_mode8x8c_fix[m_chroma_predict_mode[idx]]);
    }

    m_pBitStreamStats->FillBucket(3);

    // Write coded block pattern
    int cbp = (m_cbp_chroma  << 4) | m_cbp_luma;
    m_pBitStream->GoulombWriteUE(intra4x4_cbp_to_golomb[cbp] );

    m_pBitStreamStats->FillBucket(4);

    m_pBitStreamStats->FillBucketToo(0);

    // Write Luma
    for(int idx = 0; idx < 16; idx++ ) {
        int non_zero_predict = mb_predict_non_zero_code(idx, m_non_zero_count[a_blkNo]);
        if (m_cbp_luma & (1 << (idx/4)))
            CCAVLC::Encode(m_pBitStream, 0, m_luma4x4[idx], 16, non_zero_predict);
    }

    m_pBitStreamStats->FillBucketToo(1);

    // Write Chroma
    for (int idx = 0; idx < 2; idx++) {
        if (m_cbp_chroma != 0x0) {
            CCAVLC::Encode(m_pBitStream, BLOCK_INDEX_CHROMA_DC, m_chroma_dcU[idx], 4, 0);
            CCAVLC::Encode(m_pBitStream, BLOCK_INDEX_CHROMA_DC, m_chroma_dcV[idx], 4, 0);
            if (m_cbp_chroma & 0x2) {
                for (int i = 0; i < 4; i++) {
                    CCAVLC::Encode(m_pBitStream, 0, m_chroma_acU[idx][i], 15, 0);
                    CCAVLC::Encode(m_pBitStream, 0, m_chroma_acV[idx][i], 15, 0);
                }
            }
        }
    }

    m_pBitStreamStats->FillBucketToo(2);
    m_pBitStreamStats->FillBucket(10);

}

void CH264SliceEncoder::LoadNewBlock(int a_rowNo, int a_blkNo) {
    // Fix prediction mode for next block


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

