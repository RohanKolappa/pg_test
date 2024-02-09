/*
 * svcsliceencoder.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: rkale
 */


#include "svcsliceencoder.hh"
#include "ipputils.h"
#include "imagemanip.h"
#include "bitstreamwriter.hh"
#include "memory_tracker.h"
#include "EmulationPrevention.hh"
#include "layerUtils.hh"

#define USE_PROGRESSIVE_SCAN 1
#define BASE_LAYER_INDEX -1
#define QUAD_LAYER_INDEX 0
#define REM_LAYER_INDEX 1

unsigned char CSVCSliceEncoder::m_layer_dimension_map[9] =
{
   0, 1, 4, 9, 16, 25, 36, 49, 64,
};

CSVCSliceEncoder::CSVCSliceEncoder(int a_nMQuant, int a_nDCQuant,
        int a_nSliceWidth, int a_nSliceHeight) :
    CSliceEncoder(a_nMQuant, a_nDCQuant, 0, a_nSliceWidth, a_nSliceHeight)
{
    m_nSliceWidth = a_nSliceWidth;
    m_nSliceHeight = a_nSliceHeight;

    m_eEncodingType = ScalableEncoding;
    //m_eEncodingType = FullEncoding;
    //m_eEncodingType = LosslessEncoding;

    m_eEncodingSubType = ScalableStandard;
    m_nLayerDimension = 4;

    m_pLayer0BitStream = m_pBitStream;
    for (int i = 0; i < SVC_MAX_ENCODER_LAYERS; i++) {
        m_pLayerData[i] = NULL;
        m_pLayerBitStream[i] = NULL;
    }

    m_nNumberOfLayers = 1;
}

CSVCSliceEncoder::~CSVCSliceEncoder()
{
    for (int i = 0; i < SVC_MAX_ENCODER_LAYERS; i++) {
        delete [] m_pLayerData[i];
        delete m_pLayerBitStream[i];
    }

}


void CSVCSliceEncoder::SetEncodingType(eSVCEncodingType a_eEncodingType)
{
    m_eEncodingType = a_eEncodingType;
}

eSVCEncodingType CSVCSliceEncoder::GetEncodingType()
{
    return m_eEncodingType;
}

void CSVCSliceEncoder::SetEncodingSubType(eSVCEncodingSubType a_eEncodingSubType)
{
    m_eEncodingSubType = a_eEncodingSubType;
}

eSVCEncodingSubType CSVCSliceEncoder::GetEncodingSubType()
{
    return m_eEncodingSubType;
}

void CSVCSliceEncoder::SetLayerDimension(int aLayerDimension)
{
    if (aLayerDimension >= 2 && aLayerDimension <= 7)
        m_nLayerDimension = aLayerDimension;
}

int CSVCSliceEncoder::GetLayerDimension()
{
    return m_nLayerDimension;
}

int CSVCSliceEncoder::AddFrameHeader(uint8_t * a_outBuff,
        const SVCFrameHeader & frameHeader)
{
    // This function can be potentially called by an independent thread
    // so it uses its own bitstream
    CBitStreamWriter bitStream(a_outBuff);
    int StartFrameCode = 0x00000100;
    bitStream.PutBits(StartFrameCode, 32);

    bitStream.PutBits(1, 1);
    bitStream.PutBits(frameHeader.nFtimestamp, 31);

    bitStream.PutBits(frameHeader.bIFrame, 1);
    bitStream.PutBits(frameHeader.bFFrame, 1);
    bitStream.PutBits(frameHeader.bIsStereo, 1);


    if (frameHeader.bIsStereo) {
        bitStream.PutBits(frameHeader.bIsLeft, 1);
    }
    if (!frameHeader.bFrameDimensions) {
        bitStream.PutBits(0, 1); // frame_dimensions not coded
    }
    else {
        bitStream.PutBits(1, 1); // frame_dimensions coded

        bitStream.GoulombWriteUE(frameHeader.nFrameWidth);
        bitStream.GoulombWriteUE(frameHeader.nFrameHeight);
        bitStream.GoulombWriteUE(frameHeader.nEncodedOffsetX);
        bitStream.GoulombWriteUE(frameHeader.nEncodedOffsetY);
        bitStream.GoulombWriteUE(frameHeader.nEncodedWidth);
        bitStream.GoulombWriteUE(frameHeader.nEncodedHeight);
        bitStream.GoulombWriteUE(frameHeader.nSliceWidth);
        bitStream.GoulombWriteUE(frameHeader.nSliceHeight);
        bitStream.PutBits(frameHeader.bChromaSamplingMode, 1); // chroma_sampling_mode to 1
    }

    bitStream.PutBits(0, 1); // reserved_zero

   // bitStream.FlushToWordBoundary();
    bitStream.WriteLeftOverBits();
    /* TODO : Add Emulation check for Frame Headers */
    return bitStream.GetByteCount();

}

