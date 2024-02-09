
#include "strmsrv_compatibility.h"
#include <math.h>
#include "v2d.h"
#include "fpgarx.hh"
#include "rfbhandler.hh"
#include "v2o_lib.h"
#include "splash.hh"
#include "codecdefs.h"
#include "USB.h"
#include "KbmIPCUtils.hh"

using namespace std;

// TODO: Prevent Raw KBM when RFB is active

#define MAX_PROTOCOL_UNIT_SIZE                128
#define RELMOUSE_PKT_LEN   4  /* Relative mouse pak len is 4  configured in kernel */
#define ABSMOUSE_PKT_LEN   6
#define KEYBOARD_PKT_LEN   8 
#define KBMCRES "/tmp/current_resolution"

CPortUse CFPGARx::m_portsUsed;

CFPGARx::CFPGARx(int nId, int nIOPort) :
    CReceiver(nId)
{

    m_nPortType = V2DENCODERPORT;

    InitVariablesToZero();

    m_bDisplaySplash = false;
    m_bRealVideo = true;
    m_nVideoChanged = 1;
    m_tLastKbWritten = CTimeKeeper::Now();

    m_pVideoBuffer = new char[AV_BUFFER_SIZE + 1];
    m_pAudioBuffer = new char[AV_BUFFER_SIZE + 1];
    m_pScapBuffer = new char[AV_BUFFER_SIZE + 1];
    m_pKbBuffer = new unsigned char[KB_BUFFER_SIZE + 1];
    m_pMsBuffer = new unsigned char[MS_BUFFER_SIZE + 1];
    m_pSerialBuffer = new char[SER_BUFFER_SIZE + 1];


    if (nIOPort > 0 && m_portsUsed.ReservePort(nIOPort)) {
        m_nIOPort = nIOPort;
    }
    else {
        m_nIOPort = -1;
    }
    m_bXpiDevice = false;
    m_bIsMulticast = false;
    m_nMulticastCount = 0;

    m_pHal = NULL;
    if (m_nIOPort > 0)
        m_pHal = hal_acquire(m_nIOPort - 1);

    m_hVideo = m_hAudio = m_hKeyboard = m_hMouse = -1;
    m_hUsbEventHead = NULL;
    if (m_pHal != NULL) {
        m_hVideo = hal_get_video_fd(m_pHal);
        m_hAudio = hal_get_audio_fd(m_pHal);
        m_hKeyboard = hal_get_keyboard_fd(m_pHal);
        m_hMouse = hal_get_mouse_fd(m_pHal);
        m_hUsbEventHead = hal_get_event_fd_list(m_pHal);
        m_bXpiDevice = 
            (hal_get_hardware_type(m_pHal) == XPI_DEVICE) ? true : false;
        if (m_bXpiDevice) {
           m_pSendBuffer = new char [SEND_BUFFER_SIZE];
           m_pKbPktBuffer = new unsigned char [KB_BUFFER_SIZE];
        }
        if (m_bXpiDevice)
            m_nCodecVersion = CODEC_VERSION_STANDARD;
        else
            m_nCodecVersion = hal_get_codec_version(m_pHal);
        m_nSplashCodec = m_nCodecVersion;
    }
    else {
        SetError();
        SendErrorNotification(NOTIFICATION_ERROR_IOPORT, "Could not acquire channel %d", m_nIOPort);
    }

    if (m_pHal != NULL &&
            hal_get_board_type(m_pHal) != BOARDTYPE_TX) {
        SetError();
        SendErrorNotification(NOTIFICATION_ERROR_IOPORT, "Not a TX board");
    }

    if (IsErrored()) {
        return;
    }

    m_pSrvr = NULL;
    if (m_pHal != NULL) {
        m_pSrvr = srvr_acquire(m_pHal);
    }

    m_pRFB = new CRFBHandler(this, m_pHal);
    if (m_pRFB != NULL)
        m_pRFB->InitXpiRFBStatus(m_bXpiDevice);

    if (m_bXpiDevice) {
		m_pUsbHid = new CUSBHID();
		m_pUsb = new CUSB(m_pUsbHid,m_pSrvr,NULL);
        m_pKbmIpc = new CKbmIpc();
    }

    InitSplashFrame();

    InitSerialParameters();

    InitKBMTypes();

    InitConsoleDevice();

    SetupConnectionProfile();

    CreateScap();

    SetConnected();
}

CFPGARx::~CFPGARx()
{

    if (m_pHal != NULL)
        hal_release(m_pHal);

    if (m_pSrvr != NULL)
        srvr_release(m_pSrvr);

    /* Release the FPGA port */
    if (m_nIOPort > 0)
        m_portsUsed.ReleasePort(m_nIOPort);

    delete m_pRFB;

    delete m_LUT;
    delete m_pSplash;
    delete m_pKbmIpc;
	if(m_pUsb)
		delete m_pUsb;

	if(m_pUsbHid)
		delete m_pUsbHid;

    delete[] m_pVideoBuffer;
    delete[] m_pAudioBuffer;
    delete[] m_pScapBuffer;
    delete[] m_pKbBuffer;
    delete[] m_pMsBuffer;
    delete[] m_pSerialBuffer;

    if (m_bXpiDevice) {
        delete[] m_pSendBuffer;
        delete[] m_pKbPktBuffer;
    }

}

void CFPGARx::InitVariablesToZero()
{
    m_nVideoPackets = m_nVideoBytes = 0;
    m_nAudioPackets = m_nAudioBytes = 0;
    m_nCtlInPackets = m_nCtlInBytes = 0;
    m_nCtlOutPackets = m_nCtlOutBytes = 0;
    m_nMsPackets = m_nMsBytes = m_nMsLostPackets = 0;
    m_nKbPackets = m_nKbBytes = m_nKbLostPackets = 0;
    m_nSerInPackets = m_nSerInBytes = m_nSerInLostPackets = 0;
    m_nSerOutPackets = m_nSerOutBytes = 0;
    m_nUsbKbInBytes = 0;
    m_nUsbKbInPackets = 0;
    m_nUsbMsInBytes = 0;
    m_nUsbMsInPackets = 0;
    m_nAudioSOFerrors = 0;

    m_hUsbEventHead = NULL;
    m_pRFB = NULL;
    m_pKbmIpc = NULL;
    m_pShaper = NULL;
	m_pUsbHid = NULL;
	m_pUsb = NULL;
    m_LUT = NULL;
    m_pSplash = NULL;
    m_nMslen = 0;
    m_tLastScapSent = 0;
    m_nFSRefreshCount = 0;
    m_nLastFrameCount = 0;
    m_nSliceDropCount = 0;
    m_nNoData = 0;
    m_nKBMIdletime = 0;
    m_nComputedBandwidth = 0;
    m_nRequestBandwidth = 0;
    m_nAudioType = 0;
    m_nAVOption = 0;
    m_nMinCompression = 0;
    m_nMaxCompression = 0;
    m_fBurstRatio = 0.0;
    m_nBurstDuration = 0;
    m_nControllingSessionId = 0;
    m_tLastKBMSendEvent = 0;
    m_nVideoSeqNumber = 0;
    m_nAudioSeqNumber = 0;
    m_nOffset = 0;
    m_bKBMCflag = 1;
}

void CFPGARx::KbmControlerRes(unsigned short hres, unsigned short vres)
{
    FILE *Xpifp;
    Xpifp = fopen(KBMCRES, "wb");
    if (Xpifp == NULL)
        return;

    fprintf(Xpifp, "%d %d ", hres, vres);
    m_bKBMCflag = 0;
    fclose(Xpifp);
    
    return;
}

void CFPGARx::SetupConnectionProfile()
{

    m_nComputedBandwidth = 131072;
    m_nRequestBandwidth = m_nComputedBandwidth;
    m_nAudioType = 0;
    m_nAVOption = AVOPTION_VIDEO;
    m_nMinCompression = 2;
    m_nMaxCompression = 6;
    m_fBurstRatio = 3.0;
    m_nBurstDuration = 100;
    m_nKBMIdletime = 5;
    m_bAllowKBMSwitch = true;

    struct tx_connection_config connection_config;
    get_tx_connection_config(m_pHal, &connection_config);
    if (connection_config.bandwidth >= 1024) {
        m_nComputedBandwidth = connection_config.bandwidth;
        m_nRequestBandwidth = m_nComputedBandwidth;
    }
    if (connection_config.burst_ratio > 0.1) {
        m_fBurstRatio = connection_config.burst_ratio;
    }
    if (connection_config.burst_duration > 0) {
        m_nBurstDuration = connection_config.burst_duration;
    }
    m_nMinCompression = connection_config.low_compression;
    m_nMaxCompression = connection_config.high_compression;
    m_nKBMIdletime = connection_config.kbm_idle_limit;
    m_bAllowKBMSwitch = connection_config.kbm_switch;
    m_nAVOption = connection_config.av_option;
    m_nAudioType = connection_config.audio_type;

    hal_set_addside_low_compression(m_pHal, m_nMinCompression);
    hal_set_addside_high_compression(m_pHal, m_nMaxCompression);

    SetBandwidth();
}

