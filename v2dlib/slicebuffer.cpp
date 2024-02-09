/*
 * slicebuffer.cpp
 *
 *  Created on: Oct 24, 2011
 *      Author: rkale
 */


#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "slicebuffer.hh"
#include "imagemanip.h"
#include "ipputils.h"

using namespace V2DLib;
using namespace V2dLibColorTransform;

CSliceBuffer::CSliceBuffer(int aSize, V2dLibColorTransform::YUVToRGBConverter & a_oRGB)
: m_oRGB(a_oRGB)
{
#ifdef USE_IPP
    m_pY = ippsMalloc_8u(aSize * 2);
    m_pU = ippsMalloc_8u(aSize);
    m_pV = ippsMalloc_8u(aSize);
#else
    m_pY = new uint8_t[aSize * 2];
    m_pU = new uint8_t[aSize];
    m_pV = new uint8_t[aSize];
#endif

    memset(m_pY, 0x00, aSize * 2);
    memset(m_pU, 0x00, aSize);
    memset(m_pV, 0x00, aSize);

    m_nBitmapOffset = 0;
    m_nSize = aSize;
    m_bDirty = false;
    m_bNew = false;
    m_nSliceNumber = 0;
    m_bIs444 = false;
    m_bIsDynamic444 = false;
    m_bNeeds444Conversion = false;
}

CSliceBuffer::~CSliceBuffer()
{
#ifdef USE_IPP
    ippsFree(m_pY);
    ippsFree(m_pU);
    ippsFree(m_pV);
#else

    delete [] m_pY;
    delete [] m_pU;
    delete [] m_pV;
#endif
}

void CSliceBuffer::ComputeFramePositioningInfo(
        int a_nSliceNum, eEncodeFormat encodeFormat, ePixelFormat pixelFormat,
        int a_nSliceWidth, int a_nSliceHeight,
        int a_nFrameWidth, int a_nFrameHeight,
        int a_nEncodedWidth, int a_nEncodedHeight,
        int a_nSliceSize, int a_nScaleFactor)
{
    m_nFrameWidth = a_nFrameWidth;
    m_nFrameHeight = a_nFrameHeight;
    m_nSliceSize = a_nSliceSize;

    ComputeScaleParameters(encodeFormat, a_nSliceWidth, a_nSliceHeight,
            a_nSliceSize, a_nScaleFactor);

    SetUp444Mode(encodeFormat);

    ComputeRenderingParameters(pixelFormat);

    int nBlockNumInFrame, x, y;
    m_nDestSlicePixelWidth = m_nSlicePixelWidth;
    m_nDestSlicePixelHeight = m_nSlicePixelHeight;
    m_nSplitBlocks = 0;
    if (encodeFormat == eH264) {
        m_nBlocksPerRow = a_nEncodedWidth/16;
        nBlockNumInFrame = a_nSliceNum * a_nSliceSize;
        x = (nBlockNumInFrame % m_nBlocksPerRow) * 16 / a_nScaleFactor;
        y = (nBlockNumInFrame / m_nBlocksPerRow) * m_nBlocksPerRow
                * H264_BLOCK_SIZE * H264_SLICE_HEIGHT / (a_nScaleFactor
                * a_nScaleFactor);
        m_nBitmapOffset = x + y;
    }
    else if (encodeFormat == eSVC) {
        m_nBlocksPerRow = a_nEncodedWidth/8;
        nBlockNumInFrame = a_nSliceNum * a_nSliceWidth;
        x = (nBlockNumInFrame % m_nBlocksPerRow) * 8 / a_nScaleFactor;
        y = (nBlockNumInFrame / m_nBlocksPerRow) * a_nFrameWidth
                * 8 * a_nSliceHeight / (a_nScaleFactor
                * a_nScaleFactor);
        m_nBitmapOffset = x + y;
        // For border slices the destination area is truncated
        if (x + m_nSlicePixelWidth > a_nFrameWidth)
            m_nDestSlicePixelWidth = a_nFrameWidth - x;
        int yOffset = y / a_nFrameWidth;
        if (yOffset + m_nSlicePixelHeight > a_nFrameHeight)
            m_nDestSlicePixelHeight = a_nFrameHeight - yOffset;
    }
    else {
        m_nBlocksPerRow = a_nEncodedWidth/8;
        nBlockNumInFrame = a_nSliceNum * a_nSliceSize;
        x = (nBlockNumInFrame % m_nBlocksPerRow) * 8 / a_nScaleFactor;
        y = (nBlockNumInFrame / m_nBlocksPerRow) * m_nBlocksPerRow
                * V2D_BLOCK_SIZE / (a_nScaleFactor * a_nScaleFactor);
        m_nBitmapOffset = x + y;

        int nBlocksInThisRow = nBlockNumInFrame % m_nBlocksPerRow;
        int nRemainingBlocksInThisRow = m_nBlocksPerRow - nBlocksInThisRow;
        if (nRemainingBlocksInThisRow < (int) a_nSliceSize) {
            m_nSplitBlocks = a_nSliceSize - (a_nSliceSize - nRemainingBlocksInThisRow);
        }
    }
}


