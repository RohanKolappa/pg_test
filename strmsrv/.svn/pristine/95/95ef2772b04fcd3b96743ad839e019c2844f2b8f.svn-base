/*
 * splash.cpp
 *
 *  Created on: Oct 18, 2009
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include <sys/stat.h>
#include "splash.hh"
#include "common.hh"
#include "v2o_lib.h"

CSplash::CSplash(HAL *hp, bool a_bIsTx)
{
    m_bReady = false;

    m_bIsTx = a_bIsTx;
    m_nChannel = hal_get_channel_number(hp);
    m_bIsXpi = (hal_get_hardware_type(hp) == XPI_DEVICE) ? true : false;
    m_nLRFlag = 0;
    m_pVideoSplashLeft = NULL;
    m_pVideoSplashRight = NULL;

    if (m_bIsTx)
        InitTxSplash(hp);
    else
        InitRxSplash(hp);

    // Start thread to create splash in background
    m_priority = 0;      // Set to non-realtime priority
    m_bDetached = false; // Wait till thread is done before we can destroy object
    sprintf(m_pThreadName, "SplashThread_%s_%d", m_bIsTx ? "TX" : "Rx",
            m_nChannel);
    Start();
}

CSplash::~CSplash()
{
    // Stop the thread
    Stop(); // Wait till worker thread done

    if (m_pVideoSplashLeft != NULL)
        delete m_pVideoSplashLeft;

    if (m_pVideoSplashRight != NULL)
        delete m_pVideoSplashRight;
}

void CSplash::InitTxSplash(HAL *hp)
{
    struct tx_splash_config cfg;
    memset(&cfg, 0x0, sizeof(cfg));
    get_tx_splash_config(hp, &cfg);

    m_nType = SPLASH_TYPE_TX | cfg.tx_splash;

    m_bEnable = cfg.enable_splash;
    m_nHRes = cfg.splash_hres;
    m_nVRes = cfg.splash_vres;
    m_nOrigHRes = cfg.splash_orig_hres;
    m_nOrigVRes = cfg.splash_orig_vres;

    m_dRefresh = cfg.splash_refresh;
    m_dPCR = cfg.splash_pcr;
    m_nHfp = cfg.splash_hfp;
    m_nHbp = cfg.splash_hbp;
    m_nHSync = cfg.splash_hsync;
    m_nVfp = cfg.splash_vfp;
    m_nVbp = cfg.splash_vbp;
    m_nVSync = cfg.splash_vsync;

    m_nRate = cfg.splash_rate;
    m_nMQuant = cfg.splash_mquant;
    m_nDCQuant = cfg.splash_dcquant;
    m_nSliceSize = cfg.splash_slicesize;
    //m_nCodecVersion =  cfg.splash_codecversion;   // hal_get_codec_version(hp);
    m_nCodecVersion = CODEC_VERSION_STANDARD;
    m_bStereo = (cfg.splash_stereo == 0) ? false : true;
}

void CSplash::InitRxSplash(HAL *hp)
{
    struct rx_splash_config cfg;
    memset(&cfg, 0x0, sizeof(cfg));
    get_rx_splash_config(hp, &cfg);

    m_nType = cfg.rx_splash;

    m_bEnable = cfg.enable_splash;
    m_nHRes = cfg.splash_hres;
    m_nVRes = cfg.splash_vres;
    m_nOrigHRes = cfg.splash_orig_hres;
    m_nOrigVRes = cfg.splash_orig_vres;

    m_dRefresh = cfg.splash_refresh;
    m_dPCR = cfg.splash_pcr;
    m_nHfp = cfg.splash_hfp;
    m_nHbp = cfg.splash_hbp;
    m_nHSync = cfg.splash_hsync;
    m_nVfp = cfg.splash_vfp;
    m_nVbp = cfg.splash_vbp;
    m_nVSync = cfg.splash_vsync;

    m_nRate = 0;
    m_nMQuant = SPLASH_MQUANT;
    m_nDCQuant = SPLASH_DCQUANT;
    m_nSliceSize = cfg.splash_slicesize;
    //m_nCodecVersion = hal_get_codec_version(hp);
    m_nCodecVersion = CODEC_VERSION_STANDARD;
    m_bStereo = (cfg.splash_stereo == 0) ? false : true;
}

int CSplash::GetLUT(struct LUT_struct *a_LUT, int & a_nMQuant,
        int & a_nDCQuant, int & a_nSliceSize, int & a_nCompMode,
		int & a_nMotionCompMode)
{

    memset(a_LUT, 0x00, sizeof(LUT_struct));
    a_LUT->a_hres = m_nHRes;
    a_LUT->a_vres = m_nVRes;
    a_LUT->a_orig_hres = m_nOrigHRes;
    a_LUT->a_orig_vres = m_nOrigVRes;
    a_LUT->a_refresh = m_dRefresh;
    a_LUT->a_pcr = m_dPCR;
    a_LUT->a_hfp = m_nHfp;
    a_LUT->a_hbp = m_nHbp;
    a_LUT->a_hsync = m_nHSync;
    a_LUT->a_vfp = m_nVfp;
    a_LUT->a_vbp = m_nVbp;
    a_LUT->a_vsync = m_nVSync;
    a_LUT->a_alutreg = 0xaa;
    a_LUT->a_stereo = (m_bStereo) ? 1 : 0;
    a_LUT->a_noise = 0x0f09;    /* Default noise elim: coarse 10, fine 16 */

    if (m_bEnable)
        sprintf(a_LUT->comment, "Server Splash Screen %dx%d@%4.1f%s",
                a_LUT->a_hres, a_LUT->a_vres, a_LUT->a_refresh,
                (a_LUT->a_stereo) ? "s" : "");
    else
        sprintf(a_LUT->comment, "Server Splash Screen (Disabled)");

    a_nMQuant = m_nMQuant;
    a_nDCQuant = m_nDCQuant;
    a_nSliceSize = m_nSliceSize;
    a_nCompMode = 0;
    a_nMotionCompMode = 0;

    return 0;
}

