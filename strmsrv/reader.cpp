
#include "strmsrv_compatibility.h"
#include <sstream>


#include "common.hh"
#include "reader.hh"
#include "readerdiskiothr.hh"
#include "readerquerythr.hh"
#include "sbuffer.hh"
#include "notify.hh"
#include "StreamInfo.hh"
#include "FFUtils.hh"

using namespace std;

CReader::CReader(int nId,
                 const char *pMediaName,
                 const char *pMediaDir,
                 timestamp_t nTimecode,
                 timestamp_t nOffSet,
                 const char * speed) :
        CStreamer(nId)
{

    m_nPortType = DMSMEDIAPORT;

    m_maxQueueDrain = 0;
    m_minQueueSize = 100000;
    m_maxQueueSize = 0;
    m_savedMaxQueueDrain = m_maxQueueDrain;
    m_savedMinQueueSize = m_minQueueSize;
    m_savedMaxQueueSize = m_maxQueueSize;

    ConfigureSpeed(speed);

    m_nSessionStartTime = 0;
    m_nStartTime = 0;
    m_nTimestamp = 0;
    m_nStopTime = 0;

    m_nGapAdjust = 0;
    m_nGapServedCount = 0;


    m_drainCount = 0;
    m_startCounter = NULL;
    m_ready = false;
    m_nDiscontinuity = false;

    // Create the DiskIO thread queue source
    ostringstream qid;
    qid << "ReaderDiskIO_" << m_nId;
    m_qDiskIOSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qDiskIOSource->SetOwner(m_nId);

    // Increase the maximum size of the source queue
    m_qDiskIOSource->SetMaxSize(50000);
    m_qDiskIOSource->SetMaxBytes(20 * 1024 * 1024);

    // Create and start the Reader DiskIO thread
    m_pReaderDiskIOThread = new CReaderDiskIOThread(this,
            pMediaName,
            pMediaDir,
            m_dSpeed,
            nTimecode,
            nOffSet);
    m_pReaderDiskIOThread->Start();

}

CReader::~CReader()
{
    SYSLOG_DEBUG("Reader deleted.");
    if (m_qDiskIOSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qDiskIOSource);

    if (m_qQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qQueueSink);
}


void
CReader::DeleteStream()
{

    // If we have not been "Stopped" yet do it now
    if (IsStarted()) {
        SYSLOG_DEBUG("Reader still running, Calling stop internally.");
        Stop();
    }
    else {
        if (m_pReaderDiskIOThread != NULL) {
            m_pReaderDiskIOThread->Stop();
            m_pReaderDiskIOThread = NULL;
        }
    }
    // Remove the streamer reference also
    RemoveReference();
    // The last reference removed (most likely in diskio thread) will
    // delete this object
}

void CReader::ConfigureSpeed(const char * speed) {

    m_dSpeed = 1.0;
    m_bTrickMode = false;

    CFFUtils::ParseSpeed(speed, m_dSpeed);

    double delta = 0.0000001;
    if (m_dSpeed < 1.0 - delta || m_dSpeed > 1.0 + delta) {
        m_bTrickMode = true;
    }
}