void CSVCSliceEncoder::SetRefreshSliceBit(uint8_t * a_outBuff)
{
    // TODO: Handle SVC case
}


void CSVCSliceEncoder::InitEncode()
{
    switch (m_eEncodingType) {
    case ScalableEncoding:
        switch (m_eEncodingSubType) {
        case ScalableStandard:
            m_nNumberOfLayers = 3;
            break;
        case ScalableQuad:
            m_nNumberOfLayers = 2;
            break;
        case ScalableDC:
            m_nNumberOfLayers = 1;
            break;
        }
        break;
    case LosslessEncoding:
    case FullEncoding:
        m_nNumberOfLayers = 1;
        break;
    }
}

void CSVCSliceEncoder::EncodeHeaders()
{
    AddSVCSliceHeader(m_nSliceNumber);
    m_pBitStreamStats->FillBucket(0);
}

int CSVCSliceEncoder::EncodeFooters()
{
    uint8_t * dst ;
    int total_size;
    m_pBitStreamStats->FillBucket(11);

    m_pBitStreamStats->FillBucket(12);

    // Copy out all the layers into the output buffer
    if (m_nNumberOfLayers > 1) {
        int last_size = m_pBitStream->GetByteCount();
        m_pBitStream->WriteLeftOverBits();
        //printf("Base size %d\n", last_size);
        dst = m_pBufferStart;
        total_size = last_size;
        for (int i = 1; i < m_nNumberOfLayers; i++) {
            int current_size = m_nLayerSize[i - 1];
            //printf("Layer %d size = %d\n", i, current_size);
            memcpy(dst + total_size, m_pLayerData[i - 1], current_size);
            total_size += current_size;
        }
      }
    else {
        m_pBitStream->WriteLeftOverBits();
        dst = m_pBufferStart;
        m_pBitStreamStats->FillBucket(13);
        total_size = m_pBitStream->GetByteCount();
    }

    int emubytes =  CEmulationPrevention::EmulateSlice(dst, total_size);
    if (emubytes != 0)
    {
        printf("Emulatiuon prevention returned with %d\n", emubytes);
    }

    return total_size + emubytes;
}

void CSVCSliceEncoder::EncodeData()
{
    if (m_eEncodingType == ScalableEncoding)
        EncodeScalableLayers();
    else if (m_eEncodingType == FullEncoding)
        EncodeFullLayer();
    else if (m_eEncodingType == LosslessEncoding)
        EncodeLosslessLayer();

}

#define SCANNER m_progressive_scan
#define BKT2 8
#define BKT3 2
#define BKT4 24
#define BKT5 4
#define BKT6 40
#define BKT7 6
#define BKT8 56

void CSVCSliceEncoder::CollectACStats(int n, int signed_level) {
    if (SCANNER[n] == BKT2) {
        m_pBitStreamStats->FillBucket(3);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(3);
    }
    else if (SCANNER[n] == BKT3) {
        m_pBitStreamStats->FillBucket(4);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(4);
    }
    else if (SCANNER[n] == BKT4) {
        m_pBitStreamStats->FillBucket(5);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(5);
    }
    else if (SCANNER[n] == BKT5) {
        m_pBitStreamStats->FillBucket(6);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(6);
    }
    else if (SCANNER[n] == BKT6) {
        m_pBitStreamStats->FillBucket(7);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(7);
    }
    else if (SCANNER[n] == BKT7) {
        m_pBitStreamStats->FillBucket(8);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(8);
    }
    else if (SCANNER[n] == BKT8) {
        m_pBitStreamStats->FillBucket(9);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(9);
    }
}

