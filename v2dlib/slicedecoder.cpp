
#include "slicedecoder.hh"
#include "h264slicedecoder.hh"
#include "svcslicedecoder.hh"
#include "codecdefs.h"
#include "imagemanip.h"
#include "dct.hh"
#include "ipputils.h"
#include "svc.h"
#include "memory_tracker.h"

using namespace V2DLib;

//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define PRINTF printf
#else
#define PRINTF(...) do {} while (0)
#endif


#define UHBITS(code, nbits) (((uint32_t)(code)) >> (32 - (nbits)))
#define SHBITS(code, nbits) (((int32_t)(code)) >> (32 - (nbits)))

#define TAB1_OFFSET_10BIT 248
#define TAB1_OFFSET_15BIT 360
#define TAB1_OFFSET_16BIT 408
#define TAB1_OFFSET_8BIT_INTRA 432
#define TAB1_OFFSET_10BIT_INTRA 680

#define UNPACK_VLC1(tab_val, run, val, len) \
{ \
  Ipp32u _tab_val = tab_val; \
  run = _tab_val & 0x1f; \
  len = (_tab_val >> 5) & 0xf; \
  val = (_tab_val >> 9); \
}

#define UNPACK_VLC2(tab_val, run, val, len) \
{ \
  Ipp32u _tab_val = tab_val; \
  run = _tab_val & 0x1f; \
  len = (_tab_val >> 5) & 0xf; \
  val = (_tab_val >> 10); \
}

#define APPLY_SIGN(val, sign)  \
  val += sign;                 \
  if (val > 2047) val = 2047; /* with saturation */ \
  val ^= sign

#define SAT(val) \
  if (val > 2047) val = 2047;   \
  if (val < -2048) val = -2048;

const uint8_t MPEG2_DCSIZE_TAB[128] = {
  0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x30, 0x30, 0x30, 0x30, 0x33, 0x33, 0x33, 0x33,
  0x34, 0x34, 0x34, 0x34, 0x45, 0x45, 0x56, 0x00,
  0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67,
  0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x89, 0x89, 0x89, 0x89, 0x9a, 0x9a, 0x9b, 0x9b,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x55, 0x00,
  0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
  0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
  0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
  0x88, 0x88, 0x88, 0x88, 0x99, 0x99, 0xaa, 0xbb
};

