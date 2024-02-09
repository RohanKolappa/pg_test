/*
 * vframeencoder.cpp
 *
 *  Created on: Oct 30, 2009
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include <math.h>
#include <list>

#include "vframeencoder.hh"
#include "sliceencoder.hh"
#include "svcsliceencoder.hh"
#include "aligned_malloc.h"
#include "memory_tracker.h"
#include "imagemanip.h"

using namespace V2DLib;

//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define PRINTF printf
#else
#define PRINTF(...) do {} while (0)
#endif

CVideoFrameEncoder::CVideoFrameEncoder(int a_nWidth, int a_nHeight,
        int a_nMQuant, int a_nDCQuant, eEncodeFormat a_eEncodeFormat,
        ePixelFormat a_ePixelFormat)
{
    m_nFrameWidth = a_nWidth;
    m_nFrameHeight = a_nHeight;
    m_nMQuant = a_nMQuant;
    m_nDCQuant = a_nDCQuant;
    m_nEncodedFrameCount = 0;
    m_nServedFrameCount = 0;
    m_pBitmapArray = NULL;
    m_pInternalBitmapArray = NULL;
    m_nEncodedSlices = 0;
    m_nOutputBytes = 0;
    m_nInputBytes = 0;
    m_nSliceSplitArray = NULL;
    m_pSliceDimensionsArray = NULL;
    m_nBytesPerPixel = 4;

    m_nRefreshSliceRate = 5; // Default 5 slices per frame
    m_nRefreshPointer = 0;
    m_bFSRefresh = false;
    m_bTestable = false;

    m_pSliceEncoder = NULL;
    m_eEncodeFormat = a_eEncodeFormat;
    m_ePixelFormat = a_ePixelFormat;
    m_nSVCSliceWidth = SVC_SLICE_WIDTH;
    m_nSVCSliceHeight = SVC_SLICE_HEIGHT;


    // Should really use templates here...
    m_pMutex = new CEncoderNullMutex();


    ComputeBytesPerPixel();
    ComputeSliceSize();
    ComputeSliceSplits();
    CreateSliceArrays();
    CreateSliceEncoder();
}

CVideoFrameEncoder::~CVideoFrameEncoder()
{
    delete m_pSliceEncoder;

    delete [] m_nSliceSplitArray;
    delete [] m_pSliceDimensionsArray;

    DestroySliceArrays();

    delete m_pMutex;
    aligned_free(m_pInternalBitmapArray);
}


void CVideoFrameEncoder::SetTestable(bool a_value)
{
    m_bTestable = a_value;
    m_pSliceEncoder->SetTestable(a_value);
}

void CVideoFrameEncoder::CreateSliceEncoder()
{
    eSVCEncodingType encodingType = ScalableEncoding;
    eSVCEncodingSubType encodingSubType = ScalableStandard;
    int layerDimension = 4;
    bool reapplySVCParameters = false;

    // Retrieve SVC parameters so that we can apply them to the new slice encoder
    // we are about to create.
    if (m_pSliceEncoder != NULL && m_eEncodeFormat == eSVC) {
        CSVCSliceEncoder *pSliceEncoder = dynamic_cast<CSVCSliceEncoder *>(m_pSliceEncoder);
        if (pSliceEncoder != NULL) {
            reapplySVCParameters = true;
            encodingType = pSliceEncoder->GetEncodingType();
            encodingSubType = pSliceEncoder->GetEncodingSubType();
            layerDimension = pSliceEncoder->GetLayerDimension();
        }
    }


    delete m_pSliceEncoder;
    if (m_eEncodeFormat == eH264) {
        m_pSliceEncoder = CSliceEncoder::CreateSliceEncoder(m_nMQuant, m_nDCQuant);
    }
    else if (m_eEncodeFormat == eSVC) {
        m_pSliceEncoder = CSliceEncoder::CreateSliceEncoder(m_nMQuant, m_nDCQuant,
                m_nSliceWidth, m_nSliceHeight);
    }
    else {
        m_pSliceEncoder = CSliceEncoder::CreateSliceEncoder(m_nMQuant, m_nDCQuant, m_nSliceSize);
    }

    m_pSliceEncoder->SetBytesPerPixel(m_nBytesPerPixel);
    m_pSliceEncoder->SetTestable(m_bTestable);

    if (reapplySVCParameters) {
        CSVCSliceEncoder *pSliceEncoder = dynamic_cast<CSVCSliceEncoder *>(m_pSliceEncoder);
        if (pSliceEncoder != NULL) {
            pSliceEncoder->SetEncodingType(encodingType);
            pSliceEncoder->SetEncodingSubType(encodingSubType);
            pSliceEncoder->SetLayerDimension(layerDimension);
        }
    }

}

void CVideoFrameEncoder::SetQuants(int a_nMQuant, int a_nDCQuant)
{
    if (a_nMQuant != m_nMQuant || a_nDCQuant != m_nDCQuant) {
        m_nMQuant = a_nMQuant;
        m_nDCQuant = a_nDCQuant;
        CreateSliceEncoder();
        UpdateAll();
    }
}

void CVideoFrameEncoder::SetEncodingType(eSVCEncodingType a_eEncodingType)
{
    if (m_eEncodeFormat == eSVC) {
        CSVCSliceEncoder *pSliceEncoder = dynamic_cast<CSVCSliceEncoder *>(m_pSliceEncoder);
        if (pSliceEncoder != NULL) {
            pSliceEncoder->SetEncodingType(a_eEncodingType);
        }
    }
}

void CVideoFrameEncoder::SetEncodingSubType(eSVCEncodingSubType a_eEncodingSubType)
{
    if (m_eEncodeFormat == eSVC) {
        CSVCSliceEncoder *pSliceEncoder = dynamic_cast<CSVCSliceEncoder *>(m_pSliceEncoder);
        if (pSliceEncoder != NULL) {
            pSliceEncoder->SetEncodingSubType(a_eEncodingSubType);
        }
    }
}

void CVideoFrameEncoder::SetLayerDimension(int aLayerDimension)
{
    if (m_eEncodeFormat == eSVC) {
        CSVCSliceEncoder *pSliceEncoder = dynamic_cast<CSVCSliceEncoder *>(m_pSliceEncoder);
        if (pSliceEncoder != NULL) {
            pSliceEncoder->SetLayerDimension(aLayerDimension);
        }
    }
}

void CVideoFrameEncoder::SetQuantizationParameter(int a_qp) {
    m_pSliceEncoder->SetQuantizationParameter(a_qp);
}

// TODO: Wrap this around the base EncodeFrame() function
int CVideoFrameEncoder::EncodeFrame(unsigned char * a_outBuffer,
        unsigned char* a_bitmapArray)
{
    int sliceNo = 0;
    int nLength = 0;

    nLength += AddSVCFrameHeader(a_outBuffer + nLength);

    for (sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
        int encodeLength = m_pSliceEncoder->EncodeSlice(a_outBuffer + nLength,
                a_bitmapArray + BitMapOffset(sliceNo), m_nFrameWidth,
                sliceNo, m_nSliceSplitArray[sliceNo]);
        nLength += encodeLength;
        m_nEncodedSlices++;
        m_nOutputBytes += encodeLength;
        m_nInputBytes += SliceBytes();
    }
    int encodeLength = m_pSliceEncoder->AddEndOfFrame(a_outBuffer + nLength);
    nLength += encodeLength;

    m_nEncodedFrameCount++;

    return nLength;
}


// Encode only Updated (dirty) slices in a previously configured Bitmap
// TODO: Wrap this around the base EncodeFrame() function
int CVideoFrameEncoder::EncodeFrame(unsigned char * a_outBuffer)
{
    if (m_pBitmapArray == NULL)
        return 0;

    int nEncodeLength;
    int nTotalLength = 0;

    nTotalLength += AddSVCFrameHeader(a_outBuffer + nTotalLength);

    for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
        if (m_bDirtySlices[sliceNo] || sliceNo == 0) {
            nEncodeLength = m_pSliceEncoder->EncodeSlice(a_outBuffer + nTotalLength,
                    m_pBitmapArray + BitMapOffset(sliceNo), m_nFrameWidth,
                    sliceNo, m_nSliceSplitArray[sliceNo]);
            nTotalLength += nEncodeLength;
            m_nEncodedSlices++;
            m_nOutputBytes += nEncodeLength;
            m_nInputBytes += SliceBytes();
        }
    }
    nEncodeLength = m_pSliceEncoder->AddEndOfFrame(a_outBuffer + nTotalLength);
    nTotalLength += nEncodeLength;

    ClearUpdates();

    m_nEncodedFrameCount++;

    return nTotalLength;
}

// Encode only updated (dirty) slices in a previously configured Bitmap
void CVideoFrameEncoder::EncodeFrame()
{
    if (m_pBitmapArray == NULL)
        return;

    std::list<CEncodedSlice *> SliceList;

    // Encode all dirty slices (also slice 0 if never done yet)
    // into a list of new slices
    for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
        if (m_bDirtySlices[sliceNo]) {

            CEncodedSlice *slice = new CEncodedSlice(sliceNo);

            int nEncodeLength = m_pSliceEncoder->EncodeSlice(slice->GetBuffer(),
                    m_pBitmapArray + BitMapOffset(sliceNo), m_nFrameWidth,
                    sliceNo, m_nSliceSplitArray[sliceNo]);
            slice->SetLength(nEncodeLength);
            m_nEncodedSlices++;
            m_nOutputBytes += nEncodeLength;
            m_nInputBytes += SliceBytes();
            SliceList.push_back(slice);
        }
    }

    m_nEncodedFrameCount++;

    // Now merge in the encoded slices into the current frame
    // This is done separately so that we only need to acquire mutex locks
    // for the briefest moment possible.
    MergeSlicesIntoFrame(SliceList);

    ClearUpdates();

}

int CVideoFrameEncoder::AddSVCFrameHeader(unsigned char * a_outBuffer) {

    if (m_eEncodeFormat != eSVC)
        return 0;

    SVCFrameHeader frameHeader;
    frameHeader.nFtimestamp = 512; // set some time stamp for now
    frameHeader.bIFrame = 1;
    frameHeader.bFFrame = 1;
    frameHeader.bIsStereo = 0;
    frameHeader.bFrameDimensions = 1;
    frameHeader.nFrameWidth = m_nFrameWidth;
    frameHeader.nFrameHeight = m_nFrameHeight;
    frameHeader.nEncodedOffsetX = 0;
    frameHeader.nEncodedOffsetY = 0;
    frameHeader.nEncodedWidth = m_nEncodedWidth;
    frameHeader.nEncodedHeight = m_nEncodedHeight;
    frameHeader.nSliceWidth = m_nSliceWidth;
    frameHeader.nSliceHeight = m_nSliceHeight;
    frameHeader.bChromaSamplingMode = 1;

    return m_pSliceEncoder->AddFrameHeader(a_outBuffer, frameHeader);

}

// Get the next frame from the encoder
int CVideoFrameEncoder::GetNextFrame(unsigned char * a_outBuffer)
{
    m_pMutex->Lock();

    // If no frames encoded yet, return nothing
    if (m_nEncodedFrameCount == 0) {
        m_pMutex->Unlock();
        return 0;
    }

    int nTotalLength = 0;
    bool bIsRefresh;

    nTotalLength += AddSVCFrameHeader(a_outBuffer + nTotalLength);

    for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {

        if (CanServeSlice(sliceNo, bIsRefresh)) {

            ::memcpy(a_outBuffer + nTotalLength,
                    m_Slices[sliceNo]->GetBuffer(),
                    m_Slices[sliceNo]->GetLength());

            if (bIsRefresh) {
                m_pSliceEncoder->SetRefreshSliceBit(a_outBuffer + nTotalLength);
            }
            nTotalLength += m_Slices[sliceNo]->GetLength();
        }
    }

    int nEncodeLength = m_pSliceEncoder->AddEndOfFrame(a_outBuffer + nTotalLength);
    nTotalLength += nEncodeLength;

    m_nServedFrameCount++;

    ResetForNextFrame();

    m_pMutex->Unlock();

    return nTotalLength;
}

void CVideoFrameEncoder::SetBitmap(unsigned char* a_bitmapArray) {
    m_pBitmapArray = a_bitmapArray;
}

void CVideoFrameEncoder::AllocateBitmap() {
    aligned_free(m_pInternalBitmapArray);
    int bufferSize = GetEncodedDimension().width *
            GetEncodedDimension().height * m_nBytesPerPixel;
    m_pInternalBitmapArray = (uint8_t *)aligned_malloc(bufferSize, DEFAULT_ALIGNMENT);
    memset(m_pInternalBitmapArray, 0x00, bufferSize);
    m_pBitmapArray = m_pInternalBitmapArray;
}

uint8_t *CVideoFrameEncoder::GetBitmap() {
    return m_pBitmapArray;
}

void CVideoFrameEncoder::SetSliceRefresh(int a_nSliceRfresh)
{
    m_nRefreshSliceRate = a_nSliceRfresh;
}

void CVideoFrameEncoder::SetFSRefresh()
{
    m_bFSRefresh = true;
}

void CVideoFrameEncoder::SetLeftRightFlag(int a_lrflag)
{
    m_pSliceEncoder->SetLeftRightFlag(a_lrflag);
}

void CVideoFrameEncoder::SetMutex(IEncoderMutex *a_pMutex)
{
    m_pMutex = a_pMutex;
}

// Publish this slice in the next frame??
bool CVideoFrameEncoder::CanServeSlice(int a_nSliceNumber, bool & bIsRefresh)
{
    CEncodedSlice * pSlice = m_Slices[a_nSliceNumber];
    bIsRefresh = false;

    // If nothing at slot return false
    if (pSlice == NULL)
        return false;

    // Slice zero is always part of a frame (unless SVC mode)
    if (m_eEncodeFormat != eSVC && a_nSliceNumber == 0)
        return true;

    // A fresh slice is always to be served
    if (pSlice->IsNew())
        return true;

    // Also serve refresh slices in a round robin fashion
    int nMappedSliceNo =  a_nSliceNumber;
    if (nMappedSliceNo < m_nRefreshPointer)
        nMappedSliceNo += m_nSlicesPerFrame;
    if ((nMappedSliceNo >= m_nRefreshPointer) &&
            (nMappedSliceNo < m_nRefreshPointer + m_nRefreshSliceRate)) {
        bIsRefresh = true;
        return true;
    }

    // If Full Screen Refresh requested, send out all slices
    if (m_bFSRefresh)
        return true;

    return false;
}

void CVideoFrameEncoder::ResetForNextFrame()
{
    // Mark all slices as already served
    for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
        if (m_Slices[sliceNo] != NULL)
           m_Slices[sliceNo]->SetOld();
    }

    // Update Refresh Slice pointer
    m_nRefreshPointer += m_nRefreshSliceRate;
    m_nRefreshPointer = m_nRefreshPointer % m_nSlicesPerFrame;

    // Clear Full Screen Refresh flag
    m_bFSRefresh = false;
}

void CVideoFrameEncoder::MergeSlicesIntoFrame(std::list <CEncodedSlice *> & a_SliceList)
{

    m_pMutex->Lock();

    while (!a_SliceList.empty()) {

        CEncodedSlice * pSlice = a_SliceList.front();
        a_SliceList.pop_front();

        int nSliceNumber = pSlice->GetSliceNumber();

        // Delete previous slice at slot
        if (m_Slices[nSliceNumber] != NULL) {
            delete m_Slices[nSliceNumber];
        }

        // Mark slice as new
        pSlice->SetNew();

        // Put new slice into slot
        m_Slices[nSliceNumber] = pSlice;

    }
    m_pMutex->Unlock();

}

int CVideoFrameEncoder::SliceBytes() const
{
    return m_nSliceSize * m_nBytesPerPixel * 64;
}

void CVideoFrameEncoder::ComputeBytesPerPixel()
{
    if (m_ePixelFormat == eBGR24) {
        m_nBytesPerPixel = 3;
    }
    else if (m_ePixelFormat == eRGB24) {
        m_nBytesPerPixel = 3;
    }
    else {
        m_nBytesPerPixel = 4;
    }
}

void CVideoFrameEncoder::SetSliceSize(int aSliceWidth, int aSliceHeight)
{

    if (m_eEncodeFormat == eSVC) {
        m_nSVCSliceWidth = aSliceWidth;
        m_nSVCSliceHeight = aSliceHeight;

        DestroySliceArrays();
        ComputeBytesPerPixel();
        ComputeSliceSize();
        ComputeSliceSplits();
        CreateSliceArrays();
        CreateSliceEncoder();
    }
}

void CVideoFrameEncoder::ComputeSliceSize()
{
    if (m_eEncodeFormat == eH264) {
        m_nEncodedWidth = (int)ceil(m_nFrameWidth/16.0) * 16;
        m_nEncodedHeight = (int)ceil(m_nFrameHeight/16.0) * 16;
        m_nSliceSize =  8 * 4;
        PRINTF("Width %d Height %d EWidth %d EHeight %d\n",
                m_nFrameWidth, m_nFrameHeight, m_nEncodedWidth, m_nEncodedHeight);

        int nMacroBlocksPerRow = m_nEncodedWidth / 16;
        PRINTF("Macroblocks per row = %d\n", nMacroBlocksPerRow);
        int nSlicesPerRow = nMacroBlocksPerRow / H264_SLICE_WIDTH;
        PRINTF("Slices per row = %d\n", nSlicesPerRow);
        int nNumberOfRows = m_nEncodedHeight / (16 * H264_SLICE_HEIGHT);
        m_nSlicesPerFrame = nSlicesPerRow * nNumberOfRows;
        PRINTF("Slices per frame = %d\n", m_nSlicesPerFrame);

    }

    else if (m_eEncodeFormat == eSVC) {
        m_nSliceWidth = m_nSVCSliceWidth;
        m_nSliceHeight = m_nSVCSliceHeight;
        m_nSliceSize = m_nSliceWidth * m_nSliceHeight;
        m_nEncodedWidth = (int)ceil(m_nFrameWidth/16.0) * 16;
        m_nEncodedHeight = (int)ceil(m_nFrameHeight/8.0) * 8;
        while (m_nEncodedWidth % (m_nSliceWidth * 8) != 0)
            m_nEncodedWidth += 2;
        while (m_nEncodedHeight % (m_nSliceHeight * 8) != 0)
            m_nEncodedHeight += 2;
        PRINTF("Width %d Height %d EWidth %d EHeight %d\n",
                m_nFrameWidth, m_nFrameHeight, m_nEncodedWidth, m_nEncodedHeight);
        PRINTF("SliceWidth %d SliceHeight %d\n",
                m_nSliceWidth, m_nSliceHeight);

        int nBlocksPerRow = m_nEncodedWidth / 8;
        int nSlicesPerRow = nBlocksPerRow / m_nSliceWidth;
        PRINTF("Slices per row = %d\n", nSlicesPerRow);
        int nNumberOfRows = m_nEncodedHeight / (8 * m_nSliceHeight);
        PRINTF("Number of Rows = %d\n", nNumberOfRows);
        m_nSlicesPerFrame = nSlicesPerRow * nNumberOfRows;
        PRINTF("Slices per frame = %d\n", m_nSlicesPerFrame);
    }
    else {
        m_nSliceSize = 32;
        V2DLib::computeSliceSize(m_nFrameWidth, m_nFrameHeight, m_nSliceSize,
                m_nEncodedWidth, m_nEncodedHeight, m_nSlicesPerFrame);
        PRINTF("SliceSize %d\n", m_nSliceSize);

    }
}

V2DLib::Dimension CVideoFrameEncoder::GetEncodedDimension()
{
    return Dimension(m_nEncodedWidth, m_nEncodedHeight);
}



// Pre-compute pixel offsets where each slice splits if it spans
// two rows. If the slice does not split the value is 0
void CVideoFrameEncoder::ComputeSliceSplits()
{
    if (m_eEncodeFormat == eH264) {
        m_nSliceSplitArray = new int [m_nSlicesPerFrame];
        for (int nSliceNumber = 0; nSliceNumber < m_nSlicesPerFrame; nSliceNumber++) {
            m_nSliceSplitArray[nSliceNumber] = 0;
        }
    }
    else if (m_eEncodeFormat == eSVC) {
        delete [] m_nSliceSplitArray;
        m_nSliceSplitArray = new int [m_nSlicesPerFrame];
        delete [] m_pSliceDimensionsArray;
        m_pSliceDimensionsArray = new Dimension [m_nSlicesPerFrame];
        int nBlocksPerRow = m_nEncodedWidth / 8;
        int nSlicesPerRow = nBlocksPerRow / m_nSliceWidth;

        for (int nSliceNumber = 0; nSliceNumber < m_nSlicesPerFrame; nSliceNumber++) {
            m_nSliceSplitArray[nSliceNumber] = 0;
            int nRowNumber = nSliceNumber/nSlicesPerRow;
            int nColumnNumber = nSliceNumber % nSlicesPerRow;
            m_pSliceDimensionsArray[nSliceNumber].width = m_nSliceWidth * 8;
            m_pSliceDimensionsArray[nSliceNumber].height = m_nSliceHeight * 8;
            if ((nColumnNumber + 1) * m_nSliceWidth * 8 > m_nFrameWidth) {
                m_pSliceDimensionsArray[nSliceNumber].width -= m_nEncodedWidth - m_nFrameWidth;
            }
            if ((nRowNumber + 1) * m_nSliceHeight * 8 > m_nFrameHeight) {
                m_pSliceDimensionsArray[nSliceNumber].height -= m_nEncodedHeight - m_nFrameHeight;
            }

        }
    }
    else {
        m_nSliceSplitArray = new int [m_nSlicesPerFrame];
        int nSplitBlocks = 0;
        int nBlocksPerRow = m_nFrameWidth / 8;
        for (int nSliceNumber = 0; nSliceNumber < m_nSlicesPerFrame; nSliceNumber++) {
            int nBlockNumber = nSliceNumber * m_nSliceSize;
            int nBlocksInThisRow = nBlockNumber % nBlocksPerRow;
            int nRemainingBlocksInThisRow = nBlocksPerRow - nBlocksInThisRow;
            if (nRemainingBlocksInThisRow >= m_nSliceSize)
                nSplitBlocks = 0;
            else
                nSplitBlocks = m_nSliceSize - (m_nSliceSize - nRemainingBlocksInThisRow);
            m_nSliceSplitArray[nSliceNumber] = nSplitBlocks * 8;
        }
    }
}

void CVideoFrameEncoder::CreateSliceArrays()
{
    m_bDirtySlices = new bool [m_nSlicesPerFrame];
    UpdateAll();

    m_Slices = new CEncodedSlice * [m_nSlicesPerFrame];

    for (int i = 0; i < m_nSlicesPerFrame; i++) {
        m_Slices[i] = NULL;
    }
}

void CVideoFrameEncoder::DestroySliceArrays()
{
    delete [] m_bDirtySlices;
    m_bDirtySlices = NULL;

    for (int i = 0; i < m_nSlicesPerFrame; i++) {
        delete m_Slices[i];
    }
    delete [] m_Slices;
    m_Slices = NULL;
}

int CVideoFrameEncoder::BitMapOffset(int a_nSliceNumber)
{
    if (m_eEncodeFormat == eH264) {
        int nMacroBlocksPerRow = m_nEncodedWidth / 16;
        int nSlicesPerRow = nMacroBlocksPerRow / H264_SLICE_WIDTH;
        int nRowNumber = a_nSliceNumber/nSlicesPerRow;
        int nColumnNumber = a_nSliceNumber % nSlicesPerRow;
        int nOffset = nRowNumber * m_nFrameWidth * 16 * H264_SLICE_HEIGHT * m_nBytesPerPixel +
                nColumnNumber * 16 * H264_SLICE_WIDTH * m_nBytesPerPixel;
        return nOffset;
    }
    else if (m_eEncodeFormat == eSVC) {
        int nBlocksPerRow = m_nEncodedWidth / 8;
        int nSlicesPerRow = nBlocksPerRow / m_nSliceWidth;
        int nRowNumber = a_nSliceNumber/nSlicesPerRow;
        int nColumnNumber = a_nSliceNumber % nSlicesPerRow;
        int nOffset = nRowNumber * m_nFrameWidth * 8 * m_nSliceHeight * m_nBytesPerPixel +
                nColumnNumber * 8 * m_nSliceWidth * m_nBytesPerPixel;
        return nOffset;
    }
    else {
        int nBlockNumber = m_nSliceSize * a_nSliceNumber;

        int nBlocksPerRow = m_nFrameWidth / 8;
        int nRowNumber = nBlockNumber/nBlocksPerRow;
        int nColumnNumber = nBlockNumber % nBlocksPerRow;
        int nOffset = nRowNumber * nBlocksPerRow * 64 * m_nBytesPerPixel +
                nColumnNumber * 8 * m_nBytesPerPixel;
        return nOffset;
    }
}

void CVideoFrameEncoder::ClearUpdates()
{
    for (int i = 0; i < m_nSlicesPerFrame; i++)
        m_bDirtySlices[i] = false;
}

void CVideoFrameEncoder::UpdateAll()
{
    for (int i = 0; i < m_nSlicesPerFrame; i++)
        m_bDirtySlices[i] = true;
}


void CVideoFrameEncoder::UpdatePixel(int a_nXPos, int a_nYPos)
{
    if ((a_nXPos < 0) || (a_nXPos >= m_nFrameWidth) ||
            (a_nYPos < 0) || (a_nYPos >= m_nFrameHeight)) {
        // Bad pixel value
        return;
    }

    if (m_eEncodeFormat == eH264) {

    }
    else if (m_eEncodeFormat == eSVC) {
        int nBlocksPerRow = m_nEncodedWidth / 8;
        int nRowNumber = (a_nYPos/8) / m_nSliceHeight;
        int nColumnNumber = (a_nXPos/8) / m_nSliceWidth;
        int nSlicesPerRow = nBlocksPerRow / m_nSliceWidth;
        int nSliceNumber = nRowNumber * nSlicesPerRow + nColumnNumber;
        if (nSliceNumber < m_nSlicesPerFrame)
            m_bDirtySlices[nSliceNumber] = true;

    }
    else {
        int nBlocksPerRow = m_nFrameWidth / 8;
        int nBlockNumber = (a_nYPos/8 * nBlocksPerRow) + a_nXPos/8;
        int nSliceNumber = nBlockNumber / m_nSliceSize;

        if (nSliceNumber < m_nSlicesPerFrame)
            m_bDirtySlices[nSliceNumber] = true;
    }
}

void CVideoFrameEncoder::UpdateRectangle(int x, int y, int width, int height)
{
    // Throw away meaningless input
    if ((x < 0) || (x >= m_nFrameWidth) ||
            (y < 0) || (y >= m_nFrameHeight) ||
            (width) == 0 || (height == 0) ) {
        return;
    }

    // Clip width and height to frame boundaries
    width = (x + width <= m_nFrameWidth) ? width : m_nFrameWidth - x;
    height = (y + height <= m_nFrameHeight) ? height : m_nFrameHeight - y;

    // Call UpdatePixel once for each block intersecting the rectangle
    for (int j = (y/8) * 8; j <= ((y + height - 1)/8) * 8 ; j += 8) {
        for (int i = (x/8) * 8; i <= ((x + width - 1)/8) * 8; i += 8) {
            UpdatePixel(i, j);
        }
    }
}

void CVideoFrameEncoder::UpdateFrameFromBuffer(uint8_t *buffer)
{
    // Frame Differencing: Copy in slices that have changed and mark them dirty
    if (m_eEncodeFormat == eH264) {
        for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
            if (CompareAndCopyRegion(buffer + BitMapOffset(sliceNo),
                    m_pBitmapArray + BitMapOffset(sliceNo),
                    H264_SLICE_WIDTH * 16,
                    H264_SLICE_HEIGHT * 16,
                    m_nFrameWidth)) {
                m_bDirtySlices[sliceNo] = true;
            }
        }
    }
    else if (m_eEncodeFormat == eSVC) {
        for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
            if (CompareAndCopyRegion(buffer + BitMapOffset(sliceNo),
                    m_pBitmapArray + BitMapOffset(sliceNo),
                    m_pSliceDimensionsArray[sliceNo].width,
                    m_pSliceDimensionsArray[sliceNo].height,
                    m_nFrameWidth)) {
                m_bDirtySlices[sliceNo] = true;
            }
        }
    }
    else if (m_eEncodeFormat == eYUV422P) {
        for (int sliceNo = 0; sliceNo < m_nSlicesPerFrame; sliceNo++) {
            if (m_nSliceSplitArray[sliceNo] == 0) {
                if (CompareAndCopyRegion(buffer + BitMapOffset(sliceNo),
                        m_pBitmapArray + BitMapOffset(sliceNo),
                        m_nSliceSize * 8, 8, m_nFrameWidth)) {
                    m_bDirtySlices[sliceNo] = true;
                }
            }
            else {
                // Handle slices that split
                if (CompareAndCopyRegion(buffer + BitMapOffset(sliceNo),
                        m_pBitmapArray + BitMapOffset(sliceNo),
                        m_nSliceSplitArray[sliceNo], 8, m_nFrameWidth)) {
                    m_bDirtySlices[sliceNo] = true;
                }
                int nSplitOffset = (m_nSliceSplitArray[sliceNo] * m_nBytesPerPixel) +
                        (m_nFrameWidth * 7 * m_nBytesPerPixel);
                if (CompareAndCopyRegion(buffer + BitMapOffset(sliceNo) + nSplitOffset,
                        m_pBitmapArray + BitMapOffset(sliceNo) + nSplitOffset,
                        ((m_nSliceSize * 8) - m_nSliceSplitArray[sliceNo]), 8, m_nFrameWidth)) {
                    m_bDirtySlices[sliceNo] = true;
                }
            }
        }
    }
}

// Compare source region to destination. If different, copy the slice and return true
bool CVideoFrameEncoder::CompareAndCopyRegion(uint8_t *srcBuffer, uint8_t *dstBuffer,
        int a_nWidth, int a_nHeight, int a_nStride) {

	uint8_t *srcPtr, *dstPtr;
	bool bDifferent = false;
	// Compare one scan line at a time and copy if different
	for (int j = 0; j < a_nHeight; j++) {
	    srcPtr = srcBuffer + j * a_nStride * m_nBytesPerPixel;
	    dstPtr = dstBuffer + j * a_nStride * m_nBytesPerPixel;
	    if (memcmp(dstPtr, srcPtr, a_nWidth * m_nBytesPerPixel) != 0) {
            memcpy(dstPtr, srcPtr, a_nWidth * m_nBytesPerPixel);
	        bDifferent = true;
	    }
	}
	return bDifferent;
}


// CEncodedSlice class

CEncodedSlice::CEncodedSlice(int a_nSliceNumber)
: m_pData(NULL)
, m_nLen(0)
, m_nSliceNumber(a_nSliceNumber)
, m_bNewSlice(false)
{
#ifdef USE_IPP
    m_pData = ippsMalloc_8u(1024 * 8);
#else
    m_pData = new unsigned char[1024 * 8];
    //memset(m_pData, 0x00, 1024 * 8); // TODO rkale: delete this
#endif
}

CEncodedSlice::~CEncodedSlice()
{
#ifdef USE_IPP
    ippsFree(m_pData);
#else
    delete [] m_pData;
#endif
}

