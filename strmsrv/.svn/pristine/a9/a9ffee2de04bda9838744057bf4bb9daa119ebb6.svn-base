#include "strmsrv_compatibility.h"
#include <sstream>


#include "v2dff.hh"
#include "common.hh"
#include "notify.hh"

using namespace std;

CV2dFF::CV2dFF (int nId) :
    CSender(nId),
    m_pSRDPacket()
{
    m_nPortType = V2DFFPORT;
    m_nSendQueueId = 0;
    m_qSendQueueSink = NULL;
    m_bFramePending = false;
    m_lastReducedFrameCount = 0;

    m_srd = (sliceRedData_t *)malloc(sizeof(sliceRedData_t));
    if (m_srd != NULL) {
        if (initSRD(m_srd) < 0) {
            destroySRD(m_srd);
            free(m_srd);
            m_srd = NULL;
        }
        else {
            setStereoPolicy(m_srd, SP_SOURCE); // set current policy

        }
    }


}


CV2dFF::~CV2dFF()
{

    if(m_qSendQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qSendQueueSink);
    return;
}

int
CV2dFF::ProcessStream()
{


    int rc = 0;
    bool senddata = false;
    char *pData = NULL;
    int len = 0;
    unsigned long mediaFlag = 0;
    CSFrame *sFrame = NULL;


    if (m_qQueueSource != NULL)
        sFrame = m_qQueueSource->GetFrame();

    if (sFrame != NULL && m_srd != NULL) {

        unsigned long nFrameType = RetrieveFrameType(sFrame);

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }
        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        m_bytesTransfered += sFrame->GetLength();
        m_frameCount++;

        switch (nFrameType) {

        case FRAMETYPE_V2D_CTL:
            // All Control packets forwarded
            pData = sFrame->GetData();
            if (pData != NULL) {
                senddata = true;
                len = sFrame->GetLength();
                mediaFlag =  CSFrame::CreateMediaFlag(FRAMETYPE_V2D_CTL,
                                                      STREAMTYPE_V2D);
            }
            break;

        case FRAMETYPE_V2D_VIDEO:
            // Video data send to SRD module
            parsePackets(m_srd, (unsigned char *)sFrame->GetData(),
                         sFrame->GetLength());

            break;

        case FRAMETYPE_V2D_AUDIO:
            // Audio frames dropped
            break;

        default:

            break;
        }

        // We saw a frame in our queue so call us right back
        rc = 1;

    }

    // Does SRD have any thing for us??
    if (senddata == false && m_srd != NULL) {
        if ((m_bFramePending == false) &&
            (m_srd->reducedFrameCount - m_lastReducedFrameCount >= 7)) {
            m_bFramePending = true;
            m_lastReducedFrameCount = m_srd->reducedFrameCount;
        }
        if (m_bFramePending == true) {
            if (getPackets(m_srd, m_pSRDPacket, SEND_BUFFER_SIZE, 1) != -1) {
                senddata = true;
                pData = (char *) m_pSRDPacket;
                len = SIZE_OF_AV_PACKET;
                mediaFlag =  CSFrame::CreateMediaFlag(FRAMETYPE_V2D_VIDEO,
                                                      STREAMTYPE_V2D);
                // SRD had something so call us right back
                rc = 1;
            }
            else {
                m_bFramePending = false;
            }
        }
    }

    // Do we have any data to send ??
    if (senddata &&
        m_qSendQueueSink != NULL) {
        ASSERT(len != 0);
        ASSERT(mediaFlag != 0);
        // todo : determine a good timestamp to plug in
        CSFrame * newFrame =
            new CSFrame(pData, len, mediaFlag, m_nTimestamp);
        m_qSendQueueSink->InsertFrame(newFrame);
    }



    if (sFrame != NULL) {
        m_qQueueSource->ReleaseFrame(sFrame);
    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0 && m_qQueueSource != NULL) {
            CheckQueueTimestamp();
        }
    }


    return rc;
}


int
CV2dFF::Update(CCommand *a_pCmd)
{
    int retval=0;
    int nSendQueueId = 0;

    if (a_pCmd->GetIntVar("startqueue", &nSendQueueId) != -1) {

        if (m_qSendQueueSink == NULL) {
            ostringstream qid;
            m_nSendQueueId = nSendQueueId;
            qid << "Queue_" << m_nSendQueueId;
            m_qSendQueueSink =
                CSQueueManager::Instance()->GetQueueSink(qid.str());

            retval = 1;
        }
    }
    else if (a_pCmd->GetIntVar("stopqueue", &nSendQueueId) != -1) {

        if (m_qSendQueueSink != NULL) {

            CSQueueManager::Instance()->ReleaseQueueSink(m_qSendQueueSink);
            m_qSendQueueSink = NULL;
            retval = 1;
        }
    }
    return retval;
}

