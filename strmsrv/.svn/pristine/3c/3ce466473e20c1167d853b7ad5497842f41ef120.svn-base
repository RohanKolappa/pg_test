 /* fbrx.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include <sstream>

#include "common.hh"
#include "fbrx.hh"
#include "notify.hh"
#include "v2d.h"
#include "v2o_lib.h"
#include "shaper.hh"
#include "codecdefs.h"
#include "vncencoderthr.hh"
#include "meter.hh"
#include "ratecontroller.hh"

using namespace std;

CFBRx::CFBRx(int nId, int compression, int codecversion) :
    CReceiver(nId)
{
    InitializeVariables();

    m_nCompression = compression;
    m_nCodecVersion = FIX_CODEC_VERSION(codecversion);
    m_nSliceRefresh = 5;
    m_eEncodingType = ScalableEncoding;
    m_eEncodingSubType = ScalableStandard;
    m_nLayerDimension = 4;

    // Allocate memory
    m_pScapBuffer = new char[AV_BUFFER_SIZE + 1];
    CreateEncodedBuffer();

    m_nBandwidth = 10 * 1024 * 1024;
    m_pShaper = new CShaper(m_nBandwidth, 2.0, 100);

    m_pMeter = new CMeter(2);

    m_pOutputRateController = new CRateController(24);

}

CFBRx::~CFBRx()
{
    if (m_qEncoderSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qEncoderSink);

    delete m_pShaper;
    delete m_pMeter;

    delete [] m_pEncodedBuffer;
    delete [] m_pScapBuffer;
    delete m_pLUT;
    delete m_pOutputRateController;
}


void CFBRx::InitializeVariables()
{
    m_nEncoderThrottled = 0;
    m_pEncodedBuffer = NULL;
    m_pWritePtr = NULL;
    m_pEndPtr = NULL;
    m_nSequenceNumber = 0;
    m_tLastScapSent = 0;
    m_nScapLength = 0;
    m_nKeyboardType = KB_TYPE_PC;
    m_nMouseType = MS_TYPE_IMPS2;
    m_nVideoChanged = 1;
    m_nSliceSize = 32;
    m_nMQuant = 1;
    m_nDCQuant = 1;
    m_nCodecVersion = CODEC_VERSION_STANDARD;
    m_nFramesServed = 0;
    m_pLUT = NULL;
    m_nPadding = 0;
    m_nVideoOutBytes = 0;
    m_nFSResfreshCount = 0;
    m_bResolutionChanged = false;
    m_nCtlOutPackets = 0;
    m_nCtlOutBytes = 0;
    m_nCtlInPackets = 0;
    m_nCtlInBytes = 0;
    m_nKbPackets = 0;
    m_nKbBytes = 0;
    m_nMsPackets = 0;
    m_nMsBytes = 0;
    m_bKBMEvent = false;
    m_pEncoderThread = NULL;
}

void CFBRx::DeleteStream()
{
    // If we have not been "Stopped" yet do it now
    if (IsStarted()) {
        SYSLOG_DEBUG("VNCRx still running, Calling stop internally.");
        Stop();
    }

    // Send stop request to VNC encoder thread which should end
    // up deleting itself
    if (m_pEncoderThread != NULL)
        m_pEncoderThread->Stop();
    m_pEncoderThread = NULL;

    // Remove our own (streamer) reference also.
    RemoveReference();
}

int CFBRx::ProcessStream()
{
    int retval = 0;

    // Don't do anything till we are started
    if (m_qQueueSink == NULL) {
        return 0;
    }

    // Set timestamps
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


    retval = ProcessFrameBuffers();

    // Send SCAP if needed
    PublishScap();

    return retval;
}

int CFBRx::ProcessAltStream()
{
    int retval = 0;
    char *pData;

    // Wait for queue to be assigned
    if (m_qQueueSource == NULL)
        return 0;


    // Get frame from Alt Queue
    CSFrame * sAltFrame = m_qQueueSource->GetFrame();
    if (sAltFrame == NULL)
        return 0;

    // In errored state, just release incoming frame and return
    if (IsErrored()) {
        m_qQueueSource->ReleaseFrame(sAltFrame);
        return 0;
    }

    if (sAltFrame->GetMessageType() == CSFrame::Normal) {

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
                    HandleAVRequest(pData);
                    m_nCtlInPackets++;
                    m_nCtlInBytes += nBytes;
                    break;
                case IDENT_CLIENT_CSTAT:
                    m_nCtlInPackets++;
                    m_nCtlInBytes += nBytes;
                    break;
                case IDENT_CLIENT_SERDAT:
                    break;
                case IDENT_CLIENT_KBDAT:
                    m_nKbPackets++;
                    m_nKbBytes += nBytes;
                    break;
                case IDENT_CLIENT_MSDAT:
                    m_nMsPackets++;
                    m_nMsBytes += nBytes;
                    break;
                case IDENT_CLIENT_KBABS:
                    ProcessKeyboard(pData);
                    m_nKbPackets++;
                    m_nKbBytes += nBytes;
                    break;
                case IDENT_CLIENT_MSABS:
                    ProcessMouse(pData);
                    m_nMsPackets++;
                    m_nMsBytes += nBytes;
                    break;
                case IDENT_CLIENT_RFBQUERY:
                    ProcessRFBQuery(pData);
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
    }
    else if (sAltFrame->GetMessageType() == CSFrame::Connected) {
        HandleDownstreamConnected();
    }
    else if (sAltFrame->GetMessageType() == CSFrame::Disconnected) {
        HandleDownstreamDisconnected();
    }

    m_qQueueSource->ReleaseFrame(sAltFrame);


    return retval;
}

void CFBRx::HandleConnected()
{
    if (IsConnected())
        return;

    SetConnected();
    m_nEventId = NOTIFICATION_EVENT_CONNECTED;
    m_bSendNotify = true;

    CSFrame *pSFrame = new CSFrame(GetCurrentTimeStamp(), CSFrame::Connected);
    if (pSFrame != NULL)
        m_qQueueSink->InsertFrame(pSFrame);

}

void CFBRx::HandleDisconnected()
{
    if (!IsConnected())
        return;

    SetDisConnected();
    m_nEventId = NOTIFICATION_EVENT_DISCONNECTED;
    m_bSendNotify = true;

    CSFrame *pSFrame =
            new CSFrame(GetCurrentTimeStamp(), CSFrame::Disconnected);
    if (pSFrame != NULL)
        m_qQueueSink->InsertFrame(pSFrame);

}

void CFBRx::CreateEncoderThread()
{
    // Create queue to send messages to the V2D Encoder thread
    ostringstream qid;
    qid << "V2DEncoder_" << m_nId;
    m_qEncoderSink = CSQueueManager::Instance()->GetQueueSink(qid.str());

    // Create and start the VNC Encoder thread
    m_pEncoderThread = new CVNCEncoderThread(this, this);
    m_pEncoderThread->SetCompression(m_nCompression);
    m_pEncoderThread->SetCodecVersion(m_nCodecVersion);
    m_pEncoderThread->Start();
}

// Function called by encoder thread when it
// sets up a new video resolution
void CFBRx::LutChanged(struct LUT_struct * a_pLUT, int a_nSliceSize)
{
    {
        CScopedLock lock(m_LUTMutex);
        struct LUT_struct *pLUT = new struct LUT_struct;
        memcpy(pLUT, a_pLUT, sizeof(struct LUT_struct));

        if (m_pLUT != NULL) {
            delete m_pLUT;
        }
        m_pLUT = pLUT;
        m_tLastScapSent = 0;
        m_nSliceSize = a_nSliceSize;
        m_bResolutionChanged = true;
    }

    CreateEncodedBuffer();
}

void CFBRx::CompressionChanged(int a_nMQuant, int a_nDCQuant)
{
    m_nMQuant = a_nMQuant;
    m_nDCQuant = a_nDCQuant;
}

// Chunk up packets from the encoder thread and push to queue
int CFBRx::ProcessEncodedPackets()
{
    if (m_pEncoderThread == NULL)
        return 0;

    CScopedLock lock(m_BufferMutex);

    // Request new frames from encoder at the configured framerate
    if (m_pWritePtr == NULL && m_pOutputRateController->Allow()) {
        int nEncodedLength = m_pEncoderThread->GetNextFrame(m_pEncodedBuffer);
        if (nEncodedLength > 0) {
            // Currently padding to next 1024 byte boundary
            // TODO: avoid padding
            memset(m_pEncodedBuffer + nEncodedLength, 0x00, 1024);
            int padding = 1024 - (nEncodedLength % 1024);
            m_nPadding += padding;
            m_pWritePtr = m_pEncodedBuffer;
            m_pEndPtr = m_pEncodedBuffer + nEncodedLength + padding;
            m_nFramesServed++;
            m_pMeter->Fill();
        }
    }

    if (m_pWritePtr == NULL)
        return 0;

    // Do we have the bandwidth to let a packet out?
    if (!(m_pShaper->CheckTokens(SIZE_OF_AV_PACKET))) {
        return 0;
    }

    // OK to let a packet be served in this round
    unsigned char packet[SIZE_OF_AV_PACKET];
    memset(packet, 0x00, 16);
    unsigned long *pSequenceNumber = (unsigned long *) (packet + 8);
    *pSequenceNumber = m_nSequenceNumber++;
    memcpy(packet + 16, m_pWritePtr, SIZE_OF_AV_PACKET - 16);
    m_pWritePtr += (SIZE_OF_AV_PACKET - 16);
    if (m_pWritePtr >= m_pEndPtr)
        m_pWritePtr = NULL;

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_VIDEO);

    CSFrame * sFrame = new CSFrame((char *) packet, 1040,
            v2dSMediaFlag.GetMediaFlag(), m_nTimestamp);

    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);
    m_frameCount++;
    m_bytesTransfered += SIZE_OF_AV_PACKET;
    m_nVideoOutBytes += SIZE_OF_AV_PACKET;

    return 1;
}

// Send SCAP as soon as video changed or every five seconds
void CFBRx::PublishScap()
{
    if (m_nTimestamp - m_tLastScapSent > 5000) {
        {
            CScopedLock lock(m_LUTMutex);
            if (m_pLUT != NULL) {
                if (m_bResolutionChanged) {
                    CreateScap(1);
                    SendScap();
                    CreateScap(0);
                    m_bResolutionChanged = false;
                }
                else {
                    SendScap();
                }
            }
            m_tLastScapSent = m_nTimestamp;
        }
        // Need a five sec tick of this
        RequestReady();
    }
}

int CFBRx::CreateScap(int a_nVideoChanged)
{
    int retval = 0;
    int pl, nLen = 0;
    unsigned char *cp = NULL;

    if (m_pLUT == NULL)
        return 0;
    SYSLOG_DEBUG("VNCRx: Creating Scap");

    cp = (unsigned char *) m_pScapBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_CAP);
    cp += 4;
    nLen += 4;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CHANGED, a_nVideoChanged, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_ID, m_pLUT->a_alutreg, cp);
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

    pl = strlen(m_pLUT->comment) + 1 + 4;
    FILL_PROTOCOL_UNIT_STRING(strlen(m_pLUT->comment) + 1, SCAP_IDENT_VIDEO_NAME,
            m_pLUT->comment, cp);
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
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HRES, m_pLUT->a_hres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VRES, m_pLUT->a_vres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_ORIG_HRES, m_pLUT->a_orig_hres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_ORIG_VRES, m_pLUT->a_orig_vres, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_PCR,
            (unsigned long)(m_pLUT->a_pcr * 1000.0), cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_REFRESH,
            (unsigned long)(m_pLUT->a_refresh * 1000.0), cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HFP, m_pLUT->a_hfp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HBP, m_pLUT->a_hbp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HSYNC, m_pLUT->a_hsync, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VFP, m_pLUT->a_vfp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VBP, m_pLUT->a_vbp, cp);
    nLen += pl;
    cp += pl;

    pl = 8;
    FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VSYNC, m_pLUT->a_vsync, cp);
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
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_COMPMODE, 0, cp);
    nLen += pl;
    cp += pl;

    pl = 6;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CODEC_VERSION, m_nCodecVersion, cp);
    nLen += pl;
    cp += pl;

    // Fill the total length
    m_nScapLength = nLen;
    cp = (unsigned char *) m_pScapBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, nLen);

    return retval;
}

void CFBRx::SendScap()
{

    if (m_nScapLength == 0)
        return;

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    CSFrame * sFrame = new CSFrame(m_pScapBuffer, m_nScapLength,
            v2dSMediaFlag.GetMediaFlag(), m_nTimestamp);
    ASSERT(m_qQueueSink != NULL);
    m_qQueueSink->InsertFrame(sFrame);
    m_frameCount++;
    m_bytesTransfered += m_nScapLength;
    m_nCtlOutPackets++;
    m_nCtlOutBytes += m_nScapLength;
}

// Parse AV Request to see if we need to serve a full screen refresh
void CFBRx::HandleAVRequest(char *pData)
{
    AVReqData_t avReqData;
    memset((void *) &avReqData, 0, sizeof(avReqData));

    if (v2dLibParseAVReq(&avReqData, pData) < 0)
        return;

    /* FS Refresh requested? */
    if (avReqData.m_slicedropSet) {
        // Trigger scap
        m_tLastScapSent = 0;

        // Ask our encoder for a Full screen refresh
        if (m_pEncoderThread != NULL)
            m_pEncoderThread->FullScreenRefresh();

        m_nFSResfreshCount++;
    }

    ProcessAVRequest(&avReqData);

}

