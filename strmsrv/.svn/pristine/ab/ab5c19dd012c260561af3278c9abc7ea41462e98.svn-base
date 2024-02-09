
#include "strmsrv_compatibility.h"
#include <math.h>

#include "v2dtx.hh"
#include "common.hh"
#include "notify.hh"
#include "rtp.h"
#include "rtputils.hh"
#include "MulticastUtils.hh"
#include "codecdefs.h"
#include "ScapProcessor.hh"
#include "StreamInfo.hh"
#include "netutils.hh"

using namespace std;

CIdProvider CV2dTx::m_IdProvider(50000);

CV2dTx::CV2dTx (int nId,
                int nSrvPort,
                int nTTL,
                char *sMcastAddr,
                int maxBandwidth,
                int bIsV2DTx,
                int nListener,
                int bPolicyCheck,
                char *sPolicyFileName,
                char *sProtocolVersion,
                const char *sAuthScheme,
                const char *sAuthData,
                int nInterface) :
    CSender(nId)
{
    m_nPortType    = V2DDECODERPORT;
    m_nServerPort = nSrvPort;
    m_bSentCachedScap = false;
    m_bCheckIFrameRequest = true;
    m_bForceIFrameRequest = false;
    m_framesServed = 0;
    m_bytesServed = 0;
    m_bIsV2DTx = false;
    m_bPolicyCheck = false;
    m_bNetworkHeadersSent = false;
    m_scapProcessor = new CScapProcessor();

    m_pHal = NULL;

    m_nFrameOffset = 0;

    if (bIsV2DTx) {
        m_bIsV2DTx = true;
#ifdef HAVE_V2O
        m_pHal = hal_acquire(0);
#endif
    }

    if (bPolicyCheck) {
        m_bPolicyCheck = true;
    }

    v2dLibSetTime(CTimeKeeper::Now());


#ifdef HAVE_V2O
    if (m_pHal != NULL && hal_get_hardware_type(m_pHal) != XPI_DEVICE) {
        hal_LED_keepalive(m_pHal, CONNECTION_LED, 2);
        hal_LED_blink(m_pHal, CONNECTION_LED, LED_BLINK_FAST);
    }
#endif

    m_sPolicyFileName = NULL;
    if (strlen(sPolicyFileName) > 0)
        m_sPolicyFileName = strdup(sPolicyFileName);
    m_sProtocolVersion = NULL;
    if (strlen(sProtocolVersion) > 0)
        m_sProtocolVersion = strdup(sProtocolVersion);
    m_TxSession = Tx_Create(nSrvPort, nTTL, sMcastAddr,
                            bIsV2DTx, m_bPolicyCheck, m_sPolicyFileName, m_sProtocolVersion);

    if (m_TxSession != NULL) {
        Tx_SetListenerCallback(m_TxSession, ListenerCallback, this);
        Tx_SetErrorCallback(m_TxSession, ErrorCallback, this);
        Tx_SetNotificationCallback(m_TxSession, NotificationCallback, this);

        CMulticastUtils::RegisterMulticastSendInterface(m_TxSession,
                nInterface, m_sInterfaceInfo);

        if (strcasecmp(sAuthScheme, "ip") == 0) {
            Tx_SetExpectedClient(m_TxSession, sAuthData);
        }
        else if (strcasecmp(sAuthScheme, "token") == 0) {
            Tx_SetAuthToken(m_TxSession, sAuthData);
        }
    }
    m_lastTimeStamp = CTimeKeeper::Now();
    m_flowcontrol = true;

    m_enableSRD = true;
    m_stereoPolicy = SP_SOURCE;

    m_pShaper = NULL;
    m_srd = NULL;
    m_nSRDPacketOffset = 0;
    m_nMaxBandwidth = (100 * 1024 * 1024)/8;

    RecoverSRDPackets();

    if (maxBandwidth == -1 || m_bIsV2DTx) {
        m_enableSRD = false;
    }
    else if (maxBandwidth/8 > 0) {
        m_nMaxBandwidth = maxBandwidth/8;
    }

    // Burst Ratio of 1.5 and Burst Duration of 50 ms
    m_pShaper = new CShaper(m_nMaxBandwidth * 8, 1.5, 50);

    if (m_enableSRD) {
        EnableSRD();
    }

    if (m_TxSession == NULL) {
        // TODO: Handle Error
    }

    // On a V2D Tx, we are a Listener or Cloned (m_nServerPort == 0),
    // Create the queues for sending commands to controller thread
    m_nListener = nListener ? true : false;
    m_bReleaseId = false;
    m_qSchedulerSink = NULL;
    m_qSchedulerSource = NULL;
    if (nListener || m_nServerPort == 0) {
        m_qSchedulerSink =
            CSQueueManager::Instance()->GetQueueSink("ToScheduler");
        // todo: do we need to handle response for our messages to the
        // controller thread ??

        // m_qSchedulerSource =
        //    CSQueueManager::Instance()->GetQueueSource("FromScheduler");
    }

    m_bIsAudioSrcRTP = false;
    m_bIsVideoSrcRTP = false;

    RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_AUDIO], CTimeKeeper::Now());
    RTPInitContext(&m_RTPContext[RTPCONTEXT_AUDIO], RTPAUDIOCLOCKFREQ,
                    V2D_AUDIO_SAMPLESPERFRAME, RTPAUDIOPAYLOAD);
    RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_VIDEO], CTimeKeeper::Now());
    RTPInitContext(&m_RTPContext[RTPCONTEXT_VIDEO], RTPAUDIOCLOCKFREQ,
                    0, RTPVIDEOPAYLOAD);

    m_pAuxPacket = new unsigned char[AV_BUFFER_SIZE];
}

CV2dTx::~CV2dTx()
{
#ifdef HAVE_V2O
    if (m_pHal != NULL) 
        hal_release(m_pHal);
#endif

    DisableSRD();
    Tx_Destroy(m_TxSession);
    m_TxSession = NULL;

    delete m_pShaper;
    if (m_sPolicyFileName != NULL)
        free(m_sPolicyFileName);
    if (m_sProtocolVersion != NULL)
        free(m_sProtocolVersion);

    if (m_qSchedulerSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qSchedulerSink);
    if (m_qSchedulerSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qSchedulerSource);



    delete [] m_pAuxPacket;
    delete m_scapProcessor;

    if (m_bReleaseId)
    	m_IdProvider.ReleaseId(m_nId);

    return;
}

void
CV2dTx::StoppingSender()
{
    if (m_bIsV2DTx && IsConnected() && m_bNetworkHeadersSent) {
        SYSLOG_INFO("V2DTx: Stopping sender...");
        SendConnectionStatus(false);
        m_bNetworkHeadersSent = false;
        // TODO: ensure this logic holds true even on trunk for non XPI case
    }
}

int
CV2dTx::StopSender()
{
    m_bSentCachedScap = false;
    return 0;
}

