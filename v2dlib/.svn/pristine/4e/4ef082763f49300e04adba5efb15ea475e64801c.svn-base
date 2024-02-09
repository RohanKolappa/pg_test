#include "v2dlib_compatibility.h"
#include <iostream>

#include "vframedecoder.hh"
#include "codecdefs.h"
#include "slicebuffer.hh"
#include "imagemanip.h"
#include "colortransform.h"
#include "ipputils.h"
#include "svcslicedecoder.hh"
#include "memory_tracker.h"


using namespace V2DLib;
using namespace V2dLibColorTransform;

CVideoFrameDecoder*
CVideoFrameDecoder::CreateObj(v2dDecConfigurationPtr a_pDecConfig)
{
    CVideoFrameDecoder* pFrameDecoder = new CVideoFrameDecoder(a_pDecConfig);
    a_pDecConfig->m_nDecodeBufLen = pFrameDecoder->GetDecodeBufLen();

    return pFrameDecoder;
}

void
CVideoFrameDecoder::DeleteObj(CVideoFrameDecoder * a_pFrameDecoder)
{
    delete a_pFrameDecoder;
}

CVideoFrameDecoder::CVideoFrameDecoder(v2dDecConfigurationPtr a_pDecConfig)
{

    m_nFrameWidth   = a_pDecConfig->m_nWidth;
    m_nFrameHeight  = a_pDecConfig->m_nHeight;
    m_nSliceSize    = a_pDecConfig->m_nSliceSize;
    m_eEncodeFormat = a_pDecConfig->m_eEncodeFormat;
    m_eDecodeFormat = a_pDecConfig->m_eDecodeFormat;
    m_nEncodedOffsetX = 0;
    m_nEncodedOffsetY = 0;
    m_nEncodedWidth = m_nFrameWidth;
    m_nEncodedHeight = m_nFrameHeight;
    m_nSliceWidth = SVC_SLICE_WIDTH;
    m_nSliceHeight = SVC_SLICE_HEIGHT;

    m_nScaleFactor = 1;

    m_pSliceDecoder = NULL;
    m_nSlicesPerFrame = 0;
    m_bSliceDebug = false;
    m_bDisableSliceDebug = false;
    m_bErr = false;
    m_strError = "";
    m_nFramesDecoded = 0;
    m_bDoubleBuffer = false;

    InitDecoder();

}

CVideoFrameDecoder::~CVideoFrameDecoder()
{
    delete m_pSliceDecoder;

    m_Slices.clear();

    DeleteSliceBuffers();

}

void CVideoFrameDecoder::InitDecoder()
{
    delete m_pSliceDecoder;
    m_pSliceDecoder = CSliceDecoder::CreateSliceDecoder(m_eEncodeFormat);

    m_pSliceDecoder->SetScaleFactor(m_nScaleFactor);

    CSVCSliceDecoder *pSliceDecoder = dynamic_cast<CSVCSliceDecoder *>(m_pSliceDecoder);
    if (pSliceDecoder != NULL)
        pSliceDecoder->SetSliceDimensions(m_nSliceWidth, m_nSliceHeight);

    size_t nSize;
    if (m_eEncodeFormat == eH264) {
        m_nSliceSize = H264_SLICE_WIDTH;
        nSize = H264_BLOCK_SIZE * H264_SLICE_HEIGHT * m_nSliceSize;
    }
    else if (m_eEncodeFormat == eSVC) {
        m_nSliceSize = m_nSliceWidth * m_nSliceHeight;
        nSize =  V2D_BLOCK_SIZE * m_nSliceSize;
    }
    else {
        nSize = V2D_BLOCK_SIZE * m_nSliceSize;
    }


    DeleteSliceBuffers();

    ComputeScaleParameters();

    ComputeSlicesPerFrame();

    CreateSliceBuffers(nSize);

}

