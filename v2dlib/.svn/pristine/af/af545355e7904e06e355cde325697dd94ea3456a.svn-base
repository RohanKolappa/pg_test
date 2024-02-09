/*
 * svcslicedecoder.cpp
 *
 *  Created on: Nov 1, 2010
 *      Author: rkale
 */


#include "svcslicedecoder.hh"
#include "codecdefs.h"
#include "imagemanip.h"
#include "dct.hh"
#include "ipputils.h"
#include "svc.h"
#include "imagemanip.h"
#include "memory_tracker.h"
#include "EmulationPrevention.hh"

using namespace V2DLib;

#define  USE_PROGRESSIVE_SCAN 1
//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define PRINTF printf
#else
#define PRINTF(...) do {} while (0)
#endif

unsigned char CSVCSliceDecoder::m_layer_dimension_map[9] =
{
   0, 1, 4, 9, 16, 25, 36, 49, 64,
};

CSVCSliceDecoder::CSVCSliceDecoder() :
    CSliceDecoder(eSVC)
{
    m_nSliceWidth = SVC_SLICE_WIDTH;
    m_nSliceHeight = SVC_SLICE_HEIGHT;
    m_nLayerDimension = 4;
    m_nLayerSizeError = 0;

    // Setup the scan matrix (overwrite base class setup)
    for(unsigned int i = 0; i < 8; i++) {
        for(unsigned int j = 0; j < 8; j++) {
        #ifdef USE_PROGRESSIVE_SCAN
          m_scan_matrix[8 * i + j] = progressive_scan[8 * i + j];
        #else
          m_scan_matrix[8 * i + j] = scan[8 * i + j];
        #endif
        }
    }

    for (int i = 0; i < SVC_MAX_DECODER_LAYERS; i++) {
        m_pLayerBitStream[i] = NULL;
    }

}

CSVCSliceDecoder::~CSVCSliceDecoder()
{
    for (int i = 0; i < SVC_MAX_DECODER_LAYERS; i++) {
        delete m_pLayerBitStream[i];
    }

}

void CSVCSliceDecoder::SetSliceDimensions(int aSliceWidth, int aSliceHeight)
{
    m_nSliceWidth = aSliceWidth;
    m_nSliceHeight = aSliceHeight;
}

extern "C" {

int ParseSVCFrameHeader(uint8_t * a_pData, int a_nDataLen,
        SVCFrameHeader * pFrameHeader)
{
    if (pFrameHeader == NULL)
        return 0;
    bool retVal = CSVCSliceDecoder::DecodeSVCFrameHeader(a_pData, a_nDataLen,
            *pFrameHeader);
    return retVal ? 1 : 0;
}

} // extern "C"

bool CSVCSliceDecoder::DecodeSVCFrameHeader(uint8_t * a_pData, size_t a_nDataLen,
        SVCFrameHeader & frameHeader)
{
    /* TODO : Add Demulation check for Frame Headers */
    CBitStream bitStream(a_pData, a_nDataLen);
    return DecodeSVCFrameHeader(&bitStream, frameHeader);
}


