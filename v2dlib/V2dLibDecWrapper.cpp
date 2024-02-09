#include "V2dLibDecWrapper.h"
#include "v2dlib_compatibility.h"
#include "vframedecoder.hh"

struct v2dLibWrapper_t {
    CVideoFrameDecoder *pFrameDecoder;
    unsigned char* pDecodedBuffer;
    int nDecodeBufferLen;
    int nEncodedFormat;
    int nWidth;
    int nHeight;
    int nSliceSize;
    bool bDoubleBuffer;
};


v2dLibWrapper_t * createV2dLibDecObj(int a_nEncodedFormat, int a_nWidth, 
                                     int a_nHeight, int a_nSliceSize)
{
    if (a_nWidth <= 0 || a_nHeight <= 0)
        return NULL;

    v2dLibWrapper_t *pV2dLibWrapper = new v2dLibWrapper_t;
    if (pV2dLibWrapper == NULL)
        return NULL;

    pV2dLibWrapper->pDecodedBuffer = NULL;
    pV2dLibWrapper->bDoubleBuffer = true;

    eEncodeFormat eFormat;
    switch(a_nEncodedFormat)
    {
    case IPVS_ENCODE_TYPE_YUV422P:
        eFormat = eYUV422P;
        break;

    case IPVS_ENCODE_TYPE_YUV444S:
        eFormat = eYUV444S;
        break;

    case IPVS_ENCODE_TYPE_YUV444D:
        eFormat = eYUV444D;
        break;

    case IPVS_ENCODE_TYPE_SVC:
        eFormat = eSVC;
        break;
    }

    ePixelFormat eDecodePixelFormat = eBGR24;

    v2dDecConfiguration stDecConfig;

    stDecConfig.m_eEncodeFormat = eFormat;
    stDecConfig.m_nWidth = a_nWidth;
    stDecConfig.m_nHeight = a_nHeight;
    stDecConfig.m_nSliceSize = a_nSliceSize;
    stDecConfig.m_eDecodeFormat = eDecodePixelFormat;

    pV2dLibWrapper->pFrameDecoder = 
                                CVideoFrameDecoder::CreateObj(&stDecConfig);

    if (pV2dLibWrapper->pFrameDecoder == NULL)
        return NULL;

    pV2dLibWrapper->nDecodeBufferLen = stDecConfig.m_nDecodeBufLen;

    if (pV2dLibWrapper->bDoubleBuffer == true)
    {
        pV2dLibWrapper->pFrameDecoder->SetDoubleBuffering(true);
    }
    else
    {
        pV2dLibWrapper->pDecodedBuffer = 
                        new unsigned char[pV2dLibWrapper->nDecodeBufferLen];
    }

    return pV2dLibWrapper;
}

void destroyV2dLibDecObj(v2dLibWrapper_t *a_pV2dLibWrapper)
{
    if (a_pV2dLibWrapper != NULL)
    {
        if (a_pV2dLibWrapper->pFrameDecoder != NULL)
        {
            delete a_pV2dLibWrapper->pFrameDecoder;
            a_pV2dLibWrapper->pFrameDecoder = NULL;
        }

        if (a_pV2dLibWrapper->pDecodedBuffer != NULL)
        {
            delete [] a_pV2dLibWrapper->pDecodedBuffer;
            a_pV2dLibWrapper->pDecodedBuffer = NULL;
        }

        delete a_pV2dLibWrapper;
        a_pV2dLibWrapper = NULL;
    }
}

int decodeFrame(v2dLibWrapper_t *a_pV2dLibWrapper, unsigned char *a_pInBuffer,
                int a_nInLen, unsigned char* a_pOutBuffer)
{
    int nRetVal = -1;

    if (a_pV2dLibWrapper->pFrameDecoder == NULL)
        return nRetVal;

    if (a_pV2dLibWrapper->bDoubleBuffer == false)
    {
        if (a_pV2dLibWrapper->pDecodedBuffer == NULL)
            return nRetVal;

        nRetVal = a_pV2dLibWrapper->pFrameDecoder->Decode(a_pInBuffer, a_nInLen,
                                                    a_pV2dLibWrapper->pDecodedBuffer,
                                                    a_pV2dLibWrapper->nDecodeBufferLen);

        ::memcpy(a_pOutBuffer, a_pV2dLibWrapper->pDecodedBuffer,
                 a_pV2dLibWrapper->nDecodeBufferLen);

        if (a_pV2dLibWrapper->pFrameDecoder->GetSliceDebug() == false)
        {
            if (a_pV2dLibWrapper->pFrameDecoder->GetDoubleBuffering() == false)
            {
                a_pV2dLibWrapper->pFrameDecoder->SetDoubleBuffering(true);
            }
            else
            {
                a_pV2dLibWrapper->bDoubleBuffer = true;
            }
        }
    }
    else
    {
         nRetVal = a_pV2dLibWrapper->pFrameDecoder->Decode(a_pInBuffer,
                                                           a_nInLen,
                                                           a_pOutBuffer,
                                                           a_pV2dLibWrapper->nDecodeBufferLen);
    }

    return nRetVal;
}

void setSliceDebug(v2dLibWrapper_t *a_pV2dLibWrapper, int a_bEnableSliceDebug)
{
    if (a_pV2dLibWrapper == NULL)
        return;

    //The frame decoder does not support both SliceDebug and DoubleBuffer at the same time.
    //In SliceDebug mode we disable DoubleBuffer and use an extra local buffer into which we
    //do a memcopy of the frame. Also we have to ensure that we wait for an extra frame to be 
    //decoded before we enable DoubleBuffer again.
    if (a_bEnableSliceDebug == 1)
    {
        a_pV2dLibWrapper->pFrameDecoder->SetSliceDebug(true);
        a_pV2dLibWrapper->pFrameDecoder->SetDoubleBuffering(false);

        if (a_pV2dLibWrapper->pDecodedBuffer == NULL)
        {
            a_pV2dLibWrapper->pDecodedBuffer = 
                        new unsigned char[a_pV2dLibWrapper->nDecodeBufferLen];
        }
        a_pV2dLibWrapper->bDoubleBuffer = false;
    }
    else
    {
        a_pV2dLibWrapper->pFrameDecoder->SetSliceDebug(false);
    }
}