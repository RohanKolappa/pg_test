#ifndef __V2D_VIDEOFRAMEDECODER_H__
#define __V2D_VIDEOFRAMEDECODER_H__

#include "codecdefs.h"

#include "slicedecoder.hh"
#include "colortransform.h"
#include <list>
#include <map>

#include <string>


typedef struct v2dDecConfiguration {
    /* set before creating decoder */
    size_t m_nSize;
    size_t m_nWidth;
    size_t m_nHeight;
    size_t m_nSliceSize;
    enum eEncodeFormat m_eEncodeFormat;
    enum ePixelFormat m_eDecodeFormat;

    /* set by the decoder */
    size_t m_nDecodeBufLen;
} v2dDecConfiguration, *v2dDecConfigurationPtr;

#ifdef __cplusplus

const int EDECODER_NOMEMORY        = -1;
const int EDECODER_MALFORMEDFRAME  = -2;
const int EDECODER_MALFORMEDSLICE  = -3;
const int EDECODER_OUTBUFFTOOSMALL = -4;

class CSlice;
class CSliceBuffer;
class CRect;

class CRect
{
public:
    CRect();
    CRect(int x, int y, int w, int h);
    bool IsNull() const;
    int area() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
    void setWidth(int w);

    private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
};

inline CRect::CRect()
: m_x(0), m_y(0), m_w(0), m_h(0)
{}

inline CRect::CRect(int x, int y, int w, int h)
: m_x(x), m_y(y), m_w(w), m_h(h)
{}

inline bool CRect::IsNull() const {
    return (m_w == 0);
}

inline int CRect::area() const {
    return m_w * m_h;
}

inline int CRect::x() const {
    return m_x;
}

inline int CRect::y() const {
    return m_y;
}

inline int CRect::width() const {
    return m_w;
}

inline int CRect::height() const {
    return m_h;
}

inline void CRect::setWidth(int w) {
    m_w = w;
}


class CVideoFrameDecoder
{
public:

   DLLEXPORT static CVideoFrameDecoder* CreateObj(v2dDecConfigurationPtr a_pDecConfig);
    DLLEXPORT static void DeleteObj(CVideoFrameDecoder * a_pFrameDecoder);

    DLLEXPORT ~CVideoFrameDecoder();

   DLLEXPORT int Decode(unsigned char* a_pEncFrame, const size_t a_nEncLen,
        unsigned char* a_pDecFrame, const size_t a_nDecLen);

    DLLEXPORT size_t GetFrameWidth() const;
    DLLEXPORT size_t GetFrameHeight() const;
    DLLEXPORT size_t GetScaledWidth() const;
    DLLEXPORT size_t GetScaledHeight() const;
    DLLEXPORT size_t GetSliceSize() const;
    DLLEXPORT size_t GetDecodeBufLen() const;
    DLLEXPORT size_t GetMaxRectanglesBufferLen() const;
    DLLEXPORT size_t GetRectanglesBuffer(unsigned char *a_DecodeBuffer,
            unsigned char *a_DestBuffer) const;
    DLLEXPORT int GetScaleFactor() const;
    DLLEXPORT void SetScaleFactor(int a_nScaleFactor);
    DLLEXPORT void SetSliceDebug(bool a_bSliceDebug);
    DLLEXPORT uint32_t GetLayerErrors();
    DLLEXPORT bool GetSliceDebug();
    DLLEXPORT void SetDoubleBuffering(bool a_bDoubleBuffer);
    DLLEXPORT bool GetDoubleBuffering();

private:

    CVideoFrameDecoder(v2dDecConfigurationPtr a_pDecConfig);
    int Decode(unsigned char* a_pEncFrame, const size_t a_nEncLen);
	void InitDecoder();
    int DecodeSlicesIntoFrameBuffer();
    bool DecodeSliceAsFrameHeader();
    void CopySliceToFrame(const int a_nSliceNumber, bool a_bIsRefreshSlice);
    void RenderSlicePixelsIntoFrame(CSliceBuffer * a_sliceBuffer, bool a_bIsRefreshSlice);
    void MarkSlicesInFrame();
    void InitRectangles();
    void AddCurrentRectangle();
    void DumpRectangles();
    void CoaleseceRectangles(CSliceBuffer * a_sliceBuffer);
    int BytesPerPixel() const;
    void ComputeScaleParameters();
    void ComputeSlicesPerFrame();
    void CreateSliceBuffers(int aSize);
    void DeleteSliceBuffers();
    void MarkSliceBuffersAsOld();
    void SaveSliceToBuffer(CSliceBuffer * a_sliceBuffer, bool a_bIsRefreshSlice);

    void CheckDimensions(const SVCFrameHeader &aFrameHeader);
    void ParseSlicesInBuffer(std::list<CSlice> &a_rSlices,
        unsigned char* a_pBuffer, const size_t a_nBufLen, const bool isSVC);
    int ParseForChromaSlice(uint8_t * a_pBuffer, size_t a_nBufLen);
    void GetHighLightColors(int & red, int & blue, int & green);
    void HighLightSlice(CSliceBuffer * a_sliceBuffer, bool a_bIsRefreshSlice);
    void UnHighLightSlices();

    CVideoFrameDecoder(const CVideoFrameDecoder& a_rhs);
    const CVideoFrameDecoder& operator=(const CVideoFrameDecoder& a_rhs);

    bool m_bErr;
    std::string m_strError;

    CSliceDecoder *m_pSliceDecoder;
    uint64_t m_nFramesDecoded;

    std::list<CSlice> m_Slices;
    std::list<CRect> m_Rects;
    std::map<int, CSliceBuffer *> m_SliceBuffers;

    CRect m_CurrentRect;

    unsigned char *m_pDecodeBuffer;

    size_t m_nFrameWidth;
    size_t m_nFrameHeight;
    size_t m_nEncodedWidth;
    size_t m_nEncodedHeight;
    size_t m_nEncodedOffsetX;
    size_t m_nEncodedOffsetY;
    size_t m_nScaledWidth;
    size_t m_nScaledHeight;
    size_t LayerErrorCount;
    int m_nScaleFactor;

    size_t m_nSliceSize;
    size_t m_nSliceWidth;
    size_t m_nSliceHeight;
    size_t m_nDecodeBufLen;
    int m_nSlicesPerFrame;

    int m_nLastSliceNum;
    unsigned int m_LastYPos;
    bool m_bSliceDebug;
    bool m_bDisableSliceDebug;
    bool m_bDoubleBuffer;

    eEncodeFormat m_eEncodeFormat;
    ePixelFormat m_eDecodeFormat;
    V2dLibColorTransform::YUVToRGBConverter m_oRGB;

    enum eToken {
        ETOKEN_NONE,
        ETOKEN_START_LEFT_FRAME,
        ETOKEN_START_RIGHT_FRAME,
        ETOKEN_START_SLICE
    };

};

class CSlice
{
public:
    CSlice();
    CSlice(unsigned char* a_pData, size_t a_nLen);
    unsigned char* m_pData;
    size_t         m_nLen;
};

inline CSlice::CSlice()
: m_pData(0)
, m_nLen(0)
{}

inline CSlice::CSlice(unsigned char* a_pData, size_t a_nLen)
: m_pData(a_pData)
, m_nLen(a_nLen)
{
}


#endif

#endif //__V2D_VIDEOFRAMEDECODER_H__