void CSliceBuffer::ComputeScaleParameters(
        eEncodeFormat encodeFormat,
        int a_nSliceWidth, int a_nSliceHeight,
        int a_nSliceSize, int a_nScaleFactor)
{
    m_nScaledWidth = m_nFrameWidth / a_nScaleFactor;
    m_nScaledHeight = m_nFrameHeight / a_nScaleFactor;

    if (encodeFormat == eH264) {
        m_nSlicePixelWidth = a_nSliceSize * 16 / a_nScaleFactor;
        m_nSlicePixelHeight = 16 * H264_SLICE_HEIGHT / a_nScaleFactor;
    }
    else if (encodeFormat == eSVC) {
        m_nSlicePixelWidth = 8 * a_nSliceWidth / a_nScaleFactor;
        m_nSlicePixelHeight = 8 * a_nSliceHeight / a_nScaleFactor;
    }
    else {
        m_nSlicePixelWidth = a_nSliceSize * 8 / a_nScaleFactor;
        m_nSlicePixelHeight = 8 / a_nScaleFactor;
    }

    m_sliceStrides[0] =  m_nSlicePixelWidth;
    m_sliceStrides[1] =  m_nSlicePixelWidth/2;
    m_sliceStrides[2] =  m_nSlicePixelWidth/2;


#ifdef USE_IPP
    m_sliceRoi.width  = m_nSlicePixelWidth;
    m_sliceRoi.height = m_nSlicePixelHeight;
#endif

}

void CSliceBuffer::SetUp444Mode(eEncodeFormat encodeFormat) {
    if (encodeFormat == eYUV444S) {
        m_bIs444 = true;
        m_bIsDynamic444 = false;
    }
    else if (encodeFormat == eYUV444D) {
        m_bIs444 = true;
        m_bIsDynamic444 = true;
    }
}

void CSliceBuffer::ComputeRenderingParameters(ePixelFormat pixelFormat) {

    m_pixelFormat = pixelFormat;
    m_nBytesPerPixel = 4;
    m_eImageFormat = YUVToRGBConverter::ImageFormat_BGRX;

    switch (m_pixelFormat) {
    case eYUV422:
        m_nBytesPerPixel = 2;
        break;
    case eRGB24:
        m_nBytesPerPixel = 3;
        m_eImageFormat = YUVToRGBConverter::ImageFormat_RGB;
        break;
    case eBGR24:
        m_nBytesPerPixel = 3;
        m_eImageFormat = YUVToRGBConverter::ImageFormat_BGR;
        break;
    case eBGRX32:
        m_nBytesPerPixel = 4;
        m_eImageFormat = YUVToRGBConverter::ImageFormat_BGRX;
        break;
    }
}

void CSliceBuffer::SaveSliceData(bool a_bIsRefreshSlice)
{
    if (m_bIsDynamic444) {
        m_bNeeds444Conversion = true;
    }
    else if (HasOdd444Content(a_bIsRefreshSlice)) {
        m_bNeeds444Conversion = true;
    }
    else {
        m_bNeeds444Conversion = false;
    }
}


uint8_t * CSliceBuffer::GetYOffset(bool a_bIsRefreshSlice)
{
    return m_pY;
}

uint8_t * CSliceBuffer::GetUOffset(bool a_bIsRefreshSlice)
{
    if (HasOdd444Content(a_bIsRefreshSlice)) {
        return m_pU + m_nSize/2;
    }
    return m_pU;
}

