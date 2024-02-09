#ifndef __V2D_SLICEDECODER_H__
#define __V2D_SLICEDECODER_H__

#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "bitstream.h"
#include "ipputils.h"

struct x264_vlc_table_t; // forward declaration


#define    V2D_START_R_FRAME_DETECT   1
#define    V2D_START_L_FRAME_DETECT   2
#define    V2D_END_FRAME_DETECT       3
#define    V2D_START_SLICE_DETECT     4
#define    V2D_END_SLICE_DETECT       5
#define    V2D_NO_START_FRAME_DETECT  6
#define    V2D_NO_END_SLICE_DETECT    7
#define    V2D_NO_START_SLICE_DETECT  8

class DLLEXPORT CSliceDecoder
{
public:

    CSliceDecoder(eEncodeFormat aEncodeFormat = eYUV422P);
    virtual ~CSliceDecoder();

    bool Decode(uint8_t * a_pData, size_t a_nDataLen,
        size_t a_nSliceSize, 
        uint8_t * a_pY,
        uint8_t * a_pU,
        uint8_t * a_pV,
        int &a_rnSliceNum);

    bool DecodeHeader(uint8_t * a_pData, size_t a_nDataLen,
            int & a_rnSliceNum, bool & a_bIsRefreshSlice);


    unsigned int  GetSliceNum() const { return m_nSliceNumber; }
    bool  IsRefreshSlice() const { return m_bIsRefreshSlice; }
    void  SetScaleFactor(int a_nScaleFactor);
    static bool CheckRange(unsigned int aValue, unsigned int aMin, unsigned int aMax);
    static CSliceDecoder * CreateSliceDecoder(eEncodeFormat aEncodeFormat);

protected:


    virtual bool DecodeHeaders(int & iMQuant, int & iDCQuant);
    virtual bool DecodeData(uint8_t * pYUV[3],
            int iMQuant, int iDCQuant, int a_nSliceSize);

    void InitIdct();
    void IDct(short* a_psBlock);
	void IDctCol(short* a_psBlock);
	void IDctRow(short* a_psBlock);

    bool IsSliceZero();

    bool ParseStartFrameCode(int& a_mQuant, int& a_dcQuant);

    bool ParseStartSliceCode(int& a_mQuant, int& a_dcQuant);
    virtual bool GetQuantBits(int& a_mQuant, int& a_dcQuant);

    virtual bool DecodeSlice(const int a_nMQuant,
                        const int a_nDCQuant, 
                        const size_t a_nSliceSize,
                        uint8_t * a_pYUV[3],
                        int a_yuvStrides[3]);


    virtual bool DecodeBlock(short* a_block,
        int* a_dcDctPred, 
        const int a_mquant, 
        const int a_dcquant, 
        const int a_comp, 
        uint8_t * pDest,
        const int aStride);


    bool GetDcDctDiff(int &a_rnDctDiff);

    void Add128AndSaturateto8BitSlice(const size_t a_nSliceSize,
            uint8_t * a_pY,
            uint8_t * a_pU,
            uint8_t * a_pV);

    void CopyToRgbImage(const size_t a_nSliceSize,
            uint8_t * aDest, int aStride);

    CSliceDecoder(const CSliceDecoder& a_rSD);                  
    const CSliceDecoder& operator=(const CSliceDecoder& a_rSD); 


    static int GolombReadUE(CBitStream* pBitStream);
    static int GolombReadSE(CBitStream* pBitStream);
    static int GolombReadTE(CBitStream* pBitStream, int x);

    unsigned int m_nSliceNumber;
    bool m_bIsRefreshSlice;

    CBitStream* m_pBitStream;

    short*      m_psrcoeffY;
    short*      m_psrcoeffU;
    short*      m_psrcoeffV;
    int         m_nScaleFactor;
    int         m_nSliceWidth;
    int         m_nSliceHeight;

    uint8_t * m_scan_matrix;

#ifdef USE_IPP_5DOT3
    IppiSize    m_blockRoi;
#endif

#ifdef USE_IPP
    IppiSize    m_blockRoi;
#endif
    short   m_iclip[1024];
    short*  m_iclp;
    uint32_t m_nBlocksDecoded;

};


inline bool CSliceDecoder::CheckRange(unsigned int aValue, unsigned int aMin, unsigned int aMax) {
    if (aValue < aMin || aValue > aMax)
        return false;
    return true;
}

#endif //__V2D_SLICEDECODER_H__
