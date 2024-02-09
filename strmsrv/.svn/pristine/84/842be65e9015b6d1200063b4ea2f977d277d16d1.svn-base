/*
 * vncrx.cpp
 *
 *  Created on: Nov 22, 2009
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include <sstream>

#include "common.hh"
#include "vncrx.hh"
#include "notify.hh"
#include "v2d.h"
#include "v2o_lib.h"
#include "shaper.hh"
#include "geometry.hh"


CVNCRx::CVNCRx(int nId, char *serverip, int port, char *password,
        int compression, int codecversion) :
    CFBRx(nId, compression, codecversion)
{
    m_nRectangleCount = 0;
    m_nFBRequestCount = 0;
    m_nPartialUpdates = 0;
    m_bReceivedFBUpdate = false;
    m_nFBRequestTimestamp = 0;
    m_tKBMTimeStamp = 0;

    m_nPortType = VNCNETWORKPORT;

    m_pRFB = new CRFBHandler(this, serverip, password, port);

    CreateEncoderThread();
}

CVNCRx::~CVNCRx()
{
    delete m_pRFB;
}

int CVNCRx::ProcessFrameBuffers()
{
    // Move the RFB protocol along
    // Further activity performed in callbacks issued by CRFBHandler
    if (m_pRFB->ProcessRFBConnection() > 0) {
        return 1;
    }
    return 0;
}

void CVNCRx::ProcessAVRequest(AVReqData_t *)
{
    if (m_pRFB != NULL)
        m_pRFB->ResetReady();
}

void CVNCRx::ProcessRFBQuery(char *pData)
{
    if (m_pRFB != NULL)
        m_pRFB->ProcessRFBQuery(pData);
}

void CVNCRx::ProcessKeyboard(char *pData)
{
    if (m_pRFB != NULL)
        m_pRFB->ProcessKeyboard(pData);
}

void CVNCRx::RequestReady()
{
    if (m_pRFB != NULL)
        m_pRFB->RequestReady();
}

void CVNCRx::ProcessMouse(char *pData)
{
    if (m_pRFB != NULL)
        m_pRFB->ProcessMouse(pData);
}

void CVNCRx::HandleRFBConnected()
{
    // Init RFB Video protocol
    InitializeVideo();

    HandleConnected();

}

void CVNCRx::HandleRFBDisconnected()
{
    HandleDisconnected();
}

int CVNCRx::HandleRFBReady()
{
    ProcessRectangles();

    RequestFBUpdates();

    ProcessEncodedPackets();

    return 0;
}

void CVNCRx::InitializeVideo()
{
    // Setup RFB client to request/receive FrameBuffer updates
    m_FrameDim.SetDim(m_pRFB->GetClient()->m_frameWidth,
            m_pRFB->GetClient()->m_frameHeight);
    RFBSendExpectedPixelFormat(m_pRFB->GetClient());
    RFBSendSupportedEncodings(m_pRFB->GetClient());
    RFBSendVideoRequest(m_pRFB->GetClient(), 0, 0, m_FrameDim.width,
            m_FrameDim.height, 0);
    m_nFBRequestTimestamp = CTimeKeeper::Now();
    m_nFBRequestCount++;

    // Send the latest video parameters to our encoder
    CNewVideoResolutionInfo *encoderInfo = new CNewVideoResolutionInfo(
            m_FrameDim.width, m_FrameDim.height);
    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(encoderInfo);
    m_qEncoderSink->InsertFrame(pSFrame);
}


void CVNCRx::ProcessRectangles()
{
    // Check if RFB protocol delivered us a new rectangle
    RFBRect_t * pRectangle = RFBGetRectangle(m_pRFB->GetClient());

    if (pRectangle != NULL) {
        // We have a new rectangle from VNC client
        m_nRectangleCount++;

        // Are more rectangles expected??
        if (LAST_RECTANGLE(pRectangle)) {
            m_bReceivedFBUpdate = true;
        }
        else {
            m_nPartialUpdates++;
        }

        // Send rectangle to encoder's queue
        CRectangleUpdate *encoderInfo = new CRectangleUpdate(pRectangle);

        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(encoderInfo);
        m_qEncoderSink->InsertFrame(pSFrame);

    }

}

void CVNCRx::UpdateKBMEvent(unsigned long a_sessionId)
{
    m_tKBMTimeStamp = CTimeKeeper::GetTime();
    m_bKBMEvent = true;
}

// Ask for another Frame Buffer update if ready
int CVNCRx::RequestFBUpdates()
{
    // Until we are done with the previous updates, do not ask
    // for more
    if (!m_bReceivedFBUpdate)
        return 0;

    // Ensure previous requests have all been handled by the encoder
    if (m_qEncoderSink->Size() > 4) {
        m_nEncoderThrottled++;
        return 0;
    }

    // Try to keep the FrameBuffer Updates to no more than 24 FPS
    if (m_bKBMEvent == true) {
        m_bKBMEvent = false;
    }
    else {
        if ((CTimeKeeper::Now() - m_nFBRequestTimestamp) < WAIT_24_FPS)
            return 0;
    }

    // Ok to request another frame
    RFBSendVideoRequest(m_pRFB->GetClient(), 0, 0, m_FrameDim.width,
            m_FrameDim.height, 1);
    m_nFBRequestTimestamp = CTimeKeeper::Now();
    m_bReceivedFBUpdate = false;
    m_nFBRequestCount++;
    return 1;
}

int CVNCRx::CustomUpdate(CCommand *a_pCmd)
{
    if ((m_pRFB != NULL) && m_pRFB->Update(a_pCmd))
        return 1;
    return 0;

}

void CVNCRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    // Update parent class status first
    CFBRx::UpdateCustomStatus(a_pCmd);

    if (m_pRFB != NULL)
        m_pRFB->UpdateCustomStatus(a_pCmd);

    a_pCmd->SetObjectCounterParam("RectangleCount", m_nRectangleCount);
    a_pCmd->SetObjectCounterParam("PartialUpdates", m_nPartialUpdates);
    a_pCmd->SetObjectCounterParam("FBRequests", m_nFBRequestCount);
    if (m_pEncoderThread != NULL) {
        timestamp_t tEncodeTimestamp = m_pEncoderThread->GetEncodeTimestamp();
        if (m_tKBMTimeStamp > 0 && (tEncodeTimestamp >= m_tKBMTimeStamp))
            a_pCmd->SetObjectParam("MouseLatency",
                    (unsigned long long) (tEncodeTimestamp - m_tKBMTimeStamp));
        else
            a_pCmd->SetObjectParam("MouseLatency", -1);
    }

}
