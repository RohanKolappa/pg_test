
#include "strmsrv_compatibility.h"
#include <sstream>

#include "sender.hh"
#include "squeue.hh"
#include "common.hh"

using namespace std;

CSender::CSender(int a_nId) :
    CStreamer(a_nId)
{
}

CSender::~CSender()
{
    if (m_qQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
    if (m_qQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qQueueSink);
}

int
CSender::StartSenderStream(int nQueueId)
{
    // Default implementation does nothing.
    // Subclasses customize as needed.
    return 0;
}

/// Assign a Queue to the Sender
int
CSender::StartStream(int nQueueId)
{
    ASSERT(m_qQueueSource == NULL);
    ostringstream qid;
    qid << "Queue_" << nQueueId;
    m_qQueueSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qQueueSource->SetOwner(m_nId);

    ASSERT(m_qQueueSink == NULL);
    ostringstream altqid;
    altqid << "AltQueue_" << nQueueId;
    m_qQueueSink = CSQueueManager::Instance()->GetQueueSink(altqid.str());

    StartSenderStream(nQueueId);

    return 0;
};

/// Remove the Queue Assignment
int
CSender::StopStream()
{
    // Call sender specific Stop Hook that requires the queue
    StoppingSender();

    if (m_qQueueSource != NULL) {
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
        m_qQueueSource = NULL;
    }

    if (m_qQueueSink != NULL) {
        CSQueueManager::Instance()->ReleaseQueueSink(m_qQueueSink);
        m_qQueueSink = NULL;
    }

    // Call sender specific Stop Hook
    return StopSender();
}

void
CSender::StoppingSender()
{
    // Default implementation does nothing, subclasses overide as needed
}

int
CSender::StopSender()
{
    // Default implementation does nothing, subclasses overide as needed
    return 0;
}

void
CSender::HandleFirstTimestamp(timestamp_t nFirstTimeStamp)
{
    // Subclasses may overide this method but must call this
    // superclass method before they do anything else
    m_nTimestamp = nFirstTimeStamp;
    m_nStartTimestamp = m_nTimestamp;
    m_nFrameWallClock = CTimeKeeper::Now();
    m_nStartWallClock = m_nFrameWallClock;
    m_bSendNotify = true;
    m_nEventId = NOTIFICATION_EVENT_BEGIN;


    return;
}

void
CSender::CheckQueueTimestamp()
{

    ASSERT(m_qQueueSource != NULL);
    ASSERT(m_nTimestamp == 0);

    CSQueueSink * qSink = m_qQueueSource->GetSink();
    if (qSink->GetLastFrameTimestamp() != 0) {
        timestamp_t currentTimeStamp =
            qSink->GetLastFrameTimestamp() +
            (CTimeKeeper::Now() - qSink->GetLastFrameWallClock());
        // Call subclass specific code to set the first time stamp
        HandleFirstTimestamp(currentTimeStamp);
    }
}

void
CSender::CheckSyncStop()
{
    if (m_nStopTime > 0 &&
        (GetCurrentTimeStamp() > m_nStopTime || GetCurrentTimeStamp() == 0) &&
        m_qQueueSource != NULL) {
        Stop();
    }
}