const uint16_t MPEG2_VLC_TAB1[] =
{
  0xae2, 0xae2,
  0x6e9, 0x6e9,
  0x12e0, 0x12e0,
  0x6e8, 0x6e8,
  0x6c7, 0x6c7, 0x6c7, 0x6c7,
  0x6c6, 0x6c6, 0x6c6, 0x6c6,
  0xac1, 0xac1, 0xac1, 0xac1,
  0x6c5, 0x6c5, 0x6c5, 0x6c5,
  0x70d,
  0x1b00,
  0x70c,
  0x70b,
  0xb03,
  0xf01,
  0x1700,
  0x70a,
  0xea0, 0xea0, 0xea0, 0xea0, 0xea0, 0xea0, 0xea0, 0xea0, // 0010 10xx
  0x6a4, 0x6a4, 0x6a4, 0x6a4, 0x6a4, 0x6a4, 0x6a4, 0x6a4, // 0011 00xx
  0x6a3, 0x6a3, 0x6a3, 0x6a3, 0x6a3, 0x6a3, 0x6a3, 0x6a3, // 0011 10xx
  0xa80, 0xa80, 0xa80, 0xa80, 0xa80, 0xa80, 0xa80, 0xa80,
  0xa80, 0xa80, 0xa80, 0xa80, 0xa80, 0xa80, 0xa80, 0xa80, // 0100 0xxx
  0x682, 0x682, 0x682, 0x682, 0x682, 0x682, 0x682, 0x682,
  0x682, 0x682, 0x682, 0x682, 0x682, 0x682, 0x682, 0x682, // 0101 0xxx
  0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661, 0x661, // 0110 xxxx
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, 0x640, // 264

  /* 13-bit code */
  0xbaa, 0xba9, 0xfa5, 0x13a3,
  0x17a2, 0x1fa1, 0x1ba1, 0x3fa0,
  0x3ba0, 0x37a0, 0x33a0, 0x7ba,
  0x7b9, 0x7b8, 0x7b7, 0x7b6,
  0x2f80, 0x2f80, 0xb88, 0xb88,
  0xf84, 0xf84, 0x2b80, 0x2b80,
  0x1382, 0x1382, 0xb87, 0xb87,
  0x795, 0x795, 0x794, 0x794,
  0x2780, 0x2780, 0x793, 0x793,
  0x792, 0x792, 0x1781, 0x1781,
  0xf83, 0xf83, 0x2380, 0x2380,
  0xb86, 0xb86, 0x791, 0x791, // 312

  /* 10-bit code  */
  0x750, 0x750, 0x750, 0x750, 0x750, 0x750, 0x750, 0x750,
  0xb45, 0xb45, 0xb45, 0xb45, 0xb45, 0xb45, 0xb45, 0xb45,
  0x1f40, 0x1f40, 0x1f40, 0x1f40, 0x1f40, 0x1f40, 0x1f40, 0x1f40,
  0xf42, 0xf42, 0xf42, 0xf42, 0xf42, 0xf42, 0xf42, 0xf42,
  0x1341, 0x1341, 0x1341, 0x1341, 0x1341, 0x1341, 0x1341, 0x1341,
  0x74f, 0x74f, 0x74f, 0x74f, 0x74f, 0x74f, 0x74f, 0x74f,
  0x74e, 0x74e, 0x74e, 0x74e, 0x74e, 0x74e, 0x74e, 0x74e,
  0xb44, 0xb44, 0xb44, 0xb44, 0xb44, 0xb44, 0xb44, 0xb44, // 376

  /* 15-bit code */
  0xa3e0, 0x9fe0, 0x9be0, 0x97e0,
  0x93e0, 0x8fe0, 0x8be0, 0x87e0,
  0x83e0, 0x3be1, 0x37e1, 0x33e1,
  0x2fe1, 0x2be1, 0x27e1, 0x23e1,
  0x7fc0, 0x7fc0, 0x7bc0, 0x7bc0,
  0x77c0, 0x77c0, 0x73c0, 0x73c0,
  0x6fc0, 0x6fc0, 0x6bc0, 0x6bc0,
  0x67c0, 0x67c0, 0x63c0, 0x63c0,
  0x5fc0, 0x5fc0, 0x5bc0, 0x5bc0,
  0x57c0, 0x57c0, 0x53c0, 0x53c0,
  0x4fc0, 0x4fc0, 0x4bc0, 0x4bc0,
  0x47c0, 0x47c0, 0x43c0, 0x43c0, // 424
  /* 16-bit code */
  0x4a01, 0x4601, 0x4201, 0x3e01,
  0xe06, 0xa10, 0xa0f, 0xa0e,
  0xa0d, 0xa0c, 0xa0b, 0x61f,
  0x61e, 0x61d, 0x61c, 0x61b, // 440

  /* 8-bit code for B15 */
  0x6e7, 0x6e7, 0x6e8, 0x6e8,
  0x6e6, 0x6e6, 0xae2, 0xae2,
  0x1ec0, 0x1ec0, 0x1ec0, 0x1ec0,
  0x1ac0, 0x1ac0, 0x1ac0, 0x1ac0,
  0x6c4, 0x6c4, 0x6c4, 0x6c4,
  0x6c5, 0x6c5, 0x6c5, 0x6c5,
  0x1701, 0x70b, 0x2f00, 0x2b00,
  0x70d, 0x70c, 0xb03, 0x1301,
  0x6a2, 0x6a2, 0x6a2, 0x6a2,
  0x6a2, 0x6a2, 0x6a2, 0x6a2,
  0xaa1, 0xaa1, 0xaa1, 0xaa1,
  0xaa1, 0xaa1, 0xaa1, 0xaa1,
  0x6a3, 0x6a3, 0x6a3, 0x6a3,
  0x6a3, 0x6a3, 0x6a3, 0x6a3,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0x661, 0x661, 0x661, 0x661,
  0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0xffff,
  0xe80, 0xe80, 0xe80, 0xe80,
  0xe80, 0xe80, 0xe80, 0xe80,
  0xe80, 0xe80, 0xe80, 0xe80,
  0xe80, 0xe80, 0xe80, 0xe80,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0x640, 0x640, 0x640, 0x640,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0xa60, 0xa60, 0xa60, 0xa60,
  0x12a0, 0x12a0, 0x12a0, 0x12a0,
  0x12a0, 0x12a0, 0x12a0, 0x12a0,
  0x16a0, 0x16a0, 0x16a0, 0x16a0,
  0x16a0, 0x16a0, 0x16a0, 0x16a0,
  0x6e9, 0x6e9, 0xee1, 0xee1,
  0x6ea, 0x6ea, 0x22e0, 0x22e0,
  0x26e0, 0x26e0, 0x3300, 0x3700,
  0xf02, 0xb04, 0x3b00, 0x3f00, // 688
  /* 10-bit code for B15 */
  0xb25, 0xb25, 0x72e, 0x72e,
  0x1342, 0x750, 0x72f, 0x72f,
};