void CFBRx::CreateEncodedBuffer()
{
    CScopedLock lock(m_BufferMutex);
    delete [] m_pEncodedBuffer;
    int size = 1024 * 768 * 1;
    if (m_FrameDim.Area() * 4 > size) {
        size = m_FrameDim.Area() * 4;
    }
    m_pEncodedBuffer = new uint8_t[size];
    m_pWritePtr = NULL;
}

int CFBRx::Update(CCommand *a_pCmd)
{
    int retval = 0;

    // Process RFB parameters
    int nCompression = 0;
    if (a_pCmd->GetIntVar("compression", &nCompression) != -1) {
        m_nCompression = nCompression;
        if (m_pEncoderThread != NULL) {
            CEncoderInfo *encoderInfo =
                    new CCompressionUpdate(m_nCompression, m_nCodecVersion, m_nSliceRefresh);
            CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
            pSFrame->SetCustomObject(encoderInfo);
            m_qEncoderSink->InsertFrame(pSFrame);
        }
        retval = 1;
    }

    int nCodecVersion = 1;
    if (a_pCmd->GetIntVar("codecversion", &nCodecVersion) != -1) {
        m_nCodecVersion = FIX_CODEC_VERSION(nCodecVersion);
        if (m_pEncoderThread != NULL) {
            CEncoderInfo *encoderInfo =
                    new CCompressionUpdate(m_nCompression, m_nCodecVersion, m_nSliceRefresh);
            CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
            pSFrame->SetCustomObject(encoderInfo);
            m_qEncoderSink->InsertFrame(pSFrame);
        }
        retval = 1;
    }

    int nSliceRefresh = 5;
    if (a_pCmd->GetIntVar("slicerefresh", &nSliceRefresh) != -1) {
        m_nSliceRefresh = nSliceRefresh;
        if (m_pEncoderThread != NULL) {
            CEncoderInfo *encoderInfo =
                    new CCompressionUpdate(m_nCompression, m_nCodecVersion, m_nSliceRefresh);
            CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
            pSFrame->SetCustomObject(encoderInfo);
            m_qEncoderSink->InsertFrame(pSFrame);
        }
        retval = 1;
    }

    int nLayerDimension = 4;
    if (a_pCmd->GetIntVar("svclayerdimension", &nLayerDimension) != -1) {
        m_nLayerDimension = nLayerDimension;
        if (m_pEncoderThread != NULL) {
            CEncoderInfo *encoderInfo =
                    new CSVCCompressionUpdate(m_eEncodingType, m_eEncodingSubType, m_nLayerDimension);
            CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
            pSFrame->SetCustomObject(encoderInfo);
            m_qEncoderSink->InsertFrame(pSFrame);
        }
        retval = 1;
    }

    char buff[64];
    if (a_pCmd->GetVar("svcencodingsubtype", buff, 64) != -1) {
        if (strcasecmp(buff, "Standard") == 0)
            m_eEncodingSubType = ScalableStandard;
        else if (strcasecmp(buff, "ScalableQuad") == 0)
            m_eEncodingSubType = ScalableQuad;
        else if (strcasecmp(buff, "ScalableDC") == 0)
            m_eEncodingSubType = ScalableDC;

        if (m_pEncoderThread != NULL) {
            CEncoderInfo *encoderInfo =
                    new CSVCCompressionUpdate(m_eEncodingType, m_eEncodingSubType, m_nLayerDimension);
            CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
            pSFrame->SetCustomObject(encoderInfo);
            m_qEncoderSink->InsertFrame(pSFrame);
        }
        retval = 1;
    }

    int nBandwidth = 10 * 1024 * 1024;
    if (a_pCmd->GetIntVar("bw", &nBandwidth) != -1) {
        if (nBandwidth < 1024)
            nBandwidth = 1024;
        m_nBandwidth = nBandwidth;
        SetBandwidth();
        retval = 1;
    }


    int nOutputFrameRate = 24;
    if (a_pCmd->GetIntVar("outputframerate", &nOutputFrameRate) != -1) {
        if (nOutputFrameRate < 1)
            nOutputFrameRate = 1;
        if (nOutputFrameRate > 100)
            nOutputFrameRate = 100;
        m_pOutputRateController->SetRate(nOutputFrameRate);
        retval = 1;
    }

    // More parameters handled by subclass
    if (CustomUpdate(a_pCmd))
        retval = 1;

    return retval;
}