bool CSVCSliceDecoder::DecodeSVCFrameHeader(CBitStream* pBitStream, SVCFrameHeader & frameHeader)
{

    unsigned int nCode;
    if (!pBitStream->get32Bits(nCode))
        return false;

    if (nCode != V2D_VIDEO_SVC_START_FRAME_CODE) {
        return false;
    }

    nCode = 0;
    pBitStream->getBits(1, nCode); 

    if (!nCode) {
      // reserved bit supposed to be 1 always
      PRINTF("\n reserved bit in Frame Header is not set to 1");
      return false;
    }
    PRINTF("reserved_one  %d\n", nCode);

    pBitStream->getBits(31, frameHeader.nFtimestamp); 
    PRINTF("Ftimestamp %d\n", frameHeader.nFtimestamp);

    pBitStream->getBits(1, frameHeader.bIFrame);
    PRINTF("IFrame %d\n", frameHeader.bIFrame);

    pBitStream->getBits(1, frameHeader.bFFrame);
    PRINTF("FFrame %d\n", frameHeader.bFFrame);

    pBitStream->getBits(1, frameHeader.bIsStereo);
    PRINTF("IsStereo %d\n", frameHeader.bIsStereo);

    if (frameHeader.bIsStereo) {
        pBitStream->getBits(1, frameHeader.bIsLeft);
        PRINTF("IsLeft %d\n", frameHeader.bIsLeft);
    }

    pBitStream->getBits(1, frameHeader.bFrameDimensions);
    if (frameHeader.bFrameDimensions) {
        frameHeader.nFrameWidth = GolombReadUE(pBitStream);
        PRINTF("FrameWidth %d\n", frameHeader.nFrameWidth);
        if (!CheckRange(frameHeader.nFrameWidth, 1, SVC_MAX_FRAME_WIDTH))
            return false;

        frameHeader.nFrameHeight = GolombReadUE(pBitStream);
        PRINTF("FrameHeight %d\n", frameHeader.nFrameHeight);
        if (!CheckRange(frameHeader.nFrameHeight, 1, SVC_MAX_FRAME_HEIGHT))
            return false;

        frameHeader.nEncodedOffsetX = GolombReadUE(pBitStream);
        PRINTF("EncodedOffsetX %d\n", frameHeader.nEncodedOffsetX);
        if (!CheckRange(frameHeader.nEncodedOffsetX, 0, SVC_MAX_FRAME_WIDTH))
            return false;

        frameHeader.nEncodedOffsetY = GolombReadUE(pBitStream);
        PRINTF("EncodedOffsetY %d\n", frameHeader.nEncodedOffsetY);
        if (!CheckRange(frameHeader.nEncodedOffsetY, 0, SVC_MAX_FRAME_HEIGHT))
            return false;

        frameHeader.nEncodedWidth = GolombReadUE(pBitStream);
        PRINTF("EncodedWidth %d\n", frameHeader.nEncodedWidth);
        if (!CheckRange(frameHeader.nEncodedWidth, 16, SVC_MAX_FRAME_WIDTH))
            return false;

        frameHeader.nEncodedHeight = GolombReadUE(pBitStream);
        PRINTF("EncodedHeight %d\n", frameHeader.nEncodedHeight);
        if (!CheckRange(frameHeader.nEncodedHeight, 8, SVC_MAX_FRAME_HEIGHT))
            return false;

        frameHeader.nSliceWidth = GolombReadUE(pBitStream);
        frameHeader.nSliceHeight = GolombReadUE(pBitStream);
        PRINTF("Slice Size %d x %d\n", frameHeader.nSliceWidth, frameHeader.nSliceHeight);
        if (frameHeader.nSliceWidth * frameHeader.nSliceHeight > SVC_MAX_SLICE_SIZE)
            return false;

        pBitStream->getBits(1, frameHeader.bChromaSamplingMode);
        PRINTF("ChromaSamplingMode %d\n", frameHeader.bChromaSamplingMode);

        // Verify video dimensions are within sane ranges
        if (frameHeader.nFrameWidth + frameHeader.nEncodedOffsetX > frameHeader.nEncodedWidth)
            return false;
        if (frameHeader.nFrameHeight + frameHeader.nEncodedOffsetY > frameHeader.nEncodedHeight)
            return false;

    }
    else {
        PRINTF("Frame Dimensions not available\n");
    }

    // Ensure reserve zero bit is clear
    unsigned int reservedZero;
    pBitStream->getBits(1, reservedZero);
    if (reservedZero != 0)
        return false;

    return true;
}

