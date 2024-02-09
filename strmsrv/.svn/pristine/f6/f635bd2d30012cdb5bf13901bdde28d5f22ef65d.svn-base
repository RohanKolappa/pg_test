#include "strmsrv_compatibility.h"
#include "rtprx.hh"
#include "common.hh"

using namespace std;

/// The videoCodec is the codec e.g. in the rtpmap line in the sdp
/// e.g. a=rtpmap:96 MP4V-ES/600, the Encoding is "MP4VES"
/// This is used to figure out the codec specific flags to be
/// inserted in the stored frame which will then by used by RtpTx
/// Examples of codec specific flags are MPEG4_I_FRAME, MPEG4_B_FRAME
CRtpRx::CRtpRx(int nId, char *ip, char *audioip, int videoPort, int audioPort,
        char *videoCodec, char *audioCodec, intlist & a_interfaceList) :
    CReceiver(nId)
{

    m_nPortType = RTPNETWORKPORT;

    m_strSrcIP = ip;
    if (strlen(audioip) > 0)
        m_strAudioSrcIP = audioip;
    else
        m_strAudioSrcIP = ip;
    char audioip_str[64];
    strcpy(audioip_str, m_strAudioSrcIP.c_str());

    m_strRemoteIP = "";

    m_nAudioPort = audioPort;
    m_nVideoPort = videoPort;

    _session = new RTPSession(a_interfaceList);

    CRTPStreamMediaFlag rtpSMediaFlag(FRAMETYPE_RTP_ENCAPS_RTP,
            FRAMETYPE_RTP_CHAN_AUDIO);
    int status = RTP_OK;
    if (audioPort > 0) {
        status = _session->addSubSession(audioPort,
                rtpSMediaFlag.GetMediaFlag(), audioip_str, audioPort, 0);

        if (status != RTP_OK) {
            SYSLOG_ERROR("Failed to add RTP SubSession for audioPort: %d", audioPort);
            SetDisConnected();
            return;
        }
    }
    rtpSMediaFlag.SetChannel(FRAMETYPE_RTP_CHAN_VIDEO);
    if (videoPort > 0) {
        status = _session->addSubSession(videoPort,
                rtpSMediaFlag.GetMediaFlag(), ip, videoPort, 2*1024*1024);

        if (status != RTP_OK) {
            SYSLOG_ERROR("Failed to add RTP SubSession for videoPort: %d", videoPort);
            SetDisConnected();
            return;
        }
    }

    SetConnected();

}

CRtpRx::~CRtpRx()
{
    delete _session;
}

// TODO: Optmize this by using SBuffer to save a memcpy
int CRtpRx::updateRxFrame(char *pData, int nLen, unsigned long nMediaFlag)
{

    CSFrame * sFrame = new CSFrame(pData, nLen, nMediaFlag, CTimeKeeper::Now());

    m_qQueueSink->InsertFrame(sFrame);
    m_bytesTransfered += nLen;
    m_frameCount++;

    return 0;
}

int CRtpRx::ProcessStream()
{
    int nRetVal = 0;
    if (m_qQueueSink == NULL)
        return nRetVal;

    list<RTPSessionPkt *> pkts;
    list<RTPSessionPkt *>::iterator pIter;

    if (m_nTimestamp == 0) {
        m_nStartTimestamp = CTimeKeeper::Now();
        m_nStartWallClock = CTimeKeeper::Now();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_BEGIN;
    }
    m_nTimestamp = CTimeKeeper::Now();
    m_nFrameWallClock = CTimeKeeper::Now();

    int status = _session->receivePackets(pkts);
    if (status != RTP_OK) {
        SetDisConnected();
        return nRetVal;
    }
    for (pIter = pkts.begin(); pIter != pkts.end(); ++pIter) {
        RTPSessionPkt *p = (*pIter);
        updateRxFrame((char *) (p->_rtpHeader), p->_size, p->_frameType);
        free(p->_rtpHeader);
        p->_rtpHeader = NULL;
        delete p;

        nRetVal++;
    }
    pkts.clear();

    return nRetVal;
}

void CRtpRx::PrintStatistics(string &a_stats)
{
    char cp[256];
    sprintf(cp, "p:0 b:0 r:0 f:0");
    a_stats.append(cp);
}

void CRtpRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("SourceIP", m_strSrcIP.c_str());
    a_pCmd->SetObjectParam("AudioSourceIP", m_strAudioSrcIP.c_str());

    if (m_strRemoteIP == "") {
        int iSubSessionId = (m_nVideoPort > 0) ? m_nVideoPort : m_nAudioPort;
        m_strRemoteIP = _session->getPeerAddress(iSubSessionId);
    }
    a_pCmd->SetObjectParam("RemoteIP", m_strRemoteIP.c_str());

    a_pCmd->SetObjectConstantParam("AudioPort", m_nAudioPort);
    a_pCmd->SetObjectConstantParam("VideoPort", m_nVideoPort);

    a_pCmd->SetObjectCounterParam("AudioPackets", _session->getPacketCount(
            m_nAudioPort));
    a_pCmd->SetObjectCounterParam("VideoPackets", _session->getPacketCount(
            m_nVideoPort));

    a_pCmd->SetObjectByteCounterParam("AudioBytes", _session->getByteCount(
            m_nAudioPort));
    a_pCmd->SetObjectByteCounterParam("VideoBytes", _session->getByteCount(
            m_nVideoPort));

    a_pCmd->SetObjectErrorCounterParam("AudioLoss", _session->getPacketLoss(
            m_nAudioPort));
    a_pCmd->SetObjectErrorCounterParam("VideoLoss", _session->getPacketLoss(
            m_nVideoPort));
    a_pCmd->SetObjectParam("Multicast", _session->isMulticast());
    a_pCmd->SetObjectParam("MulticastInterfaces", _session->getInterfaceInfo());
}