void CSVCSliceEncoder::PutIntraBlk(short* blk, int cc, short* dc_dct_pred,
        int a_firstBlkFlg, int nonZeroCoefficients)
{
    int n, dct_diff, run, signed_level;

    SwitchBitstreamToLayer(BASE_LAYER_INDEX);
    m_pBitStreamStats->FillBucket(1);

    if (a_firstBlkFlg)
        dc_dct_pred[cc] = 0;

    dct_diff = blk[0] - dc_dct_pred[cc];
    dc_dct_pred[cc] = blk[0];

    // Encode the DC coefficient
    PutDC(DClumtabHW, dct_diff);

    m_pBitStreamStats->FillBucket(2);


    SwitchBitstreamToLayer(QUAD_LAYER_INDEX);

    run = 0;
    int nonZeroFound = 0;
    int layer_dimension_switch_coeff = m_layer_dimension_map[m_nLayerDimension];

    for (n = 1; nonZeroFound < nonZeroCoefficients && n < 64; n++) {
        if ((n == layer_dimension_switch_coeff) && (m_eEncodingType == ScalableEncoding)) {
            //printf("Switched at %d %d\n", n, run);
            m_pBitStream->PutBits(6, 4);
            SwitchBitstreamToLayer(REM_LAYER_INDEX);
            run = 0;
        }

      #ifdef USE_PROGRESSIVE_SCAN
        signed_level = blk[m_progressive_scan[n]];
      #else
        signed_level = blk[zig_zag_scan[n]];
      #endif
        
        if (signed_level != 0) {
            //printf("putting %d %d %d\n", signed_level, n, run);
            PutAC(run, signed_level);
            run = 0;
            nonZeroFound++;
        }
        else {
            run++;
        }

#ifdef COLLECT_STATS
        CollectACStats(n, signed_level);
#endif
    }
    m_pBitStream->PutBits(6, 4);

    m_pBitStreamStats->FillBucket(10);
    m_pBitStreamStats->FillCounter(1);
}

void CSVCSliceEncoder::AddSVCSliceHeader(int a_nSliceNumber)
{
    // Start of slice code
    m_pBitStream->PutBits(V2D_VIDEO_SVC_START_SLICE_CODE, 32);

    // reserved bit
    m_pBitStream->PutBits(1, 1);

    // Leave space for Last Layer flag and layer0 size

    m_pBitStream->PutBits(0xFF, SVC_LAYERSIZE_BITS);
    m_pBitStream->PutBits(1, 1);
    

    // Slice Number
    m_pBitStream->PutBits(a_nSliceNumber, SVC_SLICENUMBER_BITS);

    // Refresh Slice (set to zero now, changed later)
    m_pBitStream->PutBits(0, 1);

    // Slice type is always ISlice currently
    m_pBitStream->PutBits(0, 1);

    // MQuant / DCQuant
    m_pBitStream->PutBits(m_nMQuant, 3);
    m_pBitStream->PutBits(m_nDCQuant, 3);

    // alt_quant alt_dequant curently set to Mquant values 
    m_pBitStream->PutBits(m_nMQuant, 3);
    m_pBitStream->PutBits(m_nDCQuant, 3);

    // Layer Type
    if (m_eEncodingType == ScalableEncoding)
        m_pBitStream->GoulombWriteUE(SVC_LAYERTYPE_DCLAYER);
    else if (m_eEncodingType == FullEncoding)
        m_pBitStream->GoulombWriteUE(SVC_LAYERTYPE_FULLLAYER);
    else if (m_eEncodingType == LosslessEncoding)
        m_pBitStream->GoulombWriteUE(SVC_LAYERTYPE_LOSSLESSLAYER);
    else
        m_pBitStream->GoulombWriteUE(SVC_LAYERTYPE_NULLLAYER);

}