bool CSVCSliceDecoder::DecodeSliceHeader(CBitStream* pBitStream, SVCSliceHeader & sliceHeader)
{
    // Confirm SOS
    unsigned int nCode;
    if (!pBitStream->get32Bits(nCode))
        return false;
    if (nCode != V2D_VIDEO_SVC_START_SLICE_CODE) {
        return false;
    }
    
    nCode = 0;
    pBitStream->getBits(1, nCode);

    if(!nCode) {
      // reserved bit always supposed to 1
      PRINTF("\n reserved bit Slice Header is not set to 1");
      return false;
    }
       
    // Layer0 Size
    pBitStream->getBits(SVC_LAYERSIZE_BITS, sliceHeader.layerHeaders[0].nLayerSize);

    // Last Layer Flag
    pBitStream->getBits(1, sliceHeader.layerHeaders[0].bLastLayer);

    // Slice Number
    pBitStream->getBits(SVC_SLICENUMBER_BITS, sliceHeader.nSliceNumber);

    // Refresh Slice
    pBitStream->getBits(1, sliceHeader.bRefreshSlice);

    // Slice Type
    pBitStream->getBits(1, sliceHeader.bSliceType);

    // MQuant/DCQuant
    unsigned int tmp1;
    pBitStream->getBits(3, tmp1);
    sliceHeader.nMQuant = 1 << tmp1;

    pBitStream->getBits(3, tmp1);
    sliceHeader.nDCQuant = 1 << tmp1;
    
    // AltQuant/AltDequant
    pBitStream->getBits(3, tmp1);
    sliceHeader.nAltnMQuant =  1 << tmp1;

    pBitStream->getBits(3, tmp1);
    sliceHeader.nAltnDCQuant = 1 << tmp1;
    // Layer0 Type
    sliceHeader.layerHeaders[0].nLayerType = pBitStream->GolombReadUE();

    return true;
}

bool CSVCSliceDecoder::DecodeLayerHeader(CBitStream *pBitStream, SVCLayerHeader & layerHeader)
{
    unsigned int Code;

    // reserved
    pBitStream->getBits(1, Code);
    if(!Code) {
      PRINTF("\n reserved bit in LayerHeader is not set to 1");
      return false;
    }  
 
    // Layer Size
    pBitStream->getBits(SVC_LAYERSIZE_BITS, layerHeader.nLayerSize);

    // Last Layer Flag
    pBitStream->getBits(1, layerHeader.bLastLayer);

    // Layer Type
    layerHeader.nLayerType = pBitStream->GolombReadUE();

    return true;
}

bool CSVCSliceDecoder::DecodeLayerHeaders(uint8_t * a_pData, size_t a_nDataLen,
        SVCSliceHeader & sliceHeader,
        CBitStream * a_pLayerBitStream[])
{
    bool bRetVal = true;

    // We already have layer zero
    sliceHeader.nNumberOfLayers = 0;

    //sliceHeader.layerHeaders[0] = sliceHeader.layer0Header;

    // Walk down the data to discover all layers
    uint8_t * src = a_pData;
    int len = 4;
    src += 4;
    int layerNo = 0;

    while (true) {
//        printf("Layer[%d]: size=%d type=%d\n", layerNo,
//                a_LayerHeaders[layerNo].nLayerSize, a_LayerHeaders[layerNo].nLayerType);

        if (sliceHeader.layerHeaders[layerNo].nLayerSize == 0) {
            printf("Layer size == 0\n");
            sliceHeader.nNumberOfLayers = layerNo;
            bRetVal = false;
            break;
        }
        // Error if size goes beyond total length
        if (len +  sliceHeader.layerHeaders[layerNo].nLayerSize > a_nDataLen) {
            printf("Layer %d size %d longer than slice length %d so far = %d\n",
                    layerNo, sliceHeader.layerHeaders[layerNo].nLayerSize, a_nDataLen, len);
            sliceHeader.nNumberOfLayers = layerNo;
            bRetVal = false;
            break;
        }

        // Done if this is the last layer
        if (sliceHeader.layerHeaders[layerNo].bLastLayer) {
            sliceHeader.nNumberOfLayers = layerNo + 1;
            break;
        }

        // Decode next layer header
        len += sliceHeader.layerHeaders[layerNo].nLayerSize;
        src += sliceHeader.layerHeaders[layerNo].nLayerSize;
        layerNo++;
        if (layerNo >= SVC_MAX_DECODER_LAYERS) {
            sliceHeader.nNumberOfLayers = layerNo - 1;
            break;
        }

        if (a_pLayerBitStream != NULL) {
            if (a_pLayerBitStream[layerNo] == NULL)
                a_pLayerBitStream[layerNo] = new CBitStream();
            a_pLayerBitStream[layerNo]->init(src, a_nDataLen - len);
            DecodeLayerHeader(a_pLayerBitStream[layerNo], sliceHeader.layerHeaders[layerNo]);
        }
        else {
            CBitStream bitStream(src, a_nDataLen - len);
            DecodeLayerHeader(&bitStream, sliceHeader.layerHeaders[layerNo]);
        }

    }

    return bRetVal;
}