uint8_t * CSliceBuffer::GetVOffset(bool a_bIsRefreshSlice)
{
    if (HasOdd444Content(a_bIsRefreshSlice)) {
        return m_pV + m_nSize/2;
    }
    return m_pV;
}

uint8_t * CSliceBuffer::GetExtendedYOffset(bool a_bIsRefreshSlice)
{
    return m_pY;
}

uint8_t * CSliceBuffer::GetExtendedUOffset(bool a_bIsRefreshSlice)
{
    if (HasEven444Content(a_bIsRefreshSlice)) {
        return m_pU + m_nSize/2;
    }
    return m_pU;
}

uint8_t * CSliceBuffer::GetExtendedVOffset(bool a_bIsRefreshSlice)
{
    if (HasEven444Content(a_bIsRefreshSlice)) {
        return m_pV + m_nSize/2;
    }
    return m_pV;
}

bool inline CSliceBuffer::HasOdd444Content(bool a_bIsRefreshSlice)
{
    if (!a_bIsRefreshSlice)
        return false;

    if (m_bIs444)
        return true;

    return false;
}

bool inline CSliceBuffer::HasEven444Content(bool a_bIsRefreshSlice)
{
    return HasOdd444Content(!a_bIsRefreshSlice);
}

void CSliceBuffer::RenderSliceIntoFrame(uint8_t * a_pDecodeBuffer, bool a_bIsRefreshSlice) {

    if (m_pixelFormat == eYUV422) {
        RenderSliceIntoYUVFrame(a_pDecodeBuffer);
    }
    else {
        RenderSliceIntoRGBFrame(a_pDecodeBuffer);
    }
}
void CSliceBuffer::RenderSliceIntoRGBFrame(uint8_t * a_pDecodeBuffer) {

#if !defined(USE_IPP) || defined(PASS_TESTS)

    int destWidth = GetSlicePixelWidth();
    if (GetSplitBlocks() > 0) {
        // Un-Highlight first section for split slices
        destWidth = destWidth - (m_nSliceSize - GetSplitBlocks()) * 8;
    }
    const unsigned char *slicePlanes[3];
    slicePlanes[0] = m_pY;
    slicePlanes[1] = m_pU;
    slicePlanes[2] = m_pV;
    ConvertYUVToRGB(
            slicePlanes,
            m_sliceStrides,
            destWidth,
            GetSlicePixelHeight(),
            a_pDecodeBuffer + GetBitmapOffset() * m_nBytesPerPixel,
            m_nScaledWidth * m_nBytesPerPixel,
            m_eImageFormat);


    if (GetSplitBlocks() > 0) {
        // Un-Highlight remaining section for split slices
        uint8_t * bitmapOffset = a_pDecodeBuffer + GetBitmapOffset() * m_nBytesPerPixel;
        bitmapOffset += (destWidth + m_nScaledWidth * 7) * m_nBytesPerPixel;
        slicePlanes[0] = m_pY + destWidth;
        slicePlanes[1] = m_pU + destWidth/2;
        slicePlanes[2] = m_pV + destWidth/2;
        destWidth = (m_nSliceSize - GetSplitBlocks()) * 8;
        ConvertYUVToRGB(
                slicePlanes,
                m_sliceStrides,
                destWidth,
                GetSlicePixelHeight(),
                bitmapOffset,
                m_nScaledWidth * m_nBytesPerPixel,
                m_eImageFormat);
    }

#else
    // For BGRX

    // First convert YUV to 24 bit BGR
    ippiYCbCr422ToBGR_8u_P3C3R(m_pSlicePlanes, m_sliceStrides,
            m_pRGBSlice, m_nSlicePixelWidth * 3, m_sliceRoi);

    // Add an alpha channnel
    const int dstOrder[4] = {0,1,2,3};
    ippiSwapChannels_8u_C3C4R(m_pRGBSlice, m_nSlicePixelWidth * 3,
            m_pDecodeBuffer + bitmapoffset * 4, m_nScaledWidth * 4,
            m_sliceRoi, dstOrder, 0);

    // For RGB24
    ippiYCbCr422ToRGB_8u_P3C3R(m_pSlicePlanes, m_sliceStrides,
            m_pDecodeBuffer + bitmapoffset * 3, m_nScaledWidth * 3,
            m_sliceRoi);

    // For BGR4
    ippiYCbCr422ToBGR_8u_P3C3R(m_pSlicePlanes, m_sliceStrides,
            m_pDecodeBuffer + bitmapoffset * 3, m_nScaledWidth * 3,
            m_sliceRoi);

    // For YUV422
    IppiSize roiSizeY = {m_nSlicePixelWidth, m_nSlicePixelHeight};
    IppiSize roiSizeUV = {m_nSlicePixelWidth/2, m_nSlicePixelHeight};
    ippiCopy_8u_C1R(m_pYSlice, m_nSlicePixelWidth,
            pYArr + bitmapoffset, nYStride, roiSizeY);
    ippiCopy_8u_C1R(m_pUSlice, m_nSlicePixelWidth/2,
            pUArr + (bitmapoffset >> 1), nUVStride, roiSizeUV);
    ippiCopy_8u_C1R(m_pVSlice, m_nSlicePixelWidth/2,
            pVArr + (bitmapoffset >> 1), nUVStride, roiSizeUV);

#endif
}