int CSliceDecoder::GolombReadUE(CBitStream* pBitStream)
{
    int i = 0;

    while(pBitStream->getBitsValue(1) == 0 && i < 32 )
    {
        i++;
    }
    return( ( 1 << i) - 1 + pBitStream->getBitsValue(i));
}

int CSliceDecoder::GolombReadSE(CBitStream* pBitStream)
{
    int val = GolombReadUE(pBitStream);

    return val&0x01 ? (val+1)/2 : -(val/2);
}

int CSliceDecoder::GolombReadTE(CBitStream* pBitStream, int x)
{
    if( x == 1 )
    {
        return 1 - pBitStream->getBitsValue(1);
    }
    else if( x > 1 )
    {
        return GolombReadUE(pBitStream);
    }
    return 0;
}


// Factory method to Create a specific decoder
CSliceDecoder * CSliceDecoder::CreateSliceDecoder(eEncodeFormat aEncodeFormat) {
    if (aEncodeFormat == eH264) {
        return new CH264SliceDecoder();
    }
    else if (aEncodeFormat == eSVC) {
        return new CSVCSliceDecoder();
    }
    else {
        return new CSliceDecoder(aEncodeFormat);
    }
}

CSliceDecoder::CSliceDecoder(eEncodeFormat aEncodeFormat)
{
    m_pBitStream = new CBitStream();
    m_nScaleFactor = 1;
    m_nSliceNumber = 0;
    m_iclp = NULL;
    InitIdct();


    size_t nSize = V2D_BLOCK_SIZE * V2D_MAX_SLICE_SIZE;
#ifdef USE_IPP
    m_scan_matrix = ippsMalloc_8u(64) ;
    m_psrcoeffY = ippsMalloc_16s(nSize * sizeof(Ipp16s));
    nSize /= 2;
    m_psrcoeffU = ippsMalloc_16s(nSize * sizeof(Ipp16s));
    m_psrcoeffV = ippsMalloc_16s(nSize * sizeof(Ipp16s));

#else
    m_scan_matrix = new uint8_t [64];

    nSize = V2D_BLOCK_SIZE * V2D_MAX_SLICE_SIZE;
    m_psrcoeffY = new short[nSize];
    ::memset(m_psrcoeffY, 0x00, nSize * sizeof(short));

    nSize /= 2;
    m_psrcoeffU = new short[nSize];
    ::memset(m_psrcoeffU, 0x00, nSize * sizeof(short));

    m_psrcoeffV = new short[nSize];
    ::memset(m_psrcoeffV, 0x00, nSize * sizeof(short));
#endif

    // Setup the scan matrix
    for(unsigned int i = 0; i < 8; i++) {
        for(unsigned int j = 0; j < 8; j++) {
            m_scan_matrix[8 * i + j] = scan[8 * i + j];
        }
    }


#ifdef USE_IPP
    m_blockRoi.width = 8;
    m_blockRoi.height = 8;
#endif

#ifdef USE_IPP_5DOT3
    m_blockRoi.width = 8;
    m_blockRoi.height = 8;
#endif

    m_nBlocksDecoded = 0;
}

CSliceDecoder::~CSliceDecoder()
{
    delete      m_pBitStream;
#ifdef USE_IPP
    ippsFree(m_scan_matrix);
    ippsFree(m_psrcoeffY);
    ippsFree(m_psrcoeffU);
    ippsFree(m_psrcoeffV);
#else
    delete []   m_scan_matrix;
    delete []   m_psrcoeffY;
    delete []	m_psrcoeffU;
    delete []	m_psrcoeffV;
#endif

#ifdef COLLECT_STATS
    printf("Blocks Decoded = %u\n", m_nBlocksDecoded);
#endif


}


void  CSliceDecoder::SetScaleFactor(int a_nScaleFactor)
{
    if (!(a_nScaleFactor == 8 ||
            a_nScaleFactor == 4 ||
            a_nScaleFactor == 2 ||
            a_nScaleFactor == 1))
        return;

    m_nScaleFactor = a_nScaleFactor;
}


bool CSliceDecoder::Decode(uint8_t * a_pData, size_t a_nDataLen,
                          size_t a_nSliceSize,
                          uint8_t * a_pY,
                          uint8_t * a_pU,
                          uint8_t * a_pV,
                          int &a_rnSliceNum)
{
    m_pBitStream->init(a_pData, a_nDataLen);

    int iMQuant = 0, iDCQuant = 0;
    m_bIsRefreshSlice = false;
    if (!DecodeHeaders(iMQuant, iDCQuant))
        return false;

    // Caller needs the slice number
    a_rnSliceNum = m_nSliceNumber;

    uint8_t * pYUV[3] = {a_pY, a_pU, a_pV};
    if (!DecodeData(pYUV, iMQuant, iDCQuant, a_nSliceSize))
        return false;

    return true;
}

