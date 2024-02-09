/*
 * rtpvideodecoderthr.cpp
 *
 *  Created on: Apr 17, 2011
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include <sstream>

#include "rtpvideodecoderthr.hh"
#include "common.hh"
#include "renderermessage.hh"
#include "aligned_malloc.h"
#include "FFmpegUtils.hh"
#include "H264VideoRTPParser.hh"
#include "MPEG4VideoRTPParser.hh"

using namespace std;


CRTPVideoDecoderThread::CRTPVideoDecoderThread(CStreamer *a_pStreamer,
        MediaSubsession *a_pSubsession, ePixelFormat a_ePixelFormat)
    : CDetachedThread(a_pStreamer, "RTPVideoDecoder")
{
    m_bError = false;
    m_pDecodeBuffer = NULL;
    m_nDecodeSize = 0;
    m_nPacketsDecoded = 0;
    m_nFramesParsed = 0;
    m_nFramesDecoded = 0;
    m_nPicturesDecoded = 0;
    m_nPicturesRendered = 0;
    m_nPicturesDropped = 0;
    m_nDecodeErrors = 0;
    m_pSwsCtx = NULL;
    m_pCodecCtx = NULL;
    m_pCodec = NULL;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nScaledWidth = 0;
    m_nScaledHeight = 0;
    m_nScalingMode = SCALING_MODE_NATIVE;
    m_nScaledWidth = 600;
    m_nScaledHeight = 300;

    m_bSendFrameInfo = false;
    m_pRTPParser = NULL;
    m_pParserContext = NULL;
    m_qQueueSource = NULL;
    m_qRendererSink = NULL;
    m_qRendererSource = NULL;
    m_bNeedsFrameParsing = false;
    m_nFirstTimeOffset = 0xFFFF;
    m_sDelayedFrame = NULL;
    m_sBuffer[0] = m_sBuffer[1] = NULL;
    m_pSubsession = a_pSubsession;
    m_ePixelFormat = a_ePixelFormat;
    m_nDelayTimestamp = 0;

    // Initialize FFmpeg
    FFmpegUtils::Initialize();

    // Setup Codec Context
    m_pCodecCtx = avcodec_alloc_context();

    m_pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    // Find the decoder
    if (!FindDecoder()) {
        m_bError = true;
        return;
    }

    // Prime the decoder
    m_pRTPParser->RetrieveCodecConfig(m_pSubsession, m_pCodecCtx);

    // Open codec for decoding
    if (!FFmpegUtils::OpenCodec(m_pCodecCtx, m_pCodec)) {
        m_bError = true;
        return;
    }

    if (m_pCodec->capabilities & CODEC_CAP_TRUNCATED) {
        m_pCodecCtx->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
    }

    // Get initial width and height (most likely still unknown, unless SDP specifies it)
    m_nWidth = m_pCodecCtx->width;
    m_nHeight = m_pCodecCtx->height;
    SetupScaledDimensions();

    // Get the handle to the packet parser for the codec
    m_pParserContext = av_parser_init(m_pCodecCtx->codec_id);

    // Allocate picture frames
    m_pPicture = avcodec_alloc_frame();

    m_pDestPicture = NULL;
    m_pDestPictureArray[0] = NULL;
    m_pDestPictureArray[1] = NULL;

    m_pDestPictureArray[0] = avcodec_alloc_frame();
    m_pDestPictureArray[1] = avcodec_alloc_frame();

    // Create queue for reading data from sender thread
    ostringstream qid;
    qid << "ToRTPVideoDecoder_" << GetId();
    m_qQueueSource =
            CSQueueManager::Instance()->GetQueueSource(qid.str());

    // Create the queues for sending/receiving data from renderer
    qid.str("");
    qid << "ToRenderer_" << GetId();
    m_qRendererSink =
            CSQueueManager::Instance()->GetQueueSink(qid.str());

    // TODO: handle messages from the renderer
    qid.str("");
    qid << "FromRenderer_" << GetId();
    m_qRendererSource =
            CSQueueManager::Instance()->GetQueueSource(qid.str());
}

CRTPVideoDecoderThread::~CRTPVideoDecoderThread()
{
    // Release ffmpeg resources
    av_parser_close(m_pParserContext);
    FFmpegUtils::CloseCodec(m_pCodecCtx);
    av_free(m_pCodecCtx->extradata);
    av_free(m_pCodecCtx);
    if (m_pPicture != NULL)
        av_free(m_pPicture);

    aligned_free(m_pDecodeBuffer);
    if (m_pDestPicture != NULL) {
        av_free(m_pDestPicture);
    }

    FreeSBuffers();
    if (m_pDestPictureArray[0] != NULL) {
        av_free(m_pDestPictureArray[0]);
    }
    if (m_pDestPictureArray[1] != NULL) {
        av_free(m_pDestPictureArray[1]);
    }
    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);

    // Release our RTP parser
    delete m_pRTPParser;

    // Release the queues
    if (m_qQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);

    if (m_qRendererSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qRendererSource);

    if (m_qRendererSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qRendererSink);

}

int CRTPVideoDecoderThread::RunThread()
{
    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        int rc = DecodeProcessor();
        if (rc < 0) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            SYSLOG_INFO("Thread %s giving up\n", m_pThreadName);
            break;
        }
        if (rc == 0)
            LightSleep();
    }

    SYSLOG_INFO("RTP Video decoder thread done\n");

    return 0;
}

bool CRTPVideoDecoderThread::FindDecoder() {

    if (strcasecmp(m_pSubsession->codecName(), "H264") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_H264;
        m_pRTPParser = new CH264VideoRTPParser(m_pSubsession->rtpPayloadFormat());
        m_bNeedsFrameParsing = true;
    }
    else if (strcasecmp(m_pSubsession->codecName(), "MP4V-ES") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_MPEG4;
        m_pRTPParser = new CMPEG4VideoRTPParser(m_pSubsession->rtpPayloadFormat());
        m_bNeedsFrameParsing = false;
    }
    else {
        return false;
    }

    m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
    if (m_pCodec == NULL)
        return false;
    return true;
}

int CRTPVideoDecoderThread::ParsePacket(uint8_t * packet_buffer, int packet_size)
{
    uint8_t *in_data = packet_buffer;
    int in_len = packet_size;
    uint8_t *frame_buffer;
    int frame_size = 0;
    uint64_t pts = 0, dts = 0, pos = 0;

    m_nFramesParsed++;

    // Send the packets to the frame parser. It will collect multiple packets
    // till it detects a frame and return a frame in frame_buffer.
    while (in_len) {
        int len = av_parser_parse2(m_pParserContext, m_pCodecCtx, &frame_buffer, &frame_size,
                in_data, in_len, pts, dts, pos);

        in_data += len;
        in_len  -= len;

        if (frame_size > 0) {
            DecodeFrame(frame_buffer, frame_size);
        }
    }
    return 0;
}

void CRTPVideoDecoderThread::DecodeFrame(uint8_t *frame_buffer, int frame_size)
{
    AVPacket avFramePkt;

    int got_picture;

    av_init_packet(&avFramePkt);
    avFramePkt.data = frame_buffer;
    avFramePkt.size = frame_size;

    m_nFramesDecoded++;

    while (avFramePkt.size > 0) {

        int len = avcodec_decode_video2(m_pCodecCtx, m_pPicture, &got_picture, &avFramePkt);
        if (len < 0) {
            m_nDecodeErrors++;
            break;
        }

        if (got_picture) {
            m_nPicturesDecoded++;

            CheckFrameDimensions();

            // Convert to RGB format
            if (m_pSwsCtx != NULL) {
                // Let the renderer catch up
                int doubleBufferIndex = GetDoubleBufferIndex();
                if (doubleBufferIndex >= 0) {
                    sws_scale(m_pSwsCtx, (const uint8_t * const*)m_pPicture->data,
                            m_pPicture->linesize, 0, m_nHeight,
                            m_pDestPictureArray[doubleBufferIndex]->data,
                            m_pDestPictureArray[doubleBufferIndex]->linesize);
                    PublishFrame(doubleBufferIndex);
                }
                else {
                    m_nPicturesDropped++;
                }
            }
        }
        avFramePkt.size -= len;
        avFramePkt.data += len;
    }
}

void CRTPVideoDecoderThread::PublishFrame(int aDoubleBufferIndex)
{
    // Ensure that someone is listening on the queue
    if (m_qRendererSink->GetListenerCount() == 0)
        return;

    // Send the latest frame dimension before sending the first frame out
    if (m_bSendFrameInfo)
        SendFrameInfo();

    CSFrame *sFrame = new CSFrame(m_sBuffer[aDoubleBufferIndex], 0,
                m_nDecodeSize, 0, 0);

    m_qRendererSink->InsertFrame(sFrame);
    m_nPicturesRendered++;
    m_sDelayedFrame = NULL;
}

void CRTPVideoDecoderThread::PublishFrameSmooth(bool bNewPicture)
{
    // No smoothing yet (TBD)
    bool bNoSmoothing = true;

    // Ensure that someone is listening on the queue
    if (m_qRendererSink->GetListenerCount() == 0)
        return;

    // Send the latest frame dimension before sending the first frame out
    if (m_bSendFrameInfo)
        SendFrameInfo();

    if (m_sDelayedFrame != NULL && bNewPicture) {
        printf("Too late for delayed picture\n");
        delete m_sDelayedFrame;
        m_sDelayedFrame = NULL;
        m_nPicturesDropped++;
    }
    else if (m_sDelayedFrame != NULL) {
        timestamp_t now = CTimeKeeper::Now() - m_nFirstTimeOffset;
        if (now >= m_nDelayTimestamp) {
            printf("Sending Delayed Frame\n");
            m_qRendererSink->InsertFrame(m_sDelayedFrame);
            m_nPicturesRendered++;
            m_sDelayedFrame = NULL;
            return;
        }
        else {
            //printf("Still early %d\n", (int)(now - m_nDelayTimestamp));
            return;
        }
    }
    // bNewPicure better be true now
    if (!bNewPicture) {
        printf("Bug??\n");
        return;
    }

    CSFrame *sFrame =
            new CSFrame((char *)m_pDecodeBuffer, m_nDecodeSize, 0, 0);

    if (m_nFirstTimeOffset == 0xFFFF) {
        m_nFirstTimeOffset = CTimeKeeper::Now() - m_pRTPParser->GetTimestamp();
        m_qRendererSink->InsertFrame(sFrame);
        m_nPicturesRendered++;
        m_sDelayedFrame = NULL;
        return;
    }

    timestamp_t now = CTimeKeeper::Now() - m_nFirstTimeOffset;
    if (bNoSmoothing || now >= m_pRTPParser->GetTimestamp()) {
    //    printf("Late by %d %d\n", (int)(now - m_pRTPParser->GetTimestamp()), m_nFirstTimeOffset);
        m_qRendererSink->InsertFrame(sFrame);
        m_nPicturesRendered++;
        m_sDelayedFrame = NULL;
    }
    else {
//        printf("Early by %d %d\n", (int)(m_pRTPParser->GetTimestamp() - now), m_nFirstTimeOffset);
        m_sDelayedFrame = sFrame;
        m_nDelayTimestamp = m_pRTPParser->GetTimestamp();
    }
}

int CRTPVideoDecoderThread::DecodeProcessor()
{

    int retval = 0;

    ProcessAltStream();

    if (m_qQueueSource == NULL)
        return 0;

    if (m_sDelayedFrame) {
        PublishFrame(false);
    }

    CSFrame *sFrame = m_qQueueSource->GetFrame();

    AVPacket avpkt;

    if (sFrame) {

        // Extract RTP payload into avpkt
        if (m_pRTPParser->GetPacket((uint8_t *)sFrame->GetData(), sFrame->GetLength(), &avpkt)) {

            // Send packet onwards for frame parsing and decoding
            if (m_bNeedsFrameParsing)
                ParsePacket((uint8_t *)avpkt.data, (int)avpkt.size);
            else
                DecodeFrame((uint8_t *)avpkt.data, (int)avpkt.size);

            av_free_packet(&avpkt);
        }

        m_nPacketsDecoded++;
        m_qQueueSource->ReleaseFrame(sFrame);
        retval = 1;
    }

    return retval;
}

int
CRTPVideoDecoderThread::ProcessAltStream()
{
    int retval = 0;

    CSFrame * sFrame = m_qRendererSource->GetFrame();

    if (sFrame != NULL) {

        if (sFrame->GetMessageType() == CSFrame::Custom) {
            CRendererMessage *rendererMessage =
                    static_cast<CRendererMessage *> (sFrame->GetCustomObject());

            switch(rendererMessage->GetMessageType()) {
            case CRendererMessage::DebugVideo: {
//                CRendererDebugVideo *debugMessage =
//                        static_cast<CRendererDebugVideo *>(rendererMessage);
                retval = 1;
                break;
            }

            case CRendererMessage::ScalingRequest: {
                CRendererScalingRequest *scalingRequest =
                        static_cast<CRendererScalingRequest *>(rendererMessage);
                SetupScaledDimensions(scalingRequest->scalingMode(),
                        scalingRequest->scaledWidth(),
                        scalingRequest->scaledHeight());
                retval = 1;
                break;
            }

            default:
                retval = 1;
                break;
            }

            delete(rendererMessage);


        }

        m_qRendererSource->ReleaseFrame(sFrame);

    }


    return retval;
}

void CRTPVideoDecoderThread::CheckFrameDimensions()
{
    if (m_nWidth != m_pCodecCtx->width || m_nHeight != m_pCodecCtx->height) {
        m_nWidth = m_pCodecCtx->width;
        m_nHeight = m_pCodecCtx->height;
        SetupScaledDimensions();
        SetupDestinationImage();
    }
}

void CRTPVideoDecoderThread::SetupScaledDimensions()
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

void CRTPVideoDecoderThread::SetupScaledDimensions(int aScalingMode,
        int aScaledWidth, int aScaledHeight)
{
    SYSLOG_INFO("Got scaling change request mode = %d, scaling dim = %d x %d\n",
            aScalingMode, aScaledWidth, aScaledHeight);

    m_nScalingMode = aScalingMode;
    m_nScaledWidth = aScaledWidth;
    m_nScaledHeight = aScaledHeight;

    SetupScaledDimensions();
    SetupDestinationImage();
}

void CRTPVideoDecoderThread::SetupDestinationImage()
{
    enum PixelFormat dst_pix_fmt;
    enum PixelFormat src_pix_fmt;
    if (m_ePixelFormat == eRGB24)
        dst_pix_fmt = PIX_FMT_RGB24;
    else
        dst_pix_fmt = PIX_FMT_RGB32;  // BGRX on little-endian
    src_pix_fmt = m_pCodecCtx->pix_fmt;

    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);
    m_pSwsCtx = sws_getContext(
            m_nWidth, m_nHeight, src_pix_fmt,
            m_nScaledWidth, m_nScaledHeight, dst_pix_fmt,
            SWS_BICUBIC, NULL, NULL, NULL);

    if (m_pSwsCtx == NULL)
        return;

    // Determine required buffer size to hold destination image and allocate buffer
    m_nDecodeSize = avpicture_get_size(dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);


    // Create double buffers using SBuffers
    FreeSBuffers();
    m_sBuffer[0] = new CSBuffer(m_nDecodeSize, 32, false);
    m_sBuffer[1] = new CSBuffer(m_nDecodeSize, 32, false);
    m_sBuffer[0]->AddReference();
    m_sBuffer[1]->AddReference();

    // Assign appropriate parts of buffer to image planes in m_pDestPictureArrays
    avpicture_fill((AVPicture *)m_pDestPictureArray[0], (uint8_t *)m_sBuffer[0]->GetBuffer(0),
            dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);
    avpicture_fill((AVPicture *)m_pDestPictureArray[1], (uint8_t *)m_sBuffer[1]->GetBuffer(0),
            dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);
    m_bSendFrameInfo = true;
}

void CRTPVideoDecoderThread::FreeSBuffers()
{
    if (m_sBuffer[0] != NULL)
        m_sBuffer[0]->RemoveReference();
    if (m_sBuffer[1] != NULL)
        m_sBuffer[1]->RemoveReference();
}

int CRTPVideoDecoderThread::GetDoubleBufferIndex() {
    // Use ref count to determine which SBuffer downstream has used up and
    // made available for us to reuse
    if (m_sBuffer[0]->GetReferenceCount() == 1) {
        return 0;
    }
    else if (m_sBuffer[1]->GetReferenceCount() == 1) {
        return 1;
    }
    return -1;
}

void CRTPVideoDecoderThread::SendFrameInfo()
{
    CRendererFrameInfo *frameInfo =
            new CRendererFrameInfo(m_nWidth, m_nHeight, m_nScaledWidth, m_nScaledHeight);
    CSFrame *sFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    sFrame->SetCustomObject(frameInfo);
    m_qRendererSink->InsertFrame(sFrame);
    m_bSendFrameInfo = false;
}

void CRTPVideoDecoderThread::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectCounterParam("VideoPacketsDecoded", m_nPacketsDecoded);
    a_pCmd->SetObjectCounterParam("VideoFramesParsed", m_nFramesParsed);
    a_pCmd->SetObjectCounterParam("VideoFramesDecoded", m_nFramesDecoded);
    a_pCmd->SetObjectCounterParam("PicturesDecoded", m_nPicturesDecoded);
    a_pCmd->SetObjectCounterParam("PicturesRendered", m_nPicturesRendered);
    a_pCmd->SetObjectCounterParam("PicturesDropped", m_nPicturesDropped);

    a_pCmd->SetObjectErrorCounterParam("VideoDecodeErrors", m_nDecodeErrors);
    a_pCmd->SetObjectGuageParam("InputQueueLength", m_qQueueSource->Size());
    a_pCmd->SetObjectErrorCounterParam("InputQueueDrops", m_qQueueSource->DropCount());
    a_pCmd->SetObjectConstantParam("FrameWidth", m_nWidth);
    a_pCmd->SetObjectConstantParam("FrameHeight", m_nHeight);
    a_pCmd->SetObjectConstantParam("ScaledWidth", m_nScaledWidth);
    a_pCmd->SetObjectConstantParam("ScaledHeight", m_nScaledHeight);
}

