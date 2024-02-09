/*
 * vncencoderthr.cpp
 *
 *  Created on: Nov 23, 2009
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include <sstream>

#include "vncencoderthr.hh"
#include "fbrx.hh"
#include "common.hh"
#include "rfbclient.h"
#include "vframeencoder.hh"
#include "v2d.h"
#include "v2o_lib.h"
#include "encodermutex.hh"
#include "V2DUtils.hh"

using namespace std;


CVNCEncoderThread::CVNCEncoderThread(CStreamer *a_pStreamer,
        IV2DEncoderListener *a_pV2DEncoderListener)
    : CDetachedThread(a_pStreamer, "V2DEncoder")
{
    InitializeVariables();

    // Setup thread variables
    m_priority = 0; // A low priority non real-time thread

    m_pV2DEncoderListener = a_pV2DEncoderListener;

    // Setup video encoding parameters
    SetCompression(2);

    m_eEncodeFormat = eYUV422P;
    m_ePixelFormat = eBGRX32;
    m_nSliceRefresh = 5;

    m_pCurrentLUT = new struct LUT_struct;
    memset(m_pCurrentLUT, 0x00, sizeof(struct LUT_struct));

    // Create the queue source for getting our messages
    ostringstream qid;
    qid << "V2DEncoder_" << GetId();
    m_qEncoderSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qEncoderSource->SetOwner(GetId());
}

CVNCEncoderThread::~CVNCEncoderThread()
{
    SYSLOG_DEBUG("Deleting VNC Encoder Thread");

    delete m_pFrameEncoder;

    CSQueueManager::Instance()->ReleaseQueueSource(m_qEncoderSource);

}

bool CVNCEncoderThread::SetCompression(int nCompression)
{
    int nMQuant = mquant_from_compression(nCompression);
    int nDCQuant = dcquant_from_compression(nCompression);
    if (m_nMQuant != nMQuant || m_nDCQuant != nDCQuant) {
        m_nMQuant = nMQuant;
        m_nDCQuant = nDCQuant;
        return true;
    }
    return false;
}

bool CVNCEncoderThread::SetCodecVersion(int nCodecVersion)
{
    enum eEncodeFormat eEncodeFormat;
    if (nCodecVersion == 2)
        eEncodeFormat = eSVC;
    else
        eEncodeFormat = eYUV422P;

    if (m_eEncodeFormat != eEncodeFormat) {
        m_eEncodeFormat = eEncodeFormat;
        return true;
    }
    return false;
}

int CVNCEncoderThread::GetCodecVersion()
{
    if (m_eEncodeFormat == eSVC)
        return 2;
    return 1;
}

bool CVNCEncoderThread::SetSliceRefresh(int nSliceRefresh)
{
    if (m_nSliceRefresh != nSliceRefresh) {
        m_nSliceRefresh = nSliceRefresh;
        return true;
    }
    return false;
}

int CVNCEncoderThread::GetSliceRefresh() {
    return m_nSliceRefresh;
}


void CVNCEncoderThread::InitializeVariables()
{
    m_nFrameWidth = 0;
    m_nFrameHeight = 0;
    m_nMQuant = 1;
    m_nDCQuant = 1;
    m_pFrameEncoder = NULL;
    m_pCurrentLUT = NULL;
    m_tEncodeTimestamp = 0;
    m_nDroppedRectangleCount = 0;
    m_nRectangleCount = 0;
}

int CVNCEncoderThread::RunThread()
{
    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        if (EncoderProcessor() == -1) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            break;
        }
        LightSleep();

    }

    // Inform the listener that we are done
    m_pV2DEncoderListener->Stopped();

    return 0;
}


int CVNCEncoderThread::EncoderProcessor()
{
    CSFrame * sFrame = m_qEncoderSource->GetFrame();

    if (sFrame == NULL) {
        return 0;
    }

    if (sFrame->GetMessageType() != CSFrame::Custom) {
        // Not expecting any other frame type
        ASSERT(0);
        m_qEncoderSource->ReleaseFrame(sFrame);
        return 0;
    }

    CEncoderInfo *encoderInfo =
            static_cast<CEncoderInfo *> (sFrame->GetCustomObject());


    switch (encoderInfo->GetMessageType()) {

    case CEncoderInfo::CompressionUpdate:
    {
        CCompressionUpdate * compressionUpdate =
                static_cast <CCompressionUpdate *>(encoderInfo);
        SYSLOG_INFO("Got Compression Update");
        SetupEncoder(compressionUpdate);
        break;
    }

    case CEncoderInfo::SVCCompressionUpdate:
    {
        CSVCCompressionUpdate * svcCompressionUpdate =
                static_cast <CSVCCompressionUpdate *>(encoderInfo);
        SYSLOG_INFO("Got SVC Compression Update");
        SetupEncoder(svcCompressionUpdate);
        break;
    }

    case CEncoderInfo::NewVideoResolution:
    {
        CNewVideoResolutionInfo * videoResInfo =
                static_cast <CNewVideoResolutionInfo *>(encoderInfo);
        SetupFrameBuffer(videoResInfo->GetWidth(), videoResInfo->GetHeight(),
                videoResInfo->GetPixelFormat());
        break;
    }

    case CEncoderInfo::RectangleUpdate:
    {
        CRectangleUpdate * rectangleUpdate =
                static_cast <CRectangleUpdate *>(encoderInfo);
        RFBRect_t *pRectangle = rectangleUpdate->GetRectangle();
        UpdateFrameBuffer(pRectangle);

        // Encode frame if this is the last rectangle received
        if (LAST_RECTANGLE(pRectangle)) {
            if (m_pFrameEncoder != NULL) {
                m_pFrameEncoder->EncodeFrame();
                m_tEncodeTimestamp = CTimeKeeper::GetTime();
            }
        }

        RFBDestroyRectangle(pRectangle);
        break;
    }

    case CEncoderInfo::FrameBufferUpdate:
    {
        CFrameBufferUpdate * frameBufferUpdate =
                static_cast <CFrameBufferUpdate *>(encoderInfo);
        unsigned char *pBuffer = frameBufferUpdate->GetBuffer();
        UpdateFrameBuffer(pBuffer);
        if (m_pFrameEncoder != NULL) {
            m_pFrameEncoder->EncodeFrame();
            m_tEncodeTimestamp = CTimeKeeper::GetTime();
        }
        break;
    }

    default:
        ASSERT(0);
        break;
    }

    // Release frame resources
    delete (encoderInfo);
    m_qEncoderSource->ReleaseFrame(sFrame);

    return 0;
}

void CVNCEncoderThread::SetupEncoder(CCompressionUpdate * compressionUpdate) {

    if (SetCompression(compressionUpdate->GetCompression()))
        m_pV2DEncoderListener->CompressionChanged(m_nMQuant, m_nDCQuant);

    SetSliceRefresh(compressionUpdate->GetSliceRefresh());

    if (m_pFrameEncoder!= NULL) {
        m_pFrameEncoder->SetQuants(m_nMQuant, m_nDCQuant);
        m_pFrameEncoder->SetSliceRefresh(m_nSliceRefresh);
    }

    // Recreate encoder if encode format changes
    if (SetCodecVersion(compressionUpdate->GetCodecVersion())) {
        if (m_pFrameEncoder != NULL) {
            NewFrameEncoder();
        }
        m_pV2DEncoderListener->LutChanged(m_pCurrentLUT, m_pFrameEncoder->GetSliceSize());
    }
}

void CVNCEncoderThread::SetupEncoder(CSVCCompressionUpdate * svcCompressionUpdate) {
    if (m_pFrameEncoder!= NULL) {
        m_pFrameEncoder->SetEncodingType(svcCompressionUpdate->GetEncodingType());
        m_pFrameEncoder->SetEncodingSubType(svcCompressionUpdate->GetEncodingSubType());
        m_pFrameEncoder->SetLayerDimension(svcCompressionUpdate->GetLayerDimension());
        m_pFrameEncoder->UpdateAll();
    }
}

void CVNCEncoderThread::SetupFrameBuffer(int a_nWidth, int a_nHeight,
        enum ePixelFormat a_ePixelFormat) {

    SYSLOG_INFO("New Video Resolution %d x %d -> %d x %d",
            m_nFrameWidth, m_nFrameHeight, a_nWidth, a_nHeight);

    V2DUtils::CreateLutEntry(a_nWidth, a_nHeight, m_pCurrentLUT);

    m_nFrameWidth = a_nWidth;
    m_nFrameHeight = a_nHeight;
    m_ePixelFormat = a_ePixelFormat;

    // Recreate the frame encoder
    NewFrameEncoder();

    m_pV2DEncoderListener->LutChanged(m_pCurrentLUT, m_pFrameEncoder->GetSliceSize());
}


void CVNCEncoderThread::UpdateFrameBuffer(unsigned char * a_pBuffer)
{
    // Tell Encoder about the new buffer
    if (m_pFrameEncoder != NULL)
        m_pFrameEncoder->UpdateFrameFromBuffer(a_pBuffer);

}

void CVNCEncoderThread::UpdateFrameBuffer(RFBRect_t * a_pRectangle)
{
    // Update our frame buffer
    if (m_pFrameEncoder == NULL) {
        m_nDroppedRectangleCount++;
        return;
    }

    if (a_pRectangle->m_Encoding != 0) {
        // Only handling raw encodings
        m_nDroppedRectangleCount++;
        return;
    }

    m_nRectangleCount++;

    // Handle Raw encoding
    int xPos = a_pRectangle->m_XPos;
    int yPos = a_pRectangle->m_YPos;
    int width = a_pRectangle->m_Width;
    int height = a_pRectangle->m_Height;

    width = (xPos + width <= m_nFrameWidth) ? width : m_nFrameWidth - xPos;
    height = (yPos + height <= m_nFrameHeight) ? height : m_nFrameHeight - yPos;

    unsigned char *dst = m_pFrameEncoder->GetBitmap();
    dst += (yPos * m_nFrameWidth * 4) + xPos * 4;

    unsigned char *src = a_pRectangle->m_Data;
    for (int j = 0; j < height; j++) {
        memcpy(dst, src, width * 4);
        src += (a_pRectangle->m_Width * 4);
        dst += (m_nFrameWidth * 4);
    }


    // Tell encoder about the area we just updated
    if (m_pFrameEncoder != NULL)
        m_pFrameEncoder->UpdateRectangle(xPos, yPos, width, height);

}

void CVNCEncoderThread::NewFrameEncoder() {
    CScopedLock lock(m_encoderMutex);
    // Delete previous encoder
    delete m_pFrameEncoder;

    // Setup New Encoder
    m_pFrameEncoder = new CVideoFrameEncoder(m_nFrameWidth, m_nFrameHeight,
            m_nMQuant, m_nDCQuant, m_eEncodeFormat, m_ePixelFormat);
    m_pFrameEncoder->SetMutex(new CEncoderMutex());
    m_pFrameEncoder->SetSliceRefresh(m_nSliceRefresh);

    m_pFrameEncoder->AllocateBitmap();

    // Get ready for first image
    m_pFrameEncoder->UpdateAll();

}

int CVNCEncoderThread::GetNextFrame(unsigned char *a_pBuffer) {
    CScopedLock lock(m_encoderMutex);
    if (m_pFrameEncoder == NULL)
        return 0;
    return m_pFrameEncoder->GetNextFrame(a_pBuffer);
}


void CVNCEncoderThread::FullScreenRefresh()
{
    CScopedLock lock(m_encoderMutex);
    if (m_pFrameEncoder != NULL)
        m_pFrameEncoder->SetFSRefresh();
}


timestamp_t CVNCEncoderThread::GetEncodeTimestamp() {
    return m_tEncodeTimestamp;
}

uint64_t CVNCEncoderThread::GetDroppedRectangleCount() {
    return m_nDroppedRectangleCount;
}

uint64_t CVNCEncoderThread::GetRectangleCount() {
    return m_nRectangleCount;
}


uint64_t CVNCEncoderThread::GetEncodedFrameCount() {
    if (m_pFrameEncoder == NULL)
        return 0;
    return m_pFrameEncoder->GetEncodedFrameCount();
}

void CVNCEncoderThread::UpdateCustomStatus(CCommand *a_pCmd)
{
    if (m_pCurrentLUT != NULL)
        a_pCmd->SetObjectParam("VideoName", m_pCurrentLUT->comment);
    else
        a_pCmd->SetObjectParam("VideoName", "");
    a_pCmd->SetObjectConstantParam("FrameWidth", m_nFrameWidth);
    a_pCmd->SetObjectConstantParam("FrameHeight", m_nFrameHeight);
    a_pCmd->SetObjectConstantParam("MQuant", m_nMQuant);
    a_pCmd->SetObjectConstantParam("DCQuant", m_nDCQuant);
    a_pCmd->SetObjectConstantParam("SliceRefresh", m_nSliceRefresh);
    if (m_pFrameEncoder != NULL) {
        a_pCmd->SetObjectCounterParam("EncodedFrames",
                m_pFrameEncoder->GetEncodedFrameCount());
        a_pCmd->SetObjectByteCounterParam("EncodedOutFrames",
                m_pFrameEncoder->GetServedFrameCount());
        a_pCmd->SetObjectCounterParam("EncodedSlices",
                m_pFrameEncoder->GetEncodedSliceCount());
        a_pCmd->SetObjectByteCounterParam("EncodedInBytes",
                m_pFrameEncoder->GetEncodedInBytes());
        a_pCmd->SetObjectByteCounterParam("EncodedOutBytes",
                m_pFrameEncoder->GetEncodedOutBytes());
        a_pCmd->SetObjectParam("SVC",
                (m_pFrameEncoder->GetEncodeFormat() == eSVC));
    }

}
