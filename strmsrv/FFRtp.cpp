/*
 * FFRtp.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: durga
 */
#include "strmsrv_compatibility.h"
#include <sstream>

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "FFRtp.hh"
#include "sframe.hh"
#include "timekeeper.hh"
#include "utils.hh"
#include "squeue.hh"
#include "FFmpegUtils.hh"
#include "H264VideoRTPParser.hh"
#include "MPEG4VideoRTPParser.hh"
#include "MPEG4AudioRTPParser.hh"
#include "MPEG4LATMAudioRTPParser.hh"
#include "BasicUsageEnvironment.hh"
#include "ThumbnailGenerator.h"
#include "RTPMediaHandler.h"
#include "PTSGenerator.hh"

using namespace std;

#define NAL_START_SIZE 3
#define NAL_TYPE_SPS 7
#define NAL_TYPE_PPS 8

CFrameData::CFrameData(uint8_t *a_pData, int a_nBuffLen,
                       bool a_bVideoFrame, uint32_t a_nRTPTimestamp,
                       uint64_t a_nTimestamp)
{
    m_nBuffLen = a_nBuffLen;

    m_pData = new uint8_t [a_nBuffLen + FF_INPUT_BUFFER_PADDING_SIZE];
    memcpy(m_pData, a_pData, a_nBuffLen);
    memset(m_pData + a_nBuffLen, 0x00, FF_INPUT_BUFFER_PADDING_SIZE);

    m_bVideoFrame = a_bVideoFrame;
    m_nRTPTimestamp = a_nRTPTimestamp;
    m_nTimestamp = a_nTimestamp;
}

CFrameData::~CFrameData()
{
    DeleteData();
}

uint8_t* CFrameData::GetData()
{
    return m_pData;
}

uint32_t CFrameData::GetLength()
{
    return m_nBuffLen;
}

uint32_t CFrameData::GetRTPTimestamp()
{
    return m_nRTPTimestamp;
}

uint64_t CFrameData::GetTimestamp()
{
    return m_nTimestamp;
}

bool CFrameData::IsVideoFrame()
{
    return m_bVideoFrame;
}

void CFrameData::DeleteData()
{
    if (m_pData != NULL)
        delete [] m_pData;
    m_pData = NULL;
}

CFFRtp::CFFRtp(int a_nId, std::string a_strSDPURL, bool a_bShowProgress, bool a_bLiveMode,
        bool a_bNativeOutput)
: CFFCodec("RTP", a_bShowProgress, a_bLiveMode, a_bNativeOutput)
{
    m_pVideoSubsession = NULL;
    m_pAudioSubsession = NULL;
    m_pTaskScheduler = NULL;
    m_pUsageEnv = NULL;
    m_pSession = NULL;

    m_pVideoRTPParser = NULL;
    m_pAudioRTPParser = NULL;

    m_pVideoCodecCtx = NULL;
    m_pAudioCodecCtx = NULL;
    m_pVideoCodec = NULL;
    m_pAudioCodec = NULL;
    m_pVideoParserContext = NULL;
    m_pAudioParserContext = NULL;
    m_pPicture = NULL;
    m_pDestPicture = NULL;
    m_pSwsCtx = NULL;
    m_pDecodeBuffer = NULL;
    m_pCachedFrameData = NULL;

    m_nFrameWidth = 0;
    m_nFrameHeight = 0;

    m_nDecodeSize = 0;

    m_bNeedsFrameParsing = false;
    m_bInitialized = false;

    m_nBeginTimestamp = 0;
    m_nCurrentTimestamp = 0;

    m_nGapStartTime = 0;

    m_pPTSGeneratorForVideo = NULL;
    m_pPTSGeneratorForAudio = NULL;

    m_nIFrameCount = 0;
    m_nInAudioFrameCount = 0;

    m_bGotResolution = false;

    m_nInFrameCount = 0;

    m_bNativeOutput = false;
    m_bVideoFound = false;
    m_bAudioFound = false;
    m_bWrittenTrailer = false;

    m_pSPSData = NULL;
    m_pPPSData = NULL;

    m_nSPSData_Size = 0;
    m_nPPSData_Size = 0;

    m_nInputVideoSSRC = 0;
    m_nInputAudioSSRC = 0;

    m_bFoundSPSAndPPS = false;

    m_eDecodePixelFormat = eBGRX32;
    m_strSDPURL = a_strSDPURL;

    CreateQueues(a_nId);

    if (m_bLiveMode == true) {
        this->Start();
    }
}

CFFRtp::~CFFRtp() {
    StopStream();

    av_parser_close(m_pVideoParserContext);
    av_parser_close(m_pAudioParserContext);

    if (m_pVideoCodecCtx != NULL) {
        FFmpegUtils::CloseCodec(m_pVideoCodecCtx);
        av_free(m_pVideoCodecCtx->extradata);
    }

    if (m_pAudioCodecCtx != NULL) {
        FFmpegUtils::CloseCodec(m_pAudioCodecCtx);
        av_free(m_pAudioCodecCtx->extradata);
    }

    av_free(m_pVideoCodecCtx);
    av_free(m_pAudioCodecCtx);
    av_free(m_pPicture);
    av_free(m_pDestPicture);
    sws_freeContext(m_pSwsCtx);

    if (m_pSession != NULL)
        Medium::close(m_pSession);

    if (m_pUsageEnv != NULL)
        m_pUsageEnv->reclaim();

    delete m_pTaskScheduler;

    delete [] m_pDecodeBuffer;

    delete m_pVideoRTPParser;
    delete m_pAudioRTPParser;

    DeletePTSGeneratorObjs();

    delete [] m_pSPSData;
    delete [] m_pPPSData;

    DeleteQueues();
    DeleteFrameData();
}