// Listener Callback from v2dlib. For V2DTx, we have to clone a new
// object to handle additional connections on the listen socket by
// sending action=add/start messages to the controller thread on the
// scheduler queue
void
CV2dTx::ListenerCallback(void *a_object, int a_fd)
{

    CV2dTx * This = static_cast<CV2dTx *> (a_object);
    ASSERT(a_fd > 0);

    // We better never lose incoming fd. Someone has to close it.
    SYSLOG_TRACE("V2DTx: Listener Callback Invoked with FD=%d\n", a_fd);

    // Create and start a V2D decoder object to handle this fd
    if (This->m_nListener && This->m_qSchedulerSink != NULL) {

        char buff[512];
        char policyfiledef[80];
        char protocolversiondef[80];
        char authenticationdef[80];

        int nCloneId = This->m_IdProvider.GetId();

        strcpy(policyfiledef, "");
        if (This->m_sPolicyFileName != NULL)
            snprintf(policyfiledef, 80, "policyfile=\"%s\"",
                     This->m_sPolicyFileName);

        strcpy(protocolversiondef, "");
        if (This->m_sProtocolVersion != NULL)
            snprintf(protocolversiondef, 80, "protocolversion=%s",
                     This->m_sProtocolVersion);

        strcpy(authenticationdef, "");
        if (This->m_TxSession != NULL && strlen(Tx_GetAuthToken(This->m_TxSession)) > 0)
            snprintf(authenticationdef, 80, "authscheme=token authdata=%s",
                    Tx_GetAuthToken(This->m_TxSession));

        sprintf(buff, "action=add id=%d operation=output dsttype=V2DDecoderPort streamtype=V2D dstport=0 v2dtx=1 listener=0  mcastttl=%d mcastaddr=%s policycheck=%d %s %s %s schedid=%d",
                nCloneId, This->m_TxSession->m_nTTL,
                This->m_TxSession->m_McastAddress, This->m_bPolicyCheck ? 1 : 0,
                policyfiledef, protocolversiondef, authenticationdef, This->m_nId);
        CSFrame * sFrame = new CSFrame(buff, strlen(buff));
        This->m_qSchedulerSink->InsertFrame(sFrame);

        sprintf(buff, "action=start id=%d queueid=%d operation=output dsttype=V2DDecoderPort streamtype=V2D schedid=%d",
                nCloneId, This->m_nQueueId, This->m_nId);
        sFrame = new CSFrame(buff, strlen(buff));
        This->m_qSchedulerSink->InsertFrame(sFrame);

        sprintf(buff, "action=update id=%d operation=output dsttype=V2DDecoderPort streamtype=V2D tcpcontrolfd=%d schedid=%d",
                nCloneId, a_fd, This->m_nId);
        sFrame = new CSFrame(buff, strlen(buff));
        This->m_qSchedulerSink->InsertFrame(sFrame);

    }
    else {
        CloseSocket(a_fd);
    }
    return;
}

void
CV2dTx::CloseSocket(int a_fd)
{
#ifdef WINDOWS
    closesocket(a_fd);
#else
    close(a_fd);
#endif
}

// Schedule a action=stop/delete for this object via the controller
// thread's scheduler queue.
void CV2dTx::ScheduleDelete()
{

    if (m_qSchedulerSink == NULL)
        return;
    ASSERT(m_nServerPort == 0);

    char buff[256];

    sprintf(buff, "action=stop id=%d operation=output dsttype=V2DDecoderPort streamtype=V2D schedid=%d",
            m_nId, m_nId);
    CSFrame * sFrame = new CSFrame(buff, strlen(buff));
    if (sFrame != NULL) {
        m_qSchedulerSink->InsertFrame(sFrame);
    }

    sprintf(buff, "action=delete id=%d operation=output dsttype=V2DDecoderPort streamtype=V2D schedid=%d",
            m_nId, m_nId);
    sFrame = new CSFrame(buff, strlen(buff));
    if (sFrame != NULL) {
        m_qSchedulerSink->InsertFrame(sFrame);
    }

    // Arrange to give back the clonedId to the pool when object is destroyed
    m_bReleaseId = true;
}

void CV2dTx::ProcessSCAP(CSFrame *a_sFrame) {

    if (!m_scapProcessor->Process(a_sFrame->GetData()))
        return;

    if (m_scapProcessor->ResolutionChangeDetected()) {
        string message;
        m_scapProcessor->ResolutionChangeString(message);
        SYSLOG_INFO("V2DPlayerTx: Detected Video ResolutionChange: %s", message.c_str());

        if (m_enableSRD) {
            DisableSRD();
            EnableSRD();
        }
    }

    if (!m_enableSRD || m_srd == NULL)
        return;

    m_srd->sliceSize = m_scapProcessor->GetSliceSize();
    m_srd->MaxSliceNo = m_scapProcessor->GetMaxSliceNumber();
    setStereoVideo(m_srd, m_scapProcessor->GetStereoMode());
    setCompMode(m_srd, m_scapProcessor->GetVideoCompMode());
    setMotionCompMode(m_srd, m_scapProcessor->GetVideoMotionCompMode());
}


/// Send a cached scap to the client if it exists
void CV2dTx::SendCachedSCAP() {

    if (m_bSentCachedScap == true)
        return;

    if (m_TxSession->m_AV == NULL)
        return;

    if (m_TxSession->m_nTxState != CLIENT_STATE_READY)
        return;

    if (m_TxSession->m_CtlSnd != NULL &&
            m_scapProcessor->GetCachedSCAP(m_TxSession->m_CtlSnd)) {
        SYSLOG_DEBUG("V2DTx: Sending Cached Scap.\n");
        m_TxSession->m_nFlag |= O_CTL;
        Tx_Go(m_TxSession);
    }

    m_bSentCachedScap = true;
}


void CV2dTx::ErrorCallbackHandler()
{

    // TODO We should queue up all errors or remember the first error

    if (IsConnected()) {
        SetDisConnected();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_DISCONNECTED;
        m_TxSession->m_nFlag = 0;
        m_bSentCachedScap = false;
        RecoverSRDPackets();

        if (m_bNetworkHeadersSent) {
            SendConnectionStatus(false);
            m_bNetworkHeadersSent = false;
        }
    }
    if (m_nServerPort == 0)
        ScheduleDelete();

}

void CV2dTx::ErrorCallback(void *a_object)
{
    CV2dTx * This = static_cast<CV2dTx *> (a_object);
    This->ErrorCallbackHandler();
}

void CV2dTx::NotificationCallbackHandler()
{

    switch (m_TxSession->m_nTxState) {
    case CLIENT_STATE_CONNECTED:
        if (!IsConnected()) {
            SetConnected();
            m_bSendNotify = true;
            m_nEventId = NOTIFICATION_EVENT_CONNECTED;
            SendConnectionStatus(true);
        }
        break;
    case CLIENT_STATE_READY:
        if (!IsConnected()) {
            SetConnected();
            m_bSendNotify = true;
            m_nEventId = NOTIFICATION_EVENT_CONNECTED;
            SendConnectionStatus(true);
        }

#ifdef HAVE_V2O
        if (hal_get_hardware_type(m_pHal) == XPI_DEVICE) {
            if (!SendNetworkHeader()) {
                SYSLOG_INFO("Failed to set network header. "
                        "Resetting connection to client.");
                Tx_ResetConnection(m_TxSession);
            }
        }
#endif

        if (m_TxSession->m_nRequestBandwidth <= 0)
            m_TxSession->m_nRequestBandwidth = (64 * 1024)/8; // 64 Kpbs
        if (m_TxSession->m_nRequestBandwidth >= m_nMaxBandwidth)
            m_TxSession->m_nRequestBandwidth = m_nMaxBandwidth;
        if (m_pShaper != NULL) {
            m_pShaper->SetBandwidth(m_TxSession->m_nRequestBandwidth * 8);
        }
        if (m_srd != NULL) {
            SYSLOG_DEBUG("V2DTx: Requesting SRD for full frame.\n");
            requestFullFrame(m_srd);
        }
        break;
    case CLIENT_STATE_DISABLED:
        ASSERT(0); // currently not supposed to happen
        SetDisConnected();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_DISCONNECTED;
        RecoverSRDPackets();
        if (m_bNetworkHeadersSent) {
            SendConnectionStatus(false);
            m_bNetworkHeadersSent = false;
        }
        break;

    default:
        SYSLOG_ERROR("V2DTx: Unhandled state %d", m_TxSession->m_nTxState);
    }

}


