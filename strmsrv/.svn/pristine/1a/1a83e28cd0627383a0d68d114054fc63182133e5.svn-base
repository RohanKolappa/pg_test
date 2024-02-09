/*
 * rtpplayertx.cpp
 *
 *  Created on: Apr 16, 2011
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"
#include <sstream>

#include "common.hh"
#include "rtpplayertx.hh"
#include "rtpvideodecoderthr.hh"
#include "rtpaudiodecoderthr.hh"
#include "utils.hh"

#include "LiveMediaHeaders.hh"
#include "BasicUsageEnvironment.hh"

using namespace std;

CRtpPlayerTx::CRtpPlayerTx(int aId, char *sSDPFile, int nPixelFormat) :
    CSender(aId)
{
    m_nPortType = RTPPLAYERPORT;
    m_sSDPFile = sSDPFile;

    m_pSession = NULL;
    m_pVideoSubsession = NULL;
    m_pAudioSubsession = NULL;

    m_pVideoDecoderThread = NULL;
    m_pAudioDecoderThread = NULL;

    m_nVideoPkts = 0;
    m_nVideoBytes = 0;
    m_nAudioPkts = 0;
    m_nAudioBytes = 0;
    m_nDiscontinuities = 0;
    m_ePixelFormat = (ePixelFormat) nPixelFormat;

    ParseSDP();

    // Create the queues for sending data to decoder threads
    ostringstream videoQid;
    videoQid << "ToRTPVideoDecoder_" << m_nId;
    m_qVideoDecoderSink =
            CSQueueManager::Instance()->GetQueueSink(videoQid.str());

    ostringstream audioQid;
    audioQid << "ToRTPAudioDecoder_" << m_nId;
    m_qAudioDecoderSink =
            CSQueueManager::Instance()->GetQueueSink(audioQid.str());


}

CRtpPlayerTx::~CRtpPlayerTx()
{

    if (m_pVideoDecoderThread != NULL)
        m_pVideoDecoderThread->Stop();

    if (m_pAudioDecoderThread != NULL)
        m_pAudioDecoderThread->Stop();

    if (m_pSession != NULL)
        Medium::close(m_pSession);
    if (m_pUsageEnv != NULL)
        m_pUsageEnv->reclaim();
    if (m_pTaskScheduler)
        delete m_pTaskScheduler;

    if (m_qVideoDecoderSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qVideoDecoderSink);
    if (m_qAudioDecoderSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qAudioDecoderSink);

}

void CRtpPlayerTx::DeleteStream()
{
    // If we have not been "Stopped" yet do it now
    if (IsStarted()) {
        Stop();
    }

    // Send stop request to decoder threads so that they can delete themselves
    if (m_pVideoDecoderThread != NULL)
        m_pVideoDecoderThread->Stop();
    m_pVideoDecoderThread = NULL;

    if (m_pAudioDecoderThread != NULL)
        m_pAudioDecoderThread->Stop();
    m_pAudioDecoderThread = NULL;

    // Remove our own (streamer) reference also.
    RemoveReference();
}

void CRtpPlayerTx::ParseSDP()
{
    if (m_sSDPFile.length() <= 0)
        return;

    // Grab file into a buffer
    uint8_t * fileBuffer;
    int rc = ReadFileIntoBuffer(m_sSDPFile.c_str(), fileBuffer);

    if (rc == -1) {
        SendErrorNotification(NOTIFICATION_ERROR_FILE_READ,
                "Failed to open file: %s", m_sSDPFile.c_str());
        SetError();
        return;
    }

    // Parse SDP file
    m_pTaskScheduler = BasicTaskScheduler::createNew();
    m_pUsageEnv = BasicUsageEnvironment::createNew(*m_pTaskScheduler);
    m_pSession = MediaSession::createNew(*m_pUsageEnv, (const char *)fileBuffer);

    // Pick out the first audio and video sessions from the SDP description
    MediaSubsessionIterator iter(*m_pSession);
    MediaSubsession *subsession;
    while ((subsession = iter.next()) != NULL) {
        if (strcmp(subsession->mediumName(), "video") == 0) {
            if (m_pVideoSubsession == NULL) {
                m_pVideoSubsession = subsession;
                m_pVideoDecoderThread = new CRTPVideoDecoderThread(this, m_pVideoSubsession, m_ePixelFormat);
                m_pVideoDecoderThread->Start();
            }
        }
        else if (strcmp(subsession->mediumName(), "audio") == 0) {
            if (m_pAudioSubsession == NULL) {
                m_pAudioSubsession = subsession;
                m_pAudioDecoderThread = new CRTPAudioDecoderThread(this, m_pAudioSubsession);
                m_pAudioDecoderThread->Start();
            }
        }
    }

    delete [] fileBuffer;

}

bool CRtpPlayerTx::ParseSDPFile(std::string a_sdpFile,
        int & videoPort, int & audioPort, std::string & a_ipaddress)
{
    videoPort = 0;
    audioPort = 0;
    a_ipaddress = "0.0.0.0";


    if (a_sdpFile.length() <= 0)
        return false;

    // Grab file into a buffer
    uint8_t * fileBuffer;
    int rc = ReadFileIntoBuffer(a_sdpFile.c_str(), fileBuffer);

    if (rc == -1) {
        return false;
    }


    // Parse SDP file
    TaskScheduler *pTaskScheduler = BasicTaskScheduler::createNew();
    UsageEnvironment *pUsageEnv = BasicUsageEnvironment::createNew(*pTaskScheduler);
    MediaSession *pSession = MediaSession::createNew(*pUsageEnv, (const char *)fileBuffer);

    if (pSession == NULL) {
        return false;
    }

    // Try to get the IP address from the session first
    // If not found, we override with the first IP address found in audio/video subsessions
    // Our RTP Network object currently accepts only one IP address
    bool bIPAddressSet = false;
    if (pSession->connectionEndpointName() != NULL) {
        a_ipaddress = pSession->connectionEndpointName();
        bIPAddressSet = true;
    }

    // Pick out the first audio and video sessions from the SDP description
    MediaSubsessionIterator iter(*pSession);
    MediaSubsession *subsession;
    while ((subsession = iter.next()) != NULL) {
        if (strcmp(subsession->mediumName(), "video") == 0 && videoPort == 0) {
            videoPort = subsession->clientPortNum();
            if (!bIPAddressSet && subsession->connectionEndpointName() != NULL) {
                a_ipaddress = subsession->connectionEndpointName();
                bIPAddressSet = true;
            }
        }
        else if (strcmp(subsession->mediumName(), "audio") == 0 && audioPort == 0) {
            audioPort = subsession->clientPortNum();
            if (!bIPAddressSet && subsession->connectionEndpointName() != NULL) {
                a_ipaddress = subsession->connectionEndpointName();
                bIPAddressSet = true;
            }
        }
    }

    a_ipaddress = TrimString(a_ipaddress);

    if (pSession != NULL)
        Medium::close(pSession);
    if (pUsageEnv != NULL)
        pUsageEnv->reclaim();
    if (pTaskScheduler)
        delete pTaskScheduler;

    delete [] fileBuffer;

    return true;
}

int
CRtpPlayerTx::ProcessStream()
{
    int retval = 0;

    if (m_qQueueSource == NULL)
        return 0;

    CSFrame *sendSFrame;
    CSFrame *sFrame = m_qQueueSource->GetFrame();

    if (sFrame) {
        if (sFrame->HasDiscontinuity()) {
            m_nDiscontinuities++;
        }

        char * pData = sFrame->GetData();

        int nLen = sFrame->GetLength();

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }
        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        if (sFrame->GetMessageType() == CSFrame::Normal) {

            unsigned long nStreamType = sFrame->GetStreamType();

            if (nStreamType == STREAMTYPE_RTP) {
                CRTPStreamMediaFlag rtpSMediaFlag(sFrame->GetMediaFlag());
                switch(rtpSMediaFlag.GetEncaps()) {
                case FRAMETYPE_RTP_ENCAPS_RTP:
                    switch (rtpSMediaFlag.GetChannel()) {
                    case FRAMETYPE_RTP_CHAN_VIDEO:

                        sendSFrame = new CSFrame(pData, nLen, 0, CTimeKeeper::Now());
                        m_qVideoDecoderSink->InsertFrame(sendSFrame);
                        m_nVideoPkts++;
                        m_nVideoBytes += nLen;
                        break;

                    case FRAMETYPE_RTP_CHAN_AUDIO:
                        sendSFrame = new CSFrame(pData, nLen, 0, CTimeKeeper::Now());
                        m_qAudioDecoderSink->InsertFrame(sendSFrame);
                        m_nAudioPkts++;
                        m_nAudioBytes += nLen;
                        break;

                    default:

                        m_errors++;
                        break;
                    }
                    break;
                default:
                    m_errors++;
                    break;
                }
            }
            else if (nStreamType == STREAMTYPE_WIS) {
                m_errors++;
            }
            m_frameCount++;
            m_bytesTransfered += nLen;
        }

        m_qQueueSource->ReleaseFrame(sFrame);
        retval = 1;

    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0) {
            CheckQueueTimestamp();
        }
    }

    CheckSyncStop();

    return retval;
}

void
CRtpPlayerTx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("SDPFile", m_sSDPFile);
    if (m_pSession != NULL) {
        a_pCmd->SetObjectParam("SessionName", m_pSession->sessionName());
        if (m_pVideoSubsession != NULL) {
            a_pCmd->SetObjectParam("VideoCodec", m_pVideoSubsession->codecName());
        }
        if (m_pAudioSubsession != NULL) {
            a_pCmd->SetObjectParam("AudioCodec", m_pAudioSubsession->codecName());
        }
    }
    else {
        a_pCmd->SetObjectParam("SessionName", "");
    }

    a_pCmd->SetObjectCounterParam("VideoPackets", m_nVideoPkts);
    a_pCmd->SetObjectByteCounterParam("VideoBytes", m_nVideoBytes);
    a_pCmd->SetObjectCounterParam("AudioPackets", m_nAudioPkts);
    a_pCmd->SetObjectByteCounterParam("AudioBytes", m_nAudioBytes);

    if (m_pVideoDecoderThread != NULL) {
        m_pVideoDecoderThread->UpdateCustomStatus(a_pCmd);
    }
    if (m_pAudioDecoderThread != NULL) {
        m_pAudioDecoderThread->UpdateCustomStatus(a_pCmd);
    }

}