void CFFRtp::Initialize()
{
    if (m_bInitialized == true)
        return;

    ParseSDP();
    InitializeVideoRTPParser();
    InitializeAudioRTPParser();

    if (m_bError == false) {
        m_bInitialized = true;

        SetStandardMediaParams();
        CreatePTSGeneratorObjs();
    }
}

void CFFRtp::SetStandardMediaParams()
{
    CRTPMediaHandler *pStandardMediaHandler =
            dynamic_cast<CRTPMediaHandler*>(GetStandardMediaHandler());
    if (pStandardMediaHandler == NULL)
        return;

    if (m_bVideoFound == true) {
        pStandardMediaHandler->SetVideoFound(m_bVideoFound);
        pStandardMediaHandler->SetVideoCodecID(m_pVideoCodecCtx->codec_id);
        pStandardMediaHandler->SetSpropData(m_pVideoCodecCtx->extradata,
                m_pVideoCodecCtx->extradata_size);
        pStandardMediaHandler->SetRTPTimestampFrequency(
                m_pVideoSubsession->rtpTimestampFrequency());
    }

    if (m_bAudioFound == true) {
        pStandardMediaHandler->SetAudioFound(m_bAudioFound);
        pStandardMediaHandler->SetAudioExtradata(m_pAudioCodecCtx->extradata,
                m_pAudioCodecCtx->extradata_size);
        pStandardMediaHandler->SetAudioCodecID(m_pAudioCodecCtx->codec_id);
        pStandardMediaHandler->SetAudioSampleRate(m_pAudioSubsession->rtpTimestampFrequency());
        pStandardMediaHandler->SetAudioChannels(m_pAudioCodecCtx->channels);
    }

    m_bNativeOutput = true;
}

void CFFRtp::CreatePTSGeneratorObjs()
{
    if (m_bNativeOutput == false)
        return;

    if (m_bVideoFound) {
        m_pPTSGeneratorForVideo = new CPTSGenerator(1000,
                                  m_pVideoSubsession->rtpTimestampFrequency());
    }

    if (m_bAudioFound) {
        m_pPTSGeneratorForAudio = new CPTSGenerator(1000,
                                  m_pAudioSubsession->rtpTimestampFrequency());
    }
}

void CFFRtp::DeletePTSGeneratorObjs()
{
    delete m_pPTSGeneratorForVideo;
    delete m_pPTSGeneratorForAudio;
}

void CFFRtp::ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset)
{
    if (sFrame == NULL) {
        return;
    }

    if (m_bError == true)
        return;

    if (ProceedToProcess() == false)
        return;

    m_nCurrentTimestamp = sFrame->GetTimeStamp();
    m_nElapsed = nMediaOffset;

    if (m_nBeginTimestamp == 0) {
        m_nBeginTimestamp = m_nCurrentTimestamp;

        Initialize();
    }

    if (sFrame->GetMessageType() == CSFrame::Normal) {
        unsigned long nStreamType = sFrame->GetStreamType();

        if (nStreamType == STREAMTYPE_RTP) {
            CRTPStreamMediaFlag rtpSMediaFlag(sFrame->GetMediaFlag());
            switch(rtpSMediaFlag.GetEncaps()) {
            case FRAMETYPE_RTP_ENCAPS_RTP:
                switch (rtpSMediaFlag.GetChannel()) {
                case FRAMETYPE_RTP_CHAN_VIDEO:
                    HandleVideoPacket(sFrame->GetData(), sFrame->GetLength());
                    break;

                case FRAMETYPE_RTP_CHAN_AUDIO:
                    HandleAudioPacket(sFrame->GetData(), sFrame->GetLength());
                    break;
                }
                break;
            }
        }
    }
    else if (sFrame->GetMessageType() == CSFrame::StartOfStream) {
        CheckForGaps();
    }
    else if (sFrame->GetMessageType() == CSFrame::EndOfStream) {
        m_nGapStartTime = sFrame->GetTimeStamp();
    }

    if (m_bLiveMode == false) {
        CreateOutput();
    }

    m_nProcessedBytes += sFrame->GetLength();

    PrintProgress();
}

void CFFRtp::CheckForGaps()
{
    if (m_nGapStartTime <= 0) //Stop never called yet.
        return;

    uint64_t nGaps = m_nCurrentTimestamp - m_nGapStartTime;
    m_nGapStartTime = 0;

    if ((m_pPTSGeneratorForVideo != NULL) &&
            (m_bVideoFound == true))
        m_pPTSGeneratorForVideo->SetDiscontinuity(nGaps);

    if ((m_pPTSGeneratorForAudio != NULL) &&
            (m_bAudioFound == true))
        m_pPTSGeneratorForAudio->SetDiscontinuity(nGaps);
}

void CFFRtp::ParseSDP()
{
    if (m_strSDPURL.length() <= 0) {
        SetErrorString("SDP URL corrupted.");
        return;
    }

    ParseSDPURL();

    // Grab file into a buffer
    uint8_t * fileBuffer;
    int rc = ReadFileIntoBuffer(m_strSDPFilePath.c_str(), fileBuffer);

    if (rc == -1) {
        SetErrorString("Failed to open file: " + m_strSDPFilePath);
        return;
    }

    // Parse SDP file
    m_pTaskScheduler = BasicTaskScheduler::createNew();
    if (m_pTaskScheduler == NULL) {
        SetErrorString("TaskScheduler creation failed.");
        delete [] fileBuffer;
        return;
    }

    m_pUsageEnv = BasicUsageEnvironment::createNew(*m_pTaskScheduler);
    if (m_pUsageEnv == NULL) {
        SetErrorString("BasicUsageEnvironment creation failed.");
        delete [] fileBuffer;
        return;
    }

    m_pSession = MediaSession::createNew(*m_pUsageEnv, (const char *)fileBuffer);
    if (m_pSession == NULL) {
        SetErrorString("MediaSession creation failed.");
        delete [] fileBuffer;
        return;
    }

    // Pick out the first audio and video sessions from the SDP description
    MediaSubsessionIterator iter(*m_pSession);
    MediaSubsession *subsession;
    while ((subsession = iter.next()) != NULL) {
        if (strcmp(subsession->mediumName(), "video") == 0) {
            m_pVideoSubsession = subsession;
            m_bVideoFound = true;
        }
        else if(strcmp(subsession->mediumName(), "audio") == 0) {
            m_pAudioSubsession = subsession;
            m_bAudioFound = true;
        }
    }

    if (m_pVideoSubsession == NULL && m_pAudioSubsession == NULL)
        SetErrorString("Video and Audio Sessions not found.");

    delete [] fileBuffer;
}

