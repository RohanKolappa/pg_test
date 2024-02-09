/*
 * slicebuffer.hh
 *
 *  Created on: Oct 24, 2011
 *      Author: rkale
 */

#ifndef SLICEBUFFER_HH_
#define SLICEBUFFER_HH_

#include "colortransform.h"

class CSliceBuffer
{
public:
    CSliceBuffer(int aSize, V2dLibColorTransform::YUVToRGBConverter & a_oRGB);
    ~CSliceBuffer();
    void ComputeFramePositioningInfo(int a_nSliceNum,
            eEncodeFormat encodeFormat, ePixelFormat pixelFormat,
            int a_nSliceWidth, int a_nSliceHeight,
            int a_nEncodedWidth, int a_nEncodedHeight,
            int a_nFrameWidth, int a_nFrameHeight,
            int a_nSliceSize, int a_nScaleFactor);

    void SaveSliceData(bool a_bIsRefreshSlice);
    void RenderSliceIntoFrame(uint8_t * a_pDecodeBuffer, bool a_bIsRefreshSlice);
    void HighLightSlice(uint8_t * a_pDecodeBuffer, eEncodeFormat encodeFormat,
            int red, int green, int blue);
    void UnHighLightSlice(uint8_t * a_pDecodeBuffer);

    uint8_t * GetYOffset(bool a_bIsRefreshSlice);
    uint8_t * GetUOffset(bool a_bIsRefreshSlice);
    uint8_t * GetVOffset(bool a_bIsRefreshSlice);
    uint8_t * GetExtendedYOffset(bool a_bIsRefreshSlice);
    uint8_t * GetExtendedUOffset(bool a_bIsRefreshSlice);
    uint8_t * GetExtendedVOffset(bool a_bIsRefreshSlice);

    int GetSize() { return m_nSize; };
    int GetSliceNumber();
    int GetBitmapOffset() { return m_nBitmapOffset; };
    int GetSlicePixelWidth() { return m_nDestSlicePixelWidth; };
    int GetSlicePixelHeight() { return m_nDestSlicePixelHeight; };
    int GetSplitBlocks() { return m_nSplitBlocks; };
    int GetBlocksPerRow();
    void SetDirty(bool bDirty);
    bool GetDirty();
    void SetNew(bool bNew);
    bool GetNew();

private:
    void ComputeScaleParameters(
            eEncodeFormat encodeFormat,
            int a_nSliceWidth, int a_nSliceHeight,
            int a_nSliceSize, int a_nScaleFactor);
    void SetUp444Mode(eEncodeFormat encodeFormat);
    void ComputeRenderingParameters(ePixelFormat pixelFormat);
    void RenderSliceIntoYUVFrame(uint8_t * a_pDecodeBuffer);
    void RenderSliceIntoRGBFrame(uint8_t * a_pDecodeBuffer);
    void ConvertYUVToRGB(
            const unsigned char *a_slicePlanes[3],
            int a_sourceStride[],
            unsigned int a_width,
            unsigned int a_height,
            unsigned char *a_destBuffer,
            int a_destStride,
            V2dLibColorTransform::YUVToRGBConverter::ImageFormat a_eImageFormat);
    bool HasOdd444Content(bool a_bIsRefreshSlice);
    bool HasEven444Content(bool a_bIsRefreshSlice);

    uint8_t * m_pY;
    uint8_t * m_pU;
    uint8_t * m_pV;
    bool m_bIs444;
    bool m_bIsDynamic444;
    bool m_bDirty;
    bool m_bNew;
    bool m_bNeeds444Conversion;
    int m_nSliceNumber;
    int m_nSize;
    int m_nSliceSize;
    int m_nFrameWidth;
    int m_nFrameHeight;
    int m_nScaledWidth;
    int m_nScaledHeight;
    int m_nBitmapOffset;
    int m_nSlicePixelWidth;
    int m_nSlicePixelHeight;
    int m_nDestSlicePixelWidth;
    int m_nDestSlicePixelHeight;
    int m_nSplitBlocks;
    int m_nBlocksPerRow;
    int m_sliceStrides[3];
    int m_nBytesPerPixel;
    ePixelFormat m_pixelFormat;
    V2dLibColorTransform::YUVToRGBConverter::ImageFormat m_eImageFormat;
    V2dLibColorTransform::YUVToRGBConverter & m_oRGB;
#ifdef USE_IPP
    IppiSize m_sliceRoi;
#endif
};

int inline CSliceBuffer::GetSliceNumber()
{
    return m_nSliceNumber;
}

int inline CSliceBuffer::GetBlocksPerRow()
{
    return m_nBlocksPerRow;
}

void inline CSliceBuffer::SetDirty(bool bDirty)
{
    m_bDirty = bDirty;
}

bool inline CSliceBuffer::GetDirty()
{
    return m_bDirty;
}

void inline CSliceBuffer::SetNew(bool bNew)
{
    m_bNew = bNew;
}

bool inline CSliceBuffer::GetNew()
{
    return m_bNew;
}


#endif /* SLICEBUFFER_HH_ */