bool
CReader::ReadytoStart()
{

    CSFrame *pSFrame;
    if ((pSFrame = m_qDiskIOSource->GetFrame()) != NULL) {
        // we have a frame
        if (m_startCounter == NULL) {
            // not a sync start, we are ready and can start now
            m_nTimestamp = pSFrame->GetTimeStamp();
            ASSERT(m_nTimestamp != 0);
            ASSERT(pSFrame->GetMessageType() != CSFrame::Normal);
            m_nFrameWallClock = CTimeKeeper::Now();
            m_nStartTimestamp = m_nTimestamp;
            m_nStartWallClock = CTimeKeeper::Now();
            m_bSendNotify = true;
            m_nEventId = NOTIFICATION_EVENT_BEGIN;
            m_nStartTime = m_nTimestamp;
            m_nSessionStartTime = CTimeKeeper::Now();
            SetConnected();
            if (pSFrame->GetMessageType() == CSFrame::StartOfStream) {
                SYSLOG_DEBUG("Notification: StartOfStream %"PRIu64"", m_nTimestamp);
            }
            return true;
        }
        else {
            // we need to wait till everyone in the sync
            // start group is ready
            if (!m_ready) {
                m_startCounter->Ready(); // We are ready
                m_ready = true;
                SYSLOG_DEBUG("I am ready %d\n", m_nId);
            }

            // is everyone ready ??
            if (m_startCounter->IsReady()) {
                // everyone is ready, we can start
                m_nTimestamp = pSFrame->GetTimeStamp();
                ASSERT(m_nTimestamp != 0);
                ASSERT(pSFrame->GetMessageType() != CSFrame::Normal);
                m_nFrameWallClock = CTimeKeeper::Now();
                m_nStartTimestamp = m_nTimestamp;
                m_nStartWallClock = CTimeKeeper::Now();
                m_bSendNotify = true;
                m_nEventId = NOTIFICATION_EVENT_BEGIN;
                m_nStartTime = m_nTimestamp;
                m_nSessionStartTime = CTimeKeeper::Now();
                m_startCounter->Release();
                m_startCounter = NULL;
                SetConnected();
                SYSLOG_DEBUG("Everyone is ready %d\n", m_nId);
                return true;
            }
        }
    }
    return false;
}