void CFFRtp::ParseSDPURL()
{
    size_t foundPos = m_strSDPURL.find("://");

    if (foundPos != std::string::npos)
        m_strSDPFilePath = m_strSDPURL.substr(foundPos + 3, m_strSDPURL.length());
    else
        m_strSDPFilePath = m_strSDPURL;
}

bool CFFRtp::InitializeVideoRTPParser()
{
    if (m_bVideoFound == false)
        return false;

    if (m_bError == true)
        return false;

    FFmpegUtils::Initialize();

    m_pVideoCodecCtx = avcodec_alloc_context();

    m_pVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    // Find the decoder
    if (!FindVideoDecoder()) {
        return false;
    }

    m_pVideoRTPParser->RetrieveCodecConfig(m_pVideoSubsession, m_pVideoCodecCtx);

    // Open codec for decoding
    if (!FFmpegUtils::OpenCodec(m_pVideoCodecCtx, m_pVideoCodec)) {
        SetErrorString("FFmpegUtils::OpenCodec failed.");
        return false;
    }

    if (m_pVideoCodec->capabilities & CODEC_CAP_TRUNCATED) {
        m_pVideoCodecCtx->flags |= CODEC_FLAG_TRUNCATED;
    }

    m_nFrameWidth = m_pVideoCodecCtx->width;
    m_nFrameHeight = m_pVideoCodecCtx->height;

    m_pVideoParserContext = av_parser_init(m_pVideoCodecCtx->codec_id);

    m_pPicture = avcodec_alloc_frame();

    m_pDestPicture = avcodec_alloc_frame();

    return true;
}

bool CFFRtp::FindVideoDecoder()
{
    if (strcasecmp(m_pVideoSubsession->codecName(), "H264") == 0) {
        m_pVideoCodecCtx->codec_id = CODEC_ID_H264;
        m_pVideoRTPParser = new CH264VideoRTPParser(m_pVideoSubsession->rtpPayloadFormat());
        m_bNeedsFrameParsing = true;
    }
    else if (strcasecmp(m_pVideoSubsession->codecName(), "MP4V-ES") == 0) {
        m_pVideoCodecCtx->codec_id = CODEC_ID_MPEG4;
        m_pVideoRTPParser = new CMPEG4VideoRTPParser(m_pVideoSubsession->rtpPayloadFormat());
        m_bNeedsFrameParsing = false;

        // For mpeg4 case, we are creating thumb-nail, as early as we get one decoded key frame.
        CThumbnailHandler *pTNHandler = GetThumbnailHandler();
        if (pTNHandler != NULL) {
            pTNHandler->SetPrimaryTNOffset(0);
        }
    }
    else {
        SetErrorString("Doesn't support this codec. \n");
        return false;
    }

    m_pVideoCodec = avcodec_find_decoder(m_pVideoCodecCtx->codec_id);
    if (m_pVideoCodec == NULL) {
        SetErrorString("Codec not found.");
        return false;
    }

    return true;
}

bool CFFRtp::InitializeAudioRTPParser()
{
    if (m_bAudioFound == false)
        return false;

    FFmpegUtils::Initialize();

    m_pAudioCodecCtx = avcodec_alloc_context();
    m_pAudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;

    if (!FindAudioDecoder()) {
        return false;
    }

    m_pAudioRTPParser->RetrieveCodecConfig(m_pAudioSubsession, m_pAudioCodecCtx);

    if (!FFmpegUtils::OpenCodec(m_pAudioCodecCtx, m_pAudioCodec)) {
        printf("FFmpegUtils::OpenCodec failed for Audio. \n");
        return false;
    }

    m_pAudioParserContext = av_parser_init(m_pAudioCodecCtx->codec_id);

    return true;
}

bool CFFRtp::FindAudioDecoder()
{
    if (strcasecmp(m_pAudioSubsession->codecName(), "MPEG4-GENERIC") == 0) {
        m_pAudioCodecCtx->codec_id = CODEC_ID_AAC;
        m_pAudioRTPParser = new CMPEG4AudioRTPParser(m_pAudioSubsession->rtpPayloadFormat());
    }
    else if (strcasecmp(m_pAudioSubsession->codecName(), "MP4V-ES") == 0) {
        m_pAudioCodecCtx->codec_id = CODEC_ID_MPEG4;
        m_pAudioRTPParser = new CMPEG4LATMAudioRTPParser(m_pAudioSubsession->rtpPayloadFormat());
    }
    else if (strcasecmp(m_pAudioSubsession->codecName(), "PCMU") == 0) {
        m_pAudioCodecCtx->codec_id = CODEC_ID_PCM_MULAW;
        m_pAudioRTPParser = new CRTPParser(m_pAudioSubsession->rtpPayloadFormat(), false);
    }
    else if (strcasecmp(m_pAudioSubsession->codecName(), "PCMA") == 0) {
        m_pAudioCodecCtx->codec_id = CODEC_ID_PCM_ALAW;
        m_pAudioRTPParser = new CRTPParser(m_pAudioSubsession->rtpPayloadFormat(), false);
    }
    else {
        printf("Doesn't support this Audio codec. \n");
        return false;
    }

    m_pAudioCodec = avcodec_find_decoder(m_pAudioCodecCtx->codec_id);
    if (m_pAudioCodec == NULL) {
        printf("Audio Codec not found. \n");
        return false;
    }

    return true;
}