bool CSliceDecoder::DecodeHeader(uint8_t * a_pData, size_t a_nDataLen,
        int & a_rnSliceNum, bool & a_bIsRefreshSlice)
{
    m_pBitStream->init(a_pData, a_nDataLen);

    int iMQuant = 0, iDCQuant = 0;
    m_bIsRefreshSlice = false;
    if (!DecodeHeaders(iMQuant, iDCQuant))
        return false;

    // Caller needs the slice number
    a_rnSliceNum = m_nSliceNumber;
    a_bIsRefreshSlice = m_bIsRefreshSlice;

    return true;
}

bool CSliceDecoder::DecodeHeaders(int & iMQuant, int & iDCQuant) {

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

    return true;
}

bool CSliceDecoder::DecodeData(uint8_t * pYUV[3], int iMQuant, int iDCQuant, int a_nSliceSize) {

    int yuvStrides[3] = { a_nSliceSize * 8, a_nSliceSize * 4,
        a_nSliceSize * 4 };

    if (!DecodeSlice(iMQuant, iDCQuant, a_nSliceSize, pYUV, yuvStrides)) {
        PRINTF("Unable to decode slice\n");
        return false;
    }
    return true;
}


void CSliceDecoder::InitIdct()
{
    m_iclp = m_iclip + 512;
    for (int i = -512; i < 512; i++)
        m_iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
}

void CSliceDecoder::IDct(short* a_psBlock)
{
    for (int i=0; i < 8; i++)
        IDctRow(a_psBlock + 8*i);

    for (int i = 0; i < 8; i++)
        IDctCol(a_psBlock + i);
}

void CSliceDecoder::IDctRow(short* a_psBlock)
{
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;

    /* shortcut */
    if (!((x1 = a_psBlock[4]<<11) | (x2 = a_psBlock[6]) | (x3 = a_psBlock[2]) |
        (x4 = a_psBlock[1]) | (x5 = a_psBlock[7]) | (x6 = a_psBlock[5]) | (x7 = a_psBlock[3])))
    {
        a_psBlock[0]=a_psBlock[1]=a_psBlock[2]=a_psBlock[3]=a_psBlock[4]=a_psBlock[5]=a_psBlock[6]=a_psBlock[7]=a_psBlock[0]<<3;
        return;
    }

    x0 = (a_psBlock[0]<<11) + 128; /* for proper rounding in the fourth stage */

    /* first stage */
    x8 = W7*(x4+x5);
    x4 = x8 + (W1-W7)*x4;
    x5 = x8 - (W1+W7)*x5;
    x8 = W3*(x6+x7);
    x6 = x8 - (W3-W5)*x6;
    x7 = x8 - (W3+W5)*x7;

    /* second stage */
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6*(x3+x2);
    x2 = x1 - (W2+W6)*x2;
    x3 = x1 + (W2-W6)*x3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;

    /* third stage */
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181*(x4+x5)+128)>>8;
    x4 = (181*(x4-x5)+128)>>8;

    /* fourth stage */
    a_psBlock[0] = (x7+x1)>>8;
    a_psBlock[1] = (x3+x2)>>8;
    a_psBlock[2] = (x0+x4)>>8;
    a_psBlock[3] = (x8+x6)>>8;
    a_psBlock[4] = (x8-x6)>>8;
    a_psBlock[5] = (x0-x4)>>8;
    a_psBlock[6] = (x3-x2)>>8;
    a_psBlock[7] = (x7-x1)>>8;
}

