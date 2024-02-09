/*
 * v2dplayer.cpp
 *
 *  Created on: Jul 5, 2010
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"
#include <math.h>
#include <sstream>

#include "v2dplayertx.hh"
#include "v2ddecoderthr.hh"
#include "vframedecoder.hh"
#include "colortransform.h"
#include "AudioRenderer.hh"
#include "renderermessage.hh"
#include "ScapProcessor.hh"

using namespace V2dLibColorTransform;
using namespace std;

CV2dPlayerTx::CV2dPlayerTx(int aId, int pixelformat) :
    CSender(aId)
{
    m_nPortType = V2DPLAYERPORT;
    m_nPixelFormat = pixelformat;

    m_nVideoPackets = 0;
    m_nVideoBytes = 0;
    m_nAudioPackets = 0;
    m_nAudioBytes = 0;
    m_nControlPackets = 0;
    m_nControlBytes = 0;

    m_srd = NULL;
    m_outBufferLen = 0;
    m_outBuffer = NULL;
    m_nSessionId = v2dLibGetRandomNumber();
    m_nQueryId = 0;
    m_bRFBReady = false;
    m_scapProcessor = new CScapProcessor();

    CreateSRD();

    // Create the queues for sending/receiving data from renderer
    ostringstream qid;
    qid << "ToRenderer_" << m_nId;
    m_qRendererSink =
        CSQueueManager::Instance()->GetQueueSink(qid.str());
    qid.str("");
    qid << "FromRenderer_" << m_nId;
    m_qRendererSource =
        CSQueueManager::Instance()->GetQueueSource(qid.str());

    m_bIsAudioSrcRTP = false;
    m_bIsVideoSrcRTP = false;

    // Create decoder thread
    m_pDecoderThread = new CV2DDecoderThread(aId, m_qRendererSink, m_qRendererSource);
    m_pDecoderThread->Start();

    m_pAudioDecodeBuffer = new uint8_t[2048];

    // Some measurements showed the V2D sampling rate to be appox. 16316.3
    // Using 16316 here causes the fewest silence insertions currently (on linux)
    m_pAudioRenderer = new CAudioRenderer(16316, 2, 253,
            CAudioRenderer::AFSigned16, true, 16000);
}

CV2dPlayerTx::~CV2dPlayerTx()
{

    m_pDecoderThread->Stop();

    DestroySRD();

    if (m_qRendererSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qRendererSink);
    if (m_qRendererSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qRendererSource);

    delete [] m_outBuffer;
    delete [] m_pAudioDecodeBuffer;
    delete m_pAudioRenderer;
    delete m_scapProcessor;
}


void CV2dPlayerTx::SRDNewFrameCallback(void *a_object)
{
    CV2dPlayerTx * This = static_cast<CV2dPlayerTx *> (a_object);
    This->ProcessSRDFrame(true);
}

int CV2dPlayerTx::ProcessStream()
{


    // Process Alt stream first
    if (ProcessAltStream() > 0) {
        return 1;
    }


    int retval = 0;
    m_bSRDPacketSent = false;
    CSFrame *sFrame = NULL;

    if (m_qQueueSource != NULL) {

        sFrame = m_qQueueSource->GetFrame();


        if (sFrame && (sFrame->GetMessageType() == CSFrame::Normal)) {

            if (sFrame->GetStreamType(STREAMTYPE_V2D) == STREAMTYPE_V2D) {
                CV2DStreamMediaFlag v2dSMediaFlag(sFrame->GetMediaFlag());

                m_frameCount++;
                m_bytesTransfered += sFrame->GetLength();

                short nChannel = v2dSMediaFlag.GetChannel();

                if (nChannel == FRAMETYPE_V2D_CHAN_VIDEO) {
                    m_bIsVideoSrcRTP =
                            (v2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP);
                }
                else if (nChannel == FRAMETYPE_V2D_CHAN_AUDIO) {
                    m_bIsAudioSrcRTP =
                            (v2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP);
                }


                switch (nChannel) {

                case FRAMETYPE_V2D_CHAN_CTL:
                    ProcessControlFrame(sFrame);
                    break;

                case FRAMETYPE_V2D_CHAN_VIDEO:
                    ProcessVideoFrame(sFrame);
                    break;

                case FRAMETYPE_V2D_CHAN_AUDIO:
                    ProcessAudioFrame(sFrame);
                    break;
                default:
                    SYSLOG_ERROR("V2DPlayerTx: Unexpected Frame MediaFlag: 0x%lx !!\n",
                            sFrame->GetMediaFlag());
                    ASSERT(0);
                }
            }
        }
        else if (sFrame && (sFrame->GetMessageType() != CSFrame::Normal)) {

            switch (sFrame->GetMessageType()) {
            case CSFrame::GapPresent:
            case CSFrame::EndOfMedia:
            case CSFrame::StartOfStream:
            case CSFrame::EndOfStream:
                SYSLOG_INFO("%s received non normal frame\n", __FUNCTION__);

                break;
            default:
                break;
            }
        }
    }

    // Check if SRD has any packets to send out
    ProcessSRDFrame(false);

    if (m_bSRDPacketSent)
        retval = 1;

    if (sFrame) {

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }
        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        m_qQueueSource->ReleaseFrame(sFrame);
        retval = 1;
    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0 && m_qQueueSource != NULL) {
            CheckQueueTimestamp();
        }
    }

    CheckSyncStop();

    return retval;
}

int CV2dPlayerTx::ProcessAltStream()
{
    int retval = 0;


    CSFrame * sFrame = m_qRendererSource->GetFrame();

    if (sFrame != NULL) {

        if (sFrame->GetMessageType() == CSFrame::Custom) {
            CRendererMessage *rendererMessage =
                    static_cast<CRendererMessage *> (sFrame->GetCustomObject());

            switch(rendererMessage->GetMessageType()) {
            case CRendererMessage::DebugVideo: {
                CRendererDebugVideo *debugMessage =
                        static_cast<CRendererDebugVideo *>(rendererMessage);
                m_pDecoderThread->SetVideoDebugLevel(debugMessage->DebugLevel());
                retval = 1;
                break;
            }

            case CRendererMessage::RFBMouseEvent: {
                CRendererRFBMouseEvent *mouseEvent =
                        static_cast<CRendererRFBMouseEvent *>(rendererMessage);
                SendMouseEvent(mouseEvent->x(), mouseEvent->y(), mouseEvent->buttonmask());
                retval = 1;
                break;
            }

            case CRendererMessage::RFBKeyboardEvent: {
                CRendererRFBKeyboardEvent *keyboardEvent =
                        static_cast<CRendererRFBKeyboardEvent *>(rendererMessage);
                SendKeyboardEvent(keyboardEvent->key(), keyboardEvent->down());
                retval = 1;
                break;
            }

            case CRendererMessage::ScalingRequest: {
                CRendererScalingRequest *scalingRequest =
                        static_cast<CRendererScalingRequest *>(rendererMessage);
                m_pDecoderThread->SetupScaledDimensions(
                        scalingRequest->scalingMode(),
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


int
CV2dPlayerTx::StartSenderStream(int nQueueId)
{
    return 0;
}

void
CV2dPlayerTx::CreateSRD()
{
    m_srd = (sliceRedData_t *)malloc(sizeof(sliceRedData_t));
    if (m_srd != NULL) {
        if (initSRD(m_srd) < 0) {
            destroySRD(m_srd);
            free(m_srd);
            m_srd = NULL;
        }
        else {
            SRDSetNewFrameCallback(m_srd, SRDNewFrameCallback, this);
            setStereoPolicy(m_srd, SP_SOURCE); // set current policy
            SRDSetCodecVersion(m_srd, m_scapProcessor->GetCodecVersion());
        }
    }
}

void CV2dPlayerTx::DestroySRD()
{
    if (m_srd != NULL) {
        destroySRD(m_srd);
        free(m_srd);
        m_srd = NULL;
    }
}


int CV2dPlayerTx::Update(CCommand *a_pCmd)
{
    return 0;
}


int CV2dPlayerTx::SendFrameToSRD(CSFrame* a_pSFrame)
{
    const int nDataLen    = a_pSFrame->GetLength();
    const int nDataOffset = (m_bIsVideoSrcRTP) ?
                            RTP_HEADER_SIZE : V2D_PACKET_HEADER_SIZE;
    int nDoneLen = 0;
    unsigned char* pCurrData =
        (unsigned char *)(a_pSFrame->GetData() + nDataOffset);

    //RecordToFile(pCurrData, SRD_VIDEO_PACKET_SIZE);

    while (nDoneLen < nDataLen) {
        findFramesinPacket(m_srd, pCurrData);
        pCurrData += nDataOffset + SRD_VIDEO_PACKET_SIZE;
        nDoneLen  += nDataOffset + SRD_VIDEO_PACKET_SIZE;
    }

    return 0;
}

void CV2dPlayerTx::RecordToFile(uint8_t * data, int length)
{
    static int file_opened = 0;
    static int filefd = 0;

    if (file_opened == 0) {
        filefd = open("v2d_data_file", O_CREAT | O_WRONLY | O_BINARY, 00666);
        file_opened = 1;
    }

    if (filefd > 0) {
        write(filefd, data, length);
    }
}

void CV2dPlayerTx::ProcessVideoFrame(CSFrame* a_pSFrame)
{
    // send packet to SRD library
    m_nVideoPackets++;
    m_nVideoBytes += a_pSFrame->GetLength();
    SendFrameToSRD(a_pSFrame);
}

void CV2dPlayerTx::DecodeAudio(uint8_t * buffer, int len) {
    for (int i = 0; i < len; i += 2) {
        unsigned short *nSample = (unsigned short*)(buffer + i);
        unsigned short nSampleSwapped = htons(*nSample);
        memcpy(m_pAudioDecodeBuffer + i, &nSampleSwapped, 2);
    }
}

void CV2dPlayerTx::ProcessAudioFrame(CSFrame* a_pSFrame)
{
    m_nAudioPackets++;
    m_nAudioBytes += 1012;

    if (m_bIsAudioSrcRTP) {
        DecodeAudio((uint8_t *) a_pSFrame->GetData() + RTP_HEADER_SIZE,
                a_pSFrame->GetLength() - RTP_HEADER_SIZE);
    }
    else {
        DecodeAudio((uint8_t *) a_pSFrame->GetData() + (V2D_PACKET_HEADER_SIZE + 8),
                a_pSFrame->GetLength() - (V2D_PACKET_HEADER_SIZE + 8 + 4));
    }
    if (!m_pAudioRenderer->IsStarted())
        m_pAudioRenderer->StartStreaming();
    m_pAudioRenderer->RenderSample(m_pAudioDecodeBuffer, 1012);
}

void CV2dPlayerTx::SendRFBStatusQuery()
{
    m_nQueryId++;
    uint8_t buffer[SEND_BUFFER_SIZE];
    int nLen = v2dLibCreateRFBStatusQuery(
        buffer, m_nSessionId, m_nQueryId);

    SendControlPacket(buffer, nLen);
}

int CV2dPlayerTx::ProcessRFBStatus(char *pData)
{
    uint32_t session_id = 0;
    uint32_t query_id = 0;
    int status = 0;

    if (v2dLibParseRFBStatus((uint8_t *)pData, &session_id, &query_id, &status) < 0)
        return -1;

    // If reply is for us, we honor the status
    if (session_id == m_nSessionId &&
            query_id == m_nQueryId) {
        m_bRFBReady = status;
        m_pDecoderThread->SendRFBStatus(m_bRFBReady);
    }
    else if (session_id == 0) {
        // Message was broadcast. We query for the latest status
        SendRFBStatusQuery();
    }

    return 0;
}

void CV2dPlayerTx::SendMouseEvent(uint16_t x, uint16_t y, uint8_t buttonmask)
{
    if (!m_bRFBReady)
        return;

    uint8_t buffer[64];
    int nLen = v2dLibCreateAbsoluteMouse(
            buffer, m_nSessionId, x, y, buttonmask);

    SendControlPacket(buffer, nLen);
}

void CV2dPlayerTx::SendKeyboardEvent(uint32_t key, uint8_t down)
{
    if (!m_bRFBReady)
        return;
    uint8_t buffer[64];
    int nLen = v2dLibCreateAbsoluteKeyboard(
            buffer, m_nSessionId, key, down);

    SendControlPacket(buffer, nLen);
}

void CV2dPlayerTx::SendControlPacket(uint8_t *a_pBuffer, int a_nLen)
{
    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    CSFrame * sFrame =
            new CSFrame((char *)a_pBuffer, a_nLen,
                    v2dSMediaFlag.GetMediaFlag(),
                    CTimeKeeper::Now());
    m_qQueueSink->InsertFrame(sFrame);
}

void CV2dPlayerTx::ProcessControlFrame(CSFrame* a_pSFrame)
{
    m_nControlPackets++;
    m_nControlBytes += a_pSFrame->GetLength();

    if (m_scapProcessor->Process(a_pSFrame->GetData())) {
        HandleSCAP();
    }
    else {
        char * pData = a_pSFrame->GetData();
        int protid = GET_PROTOCOL_UNIT_ID((unsigned char *) pData);
        switch (protid) {
        case IDENT_SERVER_RFBSTATUS:
            ProcessRFBStatus(pData);
            break;
        default:
            break;
        }
    }

}

void CV2dPlayerTx::HandleSCAP() {
    if (m_scapProcessor->ResolutionChangeDetected()) {
        string message;
        m_scapProcessor->ResolutionChangeString(message);
        SYSLOG_INFO("V2DPlayerTx: Detected Video ResolutionChange: %s", message.c_str());

        DestroySRD();
        CreateSRD();

        m_pDecoderThread->SetupDecoder(
                m_scapProcessor->GetWidth(),
                m_scapProcessor->GetHeight(),
                m_nPixelFormat,
                m_scapProcessor->GetSliceSize(),
                m_scapProcessor->IsSVC(),
                m_scapProcessor->GetEncodeFormat());
    }

    if (m_srd == NULL)
        return;

    m_srd->MaxSliceNo = m_scapProcessor->GetMaxSliceNumber();
    m_srd->sliceSize = m_scapProcessor->GetSliceSize();
    setStereoVideo(m_srd, m_scapProcessor->GetStereoMode());
    setCompMode(m_srd, m_scapProcessor->GetVideoCompMode());
    setMotionCompMode(m_srd, m_scapProcessor->GetVideoMotionCompMode());
}

void CV2dPlayerTx::ProcessSRDFrame(bool bInProgress)
{
    if (!ReadyToDecodeFrame())
        return;

    if (!GetFrameFromSRD())
        return;

    m_pDecoderThread->DecodeFrame(m_outBuffer, m_nFrameLength);
}

bool CV2dPlayerTx::ReadyToDecodeFrame()
{
    if (m_srd == NULL || m_qRendererSink == NULL)
        return false;

    if (!m_pDecoderThread->CanDecodeFrame())
        return false;

    if (!m_pDecoderThread->IsReady())
        return false;

    return true;
}

bool CV2dPlayerTx::GetFrameFromSRD()
{
    int rc = get_hoFrame(m_srd);
    if (rc == -1)
        return false;

    int bufferLength = getMinHoFrameSize(m_srd);
    CreateOutBuffer(bufferLength);

    int len = copyHoFrameToBuffer(m_srd, m_outBuffer, m_outBufferLen);
    memset(m_outBuffer + len, 0x00, 8);
    m_nFrameLength = len;

    return true;
}

void CV2dPlayerTx::CreateOutBuffer(int length)
{
    if (m_outBuffer == NULL || m_outBufferLen < length) {
        m_outBufferLen = length * 2;
        delete [] m_outBuffer;
        m_outBuffer = new uint8_t[m_outBufferLen];
    }
}

void CV2dPlayerTx::PrintStatistics(string &a_stats)
{
    if (m_nQueryId == 0  && m_nControlPackets > 0) {
        SendRFBStatusQuery();
    }
}


void CV2dPlayerTx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectCounterParam("VideoPackets", m_nVideoPackets);
    a_pCmd->SetObjectByteCounterParam("VideoBytes", m_nVideoBytes);
    a_pCmd->SetObjectCounterParam("AudioPackets", m_nAudioPackets);
    a_pCmd->SetObjectByteCounterParam("AudioBytes", m_nAudioBytes);
    a_pCmd->SetObjectCounterParam("ControlPackets", m_nControlPackets);
    a_pCmd->SetObjectByteCounterParam("ControlBytes", m_nControlBytes);
    a_pCmd->SetObjectParam("RFBStatus", m_bRFBReady);

    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectParam("MaxQueueLen",
                               (unsigned long)m_qQueueSource->MaxLength());
#ifdef SSDEBUG
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectParam("AverageQueueLen",
                                   (double)m_qQueueSource->AvgSize());
        }
#endif
    }

    a_pCmd->SetObjectConstantParam("CodecVersion", m_scapProcessor->GetCodecVersion());


    if (m_pDecoderThread) {
        m_pDecoderThread->UpdateCustomStatus(a_pCmd);
    }

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
        a_pCmd->SetObjectCounterParam("AudioSamplesDropped",
                                      m_pAudioRenderer->GetDroppedSamples());
        a_pCmd->SetObjectCounterParam("AudioSilenceInserted",
                                      m_pAudioRenderer->GetSilenceCount());
        a_pCmd->SetObjectParam("AudioErrorStatus",
                                      m_pAudioRenderer->GetErrorStatus());
        a_pCmd->SetObjectParam("AudioFreq",
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
    // SRD related stats
    if (m_srd != NULL) {
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectParam("SRDStereoDetect",
                                   (const char *) StereoDetect(m_srd));
            a_pCmd->SetObjectCounterParam("SRDRightFrames",
                                          m_srd->inRightFrameCount);
            a_pCmd->SetObjectCounterParam("SRDLeftFrames",
                                          m_srd->inLeftFrameCount);
            a_pCmd->SetObjectParam("SRD444", (bool) m_srd->is444);
            a_pCmd->SetObjectParam("SRDMOTION444", (bool) m_srd->isMotion444);
        }
        a_pCmd->SetObjectCounterParam("SRDInPackets",
                                      m_srd->inPktCount);
        a_pCmd->SetObjectCounterParam("SRDOutPackets",
                                      m_srd->outPktCount);
        a_pCmd->SetObjectCounterParam("SRDInFrames",
                                      m_srd->inFrameCount);
        a_pCmd->SetObjectCounterParam("SRDOutFrames",
                                      m_srd->outFrameCount);
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectCounterParam("SRDPrefillFrames",
                                          m_srd->prefillFrameCount);
        }
        a_pCmd->SetObjectCounterParam("SRDReducedFrames",
                                      m_srd->reducedFrameCount);
        a_pCmd->SetObjectCounterParam("SRDInSlices",
                                      m_srd->inSlices);
        a_pCmd->SetObjectCounterParam("SRDOutSlices",
                                      m_srd->outSlices);
        a_pCmd->SetObjectCounterParam("SRDReducedSlices",
                                      m_srd->reducedSlices);
        if (a_pCmd->StatusLevel(4)) {
            a_pCmd->SetObjectCounterParam("SRDReducedAltSlices",
                    m_srd->reducedAltSlices);
        }
        a_pCmd->SetObjectByteCounterParam("SRDInBytes",
                                          m_srd->inBytes);
        a_pCmd->SetObjectByteCounterParam("SRDOutBytes",
                                          m_srd->outBytes);
        a_pCmd->SetObjectByteCounterParam("SRDReducedBytes",
                                          m_srd->reducedBytes);
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectByteCounterParam("SRDPadBytes",
                    m_srd->padBytes);
        }
        a_pCmd->SetObjectErrorCounterParam("SRDSliceOutOfOrder",
                                           m_srd->parse_errors_ooo_slice);
        a_pCmd->SetObjectErrorCounterParam("SRDBadSlice",
                                           m_srd->parse_errors_bad_slice);
        a_pCmd->SetObjectErrorCounterParam("SRDBadFrame",
                                           m_srd->parse_errors_bad_frame);
        a_pCmd->SetObjectErrorCounterParam("SRDSOFError",
                                           m_srd->parse_errors_sof);
        a_pCmd->SetObjectCounterParam("SRDIFrameCount",
                                      m_srd->IFrameCount);
        a_pCmd->SetObjectCounterParam("SRDOutputIFrameCount",
                                      m_srd->OutputIFrameCount);
        a_pCmd->SetObjectCounterParam("SRDFullFrameCount",
                                      m_srd->FullFrameCount);
        a_pCmd->SetObjectGuageParam("SRDFullFrameInterval",
                                    m_srd->FullFrameInterval);
        if (a_pCmd->StatusLevel(2)) {
            if (m_srd->isSVC) {
                a_pCmd->SetObjectByteCounterParam("SRDDcBytes",
                                                  m_srd->svcDCBytes);
                a_pCmd->SetObjectByteCounterParam("SRDQuadBytes",
                                                  m_srd->svcQuadBytes);
                a_pCmd->SetObjectByteCounterParam("SRDRemBytes",
                                                  m_srd->svcRemBytes);
            }
        }
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectByterateGuageParam("SRDIFrameSize",
                                          m_srd->IFrameSize);
            a_pCmd->SetObjectGuageParam("SRDFirstFFInterval",
                                        m_srd->FirstFullFrameInterval);
        }
        a_pCmd->SetObjectErrorCounterParam("SRDFrameOverflow",
                                           m_srd->hbuff_overflow);
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectCounterParam("SRDReallocs",
                                               m_srd->realloc_count);
            a_pCmd->SetObjectParam("SRDSliceSize",
                                   (double) m_srd->avgSliceSize);
            a_pCmd->SetObjectParam("SRDFrameSize",
                                   (double) m_srd->avgFrameSize);
            a_pCmd->SetObjectGuageParam("SRDMaxSliceNumber",
                                        m_srd->MaxSliceNo);
            a_pCmd->SetObjectParam("SRDRefreshSlices",
                                   (double) m_srd->avgRefreshSlices);
           /* a_pCmd->SetObjectGuageParam("InputStaticFrame",
                                   (bool) m_srd->isInputFrameStatic);*/
        }
        a_pCmd->SetObjectParam("SRDFrameCoverage",
                               (double) m_srd->avgFrameCoverage);
        if (a_pCmd->StatusLevel(2)) {
            a_pCmd->SetObjectParam("SRDSliceDebug",
                                   (bool) m_srd->debug_slices);
            a_pCmd->SetObjectParam("SRDSliceMarking",
                                   (bool) m_srd->mark_slices);
        }
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectGuageParam("SRDBufferPool",
                                        m_srd->buffer_pool_count);
        }

    }

}

void CV2dPlayerTx::SetCustomStatusInfo()
{
}

void CV2dPlayerTx::PrintError(CNotify *a_notify)
{
}

void CV2dPlayerTx::PrintNotification(CNotify *a_notify)
{

}