void CV2dTx::NotificationCallback(void *a_object)
{
    CV2dTx * This = static_cast<CV2dTx *> (a_object);
    This->NotificationCallbackHandler();
}

void CV2dTx::SRDNewFrameCallback(void *a_object)
{
    CV2dTx * This = static_cast<CV2dTx *> (a_object);
    This->ProcessSRDFrame(true);
}

int CV2dTx::ProcessStream()
{

    if (m_TxSession == NULL) {
        if (m_qQueueSource != NULL) {
            CSFrame *sFrame = m_qQueueSource->GetFrame();
            if (sFrame != NULL) {
                m_qQueueSource->ReleaseFrame(sFrame);
                return 1;
            }
        }
        return 0;
    }

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

                if (m_nFrameOffset == 0) {
                    m_frameCount++;
                    m_bytesTransfered += sFrame->GetLength();
                }

                short nChannel = v2dSMediaFlag.GetChannel();

                if (nChannel == FRAMETYPE_V2D_CHAN_VIDEO) {
                    m_bIsVideoSrcRTP =
                       (v2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP);
                }
                else if (nChannel == FRAMETYPE_V2D_CHAN_AUDIO) {
                    m_bIsAudioSrcRTP =
                       (v2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP);
                }

                // Whether we are connected or not, save the incoming scap
                if (nChannel == FRAMETYPE_V2D_CHAN_CTL)
                    ProcessSCAP(sFrame);

                if (m_TxSession->m_nSvrState == SERVER_STATE_IDLE) {
                    // Server is idle waiting for connections

                    // Continue performing SRD on incoming packets even
                    // if the client is not connected
                    if (m_enableSRD && nChannel == FRAMETYPE_V2D_CHAN_VIDEO) {
                        SendFrameToSRD(sFrame);
                    }

                }
                else if (m_TxSession->m_nFlag & O_CTL) {
                    // v2d lib still busy with the last frame sent: TODO fix this
                    m_errors++;

                    if (m_enableSRD && nChannel == FRAMETYPE_V2D_CHAN_VIDEO) {
                        SendFrameToSRD(sFrame);
                    }

                }
                else {

                    switch (nChannel) {

                    case FRAMETYPE_V2D_CHAN_CTL:
                        ProcessControlFrame(sFrame);
                        break;

                    case FRAMETYPE_V2D_CHAN_VIDEO:
                        SendCachedSCAP();
                        ProcessVideoFrame(sFrame);
                        break;

                    case FRAMETYPE_V2D_CHAN_AUDIO:
                        SendCachedSCAP();
                        ProcessAudioFrame(sFrame);
                        break;

                    default:
                        SYSLOG_ERROR("V2DTx: Unexpected Frame MediaFlag: 0x%lx !!\n",
                                     sFrame->GetMediaFlag());
                        ASSERT(0);
                    }

                }
            }
        }
        else if (sFrame && (sFrame->GetMessageType() != CSFrame::Normal)) {

            if (sFrame->GetMessageType() == CSFrame::StartOfStream) {
                ResetStreamInfo();
                if (sFrame->GetData() != NULL) {
                    CStreamInfo *info =
                        static_cast<CStreamInfo *>((void *)sFrame->GetData());
                    SetStreamInfo(info);
                }
            }

            switch (sFrame->GetMessageType()) {
            case CSFrame::GapPresent:
            case CSFrame::EndOfMedia:
            case CSFrame::StartOfStream:
            case CSFrame::EndOfStream:
                SYSLOG_INFO("%s received non normal frame\n", __func__);
                RTPCreateNewSSRC(&m_RTPContext[RTPCONTEXT_AUDIO]);
                RTPCreateNewSSRC(&m_RTPContext[RTPCONTEXT_VIDEO]);
                break;
            default:
                break;
            }

        }

    }
    else {
        // on Stop() the queues are deleted
        m_bSentCachedScap = false;
    }

    // Check if SRD has any packets to send out
    ProcessSRDFrame(false);
    if (m_bSRDPacketSent)
        retval = 1;

    // Let the Tx_Session progress. Since the client is allowed to
    // connect we call Tx_Go() even if the queue is not assigned
    v2dLibSetTime(CTimeKeeper::Now());
    Tx_Go(m_TxSession);

    if (sFrame) {

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }
        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        if(m_nFrameOffset == 0) {
            m_qQueueSource->ReleaseFrame(sFrame);
        }
        retval = 1;
    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0 && m_qQueueSource != NULL) {
            CheckQueueTimestamp();
        }
    }

    UpdateStats(m_TxSession);

    CheckSyncStop();

    return retval;
}

int CV2dTx::ProcessAltStream()
{
    int retval=0;
    int nLen=0;
    char *pData=NULL;

    ASSERT(m_TxSession != NULL);

    // If any control data comes in we consume it, i.e. either send it 
    // to the alt queue or throw it away if alt queue is not assigned
    if (m_TxSession->m_nFlag & (I_CTL | I_SER | I_KB | I_MS)) {

        CV2DStreamMediaFlag v2dSMediaFlag;

        if (m_TxSession->m_nFlag & I_CTL) {
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            nLen = GET_PROTOCOL_PACKET_LENGTH(m_TxSession->m_CtlRcv);
            pData = (char *)m_TxSession->m_CtlRcv;
            m_TxSession->m_nFlag &= ~I_CTL;
            SYSLOG_DEBUG("V2DTx: Control Connection received.\n");
        }
        else if (m_TxSession->m_nFlag & I_SER) {
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            nLen = GET_PROTOCOL_PACKET_LENGTH(m_TxSession->m_CtlRcv);
            pData = (char *)m_TxSession->m_CtlRcv;
            m_TxSession->m_nFlag &= ~I_SER;
        }
        else if (m_TxSession->m_nFlag & I_KB) {
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            nLen = GET_PROTOCOL_PACKET_LENGTH(m_TxSession->m_CtlRcv);
            pData = (char *)m_TxSession->m_CtlRcv;
            m_TxSession->m_nFlag &= ~I_KB;
        }
        else if (m_TxSession->m_nFlag & I_MS) {
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            nLen = GET_PROTOCOL_PACKET_LENGTH(m_TxSession->m_CtlRcv);
            pData = (char *)m_TxSession->m_CtlRcv;
            m_TxSession->m_nFlag &= ~I_MS;
        }

        if (pData && (m_qQueueSink != NULL)) {
            CSFrame *sAltFrame =
                new CSFrame(pData, nLen,
                            v2dSMediaFlag.GetMediaFlag(),
                            CTimeKeeper::Now());
            m_qQueueSink->InsertFrame(sAltFrame);
            retval=1;
        }
        else {
            SYSLOG_DEBUG("V2DTx: Throwing away control data from client\n");
        }
        v2dLibSetTime(CTimeKeeper::Now());
        Tx_Go(m_TxSession);
    }
    else {
        // No pending control data to send. m_CtlRcv buffer is free.
        // Check if we need to send a full screen refresh back
        // to the connected Tx
        if (m_bCheckIFrameRequest == true) {

            if (m_srd != NULL &&
                m_srd->OutputIFrameCount > 0 &&
                m_bForceIFrameRequest == false) {
                m_bCheckIFrameRequest = false; // no need to check further
            }
            else    {
                if (((m_enableSRD && m_srd != NULL &&
                    m_srd->OutputIFrameCount == 0 &&
                    m_srd->outFrameCount > 4 &&
                    m_srd->MaxSliceNo > 0) ||
                    m_bForceIFrameRequest == true ) &&
                    m_qQueueSink != NULL &&
                    m_TxSession->m_AV != NULL ) {

                    SYSLOG_DEBUG("V2DTx: Sending special full screen request\n");
                    m_bCheckIFrameRequest = false;
                    m_bForceIFrameRequest = false;
                    Tx_SendFSRefresh(m_TxSession);
                    CV2DStreamMediaFlag v2dSMediaFlag;
                    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
                    nLen = GET_PROTOCOL_PACKET_LENGTH(m_TxSession->m_CtlRcv);
                    pData = (char *)m_TxSession->m_CtlRcv;
                    m_TxSession->m_nFlag &= ~I_CTL;
                    CSFrame *sAltFrame =
                        new CSFrame(pData, nLen,
                                    v2dSMediaFlag.GetMediaFlag(),
                                    CTimeKeeper::Now());

                    m_qQueueSink->InsertFrame(sAltFrame);
                    retval=1;
                }
            }
        }
    }

    UpdateStats(m_TxSession);

    return retval;
}