int CSplash::GetSplashType()
{
    return m_nType;
}

int CSplash::GetSCAP(ScapData_t *scap)
{

    scap->m_videoNameSet = 1;
    scap->m_hResSet = 1;
    scap->m_vResSet = 1;
    scap->m_videoPcrSet = 1;
    scap->m_videoRefreshSet = 1;
    scap->m_videoHfpSet = 1;
    scap->m_videoHbpSet = 1;
    scap->m_videoHsyncSet = 1;
    scap->m_videoVfpSet = 1;
    scap->m_videoVbpSet = 1;
    scap->m_videoVsyncSet = 1;
    scap->m_sliceSizeSet = 1;
    scap->m_videoCompmodeSet = 1;

    scap->m_hRes = m_nHRes;
    scap->m_vRes = m_nVRes;
    scap->m_videoPcr = m_dPCR;
    scap->m_videoRefresh = m_dRefresh;
    scap->m_videoHfp = m_nHfp;
    scap->m_videoHbp = m_nHbp;
    scap->m_videoHsync = m_nHSync;
    scap->m_videoVfp = m_nVfp;
    scap->m_videoVbp = m_nVbp;
    scap->m_videoVsync = m_nVSync;
    scap->m_videoRefresh = m_dRefresh;
    scap->m_sliceSize = m_nSliceSize;
    scap->m_videoCompmode = 0;
    scap->m_videoStereo = (m_bStereo) ? 1 : 0;
    scap->m_videoCodecVersion  = m_nCodecVersion;
    scap->m_origHresSet = 1;
    scap->m_origHres = m_nOrigHRes;

    scap->m_origVresSet = 1;
    scap->m_origVres = m_nOrigVRes;

    sprintf(scap->m_videoName, "Rx Splash Screen %dx%d@%4.1f%s",
                m_nOrigHRes, m_nOrigVRes, m_dRefresh, (m_bStereo) ? "s" : "");

    return 0;
}

int CSplash::GetSplashBandwidth()
{
    return m_nRate;
}