void CSVCSliceEncoder::AddSVCLayerHeader(CBitStreamWriter * a_pBitStream, int layer_type)
{
    // reserved
    a_pBitStream->PutBits(1, 1);

    // Leave space for Last Layer flags and layer size
    a_pBitStream->PutBits(0xFF, SVC_LAYERSIZE_BITS);
    a_pBitStream->PutBits(1, 1);

    // Layer type
    a_pBitStream->GoulombWriteUE(layer_type);
}

void CSVCSliceEncoder::SetLayerSize(uint8_t * a_buff, int a_nSize, bool a_bLastLayer)
{
    *a_buff |= (a_nSize >> 8) & 0x7F;
    *(a_buff + 1) = a_nSize & 0xFF;

    if (a_bLastLayer) {
        *(a_buff + 2) |= 0x80;
    }
    else {
        *(a_buff + 2) &= ~0x80;
    }
}

int CSVCSliceEncoder::GetMidACLayerType() {
    switch (m_nLayerDimension) {
    case 4:
        return SVC_LAYERTYPE_MIDAC4x4LAYER;
    case 2:
        return SVC_LAYERTYPE_MIDAC2x2LAYER;
    case 3:
        return SVC_LAYERTYPE_MIDAC3x3LAYER;
    case 5:
        return SVC_LAYERTYPE_MIDAC5x5LAYER;
    case 6:
        return SVC_LAYERTYPE_MIDAC6x6LAYER;
    case 7:
        return SVC_LAYERTYPE_MIDAC7x7LAYER;
    }
    return SVC_LAYERTYPE_MIDAC4x4LAYER;
}

int CSVCSliceEncoder::GetRemACLayerType() {
    switch (m_nLayerDimension) {
    case 4:
        return SVC_LAYERTYPE_REMAC4x4LAYER;
    case 2:
        return SVC_LAYERTYPE_REMAC2x2LAYER;
    case 3:
        return SVC_LAYERTYPE_REMAC3x3LAYER;
    case 5:
        return SVC_LAYERTYPE_REMAC5x5LAYER;
    case 6:
        return SVC_LAYERTYPE_REMAC6x6LAYER;
    case 7:
        return SVC_LAYERTYPE_REMAC7x7LAYER;
    }
    return SVC_LAYERTYPE_REMAC4x4LAYER;
}

void CSVCSliceEncoder::EncodeScalableLayers() {
    int yStride = m_nWidth;
    int uvStride = m_nWidth >> 1;
    short mQuant = m_nMMult;
    short dc_dct_pred[3] = {0, 0, 0};

    // Create data for additional layers
    InitializeLayerData(QUAD_LAYER_INDEX);
    m_pBitStreamStats->SetBitStream(m_pLayerBitStream[QUAD_LAYER_INDEX]);
    AddSVCLayerHeader(m_pLayerBitStream[QUAD_LAYER_INDEX], GetMidACLayerType());
    m_pBitStreamStats->FillBucket(0);

    InitializeLayerData(REM_LAYER_INDEX);
    m_pBitStreamStats->SetBitStream(m_pLayerBitStream[REM_LAYER_INDEX]);
    AddSVCLayerHeader(m_pLayerBitStream[REM_LAYER_INDEX], GetRemACLayerType());
//    m_pBitStreamStats->FillBucket(0);


    int flag = 1;
    for (int rowNo = 0; rowNo < m_nSliceHeight; rowNo++) {
        uint8_t * yArrayPos = m_yArray + rowNo * 8 * yStride;
        uint8_t * uArrayPos = m_uArray  + rowNo * 8 * uvStride;
        uint8_t * vArrayPos = m_vArray + rowNo * 8 * uvStride;

        for (int blkNo = 0; blkNo < m_nSliceWidth / 2; blkNo++) {
            m_pBitStream->PutBits(0 ,1);
            EncodeOneBlock(m_block, yArrayPos, yStride, mQuant, 0, dc_dct_pred, flag);
            yArrayPos += 8;
            EncodeOneBlock(m_block, uArrayPos, uvStride, mQuant, 1, dc_dct_pred, flag);
            uArrayPos += 8;

            EncodeOneBlock(m_block, yArrayPos, yStride, mQuant, 0, dc_dct_pred, 0);
            yArrayPos += 8;
            EncodeOneBlock(m_block, vArrayPos, uvStride, mQuant, 2, dc_dct_pred, flag);
            vArrayPos += 8;
            SwitchBitstreamToLayer(BASE_LAYER_INDEX);
            flag = 0;
        }
    }

    // Reset bitstream back to base layer
    SwitchBitstreamToLayer(BASE_LAYER_INDEX);

    // Setup layer sizes
    m_pBitStream->PadToByteBoundary();
    //printf("Writing Layer0 size %llu\n", m_pBitStream->GetByteCount() - 4);
    SetLayerSize(m_pBitStream->GetStartPointer() + 4,
            m_pBitStream->GetByteCount() - 4,
            m_nNumberOfLayers > 1 ? false : true);

    for (int layer_index = 0; layer_index < m_nNumberOfLayers - 1 ; layer_index++) {
        m_pLayerBitStream[layer_index]->PadToByteBoundary();
        m_nLayerSize[layer_index] = m_pLayerBitStream[layer_index]->GetByteCount();
        //printf("Writing Layer1 size %d\n", m_nLayerSize[QUAD_LAYER_INDEX]);
        SetLayerSize(m_pLayerBitStream[layer_index]->GetStartPointer(),
                m_nLayerSize[layer_index],
                layer_index < m_nNumberOfLayers - 2 ? false : true);
        m_pLayerBitStream[layer_index]->WriteLeftOverBits();
    }

}