int CFPGARx::UpdateConnectionProfile(CCommand *a_pCmd)
{

    int retval = 0;

    bool bSetBandwidth = false;
    int nBw;
    if (a_pCmd->GetIntVar("bandwidth", &nBw) != -1) {
        nBw = nBw / 8;
        if (nBw > 0) {
            m_nRequestBandwidth = nBw;
            bSetBandwidth = true;
        }
        retval = 1;
    }

    double fBurstRatio;
    if (a_pCmd->GetFloatVar("burstratio", &fBurstRatio) != -1) {
        m_fBurstRatio = fBurstRatio;
        bSetBandwidth = true;
        retval = 1;
    }
    int nBurstDuration;
    if (a_pCmd->GetIntVar("burstduration", &nBurstDuration) != -1) {
        m_nBurstDuration = nBurstDuration;
        bSetBandwidth = true;
        retval = 1;
    }
    if (bSetBandwidth)
        SetBandwidth();

    int nKBMIdleTime;
    if (a_pCmd->GetIntVar("kbmidletime", &nKBMIdleTime) != -1) {
        m_nKBMIdletime = nKBMIdleTime;
        retval = 1;
    }

    int nAllowKBMSwitch;
    if (a_pCmd->GetIntVar("allowkbmswitch", &nAllowKBMSwitch) != -1) {
        m_bAllowKBMSwitch = (nAllowKBMSwitch != 0) ? true : false;
        retval = 1;
    }

    int nAVOption;
    if (a_pCmd->GetIntVar("avoption", &nAVOption) != -1) {
        m_nAVOption = nAVOption;
        retval = 1;
    }

    int nAudioType;
    if (a_pCmd->GetIntVar("audiotype", &nAudioType) != -1) {
        m_nAudioType = nAudioType;
        retval = 1;
    }
    return retval;
}

void CFPGARx::InitSerialParameters()
{
    struct serial_config serial_cfg;
    memset(&serial_cfg, 0x0, sizeof(serial_cfg));
    get_tx_serial_config(m_pHal, &serial_cfg);
    m_hSerial = -1;
    if (serial_cfg.serial_enable) {
        m_hSerial = hal_get_serial_fd(m_pHal);
        if (m_hSerial > 0) {
            set_serial_parameters(m_hSerial, serial_cfg.baudrate,
                    serial_cfg.databits, serial_cfg.stopbits,
                    serial_cfg.parity, serial_cfg.flowcontrol);
        }
    }
}

void CFPGARx::InitKBMTypes()
{
    struct tx_kbm_config kbm_cfg;
    get_tx_kbm_config(m_pHal, &kbm_cfg);
    m_nMouseType = kbm_cfg.mouse_type;
    m_nKeyboardType = kbm_cfg.keyboard_type;
}

void CFPGARx::InitScreenResolution()
{
    if ( (m_LUT == NULL) || (m_pUsb == NULL) ) 
    {
        SYSLOG_ERROR("-%u- %s : Fail to set the screen resolution.\n",__LINE__, __FUNCTION__);
        return;
    }

    m_pUsb->SetScreenResolution(m_LUT->a_orig_hres,m_LUT->a_orig_vres);


}
int CFPGARx::UpdateKBMTypes(CCommand *a_pCmd)
{
    int retval = 0;

    int nKBType;
    if (a_pCmd->GetIntVar("kbtype", &nKBType) != -1) {
        m_nKeyboardType = nKBType;
        retval = 1;
    }
    int nMSType;
    if (a_pCmd->GetIntVar("mstype", &nMSType) != -1) {
        m_nMouseType = nMSType;
        retval = 1;
    }

    return retval;
}

int CFPGARx::UpdateKeyboardEscapeSequences(CCommand *a_pCmd)
{
    int retval = 0;
    int nesc1, nesc2, nesc3, nesc4;
    bool bEscape = false;

    if (m_pSrvr == NULL)
        return 0;

    if (a_pCmd->GetIntVar("consoleesc1", &nesc1) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc1 = m_pSrvr->console_escape_keycode[0];
    }
    if (a_pCmd->GetIntVar("consoleesc2", &nesc2) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc2 = m_pSrvr->console_escape_keycode[1];
    }
    if (a_pCmd->GetIntVar("consoleesc3", &nesc3) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc3 = m_pSrvr->console_escape_keycode[2];
    }
    if (a_pCmd->GetIntVar("consoleesc4", &nesc4) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc4 = m_pSrvr->console_escape_keycode[3];
    }
    if (bEscape) {
        srvr_set_console_escape(m_pHal, m_pSrvr, nesc1, nesc2, nesc3, nesc4,
                m_nKeyboardType);
    }
    if (a_pCmd->GetIntVar("localesc1", &nesc1) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc1 = m_pSrvr->local_escape_keycode[0];
    }
    if (a_pCmd->GetIntVar("localesc2", &nesc2) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc2 = m_pSrvr->local_escape_keycode[1];
    }
    if (a_pCmd->GetIntVar("localesc3", &nesc3) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc3 = m_pSrvr->local_escape_keycode[2];
    }
    if (a_pCmd->GetIntVar("localesc4", &nesc4) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc4 = m_pSrvr->local_escape_keycode[3];
    }
    if (bEscape) {
        srvr_set_local_escape(m_pHal, m_pSrvr, nesc1, nesc2, nesc3, nesc4,
                m_nKeyboardType);
    }
    return retval;
}

void CFPGARx::InitConsoleDevice()
{
    if (hal_get_console_fd(m_pHal) > 0)
        hal_initialize_console(m_pHal);
}

void CFPGARx::InitSplashFrame()
{

    m_pSplash = new CSplash(m_pHal, true);

    m_LUT = new struct LUT_struct;
    m_pSplash->GetLUT(m_LUT, m_nMQuant,
            m_nDCQuant, m_nSliceSize, m_nCompMode, m_nMotionCompMode);
}

void CFPGARx::DeInitSplashFrame()
{
    delete m_pSplash;
    delete m_LUT;
}

int CFPGARx::UpdateSplash(CCommand *a_pCmd)
{
    int retval = 0;
    int nUpdateSplash;

    if (a_pCmd->GetIntVar("updatesplash", &nUpdateSplash) != -1) {
        if (nUpdateSplash) {
            if (m_bXpiDevice || m_pSplash->IsReady()) {
                delete m_pSplash;
                m_pSplash = new CSplash(m_pHal, true);
                if (m_bXpiDevice) {
                    m_bDisplaySplash = false;
                    hal_disable_splash_video(m_pHal);
                }
                CreateScap();
                SendScap();
                SetBandwidth();
            }
            else {
                SYSLOG_INFO("V2DFPGA: Ignoring splash update. "
                        "Older splash object still busy.");
            }
            retval = 1;
        }
    }
    return retval;
}

int CFPGARx::GetSerialData()
{
    char *pData;
    int nLen = 0;
    char *m_pSerBuf = new char[SER_BUFFER_SIZE + 1];

    if (m_pSerBuf == NULL)
        return 0;

    ASSERT(m_hSerial > 0);
    nLen = read(m_hSerial, m_pSerBuf, SER_BUFFER_SIZE);
    nLen = v2dLibBuildSerialData(m_pSerialBuffer, m_pSerBuf, nLen);
    pData = m_pSerialBuffer;

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

    CSFrame * sFrame = new CSFrame(pData, nLen, v2dSMediaFlag.GetMediaFlag(),
            m_nTimestamp);

    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);

    m_nSerInBytes += nLen;
    m_nSerInPackets++;

    UpdateCtlPacketStats(nLen);

    delete[] m_pSerBuf;
    return 0;
}

int CFPGARx::ProcessUsbEventData(struct event_s *eventp)
{
    int nDataType = 0;
    int nDataLen;
    unsigned char pDataBuf[KB_BUFFER_SIZE + 1];
    
    ASSERT(eventp != NULL && eventp->event_fd > 0);

    nDataType = process_server_event_data(m_pHal, m_pSrvr, eventp, (unsigned char *) pDataBuf,  m_nKeyboardType, m_nMouseType, &nDataLen);

    if (m_bXpiDevice && (nDataLen > 0) ) {

       if (nDataType == KBM_TYPE_MOUSE) {
           ProcessRelMouse(pDataBuf, m_pSendBuffer, nDataLen);
           m_nUsbMsInBytes += nDataLen;
           m_nUsbMsInPackets++;

       } else if (nDataType == KBM_TYPE_KEYBOARD) {
           //CreateRFBKbData(m_pSrvr->kb_buf, (unsigned char *)m_pSendBuffer, nDataLen);
           CreateRawKbData(m_pSrvr->kb_buf, (unsigned char *)m_pSendBuffer, nDataLen);
           m_nUsbKbInBytes += nDataLen;
           m_nUsbKbInPackets++;
       }
     }
    return 0;
}