/* column (vertical) IDCT
*
*             7                         pi         1
* dst[8*k] = sum c[l] * src[8*l] * cos( -- * ( k + - ) * l )
*            l=0                        8          2
*
* where: c[0]    = 1/1024
*        c[1..7] = (1/1024)*sqrt(2)
*/
void CSliceDecoder::IDctCol(short* a_psBlock)
{
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;

    /* shortcut */
    if (!((x1 = (a_psBlock[8*4]<<8)) | (x2 = a_psBlock[8*6]) | (x3 = a_psBlock[8*2]) |
        (x4 = a_psBlock[8*1]) | (x5 = a_psBlock[8*7]) | (x6 = a_psBlock[8*5]) | (x7 = a_psBlock[8*3])))
    {
        a_psBlock[8*0]=a_psBlock[8*1]=a_psBlock[8*2]=a_psBlock[8*3]=a_psBlock[8*4]=a_psBlock[8*5]=a_psBlock[8*6]=a_psBlock[8*7]=
            m_iclp[(a_psBlock[8*0]+32)>>6];
        return;
    }

    x0 = (a_psBlock[8*0]<<8) + 8192;

    /* first stage */
    x8 = W7*(x4+x5) + 4;
    x4 = (x8+(W1-W7)*x4)>>3;
    x5 = (x8-(W1+W7)*x5)>>3;
    x8 = W3*(x6+x7) + 4;
    x6 = (x8-(W3-W5)*x6)>>3;
    x7 = (x8-(W3+W5)*x7)>>3;

    /* second stage */
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6*(x3+x2) + 4;
    x2 = (x1-(W2+W6)*x2)>>3;
    x3 = (x1+(W2-W6)*x3)>>3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;

    /* third stage */
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181*(x4+x5)+128)>>8;
    x4 = (181*(x4-x5)+128)>>8;

    /* fourth stage */

    a_psBlock[8*0] = m_iclp[_CLIP(((x7+x1)>>14),-128,127)];
    a_psBlock[8*1] = m_iclp[_CLIP(((x3+x2)>>14),-128,127)];
    a_psBlock[8*2] = m_iclp[_CLIP(((x0+x4)>>14),-128,127)];
    a_psBlock[8*3] = m_iclp[_CLIP(((x8+x6)>>14),-128,127)];
    a_psBlock[8*4] = m_iclp[_CLIP(((x8-x6)>>14),-128,127)];
    a_psBlock[8*5] = m_iclp[_CLIP(((x0-x4)>>14),-128,127)];
    a_psBlock[8*6] = m_iclp[_CLIP(((x3-x2)>>14),-128,127)];
    a_psBlock[8*7] = m_iclp[_CLIP(((x7-x1)>>14),-128,127)];
}

bool CSliceDecoder::IsSliceZero()
{
    unsigned int nCode;
    m_pBitStream->showBits(32, nCode);

    return ((nCode == V2D_VIDEO_START_R_FRAME_CODE) ||
        (nCode == V2D_VIDEO_START_L_FRAME_CODE));
}

bool CSliceDecoder::ParseStartFrameCode(int& a_mQuant,
                                       int& a_dcQuant)
{
    if (m_pBitStream->getRemainingBytes() < 5)
        return false;

    unsigned int value;
    m_pBitStream->getBits(32, value);

    GetQuantBits(a_mQuant, a_dcQuant);
    m_nSliceNumber = 0;
    return true;
}


bool CSliceDecoder::ParseStartSliceCode(int& a_mQuant,
                                       int& a_dcQuant)
{
    if (m_pBitStream->getRemainingBytes() < 6)
        return false;

    unsigned int val1 = 0, val2 = 0, val3 = 0, firstByte = 0;
    unsigned int value;

    m_pBitStream->getBits(8, val1);
    m_pBitStream->getBits(8, val2);
    m_pBitStream->getBits(8, val3);

    value = ((firstByte << 24) | (val1 << 16) | (val2 << 8) | val3);

    if (value != V2D_VIDEO_START_SLICE_CODE) {
        if (value != V2D_VIDEO_START_CHROMA_SLICE_CODE) { // For dynamic 444 case
            return false;
        }
    }

    // 11 bit slice number
    unsigned int curSliceNo = 0;
    m_pBitStream->getBits(11, curSliceNo);

    // 1 bit unused
    unsigned int i_frame_d1  = 0;
    m_pBitStream->getBits(1, i_frame_d1);

    // 1 bit refresh slice
    m_pBitStream->getBits(1, value);
    m_bIsRefreshSlice = (value == 1);

    GetQuantBits(a_mQuant, a_dcQuant);

    m_nSliceNumber = curSliceNo;

    return true;
}

bool CSliceDecoder::GetQuantBits(int& a_mQuant, int& a_dcQuant)
{
    unsigned int tmp1;
    m_pBitStream->getBits(3, tmp1);
    a_mQuant = 1 << tmp1;

    m_pBitStream->getBits(3, tmp1);
    a_dcQuant = 1 << tmp1;
    return true;
}