int CFFRtp::ParsePacket(uint8_t * packet_buffer, int packet_size)
{
    uint8_t *in_data = packet_buffer;
    int in_len = packet_size;
    uint8_t *frame_buffer;
    int frame_size = 0;
    uint64_t pts = 0, dts = 0, pos = 0;

    // Send the packets to the frame parser. It will collect multiple packets
    // till it detects a frame and return a frame in frame_buffer.
    while (in_len) {
        int len = av_parser_parse2(m_pVideoParserContext, m_pVideoCodecCtx, &frame_buffer, &frame_size,
                                   in_data, in_len, pts, dts, pos);

        if (frame_size > 0) {
            m_nInFrameCount++;
            ParseSPSAndPPSValues(frame_buffer, frame_size);

            if ((m_bGotResolution == false) || (m_pVideoParserContext->key_frame == 1)
                    || (m_bNativeOutput == true)) {
                SaveFrameBuffer(frame_buffer, frame_size);
            }

            if (m_pVideoParserContext->key_frame == 1) {
                m_nIFrameCount++;
            }
        }
        in_data += len;
        in_len  -= len;
    }
    return 0;
}

void CFFRtp::SaveFrameBuffer(uint8_t *a_pFrameBuffer, int a_nBufferLen,
                             bool a_bVideoFrame)
{
    CSFrame *pSFrame = new CSFrame(m_nCurrentTimestamp, CSFrame::Custom);

    CSBuffer *sBuffer = new CSBuffer(sizeof(CFrameData), 64);
    char * buffer = sBuffer->GetBuffer(0);

    // Using "placement new" ctor to copy CSFrame into buffer
    new (buffer) CFrameData(a_pFrameBuffer,
                            a_nBufferLen,
                            a_bVideoFrame,
                            (a_bVideoFrame == true) ? m_pVideoRTPParser->GetRawTimestamp() :
                                m_pAudioRTPParser->GetRawTimestamp(),
                            m_nCurrentTimestamp);

    pSFrame->SetData(sBuffer, 0, sizeof(CFrameData));

    m_qTNQueueSink->InsertFrame(pSFrame);
}

bool CFFRtp::DecodeFrame(uint8_t *frame_buffer, int frame_size)
{
    bool bDecoded = false;

    AVPacket avFramePkt;

    int got_picture = 0;

    av_init_packet(&avFramePkt);
    avFramePkt.data = frame_buffer;
    avFramePkt.size = frame_size;

    while (avFramePkt.size > 0) {
        int len = avcodec_decode_video2(m_pVideoCodecCtx, m_pPicture, &got_picture, &avFramePkt);
        if (len < 0) {
            break;
        }

        if ( m_pVideoCodecCtx->width > 0 && m_pVideoCodecCtx->height > 0) {
            m_bGotResolution = true;
            CheckFrameDimensions();
        }

        if (got_picture > 0) {
            bDecoded = true;
        }

        // For mpeg4 for streams, we are decoding everything till to get first key frame.
        if (m_bNeedsFrameParsing == false) {
            if (m_pPicture->key_frame == true) {
                bDecoded = true;
            }
            else {
                bDecoded = false;
            }
        }
        else {
            if (m_bGotResolution == true) {
                // Since we only decode IFrames, we need to decode twice to flush the data in the decoder so that
                // it gives the latest frame instead of previous frame.
                avFramePkt.data = NULL;
                avFramePkt.size = 0;
                avcodec_decode_video2(m_pVideoCodecCtx, m_pPicture, &got_picture, &avFramePkt);

                if (got_picture > 0) {
                    if (m_pPicture->key_frame == true) {
                        bDecoded = true;
                    }
                }
            }
        }

        avFramePkt.size -= len;
        avFramePkt.data += len;
    }
    av_free_packet(&avFramePkt);

    return bDecoded;
}

void CFFRtp::ConvertToRGB()
{
    // Convert to RGB format
    if (m_pSwsCtx != NULL) {
        sws_scale(m_pSwsCtx, (const uint8_t * const*)m_pPicture->data,
                  m_pPicture->linesize, 0, m_nFrameHeight,
                  m_pDestPicture->data,
                  m_pDestPicture->linesize);
    }
}

void CFFRtp::SetupDestinationImage()
{
    enum PixelFormat dst_pix_fmt;
    enum PixelFormat src_pix_fmt;

    if (m_eDecodePixelFormat == eRGB24)
        dst_pix_fmt = PIX_FMT_RGB24;
    else
        dst_pix_fmt = PIX_FMT_RGB32;  // BGRX on little-endian

    src_pix_fmt = m_pVideoCodecCtx->pix_fmt;

    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);
    m_pSwsCtx = NULL;

    m_pSwsCtx = sws_getContext(
                      m_nFrameWidth, m_nFrameHeight, src_pix_fmt,
                      m_nFrameWidth, m_nFrameHeight, dst_pix_fmt,
                      SWS_BICUBIC, NULL, NULL, NULL);

    if (m_pSwsCtx == NULL) {
        SetErrorString("SwsGetContext failed.");
        return;
    }

    if (m_pDestPicture != NULL)
        av_free(m_pDestPicture);
    m_pDestPicture = avcodec_alloc_frame();

    // Determine required buffer size to hold destination image and allocate buffer
    m_nDecodeSize = avpicture_get_size(dst_pix_fmt, m_nFrameWidth, m_nFrameHeight);

    if (m_pDecodeBuffer != NULL)
        delete [] m_pDecodeBuffer;

    m_pDecodeBuffer = new unsigned char[m_nDecodeSize];

    avpicture_fill((AVPicture *)m_pDestPicture, (uint8_t *)m_pDecodeBuffer,
                   dst_pix_fmt, m_nFrameWidth, m_nFrameHeight);
}