extern "C" {

int ParseSVCSliceLayers(uint8_t * a_pData, int a_nDataLen, SVCSliceHeader * sliceHeader)
{
    if (CSVCSliceDecoder::DecodeSVCSliceLayers(a_pData, a_nDataLen, *sliceHeader)) {
        return 1;
    }
    return 0;
}

int StripSVCLayer(uint8_t * a_pData, int a_nDataLen,
        const SVCSliceHeader * sliceHeader, uint8_t * a_outBuf, int stripLayerType)
{
    // Find out how much to copy
    int totalSize = 4;
    int lastLayerOffset = 4;

    for (int i = 0; i < (int) sliceHeader->nNumberOfLayers; i++) {
        if (stripLayerType == SVC_LAYERTYPE_REMAC4x4LAYER) {
            if (CSVCSliceDecoder::IsRemACLayer(sliceHeader->layerHeaders[i].nLayerType))
                break;
        }
        if (stripLayerType == SVC_LAYERTYPE_MIDAC4x4LAYER) {
            if (CSVCSliceDecoder::IsMidACLayer(sliceHeader->layerHeaders[i].nLayerType))
                break;
        }
        lastLayerOffset = totalSize;
        totalSize += sliceHeader->layerHeaders[i].nLayerSize;
        if (totalSize > a_nDataLen) {
            totalSize = a_nDataLen;
        }
    }

    // Copy data to output buffer
    memcpy(a_outBuf, a_pData, totalSize);

    // Update last layer flag
    *(a_outBuf + lastLayerOffset + 2) |= 0x80;

#if 0
    // Round to multiple of 4, enable this function if required for 4 byte boundary
    while (totalSize % 4 != 0) {
        *(a_outBuf + totalSize) = 0;
        totalSize++;
    }
#endif     
    return totalSize;
}

} // extern "C"

bool CSVCSliceDecoder::DecodeSVCSliceLayers(uint8_t * a_pData, size_t a_nDataLen,
        SVCSliceHeader & sliceHeader)
{
    /* TODO : Add Demulation check for Layer Headers from SRD  */
    sliceHeader.nNumberOfLayers = 0;
    CBitStream bitStream(a_pData, a_nDataLen);
    if (!DecodeSliceHeader(&bitStream, sliceHeader))
        return false;
    DecodeLayerHeaders(a_pData, a_nDataLen, sliceHeader, NULL);
    return true;
}

bool CSVCSliceDecoder::DecodeHeaders(int & iMQuant, int & iDCQuant) {

    int reducedbytes = CEmulationPrevention::DeemulateSlice(
            (uint8_t *) m_pBitStream->getStartPointer(),
            m_pBitStream->getLength());
    if (reducedbytes > 0)
        m_pBitStream->ReduceLength(reducedbytes);

    if (reducedbytes != 0) {
        printf("DecodeHeaders DeEmulatiuon in prevention returned with %d\n",
                reducedbytes);
        m_nLayerSizeError++;
    }


    // First decode the slice header which gives us layer0 header too
    if (!DecodeSliceHeader(m_pBitStream, m_SliceHeader))
        return false;

    iMQuant = m_SliceHeader.nMQuant;
    iDCQuant = m_SliceHeader.nDCQuant;
    m_nSliceNumber = m_SliceHeader.nSliceNumber;

    // Discover other layers
    DecodeLayerHeaders((uint8_t *)m_pBitStream->getStartPointer(), m_pBitStream->getLength(),
            m_SliceHeader, m_pLayerBitStream);
    return true;
}

bool CSVCSliceDecoder::DecodeData(uint8_t * pYUV[3], int iMQuant, int iDCQuant, int a_nSliceSize) {

    int yuvStrides[3] = { m_nSliceWidth * 8, m_nSliceWidth * 4,
        m_nSliceWidth * 4 };
    if (!DecodeSlice(pYUV, yuvStrides)) {
        PRINTF("Unable to decode slice\n");
        return false;
    }
    return true;
}

