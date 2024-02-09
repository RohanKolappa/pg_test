/*
 * sliceencoder.hh
 *
 *  Created on: Oct 22, 2009
 *      Author: rkale
 */

#ifndef SLICEENCODER_HH_
#define SLICEENCODER_HH_

#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "ipputils.h"
#include "bitstreamwriter.hh"


class  DLLEXPORT CSliceEncoder {
public:
    virtual ~CSliceEncoder();

    int EncodeSlice(uint8_t * a_outBuff, uint8_t * a_bitmapArray,
            int a_nStride, int a_nSliceNumber = 1, int a_nSliceSplit = 0);
    virtual void SetQuantizationParameter(int a_qp);
    virtual int AddFrameHeader(uint8_t * a_outBuff, const SVCFrameHeader & frameHeader);
    virtual int AddEndOfFrame(uint8_t * a_outBuff);
    virtual void SetRefreshSliceBit(uint8_t * a_outBuff);
    virtual void SetBytesPerPixel(int a_nBytesPerPixel);
    void SetTestable(bool a_value);
    static CSliceEncoder * CreateSliceEncoder(int a_nMQuant, int a_nDCQuant,
            int a_nSliceSize);
    static CSliceEncoder * CreateSliceEncoder(int a_nMQuant, int a_nDCQuant,
            int a_nSliceWidth, int a_nSliceHeight);
    static CSliceEncoder * CreateSliceEncoder(int a_nMQuant, int a_nDCQuant);
    void SetLeftRightFlag(int lrflag);

protected:

    short * m_block;
    CBitStreamWriter * m_pBitStream;

    int m_nWidth;
    int m_nHeight;

    int m_nMMult;
    int m_nMQuant;
    int m_nDCQuant;
    short m_nDCMult;

    int m_nSliceSize;
    int m_nSliceWidth;
    int m_nSliceHeight;
    int m_nBytesPerPixel;

    uint8_t * m_pBufferStart;
    uint8_t * m_yArray;
    uint8_t * m_uArray;
    uint8_t * m_vArray;

    int m_nLRFlag;
    uint32_t  m_nBlocksEncoded;
    int m_nSliceNumber;
    bool m_bPassTests;

#ifdef USE_IPP
    float m_quant_matrix[64];
    uint8_t * m_yuvarray[3];
    int m_dstStep[3];
    IppiSize m_sliceRoi;
    IppiSize m_blockRoi;
    IppVCHuffmanSpec_32s *m_vlcTableB15;
#endif


    CBitStreamStats *m_pBitStreamStats;

    static unsigned char quant_mat_trans[64];
    static unsigned char quant_mat_trans_mult[64];
    static VLCtable dct_code_tab1[2][40];
    static VLCtable dct_code_tab2[30][5];
    static VLCtable dct_code_tab1a[2][40];
    static VLCtable dct_code_tab2a[30][5];
    static unsigned char zig_zag_scan[64];
    static unsigned char m_progressive_scan[64];
    static sVLCtable DClumtabHW[12];
#ifdef USE_IPP
    static const IppVCHuffmanSpec_32u Y_DC_Tbl[12];
    static const Ipp32s Table15[];
#endif


    CSliceEncoder(int a_nMQuant, int a_nDCQuant,
            int a_nSliceSize, int a_nSliceWidth = 0, int a_nSliceHeight = 0); // protected constructor

    void PutStartFrameCode(void);
    void PutStartSliceCode(int a_nSliceNumber);
    virtual void PutQuantBits();
    void PutEndSliceCode(void);

    virtual void PrintStats();
    virtual void InitEncode();
    virtual void EncodeHeaders();
    virtual void EncodeData() = 0;  // Pure virtual makes this class abstract
    virtual int EncodeFooters();
    virtual void CheckSlice(uint8_t *a_buffer, int a_len);
    void RgbToYuv(uint8_t * a_bitmapArray, int a_nStride, int a_nSliceSplit);


    void PutDC(sVLCtable *tab, int val);
    void PutAC(int run, int signed_level);

    static void GetBlock(short* block, uint8_t * arrayPos, short stride);
    static void GetBlock(uint8_t * block, uint8_t * arrayPos,
            short stride);

    static void AddBlk(short* block, short offset);
    void Transpose(short* block);
    void QuantIntraOrig(short* blk, int mquant);
    void QuantIntra(short* blk);
    virtual void CollectACStats(int n, int signed_level);
    virtual void PutIntraBlk(short* blk, int cc, short* dc_dct_pred,
            int a_firstBlkFlg, int nonZeroCoefficients);

    void EncodeOneBlock(short* a_block, uint8_t * a_arrayPos,
            short a_stride, unsigned int a_iMQuant, int a_cc,
            short* a_dc_dct_pred, int a_firstBlkFlg);


private:
    void BuildTables();

    CSliceEncoder();
    CSliceEncoder(const CSliceEncoder& rhs);
    const CSliceEncoder& operator=(const CSliceEncoder& rhs);
};



#endif /* SLICEENCODER_HH_ */
