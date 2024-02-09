/*
 * v2ddecoderthr.cpp
 *
 *  Created on: Jul 8, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include "FFmpegHeaders.hh"

#include "v2ddecoderthr.hh"
#include "vframedecoder.hh"
#include "colortransform.h"
#include "common.hh"
#include "sframe.hh"
#include "squeue.hh"
#include "renderermessage.hh"
#include "command.hh"

//#define POST_RGB
//#define USE_RECTANGLES
using namespace V2dLibColorTransform;

CV2DDecoderThread::CV2DDecoderThread(int aId, CSQueueSink *qRendererSink,
        CSQueueSource *qRendererSource)
{
    m_qRendererSink = qRendererSink;
    m_qRendererSource = qRendererSource;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nScaledWidth = 0;
    m_nScaledHeight = 0;
    m_nScalingMode = SCALING_MODE_NATIVE;
    m_nSliceSize = 0;
    m_pDecodeBuffer = NULL;
    m_pFrameDecoder = NULL;
    m_nDecodeSize = 0;
    m_nScaledDecodeSize = 0;
    m_nDecodedFrames = 0;
    m_pY = NULL;
    m_pU = NULL;
    m_pV = NULL;
    m_nImageBufferSize = 0;
    m_pImageBuffer = NULL;
    m_inBuffer = NULL;
    m_inBufferLen = 0;
    m_bSendFrameInfo = false;
    m_bDecoderDebug = false;
    m_bSendRFBStatus = false;
    m_bRFBReady = false;
    m_sBuffer[0] = m_sBuffer[1] = NULL;
    m_nDoubleBufferIndex = -1;
    m_pSwsCtx = NULL;
    m_ePixelFormat = eBGRX32;

    m_pDestPictureArray[0] = NULL;
    m_pDestPictureArray[1] = NULL;
    m_pDestPictureArray[0] = avcodec_alloc_frame();
    m_pDestPictureArray[1] = avcodec_alloc_frame();

    if (m_pThreadName != NULL) {
        sprintf(m_pThreadName, "V2DDecoderThread_%d", aId);
    }

}

CV2DDecoderThread::~CV2DDecoderThread()
{
    delete[] m_pDecodeBuffer;
    delete[] m_pImageBuffer;
    delete m_pFrameDecoder;
    FreeSBuffers();

    if (m_pDestPictureArray[0] != NULL) {
        av_free(m_pDestPictureArray[0]);
    }
    if (m_pDestPictureArray[1] != NULL) {
        av_free(m_pDestPictureArray[1]);
    }
    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);

}

void CV2DDecoderThread::FreeSBuffers()
{
    if (m_sBuffer[0] != NULL)
        m_sBuffer[0]->RemoveReference();
    if (m_sBuffer[1] != NULL)
        m_sBuffer[1]->RemoveReference();
}

int CV2DDecoderThread::GetDoubleBufferIndex() {
    // Use ref count to determine which SBuffer downstream has used up and
    // made available for us to reuse
    if (m_sBuffer[0] == NULL || m_sBuffer[1] == NULL) {
        return -1;
    }
    if (m_sBuffer[0]->GetReferenceCount() == 1) {
        return 0;
    }
    else if (m_sBuffer[1]->GetReferenceCount() == 1) {
        return 1;
    }
    return -1;
}

int
CV2DDecoderThread::Run()
{
    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        if (DecodeProcessor() == -1) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            break;
        }
        LightSleep();
    }
    delete(this);
    return 0;
}

void CV2DDecoderThread::SetupDecoder(int width, int height, int pixelFormat, 
        int sliceSize, bool bIsSVC, eEncodeFormat encodeFormat)
{
    CScopedLock lock(m_mutex);

    delete[] m_pDecodeBuffer;
    delete[] m_pImageBuffer;
    delete m_pFrameDecoder;

    m_nWidth = width;
    m_nHeight = height;
    m_nSliceSize = sliceSize;
    m_ePixelFormat = (ePixelFormat) pixelFormat;
    SetupScaledDimensions();

    SYSLOG_INFO("Setting: up decoder %d %d %d ", width, height, sliceSize);

    v2dDecConfiguration decConfig;
    decConfig.m_nWidth = width;
    decConfig.m_nHeight = height;
    decConfig.m_nSliceSize = sliceSize;
    if (bIsSVC) {
        decConfig.m_eEncodeFormat = eSVC;
    }
    else {
        decConfig.m_eEncodeFormat = encodeFormat;
    }

#ifdef POST_RGB
    decConfig.m_eDecodeFormat = eYUV422P;
#else
//    decConfig.m_eDecodeFormat = (ePixelFormat) pixelFormat;
    decConfig.m_eDecodeFormat = eBGRX32;
#endif

    m_pFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);
    m_pFrameDecoder->SetScaleFactor(1);
    m_pFrameDecoder->SetSliceDebug(m_bDecoderDebug);

    m_nDecodeSize = m_pFrameDecoder->GetDecodeBufLen();
    SYSLOG_INFO("Setting up decoder size %d", m_nDecodeSize);

    SetupScaler();

    SetupDoubleBuffers();

    m_pDecodeBuffer = new unsigned char[m_nDecodeSize];
    m_pY = m_pDecodeBuffer;
    m_pU = m_pY + (width * height);
    m_pV = m_pU + (width * height) / 2;

    m_nImageBufferSize = m_pFrameDecoder->GetMaxRectanglesBufferLen();
    SYSLOG_INFO("Setting up ImageBuffer size %d", m_nImageBufferSize);

    m_pImageBuffer = new unsigned char[m_nImageBufferSize];

    // Schedule Frame info to be send
    m_bSendFrameInfo = true;
}


void CV2DDecoderThread::SetupScaledDimensions()
{
    if (m_nWidth <= 0 || m_nHeight <= 0)
        return;
    if (m_nScalingMode == SCALING_MODE_NATIVE) {
        SYSLOG_DEBUG("Scaler: Native Mode: %d %d\n", m_nWidth, m_nHeight);
        m_nScaledWidth = m_nWidth;
        m_nScaledHeight = m_nHeight;
    }
    else if (m_nScalingMode == SCALING_MODE_KEEP_ASPECT) {
        SYSLOG_DEBUG("Keep Aspect Ratio request %d %d\n", m_nScaledWidth, m_nScaledHeight);
        if (m_nScaledWidth <= 0 || m_nScaledHeight <= 0) {
            printf("Keep Aspect Ratio override\n");
            m_nScaledWidth = m_nWidth;
            m_nScaledHeight = m_nHeight;
        }
        else {
            double aspectRatio = ((double)m_nWidth)/m_nHeight;
            int targetHeight = (int)rint((double)m_nScaledWidth / aspectRatio);
            if (targetHeight <= m_nScaledHeight) {
                m_nScaledHeight = targetHeight;
                SYSLOG_DEBUG("Fitting to width %d, height set to %d\n", m_nScaledWidth,  m_nScaledHeight);
            }
            else {
                m_nScaledWidth = (int)rint((double)m_nScaledHeight * aspectRatio);
                SYSLOG_DEBUG("Fitting to height %d, width set to %d\n", m_nScaledHeight, m_nScaledWidth);
            }
        }
    }
    else if (m_nScalingMode == SCALING_MODE_FILL) {
        SYSLOG_DEBUG("Fill mode request %d %d\n", m_nScaledWidth, m_nScaledHeight);
        // Setup scaled dimensions only if target dimension have never been
        // initialized so far
        if (m_nScaledWidth <= 0 || m_nScaledHeight <= 0) {
            m_nScaledWidth = m_nWidth;
            m_nScaledHeight = m_nHeight;
        }
    }
}

void CV2DDecoderThread::SetupScaledDimensions(
        int aScalingMode, int aScaledWidth, int aScaledHeight)
{
    CScopedLock lock(m_mutex);
    SYSLOG_INFO("Got scaling change request mode = %d, scaling dim = %d x %d\n",
            aScalingMode, aScaledWidth, aScaledHeight);

    m_nScalingMode = aScalingMode;
    m_nScaledWidth = aScaledWidth;
    m_nScaledHeight = aScaledHeight;

    SetupScaledDimensions();
    SetupScaler();
    SetupDoubleBuffers();

    // Send updated frame info
    m_bSendFrameInfo = true;
}

void CV2DDecoderThread::SetupScaler()
{
    enum PixelFormat dst_pix_fmt;
    if (m_ePixelFormat == eRGB24)
        dst_pix_fmt = PIX_FMT_RGB24;
    else
        dst_pix_fmt = PIX_FMT_RGB32;  // BGRX on little-endian

    enum PixelFormat src_pix_fmt = PIX_FMT_RGB32;

    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);
    m_pSwsCtx = sws_getContext(
            m_nWidth, m_nHeight, src_pix_fmt,
            m_nScaledWidth, m_nScaledHeight, dst_pix_fmt,
            SWS_BICUBIC, NULL, NULL, NULL);

    m_nScaledDecodeSize = avpicture_get_size(dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);
}

void CV2DDecoderThread::SetupDoubleBuffers()
{
    enum PixelFormat dst_pix_fmt;
    if (m_ePixelFormat == eRGB24)
        dst_pix_fmt = PIX_FMT_RGB24;
    else
        dst_pix_fmt = PIX_FMT_RGB32;  // BGRX on little-endian

    FreeSBuffers();
    m_sBuffer[0] = new CSBuffer(m_nScaledDecodeSize, 32, false);
    m_sBuffer[1] = new CSBuffer(m_nScaledDecodeSize, 32, false);
    m_sBuffer[0]->AddReference();
    m_sBuffer[1]->AddReference();
    m_nDoubleBufferIndex = -1;

    // Assign appropriate parts of buffer to image planes in m_pDestPictureArrays
    avpicture_fill((AVPicture *)m_pDestPictureArray[0], (uint8_t *)m_sBuffer[0]->GetBuffer(0),
            dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);
    avpicture_fill((AVPicture *)m_pDestPictureArray[1], (uint8_t *)m_sBuffer[1]->GetBuffer(0),
            dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);
}

void CV2DDecoderThread::SendRFBStatus(bool a_bRFBReady)
{
    CScopedLock lock(m_mutex);
    m_bRFBReady = a_bRFBReady;
    m_bSendRFBStatus = true;
}

void CV2DDecoderThread::SetVideoDebugLevel(int a_debugLevel)
{
    CScopedLock lock(m_mutex);
    if (m_pFrameDecoder != NULL) {
        m_bDecoderDebug = (a_debugLevel != 0);
        m_pFrameDecoder->SetSliceDebug(m_bDecoderDebug);
    }
}

bool CV2DDecoderThread::CanDecodeFrame()
{
    if (m_nDoubleBufferIndex < 0) {
        m_nDoubleBufferIndex = GetDoubleBufferIndex();
    }
    if (m_nDoubleBufferIndex >= 0)
        return true;
    return false;
}

bool CV2DDecoderThread::IsReady()
{
    return (m_inBuffer == NULL);
}

void CV2DDecoderThread::DecodeFrame(unsigned char * aBuffer, int aLen)
{
    ASSERT(m_inBuffer = NULL);
    m_inBuffer = aBuffer;
    m_inBufferLen = aLen;
}

int CV2DDecoderThread::DecodeProcessor()
{
    CScopedLock lock(m_mutex);

    ProcessAltStream();

    if (m_inBuffer == NULL || m_qRendererSink == NULL)
        return 0;

    // Ensure that some one is interested in our queue
    if (m_qRendererSink->GetListenerCount() == 0) {
        return 0;
    }

    // Ensure Decoder has been set up
    if (m_pFrameDecoder == NULL) {
        return 0;
    }


    // Send frame parameters to renderer before we send first frame out
    if (m_bSendFrameInfo) {
        CRendererFrameInfo *frameInfo =
                new CRendererFrameInfo(m_nWidth, m_nHeight,
                        m_nScaledWidth,
                        m_nScaledHeight);
        CSFrame *sFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        sFrame->SetCustomObject(frameInfo);
        m_qRendererSink->InsertFrame(sFrame);
        m_bSendFrameInfo = false;
        return 0;
    }

    if (m_bSendRFBStatus) {
        CRendererRFBStatus *rfbStatus = new CRendererRFBStatus(m_bRFBReady);
        CSFrame *sFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        sFrame->SetCustomObject(rfbStatus);
        m_qRendererSink->InsertFrame(sFrame);
        m_bSendRFBStatus = false;
        return 0;
    }

    // Decode the frame
    m_pFrameDecoder->Decode(m_inBuffer, m_inBufferLen,
            m_pDecodeBuffer, m_nDecodeSize);

    // Copy decoded image into the designated double buffer
    if (m_nDoubleBufferIndex >= 0) {
        const uint8_t* srcSlice[3] = {m_pDecodeBuffer, 0, 0};
        const int srcStride[3] = {m_nWidth * 4, 0, 0};
        sws_scale(m_pSwsCtx, srcSlice, srcStride,
                0, m_nHeight,
                m_pDestPictureArray[m_nDoubleBufferIndex]->data,
                m_pDestPictureArray[m_nDoubleBufferIndex]->linesize);

    }

#ifdef USE_RECTANGLES
    int rectBufferSize = m_pFrameDecoder->GetRectanglesBuffer(m_pDecodeBuffer, m_pImageBuffer);
    SYLOG_DEBUG("rectBufferSize = %d", rectBufferSize);
#endif

#ifdef POST_RGB
    // Convert the image to RGB
    YUVToRGBConverter oRGB;
    oRGB.yuv422ToRgbScaled(m_pY, m_pU, m_pV, m_pImageBuffer,
            m_nWidth, m_nHeight, 1, YUVToRGBConverter::ImageFormat_BGRX);
#endif

    // Send frame to renderer
    if (m_qRendererSink != NULL) {
#ifdef POST_RGB
        CSFrame *sFrame =
                new CSFrame((char *)m_pImageBuffer, m_nImageBufferSize, 0, 0);
#else
#ifdef USE_RECTANGLES
        CSFrame *sFrame =
                new CSFrame((char *)m_pImageBuffer, rectBufferSize, 0, 0);
#else
        CSFrame *sFrame = NULL;
        if (m_nDoubleBufferIndex >= 0) {
            sFrame = new CSFrame(m_sBuffer[m_nDoubleBufferIndex], 0,
                    m_nDecodeSize, 0, 0);
            m_nDoubleBufferIndex = -1;
        }
#endif
#endif
        if (sFrame != NULL)
            m_qRendererSink->InsertFrame(sFrame);
    }
    m_nDecodedFrames++;

    // We are done
    m_inBuffer = NULL;

    return 0;
}

int CV2DDecoderThread::ProcessAltStream()
{

#if 0
    if (m_qRendererSource == NULL)
        return 0;
    CSFrame * sFrame = m_qRendererSource->GetFrame();

    if (sFrame != NULL) {

        if (sFrame->GetMessageType() == CSFrame::Custom) {
            CRendererMessage *rendererMessage =
                    (CRendererMessage *) sFrame->GetCustomObject();

            switch(rendererMessage->GetMessageType()) {
            case CRendererMessage::DebugVideo: {
                CRendererDebugVideo *debugMessage =
                        static_cast<CRendererDebugVideo *>(rendererMessage);
                if (m_pFrameDecoder != NULL) {
                    m_bDecoderDebug = (debugMessage->DebugLevel() != 0);
                    m_pFrameDecoder->SetSliceDebug(m_bDecoderDebug);
                }
                break;

            }
            default:
                break;
            }

            delete(rendererMessage);


        }

        m_qRendererSource->ReleaseFrame(sFrame);

    }
#endif
    return 0;
}

void
CV2DDecoderThread::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectCounterParam("FramesDecoded", m_nDecodedFrames);
    if (m_pFrameDecoder != NULL) {
        a_pCmd->SetObjectErrorCounterParam("LayerErrors",
                m_pFrameDecoder->GetLayerErrors());
    }
}