int
CReader::ProcessStream()
{
    long long nTimeDifference = 0;
    timestamp_t NextTimestamp;
    CSFrame *pSFrame;

    if (m_qQueueSink == NULL)
        goto EXIT_LABEL;


    // If no frames seen yet, we need to wait for the first frame
    // to be inserted in our DiskIO buffers so that we can
    // initialize our timestamps

    if (m_nTimestamp == 0) {
        if (!ReadytoStart())
            goto EXIT_LABEL;
    }

    ASSERT(m_nSessionStartTime != 0);
    ASSERT(m_nTimestamp != 0);
    ASSERT(m_nStartTime != 0);

    // How far are we ahead of presentation time?
    // Peek into the next frame and find its timestamp
    pSFrame = m_qDiskIOSource->GetFrame();
    if (pSFrame == NULL)
        goto EXIT_LABEL;
    NextTimestamp = pSFrame->GetTimeStamp();

    // If next frame marks a gap, adjust our session timestamps so
    // that we can jump the gap smoothly
    if (pSFrame->GetMessageType() == CSFrame::GapPresent) {

        // m_nGapAdjust does not store the total gap time but the play-out time
        // so far before the current gap
        nTimeDifference =
                (timestamp_t)((CTimeKeeper::Now() - m_nSessionStartTime) * m_dSpeed) -
                (m_nGapAdjust + m_nTimestamp - m_nStartTime);

        if (nTimeDifference > 0) {
            SYSLOG_DEBUG("Gap Adjustment: %"PRIu64", %"PRIu64", %"PRIu64"\n",
                         m_nGapAdjust, m_nTimestamp, m_nStartTime);
            m_nGapAdjust += m_nTimestamp - m_nStartTime;
            m_nStartTime = NextTimestamp;
            m_nGapServedCount++;
        }
    }

    nTimeDifference =
        (timestamp_t)((CTimeKeeper::Now() - m_nSessionStartTime) * m_dSpeed) -
        (NextTimestamp - m_nStartTime + m_nGapAdjust);

    if (nTimeDifference > 0) {

        if (m_minQueueSize > m_qDiskIOSource->Size())
            m_minQueueSize = m_qDiskIOSource->Size();
        if (m_maxQueueSize < m_qDiskIOSource->Size())
            m_maxQueueSize = m_qDiskIOSource->Size();


        if (pSFrame->GetMessageType() == CSFrame::Normal) {
            m_nTimestamp = pSFrame->GetTimeStamp();
            m_nFrameWallClock = CTimeKeeper::Now();

            CSFrame *sFrame =
                new CSFrame(pSFrame->GetBuffer(),
                            pSFrame->GetOffset(),
                            pSFrame->GetLength(),
                            pSFrame->GetMediaFlag(),
                            pSFrame->GetTimeStamp());

            if (m_nDiscontinuity) {
                sFrame->SetDiscontinuity();
                m_nDiscontinuity = false;
            }
            if (sFrame != NULL)
                m_qQueueSink->InsertFrame(sFrame);

            m_bytesTransfered += pSFrame->GetLength();
            m_frameCount++;
        }
        else if (pSFrame->GetMessageType() == CSFrame::StartOfStream) {
            // This case better already have been handled in ReadytoStart
            // We forward a StartOfStream frame for all listeners to this queue
            ASSERT(m_nTimestamp == pSFrame->GetTimeStamp());
            SYSLOG_DEBUG("StartOfStream Acknowledged: %"PRIu64"\n",
                         m_nTimestamp);
            CSFrame *sFrame = new CSFrame(m_nTimestamp, CSFrame::StartOfStream);

            // StartOfStream adds custom StreamInfo data
            CSBuffer *sBuffer = new CSBuffer(sizeof(CStreamInfo) + 32, 64);
            char * buffer = sBuffer->GetBuffer(0);
            new (buffer) CStreamInfo(m_dSpeed, m_bTrickMode); // Using "placement new" ctor
            sFrame->SetData(sBuffer, 0, sizeof(CStreamInfo));
            m_qQueueSink->InsertFrame(sFrame);
        }
        else if (pSFrame->GetMessageType() == CSFrame::GapPresent) {
            // Already adjusted for gaps, immediately send a
            // StartOfStream packet and arrange for the next real
            // packet that will go out to have the discontinuity bit set
            SYSLOG_DEBUG("Gap Acknowledged: %"PRIu64", %"PRIu64"\n",
                         m_nGapAdjust, m_nTimestamp);
            m_nTimestamp = pSFrame->GetTimeStamp();
            m_nFrameWallClock = CTimeKeeper::Now();
            m_nDiscontinuity = true;
            CSFrame *pSFrame = new CSFrame(m_nTimestamp, CSFrame::StartOfStream);
            if (pSFrame != NULL)
                m_qQueueSink->InsertFrame(pSFrame);
        }
        else if (pSFrame->GetMessageType() == CSFrame::EndOfStream) {
            // We have reached a start of gap. Just update the
            // current timestamp
            m_nTimestamp = pSFrame->GetTimeStamp();
            m_nFrameWallClock = CTimeKeeper::Now();
            SYSLOG_DEBUG("EndOfStream: A gap is about to start: %"PRIu64"",
                         m_nTimestamp);
        }
        else if (pSFrame->GetMessageType() == CSFrame::EndOfMedia) {
            SYSLOG_DEBUG("Notification: EndOfMedia");
            m_bSendNotify = true;
            m_nEventId = NOTIFICATION_EVENT_ENDOFMEDIA;
            SetDisConnected();
        }
        else if (pSFrame->GetMessageType() == CSFrame::MediaError) {
            SYSLOG_DEBUG("Notification: MediaError");
            SetDisConnected();
            ostringstream os;
            os << pSFrame->GetData();
            SendErrorNotification(NOTIFICATION_ERROR_FILE_READ, os);
            SetError();
        }
        else if (pSFrame->GetMessageType() == CSFrame::SystemError) {
            SYSLOG_DEBUG("Notification: SystemError");
            SetDisConnected();
            ostringstream os;
            os << pSFrame->GetData();
            SendErrorNotification(NOTIFICATION_ERROR_OUT_OF_MEMORY, os);
            SetError();
        }
        else {
            SYSLOG_ERROR("Unknown message type %d\n",
                         pSFrame->GetMessageType());
            ASSERT(0);
        }
        m_qDiskIOSource->ReleaseFrame(pSFrame);

        m_drainCount++;
        if (m_maxQueueDrain < m_drainCount)
            m_maxQueueDrain = m_drainCount;
        return 1;  // need to check more frames

    }

EXIT_LABEL:
    m_drainCount = 0;
    return 0;
}



/* Static method to retrieve the timecode for a media */
void
CReader::GetTimecodeAtOffset(CCommand *a_pCmd,
                             char * pMediaName,
                             char * pMediaDir,
                             timestamp_t nOffSet)
{


    // Create and start a ReaderQuery worker thread to handle this
    // request
    CReaderQueryThread * pReaderQueryThread =
        new CReaderQueryThread(a_pCmd,
                               pMediaName,
                               pMediaDir,
                               COMMAND_TC_AT_OFFSET,
                               nOffSet);
    pReaderQueryThread->Start();

    return;
}