int CVideoFrameDecoder::Decode(unsigned char* a_pEncFrame,
                                const size_t a_nEncLen,
                                unsigned char* a_pDecFrame,
                                const size_t a_nDecLen)
{
    if (a_nDecLen < m_nDecodeBufLen)
        return EDECODER_OUTBUFFTOOSMALL;

    m_pDecodeBuffer = a_pDecFrame;

    InitRectangles();

    return Decode(a_pEncFrame, a_nEncLen);
}

int CVideoFrameDecoder::Decode(unsigned char* a_pEncFrame,
                                const size_t a_nEncLen)
{
    if (m_bErr)
        return EDECODER_NOMEMORY;

    ParseSlicesInBuffer(m_Slices, a_pEncFrame, a_nEncLen, m_eEncodeFormat == eSVC);

    int rc = DecodeSlicesIntoFrameBuffer();
    if (rc != 0)
        return rc;

    return rc;
}


void CVideoFrameDecoder::CreateSliceBuffers(int aSize) {
    for (int sliceNumber = 0; sliceNumber < m_nSlicesPerFrame; sliceNumber++) {
        m_SliceBuffers[sliceNumber] = new CSliceBuffer(aSize, m_oRGB);

        m_SliceBuffers[sliceNumber]->ComputeFramePositioningInfo(
                sliceNumber, m_eEncodeFormat, m_eDecodeFormat,
                m_nSliceWidth, m_nSliceHeight,
                m_nFrameWidth, m_nFrameHeight,
                m_nEncodedWidth, m_nEncodedHeight,
                m_nSliceSize, m_nScaleFactor);
    }
}

void CVideoFrameDecoder::DeleteSliceBuffers() {
    for (int i = 0; i < m_nSlicesPerFrame; i++) {
        CSliceBuffer * sliceBuffer = m_SliceBuffers[i];
        if (sliceBuffer != NULL) {
            delete m_SliceBuffers[i];
        }
        m_SliceBuffers.erase(i);
    }
}

void CVideoFrameDecoder::MarkSliceBuffersAsOld() {
    for (int i = 0; i < m_nSlicesPerFrame; i++) {
        CSliceBuffer * sliceBuffer = m_SliceBuffers[i];
        if (sliceBuffer != NULL) {
            sliceBuffer->SetNew(false);
        }
    }
}

void CVideoFrameDecoder::SaveSliceToBuffer(CSliceBuffer * a_sliceBuffer, bool a_bIsRefreshSlice)  {
    a_sliceBuffer->SaveSliceData(a_bIsRefreshSlice);
    a_sliceBuffer->SetNew(true);
    a_sliceBuffer->SetDirty(true);
}

void CVideoFrameDecoder::ComputeScaleParameters()
{
    m_nScaledWidth = m_nFrameWidth / m_nScaleFactor;
    m_nScaledHeight = m_nFrameHeight / m_nScaleFactor;

    int numbPixels = (m_nFrameWidth * m_nFrameHeight)/(m_nScaleFactor * m_nScaleFactor);
    m_nDecodeBufLen = numbPixels * BytesPerPixel();
}

int CVideoFrameDecoder::BytesPerPixel() const {
    int bytesPerPixel = 4;
    switch(m_eDecodeFormat) {
    case eYUV422:
        bytesPerPixel = 2;
        break;
    case eRGB24:
    case eBGR24:
        bytesPerPixel = 3;
        break;
    case eBGRX32:
        bytesPerPixel = 4;
        break;
    }

    return bytesPerPixel * sizeof(unsigned char);
}

