
#include "strmsrv_compatibility.h"
#include "v2drx.hh"
#include "common.hh"
#include "notify.hh"
#include "rtputils.hh"
#include "MulticastUtils.hh"

using namespace std;

CV2dRx::CV2dRx (int nId,
                char *dest,
                int port,
                int bw,
                int mcast,
                int avoption,
                int videoport,
                int audioport,
                int audiotype,
                int httpport,
                const char *sAuthScheme,
                const char *sAuthData,
                intlist interfaceList) :
    CReceiver(nId)
{

    m_nPortType = V2DNETWORKPORT;

    if (!avoption)
        avoption=AVOPTION_VIDEO;

    // Ignore audiotype configuration (audio compression modes) and
    // use default mode

    v2dLibSetTime(CTimeKeeper::Now());
    m_RxSession = Rx_Create(dest, port, bw, mcast, videoport,
                          audioport, avoption, AUDIOTYPE_DEFAULT, 0);

    CMulticastUtils::RegisterMulticastRecvInterfaces(m_RxSession,
            interfaceList, m_sInterfaceInfo);

    if (httpport) {
        Rx_SetHTTPPort(m_RxSession, httpport);
    }
    if (strcasecmp(sAuthScheme, "token") == 0) {
        if (strlen(sAuthData) > 0)
            Rx_SetAuthToken(m_RxSession, sAuthData);
    }
    strcpy(m_errorString, "");

    m_pHal = NULL;
    m_bRTPFilterSent = false;

#ifdef HAVE_V2O
    m_pHal = hal_acquire(0);
#endif

    RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_AUDIO], CTimeKeeper::Now());
    RTPInitContext(&m_RTPContext[RTPCONTEXT_AUDIO], RTPAUDIOCLOCKFREQ,
                    V2D_AUDIO_SAMPLESPERFRAME, RTPAUDIOPAYLOAD);

    RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_VIDEO], CTimeKeeper::Now());
    RTPInitContext(&m_RTPContext[RTPCONTEXT_VIDEO], RTPAUDIOCLOCKFREQ,
                    0, RTPVIDEOPAYLOAD);
    m_pRTPPacket = new unsigned char[AV_BUFFER_SIZE];
}

CV2dRx::~CV2dRx()
{
    Rx_Destroy(m_RxSession);
    m_RxSession=NULL;

    delete [] m_pRTPPacket;
    return;
}

void CV2dRx::SetFirstTimestamp()
{
    m_nTimestamp = CTimeKeeper::Now();
    m_nStartTimestamp = CTimeKeeper::Now();
    m_nStartWallClock = CTimeKeeper::Now();
    m_bSendNotify = true;
    m_nEventId = NOTIFICATION_EVENT_BEGIN;
    m_nFrameWallClock = CTimeKeeper::Now();
}