int CFPGARx::ProcessRelMouse(unsigned char *pInMsBuf, 
                             char *pOutMsBuf,
                             int msLen)
{
    int indx = 0;
    int MousePktLen = 8;
    int cnt = 0;
    int nLen = 0;
    int rc = 1;
    int mstype;

    if (pInMsBuf == NULL || msLen <= 0 || m_pUsb == NULL)
        return 0;   

    if (pInMsBuf[0] == 3 && msLen == 8) {
        mstype = 3; /* received IMPS2 format data */
    }
    else if (pInMsBuf[0] == 0 && msLen == 6) {
        /* HID USB Mouse requires 4 bytes , if receive 
          6 bytes of data it means it is in PS2 format without 
          wheel for this reason just appending last 2 bytes with 0
          where wheel coridnate is 0 always for PS2 without wheel*/
        pInMsBuf[6] = 0x00;
        pInMsBuf[7] = 0x00;
        mstype = 0;  /* received PS2 format data */
    }
    else
        return 0;


    for (indx = 0; indx < msLen; indx += MousePktLen)
    {
        pOutMsBuf[0] = pInMsBuf[cnt * MousePktLen + 1];   /* button press */
        pOutMsBuf[1] = pInMsBuf[cnt * MousePktLen + 3];   /* X Rel cordinate */
        pOutMsBuf[2] = pInMsBuf[cnt * MousePktLen + 5];   /* Y Rel cordinate */
       
        if (mstype == 0) {
            pOutMsBuf[1] = (pOutMsBuf[0] & 0x10) ? 
                          pOutMsBuf[1] - 256 : pOutMsBuf[1];
            pOutMsBuf[2] = (pOutMsBuf[0] & 0x20) ?
                          -(pOutMsBuf[2] - 256) : -pOutMsBuf[2];
            pOutMsBuf[3] = 0;   /* Wheel Movement */
        } else {
            pOutMsBuf[2] = 0 - pOutMsBuf[2];  /* flip Y & wheel because in read mouse event Y & wheel are fliped */
            pOutMsBuf[3] = 0 - pInMsBuf[cnt * MousePktLen + 7];   /* Wheel Movement */
        }
        cnt += 1;
        
        pOutMsBuf[0] = (pOutMsBuf[0]) & 0x07;   /*LSB 3 bits are considered for buttonpress*/

		nLen = m_pUsb->ProcessRelMouseData(pOutMsBuf);

        if (nLen != RELMOUSE_PKT_LEN)
            return 0;
         
        /* clear buffers for next set of iteration */
        pOutMsBuf[0] = pOutMsBuf[1] = pOutMsBuf[2] = pOutMsBuf[3] = 0;

    }
        return rc;
}

int CFPGARx::CreateRFBKbData(unsigned char *pInKbBuf,
             unsigned char *pRFBOutBuf,
             int kbLen)
{
    int rfblen;
    unsigned char rfbbuf[KB_BUFFER_SIZE * 6 / 4];
    int rc = 0;

    if (m_pSrvr == NULL || m_pUsb == NULL)
        return 0;

    rc = CheckForBreakCode(pInKbBuf, &kbLen);
    
    if (rc == 0)
        return 0;

    rfblen = srv_keyboard_data_to_rfb(m_pSrvr, pInKbBuf, rfbbuf, kbLen);

    if (rfblen <= 0)
       return 0;

    for (int ind = 0 ; ind < rfblen; ind += 5) {
        unsigned int key = (rfbbuf[ind + 3] | 
                           ((rfbbuf[ind + 2] << 8) & 0xff00) | 
                           ((rfbbuf[ind + 1] << 16) & 0xff0000) | 
                           ((rfbbuf[ ind ] << 24) & 0xf000000));

         unsigned char down = rfbbuf[ind + 4];
		 m_pUsb->ProcessRfbKeyboardData(key,down);
         memset(m_pKbPktBuffer,0, m_nOffset + kbLen);
         m_nOffset = 0;
    }

    return KEYBOARD_PKT_LEN;
}

int CFPGARx::CreateRawKbData(unsigned char *pInKbBuf,
             unsigned char *pRFBOutBuf,
             int kbLen)
{
    //SYSLOG_INFO("-%u- %s : CreateRawKbData \n",__LINE__, __FUNCTION__);
    int rfblen;
    unsigned char rfbbuf[KB_BUFFER_SIZE * 6 / 4];
    int rc = 0;
    unsigned int scancode=0;
    unsigned char keydown=0;

    if (m_pSrvr == NULL || m_pUsb == NULL)
        return 0;

    rc = CheckForBreakCode(pInKbBuf, &kbLen);

    if (rc == 0)
        return 0;

    rfblen = get_raw_keyboard_data(m_pSrvr, pInKbBuf, rfbbuf, kbLen,&scancode,&keydown);

     if (rfblen <= 0)
       return 0;

    m_pUsb->ProcessRawKeyboardData(scancode,keydown);
    memset(m_pKbPktBuffer,0, m_nOffset + kbLen);
    m_nOffset = 0;

    return KEYBOARD_PKT_LEN;
}

bool CFPGARx::CheckForBreakCode(unsigned char *buf,int *buflen)
{
    int rc = 0;
    int i;
    int cnt = 0;
    
    if (*buflen < 2) return rc;
    
    for (i = 0; i < *buflen; i += 2) {
        if (i > KB_BUFFER_SIZE) i -= KB_BUFFER_SIZE;
        if ((buf[i + 1] == 0xe0 || buf[i + 1] == 0xe1 || buf[i + 1] == 0xf0)) {
            rc = 0;
            cnt++;
        } else
            rc = 1; /* received full make or break code pkt*/
    }
    
    if (cnt == 1 && !rc) {
        memcpy(m_pKbPktBuffer + m_nOffset, buf, *buflen * sizeof(char));
        m_nOffset += *buflen;
    } else if (cnt == 0 && *buflen == 2 && m_nOffset > 0) {
        memcpy(buf + m_nOffset, buf, *buflen * sizeof(char));
        memcpy(buf, m_pKbPktBuffer, m_nOffset * sizeof(char));
        *buflen = m_nOffset + *buflen;
    }
    
    return rc;


}

void CFPGARx::ProcessVNCKeyboardEventData() 
{
    unsigned int key;
    unsigned int down;
    int nBytes;

	if (m_pKbmIpc == NULL || m_pUsb == NULL)
        return ;

    nBytes =  m_pKbmIpc->GetKbData(&key, &down);
    if (nBytes != KEYBOARD_PKT_LEN) {
        return;
    }
    
    if (m_pRFB->IsRFBEnabled())
        m_pRFB->ProcessVNCKeyboard(key, down);
    else
		m_pUsb->ProcessRfbKeyboardData(key,down);
  

    return;

}

void CFPGARx:: ProcessVNCMouseEventData()
{
   int nBytes;
   int buttonMask;
   int VncAbsX;
   int VncAbsY;
   int pktlen = 20;

   if (m_pKbmIpc == NULL || m_pUsb == NULL)
       return;

    nBytes =  m_pKbmIpc->GetMsData(&buttonMask, &VncAbsX, &VncAbsY);
    if (nBytes != pktlen) {
        return;
    }

    GetAndSetWidthHeight();


    if (m_pRFB->IsRFBEnabled())
        m_pRFB->ProcessVNCMouse(buttonMask,VncAbsX,VncAbsY);
    else
		m_pUsb->ProcessAbsMouseData(buttonMask, VncAbsX, VncAbsY);
   
    
    return;

}
void CFPGARx::GetAndSetWidthHeight()
{
    int width;
    int height;

	if (m_pKbmIpc == NULL || m_pUsb == NULL)
        return ;

    width = m_pKbmIpc->GetScreenWidth();
    height = m_pKbmIpc->GetScreenHeight();
   
	m_pUsb->SetScreenResolution(width,height);

}
int CFPGARx::WriteSplashPacket()
{

    char *pData;
    int nLen = 0;


    // First 16 bytes is for seq number
    int len = m_pSplash->GetSplashPacket(m_pVideoBuffer + 16);

    if (len == 0) {
        if (m_pShaper != NULL)
            m_pShaper->RevertTokens(SIZE_OF_AV_PACKET);
        return 0;
    }

    pData = m_pVideoBuffer;
    nLen = len + 16;

    unsigned long * nSeqNumber = (unsigned long *) pData;
    *nSeqNumber = m_nAudioSeqNumber;
    nSeqNumber = (unsigned long *) (pData + 8);
    *nSeqNumber = m_nVideoSeqNumber++;

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_VIDEO);

    CSFrame * sFrame = new CSFrame(pData, nLen, v2dSMediaFlag.GetMediaFlag(),
            m_nTimestamp);

    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);
    m_frameCount++;
    m_bytesTransfered += nLen;
    m_nVideoPackets++;
    m_nVideoBytes += nLen;


    return nLen;
}