void CVideoFrameDecoder::ComputeSlicesPerFrame()
{
    if (m_eEncodeFormat == eH264) {

        int nMacroBlocksPerRow = m_nEncodedWidth / 16;
        int nSlicesPerRow = nMacroBlocksPerRow / H264_SLICE_WIDTH;
        int nNumberOfRows = m_nEncodedHeight / (16 * H264_SLICE_HEIGHT);
        m_nSlicesPerFrame = nSlicesPerRow * nNumberOfRows;
    }
    else if (m_eEncodeFormat == eSVC) {

        int nBlocksPerRow = m_nEncodedWidth / 8;
        int nSlicesPerRow = nBlocksPerRow / m_nSliceWidth;
        int nNumberOfRows = m_nEncodedHeight / (8 * m_nSliceHeight);
        m_nSlicesPerFrame = nSlicesPerRow * nNumberOfRows;
    }
    else {
        int nBlocksPerRow = m_nEncodedWidth / 8;
        int nNumberOfRows = m_nEncodedHeight / 8;
        int nTotalNumberOfBlocks = nBlocksPerRow * nNumberOfRows;
        m_nSlicesPerFrame = nTotalNumberOfBlocks / m_nSliceSize;
    }
}

void CVideoFrameDecoder::CheckDimensions(const SVCFrameHeader &aFrameHeader)
{
    if (aFrameHeader.bFrameDimensions == 0)
        return;

    if (aFrameHeader.nFrameWidth != m_nFrameWidth ||
            aFrameHeader.nFrameHeight != m_nFrameHeight ||
            aFrameHeader.nEncodedOffsetX != m_nEncodedOffsetX ||
            aFrameHeader.nEncodedOffsetY != m_nEncodedOffsetY ||
            aFrameHeader.nEncodedWidth != m_nEncodedWidth ||
            aFrameHeader.nEncodedHeight != m_nEncodedHeight ||
            aFrameHeader.nSliceWidth != m_nSliceWidth ||
            aFrameHeader.nSliceHeight != m_nSliceHeight)
    {
        m_nFrameWidth = aFrameHeader.nFrameWidth;
        m_nFrameHeight = aFrameHeader.nFrameHeight;
        m_nEncodedOffsetX = aFrameHeader.nEncodedOffsetX;
        m_nEncodedOffsetY = aFrameHeader.nEncodedOffsetY;
        m_nEncodedWidth = aFrameHeader.nEncodedWidth;
        m_nEncodedHeight = aFrameHeader.nEncodedHeight;
        m_nSliceWidth = aFrameHeader.nSliceWidth;
        m_nSliceHeight = aFrameHeader.nSliceHeight;

        InitDecoder();
    }

}

uint32_t CVideoFrameDecoder::GetLayerErrors()
{
    CSVCSliceDecoder *pSliceDecoder = dynamic_cast<CSVCSliceDecoder *>(m_pSliceDecoder);
    if (pSliceDecoder == NULL)
        return 0;
    return pSliceDecoder->GetLayerErrors();
}

size_t CVideoFrameDecoder::GetFrameWidth() const
{
    return m_nFrameWidth;
}
size_t CVideoFrameDecoder::GetFrameHeight() const
{
    return m_nFrameHeight;
}

size_t CVideoFrameDecoder::GetScaledWidth() const
{
    return m_nScaledWidth;
}

size_t CVideoFrameDecoder::GetScaledHeight() const
{
    return m_nScaledHeight;
}

size_t CVideoFrameDecoder::GetSliceSize() const
{
    return m_nSliceSize;
}

size_t CVideoFrameDecoder::GetDecodeBufLen() const
{
    return m_nDecodeBufLen;
}

int CVideoFrameDecoder::GetScaleFactor() const
{
    return m_nScaleFactor;
}

void CVideoFrameDecoder::SetScaleFactor(int a_nScaleFactor) {
    if (!(a_nScaleFactor == 8 ||
            a_nScaleFactor == 4 ||
            a_nScaleFactor == 2 ||
            a_nScaleFactor == 1))
        return;

    m_nScaleFactor = a_nScaleFactor;
    ComputeScaleParameters();
    m_pSliceDecoder->SetScaleFactor(m_nScaleFactor);
}

