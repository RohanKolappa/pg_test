
#include "strmsrv_compatibility.h"
#include <sstream>

#include "receiver.hh"
#include "common.hh"

using namespace std;

CReceiver::CReceiver(int a_nId) :
    CStreamer(a_nId)
{

}

CReceiver::~CReceiver()
{

    if(m_qQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
    if(m_qQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qQueueSink);

}

/// Assign a Queue to the Receiver
int
CReceiver::StartStream(int nQueueId)
{
    ASSERT(m_qQueueSink == NULL);
    ostringstream qid;
    qid << "Queue_" << nQueueId;
    m_qQueueSink = CSQueueManager::Instance()->GetQueueSink(qid.str());

    ASSERT(m_qQueueSource == NULL);
    ostringstream altqid;
    altqid << "AltQueue_" << nQueueId;
    m_qQueueSource = CSQueueManager::Instance()->GetQueueSource(altqid.str());
    m_qQueueSource->SetOwner(m_nId);


    // Insert a "StartOfStream" packet as soon as the output queue is
    // started
    timestamp_t nStreamStartTimestamp = CTimeKeeper::Now();
    CSFrame *pSFrame = new CSFrame(nStreamStartTimestamp, CSFrame::StartOfStream);
    if (pSFrame != NULL)
        m_qQueueSink->InsertFrame(pSFrame);

    return 0;
};

/// Remove the Queue Assignment
int
CReceiver::StopStream()
{

    // Insert a "EndOfStream" packet into the output queue just before
    // tearing down the queue
    if(m_qQueueSink != NULL) {
        CSFrame *pSFrame = new CSFrame(m_nStopWallClock, CSFrame::EndOfStream);
        if (pSFrame != NULL)
            m_qQueueSink->InsertFrame(pSFrame);
    }

    if(m_qQueueSource != NULL) {
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
        m_qQueueSource = NULL;
    }
    if(m_qQueueSink != NULL) {
        CSQueueManager::Instance()->ReleaseQueueSink(m_qQueueSink);
        m_qQueueSink = NULL;
    }

    return 0;
}


