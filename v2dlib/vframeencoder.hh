/*
 * vframeencoder.hh
 *
 *  Created on: Oct 30, 2009
 *      Author: rkale
 */

#ifndef VFRAMEENCODER_HH_
#define VFRAMEENCODER_HH_

#include <list>
#include "codecdefs.h"
#include "geometry.hh"

/*********************************************************
 * Macro to convert between compression and quant values *
 *********************************************************/
#define mquant_from_compression(value)  ((value)==0 ? 0: \
                                        ((value)<=2 ? 1: \
                                        ((value)<=5 ? 2: \
                                        ((value)<=7 ? 3: \
                                        ((value)==8 ? 4: \
                                        ((value)==9 ? 5: 6))))))
#define dcquant_from_compression(value) ((value)<=1 ? 0: \
                                        ((value)==2 ? 1: \
                                        ((value)==3 ? 0: \
                                        ((value)==4 ? 1: \
                                        ((value)<=6 ? 2: 3)))))

class CSliceEncoder;

class CEncodedSlice;
class IEncoderMutex;

class  DLLEXPORT CVideoFrameEncoder {
public:

    CVideoFrameEncoder(int a_nWidth, int a_nHeight,
            int a_nQuant = 3, int a_DCQuant = 3,
            eEncodeFormat a_eEncodeFormat = eYUV422P,
            ePixelFormat a_ePixelFormat = eBGRX32);

    ~CVideoFrameEncoder();

    int EncodeFrame(unsigned char * a_outBuffer, unsigned char* a_bitmapArray);

    int EncodeFrame(unsigned char * a_outBuffer);
    void EncodeFrame();
    int GetNextFrame(unsigned char * a_outBuffer);

    void SetBitmap(unsigned char* a_bitmapArray);
    void AllocateBitmap();
    uint8_t * GetBitmap();
    void SetTestable(bool a_value);
    void SetSliceSize(int aSliceWidth, int aSliceHeight);
    void SetEncodingType(eSVCEncodingType a_eEncodingType);
    void SetEncodingSubType(eSVCEncodingSubType a_eEncodingSubType);
    void SetLayerDimension(int aLayerDimension);

    void UpdatePixel(int x, int y);
    void UpdateRectangle(int x, int y, int width, int height);
    void UpdateFrameFromBuffer(uint8_t *buffer);
    bool CompareAndCopyRegion(uint8_t *srcBuffer, uint8_t *dstBuffer,
            int a_nWidth, int a_nHeight, int a_nStride);
    void ClearUpdates();
    void UpdateAll();
    void SetSliceRefresh(int a_nSliceRfresh);
    void SetQuants(int a_nMQuant, int a_nDCQuant);
    void SetFSRefresh();
    void SetLeftRightFlag(int a_lrflag);
    void SetMutex(IEncoderMutex *a_pMutex);
    void SetQuantizationParameter(int a_qp);

    int GetFrameWidth() {
        return m_nFrameWidth;
    }
    int GetFrameHeight() {
        return m_nFrameHeight;
    }
    int GetSliceSize() {
        return m_nSliceSize;
    }
    V2DLib::Dimension GetEncodedDimension();

    uint64_t GetEncodedFrameCount() {
        return m_nEncodedFrameCount;
    }
    uint64_t GetServedFrameCount() {
        return m_nServedFrameCount;
    }
    uint64_t GetEncodedSliceCount() {
        return m_nEncodedSlices;
    }
    uint64_t GetEncodedOutBytes() {
        return m_nOutputBytes;
    }
    uint64_t GetEncodedInBytes() {
        return m_nInputBytes;
    }
    enum eEncodeFormat GetEncodeFormat() {
        return m_eEncodeFormat;
    }

private:

    int m_nFrameWidth;
    int m_nFrameHeight;
    int m_nEncodedWidth;
    int m_nEncodedHeight;
    int m_nSliceSize;
    int m_nSliceWidth;
    int m_nSliceHeight;
    int m_nSVCSliceWidth;
    int m_nSVCSliceHeight;
    int m_nBytesPerPixel;
    int m_nMQuant;
    int m_nDCQuant;
    int m_nSlicesPerFrame;
    bool *m_bDirtySlices;
    int *m_nSliceSplitArray;
    V2DLib::Dimension *m_pSliceDimensionsArray;
    int m_nRefreshSliceRate;
    uint64_t m_nEncodedFrameCount;
    uint64_t m_nServedFrameCount;
    uint64_t m_nEncodedSlices;
    uint64_t m_nOutputBytes;
    uint64_t m_nInputBytes;
    int m_nRefreshPointer;
    bool m_bFSRefresh;
    bool m_bTestable;
    uint8_t *m_pBitmapArray;
    uint8_t *m_pInternalBitmapArray;
    CSliceEncoder *m_pSliceEncoder;
    IEncoderMutex *m_pMutex;
    CEncodedSlice **m_Slices;
    enum eEncodeFormat m_eEncodeFormat;
    enum ePixelFormat m_ePixelFormat;
    bool m_x;

    inline int SliceBytes() const;
    void ComputeBytesPerPixel();
    void ComputeSliceSize();
    void ComputeSliceSplits();
    void CreateSliceEncoder();
    void CreateSliceArrays();
    void DestroySliceArrays();
    int BitMapOffset(int a_nSliceNumber);
    void MergeSlicesIntoFrame(std::list <CEncodedSlice *> & a_SliceList);
    bool CanServeSlice(int a_nSliceNumber, bool & bIsRefresh);
    void ResetForNextFrame();
    int AddSVCFrameHeader(unsigned char * a_outBuffer);

    CVideoFrameEncoder();
    CVideoFrameEncoder(const CVideoFrameEncoder& rhs);
    const CVideoFrameEncoder& operator=(const CVideoFrameEncoder& rhs);
};

class CEncodedSlice
{
public:
    CEncodedSlice(int a_nSliceNumber);
    ~CEncodedSlice();

    unsigned char *GetBuffer() {
        return m_pData;
    }

    int GetLength() {
        return m_nLen;
    }

    void SetLength(int a_nLen) {
        m_nLen = a_nLen;
    }

    int GetSliceNumber() {
        return m_nSliceNumber;
    }

    void SetNew() {
        m_bNewSlice = true;
    }

    void SetOld() {
        m_bNewSlice = false;
    }

    bool IsNew() {
        return m_bNewSlice;
    }

private:
    unsigned char* m_pData;
    int m_nLen;
    int m_nSliceNumber;
    bool m_bNewSlice;

    CEncodedSlice();
    CEncodedSlice(const CEncodedSlice& rhs);
    const CEncodedSlice& operator=(const CEncodedSlice& rhs);
};

class IEncoderMutex
{
public:
    virtual ~IEncoderMutex() {};
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

class CEncoderNullMutex : public IEncoderMutex
{
public:
    void Lock() {};
    void Unlock() {};

};

#endif /* VFRAMEENCODER_HH_ */
