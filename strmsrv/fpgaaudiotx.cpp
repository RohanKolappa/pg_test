#include "strmsrv_compatibility.h"
#include "fpgaaudiotx.hh"
#include "common.hh"
#include "rtputils.hh"
#include "netutils.hh"

using namespace std;

CPortUse CFPGAAudioTx::m_portsUsed;

CFPGAAudioTx::CFPGAAudioTx (int nId,
                            int nIOPort,
                            int naudioport,
                            int naudiotype) :
    CSender (nId),
    m_nAudioPackets (0),
    m_hAudio (-1),
    m_nAudioType (0)
{
    m_nPortType = V2DAUDIODECODERPORT;

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

    int hardware_type, board_type;
    hardware_type = hal_get_hardware_type(m_pHal);
    board_type = hal_get_board_type(m_pHal);

    m_bXpiDevice = false;
    if (hardware_type == XPI_DEVICE)
        m_bXpiDevice = true;

    if (m_bXpiDevice && board_type == BOARDTYPE_TX) {
        char sIPAddress[33];
        if (((CNETutils *)NULL)->GetLocalIPAddress(sIPAddress, 32)) {
            SYSLOG_INFO("Enabling receiving reverse audio on port %d.", 
                        naudioport);
            if (hal_set_rtpfilter(m_pHal, RTPFILTER_INDEX_AUDIO,
                    htonl(inet_addr(sIPAddress)), naudioport, 0) < 0) {
                SYSLOG_ERROR("Failed to write RTP filter.");
            }
        }
    }
    else {
        if (m_hAudio <= 0) {
            SYSLOG_ERROR("Failed to initialize audio file handle in V2dAudioDecoder module");
        }
    }
    m_nAudioType = naudiotype;
    m_pRevAudioBuffer = new char[REVAUDIO_BUFF_SIZE];
    m_nBufferLen = 0;
    m_bV2DI50Tx = false;
    m_nAudioFrameSize = V2D_AUDIO_FRAME_SIZE;
    if (hardware_type == V2DI50_DEVICE && board_type == BOARDTYPE_TX) {
        m_bV2DI50Tx = true;
        m_nAudioFrameSize = SEND_BUFFER_SIZE;
   } 
};

CFPGAAudioTx::~CFPGAAudioTx()
{
    if (m_bXpiDevice) {
        SYSLOG_ERROR("Disabling reverse audio on XPi Encoder.");
        hal_clear_rtpfilter(m_pHal, RTPFILTER_INDEX_AUDIO);
    }
    /* Release the FPGA port */
    if (m_nIOPort > 0)
        m_portsUsed.ReleasePort(m_nIOPort);

    delete [] m_pRevAudioBuffer;

    if (m_pHal != NULL)
        hal_release(m_pHal);
} 

void 
CFPGAAudioTx::ProcessFrameTypeAudio(CSFrame *sFrame){ 
    unsigned char tmpbuff[REV_AV_BUFFER_SIZE];

    int lenDataIn = 0;

    if (sFrame->GetLength() + m_nBufferLen  <= REVAUDIO_BUFF_SIZE) {
        if (sFrame->GetStreamType() == STREAMTYPE_RTP) {
            WriteAudioPacketIntoBuffer(sFrame->GetData(),
                                        sFrame->GetLength(),
                                        (char *)m_pRevAudioBuffer,
                                        &m_nBufferLen);
        }
        else {
            if (++m_errors == 1)
                SYSLOG_INFO("FPGAAUDIOTX:Non RTP steam type.\n");
        }
    }
    else {
        if (++m_errors == 1)
            SYSLOG_INFO("FPGAAUDIOTX:Audio buffer overflow: Dropping packets\n");
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
    len = v2dLibAudioUnPack(tmpbuff, pDataIn, lenDataIn,
                            m_nAudioType);

    if (len < 0) {
        if(++m_errors == 1)
            SYSLOG_INFO("Could not unpack audio data");
        return;
    }
    if (len == 0){
        if(++m_errors == 1)
            SYSLOG_INFO("EOF on audio data");
        return;
    }
    else if (len < 32){
        if(++m_errors == 1)
            SYSLOG_INFO("Audio packet too short");
    }
#endif
    if (m_hAudio > 0) {
        while (m_nBufferLen >= m_nAudioFrameSize) {
            if( m_bV2DI50Tx == true) {
                ConvertAudioPacketToFPGAI50Tx(m_pRevAudioBuffer,
                                            m_nAudioFrameSize,
                                            (char *)tmpbuff,
                                            &lenDataIn);
            }
            else {
                ConvertAudioPacketToFPGA(m_pRevAudioBuffer,
                                        m_nAudioFrameSize,
                                        (char *)tmpbuff,
                                        &lenDataIn);
            }
            m_nBufferLen = m_nBufferLen - m_nAudioFrameSize;
            memmove(m_pRevAudioBuffer, m_pRevAudioBuffer + m_nAudioFrameSize, m_nBufferLen);
            int len;
            if ((len = write(m_hAudio, tmpbuff, lenDataIn)) < 0) {
                if (++m_errors == 1)
                    SYSLOG_INFO("FPGAAUDIOTX:RETURN AFTER WRITE , LEN  %d\n", len);
            }
        }
        m_nAudioPackets++;
    }
}

int
CFPGAAudioTx::ProcessStream()
{

    int retval = 0;

    if (m_bXpiDevice)
        return 0;

    if (m_qQueueSource != NULL) {

        CSFrame *sFrame = m_qQueueSource->GetFrame();
        if (sFrame) {
            if (m_nTimestamp == 0) {
                HandleFirstTimestamp(sFrame->GetTimeStamp());
            }
            m_nTimestamp = sFrame->GetTimeStamp();
            m_nFrameWallClock = CTimeKeeper::Now();

            if (sFrame->GetMessageType() == CSFrame::Normal) {
                m_bytesTransfered += sFrame->GetLength();
                m_frameCount++;
                ProcessFrameTypeAudio(sFrame);
            }

            m_qQueueSource->ReleaseFrame(sFrame);
            retval = 1;
        }
        else {
            if (m_nTimestamp == 0)
                CheckQueueTimestamp();

        }
    }
    return retval;
}

void
CFPGAAudioTx::PrintStatistics(string &a_stats)
{
}


void
CFPGAAudioTx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("FPGAAudioPort", m_nIOPort);
    a_pCmd->SetObjectCounterParam("AudioPackets", m_nAudioPackets);
    a_pCmd->SetObjectByteCounterParam("AudioBytes", m_bytesTransfered);
    a_pCmd->SetObjectParam("AudioType", m_nAudioType);
    a_pCmd->SetObjectParam("IsI50Tx", m_bV2DI50Tx);
}





