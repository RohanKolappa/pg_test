#include "strmsrv_compatibility.h"
#include "v2d.h"
#include "fpgaaudiorx.hh"
#include "rtputils.hh"
#include "netutils.hh"

using namespace std;

CPortUse CFPGAAudioRx::m_portsUsed;

CFPGAAudioRx::CFPGAAudioRx (int nId,
                            int nIOPort,
                            int naudiotype) :
    CReceiver (nId),
    m_hAudio (-1),
    m_nMaxfd (0),
    m_nAudioType (0)
{
    m_nPortType = V2DAUDIOENCODERPORT;

    /* Make sure the port is not already in use */
    if (m_portsUsed.ReservePort(nIOPort)) {
        m_nIOPort = nIOPort;
    }
    else {
        m_nIOPort = -1;
    }
    m_pHal = NULL;
    if (m_nIOPort > 0)
        m_pHal = hal_acquire(m_nIOPort - 1);

    if (m_pHal != NULL) {
        m_hAudio = hal_get_audio_fd(m_pHal);
    }
    else {
        SetError();
    }

    m_bXpiDevice = false;
    if(hal_get_hardware_type(m_pHal) == XPI_DEVICE) {
         m_bXpiDevice = true;
    }

    if (!m_bXpiDevice && m_hAudio <= 0) {
        SYSLOG_ERROR("Failed to initialize audio file handle in V2dAudioEncoder module");
    }

    if (m_nMaxfd < m_hAudio) m_nMaxfd = m_hAudio;

    RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_AUDIO], CTimeKeeper::Now());
    RTPInitContext(&m_RTPContext[RTPCONTEXT_AUDIO], RTPAUDIOCLOCKFREQ,
                    V2D_AUDIO_SAMPLESPERFRAME, RTPAUDIOPAYLOAD);

    m_nAudioType = naudiotype;

    m_bV2DI50Rx = false;
    if((hal_get_hardware_type(m_pHal) == V2DI50_DEVICE)
            && (hal_get_board_type(m_pHal) == BOARDTYPE_RX)) {
         m_bV2DI50Rx = true;
    }
}

CFPGAAudioRx::~CFPGAAudioRx() {

    if (m_pHal != NULL)
        hal_release(m_pHal);
    /* Release the FPGA port */
    if (m_nIOPort > 0)
        m_portsUsed.ReleasePort(m_nIOPort);

}

int
CFPGAAudioRx::GetAudioFrame() {
    //char pData[AV_BUFFER_SIZE];
    char pAudioBuffer[AV_BUFFER_SIZE];
    int nLen = 0;
    int nOutLen = 0;

    ASSERT(m_hAudio > 0);

    nLen = read(m_hAudio, pAudioBuffer, AV_BUFFER_SIZE);

    if (nLen <= 0)
        return nLen;
    // Reset the audio and discard the data if it's not SOF alighed
    unsigned char sof[] = {0x00, 0x00, 0x01, 0xb3, 0x00, 0x00, 0x00, 0x00};
    if (memcmp(&pAudioBuffer[16], sof, 8) != 0) {
        m_nAudioSOFerrors++;
        hal_resetAddsideAudio(m_pHal);
        return 0;
    }
#if 0
    /* Audio Type is forced to be of default type. because for reverse audio
    the 4 bytes of audio stream identifier is not 0. Hence while packing it
    simply copies the i/p buffer into o/p buffer. While unpacking there would
    be a problem because ConvertAudioPacketFromRTPToV2D introduces thoes 4
    bytes of 0 and unpack tries to unpack if it is of any other kind of
    audio type. There would be a segmentation fault hence we are forcing
    Audio Type to AUDIOTYPE_DEFAULT in receiver and sender in reverse audio */

        m_nAudioType = AUDIOTYPE_DEFAULT;
        nLen = v2dLibAudioPack((unsigned char*)pData,
                            (unsigned char*)pAudioBuffer,
                            nLen,
                            m_nAudioType);
        Data = (char *) pData;
#endif
    char * Data = (char *) pAudioBuffer;
    RTPSetCurrentTime(&m_RTPContext[RTPCONTEXT_AUDIO], CTimeKeeper::Now());
    if (m_bV2DI50Rx == true) {
        Data = ConvertAudioPacketFromV2DToRTPRx(
                    (char *)pAudioBuffer,
                    nLen,
                    NULL,
                    &nOutLen,
                    &m_RTPContext[RTPCONTEXT_AUDIO]);
    }
    else {
        Data = ConvertAudioPacketFromV2DToRTP(
                    (char *)pAudioBuffer,
                    nLen,
                    (char *)pAudioBuffer,
                    &nOutLen,
                    &m_RTPContext[RTPCONTEXT_AUDIO]);
    }
    CRTPStreamMediaFlag rtpSMediaFlag(FRAMETYPE_RTP_ENCAPS_RTP,
                                        FRAMETYPE_RTP_CHAN_AUDIO);

    CSFrame * sFrame = new CSFrame(Data, nOutLen,
                                    rtpSMediaFlag.GetMediaFlag(),
                                    m_nTimestamp);

    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);
    m_frameCount++;
    m_bytesTransfered += nLen;
    return nLen;
}

