/*
 * winfbrx.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

#include <sstream>

#include "common.hh"
#include "winfbrx.hh"
#include "winscreenscraper.hh"
#include "wineventhandler.hh"
#include "geometry.hh"
#include "ratecontroller.hh"
#include "meter.hh"
#include "shaper.hh"

using namespace V2DLib;

CWinFBRx::CWinFBRx(int nId, int compression, int codecversion) :
    CFBRx(nId, compression, codecversion)
{
    m_nPortType = WINFBENCODERPORT;

    m_bEnableScraping = false;
    m_bScheduleRFBReady = false;
    m_nGrabCount = 0;
    m_nGrabErrors = 0;
    m_nGrabFrameRate = 16;
    m_nConnectCount = 0;
    m_nRequestBandwidth = 0;

    m_pScraper = new CWindowsScreenScraper(this);
    m_pEventHandler = new CWindowsEventHandler();
    m_pGrabRateController = new CRateController(1);
    CreateEncoderThread();

    InitializeVideo();

}

CWinFBRx::~CWinFBRx()
{
    delete m_pScraper;
    delete m_pEventHandler;
    delete m_pGrabRateController;
}

int CWinFBRx::ProcessFrameBuffers()
{
    ProcessScreenUpdates();

    if (ProcessEncodedPackets() == 1)
        return 1;

    return 0;
}

void CWinFBRx::OnResolutionChange()
{
    InitializeVideo();
}

void CWinFBRx::ProcessAVRequest(AVReqData_t * avReqData)
{
    m_bScheduleRFBReady = true;
    if (avReqData->m_burstRatioSet && avReqData->m_burstDurationSet
            && avReqData->m_videoRateSet) {

        // Set bandwidth
        m_nRequestBandwidth = avReqData->m_videoRate * 8;

        SYSLOG_DEBUG("WINFB: RequestedBandwidth %u\n",
                m_nRequestBandwidth);
        SetBandwidth();
    }
}

void CWinFBRx::SetBandwidth()
{
    int nBandwidth = m_nBandwidth;
    if (m_nRequestBandwidth > 0 && m_nRequestBandwidth < m_nBandwidth)
        nBandwidth = m_nRequestBandwidth;
    m_pShaper->SetBandwidth(nBandwidth);
}

void CWinFBRx::HandleDownstreamConnected()
{
    m_nConnectCount++;
    if (m_nConnectCount > 0) {
        m_bEnableScraping = true;
        m_pGrabRateController->SetRate(m_nGrabFrameRate);
    }
    m_bScheduleRFBReady = true;
}

void CWinFBRx::HandleDownstreamDisconnected()
{
    m_nConnectCount--;
    if (m_nConnectCount <= 0) {
        m_pGrabRateController->SetRate(1);
        m_bEnableScraping = false;
        m_nConnectCount = 0;
        m_nRequestBandwidth = m_nBandwidth;
        SetBandwidth();
    }
}


void CWinFBRx::ProcessRFBQuery(char *pData)
{
    uint32_t sessionId = 0;
    uint32_t queryId = 0;

    int rc = v2dLibParseRFBStatusQuery((uint8_t *)pData, &sessionId, &queryId);

    if (rc != -1 && sessionId != 0) {
        SYSLOG_DEBUG("RFBHANDLER: RFB: Received query from %u:%u",
                sessionId, queryId);
        SendRFBReady(sessionId, queryId);
    }
}


void CWinFBRx::RequestReady()
{
    if (m_bScheduleRFBReady == true) {
        SendRFBReady();
        m_bScheduleRFBReady = false;
    }
}

void CWinFBRx::ProcessKeyboard(char *pData)
{
    uint32_t key;
    uint8_t down;
    uint32_t sessionId;

    int rc = v2dLibParseAbsoluteKeyboard((uint8_t *)pData, &key, &down, &sessionId);
    if (rc > 0) {
        m_pEventHandler->SendKeyboardEvent(key, down);
    }
}

void CWinFBRx::ProcessMouse(char *pData)
{
    uint16_t x, y;
    uint8_t buttonmask;
    uint32_t sessionId;

    int rc = v2dLibParseAbsoluteMouse((uint8_t *)pData, &x, &y, &buttonmask, &sessionId);
    if (rc > 0) {
        //SYSLOG_INFO("Process Mouse %d %d", x, y);
        m_pEventHandler->SendMouseEvent(Point(x, y), buttonmask);
    }
}

int CWinFBRx::SendRFBReady(uint32_t a_nSessionId, uint32_t a_nQueryId)
{
    unsigned char buffer[SEND_BUFFER_SIZE];
    int nLen = 0;

    SYSLOG_DEBUG("WINFBRX: RFB: Sending status message %d to %u:%u",
            1, a_nSessionId, a_nQueryId);
    nLen = v2dLibCreateRFBStatus(buffer, 1, a_nSessionId, a_nQueryId);

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

    CSFrame * sFrame = new CSFrame((char *) buffer, nLen,
            v2dSMediaFlag.GetMediaFlag(), m_nTimestamp);

    m_qQueueSink->InsertFrame(sFrame);
    m_nCtlOutPackets++;
    m_nCtlOutBytes += nLen;

    return nLen;

}

void CWinFBRx::InitializeVideo()
{
    // Send the latest video parameters to our encoder
    //m_FrameDim.SetDim(m_pScraper->Width(), m_pScraper->Height());
    m_FrameDim = m_pScraper->FrameDimension();

    SYSLOG_INFO("New Frame Dimension %d %d", m_FrameDim.width, m_FrameDim.height);
    CNewVideoResolutionInfo *encoderInfo =
            new CNewVideoResolutionInfo(m_FrameDim.width, m_FrameDim.height,
                    m_pScraper->GetPixelFormat());
    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(encoderInfo);
    m_qEncoderSink->InsertFrame(pSFrame);

    // Setup KBM event handler
    m_pEventHandler->SetFrameDimensions(m_FrameDim);

}


void CWinFBRx::ProcessScreenUpdates()
{

    // Ensure previous requests have all been handled by the encoder
    if (m_qEncoderSink->Size() > 0) {
        m_nEncoderThrottled++;
        return;
    }

    // Limit the scraping/encoding frame rate
    if (!m_pGrabRateController->Allow())
        return;

    if (!m_pMeter->Allow())
        return;

    m_nGrabCount++;

    if (!m_pScraper->Grab()) {
        m_nGrabErrors++;
        return;
    }

    CEncoderInfo *encoderInfo = new CFrameBufferUpdate((uint8_t *) m_pScraper->GetBuffer());
    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(encoderInfo);
    m_qEncoderSink->InsertFrame(pSFrame);
}



int CWinFBRx::CustomUpdate(CCommand *a_pCmd)
{
    int retval = 0;
    int nGrabFrameRate;
    if (a_pCmd->GetIntVar("grabframerate", &nGrabFrameRate) != -1) {
        if (nGrabFrameRate < 1)
            nGrabFrameRate = 1;
        if (nGrabFrameRate > 100)
            nGrabFrameRate = 100;
        m_nGrabFrameRate = nGrabFrameRate;
        if (m_bEnableScraping)
            m_pGrabRateController->SetRate(nGrabFrameRate);
        retval = 1;
    }

    return retval;
}

void CWinFBRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    // Update parent class status first
    CFBRx::UpdateCustomStatus(a_pCmd);

    a_pCmd->SetObjectBitrateGuageParam("RequestBandwidth", m_nRequestBandwidth);
    a_pCmd->SetObjectParam("Active", m_bEnableScraping);
    if (a_pCmd->StatusLevel(2))
        a_pCmd->SetObjectParam("ActiveCount", m_nConnectCount);
    a_pCmd->SetObjectParam("FrameWidth", m_FrameDim.width);
    a_pCmd->SetObjectParam("FrameHeight", m_FrameDim.height);
    a_pCmd->SetObjectConstantParam("WindowGrabRate", m_pGrabRateController->GetRate());
    a_pCmd->SetObjectCounterParam("WindowGrabCount", m_nGrabCount);
    a_pCmd->SetObjectErrorCounterParam("GrabsErrors", m_nGrabErrors);
    a_pCmd->SetObjectCounterParam("MouseEvents",
            m_pEventHandler->GetMouseInputCount());
    a_pCmd->SetObjectCounterParam("KeyboardEvents",
            m_pEventHandler->GetKeyboardInputCount());
    a_pCmd->SetObjectErrorCounterParam("KBMInputErrors",
            m_pEventHandler->GetInputErrorCount());
    if (a_pCmd->StatusLevel(2))
        a_pCmd->SetObjectCounterParam("FrameRateBackoff", m_pGrabRateController->GetBackoffCount());


}