void CVideoFrameDecoder::SetSliceDebug(bool a_bSliceDebug) {
    if (a_bSliceDebug) {
        m_bSliceDebug = true;
    }
    else {
        // Disabled lazily
        if (m_bSliceDebug == true)
            m_bDisableSliceDebug = true;
    }
}

bool CVideoFrameDecoder::GetSliceDebug()
{
    if (m_bDisableSliceDebug == true)
        return false;

    if (m_bSliceDebug == true)
        return true;

    return false;
}

void CVideoFrameDecoder::SetDoubleBuffering(bool a_bDoubleBuffer) {
    m_bDoubleBuffer = a_bDoubleBuffer;
}

bool CVideoFrameDecoder::GetDoubleBuffering()
{
    return m_bDoubleBuffer;
}

void CVideoFrameDecoder::InitRectangles() {
    m_Rects.clear();
    m_CurrentRect = CRect(); // init to null rect;
    m_LastYPos = 0;
    m_nLastSliceNum = 0; // do not set to -1
}

void CVideoFrameDecoder::AddCurrentRectangle() {
    if (!m_CurrentRect.IsNull())
        m_Rects.push_back(m_CurrentRect);
}

void CVideoFrameDecoder::DumpRectangles()
{
    std::list<CRect>::const_iterator iter;
    int count = 0;
    int blocks = 0;
    for(iter = m_Rects.begin(); iter != m_Rects.end(); iter++) {
        count++;
        blocks += iter->area();
    }
    blocks = blocks / V2D_BLOCK_SIZE;
}


// Compute worst case size needed to store updated rectangles
size_t CVideoFrameDecoder::GetMaxRectanglesBufferLen() const
{
    int slicesInRow = (m_nEncodedWidth/8) / m_nSliceSize;
    if ((m_nEncodedWidth/8) % m_nSliceSize > 0) {
        slicesInRow++;
    }
    int numbRows = m_nEncodedHeight/8;
    int maxRects = numbRows * slicesInRow;

    // If every slice is touched we need room for a maximum of a full frame of pixels
    // In addition, each rectangle header will be of size 8 (four shorts)
    // 2 bytes are also reserved for rectangle count
    int bufferLen = (m_nEncodedWidth * m_nEncodedHeight) * BytesPerPixel() +
            (maxRects * 8) + 2;

    return bufferLen;
}

size_t CVideoFrameDecoder::GetRectanglesBuffer(unsigned char *a_DecodeBuffer,
        unsigned char *a_DestBuffer) const
{
    std::list<CRect>::const_iterator iter;
    int nTotalLen = 0;
    int rectCount = 0;
    unsigned char * dst = a_DestBuffer;

    int bytesPerPixel = BytesPerPixel();

    // Reserve space for rectangle count
    dst += 2;
    nTotalLen += 2;
    //printf("Progress: %d %d\n", nTotalLen, dst - a_DestBuffer);

    // For each rectangle
    for(iter = m_Rects.begin(); iter != m_Rects.end(); iter++) {
        // Copy rectangle Header
        *((uint16_t *) dst) = (uint16_t) iter->x();
        *((uint16_t *) (dst + 2)) = (uint16_t) iter->y();
        *((uint16_t *) (dst + 4)) = (uint16_t) iter->width();
        *((uint16_t *) (dst + 6)) = (uint16_t) iter->height();
        nTotalLen += 8;
        dst += 8;
        //printf("Progress: %d %d %d\n", nTotalLen, dst - a_DestBuffer, rectCount);

        // Copy rectangle data
        unsigned char * src = a_DecodeBuffer + iter->y() * m_nFrameWidth * bytesPerPixel;
        src += iter->x() * bytesPerPixel;
        for (int row = 0; row < iter->height(); row++) {
            //printf("Copying %d from %d (%d)\n",
              //      iter->width() * bytesPerPixel, src - a_DecodeBuffer, iter->y());
            ::memcpy(dst, src, iter->width() * bytesPerPixel);
            dst += iter->width() * bytesPerPixel;
            src += m_nFrameWidth * bytesPerPixel;
            nTotalLen += iter->width() * bytesPerPixel;
            //printf("Progress: %d %d %d\n", nTotalLen, dst - a_DestBuffer, rectCount);
        }

        // Update rectangle count
        rectCount++;
    }

    // Plug in the rectangle count at beginning of destination buffer
    *((uint16_t *) a_DestBuffer) = (uint16_t) rectCount;

    return nTotalLen;
}