void CV2dTx::SendConnectionStatus(bool a_status)
{
    if (m_qQueueSink != NULL) {
        CSFrame *pSFrame = new CSFrame(GetCurrentTimeStamp(), 
                a_status ? CSFrame::Connected : CSFrame::Disconnected);
        m_qQueueSink->InsertFrame(pSFrame);
    }
}

#ifdef HAVE_V2O
bool CV2dTx::SendNetworkHeader()
{
    int nLen;
    char pMsgBuffer[128];
    CV2DStreamMediaFlag v2dSMediaFlag;
    unsigned char *cp;
    unsigned short dst_mac[3];

    if (m_qQueueSink == NULL) { 
        return false;
    }

    if (m_TxSession->m_AV->m_nVideoSock > 0) {
        if (((CNETutils *)NULL)->GetDestinationMacAddress(
                    m_TxSession->m_AV->m_nVideoSock, 
                    &m_TxSession->m_AV->m_VideoDestination, 
                    m_TxSession->m_AV->m_isMulticast, dst_mac) < 0) {
            SYSLOG_ERROR("V2DTx: Failed to get destination MAC address");
            return false;
        }
    }
    else if (m_TxSession->m_AV->m_nAudioSock > 0) {
        if (((CNETutils *)NULL)->GetDestinationMacAddress(
                    m_TxSession->m_AV->m_nAudioSock, 
                    &m_TxSession->m_AV->m_AudioDestination, 
                    m_TxSession->m_AV->m_isMulticast, dst_mac) < 0) {
            SYSLOG_ERROR("V2DTx: Failed to get destination MAC address");
            return false;
        }
    }

    if (Tx_BuildNetHeader(m_TxSession, dst_mac, pMsgBuffer, 
                sizeof(pMsgBuffer)) < 0) {
        SYSLOG_ERROR("V2DTx: Failed to build network header");
        return false;
    }

    cp = (unsigned char *) pMsgBuffer;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    nLen = GET_PROTOCOL_PACKET_LENGTH(cp);
    CSFrame *sFrame = new CSFrame(pMsgBuffer, nLen + 4,
                                v2dSMediaFlag.GetMediaFlag(),
                                CTimeKeeper::Now());
    m_qQueueSink->InsertFrame(sFrame);

    m_bNetworkHeadersSent = true;

    return true; 
}
#endif

int CV2dTx::UpdateStats(V2DTx_t *m_TxSession) {

    return 0;
}

int CV2dTx::StartSenderStream(int nQueueId)
{
    // Fix for bugs 2284, 3009. Send a FS Refresh at start time so that there
    // is more time for the FS refresh to be acted upon and the I-frame to come
    // in.
    if (m_TxSession != NULL)
        Tx_SendFSRefresh(m_TxSession);

    // If current object has been stopped and started again,
    // schedule an IFrame request
    if (GetStartCount() > 1) {
        m_bForceIFrameRequest = true;
        m_bCheckIFrameRequest = true;
        SYSLOG_DEBUG("V2DTx: Restart detected: need IFRAME request\n");
    }

    return 0;
}

void CV2dTx::EnableSRD()
{
    m_srd = (sliceRedData_t *)malloc(sizeof(sliceRedData_t));
    if (m_srd != NULL) {
        if (initSRD(m_srd) < 0) {
            destroySRD(m_srd);
            free(m_srd);
            m_srd = NULL;
            m_enableSRD = false;
        }
        else {
            SRDSetNewFrameCallback(m_srd, SRDNewFrameCallback, this);
            setStereoPolicy(m_srd, m_stereoPolicy); // set current policy
            m_srd->slice_drain = 1; // set slice drain mode to true
            SRDSetCodecVersion(m_srd, m_scapProcessor->GetCodecVersion());
        }
    }
}

void CV2dTx::DisableSRD()
{
    if (m_srd != NULL) {
        destroySRD(m_srd);
        free(m_srd);
        m_srd = NULL;
    }
    if (m_TxSession != NULL && m_TxSession->m_AV != NULL) {
        m_TxSession->m_AV->m_nVideoSeq = 0;
        m_TxSession->m_AV->m_nVideoLoss = 0;
        m_TxSession->m_AV->m_nVideoOutOfOrder = 0;
    }
}

const char * CV2dTx::PolicyToString(int a_policy)
{
    switch (a_policy) {
    case SP_SOURCE:
        return "Source";
    case SP_MONO:
        return "Mono";
    case SP_LEFT_ONLY:
        return "Left";
    case SP_RIGHT_ONLY:
        return "Right";
    }
    return NULL;
}