bool CSVCSliceDecoder::DecodeSlice(uint8_t * a_pYUV[3], int a_yuvStrides[3])
{
    if (m_SliceHeader.layerHeaders[0].nLayerType == SVC_LAYERTYPE_DCLAYER) {
        return DecodeScalableLayers(a_pYUV, a_yuvStrides);
    }
    else if (m_SliceHeader.layerHeaders[0].nLayerType == SVC_LAYERTYPE_FULLLAYER) {
        return DecodeFullLayer(a_pYUV, a_yuvStrides);
    }
    else if (m_SliceHeader.layerHeaders[0].nLayerType == SVC_LAYERTYPE_LOSSLESSLAYER) {
        return DecodeLosslessLayer(a_pYUV, a_yuvStrides);
    }

    return false;
}

int CSVCSliceDecoder::GetDecodeType ()
{
    return m_nDecodeType;
}

extern "C" {
int IsMidACLayerType(int layerType) {
    if (CSVCSliceDecoder::IsMidACLayer(layerType))
        return 1;
    return 0;
}

int IsRemACLayerType(int layerType) {
    if (CSVCSliceDecoder::IsRemACLayer(layerType))
        return 1;
    return 0;
}
}

bool CSVCSliceDecoder::IsMidACLayer(int layer_type) {
    switch (layer_type) {
    case SVC_LAYERTYPE_MIDAC4x4LAYER:
    case SVC_LAYERTYPE_MIDAC2x2LAYER:
    case SVC_LAYERTYPE_MIDAC3x3LAYER:
    case SVC_LAYERTYPE_MIDAC5x5LAYER:
    case SVC_LAYERTYPE_MIDAC6x6LAYER:
    case SVC_LAYERTYPE_MIDAC7x7LAYER:
        return true;
    }
    return false;
}

bool CSVCSliceDecoder::IsRemACLayer(int layer_type) {
    switch (layer_type) {
    case SVC_LAYERTYPE_REMAC4x4LAYER:
    case SVC_LAYERTYPE_REMAC2x2LAYER:
    case SVC_LAYERTYPE_REMAC3x3LAYER:
    case SVC_LAYERTYPE_REMAC5x5LAYER:
    case SVC_LAYERTYPE_REMAC6x6LAYER:
    case SVC_LAYERTYPE_REMAC7x7LAYER:
        return true;
    }
    return false;
}

void CSVCSliceDecoder::SetupLayerDimension(int layer_type) {
    switch (layer_type) {
    case SVC_LAYERTYPE_MIDAC4x4LAYER:
        m_nLayerDimension = 4;
        break;
    case SVC_LAYERTYPE_MIDAC2x2LAYER:
        m_nLayerDimension = 2;
        break;
    case SVC_LAYERTYPE_MIDAC3x3LAYER:
        m_nLayerDimension = 3;
        break;
    case SVC_LAYERTYPE_MIDAC5x5LAYER:
        m_nLayerDimension = 5;
        break;
    case SVC_LAYERTYPE_MIDAC6x6LAYER:
        m_nLayerDimension = 6;
        break;
    case SVC_LAYERTYPE_MIDAC7x7LAYER:
        m_nLayerDimension = 7;
        break;
    default:
        m_nLayerDimension = 4;
        break; 
    }
}