int CFPGARx::GetVideoFrame()
{
    char *pData;
    int nLen;

    if (m_nAVOption != AVOPTION_AUDIO) {
        if (m_pShaper && !(m_pShaper->CheckTokens(SIZE_OF_AV_PACKET))) {
            return -1;
        }
    }

    if (m_hVideo <= 0) {
        /* Driver not even present: do splash only */
        if (WriteSplashPacket() == 0)
            return -1;
        return 0;
    }

    ASSERT(m_hVideo > 0);

    nLen = read(m_hVideo, m_pVideoBuffer, AV_BUFFER_SIZE);

    if (nLen == 0) {

        SYSLOG_DEBUG("V2DFPGA: Video Changed Signal");

        CreateScap();
        SendScap();

        return 0;
    }

    bool bRealVideo = hal_real_video_available(m_pHal);
    if (bRealVideo != m_bRealVideo) {
        SYSLOG_DEBUG("V2DFPGA: Real Video status changed to %d", bRealVideo);
    }
    m_bRealVideo = bRealVideo;

    if ((!m_bRealVideo) && (!m_bDisplaySplash)) {
        SYSLOG_DEBUG("V2DFPGA: No real video detected And NOT Displaying splash");
        CreateScap();
        SendScap();
        return 0;
    }
    else if ((m_bRealVideo) && (m_bDisplaySplash)) {
        SYSLOG_DEBUG("V2DFPGA: Real video detected And Displaying splash");
        CreateScap();
        SendScap();
        return 0;
    }

    if (m_bDisplaySplash == true) {
        if (WriteSplashPacket() == 0)
            return -1;
        return 0;
    }

    /* Set the slice-drop flags (FS refresh) ioctl if need be */
    if (m_nSliceDropCount > 0)
        SetSliceDrop();
    
    // PSCW - Reading from video descriptor always return -1 on the dongle
    if (nLen < 0)
        return 0;

    if (m_nAVOption != AVOPTION_AUDIO) {
        pData = m_pVideoBuffer;

        unsigned long * nSeqNumber = (unsigned long *) pData;
        *nSeqNumber = m_nAudioSeqNumber;
        nSeqNumber = (unsigned long *) (pData + 8);
        *nSeqNumber = m_nVideoSeqNumber++;

        CV2DStreamMediaFlag v2dSMediaFlag;
        v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_VIDEO);

        CSFrame * sFrame = new CSFrame(pData, nLen,
                v2dSMediaFlag.GetMediaFlag(), m_nTimestamp);

        ASSERT(m_qQueueSink != NULL);
        m_qQueueSink->InsertFrame(sFrame);
        m_frameCount++;
        m_bytesTransfered += nLen;
        m_nVideoPackets++;
        m_nVideoBytes += nLen;
    }
    return 0;
}

int CFPGARx::GetAudioFrame()
{
    int nLen = 0;

    // Do not collect audio if only video requested
    if (m_nAVOption == AVOPTION_VIDEO) {
        return -1;
    }

    ASSERT(m_hAudio > 0);
    ASSERT(m_pShaper != NULL);

    nLen = read(m_hAudio, m_pAudioBuffer, AV_BUFFER_SIZE);
    if (nLen <= 0)
        return nLen;
    // Reset the audio and discard the data if it's not SOF alighed
    unsigned char sof[] = {0x00, 0x00, 0x01, 0xb3, 0x00, 0x00, 0x00, 0x00};
    if (memcmp(&m_pAudioBuffer[16], sof, 8) != 0) {
        m_nAudioSOFerrors++;
        hal_resetAddsideAudio(m_pHal);
        return 0;
    }

    // Legacy audio compression modes not supported.
    // Use m_nAudiotype here to support future audio compression modes

    // The driver already leaves 16 bytes of header room for sequence numbers
    unsigned long * nSeqNumber = (unsigned long *) m_pAudioBuffer;
    *nSeqNumber = m_nVideoSeqNumber;
    nSeqNumber = (unsigned long *) (m_pAudioBuffer + 8);
    *nSeqNumber = m_nAudioSeqNumber++;

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_AUDIO);
    CSFrame * sFrame = new CSFrame(m_pAudioBuffer, nLen, v2dSMediaFlag.GetMediaFlag(),
            m_nTimestamp);

    // We always let Audio packets through, so account for its bandwidth
    if (m_pShaper)
        m_pShaper->AllowTokens(nLen);

    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);
    m_frameCount++;
    m_bytesTransfered += nLen;
    m_nAudioPackets++;
    m_nAudioBytes += nLen;
    return nLen;
}

int CFPGARx::SetBandwidth()
{
    if (m_bDisplaySplash == true) {
        if ((unsigned long)m_pSplash->GetSplashBandwidth() <= m_nRequestBandwidth)
            m_nComputedBandwidth = m_pSplash->GetSplashBandwidth();
        else
            m_nComputedBandwidth = m_nRequestBandwidth;
    }
    else {
        m_nComputedBandwidth = m_nRequestBandwidth;
    }
    
    SYSLOG_INFO("V2DFPGA: ApplyingBandwidth %ld\n", m_nComputedBandwidth);

    if (m_bXpiDevice) {
        hal_set_addside_bandwidth(m_pHal, m_nComputedBandwidth,
                                  m_fBurstRatio, m_nBurstDuration);
    }
    else {
        if (m_pShaper != NULL)
            m_pShaper->SetBandwidth(m_nComputedBandwidth * 8, m_fBurstRatio,
                                    m_nBurstDuration);
        else
            m_pShaper = new CShaper(m_nRequestBandwidth * 8, m_fBurstRatio,
                                    m_nBurstDuration);
    }

    return 0;
}

void CFPGARx::UpdateCodecVersion()
{
    int codecversion = hal_get_codec_version(m_pHal);

    if (m_bXpiDevice) {
        // codec versions not supported on XPi yet
        return;
    }

    if ((codecversion != CODEC_VERSION_UNKNOWN) ) {
        m_nCodecVersion = FIX_CODEC_VERSION(codecversion);
        if (m_nSplashCodec != m_nCodecVersion)  {
            SYSLOG_INFO("%s codec version change detected: %d != %d  \n", 
                __func__,m_nCodecVersion,m_nSplashCodec);
            m_nSplashCodec = m_nCodecVersion;
            DeInitSplashFrame();
            InitSplashFrame();
        }
    }
}