void CFFRtp::HandleVideoPacket(char * a_pData, int nLen)
{
    if (m_bError == true)
        return;

    if (m_bVideoFound == false)
    	return;

    AVPacket avpkt;

    if (a_pData != NULL) {
        if (m_pVideoRTPParser->GetPacket((uint8_t *)a_pData, nLen, &avpkt)) {
            if (m_bNativeOutput == true) {
                ParsePacket((uint8_t *)avpkt.data, (int)avpkt.size);
            }
            else {
                if (m_bNeedsFrameParsing) {
                    ParsePacket((uint8_t *)avpkt.data, (int)avpkt.size);
                }
                else {
                    m_nInFrameCount++;
                    SaveFrameBuffer((uint8_t *)avpkt.data, (int) avpkt.size);
                }
            }

            av_free_packet(&avpkt);
        }
    }
}

void CFFRtp::HandleAudioPacket(char * a_pData, int a_nLen)
{
    if (m_bAudioFound == false ||
            m_pAudioRTPParser == NULL ||
            a_pData == NULL)
        return;

    if (m_bNativeOutput == false)
        return;

    AVPacket avpkt;

    if (m_pAudioRTPParser->GetPacket((uint8_t *)a_pData, a_nLen, &avpkt)) {
        m_nInAudioFrameCount++;

        SaveFrameBuffer((uint8_t *)avpkt.data, (int) avpkt.size, false);

        av_free_packet(&avpkt);
    }
}

void CFFRtp::CheckFrameDimensions()
{
    if (m_nFrameWidth != m_pVideoCodecCtx->width || m_nFrameHeight != m_pVideoCodecCtx->height) {
        m_nFrameWidth = m_pVideoCodecCtx->width;
        m_nFrameHeight = m_pVideoCodecCtx->height;
        SetupDestinationImage();
    }

    CRTPMediaHandler *pStdMediaHandler =
                dynamic_cast<CRTPMediaHandler*>(GetStandardMediaHandler());
    if (pStdMediaHandler == NULL) {
        return;
    }

    pStdMediaHandler->SetVideoResolution(m_nFrameWidth, m_nFrameHeight);
}

void CFFRtp::CreateOutput()
{
    CreateThumbnails();
    CreateStandardMedia();
}

void CFFRtp::CreateThumbnails()
{
    CSFrame *pCSFrame = m_qTNQueueSource->GetFrame();

    if (pCSFrame == NULL) {
        return;
    }

    if (GetThumbnailHandler() == NULL) {
        return;
    }

    CSBuffer *pBuffer = pCSFrame->GetBuffer();
    CFrameData *pFrameData = (CFrameData*)pBuffer->GetBuffer(0);
    if (pFrameData == NULL)
        return;

    CacheFrameData(pCSFrame);

    if (GetThumbnailHandler()->IsTNDirectoryCreated() == false) {
        GetThumbnailHandler()->CreateTNDirectory(m_strMediaDestDir);
    }

    if (m_bGotResolution == false) {
        uint8_t *pFrameBuffer = m_pCachedFrameData->GetData();
        int nFrameBuffLen = m_pCachedFrameData->GetLength();

        DecodeFrame(pFrameBuffer, nFrameBuffLen);
    }
    else {
        CreateThumbnailsFromBuffer();
    }

    pFrameData->DeleteData();

    m_qTNQueueSource->ReleaseFrame(pCSFrame);
}

void CFFRtp::CacheFrameData(CSFrame *a_pCSFrame)
{
    if (a_pCSFrame == NULL)
        return;

    CSBuffer *pBuffer = a_pCSFrame->GetBuffer();
    CFrameData *pFrameData = (CFrameData*)pBuffer->GetBuffer(0);
    if (pFrameData == NULL)
        return;

    if (m_pCachedFrameData != NULL)
        DeleteFrameData();

    m_pCachedFrameData = new CFrameData(pFrameData->GetData(), pFrameData->GetLength(),
                                  pFrameData->IsVideoFrame(), pFrameData->GetRTPTimestamp(),
                                  pFrameData->GetTimestamp());
}

void CFFRtp::DeleteFrameData()
{
    delete m_pCachedFrameData;
    m_pCachedFrameData = NULL;
}

void CFFRtp::CreateThumbnailsFromBuffer()
{
    if(m_pCachedFrameData == NULL)
        return;

    CreatePrimaryTN(m_pCachedFrameData);
    CreateStreamTN(m_pCachedFrameData);
}