// For SVC encoding, the first slice is actually a frame header
bool CVideoFrameDecoder::DecodeSliceAsFrameHeader() {
    if (m_eEncodeFormat != eSVC)
        return true;

    if (m_Slices.empty())
        return true;

    CSlice sc = m_Slices.front();
    SVCFrameHeader frameHeader;

    CSVCSliceDecoder *pSliceDecoder = dynamic_cast<CSVCSliceDecoder *>(m_pSliceDecoder);
    if (pSliceDecoder == NULL)
        return false;
    bool bRet = CSVCSliceDecoder::DecodeSVCFrameHeader(sc.m_pData, sc.m_nLen, frameHeader);

    // Check if resolution changed
    if (bRet)
        CheckDimensions(frameHeader);

    m_Slices.pop_front();
    if (!bRet)
        return false;
    return true;
}

int CVideoFrameDecoder::DecodeSlicesIntoFrameBuffer()
{
    // Decode SVC Frame header
    if (!DecodeSliceAsFrameHeader())
        return EDECODER_MALFORMEDFRAME;

    // Clear New flag on all slice buffers
    MarkSliceBuffersAsOld();

    // Decode slices
    while(!m_Slices.empty()) {
        int nSliceNumber = 0;
        bool bIsRefreshSlice = false;

        CSlice sc = m_Slices.front();

        // Parse headers
        m_pSliceDecoder->DecodeHeader(sc.m_pData, sc.m_nLen,
                nSliceNumber, bIsRefreshSlice);

        // Verify slice number
        if (nSliceNumber >= m_nSlicesPerFrame) {
            break;
        }

        CSliceBuffer *sliceBuffer = m_SliceBuffers[nSliceNumber];
        if (sliceBuffer == NULL)
            break;

        // Decode
        bool bRet = m_pSliceDecoder->Decode(sc.m_pData, sc.m_nLen,
                m_nSliceSize,
                sliceBuffer->GetYOffset(bIsRefreshSlice),
                sliceBuffer->GetUOffset(bIsRefreshSlice),
                sliceBuffer->GetVOffset(bIsRefreshSlice),
                nSliceNumber);
        m_Slices.pop_front();

        if (m_eEncodeFormat == eYUV444D) {
            // Decode chroma slice if present
            int offset = ParseForChromaSlice(sc.m_pData, sc.m_nLen);
            if (offset > 0) {
                int chromaSliceNumber = 0;
                m_pSliceDecoder->Decode(sc.m_pData + (offset + 1), sc.m_nLen - (offset + 1),
                        m_nSliceSize,
                        sliceBuffer->GetExtendedYOffset(bIsRefreshSlice),
                        sliceBuffer->GetExtendedUOffset(bIsRefreshSlice),
                        sliceBuffer->GetExtendedVOffset(bIsRefreshSlice),
                        chromaSliceNumber);
            }
        }

        // Render into our buffer
        if (bRet) {
            CopySliceToFrame(nSliceNumber, bIsRefreshSlice);
            m_nLastSliceNum = nSliceNumber;
        }
    }

    // Un-Highlight slices not updated in this round
    UnHighLightSlices();

    // Slices may merge into a single rectangle.
    // Add the last merged rectangle to our list.
    AddCurrentRectangle();

    m_nFramesDecoded++;

    return 0;
}