bool CSVCSliceDecoder::DecodeScalableLayers(uint8_t * a_pYUV[3],
                                int a_yuvStrides[3])
{
    unsigned int alt_quant_sel;
    unsigned int dcquant;
    unsigned int acquant;
    m_nQuadLayerIndex = -1;
    m_nRemLayerIndex = -1;
    m_nDecodeType = DECODE_TYPE_SCALABLE_DC;

    if (m_SliceHeader.nNumberOfLayers > 1) {
        for (unsigned int layerNo = 1; layerNo < m_SliceHeader.nNumberOfLayers; layerNo++) {
            if (IsMidACLayer(m_SliceHeader.layerHeaders[layerNo].nLayerType)) {
                SetupLayerDimension(m_SliceHeader.layerHeaders[layerNo].nLayerType);
                m_nQuadLayerIndex = layerNo;
            }
            if (IsRemACLayer(m_SliceHeader.layerHeaders[layerNo].nLayerType))
                m_nRemLayerIndex = layerNo;
        }
        if (m_nQuadLayerIndex <= 0) {
            m_nQuadLayerIndex = -1;
        }
        else if (m_nRemLayerIndex <= m_nQuadLayerIndex) {
            m_nRemLayerIndex = -1;
            m_nDecodeType = DECODE_TYPE_SCALABLE_QUAD;
        }
        else {
            m_nDecodeType = DECODE_TYPE_SCALABLE_REM;
        }
    }

    int dcDctPred[3] = {0, 0, 0};

    uint8_t * yArrayPos;
    uint8_t * uArrayPos;
    uint8_t * vArrayPos;
    for (int rowNo = 0; rowNo < m_nSliceHeight; rowNo++) {
        yArrayPos = a_pYUV[0] + rowNo * 8 * a_yuvStrides[0];
        uArrayPos = a_pYUV[1] + rowNo * 8 * a_yuvStrides[1];
        vArrayPos = a_pYUV[2] + rowNo * 8 * a_yuvStrides[2];


        for (int nBlkNo = 0; nBlkNo < m_nSliceWidth / 2; nBlkNo++) {
            m_pBitStream->getBits(1, alt_quant_sel); // check for Alt quant usage
            dcquant = (alt_quant_sel) ?  m_SliceHeader.nAltnDCQuant : m_SliceHeader.nDCQuant;
            acquant = (alt_quant_sel) ?  m_SliceHeader.nAltnMQuant : m_SliceHeader.nMQuant;

            if (!DecodeBlock(&m_psrcoeffY[(2 * nBlkNo) * V2D_BLOCK_SIZE],
                    &dcDctPred[0], acquant, dcquant, 0,
                    yArrayPos + 2 * nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[0]))
                goto decodeslice_fail;

            if (!DecodeBlock(&m_psrcoeffU[nBlkNo * V2D_BLOCK_SIZE],
                    &dcDctPred[1],acquant, dcquant, 4,
                    uArrayPos + nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[1]))
                goto decodeslice_fail;

            if (!DecodeBlock(&m_psrcoeffY[(2 * nBlkNo + 1) * V2D_BLOCK_SIZE],
                    &dcDctPred[0], acquant, dcquant, 0,
                    yArrayPos + (2 * nBlkNo + 1) * 8/m_nScaleFactor,
                    a_yuvStrides[0]))
                goto decodeslice_fail;

            if (!DecodeBlock(&m_psrcoeffV[nBlkNo * V2D_BLOCK_SIZE],
                    &dcDctPred[2], acquant, dcquant, 5,
                    vArrayPos + nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[2]))
                goto decodeslice_fail;

        } //for
        //break;
    }
    return true;

    decodeslice_fail: return false;
}

bool CSVCSliceDecoder::DecodeFullLayer(uint8_t * a_pYUV[3],
                                int a_yuvStrides[3])
{
    m_nQuadLayerIndex = 0;
    m_nRemLayerIndex = 0;
    m_nDecodeType = DECODE_TYPE_SCALABLE_FULL;


    int   dcDctPred[3] = {0, 0, 0};

    uint8_t * yArrayPos;
    uint8_t * uArrayPos;
    uint8_t * vArrayPos;
    for (int rowNo = 0; rowNo < m_nSliceHeight; rowNo++) {
        yArrayPos = a_pYUV[0] + rowNo * 8 * a_yuvStrides[0];
        uArrayPos = a_pYUV[1] + rowNo * 8 * a_yuvStrides[1];
        vArrayPos = a_pYUV[2] + rowNo * 8 * a_yuvStrides[2];


        for (int nBlkNo = 0; nBlkNo < m_nSliceWidth / 2; nBlkNo++) {
            if (!DecodeBlock(&m_psrcoeffY[(2 * nBlkNo) * V2D_BLOCK_SIZE],
                    &dcDctPred[0], m_SliceHeader.nMQuant, m_SliceHeader.nDCQuant, 0,
                    yArrayPos + 2 * nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[0]))
                goto decodeslice_fail;


            if (!DecodeBlock(&m_psrcoeffU[nBlkNo * V2D_BLOCK_SIZE],
                    &dcDctPred[1], m_SliceHeader.nMQuant, m_SliceHeader.nDCQuant, 4,
                    uArrayPos + nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[1]))
                goto decodeslice_fail;

            if (!DecodeBlock(&m_psrcoeffY[(2 * nBlkNo + 1) * V2D_BLOCK_SIZE],
                    &dcDctPred[0], m_SliceHeader.nMQuant, m_SliceHeader.nDCQuant, 0,
                    yArrayPos + (2 * nBlkNo + 1) * 8/m_nScaleFactor,
                    a_yuvStrides[0]))
                goto decodeslice_fail;


            if (!DecodeBlock(&m_psrcoeffV[nBlkNo * V2D_BLOCK_SIZE],
                    &dcDctPred[2], m_SliceHeader.nMQuant, m_SliceHeader.nDCQuant, 5,
                    vArrayPos + nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[2]))
                goto decodeslice_fail;

        } //for
        //break;
    }
    return true;

    decodeslice_fail: return false;
}

