/*
 * h264sliceencoder.hh
 *
 *  Created on: Oct 29, 2010
 *      Author: rkale
 */

#ifndef H264SLICEENCODER_HH_
#define H264SLICEENCODER_HH_

#include "sliceencoder.hh"

class CH264SliceEncoder : public CSliceEncoder {

public:
    CH264SliceEncoder(int a_nMQuant, int a_nDCQuant);
    virtual ~CH264SliceEncoder();

    virtual void SetQuantizationParameter(int a_qp);

protected:
    virtual void PrintStats();
    virtual void EncodeData();
    virtual void PutQuantBits();

private:

    uint8_t * m_yArrayPred;
    uint8_t * m_uArrayPred;
    uint8_t * m_vArrayPred;
    int m_qp;
    int m_luma4x4[16][16];
    int m_chroma_dcU[2][4];
    int m_chroma_acU[2][4][16];
    int m_chroma_dcV[2][4];
    int m_chroma_acV[2][4][16];
    int m_non_zero[16];
    int m_cbp_luma;
    int m_cbp_chroma;
#define SCAN8_SIZE (6*8)
    int m_intra4x4_pred_mode[H264_SLICE_WIDTH][SCAN8_SIZE];
    int m_non_zero_count[H264_SLICE_WIDTH][SCAN8_SIZE];
    int m_chroma_predict_mode[2];


    // Private methods
    void EncodeH264LumaBlocks(uint8_t * a_arrayPos, uint8_t * a_arrayPredPos,
            short a_stride, unsigned int a_iMQuant, int a_blkNo, int a_firstBlkFlg, int a_firstRowFlag);
    void EncodeH264ChromaBlocks(
            uint8_t * a_uArrayPos, uint8_t * a_uArrayPredPos,
            uint8_t * a_vArrayPos, uint8_t * a_vArrayPredPos,
            short a_stride, unsigned int a_iMQuant,
            int a_blkNo, int a_firstBlkFlg, int a_firstRowFlag);

    void WriteH264Blocks(int a_blkNo);
    void LoadNewBlock(int a_rowNo, int a_blkNo);

};


#endif /* H264SLICEENCODER_HH_ */
