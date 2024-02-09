/*
 * h264slicedecoder.hh
 *
 *  Created on: Nov 1, 2010
 *      Author: rkale
 */

#ifndef H264SLICEDECODER_HH_
#define H264SLICEDECODER_HH_

#include "slicedecoder.hh"

class CH264SliceDecoder : public CSliceDecoder
{
public:

    CH264SliceDecoder();
    ~CH264SliceDecoder();


protected:
    virtual bool DecodeHeaders(int & iMQuant, int & iDCQuant);
    virtual bool DecodeData(uint8_t * pYUV[3],
            int iMQuant, int iDCQuant, int a_nSliceSize);
    virtual bool DecodeSlice(const int a_nMQuant,
                        const int a_nDCQuant,
                        const size_t a_nSliceSize,
                        uint8_t * a_pYUV[3],
                        int a_yuvStrides[3]);
    virtual bool GetQuantBits(int& a_mQuant, int& a_dcQuant);

private:

    int m_qp;
    int m_quant4_mf[6][4][4];
    int m_dequant4_mf[6][4][4];
    struct x264_vlc_table_t *m_coeff_token_lookup[5];
    struct x264_vlc_table_t *m_level_prefix_lookup;
    struct x264_vlc_table_t *m_total_zeros_lookup[15];
    struct x264_vlc_table_t *m_total_zeros_dc_lookup[3];
    struct x264_vlc_table_t *m_run_before_lookup[7];
#define SCAN8_SIZE (6*8)
    int m_intra4x4_pred_mode[H264_SLICE_WIDTH][SCAN8_SIZE];
    int m_non_zero_count[H264_SLICE_WIDTH][SCAN8_SIZE];
    int m_chroma_predict_mode[2];
    int m_cbp;

    bool ReadBlockHeaders(int aBlkNo, int aRowNo);

    bool DecodeLumaBlocks(
        int aBlkNo, int aRowNo,
        const int a_mquant,
        const int a_dcquant,
        const int a_comp,
        uint8_t *pDest,
        const int aStride);

    bool DecodeChromaBlocks(
        int aBlkNo, int aRowNo,
        const int a_mquant,
        const int a_dcquant,
        const int a_comp,
        uint8_t *pDestU,
        uint8_t *pDestV,
        const int aStride);

    void LoadNewBlock(int a_rowNo, int a_blkNo);
    void InitSVLCTable();
    int ReadVLC(struct x264_vlc_table_t *table);
    int DecodeBlockCavlc(int i_idx, int *l, int i_count, int nonZeroPredict, int & nonZero);

};
#endif /* H264SLICEDECODER_HH_ */