void CFFRtp::CreatePrimaryTN(CFrameData *a_pFrameData)
{
    if (GetThumbnailHandler()->IsPrimaryTNEnabled() == false ||
            GetThumbnailHandler()->IsPrimaryTNCreated() == true)
        return;

    uint8_t *pFrameBuffer = a_pFrameData->GetData();
    int nFrameBuffLen = a_pFrameData->GetLength();

    uint64_t nCurrentTimestamp = a_pFrameData->GetTimestamp();

    bool bDecodedFrame = true;
    if (m_bNeedsFrameParsing == false) {// This is for mpeg4 streams. Decoding everything till getting one decoded key frame.
        if (DecodeFrame(pFrameBuffer, nFrameBuffLen) == false) {
            bDecodedFrame = false;
        }
    }

    // How to handle gaps??
    if ((nCurrentTimestamp - m_nBeginTimestamp) < GetThumbnailHandler()->GetPrimaryTNOffset()) {
        return;
    }

    if (m_bNeedsFrameParsing == true) {
        if (DecodeFrame(pFrameBuffer, nFrameBuffLen) == false) {
            bDecodedFrame = false;
        }
    }

    if (bDecodedFrame == true) {
        ConvertToRGB();
        if (CreateSingleThumbnail("tn",
                GetThumbnailHandler()->GetPrimaryTNWidth(),
                GetThumbnailHandler()->GetPrimaryTNHeight()) == true) {
            GetThumbnailHandler()->SetPrimaryTNCreated();

            m_nCurrentTNTS = nCurrentTimestamp;
            GetThumbnailHandler()->IncrementTNCount();
        }
    }
}

void CFFRtp::CreateStreamTN(CFrameData *a_pFrameData)
{
    if (GetThumbnailHandler()->IsStreamTNEnabled() == false ||
            GetThumbnailHandler()->GetStreamTNFrequency() <= 0) {
        return;
    }

    uint8_t *pFrameBuffer = a_pFrameData->GetData();
    int nFrameBuffLen = a_pFrameData->GetLength();

    uint64_t nCurrentTimestamp = a_pFrameData->GetTimestamp();

    if (GetThumbnailHandler()->GetNextTNOffset() == 0 ||
           (nCurrentTimestamp - m_nBeginTimestamp) >=
           GetThumbnailHandler()->GetNextTNOffset()) {
        if (m_bNeedsFrameParsing == true) {
            if (DecodeFrame(pFrameBuffer, nFrameBuffLen) == false) {
                return;
            }
        }

        char strFileName[20];
        sprintf(strFileName, "%"PRIu64"", nCurrentTimestamp);

        ConvertToRGB();
        if (CreateSingleThumbnail(strFileName,
                GetThumbnailHandler()->GetStreamTNWidth(),
                GetThumbnailHandler()->GetStreamTNHeight()) == true) {
            GetThumbnailHandler()->SetNextTNOffset(GetThumbnailHandler()->GetStreamTNFrequency());
            GetThumbnailHandler()->SetFirstStreamTNCreated();

            m_nCurrentTNTS = nCurrentTimestamp;
            GetThumbnailHandler()->IncrementTNCount();
        }
    }
}

bool CFFRtp::CreateSingleThumbnail(const char* pFileName, int nTNWidth, int nTNHeight)
{
    char strFileName[255];
    sprintf(strFileName,"%s/%s.jpg",
            GetThumbnailHandler()->GetTNDirectory().c_str(), pFileName);

    return CreateSingleImage(m_pDecodeBuffer, strFileName, nTNWidth, nTNHeight);
}

bool CFFRtp::CreateSingleImage(unsigned char * a_pInputBuffer, char * a_pStrFileName,
                               int a_nThumbnailWidth, int a_nThumbnailHeight)
{
    if (a_pInputBuffer == NULL)
        return false;

    if (false == GetThumbnailHandler()->ReCreateThumbnailGenerator(
            m_nFrameWidth, m_nFrameHeight, m_eDecodePixelFormat)) {
        SetErrorString("ThumbnailGenerator object creation failed.");
        return false;
    }

    const int tnSize = a_nThumbnailWidth * a_nThumbnailHeight * 3;
    unsigned char *pTNBuffer = new unsigned char[tnSize];
    memset(pTNBuffer, 0x00, tnSize);

    GetThumbnailHandler()->SetThumbnailResolution(a_nThumbnailWidth, a_nThumbnailHeight);

    int nTNSize = GetThumbnailHandler()->CreateImage(&a_pInputBuffer, &pTNBuffer, tnSize);

    if (nTNSize <= 0) {
        delete [] pTNBuffer;
        SetErrorString("Thumbnail-image creation failed.");
        return false;
    }

    GetThumbnailHandler()->CreateImageFile(&pTNBuffer, nTNSize, a_pStrFileName);

    delete [] pTNBuffer;

    m_nBytesWritten += nTNSize;

    return true;
}

void CFFRtp::PrepareForFinalThumbnails()
{
    while (m_qTNQueueSource->Size() > 0) {
        if (((GetThumbnailHandler()->IsPrimaryTNEnabled() == true) &&
                (GetThumbnailHandler()->IsPrimaryTNCreated() == false)) ||
                (GetThumbnailHandler()->IsStreamTNEnabled() == true))
        {
            CSFrame *pCSFrame = m_qTNQueueSource->GetFrame();
            if (pCSFrame != NULL) {
            	if (m_qTNQueueSource->Size() == 1)
            	{
                    CacheFrameData(pCSFrame);
            	}

                m_qTNQueueSource->ReleaseFrame(pCSFrame);
             }
        }
        else {
            break;
        }
    }

    GetThumbnailHandler()->SetFinalThumbnailsStatus(true);
}