int CV2dTx::Update(CCommand *a_pCmd)
{
    int retval=0;
    int nPort;
    int nSockfd=0;
    char pConnType[80];
    int nBw = 10 * 1024 * 1024;
    int enableSRD;
    char policy[32];
    int tval;

    if (m_TxSession == NULL)
        return 0;

    if ( (a_pCmd->GetIntVar("port", &nPort) != -1) &&
         (a_pCmd->GetIntVar("sockfd", &nSockfd) != -1) ) {
        if (nPort == m_nServerPort) {
            a_pCmd->GetVar("data",pConnType, 80);
            if (strcasecmp(pConnType, "control") == 0) {
                retval = SetHTTPControlSocket(nSockfd);
            }
            else if (strcasecmp(pConnType, "video") == 0) {
                retval = SetHTTPVideoSocket(nSockfd);
            }
            else if (strcasecmp(pConnType, "audio") == 0) {
                retval = SetHTTPAudioSocket(nSockfd);
            }
        }
    }
    else if (a_pCmd->GetIntVar("tcpcontrolfd", &nSockfd) != -1) {
        retval = SetTCPControlSocket(nSockfd);
    }
    else if (a_pCmd->GetIntVar("bandwidth", &nBw) != -1) {
        // The request comes in bits per second
        nBw = nBw/8;
        if (m_pShaper != NULL && nBw > 0) {
            m_pShaper->SetBandwidth(nBw * 8);
            retval = 1;
        }
    }
    else if (a_pCmd->GetIntVar("enablesrd", &enableSRD) != -1) {
        if (enableSRD) {
            if (!m_enableSRD) {
                m_enableSRD = true;
                EnableSRD();
            }
        } else {
            if (m_enableSRD) {
                m_enableSRD = false;
                DisableSRD();
            }
        }
        retval = 1;
    }
    else if (a_pCmd->GetVar("srdpolicy", policy, 32) != -1) {
        if (strcasecmp(policy, "Source") == 0)
            m_stereoPolicy = SP_SOURCE;
        else if (strcasecmp(policy, "Mono") == 0)
            m_stereoPolicy = SP_MONO;
        else if (strcasecmp(policy, "Left") == 0)
            m_stereoPolicy = SP_LEFT_ONLY;
        else if (strcasecmp(policy, "Right") == 0)
            m_stereoPolicy = SP_RIGHT_ONLY;
        else
            retval = -1;
        if (retval == 0) {
            if (m_enableSRD)
                setStereoPolicy(m_srd, m_stereoPolicy);
            retval = 1;
        }
        else {
            retval = 0;
        }
    }
    else if (a_pCmd->GetVar("svcprocessingmode", policy, 32) != -1) {
        int processingMode = SRD_SVC_INVALID_MODE;
        if (strcasecmp(policy, "None") == 0)
            processingMode = SRD_SVC_NULL_MODE;
        else if (strcasecmp(policy, "StripRemLayer") == 0)
            processingMode = SRD_SVC_STRIP_REM_LAYER;
        else if (strcasecmp(policy, "StripQuadLayer") == 0)
            processingMode = SRD_SVC_STRIP_QUAD_LAYER;
        if (m_enableSRD && m_srd != NULL)
            SRDSetSVCProcessingMode(m_srd, processingMode);
        retval = 1;
    }
    else if (a_pCmd->GetIntVar("debugslices", &tval) != -1) {
        if (m_srd) {
            if (tval)
                enableSliceDebug(m_srd);
            else
                disableSliceDebug(m_srd);
        }
        retval = 1;
    }
    else if (a_pCmd->GetIntVar("markslices", &tval) != -1) {
        if (m_srd) {
            if (tval)
                enableSliceMarking(m_srd);
            else
                disableSliceMarking(m_srd);
        }
        retval = 1;
    }
    else if(a_pCmd->GetIntVar("disconnectall", &tval) != -1){
        SYSLOG_INFO("Requesting disconnect to all clients");
        Tx_DisconnectClients(m_TxSession);
        retval = 1;
    }

    return retval;
}


int CV2dTx::SetHTTPControlSocket(int nSockfd)
{
    int retval=0;
    if (!Tx_SetCtlSocket(m_TxSession, nSockfd, 1)) {
        retval = 1;
    }
    return retval;
}

int CV2dTx::SetTCPControlSocket(int nSockfd)
{
    int retval=0;
    if (!Tx_SetCtlSocket(m_TxSession, nSockfd, 0)) {
        retval = 1;
    }
    return retval;
}

int CV2dTx::SetHTTPAudioSocket(int nSockfd)
{
    int retval=0;
    if (!Tx_SetAudioSocket(m_TxSession, nSockfd)) {
        retval = 1;
    }
    return retval;
}

int CV2dTx::SetHTTPVideoSocket(int nSockfd)
{
    int retval=0;
    if (!Tx_SetVideoSocket(m_TxSession, nSockfd)) {
        retval = 1;
    }
    return retval;
}

int CV2dTx::SendFrameToSRD(CSFrame* a_pSFrame)
{
    const int nDataLen    = a_pSFrame->GetLength();
    const int nDataOffset = (m_bIsVideoSrcRTP) ?
                            RTP_HEADER_SIZE : V2D_PACKET_HEADER_SIZE;
    int nDoneLen = 0;
    unsigned char* pCurrData =
        (unsigned char *)(a_pSFrame->GetData() + nDataOffset);

    while (nDoneLen < nDataLen) {
        Tx_CheckForVideoLoss(m_TxSession, (char*)(pCurrData - nDataOffset),
                            m_bIsVideoSrcRTP);
        findFramesinPacket(m_srd, pCurrData);
        pCurrData += nDataOffset + SRD_VIDEO_PACKET_SIZE;
        nDoneLen  += nDataOffset + SRD_VIDEO_PACKET_SIZE;
    }

    return 0;
}

void CV2dTx::ProcessVideoFrame(CSFrame* a_pSFrame)
{
    if (m_TxSession->m_AV && m_TxSession->m_bControlling && !m_enableSRD) {

        char* pData        = a_pSFrame->GetData() + m_nFrameOffset;
        const int nDataLen = (m_bIsVideoSrcRTP) ? RTP_VIDEOBUFF_SIZE :
            AV_BUFFER_SIZE;

        Tx_CheckForVideoLoss(m_TxSession, pData, m_bIsVideoSrcRTP);

        if(m_bIsVideoSrcRTP && m_TxSession->m_bSendRTP) {
            RTPAPPLYNEWSSRC(pData,
            m_RTPContext[RTPCONTEXT_VIDEO].m_nSSRC);

            m_TxSession->m_AV->m_nVideoLen = nDataLen;
        }
        else if(m_bIsVideoSrcRTP && !m_TxSession->m_bSendRTP) {
            pData = ConvertVideoPacketFromRTPToV2D(
                        pData,
                        nDataLen,
                        (char*)m_pAuxPacket,
                        &m_TxSession->m_AV->m_nVideoLen);
        }
        else if(!m_bIsVideoSrcRTP && m_TxSession->m_bSendRTP) {
            RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_VIDEO], m_nTimestamp);
            pData = ConvertVideoPacketFromV2DToRTP(
                        pData,
                        nDataLen,
                        (char*)m_pAuxPacket,
                        &m_TxSession->m_AV->m_nVideoLen,
                        &m_RTPContext[RTPCONTEXT_VIDEO]);
        }
        else {
            m_TxSession->m_AV->m_nVideoLen = nDataLen;
        }

        m_TxSession->m_AV->m_VideoSndPtr = (unsigned char *)pData;
        m_TxSession->m_nFlag |= O_VIDEO;
        m_framesServed++;
        m_bytesServed += m_TxSession->m_AV->m_nVideoLen;

        m_nFrameOffset += nDataLen;
        if(m_nFrameOffset >= a_pSFrame->GetLength())
            m_nFrameOffset = 0;
    }
    // send packet to SRD library
    if (m_enableSRD) {
        SendFrameToSRD(a_pSFrame);
    }

}