bool CSplash::IsReady()
{
    return m_bReady;
}

void CSplash::SetFSRefresh()
{
    if (m_pVideoSplashLeft != NULL)
        m_pVideoSplashLeft->SetFSRefresh();

    if (m_pVideoSplashRight != NULL)
        m_pVideoSplashRight->SetFSRefresh();
}

int CSplash::GetSplashPacket(char * a_buffer)
{
    int pktlen;
    bool eof;

    if (!m_bReady)
        return 0;

    if (!m_bEnable)
        return 0;

    if (m_bStereo && m_nLRFlag == 1 && m_pVideoSplashRight != NULL) {
        pktlen = m_pVideoSplashRight->GetMotionPacket(a_buffer, &eof);
        if (eof) {
            //m_pVideoSplashRight->SetFSRefresh();
            if (m_pVideoSplashRight->GetNextFrame() != 0) {
                m_nLRFlag = 0;
            }
        }
    }
    else {
        pktlen = m_pVideoSplashLeft->GetMotionPacket(a_buffer, &eof);
        if (eof) {
            //m_pVideoSplashLeft->SetFSRefresh();
            if (m_pVideoSplashLeft->GetNextFrame() != 0) {
                if (m_bStereo) {
                    m_nLRFlag = 1;
                }
            }
        }
    }
    if (pktlen == 0)
        return 0;

    return pktlen;
}

// Trigger animated splash if a special file exists
bool CSplash::CheckAnimationParameters()
{
    struct stat fileStat;
    if (stat("/etc/config/animatesplash", &fileStat) == 0)
        return true;
    return false;
}

// Determine if we need to animate.
// No point creating new frames unless someone is actively
// retrieving them -- saves CPU cycles.
bool CSplash::Animate() {
    bool retval = false;

    retval = m_pVideoSplashLeft->Animate();

    if (retval && m_bStereo)
        retval = m_pVideoSplashRight->Animate();

    return retval;
}

int CSplash::Run() {
    bool bAnimate;

    // Delay creation of splash frame by 1/2 a sec.
    Sleep(500);
    if (m_bIsXpi || m_bExit) {
        SYSLOG_DEBUG("No need to create splash frame. We are scheduled for destruction.");
        return 0;
    }

    SYSLOG_DEBUG("Creating SPLASH frame... ");

    bAnimate = CheckAnimationParameters();
    int compression = 2;
    if (bAnimate)
        compression = 7;

    unsigned short nMQuant = mquant_from_compression(compression);
    unsigned short nDCQuant = dcquant_from_compression(compression);

    m_pVideoSplashLeft = new CVideoSplashFrame(m_nHRes, m_nVRes,
            nMQuant, nDCQuant, m_nSliceSize, m_nType, m_nCodecVersion,
            m_bStereo, 0, bAnimate);

    if (m_bStereo) {
        m_pVideoSplashRight = new CVideoSplashFrame(m_nHRes, m_nVRes,
            nMQuant, nDCQuant, m_nSliceSize,
            m_nType, m_nCodecVersion,  m_bStereo, 1, bAnimate);
    }

    // Create first image
    if (m_pVideoSplashLeft != NULL) {
        m_pVideoSplashLeft->EncodeFirstFrame();
    }

    if (m_pVideoSplashRight != NULL) {
        m_pVideoSplashRight->EncodeFirstFrame();
    }

    m_bReady = true;

    // Create new frames
    while (!m_bExit) {

        Sleep(83);      //about 12 fps

        if (!Animate())
            continue;

        if (m_pVideoSplashLeft != NULL) {
            if (bAnimate)
                m_pVideoSplashLeft->MoveRectangle();
            m_pVideoSplashLeft->EncodeFrame();
        }

        if (m_pVideoSplashRight != NULL) {
            if (bAnimate)
                m_pVideoSplashRight->MoveRectangle();
            m_pVideoSplashRight->EncodeFrame();
        }
    }

    SYSLOG_INFO("Exiting Motion Thread");

    return 0;
}