bool CSVCSliceDecoder::DecodeLosslessLayer(uint8_t * a_pYUV[3],
                                int a_yuvStrides[3])
{
    m_nDecodeType = DECODE_TYPE_SCALABLE_LOSSLESS;
    uint8_t * yArrayPos;
    uint8_t * uArrayPos;
    uint8_t * vArrayPos;
    for (int rowNo = 0; rowNo < m_nSliceHeight; rowNo++) {
        yArrayPos = a_pYUV[0] + rowNo * 8 * a_yuvStrides[0];
        uArrayPos = a_pYUV[1] + rowNo * 8 * a_yuvStrides[1];
        vArrayPos = a_pYUV[2] + rowNo * 8 * a_yuvStrides[2];


        for (int nBlkNo = 0; nBlkNo < m_nSliceWidth / 2; nBlkNo++) {
            if (!DecodeLosslessBlock(&m_psrcoeffY[(2 * nBlkNo) * V2D_BLOCK_SIZE], 0,
                    yArrayPos + 2 * nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[0]))
                goto decodeslice_fail;


            if (!DecodeLosslessBlock(&m_psrcoeffU[nBlkNo * V2D_BLOCK_SIZE], 4,
                    uArrayPos + nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[1]))
                goto decodeslice_fail;

            if (!DecodeLosslessBlock(&m_psrcoeffY[(2 * nBlkNo + 1) * V2D_BLOCK_SIZE], 0,
                    yArrayPos + (2 * nBlkNo + 1) * 8/m_nScaleFactor,
                    a_yuvStrides[0]))
                goto decodeslice_fail;


            if (!DecodeLosslessBlock(&m_psrcoeffV[nBlkNo * V2D_BLOCK_SIZE], 5,
                    vArrayPos + nBlkNo * 8/m_nScaleFactor,
                    a_yuvStrides[2]))
                goto decodeslice_fail;

        } //for
    }
    return true;

    decodeslice_fail: return false;
}