int CFPGARx::CreateScap()
{
    int retval = 0;
    struct LUT_struct lut;
    int pl, nLen = 0;
    unsigned char *cp = NULL;
    int nCompMode = 0;
    int nMotionCompMode = 0;
    int nStereo = 0;

    SYSLOG_DEBUG("V2DFPGA: Creating Scap");
    UpdateCodecVersion();
    if (m_bXpiDevice) {
        // Need to do this here for XPi since we don't run GetVideoFrame() 
        bool bRealVideo = hal_real_video_available(m_pHal);
        if (bRealVideo != m_bRealVideo) {
            SYSLOG_DEBUG("V2DFPGA: Real Video status changed to %d", 
                    bRealVideo);
            m_bRealVideo = bRealVideo;
        }
    }
    if (m_hVideo <= 0) {
        // Driver not loaded, fill default values
        SYSLOG_DEBUG("V2DFPGA: Driver not loaded");
        memcpy((void *) &lut, (void *) m_LUT, sizeof(struct LUT_struct));
    }

    if (hal_get_addside_lut(m_pHal, &lut) < 0) {
        // This happens when there is no lut, ie. no video on Tx, 
        // cable disconnected
        SYSLOG_DEBUG("V2DFPGA: No LUT Detected");
        memset(&lut, 0x00, sizeof(lut));
        m_pSplash->GetLUT(&lut, m_nMQuant,
                m_nDCQuant, m_nSliceSize, nCompMode, nMotionCompMode);
        nStereo = lut.a_stereo;
        // In case it changed state set splash bandwidth and, if XPi,
        // enable splash on the fanout FPGA
        if (m_bDisplaySplash == false) {
            SYSLOG_DEBUG("V2DFPGA: Displaying Splash false --> true");
            m_nSliceDropCount = 4; // Force a full screen refresh now
            m_pSplash->SetFSRefresh();
            m_bDisplaySplash = true;
            SetBandwidth();
            if (m_bXpiDevice) {
                int nSplashType = m_pSplash->GetSplashType() & ~SPLASH_TYPE_TX;
                hal_enable_splash_video(m_pHal, nSplashType, &lut);
                m_nSliceDropCount = 4; 
                SetSliceDrop();
            }
        }
        m_bDisplaySplash = true;
    }
    else if (m_bRealVideo) {
        //In case it changed state set video bandwidth and, if XPi, 
        //disable splash in the fanout FPGA
        if (m_bDisplaySplash == true) {
            SYSLOG_DEBUG("V2DFPGA: Displaying Splash true --> false");
            m_bDisplaySplash = false;
            SetBandwidth();
            if (m_bXpiDevice) {
                hal_disable_splash_video(m_pHal);
            }
        }
        m_bDisplaySplash = false;

        if (m_hVideo > 0) {
            hal_get_addside_mquant(m_pHal, &m_nMQuant);
            hal_get_addside_dcquant(m_pHal, &m_nDCQuant);
            hal_get_addside_nobpslice(m_pHal, &m_nSliceSize);
            hal_get_addside_compmode(m_pHal, &nCompMode, &nMotionCompMode);
            nStereo = hal_getStereo(m_pHal);
        }

    }

    // Happens when force splash screen in menu is selected
    if (!m_bRealVideo) {

        SYSLOG_DEBUG("V2DFPGA: Forcing splash screen??");
        memset(&lut, 0x00, sizeof(lut));
        m_pSplash->GetLUT(&lut, m_nMQuant,
                m_nDCQuant, m_nSliceSize, nCompMode, nMotionCompMode);
        //In case it changed state set to splash bandwidth
        if (m_bDisplaySplash == false) {
            SYSLOG_DEBUG("V2DFPGA: Displaying Splash false --> true");
            m_nSliceDropCount = 4; // Force a full screen refresh now
            m_pSplash->SetFSRefresh();
            m_bDisplaySplash = true;
            SetBandwidth();
            if (m_bXpiDevice) {
                int nSplashType = m_pSplash->GetSplashType() & ~SPLASH_TYPE_TX;
                hal_enable_splash_video(m_pHal, nSplashType, &lut);
            }
        }
        m_bDisplaySplash = true;
    }

    if (memcmp((void *) &lut, (void *) m_LUT, sizeof(struct LUT_struct)) == 0) {
        m_nVideoChanged = 0;
    }
    else {
        m_nVideoChanged = 1;
        memcpy((void *) m_LUT, (void *) &lut, sizeof(struct LUT_struct));
    }

    // Changes in CompMode or MotionCompMode also mean a resolution change
    if (m_nVideoChanged == 0 && nCompMode != m_nCompMode) {
        SYSLOG_DEBUG("Detected Change in CompMode %d -> %d",
                m_nCompMode, nCompMode);
        m_nVideoChanged = 1;
    }
    m_nCompMode = nCompMode;
    if (m_nVideoChanged == 0 && nMotionCompMode != m_nMotionCompMode) {
        SYSLOG_DEBUG("Detected Change in MotionCompMode %d -> %d",
                m_nMotionCompMode, nMotionCompMode);
        m_nVideoChanged = 1;
    }
    m_nMotionCompMode = nMotionCompMode;

    cp = (unsigned char *) m_pScapBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_CAP);
    nLen += 4;
    cp += 4;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CHANGED, m_nVideoChanged, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_ID, lut.a_alutreg, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_PORT, 6060, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_AUDIO_PORT, 6061, cp);
    nLen += pl;
    cp += pl;

    pl = strlen(lut.comment) + 1 + 4;
    FILL_PROTOCOL_UNIT_STRING(strlen(lut.comment) + 1, SCAP_IDENT_VIDEO_NAME,
            lut.comment, cp);
    nLen += pl;
    cp += pl;

    pl = strlen("226.1.1.1") + 1 + 4;
    FILL_PROTOCOL_UNIT_STRING(strlen((char *) "226.1.1.1") + 1,
            SCAP_IDENT_MCAST_ADDRESS, (char *) "226.1.1.1", cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_KB_TYPE, m_nKeyboardType, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_MS_TYPE, m_nMouseType, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HRES, lut.a_hres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VRES, lut.a_vres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_ORIG_HRES, lut.a_orig_hres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_ORIG_VRES, lut.a_orig_vres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_PCR,
            (unsigned long)(lut.a_pcr * 1000.0), cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_REFRESH,
            (unsigned long)(lut.a_refresh * 1000.0), cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HFP, lut.a_hfp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HBP, lut.a_hbp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HSYNC, lut.a_hsync, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VFP, lut.a_vfp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VBP, lut.a_vbp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VSYNC, lut.a_vsync, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_MQUANT, m_nMQuant, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_DCQUANT, m_nDCQuant, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_SLICESIZE, m_nSliceSize, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_COMPMODE, m_nCompMode, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_MOTIONCOMPMODE, m_nMotionCompMode, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CODEC_VERSION, m_nCodecVersion, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_STEREO, nStereo, cp);
    nLen += pl;
    cp += pl;
   
    if (m_bXpiDevice) {
        pl = 8;
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_AUDIO_FREQ, NGS_AUDIO_FREQ, cp);
        nLen += pl;
        cp += pl;
    }

    /****************************************************************
     * Fill Audio parameters (need this to play back recorded data) *
     ****************************************************************/
    //pl = FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_AUDIO_TYPE,
    //audiotype_from_clientflags(client->flags), cp);
    //nLen += pl; cp += pl;

    // Fill the total length
    m_nScapLength = nLen;
    cp = (unsigned char *) m_pScapBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, nLen);

    /* Indicating Resolution change to KBM Controler */
    if (m_bXpiDevice && (m_nVideoChanged || m_bKBMCflag)) {

        /* this is to normalise the width and height of 
         * screen before writing in to hid gaget*/
        InitScreenResolution();
        KbmControlerRes(lut.a_orig_hres, lut.a_orig_vres);
        
    }
 
    // Reset video chagned flag
    m_nVideoChanged = 0;

    return retval;
}

void CFPGARx::UpdateCtlPacketStats(int nLen)
{
    m_frameCount++;
    m_bytesTransfered += nLen;
    m_nCtlOutPackets++;
    m_nCtlOutBytes += nLen;
}

int CFPGARx::SendScap()
{
    int nLen;
    char *pData;

    nLen = m_nScapLength;
    pData = m_pScapBuffer;

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    CSFrame * sFrame = new CSFrame(pData, nLen, v2dSMediaFlag.GetMediaFlag(),
            m_nTimestamp);
    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);

    UpdateCtlPacketStats(nLen);

    return 0;
}

int CFPGARx::SetEncParams(char *pData)
{
    int retval = 0;
    AVReqData_t avReqData;
    memset((void *) &avReqData, 0, sizeof(avReqData));

    if (v2dLibParseAVReq(&avReqData, pData) < 0)
        return retval;

    // Set Min Compression
    if (avReqData.m_minCompSet) {
        m_nMinCompression = avReqData.m_minComp;
        hal_set_addside_low_compression(m_pHal, m_nMinCompression);
    }

    // Set Max Compression
    if (avReqData.m_maxCompSet) {
        m_nMaxCompression = avReqData.m_maxComp;
        hal_set_addside_high_compression(m_pHal, m_nMaxCompression);
    }

    // Set bandwidth parameters
    if (avReqData.m_burstRatioSet && avReqData.m_burstDurationSet
            && avReqData.m_videoRateSet) {

        // Set Burst Ratio
        m_fBurstRatio = avReqData.m_burstRatio;

        // Set Burst Duration
        m_nBurstDuration = avReqData.m_burstDuration;

        // Set bandwidth
        m_nRequestBandwidth = avReqData.m_videoRate;

        SYSLOG_INFO("V2DFPGA: RequestedBandwidth %ld\n",
                m_nRequestBandwidth);

        // Set Audio options
        if (avReqData.m_avoptionSet) {
            m_nAVOption = avReqData.m_avoption;
            if (avReqData.m_audioTypeSet) {
                m_nAudioType = avReqData.m_audioType;
            }
            else {
                m_nAudioType = 0;
            }
        }
        else {
            /* AVOption not set. Assume video only */
            m_nAVOption = AVOPTION_VIDEO;
            m_nAudioType = 0;
        }


        SetBandwidth();

    }

    /* FS Refresh ioctl */
    if (avReqData.m_slicedropSet) {
        // Schedule an scap to be sent right away
        m_tLastScapSent = 0;

        m_nFSRefreshCount++;

        /* For I50, set the slice drop flag for four more frames */
        m_nSliceDropCount =
            (hal_get_hardware_type(m_pHal) == V2DI50_DEVICE) ? 4 : 1;
        m_pSplash->SetFSRefresh();
        SetSliceDrop();
    }

    return retval;
}