/* Static method to get the offset from timecode for a media */
void
CReader::GetOffsetAtTimecode(CCommand *a_pCmd,
                             char * pMediaName,
                             char * pMediaDir,
                             timestamp_t nTimecode)
{

    // Create and start a ReaderQuery worker thread to handle this
    // request
    CReaderQueryThread * pReaderQueryThread =
        new CReaderQueryThread(a_pCmd,
                               pMediaName,
                               pMediaDir,
                               COMMAND_OFFSET_AT_TC,
                               nTimecode);
    pReaderQueryThread->Start();
    return;
}



void
CReader::setStartCounter(SStartCounter * a_startCounter)
{
    ASSERT(m_startCounter == NULL);
    ASSERT(a_startCounter != NULL);
    m_startCounter = a_startCounter;
}

int
CReader::StartStream(int nQueueId)
{

    ASSERT(m_qQueueSink == NULL);
    ostringstream qid;
    qid << "Queue_" << m_nQueueId;
    m_qQueueSink = CSQueueManager::Instance()->GetQueueSink(qid.str());

    m_nSessionStartTime = 0;
    m_ready = false;
    m_nDiscontinuity = true;

    SYSLOG_DEBUG("Frames available at start = %d\n", m_qDiskIOSource->Size());

    return 0;
};

int
CReader::StopStream()
{

    m_nSessionStartTime = 0;

    if (m_qQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qQueueSink);
    m_qQueueSink = NULL;

    m_startCounter = NULL;

    // Stop the diskio thread
    if (m_pReaderDiskIOThread != NULL) {
        m_pReaderDiskIOThread->Stop();
        m_pReaderDiskIOThread = NULL;
    }

    return 0;
}


void
CReader::PrintStatistics(string &a_stats)
{

    m_savedMaxQueueDrain = m_maxQueueDrain;
    m_savedMinQueueSize = m_minQueueSize;
    m_savedMaxQueueSize = m_maxQueueSize;
    m_maxQueueDrain = 0;
    m_minQueueSize = 100000;
    m_maxQueueSize = 0;

}

void
CReader::UpdateCustomStatus(CCommand *a_pCmd)
{


    a_pCmd->SetObjectGuageParam("MaxQueueDrain", m_savedMaxQueueDrain);
    a_pCmd->SetObjectGuageParam("MinQueueSize", m_savedMinQueueSize);
    a_pCmd->SetObjectGuageParam("MaxQueueSize", m_savedMaxQueueSize);
    if (m_qDiskIOSource != NULL) {
        a_pCmd->SetObjectErrorCounterParam("DIOQueueDrops",
                m_qDiskIOSource->DropCount());
        a_pCmd->SetObjectByterateGuageParam("DIOQueueBytes",
                m_qDiskIOSource->ByteCount());
    }
#ifdef SSDEBUG
    if (m_qDiskIOSource != NULL) {
        a_pCmd->SetObjectParam("AverageQueueLength",
                               (double)m_qDiskIOSource->AvgSize());
        a_pCmd->SetObjectByterateGuageParam("AverageQueueSize",
                                            (int)m_qDiskIOSource->AvgBytes());
        a_pCmd->SetObjectCounterParam("DIOSrcContention",
                                      m_qDiskIOSource->GetContention());
    }

    if (m_qQueueSink != NULL) {
        a_pCmd->SetObjectCounterParam("SinkContention",
                                      m_qQueueSink->GetContention());
    }
#endif

    a_pCmd->SetObjectParam("GapAdjust", (timestamp_t) m_nGapAdjust);
    a_pCmd->SetObjectCounterParam("GapCount", m_nGapServedCount);

    a_pCmd->SetObjectParam("Speed", m_dSpeed);

    // DiskIO thread fills in the rest
    if (m_pReaderDiskIOThread != NULL)
        m_pReaderDiskIOThread->UpdateCustomStatus(a_pCmd);
}


void
CReader::PrintError(CNotify *a_notify)
{
}

void
CReader::PrintNotification(CNotify *a_notify)
{

}