void CV2dTx::ProcessAudioFrame(CSFrame* a_pSFrame)
{
    if (m_bTrickMode)
        return;

    if (m_TxSession->m_AV && m_TxSession->m_bControlling) {

        char* pData        = a_pSFrame->GetData() + m_nFrameOffset;
        const int nDataLen = (m_bIsAudioSrcRTP) ? RTP_AUDIOBUFF_SIZE :
            AV_BUFFER_SIZE;

        Tx_CheckForAudioLoss(m_TxSession, pData, m_bIsAudioSrcRTP);

        if(m_bIsAudioSrcRTP && m_TxSession->m_bSendRTP) {
            RTPAPPLYNEWSSRC(pData, m_RTPContext[RTPCONTEXT_AUDIO].m_nSSRC);
            m_TxSession->m_AV->m_nAudioLen = nDataLen;
        }
        else if(m_bIsAudioSrcRTP && !m_TxSession->m_bSendRTP) {
            pData = ConvertAudioPacketFromRTPToV2D(
                        pData,
                        nDataLen,
                        (char*)m_pAuxPacket,
                        &m_TxSession->m_AV->m_nAudioLen);
        }
        else if(!m_bIsAudioSrcRTP && m_TxSession->m_bSendRTP) {
            RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_AUDIO], m_nTimestamp);
            pData = ConvertAudioPacketFromV2DToRTP(
                        pData,
                        nDataLen,
                        (char*)m_pAuxPacket,
                        &m_TxSession->m_AV->m_nAudioLen,
                        &m_RTPContext[RTPCONTEXT_AUDIO]);
        }
        else {
            m_TxSession->m_AV->m_nAudioLen = nDataLen;
        }

        if (m_TxSession->m_nFlag & O_AUDIO) {
            SYSLOG_INFO("Audio not really ready.");
            m_TxSession->m_AV->m_nAudioTxError++;
        }
        else {
            ASSERT(m_TxSession->m_AV->m_AudioSndPtr == NULL);
        }
        m_TxSession->m_AV->m_AudioSndPtr = (unsigned char *)pData;


        m_TxSession->m_nFlag |= O_AUDIO;
        m_framesServed++;
        m_bytesServed += m_TxSession->m_AV->m_nAudioLen;

        m_nFrameOffset += nDataLen;
        if(m_nFrameOffset >= a_pSFrame->GetLength())
            m_nFrameOffset = 0;
#if 0
        // Allow audio packets to be counted against the allocated
        // bandwidth
        if (m_enableSRD && m_pShaper != NULL) {
            m_pShaper->AllowTokens(sFrame->GetLength());
        }
#endif
    }

}

void CV2dTx::ProcessControlFrame(CSFrame* a_pSFrame)
{
    if (m_TxSession->m_nTxState == CLIENT_STATE_READY)
    {
        char *pData = a_pSFrame->GetData();
        if (v2dLibIsScap(pData)) {
            m_bSentCachedScap = true;
        }
        memcpy ((void *)m_TxSession->m_CtlSnd, (void *)pData,
            a_pSFrame->GetLength());
        m_TxSession->m_nFlag |= O_CTL;
        m_framesServed++;
        m_bytesServed += a_pSFrame->GetLength();
    }

}

// Empty out the Free and Available packets queues and
// fill in the Free queue with fresh pointers
void CV2dTx::RecoverSRDPackets() {

    m_pSRDInPacket = NULL;
    m_pSRDOutPacket = NULL;

    while (!m_lSRDFreePackets.empty()) {
        m_lSRDFreePackets.pop();
    }
    for (int i = 0; i < 2; i++) {
        m_lSRDFreePackets.push(m_pSRDPacketBuffer + (SIZE_OF_AV_PACKET * i));
    }

    while (!m_lSRDAvailablePackets.empty()) {
        m_lSRDAvailablePackets.pop();
    }
    m_nSRDPacketOffset = 0;
}

void CV2dTx::ProcessSRDFrame(bool bInProgress)
{

    if (!m_enableSRD)
        return;

    // Wait till first SCAP has been sent by the v2dlib
    if (m_TxSession->m_AV == NULL || m_TxSession->m_bFirstCtrl)
        return;

    // In HTTP mode, the last
    // packet may not have been written to the socket so we hold
    // back. This ensures SRD kicks in even if client TCP
    // connection slows down
    if (m_TxSession->m_nFlag & O_VIDEO) {
        return;
    }

    if (m_pSRDInPacket == NULL) {
       if (!m_lSRDFreePackets.empty()) {
            m_pSRDInPacket = m_lSRDFreePackets.front();
            m_lSRDFreePackets.pop();
        }
    }

    while (true) {
        if (m_pSRDInPacket == NULL)
            break;

        // Till we have SRD implemented on real TX, we just hold back the 1st
        // frame if it is not a I-frame.
//        if (!(m_srd->IFrameCount > 0 || m_srd->inFrameCount > 1))
//            break;

        // Fill up current packet from SRD
        // m_nSRDPacketOffset marks the number of bytes filled so far.
        const int nDataOffset = (m_TxSession->m_bSendRTP)
                        ? RTP_HEADER_SIZE : V2D_PACKET_HEADER_SIZE;
        int needpadding = !bInProgress; // don't pad unless this is a final call
        //needpadding = 0;

        int len = getPackets(m_srd, m_pSRDInPacket + nDataOffset + m_nSRDPacketOffset,
                SEND_BUFFER_SIZE - m_nSRDPacketOffset, needpadding);

        if (len == -1) {
            break;
        }

        m_nSRDPacketOffset += len;

        // Are we done with current packet ?
        if (m_nSRDPacketOffset < SEND_BUFFER_SIZE) {
            break; // Not done yet
        }

        // Fill sequence numbers
        if(!m_TxSession->m_bSendRTP) {
            fillSequence(m_pSRDInPacket, m_srd->audioSeqNum);
            fillSequence(m_pSRDInPacket + SRD_FRAME_SEQ_NUM_LEN,
                    m_srd->videoSeqNum);
        }
        else {
            RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_VIDEO], m_nTimestamp);
            RTPInsertHeader(&m_RTPContext[RTPCONTEXT_VIDEO],
                    m_pSRDInPacket, 1);
        }

        // Get ready for next packet
        m_nSRDPacketOffset = 0;

        m_lSRDAvailablePackets.push(m_pSRDInPacket);
        m_pSRDInPacket = NULL;

        break;
    }

    if (m_pSRDOutPacket == NULL) {
        if (!m_lSRDAvailablePackets.empty()) {
            m_pSRDOutPacket = m_lSRDAvailablePackets.front();
            m_lSRDAvailablePackets.pop();
        }
    }


    // Schedule a video packet to be sent.
    if (m_pSRDOutPacket != NULL && !bInProgress) {

        const int nPktLen = (m_TxSession->m_bSendRTP)
                        ? RTP_VIDEOBUFF_SIZE : AV_BUFFER_SIZE;

        if (m_pShaper != NULL && m_pShaper->CheckTokens(nPktLen)) {
            m_TxSession->m_AV->m_VideoSndPtr =
                    m_pSRDOutPacket;
            m_TxSession->m_AV->m_nVideoLen = nPktLen;
            m_TxSession->m_nFlag |= O_VIDEO;
            m_framesServed++;
            m_bytesServed += m_TxSession->m_AV->m_nVideoLen;
            m_lSRDFreePackets.push(m_pSRDOutPacket);
            m_pSRDOutPacket = NULL;
            m_bSRDPacketSent = true;
        }
    }
}

void CV2dTx::PrintStatistics(string &a_stats)
{
    char cp[512];
    if (m_TxSession) {
        v2dLibSetTime(CTimeKeeper::Now());
        Tx_PrintStatistics(m_TxSession, cp, 511);
    }
    a_stats.append(cp);

    // This block needs a better home. We are temporarily using
    // the fact that this function is called once per sec. TODO: fix me
#ifdef HAVE_V2O
    if (m_pHal != NULL && hal_get_hardware_type(m_pHal) != XPI_DEVICE) {
        hal_LED_keepalive(m_pHal, CONNECTION_LED, 2);
        if (IsConnected()) {
            hal_LED_on(m_pHal, CONNECTION_LED);
        }
        else {
            hal_LED_blink(m_pHal, CONNECTION_LED, LED_BLINK_FAST);
        }
    }
#endif
}