int CFPGARx::ProcessNetHeader(char *pData)
{
    int retval = 0;
    NetHdrData_t hdr;
    struct in_addr dst_addr;

    memset((void *) &hdr, 0, sizeof(NetHdrData));

    if (m_bXpiDevice == false || (m_hVideo <= 0 && m_hAudio <= 0)) 
        return 0;

    memset((void *) &hdr, 0, sizeof(NetHdrData));

    if (v2dLibParseNetHeader(&hdr, pData) < 0)
        return retval;

    if (CheckForMulticast(hdr))
        return 0;

    if (hdr.video_set) {
        dst_addr.s_addr = hdr.dst_video_addr;
        SYSLOG_INFO("Setting up video network headers: sport %d, ttl %d, dstip %s, dport %d, mac %04x%04x%04x", 
                    hdr.src_video_port, 
                    hdr.ttl, 
                    inet_ntoa(dst_addr), 
                    hdr.dst_video_port,
                    hdr.dst_mac[2], hdr.dst_mac[1], hdr.dst_mac[0]);
        if (hal_set_nethdr(m_pHal, NETHDR_VIDEO_OUT_CONTEXT, 
                    hdr.src_video_port, 
                    hdr.ttl, hdr.dst_video_addr, hdr.dst_mac, 
                    hdr.dst_video_port) < 0) 
            SYSLOG_ERROR("V2DFPGA: Failed to start streaming video\n");
    }

    if (hdr.audio_set) {
        dst_addr.s_addr = hdr.dst_audio_addr;
        SYSLOG_INFO("Setting up audio network headers: sport %d, ttl %d, dstip %s, dport %d, mac %04x%04x%04x", 
                    hdr.src_audio_port, 
                    hdr.ttl, 
                    inet_ntoa(dst_addr), 
                    hdr.dst_audio_port,
                    hdr.dst_mac[2], hdr.dst_mac[1], hdr.dst_mac[0]);
        if (hal_set_nethdr(m_pHal, NETHDR_AUDIO_OUT_CONTEXT, 
                    hdr.src_audio_port, 
                    hdr.ttl, hdr.dst_audio_addr, hdr.dst_mac, 
                    hdr.dst_audio_port) < 0)
            SYSLOG_ERROR("V2DFPGA: Failed to start streaming audio\n");
    }   
    return retval;
}

bool CFPGARx::CheckForMulticast(NetHdrData_t & hdr)
{
    unsigned long dstaddr = 0;
    if (hdr.video_set) {
        dstaddr = ntohl(hdr.dst_video_addr);
    }
    else if (hdr.audio_set) {
        dstaddr = ntohl(hdr.dst_audio_addr);
    }
    if (dstaddr > 0xE0000000 && dstaddr <= 0xEFFFFFFF)
        m_bIsMulticast = true;

    if (!m_bIsMulticast)
        return false;

    m_nMulticastCount++;
    if (m_nMulticastCount > 1) {
        SYSLOG_INFO("Already serving multicast: %d",  m_nMulticastCount);
        return true;
    }
    return false;
}

int CFPGARx::SetSliceDrop()
{

    struct fc_param_io fcpio;

    if (m_hVideo <= 0)
        return 0;

    if (hal_get_fc_params(m_pHal, &fcpio) < 0)
        return 0;

    ASSERT(m_nSliceDropCount > 0);
    if (m_nSliceDropCount <= 0)
        return 0;

    if (m_nLastFrameCount == 0 || fcpio.frame_cnt > m_nLastFrameCount) {
        SYSLOG_INFO(
            "V2DFPGA: Setting slicedrop (Full Screen Request to FPGA) [%d]\n",
            m_nSliceDropCount);
        /* Force 2 Iframes on XP (I50 always sends 1 IFrame on each call) */
        if (hal_set_addside_noslicedrop(m_pHal, 2) < 0) {
            SYSLOG_ERROR("V2DFPGA: Could not set slicedrop\n");
            m_nSliceDropCount = 0;
            return 0;
        }
        m_nSliceDropCount--;
        if (m_nSliceDropCount > 0)
            m_nLastFrameCount = fcpio.frame_cnt;
        else
            m_nLastFrameCount = 0;
    }

    return 0;
}

/* Check if it OK to allow KBM events to this client: Either
 * the sessionID must be the same or the idle time is passed */
bool CFPGARx::CheckKBMEvent(unsigned long a_sessionId)
{

    /* m_bAllowKBMSwitch is not yet implemented since we need a way to
     reset m_nControllingSessionId to zero when all clients are
     disconnected */
#ifdef TOBEIMPLEMENTED
    if (!m_bAllowKBMSwitch) {
        if (m_nControllingSessionId == 0 ||
                m_nControllingSessionId == a_sessionId) {
            allow = true;
        }
    }
#endif

    if (a_sessionId == m_nControllingSessionId)
        return true;

    if ((m_nTimestamp - m_tLastKBMSendEvent) > (unsigned long) (m_nKBMIdletime
            * 1000))
        return true;

    return false;
}

void CFPGARx::UpdateKBMEvent(unsigned long a_sessionId)
{
    m_nControllingSessionId = a_sessionId;
    m_tLastKBMSendEvent = m_nTimestamp;
}

int CFPGARx::PopulateMsBuffer(char *pPayload)
{
    unsigned char *cp;
    int i = 4;
    int code;
    int len, plen;
    int protid;
    unsigned char buff[MS_BUFFER_SIZE];
    unsigned long sessionId = 0;
    int mlen = 0;

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_MSDAT)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            SYSLOG_ERROR("V2DFPGA: Malformed Raw Mouse");
            return -1;
        }

        switch (code) {
        case MSDAT_IDENT_BASIC:
            if ((plen - 4) < MS_PS2_PKTSIZE) /* discard partial packet */
                break;
            mlen = plen - 4;
            memcpy(buff, cp + 4, mlen); // collect mouse data into local buff
            break;

        case MSDAT_IDENT_SESSIONID:
            sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;

        }
        cp = &cp[plen];
        i += plen;
    }

    if (mlen > 0 && CheckKBMEvent(sessionId)) {
        UpdateKBMEvent(sessionId);
        memcpy(m_pMsBuffer + m_nMslen, buff, mlen);
        m_nMslen += mlen;
    }
    else {
        m_nMsLostPackets++;
    }

    return 0;
}

int CFPGARx::PopulateKbBuffer(char *pPayload)
{
    unsigned char *cp;
    int i = 4;
    int code;
    int len, plen;
    int protid;
    unsigned long sessionId = 0;
    unsigned char buff[KB_BUFFER_SIZE];
    int mlen = 0;

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_KBDAT)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            SYSLOG_ERROR("V2DFPGA: Malformed Raw Keyboard");
            return -1;
        }

        switch (code) {
        case KBDAT_IDENT_MEDIUMRAW:
            mlen = plen - 4;
            memcpy(buff, cp + 4, mlen); // collect kayboard data into local buff
            break;

        case KBDAT_IDENT_SESSIONID:
            sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen)
            ;
            break;

        }
        cp = &cp[plen];
        i += plen;
    }

    /* Send the raw mouse event if its from the current client or the idle
     time is passed */
    if (mlen > 0 && CheckKBMEvent(sessionId)) {
        int rc = 1;
         if (m_bXpiDevice)
             CreateRFBKbData(buff, (unsigned char *)m_pSendBuffer, mlen);
         else 
             rc = write_to_kb_fpga(m_pHal, buff, mlen);
        if (rc < 0) {
            m_nKbLostPackets++;
        }
    }
    else {
        m_nKbLostPackets++;
    }

    return 0;
}

int CFPGARx::PopulateSerialBuffer(char *pData)
{
    int retval = 0;
    int nLen = 0;
    nLen = ntohs(*(unsigned short *) pData);

    ASSERT(m_hSerial > 0);

    int rlen, len;
    unsigned char *cp;
    unsigned char buff[MAX_PROTOCOL_UNIT_SIZE];
    int unit_id;

    rlen = nLen;
    cp = (unsigned char *) (pData + 4);
    rlen -= 4;

    while (rlen > 0) {
        len = v2dLibGetProtocolUnit(buff, 0, MAX_PROTOCOL_UNIT_SIZE,
                &unit_id, cp);

        if (len == -1)
            break;
        switch (unit_id) {
        case SERDAT_IDENT_BASIC:
            if (write(m_hSerial, buff, len) < 0) {
                SYSLOG_ERROR("V2DFPGA: Unable to write serial data");
            }
            break;
        default:
            SYSLOG_DEBUG("V2DFPGA: Received Unknown Serial Protocol Unit: %d", unit_id);
            break;
        }
        rlen -= len + 4;
        cp += len + 4;
    }

    return retval;
}

int CFPGARx::ProcessMsBuffer()
{
    if (m_bXpiDevice){
        char data[40]; /* tembuf Clean this in code cleanup*/
        ProcessRelMouse(m_pMsBuffer, data, m_nMslen);
    } else {
        write_to_ms_fpga(m_pHal, m_pMsBuffer, m_nMslen, m_nMouseType);
    }
    m_nMslen = 0;
    return 0;
}

