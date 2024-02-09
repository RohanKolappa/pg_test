
#include "strmsrv_compatibility.h"
#include <termios.h>
#include <math.h>
#include "v2d.h"
#include "fpgatx.hh"
#include "common.hh"
#include "V2DUtils.hh"
#include "rtp.h"
#include "rtputils.hh"
#include "splash.hh"
#include "USB.h"
#include "netutils.hh"

using namespace std;

CPortUse CFPGATx::m_portsUsed;

CFPGATx::CFPGATx (int nId, int nIOPort, bool bEnableKBMS) :
        CSender(nId)
{

    InitVariablesToZero();

    m_nPortType = V2DHWPORT;

    m_nRetryDropCount = 0;
    m_pSendBuffer = new char [SEND_BUFFER_SIZE];

    m_scapData = new ScapData_t;
    bzero(m_scapData, sizeof(ScapData_t));

    m_splashScap = new ScapData_t;
    bzero(m_splashScap, sizeof(ScapData_t));
    m_scapData->m_videoCodecVersion = 1;
    /* Make sure the port is not already in use */
    if (m_portsUsed.ReservePort(nIOPort)) {
        m_nIOPort = nIOPort;
    }
    else {
        m_nIOPort = -1;
    }

    m_pHal = NULL;
    m_bEnableKBMS = bEnableKBMS;
    m_bRFBStatus = false;

    if (m_nIOPort > 0) {
        m_pHal = hal_acquire(m_nIOPort - 1);
    }

    m_hVideo = m_hAudio = m_hKeyboard = m_hMouse = m_hSerial = -1;

    if (m_pHal != NULL) {
        m_scapData->m_videoCodecVersion = hal_get_codec_version(m_pHal);
        m_hVideo = hal_get_video_fd(m_pHal);
        m_hAudio = hal_get_audio_fd(m_pHal);
        m_hKeyboard = hal_get_keyboard_fd(m_pHal);
        m_hMouse = hal_get_mouse_fd(m_pHal);
        m_hUsbEventHead = hal_get_event_fd_list(m_pHal);
        m_bXpiDevice = 
                (hal_get_hardware_type(m_pHal) == XPI_DEVICE) ? true : false;
    }
    else {
        SetError();
    }

    /* This object does not do any thing else in case of error */
    if (IsErrored()) {
        return;
    }

    m_pClnt = NULL;
    if (m_pHal != NULL) {
        m_pClnt = clnt_acquire(m_pHal);
    }

	m_pUsbHid = new CUSBHID();
    m_pUsb = new CUSB(m_pUsbHid,NULL,m_pClnt);

    V2DUtils::CreateDefaultScap(m_splashScap);

    InitSplashFrame();

    InitSerialConfig();

    m_nMaxfd = GetMaxfd(m_hKeyboard, m_hMouse, m_hSerial, m_hUsbEventHead);

    // Initialize remote keyboard and mouse types
    if (m_pClnt != NULL) {
        clnt_set_remote_keyboard_type(m_pHal, m_pClnt,
                m_pClnt->local_keyboard_type);
        clnt_set_remote_mouse_type(m_pHal, m_pClnt,
                m_pClnt->local_mouse_type);
    }

    // Initialize console device
    InitConsoleDevice();

    // Initialize RFB mouse and session Id
    if (m_pClnt != NULL) {
        m_pClnt->rfb_info.session_id = v2dLibGetRandomNumber();
        InitMousePointer(); // get timeout & mouse input enabled or disabled 
    }
    

}

CFPGATx::~CFPGATx()
{
    if (m_pHal != NULL)
        hal_release(m_pHal);

    if (m_pClnt != NULL)
        clnt_release(m_pClnt);

    /* Release the FPGA port */
    if (m_nIOPort > 0)
        m_portsUsed.ReleasePort(m_nIOPort);

    /* Release console device */
    CloseConsoleDevice();

    delete m_pSplash;
    
	if(m_pUsb)
		delete m_pUsb;

	if(m_pUsbHid)
		delete m_pUsbHid;
	
    delete m_scapData;
    delete m_splashScap;

    delete[] m_pSendBuffer;

}

void CFPGATx::InitVariablesToZero()
{
    m_nControlPackets = 0;
    m_nVideoPackets = 0;
    m_nAudioPackets = 0;
    m_nKbInPackets = 0;
    m_nKbInBytes = 0;
    m_nMsInPackets = 0;
    m_nMsInBytes = 0;
    m_nScapCount = 0;
    m_nVideoChanged = 0;
    m_nSerInPackets = 0;
    m_nSerInBytes = 0;
    m_nSerOutPackets = 0;
    m_nSerOutBytes = 0;
    m_nUsbKbInPackets = 0;
    m_nUsbKbInBytes = 0;
    m_nUsbMsInPackets = 0;
    m_nUsbMsInBytes = 0;
    m_nVideoPacketsWritten = 0;
    m_nVideoBytesWritten = 0;
    m_nAudioPacketsWritten = 0;
    m_nAudioBytesWritten = 0;
    m_nRFBStatusReplies = 0;
    m_nRFBStatusQueries = 0;
    m_hUsbEventHead = NULL;
    m_nMaxfd = 0;
    m_nAvoption = 0;
    m_pSplash = NULL;
    m_bSplashVideo = false;
    m_bSplashRequested = false;
    m_tSplashReqTimestamp = 0;
    m_bIsWaiting = false;
    m_pClnt = NULL;
    m_nLocalMousePointerIdleTime = 3;
    m_bLocalMousePointerEnable = 1;
    m_tLastMouseReceivedTimestamp = 0;
}

void CFPGATx::InitSplashFrame()
{

    m_pSplash = new CSplash(m_pHal, false);

    m_pSplash->GetSCAP(m_splashScap);

    // Enable Splash mode
    SwitchSplashMode(true);

}


int CFPGATx::UpdateSplash(CCommand *a_pCmd)
{
    int retval = 0;

    int nUpdateSplash;
    if (a_pCmd->GetIntVar("updatesplash", &nUpdateSplash) != -1) {
        if (nUpdateSplash) {
            if (m_bXpiDevice || m_pSplash->IsReady()) {
                delete m_pSplash;
                m_pSplash = new CSplash(m_pHal, false);
                m_pSplash->GetSCAP(m_splashScap);

                // If serving splash currently, set splash parameter in kernel
                if (m_bSplashVideo == true) {
                    if (CompareAndSetScap(m_splashScap) == 1) {
                        ScapIoctl(m_splashScap);
                    }
                    if (m_bXpiDevice) {
                        m_bSplashVideo = false;
                        hal_disable_splash_video(m_pHal);
                        SwitchSplashMode(true);
                    }
                    else {
                        m_pSplash->SetFSRefresh();
                    }
                }
                retval = 1;
            }
            else {
                SYSLOG_INFO("V2DFPGA: Ignoring splash update. "
                        "Older splash object still busy.");
            }
        }
    }
    return retval;
}

void CFPGATx::InitSerialConfig()
{
    struct serial_config serial_cfg;
    memset(&serial_cfg, 0x0, sizeof(serial_cfg));
    get_rx_serial_config(m_pHal, &serial_cfg);
    m_hSerial = -1;
    if (serial_cfg.serial_enable) {
        m_hSerial = hal_get_serial_fd(m_pHal);
        if (m_hSerial  > 0) {
            set_serial_parameters(m_hSerial,
                                  serial_cfg.baudrate,
                                  serial_cfg.databits,
                                  serial_cfg.stopbits,
                                  serial_cfg.parity,
                                  serial_cfg.flowcontrol);
        }
    }
}

void CFPGATx::InitConsoleDevice()
{
    if (hal_get_console_fd(m_pHal) > 0)
        hal_initialize_console(m_pHal);
}

void CFPGATx::CloseConsoleDevice()
{
    if (hal_get_console_fd(m_pHal) > 0)
        hal_close_console(m_pHal);
}

void CFPGATx::InitMousePointer()
{
    int MousePointerEnable;
    
    get_rx_local_mouse_pointer_config(m_pHal, &MousePointerEnable,
                                       &m_nLocalMousePointerIdleTime);
    m_bLocalMousePointerEnable = (MousePointerEnable) ? 1: 0;
      
    m_pClnt->local_mouse_pointer_enable  = m_bLocalMousePointerEnable;

}

