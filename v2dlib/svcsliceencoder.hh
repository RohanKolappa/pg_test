/*
 * svcsliceencoder.hh
 *
 *  Created on: Oct 31, 2010
 *      Author: rkale
 */

#ifndef SVCSLICEENCODER_HH_
#define SVCSLICEENCODER_HH_

#include "sliceencoder.hh"


#define SVC_MAX_ENCODER_LAYERS     8

class CSVCSliceEncoder : public CSliceEncoder
{

public:
    CSVCSliceEncoder(int a_nMQuant, int a_nDCQuant, int a_nSliceWidth = 8, int a_nSliceHeight = 4);
    virtual ~CSVCSliceEncoder();

    virtual int AddFrameHeader(uint8_t * a_outBuff, const SVCFrameHeader & frameHeader);
    virtual void SetRefreshSliceBit(uint8_t * a_outBuff);
    void SetEncodingType(eSVCEncodingType a_eEncodingType);
    void SetEncodingSubType(eSVCEncodingSubType a_eEncodingSubType);
    eSVCEncodingType GetEncodingType();
    eSVCEncodingSubType GetEncodingSubType();
    void SetLayerDimension(int aLayerDimension);
    int GetLayerDimension();

protected:
    virtual void InitEncode();
    virtual void EncodeHeaders();
    virtual void EncodeData();
    virtual int EncodeFooters();
    virtual void CollectACStats(int n, int signed_level);
    virtual void PutIntraBlk(short* blk, int cc, short* dc_dct_pred,
            int a_firstBlkFlg, int nonZeroCoefficients);

private:
    enum eSVCEncodingType m_eEncodingType;
    enum eSVCEncodingSubType m_eEncodingSubType;
    int m_nNumberOfLayers;
    unsigned char * m_pLayerData[SVC_MAX_ENCODER_LAYERS];
    int m_nLayerSize[SVC_MAX_ENCODER_LAYERS];
    CBitStreamWriter * m_pLayerBitStream[SVC_MAX_ENCODER_LAYERS];
    CBitStreamWriter * m_pLayer0BitStream;
    int m_nLayerDimension;

    static unsigned char m_layer_dimension_map[9];

    void SetLayerSize(uint8_t * a_outBuff, int a_nSize, bool a_bLastLayer);
    void AddSVCSliceHeader(int a_nSliceNumber);
    void AddSVCLayerHeader(CBitStreamWriter * a_pBitStream, int layer_type);
    int GetMidACLayerType();
    int GetRemACLayerType();
    void EncodeScalableLayers();
    void EncodeFullLayer();
    void EncodeLosslessLayer();
    void EncodeLosslessBlock(uint8_t * a_arrayPos, short a_stride, int a_cc,
            short* a_dc_dct_pred, int a_firstBlkFlg);
    void InitializeLayerData(int index);
    void SwitchBitstreamToLayer(int index);

};

#endif /* SVCSLICEENCODER_HH_ */