int CFPGARx::ProcessAltStream()
{
    int retval = 0;
    char *pData;

    // Wait for queue to be assigned
    if (m_qQueueSource == NULL)
        return 0;

    // Process KB Buffer
    // if (m_nKblen > 0) ProcessKbBuffer();

    // Process MS Buffer
    if (m_nMslen > 0)
        ProcessMsBuffer();

    // Get frame from Alt Queue and send it to v2d lib
    CSFrame * sAltFrame = m_qQueueSource->GetFrame();
    if (sAltFrame == NULL)
        goto EXIT_LABEL;

    // In errored state, just release incoming frame and return
    if (IsErrored()) {
        m_qQueueSource->ReleaseFrame(sAltFrame);
        return 0;
    }

    // For now enable/disable video on XPi TX to make it forward or
    // not forward to network
    if (m_bXpiDevice) {
        if (sAltFrame->GetMessageType() == CSFrame::Connected) {
            // Audio and video are enabled on receiving NetHdr from v2dtx
        } 
        if (sAltFrame->GetMessageType() == CSFrame::Disconnected) {
            if (m_bIsMulticast && m_nMulticastCount > 1) {
                /* Not all multicast connections have been disconnected yet */
                m_nMulticastCount--;
                SYSLOG_INFO("Continue serving multicast: %d",  m_nMulticastCount);
            }
            else {
                SYSLOG_INFO("Stopping streaming video and audio ...");
                hal_clear_nethdr(m_pHal, NETHDR_VIDEO_OUT_CONTEXT);
                hal_clear_nethdr(m_pHal, NETHDR_AUDIO_OUT_CONTEXT);
                if (m_bIsMulticast) {
                    m_nMulticastCount--;
                    SYSLOG_INFO("Done serving multicast: %d",  m_nMulticastCount);
                    m_bIsMulticast = false;
                    m_nMulticastCount = 0;
                }
            }
        } 
    }

    if (sAltFrame->GetMessageType() != CSFrame::Normal) {
        // Ignore custom messages (connect/disconnect)
        m_qQueueSource->ReleaseFrame(sAltFrame);
        return 0;
    }

    if (sAltFrame->GetStreamType(STREAMTYPE_V2D) == STREAMTYPE_V2D) {
        int protid;
        int nBytes = 0;
        CV2DStreamMediaFlag v2dSMediaFlag(sAltFrame->GetMediaFlag());

        pData = sAltFrame->GetData();

        switch (v2dSMediaFlag.GetChannel()) {
        case FRAMETYPE_V2D_CHAN_CTL:
            protid = GET_PROTOCOL_UNIT_ID((unsigned char *) pData);
            nBytes = sAltFrame->GetLength();
            switch (protid) {
            case IDENT_CLIENT_AVREQ:
                SetEncParams(pData);
                m_pRFB->ResetReady();
                m_nCtlInPackets++;
                m_nCtlInBytes += nBytes;
                break;
            case IDENT_CLIENT_CSTAT:
                m_nCtlInPackets++;
                m_nCtlInBytes += nBytes;
                break;
            case IDENT_CLIENT_SERDAT:
                PopulateSerialBuffer(pData);
                m_nSerOutPackets++;
                m_nSerOutBytes += nBytes;
                break;
            case IDENT_CLIENT_KBDAT:
                PopulateKbBuffer(pData);
                m_nKbPackets++;
                m_nKbBytes += nBytes;
                break;
            case IDENT_CLIENT_MSDAT:
                PopulateMsBuffer(pData);
                m_nMsPackets++;
                m_nMsBytes += nBytes;
                break;
            case IDENT_CLIENT_KBABS:
                if (m_bXpiDevice && !m_pRFB->IsRFBEnabled())
					m_pUsb->ProcessRemoteKeyboard(pData);
                else
                   m_pRFB->ProcessKeyboard(pData);
                m_nKbPackets++;
                m_nKbBytes += nBytes;
                break;
            case IDENT_CLIENT_MSABS:
                if (m_bXpiDevice && !m_pRFB->IsRFBEnabled())
 						m_pUsb->ProcessRemoteMouse(pData);
                else
                    m_pRFB->ProcessMouse(pData);
                m_nMsPackets++;
                m_nMsBytes += nBytes;
                break;
            case IDENT_CLIENT_RFBQUERY:
                m_pRFB->ProcessRFBQuery(pData);
                m_nCtlInPackets++;
                m_nCtlInBytes += nBytes;
                break;
            case IDENT_CLIENT_NETHDR:
                ProcessNetHeader(pData);
                m_nCtlInPackets++;
                m_nCtlInBytes += nBytes;
                break;
            }
            break;
        default:
            ASSERT(0);
            break;
        }
    }

    m_qQueueSource->ReleaseFrame(sAltFrame);

    EXIT_LABEL: return retval;
}

int CFPGARx::ProcessStream()
{
    struct timeval timeout;
    fd_set read_fdset;
    int nfds = 0;
    int retval = 0;
    struct list_head *tmp;
    struct event_s *evententryp;

    // Dont do anything till we are started
    if (m_qQueueSink == NULL)
        return 0;

    if (m_nTimestamp == 0) {
        m_nStartTimestamp = CTimeKeeper::Now();
        m_nStartWallClock = CTimeKeeper::Now();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_BEGIN;
    }
    m_nTimestamp = CTimeKeeper::Now();
    m_nFrameWallClock = CTimeKeeper::Now();

    // Process Alt stream first
    if (ProcessAltStream() > 0) {
        return 1;
    }

    // Do not do any thing in errored state
    if (IsErrored()) {
        return 0;
    }

    // Process RFB
    if (m_pRFB->ProcessRFBConnection() > 0) {
        return 1;
    }

    // Send SCAP every five seconds
    //if (m_tLastScapSent == 0 || m_nTimestamp - m_tLastScapSent > 5000) {
    if (m_tLastScapSent == 0 || m_nTimestamp - m_tLastScapSent > 1000) {
        CreateScap();
        SendScap();
        m_tLastScapSent = m_nTimestamp;
        m_pRFB->RequestReady();
    }

    /* If driver not present, do splash screen only */
    if (m_bXpiDevice == false && m_hVideo <= 0) {
        if (GetVideoFrame() == 0)
            return 1;
        return 0;
    }

    FD_ZERO(&read_fdset);

    int maxfd = 0;
    // PSCW - On XPi if we don't include video and audio descriptors in the 
    // select the CPU utilization is ~18%. If we include them (& check if 
    // read returns -1) the CPU utilization becomes ~98%. 

    if (m_bXpiDevice == false && m_hVideo > 0) {
        FD_SET(m_hVideo, &read_fdset);
        if (maxfd < m_hVideo)
            maxfd = m_hVideo;
    }

    if (m_bXpiDevice == false && m_hAudio > 0) {
        FD_SET(m_hAudio, &read_fdset);
        if (maxfd < m_hAudio)
            maxfd = m_hAudio;
    }

    if (m_hSerial > 0) {
        FD_SET(m_hSerial, &read_fdset);
        if (maxfd < m_hSerial)
            maxfd = m_hSerial;
    }

    if (m_hUsbEventHead != NULL && !list_empty(m_hUsbEventHead)) {
        list_for_each(tmp, m_hUsbEventHead) {
            evententryp = list_entry(tmp, struct event_s, event_list);
            if (evententryp->event_fd > 0) {
                FD_SET(evententryp->event_fd, &read_fdset);
                if (maxfd < evententryp->event_fd)
                    maxfd = evententryp->event_fd;
            }
        }
    }

    if (m_pKbmIpc != NULL && m_pKbmIpc->m_nKeyboardfifofd > 0) {
        FD_SET(m_pKbmIpc->m_nKeyboardfifofd, &read_fdset);
        if (maxfd < m_pKbmIpc->m_nKeyboardfifofd)
            maxfd = m_pKbmIpc->m_nKeyboardfifofd;
    }

    if (m_pKbmIpc != NULL && m_pKbmIpc->m_nMouseAbsfifofd > 0) {
         FD_SET(m_pKbmIpc->m_nMouseAbsfifofd, &read_fdset);
         if (maxfd < m_pKbmIpc->m_nMouseAbsfifofd)
             maxfd = m_pKbmIpc->m_nMouseAbsfifofd;
    }

	if (m_pUsb != NULL && m_pUsb->GetKeyboardFD() > 0) {    /* this is for reading LED status */
       FD_SET(m_pUsb->GetKeyboardFD(), &read_fdset);
       if (maxfd < m_pUsb->GetKeyboardFD())
           maxfd = m_pUsb->GetKeyboardFD();

    }

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    nfds = select(maxfd + 1, &read_fdset, NULL, NULL, &timeout);
    if (nfds < 0) {
        // Error Handling code
    }
    else if (nfds == 0) {
        // Timeout no data
        m_nNoData++;
    }
    else {
        if (m_hAudio > 0 && FD_ISSET(m_hAudio, &read_fdset)) {
            if (GetAudioFrame() > 0)
                retval++;
        }
        if (m_hVideo > 0 && FD_ISSET(m_hVideo, &read_fdset)) {
            if (GetVideoFrame() == 0)
                retval++;
        }
        else {
            m_nNoData++;
        }
        if (m_hSerial > 0 && FD_ISSET(m_hSerial, &read_fdset)) {
            GetSerialData();
            retval++;
        }
		if (m_pUsb != NULL && m_pUsb->GetKeyboardFD() > 0) {
            if (FD_ISSET(m_pUsb->GetKeyboardFD(), &read_fdset))
                m_pUsb->SetKeyboardLEDsStatus();
        }
        if (m_hUsbEventHead != NULL && !list_empty(m_hUsbEventHead)) {
            list_for_each(tmp, m_hUsbEventHead) {
                evententryp = list_entry(tmp, struct event_s, event_list);
                if ((evententryp->event_fd > 0)
                        && FD_ISSET(evententryp->event_fd, &read_fdset)) {
                    ProcessUsbEventData(evententryp);
                }
            }
        }
        if (m_pKbmIpc != NULL && m_pKbmIpc->m_nKeyboardfifofd > 0) {
           if (FD_ISSET(m_pKbmIpc->m_nKeyboardfifofd, &read_fdset)) {
              ProcessVNCKeyboardEventData();
           }
        }

        if (m_pKbmIpc != NULL && m_pKbmIpc->m_nMouseAbsfifofd > 0) {
           if (FD_ISSET(m_pKbmIpc->m_nMouseAbsfifofd, &read_fdset)) {
              ProcessVNCMouseEventData();
           }
        }
    }

    return retval;
}