/*****************************************************
* This function returns true even if any Scap value  *
* has changed. Note that only few values are         *
* considered for Video.                              *
******************************************************/
bool
CFPGATx::CompareAndSetScap(ScapData_t *a_pScap)
{
    bool rc = false;

    if (memcmp(a_pScap, m_scapData, sizeof(ScapData_t)) == 0) {
        return false;
    }
    if (a_pScap->m_videoStereoSet) {
        if (a_pScap->m_videoStereo != m_scapData->m_videoStereo) {
            m_scapData->m_videoStereo = a_pScap->m_videoStereo;
            m_scapData->m_videoStereoSet = 1;
            rc = true;
        }
    }
    if (a_pScap->m_hResSet) {
        if (a_pScap->m_hRes != m_scapData->m_hRes) {
            m_scapData->m_hRes = a_pScap->m_hRes;
            m_scapData->m_hResSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_vResSet) {
        if (a_pScap->m_vRes != m_scapData->m_vRes) {
            m_scapData->m_vRes = a_pScap->m_vRes;
            m_scapData->m_vResSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_origHresSet) {
        if (a_pScap->m_origHres != m_scapData->m_origHres) {
            m_scapData->m_origHres = a_pScap->m_origHres;
            m_scapData->m_origHresSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_origVresSet) {
        if (a_pScap->m_origVres != m_scapData->m_origVres) {
            m_scapData->m_origVres = a_pScap->m_origVres;
            m_scapData->m_origVresSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoRefresh) {
        if (a_pScap->m_videoRefresh != m_scapData->m_videoRefresh) {
            m_scapData->m_videoRefresh = a_pScap->m_videoRefresh;
            m_scapData->m_videoRefreshSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoPcrSet) {
        if (a_pScap->m_videoPcr != m_scapData->m_videoPcr) {
            m_scapData->m_videoPcr = a_pScap->m_videoPcr;
            m_scapData->m_videoPcrSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoHfpSet) {
        if (a_pScap->m_videoHfp != m_scapData->m_videoHfp) {
            m_scapData->m_videoHfp = a_pScap->m_videoHfp;
            m_scapData->m_videoHfpSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoHbpSet) {
        if (a_pScap->m_videoHbp != m_scapData->m_videoHbp) {
            m_scapData->m_videoHbp = a_pScap->m_videoHbp;
            m_scapData->m_videoHbpSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoHsyncSet) {
        if (a_pScap->m_videoHsync != m_scapData->m_videoHsync) {
            m_scapData->m_videoHsync = a_pScap->m_videoHsync;
            m_scapData->m_videoHsyncSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoVfpSet) {
        if (a_pScap->m_videoVfp != m_scapData->m_videoVfp) {
            m_scapData->m_videoVfp = a_pScap->m_videoVfp;
            m_scapData->m_videoVfpSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoVbpSet) {
        if (a_pScap->m_videoVbp != m_scapData->m_videoVbp) {
            m_scapData->m_videoVbp = a_pScap->m_videoVbp;
            m_scapData->m_videoVbpSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoVsyncSet) {
        if (a_pScap->m_videoVsync != m_scapData->m_videoVsync) {
            m_scapData->m_videoVsync = a_pScap->m_videoVsync;
            m_scapData->m_videoVsyncSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoNameSet) {
        if (strcmp(a_pScap->m_videoName, m_scapData->m_videoName) != 0) {
            strncpy(m_scapData->m_videoName, a_pScap->m_videoName, 80);
            m_scapData->m_videoNameSet = 1;
            rc = true;
        }
    }
    if (a_pScap->m_sliceSizeSet) {
        if (a_pScap->m_sliceSize != m_scapData->m_sliceSize) {
            m_scapData->m_sliceSize = a_pScap->m_sliceSize;
            m_scapData->m_sliceSizeSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoCompmodeSet) {
        if (a_pScap->m_videoCompmode != m_scapData->m_videoCompmode) {
            m_scapData->m_videoCompmode = a_pScap->m_videoCompmode;
            m_scapData->m_videoCompmodeSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoMotionCompmodeSet) {
        if (a_pScap->m_videoMotionCompmode != m_scapData->m_videoMotionCompmode) {
            m_scapData->m_videoMotionCompmode = a_pScap->m_videoMotionCompmode;
            m_scapData->m_videoMotionCompmodeSet = 1;
            rc  = true;
        }
    }
    if (a_pScap->m_videoCodecVersionSet) {
        if(a_pScap->m_videoCodecVersion != m_scapData->m_videoCodecVersion) {
          m_scapData->m_videoCodecVersion = a_pScap->m_videoCodecVersion;
          m_scapData->m_videoCodecVersionSet = 1;
          NotifyCodecVersionChange();
          rc = true;
        }
    }
    return rc;
}

/******************************************************
* This function returns 1 even if any one Scap value  *
* has changed for keyboard and mouse. Note that only  *
* these two values are considered in this case.       *
******************************************************/
bool
CFPGATx::CompareAndSetKbMsScap(ScapData_t *a_pScap)
{
    bool rc = false;
    if (a_pScap->m_kbTypeSet) {
        if (a_pScap->m_kbType != m_scapData->m_kbType) {
            m_scapData->m_kbType = a_pScap->m_kbType;
            m_scapData->m_kbTypeSet = 1;
            rc = true;
        }
    }
    if (a_pScap->m_msTypeSet) {
        if (a_pScap->m_msType != m_scapData->m_msType) {
            m_scapData->m_msType = a_pScap->m_msType;
            m_scapData->m_msTypeSet = 1;
            rc = true;
        }
    }
    return rc;
}

void
CFPGATx::NotifyCodecVersionChange()
{
  int currentversion = hal_get_codec_version(m_pHal);

  if (currentversion !=  m_scapData->m_videoCodecVersion) {
    SYSLOG_INFO("%s Codec Change detected: %d != %d", __func__, 
            currentversion, m_scapData->m_videoCodecVersion);
    hal_configure_rx_codec_version(m_pHal, m_scapData->m_videoCodecVersion);
    m_bIsWaiting = true;
  }
}

void
CFPGATx::ReloadScapIoctl()
{
  int currentVersion;

  if (m_bIsWaiting) {
    currentVersion = hal_get_codec_version(m_pHal);
    if (currentVersion == m_scapData->m_videoCodecVersion) {
      ScapIoctl(m_scapData);
      m_bIsWaiting = false;
    }
  }
}


/********************************************************
* Process Frames that are of Control type               *
*********************************************************/
void
CFPGATx::ProcessFrameTypeControl(CSFrame *sFrame)
{
    char *pData;

    ScapData_t scap;

    m_nControlPackets++;

    // In errored state, do not do anything
    if (IsErrored()) {
        return;
    }

    if (v2dLibParseScap(&scap, sFrame->GetData()) != -1) {
        m_nScapCount++;
        if (CompareAndSetScap(&scap)) {
            m_nVideoChanged++;
            if (!m_bIsWaiting) {
                ScapIoctl(&scap);
            }
        }
        if (CompareAndSetKbMsScap(&scap)) {
            if (scap.m_kbTypeSet)
                clnt_set_remote_keyboard_type(m_pHal, m_pClnt,
                    m_scapData->m_kbType);
            if (scap.m_msTypeSet)
                clnt_set_remote_mouse_type(m_pHal, m_pClnt,
                    m_scapData->m_msType);
        }
    }
    else {
        pData = sFrame->GetData();
        int protid = GET_PROTOCOL_UNIT_ID((unsigned char *) pData);
        int nBytes = sFrame->GetLength();
        switch (protid) {
        case IDENT_SERVER_SERDAT:
            PopulateSerialBuffer(pData);
            m_nSerOutPackets++;
            m_nSerOutBytes += nBytes;
            break;
        case IDENT_SERVER_RFBSTATUS:
            ProcessRFBStatus(pData);
            break;
        case IDENT_SERVER_RTPFILTER:
            ProcessRtpFilter(pData);
            break;	    
        default:
            break;
        }
    }
}

/********************************************************
* Process Frames that are of Video type                 *
*********************************************************/
void
CFPGATx::ProcessFrameTypeVideo(CSFrame *sFrame)
{

    m_nVideoPackets++;

    // In errored state, do not do anything
    if (IsErrored()) {
        return;
    }

    if (m_bSplashVideo == true)
        return;

    char * buff =  sFrame->GetData();

    unsigned long nStreamType = sFrame->GetStreamType();

    if (nStreamType == STREAMTYPE_V2D) {
        CV2DStreamMediaFlag v2dSMediaFlag(sFrame->GetMediaFlag());

        const int nDataOffset = (v2dSMediaFlag.GetEncaps() ==
                                FRAMETYPE_V2D_ENCAPS_RTP) ?
                                RTP_HEADER_SIZE : V2D_PACKET_HEADER_SIZE;

        if (buff != NULL && m_hVideo > 0) {
            write(m_hVideo, buff + nDataOffset,
                        sFrame->GetLength() - nDataOffset);
#ifdef DUMP
            {
                FILE *fp = fopen("/tmp/videodump", "a+");
                if (fp == NULL) 
                    fp = fopen("/tmp/videodump", "w+");
                if (fp == NULL) 
                    SYSLOG_INFO("Error create video dump file. %m.");
                else {
                    fwrite(buff + nDataOffset, 1, 
			sFrame->GetLength() - nDataOffset, fp);
                }   
                fclose(fp);
            }
#endif
            m_nVideoPacketsWritten++;
            m_nVideoBytesWritten += sFrame->GetLength() - nDataOffset;
        }
    }
}


/********************************************************
* Process Frames that are of Audio type                 *
*********************************************************/
void
CFPGATx::ProcessFrameTypeAudio(CSFrame *sFrame)
{
    m_nAudioPackets++;

    // In errored state, do not do anything
    if (IsErrored()) {
        return;
    }

    if (m_bSplashVideo == true)
        return;

    unsigned char tmpbuff[AV_BUFFER_SIZE], tmpbuff2[AV_BUFFER_SIZE];
    unsigned char *pDataIn = NULL;
    int lenDataIn = 0;

    unsigned long nStreamType = sFrame->GetStreamType();

    if (nStreamType == STREAMTYPE_V2D) {
        CV2DStreamMediaFlag v2dSMediaFlag(sFrame->GetMediaFlag());

        if (v2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP) {

            ConvertAudioPacketFromRTPToV2D(sFrame->GetData(),
                                           sFrame->GetLength(),
                                           (char *)tmpbuff2,
                                           &lenDataIn);
            pDataIn   = tmpbuff2;
        }
        else {
            pDataIn = (unsigned char*)sFrame->GetData();
            lenDataIn = sFrame->GetLength();
        }
        if (lenDataIn < 32) {
            SYSLOG_DEBUG("V2DFPGATX: Audio packet too short");
            return;
        }
        int len = lenDataIn;

        memcpy(tmpbuff, pDataIn, lenDataIn);

        if (m_hAudio > 0) {
            write(m_hAudio, tmpbuff + V2D_PACKET_HEADER_SIZE,
                  len - V2D_PACKET_HEADER_SIZE);
#ifdef DUMP
            {
                FILE *fp = fopen("/tmp/audiodump", "a+");
                if (fp == NULL) 
                    fp = fopen("/tmp/audiodump", "w+");
                if (fp == NULL) 
                    SYSLOG_INFO("Error create audio dump file. %m.");
                else {
                    fwrite(tmpbuff + V2D_PACKET_HEADER_SIZE, 1, 
			len - V2D_PACKET_HEADER_SIZE, fp);
                }   
                fclose(fp);
            }
#endif
            m_nAudioPacketsWritten++;
            m_nAudioBytesWritten += len - V2D_PACKET_HEADER_SIZE;

        }
    }

}

int
CFPGATx::GetMaxfd(int hkeyboard,
                  int hmouse,
                  int hserial,
                  struct list_head *heventhead) {
    int maxfd = 0;

    if (hkeyboard > 0) {
        if (maxfd < hkeyboard) maxfd = hkeyboard;
    }
    if (hmouse > 0) {
        if (maxfd < hmouse) maxfd = hmouse;
    }
    if (hserial > 0) {
        if (maxfd < hserial) maxfd = hserial;
    }
    if (heventhead != NULL && !list_empty(heventhead)) {
        struct list_head *tmp;
        struct event_s *evententryp;
        list_for_each(tmp, heventhead) {
            evententryp = list_entry(tmp, struct event_s, event_list);
            if ((evententryp->event_fd > 0) && (maxfd < evententryp->event_fd))
                maxfd = evententryp->event_fd;
        }
    }
    return maxfd;
}

int
CFPGATx::ScapIoctl(ScapData_t *a_scap)
{
    struct LUT_struct lut;
    if (V2DUtils::PopulateLutFromScap(&lut, a_scap) != -1) {
        if (m_hVideo > 0 && (m_nAvoption != AVOPTION_AUDIO)) {
            if (configure_dropside_video(m_pHal, m_pClnt, &lut,
                                         a_scap->m_sliceSize,
                                         a_scap->m_videoCompmode,
                                         a_scap->m_videoMotionCompmode) != -1) {
                if (m_pClnt) {
                    m_pClnt->rfb_info.size_x = lut.a_orig_hres;
                    m_pClnt->rfb_info.size_y = lut.a_orig_vres;
                    m_pClnt->rfb_info.abs_x = lut.a_orig_hres / 2;
                    m_pClnt->rfb_info.abs_y = lut.a_orig_vres / 2;
                    clnt_init_mouse_cursor(m_pHal, m_pClnt);

                }
                return 0;
            }
            else {
                SYSLOG_INFO("V2DFPGATX: Drop Side Configuration failed. "
                        "Retry needed.");
                m_nRetryDropCount = 3;
            }
        }
    }
    return -1;
}

int
CFPGATx::WriteSplashFrame()
{
    // Skip if XPI
    if (m_bXpiDevice) {
        return 0;
    }

    // In error state, do not do anything
    if (IsErrored()) {
        return -1;
    }

    // Wait till splash has been configured in the driver
    if (m_bSplashRequested == true)
        return 0;

    int len = m_pSplash->GetSplashPacket(m_pSendBuffer);
    if (len == 0)
        return 0;

    if (m_hVideo > 0) {
        int rc;
        if ((rc = write(m_hVideo, m_pSendBuffer, len)) < 0) {
            return -1;
        }
    }
    m_nVideoPacketsWritten++;
    m_nVideoBytesWritten += len;


    return 0;
}

int
CFPGATx::ProcessKeyboardFrame()
{
    int nLen = 0;
    char pKbBuf[KB_BUFFER_SIZE+1];

    ASSERT(m_hKeyboard > 0);

    nLen = retrieve_client_keyboard_data (m_pHal, m_pClnt,
            (unsigned char *) pKbBuf, KB_BUFFER_SIZE);
    if (nLen > 0) {
        if (m_pClnt && (m_pClnt->rfb_info.status & RFB_STATUS_ENABLE)) {
            // Create RFB message from keyboard data and send to server
            ProcessRFBKbData((unsigned char *)pKbBuf, (unsigned char *)pKbBuf, nLen);
        }
        else {
            // Create KBDAT message from keyboard data and send to server
            nLen = v2dLibBuildKbData(pKbBuf, m_pSendBuffer, nLen);

            CV2DStreamMediaFlag v2dSMediaFlag;
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

            if (m_bEnableKBMS) {
                CSFrame * sFrame =
                        new CSFrame(m_pSendBuffer, nLen,
                                v2dSMediaFlag.GetMediaFlag(),
                                m_nTimestamp);
                ASSERT(m_qQueueSink != NULL);
                m_qQueueSink->InsertFrame(sFrame);
            }
        }
        m_nKbInBytes += nLen;
        m_nKbInPackets++;
    }

    return 0;
}

int
CFPGATx::ProcessMouseFrame()
{
    char pMsBuf[MS_BUFFER_SIZE+1];

    ASSERT(m_hMouse > 0);

    int nLen = retrieve_client_mouse_data (m_pHal, m_pClnt,
            (unsigned char *) pMsBuf, MS_BUFFER_SIZE);

    if (nLen > 0) {
        if (m_pClnt && (m_pClnt->rfb_info.status & RFB_STATUS_ENABLE)) {
            // Create RFB message from mouse data and send to server
            ProcessRFBMsData((unsigned char *)pMsBuf, nLen);
        }
        else {
            // Create MSDAT message from mouse data and send to server
            int nSendLen = v2dLibBuildMsData(pMsBuf, m_pSendBuffer, nLen);

            CV2DStreamMediaFlag v2dSMediaFlag;
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

            if (m_bEnableKBMS) {
                CSFrame * sFrame =
                        new CSFrame(m_pSendBuffer, nSendLen,
                                v2dSMediaFlag.GetMediaFlag(),
                                m_nTimestamp);
                ASSERT(m_qQueueSink != NULL);
                m_qQueueSink->InsertFrame(sFrame);
            }
        }
        m_nMsInBytes += nLen;
        m_nMsInPackets++;
    }

    return 0;
}

int
CFPGATx::ProcessSerialFrame()
{
    int nLen = 0;
    char pSerBuf[SER_BUFFER_SIZE+1];

    ASSERT(m_hSerial > 0);

    nLen = read(m_hSerial, pSerBuf, SER_BUFFER_SIZE);
    nLen = v2dLibBuildSerialData(pSerBuf, m_pSendBuffer, nLen);

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

    if (m_bEnableKBMS) {
        CSFrame * sFrame =
                new CSFrame(m_pSendBuffer, nLen,
                        v2dSMediaFlag.GetMediaFlag(),
                        m_nTimestamp);
        ASSERT(m_qQueueSink != NULL);
        m_qQueueSink->InsertFrame(sFrame);
    }
    m_nSerInBytes += nLen;
    m_nSerInPackets++;

    return 0;
}

int
CFPGATx::ProcessUsbEventFrame(struct event_s *eventp)
{
    int nDataType = 0;
    unsigned char pDataBuf[KB_BUFFER_SIZE +1];
    unsigned char pRfbDataBuf[KB_BUFFER_SIZE + 1];
    int nDataLen, nSendLen, nRfbbufLen;

    ASSERT(eventp != NULL && eventp->event_fd > 0);

    nDataType = retrieve_client_event_data (m_pHal, m_pClnt, eventp,
            (unsigned char *) pDataBuf, &nDataLen, KB_BUFFER_SIZE,
             (unsigned char *) pRfbDataBuf, &nRfbbufLen);

    if (nDataType == KBM_TYPE_KEYBOARD && nDataLen > 0) {

        if (m_pClnt != NULL &&
                     m_pClnt->curr_kbmode == KBM_MODE_LOCAL &&
                     m_bXpiDevice)
        {
            ProcessLocalKeyboard(pDataBuf, nDataLen);
            return 0;
        }

        if (m_pClnt && (m_pClnt->rfb_info.status & RFB_STATUS_ENABLE)) {
            // Create RFB message from keyboard data and send to server
            ProcessRFBKbData((unsigned char *)pDataBuf, (unsigned char *) pRfbDataBuf, nDataLen);
        }
        else {
            // Create raw keyboard message and send to server
            nSendLen = v2dLibBuildKbData((char *)pDataBuf, m_pSendBuffer,
                    nDataLen);

            CV2DStreamMediaFlag v2dSMediaFlag;
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

            if (m_bEnableKBMS) {
                CSFrame * sFrame =
                        new CSFrame(m_pSendBuffer, nSendLen,
                                v2dSMediaFlag.GetMediaFlag(),
                                m_nTimestamp);
                ASSERT(m_qQueueSink != NULL);
                m_qQueueSink->InsertFrame(sFrame);
            }
        }
        m_nUsbKbInBytes += nDataLen;
        m_nUsbKbInPackets++;
    }
    else if (nDataType == KBM_TYPE_MOUSE && nDataLen > 0) {

        if (m_pClnt != NULL  &&
                m_pClnt->curr_kbmode == KBM_MODE_LOCAL && 
                m_bXpiDevice)
        {
            ProcessLocalMouse(pDataBuf, nDataLen);
            return 0;
        }

        if (m_pClnt && (m_pClnt->rfb_info.status & RFB_STATUS_ENABLE)) {
            // Create RFB message from mouse data and send to server
            ProcessRFBMsData((unsigned char *)pDataBuf, nDataLen);
            m_tLastMouseReceivedTimestamp  = m_nTimestamp;
        }
        else {
            // Create raw mouse message and send to server
            nSendLen = v2dLibBuildMsData((char *)pDataBuf, m_pSendBuffer,
                    nDataLen);
            CV2DStreamMediaFlag v2dSMediaFlag;
            v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
            if (m_bEnableKBMS) {
                CSFrame * sFrame =
                        new CSFrame(m_pSendBuffer, nSendLen,
                                v2dSMediaFlag.GetMediaFlag(),
                                m_nTimestamp);

                ASSERT(m_qQueueSink != NULL);
                m_qQueueSink->InsertFrame(sFrame);
            }
        }
        m_nUsbMsInBytes += nDataLen;
        m_nUsbMsInPackets++;
    }

    return 0;
}

int CFPGATx::ProcessLocalMouse(unsigned char *pInMsBuf, 
                               int msLen)
{

    int indx = 0;
    int MousePktLen = 8;
    int cnt = 0;
    int nLen = 0;
    int rc = 1;
    char buf[4];

	if (m_pUsb == NULL || pInMsBuf == NULL || msLen <= 0)
        return -1;    
     
    clnt_update_mouse_cursor_pos(m_pHal, m_pClnt);
     
    for (indx = 0; indx < msLen; indx += MousePktLen)
    {
        buf[0] = pInMsBuf[cnt * MousePktLen + 1];   /* button press */
        buf[1] = pInMsBuf[cnt * MousePktLen + 3];   /* X Rel cordinate */
        buf[2] = pInMsBuf[cnt * MousePktLen + 5];   /* Y Rel cordinate */
        buf[3] = pInMsBuf[cnt * MousePktLen + 7];   /* Wheel Movement */
        cnt += 1;
        
        buf[0] = (buf[0]) & 0x07;   /*LSB 3 bits are considered for buttonpress*/
        buf[2] = 0 - buf[2];  /* flip Y & wheel because in read mouse event Y & wheel are fliped */
        buf[3] = 0 - buf[3];  

		nLen =  m_pUsb->ProcessRelMouseData(buf);
 
        if (nLen != 4)
            return -1;
        
        /* clear buffers for next set of iteration */
        buf[0] = buf[1] = buf[2] = buf[3] = 0;

    }
        return rc;
}

int CFPGATx::ProcessLocalKeyboard(unsigned char *pInKbBuf,
                                  int kbLen)
{
    int rc = 1;
    int rfblen;
    unsigned char rfbbuf[KB_BUFFER_SIZE * 6 / 4];

	if (m_pUsb == NULL || m_pClnt == NULL)
        return -1;
   
    rfblen = clnt_keyboard_data_to_rfb(m_pClnt,pInKbBuf, rfbbuf, kbLen);

    if (rfblen <= 0)
       return -1;

    for (int ind = 0 ; ind < rfblen; ind += 5) {
        unsigned int key = (rfbbuf[ind + 3] | ((rfbbuf[ind + 2] << 8) & 0xff00) |
                       ((rfbbuf[ind + 1] << 16) & 0xff0000) | ((rfbbuf[ ind ] << 24) & 0xf000000));

         unsigned char down = rfbbuf[ind + 4];
		 m_pUsb->ProcessRfbKeyboardData(key, down);
    }

    return rc;
}

int
CFPGATx::CreateRFBKbData(unsigned char *pInKbBuf,
        unsigned char *xkbbuf,
        unsigned char *pRFBOutBuf,
        int kbLen) {
    int i, total=0, len, rfblen;
    unsigned char *cp, rfbbuf[KB_BUFFER_SIZE * 6 / 4];

    if (m_pClnt == NULL)
        return 0;

    cp = pRFBOutBuf;
   // rfblen =  clnt_keyboard_data_to_rfb(m_pClnt, pInKbBuf, rfbbuf, kbLen);
    rfblen = 5;
    memcpy(rfbbuf, xkbbuf, 5);

    if (rfblen <= 0)
        return 0;

    /* Create RFB keyboard buffer */
    for (i = 0; i < rfblen; i += 5) {
        unsigned long key = GET_PROTOCOL_VALUE_LONG(&rfbbuf[i]);
        int down = rfbbuf[i + 4] ? 1 : 0;
        len = v2dLibCreateAbsoluteKeyboard(cp,
                                           m_pClnt->rfb_info.session_id, 
                                           key,
                                           down);
        cp += len;
        total += len;
    }
#ifdef KBMDEBUG
    SYSLOG_INFO("%s: returning %d bytes:\n", __func__, total);
    v2dLibDumpHex((unsigned char *) pRFBOutBuf, total);
#endif

    return total;
}

int
CFPGATx::ProcessRFBKbData(unsigned char *pInKbBuf, unsigned char *pXkbbuf, int kbLen)
{
    int nLen, i;

    if (!m_bEnableKBMS)
        return 0;

    nLen =  CreateRFBKbData(pInKbBuf, pXkbbuf, (unsigned char *) m_pSendBuffer, kbLen);

    if (nLen == 0)
        return 0;

    // Send RFB keyboard messages to server
#define KBABS_PACKET_LEN      21
    for (i = 0; i < nLen; i += KBABS_PACKET_LEN) {
        CV2DStreamMediaFlag v2dSMediaFlag;
        v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
        CSFrame * sFrame =
                new CSFrame(&m_pSendBuffer[i], nLen,
        //        new CSFrame(m_pSendBuffer + i, KBABS_PACKET_LEN,
                        v2dSMediaFlag.GetMediaFlag(),
                        m_nTimestamp);
        ASSERT(m_qQueueSink != NULL);
        m_qQueueSink->InsertFrame(sFrame);

    }

    return nLen;
}

int
CFPGATx::CreateRFBMsData(unsigned char *pInMsBuf,
        unsigned char *pRFBOutBuf,
        int msLen)
{
    int i, len, total, pktsize, x, y, wheel, press;
    unsigned char *cp, button, buttonmask, z;

    if (m_pClnt == NULL)
        return 0;

    cp = pRFBOutBuf;

    total = 0;

    pktsize = (pInMsBuf[0] == MS_ID_PS2) ? MS_PS2_PKTSIZE : MS_IMPS2_PKTSIZE;

    for (i = 0; i < msLen; i += pktsize) {
        button = pInMsBuf[i * pktsize + 1];
        if (pktsize == MS_PS2_PKTSIZE) {
            wheel = 0;
        }
        else {
            z = pInMsBuf[i * pktsize + 7];
            wheel = (z & 0x08) ? ((z & 0x07) - 8) : (z & 0x07);
        }
        x = m_pClnt->rfb_info.abs_x;
        if ((button & 0x10))                    // move left
            x -= (256 - pInMsBuf[i * pktsize + 3]);
        else                                    // move right
            x += pInMsBuf[i * pktsize + 3];
        m_pClnt->rfb_info.abs_x = (x < 0) ? 0 :
            (x > m_pClnt->rfb_info.size_x) ?  m_pClnt->rfb_info.size_x : x;

        y = m_pClnt->rfb_info.abs_y;
        if ((button & 0x20))                    // move down
            y += (256 - pInMsBuf[i * pktsize + 5]);
        else                                    // move up
            y -= pInMsBuf[i * pktsize + 5];
        m_pClnt->rfb_info.abs_y = (y < 0) ? 0 :
            (y > m_pClnt->rfb_info.size_y) ?  m_pClnt->rfb_info.size_y : y;

        buttonmask = 0;
        if (button & 0x01) buttonmask |= 0x01;     // left button
        if (button & 0x02) buttonmask |= 0x04;     // right button
        if (button & 0x04) buttonmask |= 0x02;     // middle button

        press = (wheel != 0) ? 1 : 0;

        /* Create the packet */
        do {
            if (press) {/* 4th & 5th bits are fliped because in read dev events the value for wheel is read reversely to correct this the wheel is fliped here before preparing v2d packet */
                if (wheel > 0)
                    buttonmask |= 0x10;            // wheel upward
                else if (wheel < 0)
                    buttonmask |= 0x08;            // wheel downward
                press = 0;
            }
            else if (wheel != 0) {
                buttonmask &= 0x07;                // release wheel button
                if (wheel > 0)
                    wheel--;
                else if (wheel < 0)
                    wheel++;
                press = 1;
            }

            len = v2dLibCreateAbsoluteMouse(cp,
                                            m_pClnt->rfb_info.session_id, 
                                            m_pClnt->rfb_info.abs_x,
                                            m_pClnt->rfb_info.abs_y,
                                            buttonmask);
            cp += len;
            total += len;
        } while (wheel != 0);
    }

#ifdef KBMDEBUG
    SYSLOG_INFO("%s: returning %d bytes:\n", __func__, total);
    v2dLibDumpHex((unsigned char *) pRFBOutBuf, total);
#endif

    return total;
}

int
CFPGATx::ProcessRFBMsData(unsigned char *pInMsBuf, int msLen)
{
    int nLen, i;

    if (!m_bEnableKBMS)
        return 0;

    nLen = CreateRFBMsData(pInMsBuf, (unsigned char *) m_pSendBuffer, msLen);

    if (nLen == 0)
        return 0;

    // Update local mouse cursor
    clnt_update_mouse_cursor_pos(m_pHal, m_pClnt);

    // Send RFB mouse messages to server
#define MSABS_PACKET_LEN    21
    for (i = 0; i < nLen; i += MSABS_PACKET_LEN) {
        CV2DStreamMediaFlag v2dSMediaFlag;
        v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
        CSFrame * sFrame =
                new CSFrame(&m_pSendBuffer[i], nLen,
                        v2dSMediaFlag.GetMediaFlag(),
                        m_nTimestamp);
        ASSERT(m_qQueueSink != NULL);
        m_qQueueSink->InsertFrame(sFrame);
    }

    return nLen;
}

int
CFPGATx::PopulateSerialBuffer(char *pData)
{
    int nLen = 0;
    nLen = ntohs(*(unsigned short *)pData);

    ASSERT(m_hSerial > 0);
    int rlen, len;
    unsigned char *cp;
    unsigned char buff[MAX_PROTOCOL_UNIT_SIZE];
    int unit_id;

    rlen = nLen;
    cp = (unsigned char *) (pData + 4);
    rlen -= 4;

    while (rlen > 0) {
        len = v2dLibGetProtocolUnit(buff, 0, MAX_PROTOCOL_UNIT_SIZE, &unit_id, cp);

        if (len == -1)
            break;
        switch (unit_id) {
        case SERDAT_IDENT_BASIC:
            if (write(m_hSerial, buff, len) < 0) {
                SYSLOG_ERROR("V2DFPGATX: Unable to write serial data");
            }
            break;
        default:
            SYSLOG_DEBUG("V2DFPGATX: Received Unknown Serial Protocol Unit: %d", unit_id);
            break;
        }
        rlen -= len + 4;
        cp += len + 4;
    }
    return 0;
}

int
CFPGATx::ProcessRFBStatus(char *pData)
{
    int nLen = 0;
    nLen = ntohs(*(unsigned short *)pData);

    int i = 4, plen, unit_id, status = 0;
    unsigned char *cp;
    unsigned long session_id = 0, query_id = 0;

    cp = (unsigned char *) pData + 4;

    while (i < nLen) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        if (plen == -1) {
            SYSLOG_DEBUG("V2DFPGATX: Malformed RFB Status Packet");
            break;
        }
        unit_id = GET_PROTOCOL_UNIT_ID(cp);
        switch (unit_id) {
        case RFBSTAT_IDENT_SESSIONID:
            session_id = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RFBSTAT_IDENT_QUERYID:
            query_id = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RFBSTAT_IDENT_STATUS:
            status = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        default:
            SYSLOG_INFO("V2DFPGATX: Unknown RFB status protocol Unit: %d",
                    unit_id);
            break;
        }
        // Increment pointers for next round
        i += plen;
        cp += plen;
    }

    m_nRFBStatusReplies++;

    // If reply is for us, we honor the status
    if (m_pClnt && session_id == m_pClnt->rfb_info.session_id &&
            query_id == m_pClnt->rfb_info.query_id) {
        m_bRFBStatus = status;
        SYSLOG_DEBUG("V2DFPGATX: RFB: Got reply %d for 0x%lx:0x%lx",
                status, session_id, query_id);

        clnt_set_mouse_cursor_enable(m_pHal, m_pClnt, m_bEnableKBMS ? status : 0);
    }

    // If message was broadcast, we query for the latest status
    else if (session_id == 0) {
        //m_pClnt->rfb_info.status &= ~RFB_STATUS_ENABLE;
        SendRFBStatusQuery();
    }

    return 0;
}

int
CFPGATx::SendRFBStatusQuery()
{
    int nLen;

    if (m_pClnt == NULL)
        return -1;

    m_pClnt->rfb_info.query_id++;

    nLen = v2dLibCreateRFBStatusQuery((unsigned char *) m_pSendBuffer,
        m_pClnt->rfb_info.session_id, m_pClnt->rfb_info.query_id);

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

    CSFrame * sFrame =
        new CSFrame(m_pSendBuffer, nLen,
                    v2dSMediaFlag.GetMediaFlag(),
                    m_nTimestamp);
    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);
    m_nRFBStatusQueries++;

    return 0;
}

int
CFPGATx::ClearRtpFilter()
{
    if (hal_get_hardware_type(m_pHal) == XPI_DEVICE) { 

        if (hal_clear_rtpfilter(m_pHal, RTPFILTER_INDEX_VIDEO) < 0) {
            SYSLOG_ERROR("V2DFPGATX: Failed to clear RTP filter");
        }

        if (hal_clear_rtpfilter(m_pHal, RTPFILTER_INDEX_AUDIO) < 0) {
            SYSLOG_ERROR("V2DFPGATX: Failed to clear RTP filter");
        }

        m_nAvoption = 0;
    }

    return 0;
}

int
CFPGATx::ProcessRtpFilter(char *pData)
{
    int retval = 0;
    RtpFilter_t filter;
    uint32_t ipaddr = 0;

    if (hal_get_hardware_type(m_pHal) != XPI_DEVICE ||
		(m_hVideo <= 0 && m_hAudio <= 0))
        return 0;

    memset((void *) &filter, 0, sizeof(struct RtpFilter));

    if (v2dLibParseRtpFilter(&filter, pData) < 0)
        return retval;

    if (filter.multicastaddr_set) {
        ipaddr = filter.multicastaddr;
    }
    else {
        char sIPAddress[33];
        if (((CNETutils *)NULL)->GetLocalIPAddress(sIPAddress, 32)) {
            ipaddr = htonl(inet_addr(sIPAddress));
        }
    }

    if (filter.video_set) {
        if (hal_set_rtpfilter(m_pHal, RTPFILTER_INDEX_VIDEO, 
                    ipaddr, filter.video_port, 
                    filter.video_aes ? RTPFILTER_FLAG_AES : 0) < 0)
            SYSLOG_ERROR("V2DFPGATX: Failed to write RTP filter");
        m_nAvoption = AVOPTION_VIDEO;
    }
    if (filter.audio_set) {
        if (hal_set_rtpfilter(m_pHal, RTPFILTER_INDEX_AUDIO, 
                    ipaddr, filter.audio_port, 
                    filter.audio_aes ? RTPFILTER_FLAG_AES : 0) < 0)
            SYSLOG_ERROR("V2DFPGATX: Failed to write RTP filter");
        m_nAvoption = (m_nAvoption == AVOPTION_VIDEO) ? AVOPTION_AUDIO_VIDEO :
            AVOPTION_AUDIO;
    }

    return 0;
}

int
CFPGATx::ProcessStream()
{
    // Process Alt stream first
    if (ProcessAltStream() > 0) {
        return 1;
    }

    int retval = 0;

    if (m_qQueueSource != NULL) {

        CSFrame *sFrame = m_qQueueSource->GetFrame();

        if (sFrame) {
            if (m_nTimestamp == 0) {
                HandleFirstTimestamp(sFrame->GetTimeStamp());
            }
            m_nTimestamp = sFrame->GetTimeStamp();
            m_nFrameWallClock = CTimeKeeper::Now();

            if (sFrame->GetMessageType() == CSFrame::Normal) {

                unsigned long nStreamType = sFrame->GetStreamType();

                if (nStreamType == STREAMTYPE_V2D) {
                    CV2DStreamMediaFlag v2dSMediaFlag(sFrame->GetMediaFlag());

                    m_bytesTransfered += sFrame->GetLength();
                    m_frameCount++;
                    switch (v2dSMediaFlag.GetChannel()) {
                        case FRAMETYPE_V2D_CHAN_CTL:
                            ProcessFrameTypeControl(sFrame);
                            break;
                        case FRAMETYPE_V2D_CHAN_VIDEO:
                            ProcessFrameTypeVideo(sFrame);
                            break;
                        case FRAMETYPE_V2D_CHAN_AUDIO:
                            ProcessFrameTypeAudio(sFrame);
                            break;
                        default:
                            SYSLOG_DEBUG("V2DFPGATX: Unexpected Frame "
                                    "MediaFlag: 0x%lx !!",
                                    sFrame->GetMediaFlag());
                            ASSERT(0);
                    }
                }
            }
            else {
                // Look for special marker frames
                if (sFrame->GetMessageType() == CSFrame::EndOfStream ||
                        sFrame->GetMessageType() == CSFrame::Disconnected) {

                    SYSLOG_INFO("V2DFPGATX: Upstream Stopped or Disconnected.");
                    SwitchSplashMode(true);
                    m_bRFBStatus = false;
                    if (m_pHal != NULL && !m_bXpiDevice) {
                        hal_LED_blink(m_pHal, CONNECTION_LED, LED_BLINK_FAST);
                    }
                    if (m_pClnt != NULL && m_pHal != NULL)
                        clnt_set_mouse_cursor_enable(m_pHal, m_pClnt, 0);
                    
                    ClearRtpFilter();
                }
                else if (sFrame->GetMessageType() == CSFrame::Connected) {
                    SYSLOG_INFO("V2DFPGATX: Upstream Connected.\n");
                    SwitchSplashMode(false);
                    if (m_pHal != NULL && !m_bXpiDevice) 
                        hal_LED_on(m_pHal, CONNECTION_LED);
                }
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
    }
    else {
        // If queue is no longer assigned, we have been stopped so go
        // back to splash mode.
        SwitchSplashMode(true);
        if (m_pHal != NULL && !m_bXpiDevice) 
            hal_LED_blink(m_pHal, CONNECTION_LED, LED_BLINK_FAST);
    }


    if (m_bSplashVideo) {
        CheckSplashMode();
        WriteSplashFrame();
    }

    CheckSyncStop();

    return retval;
}


void
CFPGATx::SwitchSplashMode(bool a_bSplashMode)
{
    // In errored state, do not do anything
    if (IsErrored()) {
        return;
    }

    if (m_bSplashVideo == false && a_bSplashMode == true) {
        m_bSplashRequested = true;
        m_tSplashReqTimestamp = CTimeKeeper::Now();
        m_bSplashVideo = true;
        m_pSplash->SetFSRefresh();
    }
    else if (a_bSplashMode == false) {
        m_bSplashRequested = false; // Cancel pending splash switch
        m_bSplashVideo = false;
        if (m_bXpiDevice) {
            SYSLOG_INFO("%s: Disabling Splash Video. ", __func__);
            hal_disable_splash_video(m_pHal);
        }
    }
}

// After switching to Splash Mode, we try to delay
// configuring Splash in the driver by a little bit.
// This avoids showing splash when we are switching between
// connections from one Tx to another. If both Tx's are
// have the same LUT, there is no unnecessary reconfiguring
// to splash LUT in between
void
CFPGATx::CheckSplashMode()
{
    if (m_bSplashRequested == false) 
        return;

    if (IsErrored()) {
        return;
    }

    // SplashRequested is true. Delay by two seconds.
    if (CTimeKeeper::Now() - m_tSplashReqTimestamp < 2000) {
        return;
    }

    // Need to configure splash mode. Set splash
    // parameters in the kernel if they are different
    // from the current LUT
    if (m_bXpiDevice) {
        struct LUT_struct lut;
        if (V2DUtils::PopulateLutFromScap(&lut, m_splashScap) != -1) { 
            int nSplashType = m_pSplash->GetSplashType();
            if (CompareAndSetScap(m_splashScap) == 1) {
                SYSLOG_DEBUG("Configuring Splash.");
                hal_enable_splash_video(m_pHal, nSplashType, &lut);
            }
            else {
                SYSLOG_DEBUG("Enabling Splash");
                hal_enable_splash_in_driver(m_pHal, nSplashType);
            }
        }
    }
    else {
        SYSLOG_DEBUG("Configuring Splash.");
        if (CompareAndSetScap(m_splashScap) == 1) {
            ScapIoctl(m_splashScap);
        } 
        else {
            SYSLOG_DEBUG("No Need to Configure Splash");
        }
    }
    m_bSplashRequested = false;
}

int
CFPGATx::ProcessAltStream()
{
    struct timeval timeout;
    fd_set read_fdset;
    int nfds = 0;
    int retval = 0;
    struct list_head *tmp;
    struct event_s *evententryp;

    // In errored state, do not do anything
    if (IsErrored()) {
        return 0;
    }

    // Don't do anything till we are started
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

    // Process KB, MS, Serial and USB Event data
    FD_ZERO(&read_fdset);
    if (m_hKeyboard > 0)
        FD_SET(m_hKeyboard, &read_fdset);
    if (m_hMouse > 0)
        FD_SET(m_hMouse, &read_fdset);
    if (m_hSerial > 0)
        FD_SET(m_hSerial, &read_fdset);
	if (m_pUsb != NULL && m_pUsb->GetKeyboardFD() > 0)
        FD_SET(m_pUsb->GetKeyboardFD(), &read_fdset);

    if (m_hUsbEventHead != NULL) {
        list_for_each(tmp, m_hUsbEventHead) {
            evententryp = list_entry(tmp, struct event_s, event_list);
            if (evententryp->event_fd > 0) {
                FD_SET(evententryp->event_fd, &read_fdset);
            }
        }
    }

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
      if (!m_bXpiDevice) {

          if (FD_ISSET(m_hKeyboard, &read_fdset)) {
             ProcessKeyboardFrame();
             retval = 1;
          }
          if (FD_ISSET(m_hMouse, &read_fdset)) {
             ProcessMouseFrame();
             retval = 1;
          }
          if ((m_hSerial > 0) && FD_ISSET(m_hSerial, &read_fdset)) {
             ProcessSerialFrame();
             retval = 1;
          }
     }
	  if (m_pUsb != NULL && m_pUsb->GetKeyboardFD() > 0) {
        if (FD_ISSET(m_pUsb->GetKeyboardFD(),&read_fdset))
           m_pUsb->SetKeyboardLEDsStatus();
     } 
     if (m_hUsbEventHead != NULL && !list_empty(m_hUsbEventHead)) {
            list_for_each(tmp, m_hUsbEventHead) {
                evententryp = list_entry(tmp, struct event_s, event_list);
                if (FD_ISSET(evententryp->event_fd, &read_fdset)) {
                    ProcessUsbEventFrame(evententryp);
                }
            }
        }
    }

    if (m_tLastMouseReceivedTimestamp && m_bLocalMousePointerEnable )
        CheckMouseActive();

    return retval;
}

void CFPGATx::CheckMouseActive()
{
    
    if ((unsigned long)(m_nTimestamp - m_tLastMouseReceivedTimestamp) > (unsigned long) (m_nLocalMousePointerIdleTime * 1000)) {
        m_pClnt->mouse_active = 0;
        //clnt_update_mouse_cursor_pos(m_pHal, m_pClnt);
        clnt_disable_mouse_pointer(m_pHal, m_pClnt);
        m_tLastMouseReceivedTimestamp  = 0;

     } 

}

void
CFPGATx::PrintStatistics(string &a_stats)
{
    ReloadScapIoctl();

    // TODO: Need to move this out to a better place
    // Currently using the fact that this is called once a sec.
    if (m_pHal != NULL) {
        if (m_nRetryDropCount > 0 && m_pHal != NULL) {
            SYSLOG_INFO("V2DFPGATX: Retrying Drop Side Configuration: %d",
                    m_nRetryDropCount);
            if (hal_resetDropsideVideo(m_pHal) < 0) {
                // Failed, another retry needed
                m_nRetryDropCount--;
            }
            else {
                m_nRetryDropCount = 0;
            }
        }
        // Sync (with the other board) whether keyboard is in console mode
        hal_update_virtual_console(m_pHal);
    }
}


int CFPGATx::UpdateEnableKBMS(CCommand *a_pCmd)
{
    int retval = 0;
    int enablekbm;

    if (a_pCmd->GetIntVar("enablekbm", &enablekbm) != -1) {
        m_bEnableKBMS = enablekbm;

        if (m_pHal != NULL && m_pClnt != NULL) {
            clnt_set_mouse_cursor_enable(m_pHal, m_pClnt, m_bEnableKBMS ? m_bRFBStatus : 0);
        }

        retval = 1;
    }
    return retval;

}

int
CFPGATx::UpdateKeyboardMode(CCommand *a_pCmd)
{
    int retval = 0;
    int nkbmode;

    if (a_pCmd->GetIntVar("kbmode", &nkbmode) != -1) {
        if (m_pClnt != NULL && m_pClnt->curr_kbmode != nkbmode) {
            clnt_set_keyboard_mode(m_pHal, m_pClnt, nkbmode);
            clnt_update_mouse_cursor_display(m_pHal, m_pClnt);
        }
        retval = 1;
    }
    return retval;
}

int
CFPGATx::UpdateKeyboardEscapeSequences(CCommand *a_pCmd)
{
    int retval = 0;
    int nesc1, nesc2, nesc3, nesc4;
    bool bEscape = false;

    if (m_pClnt == NULL)
        return 0;

    if (a_pCmd->GetIntVar("consoleesc1", &nesc1) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc1 = m_pClnt->console_escape_keycode[0];
    }
    if (a_pCmd->GetIntVar("consoleesc2", &nesc2) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc2 = m_pClnt->console_escape_keycode[1];
    }
    if (a_pCmd->GetIntVar("consoleesc3", &nesc3) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc3 = m_pClnt->console_escape_keycode[2];
    }
    if (a_pCmd->GetIntVar("consoleesc4", &nesc4) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc4 = m_pClnt->console_escape_keycode[3];
    }
    if (bEscape) {
        clnt_set_console_escape(m_pHal, m_pClnt, nesc1, nesc2, nesc3, nesc4);
    }

    bEscape = false;
    if (a_pCmd->GetIntVar("localesc1", &nesc1) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc1 = m_pClnt->local_escape_keycode[0];
    }
    if (a_pCmd->GetIntVar("localesc2", &nesc2) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc2 = m_pClnt->local_escape_keycode[1];
    }
    if (a_pCmd->GetIntVar("localesc3", &nesc3) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc3 = m_pClnt->local_escape_keycode[2];
    }
    if (a_pCmd->GetIntVar("localesc4", &nesc4) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc4 = m_pClnt->local_escape_keycode[3];
    }
    if (bEscape) {
        clnt_set_local_escape(m_pHal, m_pClnt, nesc1, nesc2, nesc3, nesc4);
    }

    bEscape = false;
    if (a_pCmd->GetIntVar("remoteesc1", &nesc1) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc1 = m_pClnt->remote_escape_keycode[0];
    }
    if (a_pCmd->GetIntVar("remoteesc2", &nesc2) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc2 = m_pClnt->remote_escape_keycode[1];
    }
    if (a_pCmd->GetIntVar("remoteesc3", &nesc3) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc3 = m_pClnt->remote_escape_keycode[2];
    }
    if (a_pCmd->GetIntVar("remoteesc4", &nesc4) != -1) {
        bEscape = true;
        retval = 1;
    }
    else {
        nesc4 = m_pClnt->remote_escape_keycode[3];
    }
    if (bEscape) {
        clnt_set_remote_escape(m_pHal, m_pClnt, nesc1, nesc2, nesc3, nesc4);
    }

    return retval;
}

int
CFPGATx::UpdateKBMTypes(CCommand *a_pCmd)
{
    int retval = 0;
    int nkbtype, nmstype;

    if (m_pClnt == NULL)
        return 0;

    if (a_pCmd->GetIntVar("localkbtype", &nkbtype) != -1) {
        clnt_set_local_keyboard_type(m_pHal, m_pClnt, nkbtype);
        retval = 1;
    }

    if (a_pCmd->GetIntVar("localmstype", &nmstype) != -1) {
        clnt_set_local_mouse_type(m_pHal, m_pClnt, nmstype);
        retval = 1;
    }

    return retval;
}

int
CFPGATx::UpdateOverrideParameters(CCommand *a_pCmd)
{
    int retval = 0, nval;

    if (a_pCmd->GetIntVar("overrideoption", &nval) != -1 ||
            a_pCmd->GetIntVar("overridecolor", &nval) != -1 ||
            a_pCmd->GetIntVar("overridekeepaspect", &nval) != 1) {
        clnt_set_override_parameters(m_pHal, m_pClnt);
        retval = 1;
    }

    return retval;
}

int
CFPGATx::UpdateRFBDisplayParameters(CCommand *a_pCmd)
{
    int retval = 0, ndummy;

    if (a_pCmd->GetIntVar("rfbmousecursor", &ndummy) != -1) {
        clnt_update_mouse_cursor_pos(m_pHal, m_pClnt);
        retval = 1;
    }
    if (a_pCmd->GetIntVar("rfbrefresh", &ndummy) != -1) {
        clnt_update_mouse_cursor_display(m_pHal, m_pClnt);
        retval = 1;
    }

    return retval;
}

int
CFPGATx::UpdateUSBDevices(CCommand *a_pCmd)
{
    int retval = 0;
    int ndevnum;
    bool bHotplug = false;
    char sDevice[MAXDEVLEN];

    if (a_pCmd->GetIntVar("addeventdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/event%d", ndevnum);
        if (hal_add_event_device(m_pHal, sDevice) > 0) {
            bHotplug = true;
        }
        retval = 1;
    }
    if (a_pCmd->GetIntVar("removeeventdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/event%d", ndevnum);
        if (hal_remove_event_device(m_pHal, sDevice) > 0) {
            bHotplug = true;
        }
        retval = 1;
    }
    if (a_pCmd->GetIntVar("addserialdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/ttyUSB%d", ndevnum);
        if (hal_add_serial_device(m_pHal, sDevice) > 0) {
            bHotplug = true;
            InitSerialConfig();
        }
        retval = 1;
    }
    if (a_pCmd->GetIntVar("removeserialdevice", &ndevnum) != -1) {
        snprintf(sDevice, sizeof(sDevice) - 1, "/dev/ttyUSB%d", ndevnum);
        if (hal_remove_serial_device(m_pHal, sDevice) > 0) {
            bHotplug = true;
            InitSerialConfig();
        }
        retval = 1;
    }

    if (bHotplug)
        m_nMaxfd = GetMaxfd(m_hKeyboard, m_hMouse, m_hSerial, m_hUsbEventHead);

    return retval;
}

int 
CFPGATx::UpdateKeyboardLayout(CCommand *a_pCmd)
{

	int nLayout;
	char kblayout[60];

	if (a_pCmd->GetIntVar("kblayout", &nLayout) != -1) 
	{
		if (a_pCmd->GetVar("kblanguage", kblayout, 60) != -1)
		{
			clnt_update_xkb_kb_layout(m_pHal, m_pClnt);
		}
	}

	return 1;

}

int CFPGATx::UpdateLocalMousePointerParameters(CCommand *a_pCmd)
{
    int mouseinput;
    int timeout;
    int retval = 0;

    if (a_pCmd->GetIntVar("MousePointer", &mouseinput) != -1)  { 
      m_bLocalMousePointerEnable = mouseinput;
      m_pClnt->local_mouse_pointer_enable = mouseinput;
      retval = 1;
    }
    if (a_pCmd->GetIntVar("Timeout", &timeout) != -1) {
      m_nLocalMousePointerIdleTime = timeout;
      retval = 1;
    }

    return retval;

}


int
CFPGATx::Update(CCommand *a_pCmd)
{
    int retval = 0;

    if (UpdateEnableKBMS(a_pCmd))
        retval = 1;

    if (UpdateSplash(a_pCmd))
        retval = 1;

    if (UpdateKeyboardMode(a_pCmd))
        retval = 1;

    if (UpdateKeyboardEscapeSequences(a_pCmd))
        retval = 1;

    if (UpdateKBMTypes(a_pCmd))
        retval = 1;

    if (UpdateUSBDevices(a_pCmd))
        retval = 1;

    if (UpdateOverrideParameters(a_pCmd))
        retval = 1;

    if (UpdateRFBDisplayParameters(a_pCmd))
        retval = 1;

    if (UpdateKeyboardLayout(a_pCmd))
        retval = 1;

     if (UpdateLocalMousePointerParameters(a_pCmd))
         retval = 1;


    return retval;
}

void
CFPGATx::UpdateCustomStatus(CCommand *a_pCmd)
{
    char sKbMode[32];

    a_pCmd->SetObjectParam("FPGAPort", m_nIOPort);
    a_pCmd->SetObjectCounterParam("ControlPackets", m_nControlPackets);
    a_pCmd->SetObjectCounterParam("VideoPackets", m_nVideoPackets);
    a_pCmd->SetObjectCounterParam("AudioPackets", m_nAudioPackets);
    a_pCmd->SetObjectCounterParam("KeyboardInPackets", m_nKbInPackets);
    a_pCmd->SetObjectByteCounterParam("KeyboardInBytes", m_nKbInBytes);
    a_pCmd->SetObjectCounterParam("MouseInPackets", m_nMsInPackets);
    a_pCmd->SetObjectByteCounterParam("MouseInBytes", m_nMsInBytes);
    a_pCmd->SetObjectCounterParam("ScapReceived", m_nScapCount);
    a_pCmd->SetObjectCounterParam("VideoChanged", m_nVideoChanged);
    a_pCmd->SetObjectCounterParam("UsbKbInBytes", m_nUsbKbInBytes);
    a_pCmd->SetObjectCounterParam("UsbKbInPackets", m_nUsbKbInPackets);
    a_pCmd->SetObjectByteCounterParam("UsbMsInBytes", m_nUsbMsInBytes);
    a_pCmd->SetObjectByteCounterParam("UsbMsInPackets", m_nUsbMsInPackets);
    a_pCmd->SetObjectCounterParam("SerialInPackets", m_nSerInPackets);
    a_pCmd->SetObjectByteCounterParam("SerialInBytes", m_nSerInBytes);
    a_pCmd->SetObjectCounterParam("SerialOutPackets", m_nSerOutPackets);
    a_pCmd->SetObjectCounterParam("SerialOutBytes", m_nSerOutBytes);
    a_pCmd->SetObjectCounterParam("VideoPacketsWritten", m_nVideoPacketsWritten);
    a_pCmd->SetObjectByteCounterParam("VideoBytesWritten", m_nVideoBytesWritten);
    a_pCmd->SetObjectCounterParam("AudioPacketsWritten", m_nAudioPacketsWritten);
    a_pCmd->SetObjectByteCounterParam("AudioBytesWritten", m_nAudioBytesWritten);
    a_pCmd->SetObjectParam("HorizontalResolution", m_scapData->m_hRes);
    a_pCmd->SetObjectParam("VerticalResolution", m_scapData->m_vRes);
    a_pCmd->SetObjectParam("VideoName", (const char *)m_scapData->m_videoName);
    a_pCmd->SetObjectParam("SplashMode", m_bSplashVideo);
    a_pCmd->SetObjectParam("SerialStatus",(m_hSerial > 0) ? true : false);
    if (m_pClnt)
        a_pCmd->SetObjectParam("LocalCursor",
                (m_pClnt->rfb_info.status & RFB_STATUS_ENABLE) ? true : false);
    else
        a_pCmd->SetObjectParam("LocalCursor", false);
    if (m_pHal != NULL && m_pHal->console_info.kb_active)
        a_pCmd->SetObjectParam("KeyboardMode",
                (char *)clnt_get_keyboard_mode(m_pHal, m_pClnt, sKbMode));
    else
        a_pCmd->SetObjectParam("KeyboardMode", "Console");
    a_pCmd->SetObjectParam("KBMEnabled", m_bEnableKBMS);
    a_pCmd->SetObjectParam("RFBStatus", m_bRFBStatus);
}