void CFFRtp::CreateFinalThumbnails()
{
    CThumbnailHandler *pThumbnailHandler = GetThumbnailHandler();
    if (pThumbnailHandler == NULL)
        return;

    //If it is less than 10secs stream, forcing to create
    //thumbnails from cache frame data.
    GetThumbnailHandler()->SetPrimaryTNOffset(0);
    GetThumbnailHandler()->SetNextTNOffset(0);

    m_bStopStreamIssued = true;

    // If Primary thumbnail is not yet created, then drain the queue and create the thumbnail with latest frame.
    if (GetThumbnailHandler()->IsPrimaryTNCreated() == false) {
        if (m_bLiveMode == false) {
            PrepareForFinalThumbnails();
            CreateThumbnailsFromBuffer();
        }
        else {
            while ((!GetThumbnailHandler()->IsFinalThumbailsProcessed() &&
                    !GetThumbnailHandler()->IsPrimaryTNCreated()) ||
                   (!m_qTNQueueSource->Size() == 0)) {
                LightSleep();
            }
        }
    }

    if (m_bLiveMode == true)
        Stop();

    // Still didn't get the correct frame, then create a black frame as thumbnail.
    if (IsThumbnailCreationEnabled() == true) {
        if (GetThumbnailHandler()->IsPrimaryTNEnabled() == true &&
                GetThumbnailHandler()->IsPrimaryTNCreated() == false) {
            CreateBlackThumbnails();
        }
    }
}

void CFFRtp::StopStream()
{
    CreateFinalThumbnails();
    CompleteStandardMedia();
}

void CFFRtp::CreateBlackThumbnails()
{
    if (m_pDecodeBuffer != NULL)
        delete [] m_pDecodeBuffer;
    m_pDecodeBuffer = NULL;

    if (m_nFrameWidth == 0 || m_nFrameHeight == 0) {
        m_nFrameWidth = DEFAULT_FRAME_WIDTH;
        m_nFrameHeight = DEFAULT_FRAME_HEIGHT;
    }

    m_nDecodeSize = m_nFrameWidth * m_nFrameHeight * 4;
    m_pDecodeBuffer = new unsigned char[m_nDecodeSize];

    CreateSingleThumbnail("tn",
            GetThumbnailHandler()->GetPrimaryTNWidth(),
            GetThumbnailHandler()->GetPrimaryTNHeight());

    GetThumbnailHandler()->IncrementTNCount();
}

void CFFRtp::CreateStandardMedia()
{
    CSFrame *pCSFrame = m_qTNQueueSource->GetFrame();

    if (pCSFrame == NULL) {
        return;
    }

    CRTPMediaHandler *pStdMediaHandler =
            dynamic_cast<CRTPMediaHandler*>(GetStandardMediaHandler());
    if (pStdMediaHandler == NULL) {
        return;
    }

    CSBuffer *pBuffer = pCSFrame->GetBuffer();
    CFrameData *pFrameData = (CFrameData*)pBuffer->GetBuffer(0);
    if (pFrameData == NULL)
        return;

    uint8_t *pFrameBuffer = pFrameData->GetData();
    int nFrameBuffLen = pFrameData->GetLength();
    bool bVideoFrame = pFrameData->IsVideoFrame();

    HandleSSRCChange(bVideoFrame);

    uint64_t nPTSValue = GetPTSValue(bVideoFrame, pCSFrame->GetTimeStamp());

   if (m_bGotResolution == false && m_bVideoFound == true) {
        DecodeFrame(pFrameBuffer, nFrameBuffLen);
    }

    if (true == pStdMediaHandler->WriteFrame(pFrameBuffer,
                                             nFrameBuffLen,
                                             bVideoFrame,
                                             nPTSValue)) {
        m_nBytesWritten += nFrameBuffLen;
    }

    pFrameData->DeleteData();

    m_qTNQueueSource->ReleaseFrame(pCSFrame);
}

void CFFRtp::CompleteStandardMedia()
{
    if (m_bWrittenTrailer == true)
        return;

    CRTPMediaHandler *pStdMediaHandler =
            dynamic_cast<CRTPMediaHandler*>(GetStandardMediaHandler());
    if (pStdMediaHandler == NULL) {
        return;
    }

    while(m_qTNQueueSource->Size() != 0) {
        if (m_bLiveMode == false) {
            CreateStandardMedia();
        }
        else {
            LightSleep();
        }
    }

    if (m_bLiveMode == true)
        Stop();

    pStdMediaHandler->WriteOutputTrailer();
    m_bWrittenTrailer = true;
}

void CFFRtp::CreateQueues(int nId) {
    ostringstream qid;
    qid << "TNQueueSource_" << nId;
    m_qTNQueueSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qTNQueueSource->SetMaxBytes(20 * 1024 * 1024);

    m_qTNQueueSink = CSQueueManager::Instance()->GetQueueSink(qid.str());
}

void CFFRtp::DeleteQueues() {
    if (m_qTNQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qTNQueueSource);

    if (m_qTNQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qTNQueueSink);
}

void CFFRtp::PrintStats()
{
    char buffer[64];
    printf("Processed: %.2f%% %s\n", m_PercentComplete, GetTimeString(m_nElapsed, buffer));
}

int CFFRtp::Run()
{
    while (!m_bExit) {
        CreateOutput();
        LightSleep();

        if (m_bStopStreamIssued == true) {
            PrepareForFinalThumbnails();
            CreateThumbnailsFromBuffer();
        }
    }

    return 0;
}

int CFFRtp::FindNALUnit(uint8_t *a_pBuff, int a_nSize,
                        int *a_nNAL_Start, int *a_nNAL_End)
{
    int i = 0;
    *a_nNAL_Start = 0;
    *a_nNAL_End = 0;

    while((a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01) &&
          (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0 ||
                  a_pBuff[i + 3] != 0x01)) {
        i++;
        if (i + 4 >= a_nSize) {
            return 0; //Did not find NAL start.
        }
    }

    if (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01) {
        i++;
    }

    if (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01) {
        printf("There is some error. Should never happen. \n");
        return 0;//Error, should never happen.
    }

    i += 3;
    *a_nNAL_Start = i;

    while((a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0) &&
          (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01)) {
        i++;

        if(i + 3 >= a_nSize) {
            *a_nNAL_End = a_nSize;
            return (*a_nNAL_End - *a_nNAL_Start); //Did not find NAL end, stream ended first. Assuming that the nal
                                                  //end and stream end are same.
        }
    }

    *a_nNAL_End = i;

    return (*a_nNAL_End - *a_nNAL_Start);
}