int
CFPGARx::UpdateUSBDevices(CCommand *a_pCmd)
{
    int retval = 0;
    int ndevnum;
    char sDevice[MAXDEVLEN];

    if (a_pCmd->GetIntVar("addeventdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/event%d", ndevnum);
        if (hal_add_event_device(m_pHal, sDevice) > 0) {
            retval = 1;
        }
    }
    if (a_pCmd->GetIntVar("removeeventdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/event%d", ndevnum);
        if (hal_remove_event_device(m_pHal, sDevice) > 0) {
            retval = 1;
        }
    }
    if (a_pCmd->GetIntVar("addserialdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/ttyUSB%d", ndevnum);
        if (hal_add_serial_device(m_pHal, sDevice) > 0) {
            InitSerialParameters();
        }
        retval = 1;
    }
    if (a_pCmd->GetIntVar("removeserialdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/ttyUSB%d", ndevnum);
        if (hal_remove_serial_device(m_pHal, sDevice) > 0) {
            InitSerialParameters();
        }
        retval = 1;
    }

    return retval;
}


int CFPGARx::Update(CCommand *a_pCmd)
{
    int retval = 0;

    // Process RFB parameters
    if (m_pRFB != NULL && m_pRFB->Update(a_pCmd))
        retval = 1;

	if (m_bXpiDevice) {
		if (m_pUsb != NULL && m_pUsb->UpdateKeyboardLayout(a_pCmd))
            retval = 1;
    }
    

    if (UpdateConnectionProfile(a_pCmd))
        retval = 1;

    if (UpdateKBMTypes(a_pCmd))
        retval = 1;

    if (UpdateKeyboardEscapeSequences(a_pCmd))
       retval = 1;

    if (UpdateSplash(a_pCmd))
        retval = 1;

    if (UpdateUSBDevices(a_pCmd))
        retval = 1;

    return retval;
}

void CFPGARx::PrintStatistics(string &a_stats)
{
    // Sync (with the other board) whether keyboard is in console mode
    if (m_pHal != NULL)
        hal_update_virtual_console(m_pHal);
}

void CFPGARx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("FPGAPort",
                           m_nIOPort);
    a_pCmd->SetObjectParam("VideoName",
                           (char *) m_LUT->comment);
    a_pCmd->SetObjectCounterParam("VideoPackets",
                                  m_nVideoPackets);
    a_pCmd->SetObjectByteCounterParam("VideoBytes",
                                      m_nVideoBytes);
    a_pCmd->SetObjectCounterParam("AudioPackets",
                                  m_nAudioPackets);
    a_pCmd->SetObjectByteCounterParam("AudioBytes",
                                      m_nAudioBytes);
    a_pCmd->SetObjectCounterParam("ControlInPackets",
                                  m_nCtlInPackets);
    a_pCmd->SetObjectByteCounterParam("ControlInBytes",
                                      m_nCtlInBytes);
    a_pCmd->SetObjectCounterParam("ControlOutPackets",
                                  m_nCtlOutPackets);
    a_pCmd->SetObjectByteCounterParam("ControlOutBytes",
                                      m_nCtlOutBytes);
    a_pCmd->SetObjectCounterParam("MousePackets",
                                  m_nMsPackets);
    a_pCmd->SetObjectErrorCounterParam("MouseLostPackets",
                                       m_nMsLostPackets);
    a_pCmd->SetObjectByteCounterParam("MouseBytes",
                                      m_nMsBytes);
    a_pCmd->SetObjectCounterParam("KeyBoardPackets",
                                  m_nKbPackets);
    a_pCmd->SetObjectErrorCounterParam("KeyBoardLostPackets",
                                       m_nKbLostPackets);
    a_pCmd->SetObjectByteCounterParam("KeyBoardBytes",
                                      m_nKbBytes);
    a_pCmd->SetObjectCounterParam("SerialInPackets",
                                  m_nSerInPackets);
    a_pCmd->SetObjectErrorCounterParam("SerialInLostPackets",
                                       m_nSerInLostPackets);
    a_pCmd->SetObjectByteCounterParam("SerialInBytes",
                                      m_nSerInBytes);
    a_pCmd->SetObjectCounterParam("SerialOutPackets",
                                  m_nSerOutPackets);
    a_pCmd->SetObjectByteCounterParam("SerialOutBytes",
                                      m_nSerOutBytes);
    a_pCmd->SetObjectCounterParam("UsbKbInBytes",
                                  m_nUsbKbInBytes);
    a_pCmd->SetObjectByteCounterParam("UsbKbInPackets",
                                      m_nUsbKbInPackets);
    a_pCmd->SetObjectCounterParam("UsbMsInBytes",
                                  m_nUsbMsInBytes);
    a_pCmd->SetObjectByteCounterParam("UsbMsInPackets",
                                      m_nUsbMsInPackets);

    a_pCmd->SetObjectByterateGuageParam("RequestBandwidth",
                                        m_nRequestBandwidth);
    a_pCmd->SetObjectByterateGuageParam("ComputedBandwidth",
                                        m_nComputedBandwidth);
    a_pCmd->SetObjectGuageParam("LowCompression",
                                m_nMinCompression);
    a_pCmd->SetObjectGuageParam("HighCompression",
                                m_nMaxCompression);
    a_pCmd->SetObjectParam("BurstRatio",
                           (double) m_fBurstRatio);
    a_pCmd->SetObjectGuageParam("BurstDuration",
                                m_nBurstDuration);
    a_pCmd->SetObjectParam("AVOption",
                           m_nAVOption);
    a_pCmd->SetObjectParam("AudioType",
                           m_nAudioType);
    a_pCmd->SetObjectErrorCounterParam("AudioSOFerrors",
                           m_nAudioSOFerrors);

    a_pCmd->SetObjectParam("KBMIdletime",
                            m_nKBMIdletime);
    a_pCmd->SetObjectParam("FSRefreshCount",
                           m_nFSRefreshCount);
    a_pCmd->SetObjectParam("MouseType",
                            m_nMouseType);
    a_pCmd->SetObjectParam("KeyboardType",
                           m_nKeyboardType);
    if (m_pHal != NULL && m_pHal->console_info.kb_active)
        a_pCmd->SetObjectParam("KeyboardMode", "Local");
    else
        a_pCmd->SetObjectParam("KeyboardMode", "Console");

    a_pCmd->SetObjectConstantParam("CodecVersion", m_nCodecVersion);

    if (m_pRFB != NULL)
        m_pRFB->UpdateCustomStatus(a_pCmd);

    if (a_pCmd->StatusLevel(2)) {
        if (m_pShaper != NULL)
            m_pShaper->UpdateCustomStatus(a_pCmd);
        a_pCmd->SetObjectCounterParam("NoData",
                                      m_nNoData);
    }

    a_pCmd->SetObjectParam("Multicast", m_bIsMulticast);

    a_pCmd->SetObjectParam("MulticastCount", m_nMulticastCount);
}