bool CSliceDecoder::DecodeSlice(const int a_nMQuant,
                                const int a_nDCQuant,
                                const size_t a_nSliceSize,
                                uint8_t * a_pYUV[3],
                                int a_yuvStrides[3])
{
    int   dcDctPred[3] = {0, 0, 0};

    for (size_t nBlkNo = 0; nBlkNo < a_nSliceSize / 2; nBlkNo++) {
        if (!DecodeBlock(&m_psrcoeffY[(2 * nBlkNo) * V2D_BLOCK_SIZE],
                &dcDctPred[0], a_nMQuant, a_nDCQuant, 0,
                a_pYUV[0] + 2 * nBlkNo * 8/m_nScaleFactor,
                a_yuvStrides[0]))
            goto decodeslice_fail;


        if (!DecodeBlock(&m_psrcoeffU[nBlkNo * V2D_BLOCK_SIZE],
                &dcDctPred[1], a_nMQuant, a_nDCQuant, 4,
                a_pYUV[1] + nBlkNo * 8/m_nScaleFactor,
                a_yuvStrides[1]))
            goto decodeslice_fail;

        if (!DecodeBlock(&m_psrcoeffY[(2 * nBlkNo + 1) * V2D_BLOCK_SIZE],
                &dcDctPred[0], a_nMQuant, a_nDCQuant, 0,
                a_pYUV[0] + (2 * nBlkNo + 1) * 8/m_nScaleFactor,
                a_yuvStrides[0]))
            goto decodeslice_fail;


        if (!DecodeBlock(&m_psrcoeffV[nBlkNo * V2D_BLOCK_SIZE],
                &dcDctPred[2], a_nMQuant, a_nDCQuant, 5,
                a_pYUV[2] + nBlkNo * 8/m_nScaleFactor,
                a_yuvStrides[2]))
            goto decodeslice_fail;

    } //for

    return true;

    decodeslice_fail: return false;
}


#ifndef USE_IPP
bool CSliceDecoder::DecodeBlock(short* a_block,
                                int* a_dcDctPred,
                                const int a_mquant,
                                const int a_dcquant,
                                const int a_comp,
                                uint8_t *pDest,
                                const int aStride)
{
    int nc, val;
    unsigned int i, code, run, val2, sign;
    const DCTtab *tab;

    if (a_comp == 0)
        m_nBlocksDecoded++;

    memset(a_block, 0x00, sizeof(short) * 64);

    if (m_pBitStream->getRemainingBytes() <= 0)
        return false;

    // Get DC coefficients
    GetDcDctDiff(val);

    val += *a_dcDctPred;
    *a_dcDctPred = val;
    a_block[0] = val * a_dcquant;

    // Get AC coefficients
    nc = 0;
    PRINTF("val[0] = %d\n", a_block[0]);

    for (i = 1;; i++) {
        m_pBitStream->showBits(16, code);
        PRINTF("Loop index = %d\n", i);

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
            PRINTF("Aborting\n");
            return true;
        }

        m_pBitStream->flushBits(tab->len);

        if (tab->run == 64) /* end_of_block */
        {
            break;
        }

        if (tab->run == 65) /* escape */
        {
            m_pBitStream->getBits(6, run);

            i += run;

            m_pBitStream->getBits(12, val2);

            val = val2;

            if ((val & 2047) == 0) {
                break;
            }
            if ((sign = (val >= 2048)))
                val = 4096 - val;
        }
        else {
            i += run = tab->run;
            val = tab->level;
            m_pBitStream->getBits(1, sign);
        }
        if (i >= 64) {
            break;
        }

        // Un zig-zag
        unsigned int j = m_scan_matrix[i];

        // val = (val * a_mquant * a_quantmat[j]) / 16;
        val = ((val * a_mquant) << quant_mat_mult[j]) >> 4;
        a_block[j] = sign ? -val : val;
        //PRINTF("val[%d] = %d\n", j, a_block[j]);

        nc++;
    }

    if (m_nScaleFactor == 1) {
#ifdef USE_IPP_5DOT3
        ippiDCT8x8InvLSClip_16s8u_C1R(a_block, pDest, aStride, 128, 0, 255);
        ippiTranspose_8u_C1IR(pDest, aStride, m_blockRoi);
#else
        IDct(a_block);
        add128AndPutTransposedBlock (a_block, pDest, aStride);
#endif
    }
    else if (m_nScaleFactor == 2) {
        CFDCT::IDCT4x4_Add128_Transpose(a_block, pDest, aStride >> 1);
    }
    else if (m_nScaleFactor == 4) {
        CFDCT::IDCT2x2_Add128_Transpose(a_block, pDest, aStride >> 2);
    }
    else if (m_nScaleFactor == 8) {
        // 1x1 IDCT is trivial, just divide DC coeff by 8
        *(pDest) = _S((a_block[0] >> 3) + 128);
    }

    return true;
}

#else

//#define BREAK_AT    24
//#define BREAK_AT    64
#define BREAK_AT    4

