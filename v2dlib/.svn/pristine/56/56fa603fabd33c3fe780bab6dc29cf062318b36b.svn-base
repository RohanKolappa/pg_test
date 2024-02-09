/*
 * svcslicedecoder.hh
 *
 *  Created on: Nov 1, 2010
 *      Author: rkale
 */

#ifndef SVCSLICEDECODER_HH_
#define SVCSLICEDECODER_HH_

#include "slicedecoder.hh"


#define DECODE_TYPE_SCALABLE_DC           1
#define DECODE_TYPE_SCALABLE_QUAD         2
#define DECODE_TYPE_SCALABLE_REM          3
#define DECODE_TYPE_SCALABLE_FULL         4
#define DECODE_TYPE_SCALABLE_LOSSLESS     5

class CSVCSliceDecoder : public CSliceDecoder
{
public:

    CSVCSliceDecoder();
    ~CSVCSliceDecoder();

    static bool DecodeSVCFrameHeader(CBitStream* pBitStream, SVCFrameHeader & frameHeader);
    static bool DecodeSVCFrameHeader(uint8_t * a_pData, size_t a_nDataLen,
            SVCFrameHeader & frameHeader);
    static bool DecodeSVCSliceLayers(uint8_t * a_pData, size_t a_nDataLen,
            SVCSliceHeader & sliceHeader);
    static bool IsMidACLayer(int layer_type);
    static bool IsRemACLayer(int layer_type);
    void SetSliceDimensions(int aSliceWidth, int aSliceHeight);
    int GetDecodeType();
    uint32_t GetLayerErrors();

protected:
    virtual bool DecodeHeaders(int & iMQuant, int & iDCQuant);

    virtual bool DecodeData(uint8_t * pYUV[3],
            int iMQuant, int iDCQuant, int a_nSliceSize);

    bool DecodeSlice(uint8_t * a_pYUV[3], int a_yuvStrides[3]);

    virtual bool DecodeBlock(short* a_block,
        int* a_dcDctPred,
        const int a_mquant,
        const int a_dcquant,
        const int a_comp,
        uint8_t * pDest,
        const int aStride);


private:
    int m_nNumberOfLayers;
    SVCSliceHeader m_SliceHeader;
    SVCLayerHeader m_LayerHeaders[SVC_MAX_DECODER_LAYERS];
    CBitStream * m_pLayerBitStream[SVC_MAX_DECODER_LAYERS];
    int m_nQuadLayerIndex;
    int m_nRemLayerIndex;
    int m_nDecodeType;
    int m_nLayerDimension;
    uint32_t m_nLayerSizeError;


    static unsigned char m_layer_dimension_map[9];

    static bool DecodeSliceHeader(CBitStream* pBitStream, SVCSliceHeader & sliceHeader);

    static bool DecodeLayerHeader(CBitStream *pBitStream, SVCLayerHeader & layerHeader);

    static bool DecodeLayerHeaders(uint8_t * a_pData, size_t a_nDataLen,
            SVCSliceHeader & sliceHeader,
            CBitStream * a_pLayerBitStream[]);

    bool DecodeScalableLayers(uint8_t * a_pYUV[3],
            int a_yuvStrides[3]);

    bool DecodeFullLayer(uint8_t * a_pYUV[3],
            int a_yuvStrides[3]);

    bool DecodeLosslessLayer(uint8_t * a_pYUV[3],
            int a_yuvStrides[3]);

    bool DecodeLosslessBlock(short* a_block,
        const int a_comp,
        uint8_t * pDest,
        const int aStride);

    void SetupLayerDimension(int layer_type);
};

#endif /* SVCSLICEDECODER_HH_ */