void CFFRtp::ParseSPSAndPPSValues(uint8_t *a_pBuffer, int a_nBuffSize)
{
    if ((m_bGotResolution) ||
            (m_bFoundSPSAndPPS)) {
        return;
    }

    int nNalStart = 0;
    int nNalEnd = 0;

    int nSpropData_Size = 0;

    uint8_t *pBuff = a_pBuffer;
    int nRemaining = a_nBuffSize;

    unsigned char const start_code[NAL_START_SIZE] = {0x00, 0x00, 0x01};

    while(nRemaining > 0) {
        int nRes = FindNALUnit(pBuff, nRemaining, &nNalStart, &nNalEnd);
        if (nRes > 0) {
            pBuff += nNalStart;

            int nNalType = pBuff[0] & 0x1f;

            if (nNalType == NAL_TYPE_SPS) {
                m_nSPSData_Size = nRes + NAL_START_SIZE;
                m_pSPSData = new uint8_t[m_nSPSData_Size];
                memcpy(m_pSPSData, start_code, NAL_START_SIZE);
                memcpy(m_pSPSData + NAL_START_SIZE, pBuff, m_nSPSData_Size - NAL_START_SIZE);
            }
            if (nNalType == NAL_TYPE_PPS) {
                m_nPPSData_Size = nRes + NAL_START_SIZE;
                m_pPPSData = new uint8_t[m_nPPSData_Size];
                memcpy(m_pPPSData, start_code, NAL_START_SIZE);
                memcpy(m_pPPSData + NAL_START_SIZE, pBuff, m_nPPSData_Size - NAL_START_SIZE);
            }
            nRemaining -= nNalEnd;
            pBuff += (nNalEnd - nNalStart);
        }
        else {
            break;
        }
    }

    if (m_nSPSData_Size > 0 && m_nPPSData_Size > 0) {
        nSpropData_Size = m_nSPSData_Size + m_nPPSData_Size;
        uint8_t *pSpropData = new uint8_t[nSpropData_Size +
                                          FF_INPUT_BUFFER_PADDING_SIZE];
        memset(pSpropData + nSpropData_Size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
        memcpy(pSpropData, m_pSPSData, m_nSPSData_Size);
        memcpy(pSpropData + m_nSPSData_Size, m_pPPSData, m_nPPSData_Size);

        m_pVideoCodecCtx->extradata = pSpropData;
        m_pVideoCodecCtx->extradata_size = nSpropData_Size;

        CRTPMediaHandler *pStandardMediaHandler =
                        dynamic_cast<CRTPMediaHandler*>(GetStandardMediaHandler());
        if (pStandardMediaHandler == NULL)
            return;
        pStandardMediaHandler->SetSpropData(m_pVideoCodecCtx->extradata,
                                            m_pVideoCodecCtx->extradata_size);
        m_bFoundSPSAndPPS = true;
    }
}

uint64_t CFFRtp::GetPTSValue(bool bVideoFrame, uint64_t a_nWallClockTime)
{
    if (bVideoFrame) {
        return GetVideoPTSValue(a_nWallClockTime);
    }
    else {
        return GetAudioPTSValue(a_nWallClockTime);
    }
}

uint64_t CFFRtp::GetVideoPTSValue(uint64_t a_nWallClockTime)
{
    if (m_pPTSGeneratorForVideo == NULL ||
            m_pVideoRTPParser == NULL)
        return 0;

    return (m_pPTSGeneratorForVideo->GetPTSForRTPTimestamp(
                                     m_pVideoRTPParser->GetRawTimestamp(),
                                     a_nWallClockTime));
}

uint64_t CFFRtp::GetAudioPTSValue(uint64_t a_nWallClockTime)
{
    if (m_pPTSGeneratorForAudio == NULL ||
            m_pAudioRTPParser == NULL)
        return 0;

    return (m_pPTSGeneratorForAudio->GetPTSForRTPTimestamp(
                                     m_pAudioRTPParser->GetRawTimestamp(),
                                     a_nWallClockTime));
}

void CFFRtp::HandleSSRCChange(bool a_bVideoFrame)
{
    if (a_bVideoFrame) {
        HandleVideoSSRCChange();
    }
    else {
        HandleAudioSSRCChange();
    }
}

void CFFRtp::HandleVideoSSRCChange()
{
    if ((m_pVideoRTPParser == NULL) ||
            (m_pPTSGeneratorForVideo == NULL))
        return;

    unsigned long nCurrentSSRC = m_pVideoRTPParser->GetSSRC();

    if (m_nInputVideoSSRC == 0) {
        m_nInputVideoSSRC = nCurrentSSRC;
    }
    else if (nCurrentSSRC != m_nInputVideoSSRC) {
        m_nInputVideoSSRC = nCurrentSSRC;

        m_pPTSGeneratorForVideo->SetDiscontinuity(0);
    }
}

void CFFRtp::HandleAudioSSRCChange()
{
    if ((m_pAudioRTPParser == NULL) ||
            (m_pPTSGeneratorForAudio == NULL))
        return;

    unsigned long nCurrentSSRC = m_pAudioRTPParser->GetSSRC();

    if (m_nInputAudioSSRC == 0) {
        m_nInputAudioSSRC = nCurrentSSRC;
    }
    else if (nCurrentSSRC != m_nInputAudioSSRC) {
        m_nInputAudioSSRC = nCurrentSSRC;

        m_pPTSGeneratorForAudio->SetDiscontinuity(0);
    }
}