int
CV2dRx::ProcessStream()
{
    int retval = -1;
    int nLen = 0;
    char *pData = NULL;

    if (m_RxSession == NULL)
        return 0;

    // Process Alt stream first
    if (ProcessAltStream() > 0) {
        return 1;
    }

    // Don't do anything till we are started
    if (m_qQueueSink == NULL)
        return 0;

    // Create control socket if not already done so
    if (m_RxSession->m_nCtlSock <= 0) {
        v2dLibSetTime(CTimeKeeper::Now());
        Rx_OpenMediaConnection(m_RxSession);
        if (m_RxSession->m_nCtlSock <= 0 ) {
            SetError();
        }
        retval = 1;
    }
    else {
        // First step: call Rx_Go to collect any data from our client
        if (!(m_RxSession->m_nFlag & 0xff00)) {
            v2dLibSetTime(CTimeKeeper::Now());
            (void) Rx_Go(m_RxSession);
        }
        else {
            // Rx_Go can collect data on multiple channels so we are
            // just waiting for all the data to be handled below
            // (i.e. waiting for the I_XXX flags to get cleared
        }

        // Handle data on one of the channels. ProcessStream() will be called
        // repeatedly till all flags are cleared
        CV2DStreamMediaFlag v2dSMediaFlag;

        if (m_RxSession->m_nFlag & I_CTL) {
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            nLen = GET_PROTOCOL_PACKET_LENGTH(m_RxSession->m_CtlRcv);
            pData = (char *)m_RxSession->m_CtlRcv;
            m_RxSession->m_nFlag &= ~I_CTL;
            SYSLOG_TRACE("V2DRx: Control Connection received.\n");
        }
        else if (m_RxSession->m_nFlag & I_VIDEO) {
            v2dSMediaFlag.SetEncaps(FRAMETYPE_V2D_ENCAPS_RTP);
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_VIDEO);
            if(!m_RxSession->m_bRecvRTP) {
                RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_VIDEO], CTimeKeeper::Now());
                pData = ConvertVideoPacketFromV2DToRTP(
                            (char*)m_RxSession->m_VideoRcv,
                            m_RxSession->m_nVideoLen,
                            (char *)m_pRTPPacket,
                            &nLen,
                            &m_RTPContext[RTPCONTEXT_VIDEO]);
            }
            else {
                nLen=m_RxSession->m_nVideoLen;
                pData = (char *)m_RxSession->m_VideoRcv;
            }
            m_RxSession->m_nFlag &= ~I_VIDEO;
        }
        else if (m_RxSession->m_nFlag & I_AUDIO) {
            v2dSMediaFlag.SetEncaps(FRAMETYPE_V2D_ENCAPS_RTP);
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_AUDIO);
            if(!m_RxSession->m_bRecvRTP) {
                RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_AUDIO], CTimeKeeper::Now());
                pData = ConvertAudioPacketFromV2DToRTP(
                            (char*)m_RxSession->m_AudioRcv,
                            m_RxSession->m_nAudioLen,
                            (char *)m_pRTPPacket,
                            &nLen,
                            &m_RTPContext[RTPCONTEXT_AUDIO]);
            }
            else {
                nLen=m_RxSession->m_nAudioLen;
                pData = (char *)m_RxSession->m_AudioRcv;
            }
            m_RxSession->m_nFlag &= ~I_AUDIO;
        }
        else if (m_RxSession->m_nFlag & I_SER) {

            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            nLen = GET_PROTOCOL_PACKET_LENGTH(m_RxSession->m_CtlRcv);
            pData = (char *)m_RxSession->m_CtlRcv;
            m_RxSession->m_nFlag &= ~I_SER;
        }

        // We have some data to send to the queue
        if (pData) {
            if (m_nTimestamp == 0) {
                SetFirstTimestamp();
            }
            m_nTimestamp = CTimeKeeper::Now();
            m_nFrameWallClock = CTimeKeeper::Now();

            CSFrame * sFrame =
                new CSFrame(pData,
                            nLen,
                            v2dSMediaFlag.GetMediaFlag(),
                            m_nTimestamp);
            ASSERT(sFrame != NULL);
            m_qQueueSink->InsertFrame(sFrame);
            m_bytesTransfered += nLen;
            m_frameCount++;

            retval = 1; // Call us back right away
        }
        else {
            retval = 0;
        }
    }

    // Handle any error conditions that may have happened during Rx_Go()
    // Decide what kind of errors are critical enough to abandon the object
    if (m_RxSession->m_bSendError == true) {
        int errCode = (m_RxSession->m_nErrCode & 0xffff0000) >> 16;
        //int errNumber = m_RxSession->m_nErrCode & 0xffff;
        switch (errCode) {
        case RC_CRITICAL_ERR:
            SetError();
            break;
        case RC_SOCKERR:
            break;
        }

        // If the last error has been cleared, save a copy of the
        // error string for the next error report
        Rx_SendError(m_RxSession, m_errorString, 255);
        // TODO: Send error notification;

    }

    switch(m_RxSession->m_bSendNotify) {
    case TRUE:
        switch (m_RxSession->m_nRxState) {
        case CLIENT_STATE_IDLE:
        case CLIENT_STATE_DISABLED:
            if (IsConnected()) {
                SetDisConnected();
                m_nEventId = NOTIFICATION_EVENT_DISCONNECTED;
                m_bSendNotify=true;
                m_bRTPFilterSent=false;
                if (m_nTimestamp == 0) {
                    SetFirstTimestamp();
                }
                CSFrame *pSFrame = new CSFrame(GetCurrentTimeStamp(),
                                               CSFrame::Disconnected);
                if (pSFrame != NULL)
                    m_qQueueSink->InsertFrame(pSFrame);
            }
            break;
        case CLIENT_STATE_CONNECTED:
        case CLIENT_STATE_READY:
            if (!IsConnected()) {
                SetConnected();
                m_nEventId = NOTIFICATION_EVENT_CONNECTED;
                m_bSendNotify=true;
                if (m_nTimestamp == 0) {
                    SetFirstTimestamp();
                }
                CSFrame *pSFrame = new CSFrame(GetCurrentTimeStamp(),
                                               CSFrame::Connected);
                if (pSFrame != NULL)
                    m_qQueueSink->InsertFrame(pSFrame);
            }
#ifdef HAVE_V2O
            if (hal_get_hardware_type(m_pHal) == XPI_DEVICE && 
                    m_RxSession->m_nRxState == CLIENT_STATE_READY && 
                    !m_bRTPFilterSent) {
		        SendRtpFilter();
                m_bRTPFilterSent = true;
            }
#endif
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return retval;
}

int CV2dRx::ProcessAltStream()
{
    int retval=0;
    char *pData;
    char *pDestBuffer = NULL;

    // Wait for queue to be assigned
    if (m_qQueueSource == NULL)
        return 0;

    // Get frame from Alt Queue and send it to v2d lib
    CSFrame * sAltFrame = m_qQueueSource->GetFrame();
    if (sAltFrame == NULL)
        goto EXIT_LABEL;

    // Wait for v2dlib to clear any pending messages out
    if (m_RxSession->m_nFlag & O_CTL) {
        v2dLibSetTime(CTimeKeeper::Now());
        Rx_Go(m_RxSession);
        if (m_RxSession->m_nFlag & O_CTL)
            return 0;
    }

    if (sAltFrame->GetMessageType() != CSFrame::Normal) {
        // Ignore custom messages (connect/disconnect)
        m_qQueueSource->ReleaseFrame(sAltFrame);
        return 0;
    }

    if (sAltFrame->GetStreamType(STREAMTYPE_V2D) == STREAMTYPE_V2D) {
        CV2DStreamMediaFlag v2dSMediaFlag(sAltFrame->GetMediaFlag());

        pData = sAltFrame->GetData();

        switch (v2dSMediaFlag.GetChannel()) {
        case FRAMETYPE_V2D_CHAN_CTL:
            pDestBuffer = (char *)m_RxSession->m_CtlSnd;
            break;
        default:
            ASSERT(0);
            break;
        }

        // Send the packet along
        if (pDestBuffer != NULL) {
            memcpy ((void *)pDestBuffer, (void *)pData,
                    sAltFrame->GetLength());
            m_RxSession->m_nFlag |= O_CTL;
            v2dLibSetTime(CTimeKeeper::Now());
            Rx_Go(m_RxSession);
            retval = 1;
        }
    }
    m_qQueueSource->ReleaseFrame(sAltFrame);

 EXIT_LABEL:
    return retval;
}


int
CV2dRx::Update(CCommand *a_pCmd)
{
    int retval=0;
    int mouseX, mouseY, buttonMask;
    int dummy;

    if ( (a_pCmd->GetIntVar("mousex", &mouseX) != -1) &&
         (a_pCmd->GetIntVar("mousey", &mouseY) != -1)  &&
         (a_pCmd->GetIntVar("buttonmask", &buttonMask) != -1) ) {
        if (Rx_GetRFBStatus(m_RxSession)) {
            Rx_SendMouseEvent(m_RxSession, mouseX, mouseY, buttonMask);
            m_RxSession->m_nFlag |= O_CTL;
            v2dLibSetTime(CTimeKeeper::Now());
            Rx_Go(m_RxSession);
        }
        retval = 1;
    }

    if ( a_pCmd->GetIntVar("fullscreen_refresh", &dummy) != -1) {
        Rx_SendFSRefresh(m_RxSession);
        m_RxSession->m_nFlag |= O_CTL;
        v2dLibSetTime(CTimeKeeper::Now());
        Rx_Go(m_RxSession);
        retval = 1;
    }


    return retval;
}

void
CV2dRx::PrintStatistics(string &a_stats)
{
    char cp[512];
    if (m_RxSession)
        Rx_PrintStatistics(m_RxSession, cp, 511);
    a_stats.append(cp);
}

void
CV2dRx::UpdateCustomStatus(CCommand *a_pCmd)
{

    if (m_RxSession == NULL)
        return;

    char buff[128];

    a_pCmd->SetObjectParam("Server",
                           inet_ntoa(m_RxSession->m_Destination.sin_addr));
    a_pCmd->SetObjectConstantParam("Port",
                                   ntohs(m_RxSession->m_Destination.sin_port));
    a_pCmd->SetObjectParam("ProtocolVersion",
                           getV2DProtocolVersion(m_RxSession->m_version,
                                                 buff, 31));
    a_pCmd->SetObjectParam("ClientStatus",
                           (char *) v2dLibGetStatusString(buff,
                                                          m_RxSession->m_statuscode,
                                                          m_RxSession->m_nMulticast,
                                                          m_RxSession->m_bUseHTTP,
                                                          m_RxSession->m_nAVOption));
    a_pCmd->SetObjectErrorCounterParam("ResetCount",
                                       m_RxSession->m_nResetCount);
    a_pCmd->SetObjectParam("VideoName",
                           (char *) Rx_GetVideoName(m_RxSession));

    a_pCmd->SetObjectParam("Multicast",
                           (bool) (m_RxSession->m_nMulticast ? 1 : 0));
    a_pCmd->SetObjectParam("MulticastAddress",
            (char *) m_RxSession->m_MulticastAddress);
    if (m_RxSession->m_nMulticast) {
        a_pCmd->SetObjectParam("MulticastInterfaces", m_sInterfaceInfo);
    }
    else {
        a_pCmd->SetObjectParam("MulticastInterfaces", "");
    }
    if (a_pCmd->StatusLevel(4)) {
        a_pCmd->SetObjectParam("AuthToken", Rx_GetAuthToken(m_RxSession));
    }
    a_pCmd->SetObjectParam("AVOption",
                           (int)m_RxSession->m_nAVOption);
    a_pCmd->SetObjectParam("AudioType",
                           (int)m_RxSession->m_nAudioType);


    a_pCmd->SetObjectConstantParam("VideoPort",
                                   m_RxSession->m_nVideoPort);
    a_pCmd->SetObjectBitrateGuageParam("VideoRate",
                                       m_RxSession->m_nCurrVideoRate);
    a_pCmd->SetObjectByteCounterParam("VideoBytes",
                           m_RxSession->m_nVideoBytes);
    a_pCmd->SetObjectCounterParam("VideoPackets",
                           m_RxSession->m_nVideoPkts);
    a_pCmd->SetObjectErrorCounterParam("VideoLoss",
                                       m_RxSession->m_nVideoLoss);
    a_pCmd->SetObjectErrorCounterParam("VideoOutOfOrder",
                                       m_RxSession->m_nVideoOutOfOrder);
    a_pCmd->SetObjectConstantParam("AudioPort",
                                   m_RxSession->m_nAudioPort);
    a_pCmd->SetObjectBitrateGuageParam("AudioRate",
                                       m_RxSession->m_nCurrAudioRate);
    a_pCmd->SetObjectByteCounterParam("AudioBytes",
                                      m_RxSession->m_nAudioBytes);
    a_pCmd->SetObjectCounterParam("AudioPackets",
                                  m_RxSession->m_nAudioPkts);
    a_pCmd->SetObjectErrorCounterParam("AudioLoss",
                                       m_RxSession->m_nAudioLoss);
    a_pCmd->SetObjectErrorCounterParam("AudioOutOfOrder",
                                       m_RxSession->m_nAudioOutOfOrder);
    a_pCmd->SetObjectByteCounterParam("SerialRcvd",
                                      m_RxSession->m_nSerialRcvd);
    a_pCmd->SetObjectCounterParam("CtrlPacketsRcvd",
                                  m_RxSession->m_nCtrlRcvPkts);
    a_pCmd->SetObjectByteCounterParam("CtrlBytesRcvd",
                                      m_RxSession->m_nCtrlRcvBytes);
    a_pCmd->SetObjectCounterParam("CtrlPacketsSent",
                                  m_RxSession->m_nCtrlSndPkts);
    a_pCmd->SetObjectByteCounterParam("CtrlBytesSent",
                                      m_RxSession->m_nCtrlSndBytes);

    a_pCmd->SetObjectParam("RFBStatus",
                           (bool) Rx_GetRFBStatus(m_RxSession));
    a_pCmd->SetObjectCounterParam("RFBMouse",
                                  m_RxSession->m_RFBMouseSent);
    a_pCmd->SetObjectCounterParam("RFBKeyboard",
                                  m_RxSession->m_RFBKeyboardSent);
    if (a_pCmd->StatusLevel(1)) {
        a_pCmd->SetObjectParam("PayloadType",
            ((m_RxSession->m_bRecvRTP == TRUE) ? "RTP" : "V2D"));
        a_pCmd->SetObjectParam("AudioTS",
                m_RTPContext[RTPCONTEXT_AUDIO].m_nTimeStamp);
        a_pCmd->SetObjectParam("VideoTS",
                m_RTPContext[RTPCONTEXT_VIDEO].m_nTimeStamp);
    }
}

void
CV2dRx::SetCustomStatusInfo()
{
    SetInfoString("Server", "Help on Server");
    SetInfoString("Port", "Help on Port");
}

#ifdef HAVE_V2O
void
CV2dRx::SendRtpFilter(void)
{
    int nLen;
    char pMsgBuffer[128];
    CV2DStreamMediaFlag v2dSMediaFlag;
    unsigned char *cp;

    if (m_qQueueSink == NULL)
        return;

    if (Rx_BuildRtpFilter(m_RxSession, pMsgBuffer, sizeof(pMsgBuffer)) < 0) {
        SYSLOG_ERROR("V2DRx: Failed to Set RTP Filter");
        return;
    }
    cp = (unsigned char *) pMsgBuffer;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    nLen = GET_PROTOCOL_PACKET_LENGTH(cp);
    CSFrame *sFrame = new CSFrame(pMsgBuffer, nLen + 4,
                                  v2dSMediaFlag.GetMediaFlag(),
                                  CTimeKeeper::Now());
    m_qQueueSink->InsertFrame(sFrame);
}
#endif

void
CV2dRx::PrintError(CNotify *a_notify)
{
#if 0
    a_error.append(m_errorString);
#endif
}

void
CV2dRx::PrintNotification(CNotify *a_notify)
{
    switch (m_nEventId) {
    case NOTIFICATION_EVENT_CONNECTED:
    case NOTIFICATION_EVENT_DISCONNECTED:
        if (m_RxSession != NULL) {
            a_notify->SetNotificationData("ServerIP",
                                          inet_ntoa(m_RxSession->m_Destination.sin_addr));
        }
        else {
            a_notify->SetNotificationData("ServerIP", "");
        }
        break;
    default:
        break;
    }
}