void
CV2dFF::PrintStatistics(string &a_stats)
{
}


void
CV2dFF::UpdateCustomStatus(CCommand *a_pCmd)
{


    a_pCmd->SetObjectConstantParam("SendQueueId", m_nSendQueueId);

    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectParam("MaxQueueLen",
                               (unsigned long)m_qQueueSource->MaxLength());
#ifdef SSDEBUG
        a_pCmd->SetObjectParam("AverageQueueLen",
                               (double)m_qQueueSource->AvgSize());
#endif
    }

#ifdef SSDEBUG
    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectParam("SourceContention",
                               (unsigned long)m_qQueueSource->GetContention());
    }
#endif


    if (m_srd != NULL) {
        a_pCmd->SetObjectErrorCounterParam("VideoLoss",
                                           m_srd->m_nVideoLoss);
        a_pCmd->SetObjectErrorCounterParam("VideoOutOfOrder",
                                           m_srd->m_nVideoOutOfOrder);
        a_pCmd->SetObjectParam("SRDStereoDetect",
                               (const char *) StereoDetect(m_srd));
        a_pCmd->SetObjectCounterParam("SRDRightFrames",
                                      m_srd->inRightFrameCount);
        a_pCmd->SetObjectCounterParam("SRDInPackets",
                                      m_srd->inPktCount);
        a_pCmd->SetObjectCounterParam("SRDOutPackets",
                                      m_srd->outPktCount);
        a_pCmd->SetObjectCounterParam("SRDInFrames",
                                      m_srd->inFrameCount);
        a_pCmd->SetObjectCounterParam("SRDOutFrames",
                                      m_srd->outFrameCount);
        a_pCmd->SetObjectCounterParam("SRDPrefillFrames",
                                      m_srd->prefillFrameCount);
        a_pCmd->SetObjectCounterParam("SRDReducedFrames",
                                      m_srd->reducedFrameCount);
        a_pCmd->SetObjectCounterParam("SRDInSlices",
                                      m_srd->inSlices);
        a_pCmd->SetObjectCounterParam("SRDOutSlices",
                                      m_srd->outSlices);
        a_pCmd->SetObjectCounterParam("SRDReducedSlices",
                                      m_srd->reducedSlices);
        a_pCmd->SetObjectByteCounterParam("SRDInBytes",
                                          m_srd->inBytes);
        a_pCmd->SetObjectByteCounterParam("SRDOutBytes",
                                          m_srd->outBytes);
        a_pCmd->SetObjectByteCounterParam("SRDReducedBytes",
                                          m_srd->reducedBytes);
        a_pCmd->SetObjectByteCounterParam("SRDPadBytes",
                                          m_srd->padBytes);
        a_pCmd->SetObjectErrorCounterParam("SRDSliceOutOfOrder",
                                           m_srd->parse_errors_ooo_slice);
        a_pCmd->SetObjectErrorCounterParam("SRDBadSlice",
                                           m_srd->parse_errors_bad_slice);
        a_pCmd->SetObjectErrorCounterParam("SRDBadFrame",
                                           m_srd->parse_errors_bad_frame);
        a_pCmd->SetObjectErrorCounterParam("SRDSOFError",
                                           m_srd->parse_errors_sof);
        a_pCmd->SetObjectCounterParam("SRDIFrameCount",
                                      m_srd->IFrameCount);
        a_pCmd->SetObjectCounterParam("SRDOutputIFrameCount",
                                      m_srd->OutputIFrameCount);
        a_pCmd->SetObjectCounterParam("SRDFullFrameCount",
                                      m_srd->FullFrameCount);
        a_pCmd->SetObjectGuageParam("SRDFullFrameInterval",
                                    m_srd->FullFrameInterval);
        a_pCmd->SetObjectGuageParam("SRDFirstFFInterval",
                                    m_srd->FirstFullFrameInterval);
        a_pCmd->SetObjectErrorCounterParam("SRDFrameOverflow",
                                           m_srd->hbuff_overflow);
        a_pCmd->SetObjectParam("SRDSliceSize",
                               (double) m_srd->avgSliceSize);
        a_pCmd->SetObjectParam("SRDFrameCoverage",
                               (double) m_srd->avgFrameCoverage);
        a_pCmd->SetObjectParam("SRDSliceDebug",
                               (bool) m_srd->debug_slices);
        a_pCmd->SetObjectParam("SRDSliceMarking",
                               (bool) m_srd->mark_slices);
        a_pCmd->SetObjectGuageParam("SRDBufferPool",
                                    m_srd->buffer_pool_count);
    }


}