void CVideoFrameDecoder::CoaleseceRectangles(CSliceBuffer * a_sliceBuffer) {
    if (m_eEncodeFormat == eYUV422P) {
        for (size_t nBlk = 0; nBlk < m_nSliceSize; nBlk++) {
            unsigned int nBlockNumInFrame = a_sliceBuffer->GetSliceNumber() * m_nSliceSize + nBlk;
            unsigned int xPos = (nBlockNumInFrame % a_sliceBuffer->GetBlocksPerRow()) * 8;
            unsigned int yPos = (nBlockNumInFrame / a_sliceBuffer->GetBlocksPerRow()) * 8;
            if (nBlk == 0) {
                // Coalesce contiguous slices in a row into a single rectangle
                if (yPos != m_LastYPos || (m_nLastSliceNum + 1) != a_sliceBuffer->GetSliceNumber()) {
                    // Not contiguous, create a new rectangle
                    AddCurrentRectangle();
                    m_CurrentRect = CRect(xPos, yPos, m_nSliceSize * 8, 8);
                }
                else {
                    // Append rectangle for this new slice to the current rectangle
                    m_CurrentRect.setWidth(m_CurrentRect.width() + m_nSliceSize
                            * 8);
                }
            }
            if (yPos != m_LastYPos) {
                // Handle slice that splits across one scan line
                m_CurrentRect.setWidth(m_CurrentRect.width() - (m_nSliceSize
                        - nBlk) * 8);
                AddCurrentRectangle();
                m_CurrentRect = CRect(xPos, yPos, (m_nSliceSize - nBlk) * 8, 8);
            }
            m_LastYPos = yPos;

        }
    }
}

// This function takes the decoded Y, U, V slice and copies it into the frame
// buffer. The position in the frame buffer is calculated from the slice
// number.
void CVideoFrameDecoder::CopySliceToFrame(const int a_nSliceNumber, bool a_bIsRefreshSlice)
{

    CSliceBuffer * sliceBuffer = m_SliceBuffers[a_nSliceNumber];
    if (sliceBuffer == NULL)
        return;

    CoaleseceRectangles(sliceBuffer);

    SaveSliceToBuffer(sliceBuffer, a_bIsRefreshSlice);

    RenderSlicePixelsIntoFrame(sliceBuffer, a_bIsRefreshSlice);

    HighLightSlice(sliceBuffer, a_bIsRefreshSlice);
}

void CVideoFrameDecoder::RenderSlicePixelsIntoFrame(CSliceBuffer * a_sliceBuffer, bool a_bIsRefreshSlice)
{
    a_sliceBuffer->RenderSliceIntoFrame(m_pDecodeBuffer, a_bIsRefreshSlice);
}

void CVideoFrameDecoder::GetHighLightColors(int & red, int & green, int & blue) {
    red = 0;
    green = 255;
    blue = 255;
    if (m_eEncodeFormat == eSVC) {
        CSVCSliceDecoder *pSliceDecoder = dynamic_cast<CSVCSliceDecoder *>(m_pSliceDecoder);
        if (pSliceDecoder != NULL) {
            if (pSliceDecoder->GetDecodeType() == DECODE_TYPE_SCALABLE_DC) {
                red = 255;
                green = 0;
                blue = 0;
            }
            else if (pSliceDecoder->GetDecodeType() == DECODE_TYPE_SCALABLE_QUAD) {
                red = 0;
                green = 255;
                blue = 0;
            }
        }
    }
}

void CVideoFrameDecoder::HighLightSlice(CSliceBuffer * a_sliceBuffer, bool a_bIsRefreshSlice)
{
    if (m_bSliceDebug == false || m_bDisableSliceDebug == true)
        return;

    if (m_eDecodeFormat != eBGRX32 && m_eDecodeFormat != eBGR24)
        return;

    int red, green, blue;
    GetHighLightColors(red, green, blue);

    a_sliceBuffer->HighLightSlice(m_pDecodeBuffer, m_eEncodeFormat, red, green, blue);
}