int CFPGAAudioRx::ProcessStream() {
    struct timeval timeout;
    fd_set read_fdset;
    int nfds = 0;
    int retval = 0;

    // Dont do anything till we are started
    if (m_qQueueSink == NULL)
        return 0;

    if (m_bXpiDevice || IsErrored())
        return 0;

    if (m_nTimestamp == 0) {
        m_nStartTimestamp = CTimeKeeper::Now();
        m_nStartWallClock = CTimeKeeper::Now();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_BEGIN;
    }
    m_nTimestamp = CTimeKeeper::Now();
    m_nFrameWallClock = CTimeKeeper::Now();

    FD_ZERO(&read_fdset);
    if (m_hAudio > 0)
        FD_SET(m_hAudio, &read_fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    nfds = select(m_nMaxfd + 1, &read_fdset, NULL, NULL, &timeout);

    if (nfds < 0) {
        // Error Handling code
    }
    else if (nfds == 0) {
        // Timeout no data
    }
    else {
        if (FD_ISSET(m_hAudio, &read_fdset)) {
            if (GetAudioFrame() > 0)
                retval++;
        }
    }
    return retval;
}


void
CFPGAAudioRx::PrintStatistics(string &a_stats)
{
}

int
CFPGAAudioRx::Update(CCommand *a_pCmd)
{
    char pDest[80];
    int nDestPort = 0;
    int nSrcPort = 0;
    int nXmit = 0;
    int retval = 1;

    strcpy(pDest, "");

    if (a_pCmd->GetIntVar("xmit", &nXmit) == -1) 
        return 0;

    if (nXmit) {
        unsigned int ipaddr;
        unsigned short macaddr[3];
        if (a_pCmd->GetVar("dstip", pDest, 80) == -1) 
            return 0;
        if (a_pCmd->GetIntVar("dstport", &nDestPort) == -1) 
            return 0;
        if (a_pCmd->GetIntVar("srcport", &nSrcPort) == -1) 
            return 0;
        if (((CNETutils *)NULL)->GetMacForIp(pDest, macaddr)) {
            ipaddr = (((CNETutils *)NULL)->GetInAddr(pDest));
            hal_set_nethdr(m_pHal, NETHDR_AUDIO_OUT_CONTEXT, nSrcPort, 0,
                ipaddr, macaddr, nDestPort);
            SYSLOG_INFO("Starting streaming reverse audio on XPi decoder");
        }
        else {
            SYSLOG_INFO("Failed to get mac address for %s", pDest);
        }
    }
    else {
        SYSLOG_INFO("Stop streaming reverse audio on XPi decoder");
        hal_clear_nethdr(m_pHal, NETHDR_AUDIO_OUT_CONTEXT);
    }
    return retval;
}

void
CFPGAAudioRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("FPGAAudioPort", m_nIOPort);
    a_pCmd->SetObjectCounterParam("AudioPackets", m_frameCount);
    a_pCmd->SetObjectByteCounterParam("AudioBytes", m_bytesTransfered);
    a_pCmd->SetObjectParam("AudioType", m_nAudioType);
    a_pCmd->SetObjectParam("IsI50Rx", m_bV2DI50Rx);
    a_pCmd->SetObjectErrorCounterParam("AudioSOFerrors", m_nAudioSOFerrors);
}