void CFBRx::SetBandwidth()
{
    m_pShaper->SetBandwidth(m_nBandwidth);
}

int CFBRx::CustomUpdate(CCommand *a_pCmd)
{
    // Subclasses overide of needed
    return 0;
}

void CFBRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectConstantParam("OutputFrameRate", (int)m_pOutputRateController->GetRate());
    a_pCmd->SetObjectCounterParam("OutputFrameCount", m_nFramesServed);
    a_pCmd->SetObjectByteCounterParam("VideoOutBytes", m_nVideoOutBytes);
    a_pCmd->SetObjectByteCounterParam("Padding", m_nPadding);

    a_pCmd->SetObjectCounterParam("ControlOutPackets", m_nCtlOutPackets);
    a_pCmd->SetObjectByteCounterParam("ControlOutBytes", m_nCtlOutBytes);
    a_pCmd->SetObjectCounterParam("ControlInPackets", m_nCtlInPackets);
    a_pCmd->SetObjectByteCounterParam("ControlInBytes", m_nCtlInBytes);
    a_pCmd->SetObjectCounterParam("MousePackets", m_nMsPackets);
    a_pCmd->SetObjectByteCounterParam("MouseBytes", m_nMsBytes);
    a_pCmd->SetObjectCounterParam("KeyBoardPackets", m_nKbPackets);
    a_pCmd->SetObjectByteCounterParam("KeyBoardBytes", m_nKbBytes);

    a_pCmd->SetObjectConstantParam("SliceSize", m_nSliceSize);
    a_pCmd->SetObjectConstantParam("Compression", m_nCompression);
    a_pCmd->SetObjectConstantParam("CodecVersion", m_nCodecVersion);
    a_pCmd->SetObjectParam("FullScreenRefresh", m_nFSResfreshCount);
    a_pCmd->SetObjectCounterParam("Throttled", m_nEncoderThrottled);

    if (m_pEncoderThread != NULL)
        m_pEncoderThread->UpdateCustomStatus(a_pCmd);

    a_pCmd->SetObjectBitrateGuageParam("MaxBandwidth", m_nBandwidth);
    if (a_pCmd->StatusLevel(2) && m_pShaper != NULL)
        m_pShaper->UpdateCustomStatus(a_pCmd);
    if (a_pCmd->StatusLevel(2))
        a_pCmd->SetObjectCounterParam("MeterBackoff", m_pMeter->GetBackoffCount());
}