bool CSVCSliceDecoder::DecodeBlock(short* a_block,
                                int* a_dcDctPred,
                                const int a_mquant,
                                const int a_dcquant,
                                const int a_comp,
                                uint8_t *pDest,
                                const int aStride)
{
    int nc, val;
    unsigned int idx, code, run, val2, sign;
    const DCTtab *tab;
    int inRemLayer;


    if (a_comp == 0)
        m_nBlocksDecoded++;

    memset(a_block, 0x00, sizeof(short) * 64);

    unsigned int layer_dimension_switch_coeff = m_layer_dimension_map[m_nLayerDimension];

    // Get DC coefficients
    GetDcDctDiff(val);

    val += *a_dcDctPred;
    *a_dcDctPred = val;
    a_block[0] = val * a_dcquant;

    CBitStream * pBitStream;

    if (m_nQuadLayerIndex < 0)
        goto idct_block;
    if (m_nQuadLayerIndex == 0)
        pBitStream = m_pBitStream;
    else
        pBitStream = m_pLayerBitStream[m_nQuadLayerIndex];

    // Get AC coefficients
    nc = 0;
    inRemLayer = 0;

    for (idx = 1;; idx++) {
        pBitStream->showBits(16, code);

        if (code >= 1024) {
            tab = &DCTtab0a[(code >> 8) - 4];
        }
        else if (code >= 512) {
            tab = &DCTtab1a[(code >> 6) - 8];
        }
        else if (code >= 256)
            tab = &DCTtab2[(code >> 4) - 16];
        else if (code >= 128)
            tab = &DCTtab3[(code >> 3) - 16];
        else if (code >= 64)
            tab = &DCTtab4[(code >> 2) - 16];
        else if (code >= 32)
            tab = &DCTtab5[(code >> 1) - 16];
        else if (code >= 16)
            tab = &DCTtab6[code - 16];
        else {
            //printf("code = %d\n", code);
            return true;
        }

        pBitStream->flushBits(tab->len);

        if (tab->run == 64) /* end_of_block */
        {
            if (m_nRemLayerIndex > 0) {
                if (inRemLayer == 0) {
                    // First end of block, so switch to Rem Layer encoding
                    pBitStream = m_pLayerBitStream[m_nRemLayerIndex];
                    inRemLayer = 1;
                    // Index is set to 1 less so that next loop begins at 16
                    //i = 15;
                    idx = layer_dimension_switch_coeff - 1;
                    continue;
                }
                else {
                    // Already in Rem Layer encoding, we are done
                    break;
                }
            }
            else {
                // No separate Rem layer detected so we are done
                break;
            }
        }

        if (tab->run == 65) /* escape */
        {
            pBitStream->getBits(6, run);
            //printf("escape %d %d\n", i, run);
            idx += run;

            pBitStream->getBits(12, val2);

            val = val2;

            if ((val & 2047) == 0) {
                //printf("(val & 2047) == 0\n");
                break;
            }
            if ((sign = (val >= 2048)))
                val = 4096 - val;
        }
        else {
            //printf("regular %d %d\n", i, tab->run);
            idx += run = tab->run;
            val = tab->level;
            pBitStream->getBits(1, sign);
        }
        if (inRemLayer == 0 &&
                idx >= layer_dimension_switch_coeff &&
                m_nRemLayerIndex > 0) {
            //printf("Error in Rem layer decoding %d\n", i);
            goto decodeblock_fail;
        }

        if (idx >= 64) {
            //printf("i >= 64\n");
            break;
        }

        // Un zig-zag
        unsigned int j = m_scan_matrix[idx];

        val = ((val * a_mquant) << quant_mat_mult[j]) >> 4;
        a_block[j] = sign ? -val : val;

        nc++;
    }

  idct_block:
    if (m_nScaleFactor == 1) {
        //dumpBlock(a_block, "Before IDCT");
        IDct(a_block);
        //dumpBlock(a_block, "After IDCT");
        add128AndPutTransposedBlock(a_block, pDest, aStride);
        //dumpBuffer8x8(pDest, aStride, "After Transpose");
        //CFDCT::IDCT4x4_Add128_Transpose(a_block, pDest, aStride);
    }
    else if (m_nScaleFactor == 2) {
        CFDCT::IDCT4x4_Add128_Transpose(a_block, pDest, aStride >> 1);
    }
    else if (m_nScaleFactor == 4) {
        CFDCT::IDCT2x2_Add128_Transpose(a_block, pDest, aStride >> 2);
    }
    else if (m_nScaleFactor == 8) {
        // 1x1 IDCT is trivial, just divide DC coeef by 8
        *(pDest) = _S((a_block[0] >> 3) + 128);
    }

    return true;

    decodeblock_fail: return false;
}


bool CSVCSliceDecoder::DecodeLosslessBlock(short* a_block,
    const int a_comp,
    uint8_t *pDest,
    const int aStride)
{
    int val, lastval, diff;

    if (a_comp == 0)
        m_nBlocksDecoded++;

    // Get first value
    val = lastval = GolombReadSE(m_pBitStream);
    a_block[0] = val;

    // Get all the other 63 values as the difference from previous
    // value in zig-zag order
    for (int i = 1; i < 64; i++) {
        diff = GolombReadSE(m_pBitStream);
        val = lastval + diff;
        lastval = val;

        // Un zig-zag
        unsigned int j = m_scan_matrix[i];
        a_block[j] = val;
    }

    // Recover the pixel values
    add128AndPutBlock(a_block, pDest, aStride);

    return true;
}

uint32_t CSVCSliceDecoder::GetLayerErrors() {
    return m_nLayerSizeError;
}