void CSVCSliceEncoder::EncodeFullLayer() {
    int yStride = m_nWidth;
    int uvStride = m_nWidth >> 1;
    short mQuant = m_nMMult;
    short dc_dct_pred[3] = {0, 0, 0};

    int flag = 1;
    for (int rowNo = 0; rowNo < m_nSliceHeight; rowNo++) {
        uint8_t * yArrayPos = m_yArray + rowNo * 8 * yStride;
        uint8_t * uArrayPos = m_uArray  + rowNo * 8 * uvStride;
        uint8_t * vArrayPos = m_vArray + rowNo * 8 * uvStride;
        for (int blkNo = 0; blkNo < m_nSliceWidth / 2; blkNo++) {
            EncodeOneBlock(m_block, yArrayPos, yStride, mQuant, 0, dc_dct_pred, flag);
            yArrayPos += 8;
            EncodeOneBlock(m_block, uArrayPos, uvStride, mQuant, 1, dc_dct_pred, flag);
            uArrayPos += 8;

            EncodeOneBlock(m_block, yArrayPos, yStride, mQuant, 0, dc_dct_pred, 0);
            yArrayPos += 8;
            EncodeOneBlock(m_block, vArrayPos, uvStride, mQuant, 2, dc_dct_pred, flag);
            vArrayPos += 8;

            flag = 0;
        }
    }


#if 0
    if (m_nSliceNumber == 637) {
        m_pBitStream->PadToByteBoundary();

        SetLayerSize(m_pBitStream->GetStartPointer() + 4,
                m_pBitStream->GetByteCount() - 4, false);

        int last_offset = m_pBitStream->GetByteCount();

        AddSVCLayerHeader(SVC_LAYERTYPE_DCLAYER);
        for (int i = 0; i < 24; i++)
            m_pBitStream->PutBits(0x1, 5);
        m_pBitStream->PadToByteBoundary();

        SetLayerSize(m_pBitStream->GetStartPointer() + last_offset,
                m_pBitStream->GetByteCount() - last_offset, false);

        last_offset = m_pBitStream->GetByteCount();

        AddSVCLayerHeader(SVC_LAYERTYPE_MIDAC4x4LAYER);
        for (int i = 0; i < 100; i++)
            m_pBitStream->PutBits(0x1, 13);
        m_pBitStream->PadToByteBoundary();

        SetLayerSize(m_pBitStream->GetStartPointer() + last_offset,
                m_pBitStream->GetByteCount() - last_offset, false);

        last_offset = m_pBitStream->GetByteCount();

        AddSVCLayerHeader(SVC_LAYERTYPE_REMAC4x4LAYER);
        for (int i = 0; i < 500; i++)
            m_pBitStream->PutBits(0x1, 7);
        m_pBitStream->PadToByteBoundary();

        SetLayerSize(m_pBitStream->GetStartPointer() + last_offset,
                m_pBitStream->GetByteCount() - last_offset, true);

        return;
    }
#endif

    SetLayerSize(m_pBitStream->GetStartPointer() + 4,
            m_pBitStream->GetByteCount() - 4, true);
}