void CSliceBuffer::RenderSliceIntoYUVFrame(uint8_t * a_pDecodeBuffer)
{
    unsigned char * const pYArr = a_pDecodeBuffer;
    unsigned char * const pUArr = pYArr + (m_nScaledWidth * m_nScaledHeight);
    unsigned char * const pVArr = pUArr + (m_nScaledWidth * m_nScaledHeight)/2;
    const short nYStride   = m_nScaledWidth;
    const short nUVStride  = m_nScaledWidth/2;

#if !defined(USE_IPP) || defined(PASS_TESTS)
    int destWidth = m_nDestSlicePixelWidth;
    if (m_nSplitBlocks > 0) {
        // Copy first portion of the slice
        destWidth = m_nDestSlicePixelWidth - (m_nSliceSize - m_nSplitBlocks) * 8;
    }
    copyImagePlane(m_pY, m_nSliceSize * 8, destWidth,
            pYArr + m_nBitmapOffset, nYStride);
    copyImagePlane(m_pU, m_nSliceSize * 4, destWidth/2,
            pUArr + m_nBitmapOffset/2, nUVStride);
    copyImagePlane(m_pV, m_nSliceSize * 4, destWidth/2,
            pVArr + m_nBitmapOffset/2, nUVStride);

    if (m_nSplitBlocks > 0) {
        // Copy remaining portion of the slice
        int bitmapoffset = m_nBitmapOffset + destWidth + m_nScaledWidth * 7;
        int srcOffset = destWidth;
        destWidth = (m_nSliceSize - m_nSplitBlocks) * 8;
        copyImagePlane(m_pY + srcOffset, m_nSliceSize * 8, destWidth,
                pYArr + bitmapoffset, nYStride);
        copyImagePlane(m_pU + srcOffset/2, m_nSliceSize * 4, destWidth/2,
                pUArr + bitmapoffset/2, nUVStride);
        copyImagePlane(m_pV + srcOffset/2, m_nSliceSize * 4, destWidth/2,
                pVArr + bitmapoffset/2, nUVStride);
    }

#else
    IppiSize roiSizeY = {m_nSlicePixelWidth, m_nSlicePixelHeight};
    IppiSize roiSizeUV = {m_nSlicePixelWidth/2, m_nSlicePixelHeight};
    ippiCopy_8u_C1R(m_pYSlice, m_nSlicePixelWidth,
            pYArr + bitmapoffset, nYStride, roiSizeY);
    ippiCopy_8u_C1R(m_pUSlice, m_nSlicePixelWidth/2,
            pUArr + (bitmapoffset >> 1), nUVStride, roiSizeUV);
    ippiCopy_8u_C1R(m_pVSlice, m_nSlicePixelWidth/2,
            pVArr + (bitmapoffset >> 1), nUVStride, roiSizeUV);
#endif


}