bool CSliceDecoder::DecodeBlock(short* a_block,
                                int* a_dcDctPred,
                                const int a_mquant,
                                const int a_dcquant,
                                const int a_comp,
                                uint8_t *pDest,
                                const int aStride)
{
    uint32_t code, tbl;
    int32_t val, i, j, run, len, sign;
    //int done = 0;

    GetDcDctDiff(val);
    val += *a_dcDctPred;
    *a_dcDctPred = (Ipp16s)val;

    m_pBitStream->showHi9Bits(code);

    ippsZero_16s(a_block, 64);
    a_block[0] = val * a_dcquant;

    //PRINTF("val[0] = %d\n", a_block[0]);

    i = 0;
    for (;;) {
        if ((code & 0xf0000000) == 0x60000000) {
            break;
        } else if (code >= 0x08000000) {
            tbl = MPEG2_VLC_TAB1[TAB1_OFFSET_8BIT_INTRA + UHBITS(code, 8)];
        } else if (code >= 0x04000000) {
            m_pBitStream->expand17Bits(code);
            i += 1 + UHBITS (code << 6, 6);
            i &= 63;

            // zig-zag
            j = m_scan_matrix[i];
//            if (i >= BREAK_AT) {
//                done = 1;
//            }

            m_pBitStream->expand25Bits(code);
            code <<= 12;
            val = SHBITS(code, 12);
            val = ((val * a_mquant) << quant_mat_mult[j]) >> 4;
            SAT(val);
            a_block[j] = (int16_t)val;

            //PRINTF("val[%d] = %d\n", j, a_block[j]);
            m_pBitStream->flushBits(24);
            m_pBitStream->showHi9Bits(code);
            continue;
        } else {
            m_pBitStream->expand17Bits(code);

            if (code >= 0x02000000) {
                tbl = MPEG2_VLC_TAB1[TAB1_OFFSET_10BIT_INTRA + UHBITS(code, 10)];
            } else if (code >= 0x00800000) {
                tbl = MPEG2_VLC_TAB1[TAB1_OFFSET_10BIT + UHBITS(code, 13)];
            } else if (code >= 0x00200000) {
                tbl = MPEG2_VLC_TAB1[TAB1_OFFSET_15BIT + UHBITS(code, 15)];
            } else {
                tbl = MPEG2_VLC_TAB1[TAB1_OFFSET_16BIT + UHBITS(code, 16)];
                if (code < 16) {
                    return false;
                }
                code <<= 16;
                m_pBitStream->flushBits(16);
            }
        }

//        if (done) {
//            len = (tbl >> 5) & 0xf;
//            m_pBitStream->flushBits(len + 1);
//            m_pBitStream->showHi9Bits(code);
//            continue;
//        }
        i++;
        UNPACK_VLC2(tbl, run, val, len)

        i += run;
        i &= 63;
        j = m_scan_matrix[i];


        val = ((val * a_mquant) << quant_mat_mult[j]) >> 4;
        sign = SHBITS(code << len, 1);
        APPLY_SIGN(val, sign);
        m_pBitStream->flushBits(len + 1);
        a_block[j] = (int16_t)val;
//        if (i >= BREAK_AT) {
//            done = 1;
//        }

        //PRINTF("val[%d] = %d\n", j, a_block[j]);
        m_pBitStream->showHi9Bits(code);
    }

    m_pBitStream->flushBits(4);

    //V2dLibImageManip::dumpBlock(a_block, "A block");

#if defined(USE_IPP) && !defined(PASS_TESTS)
    if (m_nScaleFactor == 1) {
        ippiDCT8x8InvLSClip_16s8u_C1R(a_block, pDest, aStride, 128, 0, 255);
        ippiTranspose_8u_C1IR(pDest, aStride, m_blockRoi);
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

    //ippiDCT8x8Inv_16s_C1I(a_block);
#else
    IDct(a_block);
    add128AndPutTransposedBlock (a_block, pDest, aStride);
#endif


    return true;
}
#endif


#ifndef USE_IPP
// Get Luma/Chroma DCT Diff
bool CSliceDecoder::GetDcDctDiff(int &a_rnDctDiff)
{
    unsigned int code, size;

    /* decode length */
    m_pBitStream->showBits(5, code);

    if (code < 31) {
        size = DClumtabHW0[code].val;
        m_pBitStream->flushBits(DClumtabHW0[code].len);
    }
    else {
        m_pBitStream->showBits(9, code);

        code -= 0x1f0;
        size = DClumtabHW1[code].val;
        m_pBitStream->flushBits(DClumtabHW1[code].len);
    }

    if (size == 0) {
        a_rnDctDiff = 0;
    }
    else {
        m_pBitStream->getBits(size, code);
        a_rnDctDiff = code;
        if ((a_rnDctDiff & (1 << (size - 1))) == 0)
            a_rnDctDiff -= (1 << size) - 1;
    }
    return true;
}

#else

bool CSliceDecoder::GetDcDctDiff(int32_t &a_rnDctDiff)
{
    uint32_t code, tbl;
    int32_t len;

    const uint8_t *pTab;
    int32_t dct_dc_size;
    m_pBitStream->showHi9Bits(code);
    pTab = MPEG2_DCSIZE_TAB + (IPPVC_BLOCK_LUMA << 6);
    if (code < 0xf8000000) {
        tbl = pTab[UHBITS(code, 5)];
        dct_dc_size = tbl & 0xF;
        len = tbl >> 4;
        if (dct_dc_size) {
            m_pBitStream->expand17Bits(code);
            code <<= len;
            a_rnDctDiff = UHBITS(code, dct_dc_size) - UHBITS(SHBITS(~code, 1), dct_dc_size);
            m_pBitStream->flushBits(len + dct_dc_size);
        } else {
            a_rnDctDiff = 0;
            m_pBitStream->flushBits(len);
        }
    } else {
        tbl = pTab[32 + (UHBITS(code, 10) & 0x1f)];
        dct_dc_size = tbl & 0xF;
        len = tbl >> 4;
        m_pBitStream->expand17Bits(code);
        m_pBitStream->expand25Bits(code);
        //EXPAND_25BITS(BS, code);
        code <<= len;
        a_rnDctDiff = UHBITS(code, dct_dc_size) - UHBITS(SHBITS(~code, 1), dct_dc_size);
        m_pBitStream->flushBits(len + dct_dc_size);
    }

    return false;
}
#endif

void CSliceDecoder::Add128AndSaturateto8BitSlice(const size_t a_nSliceSize,
        uint8_t * a_pY,
        uint8_t * a_pU,
        uint8_t * a_pV)
{
    short * cY = m_psrcoeffY;
    short * cU = m_psrcoeffU;
    short * cV = m_psrcoeffV;

    for (size_t nBlkNo = 0; nBlkNo < a_nSliceSize / 2; nBlkNo++) {
#ifdef USE_IPP
        ippiAdd128_JPEG_16s8u_C1R(cY, 16, a_pY, 8, m_blockRoi);
        ippiTranspose_8u_C1IR(a_pY, 8, m_blockRoi);
        cY += 64; a_pY += 64;

        ippiAdd128_JPEG_16s8u_C1R(cY, 16, a_pY, 8, m_blockRoi);
        ippiTranspose_8u_C1IR(a_pY, 8, m_blockRoi);
        cY += 64; a_pY += 64;

        ippiAdd128_JPEG_16s8u_C1R(cU, 16, a_pU, 8, m_blockRoi);
        ippiTranspose_8u_C1IR(a_pU, 8, m_blockRoi);
        cU += 64; a_pU += 64;

        ippiAdd128_JPEG_16s8u_C1R(cV, 16, a_pV, 8, m_blockRoi);
        ippiTranspose_8u_C1IR(a_pV, 8, m_blockRoi);
        cV += 64; a_pV += 64;

#else
        transposeAndAdd128AndPutBlock(cY, a_pY);
        cY += 64; a_pY += 64;

        transposeAndAdd128AndPutBlock(cY, a_pY);
        cY += 64; a_pY += 64;

        transposeAndAdd128AndPutBlock(cU, a_pU);
        cU += 64; a_pU += 64;

        transposeAndAdd128AndPutBlock(cV, a_pV);
        cV += 64; a_pV += 64;


#endif
    } //for

}

void CSliceDecoder::CopyToRgbImage(const size_t a_nSliceSize,
        uint8_t * aDest, int aStride)
{
    short * cY = m_psrcoeffY;
    short * cU = m_psrcoeffU;
    short * cV = m_psrcoeffV;
    (void)cY;
    (void)cU;
    (void)cV;

    for (size_t nBlkNo = 0; nBlkNo < a_nSliceSize / 2; nBlkNo++) {
#ifdef USE_IPP
        ippiTranspose_16s_C1IR(cY, 16, m_blockRoi);
        cY += 64;

        ippiTranspose_16s_C1IR(cY, 16, m_blockRoi);
        cY += 64;

        ippiTranspose_16s_C1IR(cU, 16, m_blockRoi);
        cU += 64;

        ippiTranspose_16s_C1IR(cV, 16, m_blockRoi);
        cV += 64;
#endif
    } //for

    for (size_t nBlkNo = 0; nBlkNo < a_nSliceSize / 2; nBlkNo++) {
#ifdef USE_IPP
        const Ipp16s *srcArray[3] = {cY, cU, cV};

        ippiYCbCr422ToBGRLS_MCU_16s8u_P3C3R(srcArray, aDest, aStride);
        cY += 128;
        cU += 64;
        cV += 64;
        aDest += 16 * 3;
#endif
    } //for

}