void CSVCSliceEncoder::EncodeLosslessLayer() {
    int yStride = m_nWidth;
    int uvStride = m_nWidth >> 1;
    short dc_dct_pred[3] = {0, 0, 0};

    int flag = 1;
    for (int rowNo = 0; rowNo < m_nSliceHeight; rowNo++) {
        uint8_t * yArrayPos = m_yArray + rowNo * 8 * yStride;
        uint8_t * uArrayPos = m_uArray  + rowNo * 8 * uvStride;
        uint8_t * vArrayPos = m_vArray + rowNo * 8 * uvStride;
        for (int blkNo = 0; blkNo < m_nSliceWidth / 2; blkNo++) {
            EncodeLosslessBlock(yArrayPos, yStride, 0, dc_dct_pred, flag);
            yArrayPos += 8;
            EncodeLosslessBlock(uArrayPos, uvStride, 1, dc_dct_pred, flag);
            uArrayPos += 8;

            EncodeLosslessBlock(yArrayPos, yStride, 0, dc_dct_pred, 0);
            yArrayPos += 8;
            EncodeLosslessBlock(vArrayPos, uvStride, 2, dc_dct_pred, flag);
            vArrayPos += 8;

            flag = 0;
        }
    }

    SetLayerSize(m_pBitStream->GetStartPointer() + 4,
            m_pBitStream->GetByteCount() - 4, true);

}

//#define USE_DC_VLC
void CSVCSliceEncoder::EncodeLosslessBlock(uint8_t * a_arrayPos, short a_stride, int a_cc,
        short* a_dc_dct_pred, int a_firstBlkFlg)
{
    //uint8_t block[64];
    GetBlock(m_block, a_arrayPos, a_stride);
    AddBlk(m_block, -128);
    int val, lastval, diff;

    if (a_cc == 0)
        m_nBlocksEncoded++;

    val = lastval = m_block[0];
#ifdef USE_DC_VLC
    PutDC(DClumtabHW, val);
#else
    m_pBitStream->GoulombWriteSE(val);
#endif

    for (int n = 1; n < 64; n++) {
       val = m_block[zig_zag_scan[n]];
       //val = m_block[n];
       diff = val - lastval;
#ifdef USE_DC_VLC
       PutDC(DClumtabHW, diff);
#else
       m_pBitStream->GoulombWriteSE(diff);
#endif
       lastval = val;
       //printf("val[%d:%d] = %d\n", n, zig_zag_scan[n], val);
    }
    //exit(0);
}

void CSVCSliceEncoder::InitializeLayerData(int index) {
    if (m_pLayerData[index] == NULL) {
        m_pLayerData[index] = new unsigned char [m_nWidth * m_nHeight * 4];
        //memset(m_pLayerData[index], 0x00, m_nWidth * m_nHeight * 4);
    }
    if (m_pLayerBitStream[index] == NULL) {
        m_pLayerBitStream[index] = new CBitStreamWriter();
    }
    m_pLayerBitStream[index]->Init(m_pLayerData[index]);
    m_nLayerSize[index] = 0;
}


void CSVCSliceEncoder::SwitchBitstreamToLayer(int index) {

    if (m_eEncodingType != ScalableEncoding)
        return;

    if (index == BASE_LAYER_INDEX) {
        m_pBitStream = m_pLayer0BitStream;
    }
    else {
        m_pBitStream = m_pLayerBitStream[index];
    }
#ifdef COLLECT_STATS
    m_pBitStreamStats->SetBitStream(m_pBitStream);
#endif
}