void CSliceBuffer::HighLightSlice(uint8_t * a_pDecodeBuffer, eEncodeFormat encodeFormat,
        int red, int green, int blue)
{
    if (m_pixelFormat != eBGRX32 && m_pixelFormat != eBGR24)
        return;

    int bIs24Bit = m_pixelFormat == eBGR24 ? 1 : 0;

    uint8_t * bitmapByteOffset = a_pDecodeBuffer + m_nBitmapOffset * m_nBytesPerPixel;

    uint8_t * offset;
    if (encodeFormat == eSVC) {
        markSliceInImage(bitmapByteOffset, m_nSlicePixelWidth, m_nSlicePixelHeight,
                m_nDestSlicePixelWidth, m_nDestSlicePixelHeight, 0,
                m_nScaledWidth * m_nBytesPerPixel, red, green, blue, bIs24Bit);
        for (int row = 0; row < m_nDestSlicePixelHeight/8; row++) {
            offset = bitmapByteOffset + row * m_nScaledWidth * m_nBytesPerPixel * 8;
            for (int col = 0; col < m_nDestSlicePixelWidth/8; col++) {
                markBlockInImage(offset + col * 8 * m_nBytesPerPixel,
                        m_nScaledWidth * m_nBytesPerPixel, red, green, blue, bIs24Bit);
            }
        }
    }
    else if (encodeFormat == eH264) {

    }
    else  {
        // For codec version 1
        int nNumberOfBlocks = m_nSliceSize;
        int initialWidth = m_nDestSlicePixelWidth;
        if (m_nSplitBlocks > 0) {
            // Highlight first section for split slices
            initialWidth = m_nDestSlicePixelWidth - (m_nSliceSize - m_nSplitBlocks) * 8;
            nNumberOfBlocks = m_nSplitBlocks;
        }
        markSliceInImage(bitmapByteOffset, m_nSlicePixelWidth, m_nSlicePixelHeight,
                initialWidth, m_nDestSlicePixelHeight, 0,
                m_nScaledWidth * m_nBytesPerPixel, red, green, blue, bIs24Bit);
        for (int blk = 0; blk < nNumberOfBlocks; blk++) {
            markBlockInImage(bitmapByteOffset + blk * 8 * m_nBytesPerPixel,
                    m_nScaledWidth * m_nBytesPerPixel, red, green, blue, bIs24Bit);
        }

        if (m_nSplitBlocks > 0) {
            // Highlight remaining section for split slices
            bitmapByteOffset += (initialWidth + m_nScaledWidth * 7) * m_nBytesPerPixel;
            int remainingWidth = (m_nSliceSize - m_nSplitBlocks) * 8;
            nNumberOfBlocks = m_nSliceSize - m_nSplitBlocks;
            markSliceInImage(bitmapByteOffset, m_nSlicePixelWidth, m_nSlicePixelHeight,
                    remainingWidth, m_nDestSlicePixelHeight, 1,
                    m_nScaledWidth * 4, red, green, blue, bIs24Bit);
            for (int blk = 0; blk < nNumberOfBlocks; blk++) {
                markBlockInImage(bitmapByteOffset + blk * 8 * m_nBytesPerPixel,
                        m_nScaledWidth * m_nBytesPerPixel, red, green, blue, bIs24Bit);
            }
        }
    }
}

void CSliceBuffer::UnHighLightSlice(uint8_t * a_pDecodeBuffer)
{
    RenderSliceIntoRGBFrame(a_pDecodeBuffer);
}

void CSliceBuffer::ConvertYUVToRGB(
        const unsigned char *a_slicePlanes[3],
        int a_sourceStride[],
        unsigned int a_width,
        unsigned int a_height,
        unsigned char *a_destBuffer,
        int a_destStride,
        V2dLibColorTransform::YUVToRGBConverter::ImageFormat a_eImageFormat)
{
#ifdef USE_IPP_5DOT3
    IppiSize srcSize = {a_width, a_height};
#endif

    if (m_bNeeds444Conversion) {
        m_oRGB.yuv444ToRGBFormat(a_slicePlanes, a_sourceStride,
                a_width, a_height,
                a_destBuffer,
                a_destStride,
                a_eImageFormat);
    }
    else {
#ifdef USE_IPP_5DOT3
        const int dstOrder[3] = { 2, 1, 0};

        ippiYCbCr422ToRGB_8u_P3C3R(a_slicePlanes, a_sourceStride, a_destBuffer, a_destStride, srcSize);
        ippiSwapChannels_8u_C3IR(a_destBuffer, a_destStride, srcSize, dstOrder);
#else
        m_oRGB.yuv422ToRGBFormat(a_slicePlanes, a_sourceStride,
                a_width, a_height,
                a_destBuffer,
                a_destStride,
                a_eImageFormat);
#endif
    }
}