void CV2dTx::UpdateCustomStatus(CCommand *a_pCmd)
{
    if (m_TxSession == NULL)
        return;

    char buff[64];
    a_pCmd->SetObjectCounterParam("FramesServed", m_framesServed);
    a_pCmd->SetObjectByteCounterParam("BytesServed", m_bytesServed);
    a_pCmd->SetObjectParam("Client",
                           inet_ntoa(m_TxSession->m_Destination.sin_addr));
    a_pCmd->SetObjectConstantParam("Port",
                                   m_TxSession->m_nSvrPort);
    a_pCmd->SetObjectErrorCounterParam("ResetCount",
                                       m_TxSession->m_nResetCount);
    if (a_pCmd->StatusLevel(1)) {
        a_pCmd->SetObjectParam("ProtocolVersion",
                               getV2DProtocolVersion(m_TxSession->m_version,
                                                     buff, 31));
    }
    a_pCmd->SetObjectParam("VideoName",
                           (const char *) m_TxSession->m_videoName);
    a_pCmd->SetObjectParam("MulticastAddress",
                           (const char *) m_TxSession->m_McastAddress);
    if (m_TxSession->m_AV != NULL && m_TxSession->m_AV->m_isMulticast) {
        a_pCmd->SetObjectParam("Multicast", true);
        a_pCmd->SetObjectParam("MulticastInterfaces", m_sInterfaceInfo);
    } else {
        a_pCmd->SetObjectParam("Multicast", false);
        a_pCmd->SetObjectParam("MulticastInterfaces", "");
    }
    Tx_GetControllingClientIP(m_TxSession, buff, 64);
    a_pCmd->SetObjectParam("ControllingClient", buff);
    a_pCmd->SetObjectParam("ExpectedClient",
                           inet_ntoa(m_TxSession->m_expectedClient));
    if (a_pCmd->StatusLevel(4)) {
        a_pCmd->SetObjectParam("AuthToken", Tx_GetAuthToken(m_TxSession));
        a_pCmd->SetObjectParam("ClientToken", Tx_GetClientAuthToken(m_TxSession));
    }
    a_pCmd->SetObjectByteCounterParam("KeyboardRcvd",
                           m_TxSession->m_nKeyboardRcvd);
    a_pCmd->SetObjectByteCounterParam("MouseRcvd",
                           m_TxSession->m_nMouseRcvd);
    a_pCmd->SetObjectByteCounterParam("SerialRcvd",
                           m_TxSession->m_nSerialRcvd);
    a_pCmd->SetObjectCounterParam("CtrlPacketsRcvd",
                                  m_TxSession->m_nCtrlRcvPkts);
    a_pCmd->SetObjectByteCounterParam("CtrlBytesRcvd",
                                      m_TxSession->m_nCtrlRcvBytes);
    a_pCmd->SetObjectCounterParam("CtrlPacketsSent",
                                  m_TxSession->m_nCtrlSndPkts);
    a_pCmd->SetObjectCounterParam("CtrlPacketsDropped",
                                  m_TxSession->m_nCtlPacketsDropped);
    a_pCmd->SetObjectByteCounterParam("CtrlBytesSent",
                                      m_TxSession->m_nCtrlSndBytes);
    a_pCmd->SetObjectByterateGuageParam("RequestedBandwidth",
                                        m_TxSession->m_nRequestBandwidth);
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

    if (m_TxSession->m_AV != NULL) {

        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectConstantParam("AVId",
                                           m_TxSession->m_AV->m_nId);
            a_pCmd->SetObjectParam("Controlling",
                                   (bool)m_TxSession->m_bControlling);

        }
        a_pCmd->SetObjectConstantParam("VideoPort",
                           ntohs(m_TxSession->m_AV->m_VideoDestination.sin_port));
        a_pCmd->SetObjectBitrateGuageParam("VideoRate",
                               m_TxSession->m_AV->m_nCurrVideoRate);
        a_pCmd->SetObjectByteCounterParam("VideoBytes",
                                          m_TxSession->m_AV->m_nVideoBytes);
        a_pCmd->SetObjectCounterParam("VideoPackets",
                                      m_TxSession->m_AV->m_nVideoPkts);

        a_pCmd->SetObjectErrorCounterParam("VideoLoss",
                                           m_TxSession->m_AV->m_nVideoLoss);
        a_pCmd->SetObjectErrorCounterParam("VideoOutOfOrder",
                                           m_TxSession->m_AV->m_nVideoOutOfOrder);
        a_pCmd->SetObjectErrorCounterParam("VideoTxError",
                                           m_TxSession->m_AV->m_nVideoTxError);
        a_pCmd->SetObjectConstantParam("AudioPort",
                             ntohs(m_TxSession->m_AV->m_AudioDestination.sin_port));
        a_pCmd->SetObjectBitrateGuageParam("AudioRate",
                                       m_TxSession->m_AV->m_nCurrAudioRate);
        a_pCmd->SetObjectByteCounterParam("AudioBytes",
                                          m_TxSession->m_AV->m_nAudioBytes);
        a_pCmd->SetObjectCounterParam("AudioPackets",
                                      m_TxSession->m_AV->m_nAudioPkts);
        a_pCmd->SetObjectErrorCounterParam("AudioLoss",
                                           m_TxSession->m_AV->m_nAudioLoss);
        a_pCmd->SetObjectErrorCounterParam("AudioOutOfOrder",
                                           m_TxSession->m_AV->m_nAudioOutOfOrder);
        a_pCmd->SetObjectErrorCounterParam("AudioTxError",
                                           m_TxSession->m_AV->m_nAudioTxError);

        a_pCmd->SetObjectParam("HTTPMode",
                                   (bool) Tx_IsHTTPMode(m_TxSession));
        if (a_pCmd->StatusLevel(1) && Tx_IsHTTPMode(m_TxSession)) {
            a_pCmd->SetObjectCounterParam("VideoTxThrottled",
                                          m_TxSession->m_AV->m_nVideoTxThrottled);
        }

    }

    if (a_pCmd->StatusLevel(1)) {
        a_pCmd->SetObjectParam("PayloadType",
            ((m_TxSession->m_bSendRTP == TRUE) ? "RTP" : "V2D"));
    }
    a_pCmd->SetObjectConstantParam("CodecVersion", m_scapProcessor->GetCodecVersion());

    if (a_pCmd->StatusLevel(4)) {
        a_pCmd->SetObjectParam("AudioTS",
                m_RTPContext[RTPCONTEXT_AUDIO].m_nTimeStamp);
        a_pCmd->SetObjectParam("VideoTS",
                m_RTPContext[RTPCONTEXT_VIDEO].m_nTimeStamp);

        int iSourceMediaType =
            (m_bIsVideoSrcRTP ? 1 : 0) | (m_bIsAudioSrcRTP ? 2 : 0);
        a_pCmd->SetObjectParam("SourceMediaType", iSourceMediaType);
    }

    if (a_pCmd->StatusLevel(2) && m_TxSession) {
        a_pCmd->SetObjectErrorCounterParam(
            "DuplicateConnections",
            m_TxSession->m_nSimultaneousConnectionsRefused);
    }

    a_pCmd->SetObjectParam("SRDEnabled",
                           (bool) m_enableSRD);


    if (a_pCmd->StatusLevel(1)) {
        a_pCmd->SetObjectParam("SRDStereoPolicy",
                               (const char *)PolicyToString(m_stereoPolicy));
    }
    // SRD related stats
    if (m_enableSRD) {
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
            /*a_pCmd->SetObjectGuageParam("InputStaticFrame",
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

        if (a_pCmd->StatusLevel(2) && m_pShaper != NULL)
            m_pShaper->UpdateCustomStatus(a_pCmd);
    }

}

void CV2dTx::SetCustomStatusInfo()
{
    SetInfoString("FramesServed", "Number of CFrames sent to the V2DLib.");

    SetInfoString("BytesServed", "Number of bytes sent to the V2DLib.");

    SetInfoString("Client", "IP address of currently connected client.");

    SetInfoString("Port", "TCP port on which this module is listening on for client connections.");

    SetInfoString("VideoName", "The LUT \"name\" representing this video.");

    SetInfoString("MulticastAddress", "Multicast Address if enabled, else empty string.");

    SetInfoString("Multicast", "True if serving in multicast mode.");

    SetInfoString("KeyboardRcvd", "Number of keyboard packets received on the control channel.");

    SetInfoString("MouseRcvd", "Number of mouse packets received on the control channel.");

    SetInfoString("SerialRcvd", "Number of serial packets received on the control channel.");

    SetInfoString("CtrlPacketsRcvd", "Number of control packets received.");

    SetInfoString("CtrlBytesRcvd", "Number of control bytes received.");

    SetInfoString("CtrlPacketsSent", "Number of control packets sent.");

    SetInfoString("CtrlBytesSent", "Number of control bytes sent.");

    SetInfoString("RequestedBandwidth", "Bandwidth requested by the client subject to the local upper limit set (currently 100 mpbs).");

    SetInfoString("MaxQueueLen", "The maximum length of this objects message queue seen since last stat update.");

    SetInfoString("AverageQueueLen", "The average length of this objects message queue.");

    SetInfoString("BackOffCount", "Local Flow Control backoff when SRD is disabled.");

    SetInfoString("ByteBucket", "Local Flow Control byte bucket when SRd is disabled.");

    SetInfoString("BurstSize", "Local Flow control burst size.");

    SetInfoString("AVId", "The unique ID assigned by the V2Dlib for the AV object (for sharing multicast streaming).");

    SetInfoString("VideoPort", "UDP port video is being sent to .");

    SetInfoString("VideoRate", "Video bit rate as seen by V2Dlib.");

    SetInfoString("VideoBytes", "Video bytes sent as seen by V2Dlib.");

    SetInfoString("VideoPackets", "Number of video packets sent as seen by V2Dlib.");

    SetInfoString("VideoTxError", "Network errors while trying to send video packets.");

    SetInfoString("AudioPort", "UDP port audio is being sent to .");

    SetInfoString("AudioRate", "Audio bit rate as seen by V2Dlib.");

    SetInfoString("AudioBytes", "Audio bytes sent as seen by V2Dlib.");

    SetInfoString("AudioPackets", "Number of audio packets sent as seen by V2Dlib.");

    SetInfoString("AudioLoss", "Audio packets lost.");

    SetInfoString("AudioOutOfOrder", "Number of out of order audio packets detected.");

    SetInfoString("AudioTxError", "Network errors while trying to send audio packets.");

    SetInfoString("SRDEnabled", "Is SRD enabled?.");

    SetInfoString("SRDStereoPolicy", "Stereo policy: Same as Source | Mono | Left Only  | Right Only.");

    SetInfoString("VideoLoss", "Video packets lost.");

    SetInfoString("VideoOutOfOrder", "Number of out of order video packets detected.");

    SetInfoString("SRDStereoDetect", "Did we detect Stereo or Mono? If mono, were they right frames or left frames?.");

    SetInfoString("SRDRightFrames", "Number of \"right\" frames in stereo mode.");

    SetInfoString("SRDInPackets", "Number of packets sent to SRD module.");

    SetInfoString("SRDOutPackets", "Number of packets retrieved from SRD module.");

    SetInfoString("SRDInFrames", "Number of incoming video frames detected by SRD module.");

    SetInfoString("SRDPrefillFrames", "Number of outgoing video frames served in pre-fill mode.");

    SetInfoString("SRDOutFrames", "Number of output video frames served by SRD module.");

    SetInfoString("SRDReducedFrames", "Number of frames reduced by SRD module.");

    SetInfoString("SRDInSlices", "Number of incoming video slices detected by SRD module.");

    SetInfoString("SRDOutSlices", "Number of output video slices served by SRD module.");

    SetInfoString("SRDReducedSlices", "Number of slices reduced by SRD module.");

    SetInfoString("SRDInBytes", "Number of incoming video bytes detected by SRD module.");

    SetInfoString("SRDOutBytes", "Number of output video bytes served by SRD module.");

    SetInfoString("SRDReducedBytes", "Number of bytes reduced by SRD module.");

    SetInfoString("SRDPadBytes", "Number of bytes that needed to be padded while creating output packets.");

    SetInfoString("SRDSliceOutOfOrder", "Number of out of order slices detected by SRD within an incoming frame.");

    SetInfoString("SRDBadSlice", "Number of slices greater than the maximum expected slice number based on current frame dimensions.");

    SetInfoString("SRDBadFrame", "Number of frames having \"Bad Slices\" in them.");

    SetInfoString("SRDSOFError", "Number of packets ignored due no SOF marker (ignoring the first partial frame).");

    SetInfoString("SRDIFrameCount", "Number of incoming IFRAMES (frames that have all slices present)");

    SetInfoString("SRDOutputIFrameCount", "Number of output IFRAMES (frames that have all slices present)");

    SetInfoString("SRDFullFrameCount", "Number of times  a full screen refresh sent is sent to client (affected by Min/Max Refresh seting on Tx)");

    SetInfoString("SRDFullFrameInterval", "The number of frames needed for the last full screen refresh sent to client");

    SetInfoString("SRDFirstFFInterval", "This stores the \"first\" SRDFullFrameInterval");

    SetInfoString("SRDFrameOverflow", "Number of times input data was rejected due to input holding buffer overflow");

    SetInfoString("SRDSliceSize", "Average Slice size in bytes");

    SetInfoString("SRDFrameCoverage", "What percent of slices are present in each frame.");

    SetInfoString("SRDSliceDebug", "Is Slice Debug feature on (highlight unchanged slices) ?");

    SetInfoString("SRDSliceMarking", "Is Slice Marking feature on (highlight reduced slices) ?");

    SetInfoString("SRDBufferPool", "Current size of pre-allocated slice buffer pool");

    SetInfoString("DuplicateConnections", "Duplicate connections that were ignored.");
}

void CV2dTx::PrintError(CNotify *a_notify)
{
#if 0
    char cp[256];
    strcpy(cp, "");
    if (m_TxSession)
        Tx_SendError(m_TxSession, cp, 255);
    a_error.append(cp);
#endif
}

void CV2dTx::PrintNotification(CNotify *a_notify)
{

    switch (m_nEventId) {
    case NOTIFICATION_EVENT_CONNECTED:
    case NOTIFICATION_EVENT_DISCONNECTED:
        if (m_TxSession != NULL) {
            a_notify->SetNotificationData("ClientIP",
                                          inet_ntoa(m_TxSession->m_Destination.sin_addr));
        }
        else {
            a_notify->SetNotificationData("ClientIP", "");
        }
        break;
    default:
        break;
    }

}