void CVideoFrameDecoder::UnHighLightSlices() {
    if (m_bSliceDebug == false && m_bDoubleBuffer == false)
        return;

    if (m_eDecodeFormat != eBGRX32 && m_eDecodeFormat != eBGR24 && m_bSliceDebug)
        return;

    // Un-highlight all slices currently marked dirty but not new
    for (int i = 0; i < m_nSlicesPerFrame; i++) {
        CSliceBuffer * sliceBuffer = m_SliceBuffers[i];
        if (sliceBuffer == NULL)
            continue;
        if (!(sliceBuffer->GetDirty() && !sliceBuffer->GetNew()))
            continue;

        sliceBuffer->SetDirty(false);
        sliceBuffer->UnHighLightSlice(m_pDecodeBuffer);
    }

    // Disabling of slice debug is done lazily so that we get
    // a chance to UnHighlight all slices and send at least one
    // clean frame out.
    if (m_bDisableSliceDebug) {
        m_bSliceDebug = false;
        m_bDisableSliceDebug = false;
    }

}

void CVideoFrameDecoder::ParseSlicesInBuffer(std::list<CSlice> &a_rSlices,
        unsigned char* a_pBuffer, const size_t a_nBufLen, const bool isSVC)
{
    a_rSlices.clear();

    unsigned char* buff = a_pBuffer;
    unsigned char* prev = 0;
    const size_t nBufLen = a_nBufLen - (a_nBufLen % 4);
    size_t nLen = 0;

    eToken tok = ETOKEN_NONE;
    while (nLen < nBufLen) {
        if (*(buff) == 0) {
            if (*((buff) + 1) == 0) {
                if (*((buff) + 2) == 0x01) {
                    if (*((buff) + 3) == 0xa0) {
                        tok = ETOKEN_START_LEFT_FRAME;
                    }
                    else if(*((buff) + 3) == 0) {
                        tok = ETOKEN_START_RIGHT_FRAME;
                    }
                    else if (isSVC && *((buff) + 3) == 0xb2) {
                        tok = ETOKEN_START_SLICE;
                    }
                   }
            }
            else if (!isSVC && *((buff) + 1) == 0x01) {
                if (*((buff) + 2) == 0xb2)
                    tok = ETOKEN_START_SLICE;
            }
        }

        if( (nLen == 0) && (tok != ETOKEN_START_LEFT_FRAME) &&
                (tok != ETOKEN_START_RIGHT_FRAME) )
            return;

        if (tok != ETOKEN_NONE) {
            if (nLen) {
                // The data between prev and buff, is a slice.
                // Add to slice list.
                CSlice slice(prev, static_cast<size_t>(buff - prev));
                a_rSlices.push_back(slice);
            }

            prev = buff;
            tok = ETOKEN_NONE;
        }

        nLen = (isSVC) ? nLen + 1 : nLen + 4;
        buff = a_pBuffer + nLen;
    }

    // Add remaining data to the last slice
    buff = a_pBuffer + a_nBufLen;
    int remainingSize = static_cast<size_t>(buff - prev);
    if (remainingSize > 0) {
        CSlice slice(prev, remainingSize);
        a_rSlices.push_back(slice);
    }
}

int CVideoFrameDecoder::ParseForChromaSlice(uint8_t * a_pBuffer, size_t a_nBufLen)
{
    unsigned char* buff = a_pBuffer;
    const size_t nBufLen = a_nBufLen - (a_nBufLen % 4);
    size_t nLen = 0;

    // Look for code word 0x000001a2 on 4 byte boundary
    while (nLen < nBufLen) {
        if (*(buff) == 0) {
            if (*(buff + 1) == 0) {
                if (*((buff) + 2) == 0x01) {
                    if (*((buff) + 3) == 0xa2) {
                        return nLen;
                    }
                }
            }
        }
        nLen += 4;
        buff = a_pBuffer + nLen;
    }
    return 0;
}

