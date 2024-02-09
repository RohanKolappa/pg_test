/*
 * rtpaudiodecoderthr.cpp
 *
 *  Created on: Apr 25, 2011
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include <sstream>

#include "common.hh"
#include "rtpaudiodecoderthr.hh"
#include "renderermessage.hh"
#include "aligned_malloc.h"
#include "FFmpegUtils.hh"
#include "MPEG4AudioRTPParser.hh"
#include "MPEG4LATMAudioRTPParser.hh"
#include "AudioRenderer.hh"

using namespace std;


CRTPAudioDecoderThread::CRTPAudioDecoderThread(CStreamer *a_pStreamer,
        MediaSubsession *a_pSubsession)
    : CDetachedThread(a_pStreamer, "RTPAudioDecoder")
{
    m_bError = false;
    m_pDecodeBuffer = NULL;
    m_nPacketsDecoded = 0;
    m_nFramesDecoded = 0;
    m_pCodecCtx = NULL;
    m_pCodec = NULL;
    m_bSendFrameInfo = false;
    m_pRTPParser = NULL;
    m_pParserContext = NULL;
    m_qQueueSource = NULL;
    m_qRendererSink = NULL;
    m_qRendererSource = NULL;
    m_pAudioRenderer = NULL;

    m_pSubsession = a_pSubsession;

    // Initialize FFmpeg
    FFmpegUtils::Initialize();

    // Setup Codec Context
    m_pCodecCtx = avcodec_alloc_context();

    m_pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;

    // Find the decoder
    if (!FindDecoder()) {
        m_sErrorString = "Could not find codec ";
        m_sErrorString.append(m_pSubsession->codecName());
        SYSLOG_ERROR("Cound not open codec %s", m_pSubsession->codecName());
        m_bError = true;
        return;
    }

    // Prime the decoder
    m_pRTPParser->RetrieveCodecConfig(m_pSubsession, m_pCodecCtx);

    // Some codec types need additional setup
    SetupCodecParameters();

    // Open codec for decoding
    if (!FFmpegUtils::OpenCodec(m_pCodecCtx, m_pCodec)) {
        m_sErrorString = "Could not open codec";
        SYSLOG_ERROR("Cound not open codec %s", m_pSubsession->codecName());
        m_bError = true;
        return;
    }


    m_pDecodeBuffer = (uint8_t *) aligned_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE, DEFAULT_ALIGNMENT);


    // Get the handle to the packet parser for the codec
    m_pParserContext = av_parser_init(m_pCodecCtx->codec_id);

    // Create queue for reading data from sender thread
    ostringstream qid;
    qid << "ToRTPAudioDecoder_" << GetId();
    m_qQueueSource =
            CSQueueManager::Instance()->GetQueueSource(qid.str());

    // Create the queues for sending/receiving data from renderer
    m_qRendererSink =
            CSQueueManager::Instance()->GetQueueSink("ToAudioRenderer");

    // TODO: handle messages from the audio renderer (mute/volume)
    m_qRendererSource =
            CSQueueManager::Instance()->GetQueueSource("FromAudioRenderer");

}

CRTPAudioDecoderThread::~CRTPAudioDecoderThread()
{
    // Release ffmpeg resources
    av_parser_close(m_pParserContext);
    FFmpegUtils::CloseCodec(m_pCodecCtx);
    av_free(m_pCodecCtx->extradata);
    av_free(m_pCodecCtx);
    aligned_free(m_pDecodeBuffer);

    // Release our RTP parser
    delete m_pRTPParser;

    // Release the queues
    if (m_qQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);

    if (m_qRendererSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qRendererSource);

    if (m_qRendererSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qRendererSink);

    delete m_pAudioRenderer;
}

int CRTPAudioDecoderThread::RunThread()
{
    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        int rc = DecodeProcessor();
        if (rc < 0) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            break;
        }
        if (rc == 0)
            LightSleep();
    }

    return 0;
}

void CRTPAudioDecoderThread::SetupCodecParameters() {
    switch (m_pCodecCtx->codec_id) {
    case CODEC_ID_PCM_MULAW:
    case CODEC_ID_PCM_ALAW:
        m_pCodecCtx->channels = 1;
        m_pCodecCtx->sample_rate = 8000;
        break;
    case CODEC_ID_SPEEX:
        m_pCodecCtx->channels = m_pSubsession->numChannels();
        m_pCodecCtx->sample_rate = m_pSubsession->rtpTimestampFrequency();
        break;
//        if (m_pCodecCtx->channels <= 0 || m_pCodecCtx->channels > 2)
//            m_pCodecCtx->channels = 1;
//        if (m_pCodecCtx->sample_rate != 8000 ||
//                m_pCodecCtx->sample_rate != 16000 ||
//                m_pCodecCtx->sample_rate != 32000)
//            m_pCodecCtx->sample_rate = 16000;
//        break;
    default:
        break;
    }

}
bool CRTPAudioDecoderThread::FindDecoder() {

    if (strcasecmp(m_pSubsession->codecName(), "MPEG4-GENERIC") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_AAC;
        m_pRTPParser = new CMPEG4AudioRTPParser(m_pSubsession->rtpPayloadFormat());
    }
    else if (strcasecmp(m_pSubsession->codecName(), "MP4A-LATM") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_AAC;
        m_pRTPParser = new CMPEG4LATMAudioRTPParser(m_pSubsession->rtpPayloadFormat());
    }
    else if (strcasecmp(m_pSubsession->codecName(), "PCMU") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_PCM_MULAW;
        m_pRTPParser = new CRTPParser(m_pSubsession->rtpPayloadFormat(), false);
    }
    else if (strcasecmp(m_pSubsession->codecName(), "PCMA") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_PCM_ALAW;
        m_pRTPParser = new CRTPParser(m_pSubsession->rtpPayloadFormat(), false);
    }
    else if (strcasecmp(m_pSubsession->codecName(), "SPEEX") == 0) {
        m_pCodecCtx->codec_id = CODEC_ID_SPEEX;
        m_pRTPParser = new CRTPParser(m_pSubsession->rtpPayloadFormat(), false);
    }
    else {
        return false;
    }

    m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
    if (m_pCodec == NULL)
        return false;
    return true;
}

#if 0
int CRTPAudioDecoderThread::ParsePacket(uint8_t * packet_buffer, int packet_size)
{
    uint8_t *in_data = packet_buffer;
    int in_len = packet_size;
    uint8_t *frame_buffer;
    int frame_size;
    uint64_t pts = 0, dts = 0, pos = 0;

    // Send the packets to the frame parser. It will collect multiple packets
    // till it detects a frame and return a frame in frame_buffer.
    while (in_len) {
        int len = av_parser_parse2(m_pParserContext, m_pCodecCtx, &frame_buffer, &frame_size,
                in_data, in_len, pts, dts, pos);
        in_data += len;
        in_len  -= len;

        if (frame_size > 0) {
            m_nFramesDecoded++;
            DecodeFrame(frame_buffer, frame_size);
        }
    }
    return 0;
}
#endif

void CRTPAudioDecoderThread::DecodeFrame(uint8_t *frame_buffer, int frame_size)
{
    AVPacket avFramePkt;

    av_init_packet(&avFramePkt);
    avFramePkt.data = frame_buffer;
    avFramePkt.size = frame_size;


    //uint8_t *outbuf = (uint8_t *) aligned_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE, DEFAULT_ALIGNMENT);

    while (avFramePkt.size > 0) {
        int out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
//        printf("Decoding frame of size %d\n", frame_size);
        int len = avcodec_decode_audio3(m_pCodecCtx, (short *)m_pDecodeBuffer, &out_size, &avFramePkt);
        if (len < 0) {
            printf("Failed decode frame: %d\n", len);
            break;
        }
//        printf("Decoded frame: %d %d\n", len, out_size);
        if (out_size) {
            if (m_pAudioRenderer == NULL || out_size > m_pAudioRenderer->GetPacketSize()) {
                DetermineFrameSize(out_size);
                InitializeAudioRenderer();
            }
            m_pAudioRenderer->RenderSample(m_pDecodeBuffer, out_size);
            m_nFramesDecoded++;
        }

        avFramePkt.size -= len;
        avFramePkt.data += len;
    }
}


int CRTPAudioDecoderThread::DecodeProcessor()
{

    int retval = 0;

    if (m_qQueueSource == NULL)
        return 0;

    CSFrame *sFrame = m_qQueueSource->GetFrame();

    AVPacket avpkt;

    if (sFrame) {

        // Extract RTP payload into avpkt
        if (m_pRTPParser->GetPacket((uint8_t *)sFrame->GetData(), sFrame->GetLength(), &avpkt)) {

            // Send packet onwards for decoding
            DecodeFrame(avpkt.data, avpkt.size);
            av_free_packet(&avpkt);
            m_nPacketsDecoded++;
        }

        m_qQueueSource->ReleaseFrame(sFrame);
        retval = 1;
    }

    return retval;
}

void CRTPAudioDecoderThread::DetermineFrameSize(int packet_size)
{
    switch (m_pCodecCtx->codec_id) {
    case CODEC_ID_PCM_MULAW:
    case CODEC_ID_PCM_ALAW:
        m_pCodecCtx->frame_size = packet_size/2;
        break;
    default:
        break;
    }
}

void CRTPAudioDecoderThread::InitializeAudioRenderer()
{
    printf("Audio fmt = %d frame size = %d sample rate = %d channels = %d\n",
            m_pCodecCtx->sample_fmt,
            m_pCodecCtx->frame_size,
            m_pCodecCtx->sample_rate,
            m_pCodecCtx->channels);
    if (m_pCodecCtx->frame_size <= 0)
        return;
    if (m_pCodecCtx->sample_rate <= 0)
        return;
    if (m_pCodecCtx->channels <= 0)
        return;

    // Map ffmpeg's audio format to our format
    CAudioRenderer::eAudioFormat audioFormat;
    switch (m_pCodecCtx->sample_fmt) {
    case AV_SAMPLE_FMT_S16:
        audioFormat = CAudioRenderer::AFSigned16;
        break;
    case AV_SAMPLE_FMT_S32:
        audioFormat = CAudioRenderer::AFSigned32;
        break;
    case AV_SAMPLE_FMT_U8:
        audioFormat = CAudioRenderer::AFUnsigned8;
        break;
    case AV_SAMPLE_FMT_FLT:
        audioFormat = CAudioRenderer::AFFloat;
        break;
    default:
        audioFormat = CAudioRenderer::AFUnsupported;
        break;
    }

    if (m_pAudioRenderer != NULL)
        delete m_pAudioRenderer;

    m_pAudioRenderer = new CAudioRenderer(m_pCodecCtx->sample_rate, m_pCodecCtx->channels,
            m_pCodecCtx->frame_size, audioFormat);

    m_pAudioRenderer->StartStreaming();
}

void CRTPAudioDecoderThread::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectCounterParam("AudioPacketsDecoded", m_nPacketsDecoded);
    a_pCmd->SetObjectCounterParam("AudioFramesDecoded", m_nFramesDecoded);
    if (m_bError)
        a_pCmd->SetObjectParam("ErrorString", m_sErrorString);
    if (m_pRTPParser != NULL)
        a_pCmd->SetObjectErrorCounterParam("AudioRTPErrors", m_pRTPParser->GetPacketErrors());
    if (m_pAudioRenderer) {
        a_pCmd->SetObjectParam("AudioBufferLength",
                m_pAudioRenderer->GetBufferLength());
        if (a_pCmd->StatusLevel(2)) {
            a_pCmd->SetObjectParam("AudioAvgBufferLength",
                    m_pAudioRenderer->GetAvgBufferLength());
            a_pCmd->SetObjectParam("AudioAvgBufferTime",
                    m_pAudioRenderer->GetAvgBufferTime());
        }
        a_pCmd->SetObjectCounterParam("AudioSamplesRendered",
                                      m_pAudioRenderer->GetRenderedSamples());
        a_pCmd->SetObjectByteCounterParam("AudioBytesRendered",
                                      m_pAudioRenderer->GetRenderedBytes());
        if (a_pCmd->StatusLevel(2)) {
            a_pCmd->SetObjectByteCounterParam("AudioBytesRequested",
                                          m_pAudioRenderer->GetRequestedBytes());
        }
        a_pCmd->SetObjectCounterParam("AudioSamplesDropped",
                                      m_pAudioRenderer->GetDroppedSamples());
        a_pCmd->SetObjectCounterParam("AudioSilenceInserted",
                                      m_pAudioRenderer->GetSilenceCount());
        a_pCmd->SetObjectParam("AudioErrorStatus",
                                      m_pAudioRenderer->GetErrorStatus());
        a_pCmd->SetObjectParam("AudioFrequency",
                                      m_pAudioRenderer->GetFrequency());
        if (a_pCmd->StatusLevel(2)) {
            a_pCmd->SetObjectParam("AudioInputFreq",
                    m_pAudioRenderer->GetInputFrequncy());
            a_pCmd->SetObjectParam("AudioResampleAdjust",
                    m_pAudioRenderer->GetResampleAdjust());
        }
        a_pCmd->SetObjectParam("AudioChannels",
                                      m_pAudioRenderer->GetChannels());
        a_pCmd->SetObjectParam("AudioSampleSize",
                                      m_pAudioRenderer->GetSampleSize());
        a_pCmd->SetObjectParam("AudioSampleTime",
                                      m_pAudioRenderer->GetSampleTime());
    }
}

