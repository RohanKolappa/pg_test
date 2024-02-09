
#include "strmsrv_compatibility.h"
#include "common.hh"
#include "squeue.hh"

using namespace std;

/**
 * Constructor. Saves the queueid.
 */
CSQueue::CSQueue(string a_queueId) :
    m_queueId(a_queueId)
{
}


/**
 * Destructor. Does nothing.
 */
CSQueue::~CSQueue()
{
}

// CSQueueSource class


/**
 * Constructor. Initiaializes a reference to the mutex for the
 * associated sink.
 */
CSQueueSource::CSQueueSource(string a_queueId, CMutex& a_mutex) :
    CSQueue(a_queueId),
    m_sinkMutex(a_mutex),
    m_size(0),
    m_maxSize(2500),
    m_bytes(0),
    m_maxBytes (1024 * 1024/2),
    m_drops(0),
    m_owner(-1),
    m_qQueueSink(NULL)
{
    m_maxLength = 0;
    m_avgLength = 0.0;
    m_avgBytes = 0.0;
    m_inCount = 0;


#ifdef LOCK_FREE
    // Insert a  dummy frame in the queue
    CSFrame * frame = new CSFrame("Dummy", 5);
    m_pFirst = m_pLast = m_pDivider = new FrameNode(frame);

#endif
}

/**
 * Destructor. Deletes any remaining frames in the queues.
 */
CSQueueSource::~CSQueueSource()
{
    CScopedLock lock(m_mutex);

#ifndef LOCK_FREE
    while (!m_frameList.empty()) {
        CSFrame * sFrame = m_frameList.front();
        m_frameList.pop_front();
        sFrame->DecreaseRef();
        // delete the frame if no one has any reference to it.
        if (sFrame->GetRef() <= 0) {
            delete sFrame;
        }
    }
#else // LOCK_FREE
    while( m_pFirst != NULL ) {   // release the list
        FrameNode* tmpNode = m_pFirst;
        m_pFirst = tmpNode->m_pNext;
        CSFrame *sFrame = tmpNode->m_pValue;
        sFrame->DecreaseRef();
        if (sFrame->GetRef() <= 0) {
            delete sFrame;
        }
        delete tmpNode;
    }

#endif // LOCK_FREE

}

/**
 * Inserts a frame in the queue. Clients should not call this method
 * directly. Only the associated queue sink calls this (should be a
 * friend??) to insert a frame in the queue.
 */
#ifndef LOCK_FREE
void
CSQueueSource::InsertFrame(CSFrame * a_sFrame) {

    ASSERT(a_sFrame != NULL);

    /* During development we want to monitor when max size is
       reached. So we assert on the condition. For production
       release we ignore the frame if max size is reached. */

#ifdef SSDEBUG
//      if(m_size >= m_maxSize) {
//          SYSLOG_INFO("Exceeded queue capacity for %s:%d. Owner=%d \n",
//                       m_queueId.c_str(), m_maxSize, m_owner);
//          ASSERT(0);
//      }
#endif

    if (m_bytes >= m_maxBytes) {
        m_drops++;
        return;
    }
    if (m_size >= m_maxSize) {
        m_drops++;
        return;
    }

    /* Insert the packet */
    {
        CScopedLock lock(m_mutex);
        m_frameList.push_back(a_sFrame);
        a_sFrame->IncreaseRef();
        ++m_size;
        m_bytes += a_sFrame->GetLength();
#ifdef SSDEBUG
        m_inCount++;
#endif
    }
}

#else // LOCK_FREE

void
CSQueueSource::InsertFrame(CSFrame * a_sFrame) {

    ASSERT(a_sFrame != NULL);

    if (m_bytes >= m_maxBytes) {
        m_drops++;
        return;
    }
    if (m_size >= m_maxSize) {
        m_drops++;
        return;
    }

    // Add the element to the end of our queue
    m_pLast->m_pNext = new FrameNode(a_sFrame);

    a_sFrame->IncreaseRef();

    __sync_fetch_and_add(&m_size, 1);
    __sync_fetch_and_add(&m_bytes, a_sFrame->GetLength());

    // Point tail to the newly inserted element
    __sync_synchronize(); // Memory barrier
    m_pLast = m_pLast->m_pNext;

    // Delete all elements already consumed by the consumer
    while( m_pFirst != m_pDivider ) {
        FrameNode* tmpNode = m_pFirst;
        m_pFirst = tmpNode->m_pNext;
        CSFrame *sFrame = tmpNode->m_pValue;
        sFrame->DecreaseRef();
        if (sFrame->GetRef() <= 0) {
            delete sFrame;
        }
        delete tmpNode;
    }


#ifdef SSDEBUG
    m_inCount++;
#endif
}
#endif // LOCK_FREE

/**
 * Retrieve a frame from the queue. Does not actually dequeue a
 * message (behaves like a peek). Clients need to follow up with a
 * ReleaseFrame() to actually dequeue a message
 */
#ifndef LOCK_FREE
CSFrame *
CSQueueSource::GetFrame() {
    if (m_size == 0)
        return NULL;
    return m_frameList.front();
}

#else // LOCK_FREE

CSFrame *
CSQueueSource::GetFrame() {

    if (m_pDivider != m_pLast) {
        return m_pDivider->m_pNext->m_pValue;
    }
    return NULL;
}

#endif

/*
 * Release a previously retrieved frame (see GetFrame()). Clents
 * cannot retrieve another frame unless the last frame is released.
 */
#ifndef LOCK_FREE
void
CSQueueSource::ReleaseFrame(CSFrame * a_sFrame) {

    ASSERT(m_size > 0);
    ASSERT(a_sFrame == m_frameList.front());

    // First acquire the source queue lock and pop the frame
    {
        CScopedLock lock(m_mutex);
        --m_size;
        m_bytes -= m_frameList.front()->GetLength();
        m_frameList.pop_front();
    }
    // Delete the frame if no one has any reference to it.
    // To decrement the ref count and delete the frame, need to
    // acquire the corresponding sink queue lock
    {
        CScopedLock lock(m_sinkMutex);
        a_sFrame->DecreaseRef();
        if (a_sFrame->GetRef() <= 0)
            delete a_sFrame;
    }

#ifdef SSDEBUG
    // Compute average queue length. (Exponential moving average)
#define ALPHA 0.05
    m_avgLength = (double)ALPHA * m_size + (double)(1.0 - ALPHA) * m_avgLength;
    m_avgBytes = (double)ALPHA * m_bytes + (double)(1.0 - ALPHA) * m_avgBytes;
#endif
    // Compute max queuelength
    if (m_maxLength < m_size)
        m_maxLength = m_size;
}

#else // LOCK_FREE

void
CSQueueSource::ReleaseFrame(CSFrame * a_sFrame) {

    ASSERT(m_size > 0);
    ASSERT(m_pDivider != m_pLast);
    ASSERT(a_sFrame == m_pDivider->m_pNext->m_pValue);

    // Atomic Decrement size and bytes stats
    __sync_fetch_and_add(&m_size, -1);
    __sync_fetch_and_add(&m_bytes, -(a_sFrame->GetLength()));

    // Publish our consumption
    __sync_synchronize(); // Memory barrier
    m_pDivider = m_pDivider->m_pNext;


#ifdef SSDEBUG
    // Compute average queue length. (Exponential moving average)
#define ALPHA 0.05
    m_avgLength = (double)ALPHA * m_size + (double)(1.0 - ALPHA) * m_avgLength;
    m_avgBytes = (double)ALPHA * m_bytes + (double)(1.0 - ALPHA) * m_avgBytes;
#endif
    // Compute max queuelength
    if (m_maxLength < m_size)
        m_maxLength = m_size;
}
#endif //LOCK_FREE

/*
 * Retrieve the average length of the queue
 */
double
CSQueueSource::AvgSize() const {
#ifdef SSDEBUG
    return m_avgLength;
#endif
    return 0.0;
}

double
CSQueueSource::AvgBytes() const {
#ifdef SSDEBUG
    return m_avgBytes;
#endif
    return 0.0;
}

int
CSQueueSource::MaxLength() {
    // return current maxlength and reset it to zero
    int maxLength = m_maxLength;
    m_maxLength = 0;
    return maxLength;
}

unsigned long
CSQueueSource::FrameCount() const {
#ifdef SSDEBUG
    return m_inCount;
#endif
    return 0;
}

CSQueueSink *
CSQueueSource::GetSink() {

    // Lazy initialization of this source's Sink
    if (m_qQueueSink == NULL) {
        m_qQueueSink = CSQueueManager::Instance()->GetSink(this);
    }
    return m_qQueueSink;
}

// CSQueueSink class

/**
 * Constructor
 */
CSQueueSink::CSQueueSink(string a_queueId) :
    CSQueue(a_queueId),
    m_listenerCount(0),
    m_refCount(0),
    m_nLastFrameTimestamp(0),
    m_nLastFrameWallClock(0)
{
}

/**
 * Destructor
 */
CSQueueSink::~CSQueueSink()
{

    // just release the memory associated with the listener list.
    // we do not cleanup the queue sources
    m_queueListeners.clear();

}

/**
 * Sends a message (inserts a frame into the queue). Clients call this
 * to send a frame to all listeners (queue sources). If there are no
 * listeners yet, the frame is deleted right away.
 */
void
CSQueueSink::InsertFrame(CSFrame * a_sFrame) {

    CScopedLock lock(m_mutex);

    // Remember the timestamps of the packet inserted into this queue
    m_nLastFrameTimestamp = a_sFrame->GetTimeStamp();
    m_nLastFrameWallClock = CTimeKeeper::Now();

    // If no listeners, delete frame
    if (m_listenerCount == 0) {
        if (a_sFrame->GetRef() <= 0)
            delete a_sFrame;
        return;
    }


    // Insert frame into all the sink queues
    list<CSQueue *>::iterator iter;
    for (iter = m_queueListeners.begin();
         iter != m_queueListeners.end(); ++iter) {
        (*iter)->InsertFrame(a_sFrame);
    }

    // If all sinks drop the frame, we delete it
    if (a_sFrame->GetRef() <= 0) {
        delete a_sFrame;
    }

}

/* Returns the current size of the Sink queue which is the maximum
   of all the sizes of the current listeners */
int
CSQueueSink::Size() {

    CScopedLock lock(m_mutex);

    // Size is the maximum size of all current listeners
    int size = 0;
    list<CSQueue *>::iterator iter;
    for (iter = m_queueListeners.begin();
         iter != m_queueListeners.end(); ++iter) {
        CSQueueSource * source = static_cast <CSQueueSource *> (*iter);
        if (size < source->Size())
            size = source->Size();
    }
    return size;
}

/**
 * Creates and adds a new listener (queue source) to this queue
 * sink. Not intended to be called directly by clients. Only the Queue
 * Manager calls this when clients request a queue source.
 */
CSQueueSource *
CSQueueSink::CreateListener() {
    CScopedLock lock(m_mutex);

    // Create a new Queue Source and add it to our current set of listeners
    CSQueueSource *sQueueSource = new CSQueueSource(m_queueId, m_mutex);
    if (sQueueSource != NULL) {
        m_queueListeners.push_back(sQueueSource);
        m_listenerCount++;
    }
    return sQueueSource;
}

/**
 * Adds a new listener (queue source) to this queue sink. Not intended
 * to be called directly by clients. Only the Queue Manager calls this
 * when clients request a queue source.
 */
void
CSQueueSink::AddListener(CSQueue *a_queue) {
    CScopedLock lock(m_mutex);

    ASSERT(a_queue != NULL);
    if (a_queue != NULL) {
        m_queueListeners.push_back(a_queue);
        m_listenerCount++;
    }
    // the size better be correct now
    ASSERT(m_listenerCount == (int)m_queueListeners.size());
}

/**
 * Remove a queue source from the current set of active listeners. Not
 * intended to be called directly by clients. Only the Queue Manager
 * calls this when clients release a queue source.
 */
void
CSQueueSink::RemoveListener(CSQueue *a_queue) {
    CScopedLock lock(m_mutex);

    // remove the source from our list of active listener's
    m_queueListeners.remove(a_queue);
    --m_listenerCount;

    // the size better be correct now
    ASSERT(m_listenerCount == (int)m_queueListeners.size());

}


// CSQueueManager class

/// Pointer to the singleton instance
CSQueueManager * CSQueueManager::m_queueManager;

/**
 * Constructor. Intitializes private data.
 */
CSQueueManager::CSQueueManager() :
    m_queueSinkCount(0),
    m_queueSourceCount(0),
    m_pipedSourceCount(0)

{
}

/**
 * Destructor. Nothing to do.
 */
CSQueueManager::~CSQueueManager()
{

}


/**
 * Access method to retrieve the singleton instance. Not yet thread safe
 * so needs to be called at least once before any threads are created.
 */
CSQueueManager *
CSQueueManager::Instance() {

    if (m_queueManager == NULL) {
        m_queueManager = new CSQueueManager();
    }

    return m_queueManager;
}

/**
 * Deletes the singleton object. Call once before end of application
 * to make valgrind happy.
 */
void
CSQueueManager::Destroy() {

    if (m_queueManager == NULL)
        return;

    // Give a chance for all objects to release their queues
    int count = 10;
    while (count-- > 0) {
        if (m_queueManager->m_queueSourceCount == 0 &&
                m_queueManager->m_queueSinkCount == 0)
            break;
        usleep(100000);
    }
    if (m_queueManager->m_queueSourceCount > 0 ||
            m_queueManager->m_queueSinkCount > 0) {
        printf("WARNING: QueueManager: Not all queues cleaned up: Sources: %d Sinks: %d\n",
                m_queueManager->m_queueSourceCount,
                m_queueManager->m_queueSinkCount);
    }

    delete m_queueManager;
    m_queueManager = NULL;
}

/**
 * Factory method to create and return a queue source for any queue
 * id. If a queue sink exists (some client requested a queue
 * sink/source for this id earlier), adds a new listener to that
 * sink. Else creates a new queue sink first and then adds the
 * listener.
 */
CSQueueSource *
CSQueueManager::GetQueueSource(string a_queueId) {

    CScopedLock lock(m_mutex);
    CSQueueSource * sQueueSource = NULL;

    // if queue sink does not yet exist create it and insert into
    // the map
    if (m_queueMap.count(a_queueId) == 0) {
        CSQueueSink *sQueueSink = new CSQueueSink(a_queueId);
        m_queueMap[a_queueId] = sQueueSink;
        m_queueSinkCount++;
        sQueueSource = sQueueSink->CreateListener();
        m_queueSourceCount++;
        m_queueSources.push_back(sQueueSource);
        SYSLOG_DEBUG("Creating a Source and a Sink Queue for %s\n",
                     a_queueId.c_str());
    }
    else {
        // queue sink already exists, ask the sink to create a
        // new listener source queue
        CSQueueSink *sQueueSink = m_queueMap[a_queueId];
        sQueueSource = sQueueSink->CreateListener();
        m_queueSourceCount++;
        m_queueSources.push_back(sQueueSource);
        SYSLOG_DEBUG("Created Source queue # %d for %s\n",
                     sQueueSink->GetListenerCount(),
                     a_queueId.c_str());
    }

    return sQueueSource;
}

/**
 * Factory method to create and return a queue sink for any queue
 * id. If a queue sink does not exist, creates a new one and adds it
 * to the private map. Else just returns the existing queue. Either
 * way it increments the reference count on the sink to keep track of
 * how many clients have a handle on the queue.
 */
CSQueueSink *
CSQueueManager::GetQueueSink(string a_queueId) {

    CScopedLock lock(m_mutex);
    CSQueueSink * sQueueSink = NULL;

    // if queue does not yet exist create it and insert into the map
    if (m_queueMap.count(a_queueId) == 0) {
        sQueueSink = new CSQueueSink(a_queueId);
        m_queueMap[a_queueId] = sQueueSink;
        m_queueSinkCount++;
        sQueueSink->IncreaseRef();
        SYSLOG_DEBUG("Creating Sink queue %s\n", a_queueId.c_str());
    }
    else {
        // queue already exists, just return that one
        sQueueSink = m_queueMap[a_queueId];
        sQueueSink->IncreaseRef();
        SYSLOG_DEBUG("Returning existing Sink queue %s\n", a_queueId.c_str());
    }

    return sQueueSink;

}

// Test if a queue with given ID exists
bool
CSQueueManager::HasQueueSink(string a_queueId) {

    CScopedLock lock(m_mutex);

    if (m_queueMap.count(a_queueId) == 0) {
        return false;
    }
    return true;

}

/**
 * Releases a queue source. Removes the source from the corresponding
 * sink and then deletes the source. Also deletes the corresponding
 * sink if no other clients are interested in it.
 */
void
CSQueueManager::ReleaseQueueSource(CSQueueSource * a_queueSource) {

    ASSERT(a_queueSource != NULL);

    CScopedLock lock(m_mutex);

    string queueId = a_queueSource->GetQueueId();
    SYSLOG_TRACE("Releasing Source Queue %s\n",
                 queueId.c_str());


    // A queue source has to have an associated sink
    // Ask the associated sink to remove this source from
    // its list of active listeners
    ASSERT(m_queueMap.count(queueId) != 0);
    CSQueueSink *queueSink = m_queueMap[queueId];
    ASSERT(queueSink != NULL);
    queueSink->RemoveListener(a_queueSource);

    --m_queueSourceCount;
    m_queueSources.remove(a_queueSource);
    delete a_queueSource;
    SYSLOG_DEBUG("Deleted Source Queue %s\n", queueId.c_str());

    // Delete sink if this source was the last listener and there are
    // no more clients referencing the sink
    if ((queueSink->GetRef() <= 0) &&
        (queueSink->GetListenerCount() <= 0)) {
        m_queueMap.erase(queueId);
        delete queueSink;
        --m_queueSinkCount;
        SYSLOG_DEBUG("Deleted Sink with Source for Queue %s\n",
                     queueId.c_str());
    }

}

/**
 * Releases a queue sink. First decrements the client ref count on the
 * sink. Then deletes the sink if there are no active sources
 * listening on the sink and no other clients are interested in it.
 */
void
CSQueueManager::ReleaseQueueSink(CSQueueSink * a_queueSink) {

    ASSERT(a_queueSink != NULL);

    CScopedLock lock(m_mutex);

    string queueId = a_queueSink->GetQueueId();
    SYSLOG_TRACE("Releasing Sink Queue %s\n",
                 queueId.c_str());

    // The sink better be in our map
    ASSERT(m_queueMap.count(queueId) != 0);

    // Decrease refcount of the sink and delete if no more references
    // and no more listeners
    a_queueSink->DecreaseRef();
    if ((a_queueSink->GetRef() <= 0) &&
        (a_queueSink->GetListenerCount() <= 0)) {
        m_queueMap.erase(queueId);
        delete a_queueSink;
        --m_queueSinkCount;
        SYSLOG_DEBUG("Deleted Sink Queue %s\n",
                     queueId.c_str());
    }
    else {
        SYSLOG_DEBUG("Released Reference for Sink Queue %s\n",
                     queueId.c_str());
    }

}

bool
CSQueueManager::CreatePipe(int a_id,
                           string a_sinkQueueId,
                           string a_sourceQueueId)
{

    // Cannot pipe to oneself
    if (a_sinkQueueId == a_sourceQueueId)
        return false;

    if (m_pipeMap.count(a_id) == 0) {
        SYSLOG_DEBUG("Creating Pipe %d: %s -> %s",
                     a_id,
                     a_sinkQueueId.c_str(),
                     a_sourceQueueId.c_str());
        // Create new pipe object
        CPipe *pipe = new CPipe();
        pipe->m_sinkQueueId = a_sinkQueueId;
        pipe->m_sourceQueueId = a_sourceQueueId;

        // Get a handle on the sink
        pipe->m_queueSink = GetQueueSink(a_sinkQueueId);

        // Get a handle on the sink for the source
        pipe->m_queueSourceSink = GetQueueSink(a_sourceQueueId);

        // Pipe it !!
        pipe->m_queueSink->AddListener(pipe->m_queueSourceSink);
        m_pipedSourceCount++;

        // Insert this pipe info into our map
        m_pipeMap[a_id] = pipe;

        return true;
    }
    return false;
}

bool
CSQueueManager::DeletePipe(int a_id)
{

    if (m_pipeMap.count(a_id) > 0) {
        ASSERT(m_pipeMap.count(a_id) == 1);
        SYSLOG_DEBUG("Deleting Pipe %d: %s -> %s",
                     a_id,
                     m_pipeMap[a_id]->m_sinkQueueId.c_str(),
                     m_pipeMap[a_id]->m_sourceQueueId.c_str());

        CPipe * pipe = m_pipeMap[a_id];
        ASSERT(pipe != NULL);
        ASSERT(pipe->m_queueSink != NULL);
        ASSERT(pipe->m_queueSourceSink != NULL);
        pipe->m_queueSink->RemoveListener(pipe->m_queueSourceSink); // unpipe it
        ReleaseQueueSink(pipe->m_queueSink);
        ReleaseQueueSink(pipe->m_queueSourceSink);
        m_pipedSourceCount--;
        delete(pipe);
        m_pipeMap.erase(a_id);
        return true;
    }
    return false;
}

/**
 * Given a queue source, returns a pointer to the sink associated with it
 */
CSQueueSink *
CSQueueManager::GetSink(CSQueueSource * a_queueSource) {

    ASSERT(a_queueSource != NULL);

    CScopedLock lock(m_mutex);

    string queueId = a_queueSource->GetQueueId();

    // A queue source has to have an associated sink
    ASSERT(m_queueMap.count(queueId) != 0);
    CSQueueSink *queueSink = m_queueMap[queueId];
    ASSERT(queueSink != NULL);
    return queueSink;

}

/**
 * Does a source have an active sink? Given a queue source, returns
 * true if there are any active sinks associated with it
 */
bool
CSQueueManager::HasActiveSink(CSQueueSource * a_queueSource) {

    ASSERT(a_queueSource != NULL);

    CScopedLock lock(m_mutex);

    string queueId = a_queueSource->GetQueueId();

    // A queue source has to have an associated sink
    ASSERT(m_queueMap.count(queueId) != 0);
    CSQueueSink *queueSink = m_queueMap[queueId];
    ASSERT(queueSink != NULL);

    // Is the sink still being referenced??
    if (queueSink->GetRef() > 0)
        return true;

    return false;

}


/**
 * Print Statistics for all the queues being managed
 */
void
CSQueueManager::PrintStatistics() {

    CScopedLock lock(m_mutex);

    // List all the queue sources
    // Assert that our queue source count is correct and that
    // we have a queue sink for all the queue sources in our map
    SYSLOG_DEBUG("QSTATS: Source Total = %d\n",
                 m_queueSourceCount);
    ASSERT(m_queueSourceCount == (int)m_queueSources.size());

    list<CSQueueSource *>::iterator iter;
    for (iter = m_queueSources.begin();
         iter != m_queueSources.end(); ++iter) {
        CSQueueSource *sQueueSource = (*iter);
        ASSERT(m_queueMap.count(sQueueSource->GetQueueId()) != 0);
        SYSLOG_DEBUG("QSTATS: Source: Id=%s, Owner=%d, InFrames=%lu, MaxLength=%d, AvgLength=%.1f, Contention=%lu",
                     (sQueueSource->GetQueueId()).c_str(),
                     sQueueSource->GetOwner(),
                     sQueueSource->FrameCount(),
                     sQueueSource->MaxLength(),
                     sQueueSource->AvgSize(),
                     sQueueSource->GetContention());

    }

    // List all queue sinks
    // Assert that our queue sink count is correct and that
    // the total number of listeners for each sink adds up to
    // our queue source count
    SYSLOG_DEBUG("QSTATS: Sink Total = %d\n",
                 m_queueSinkCount);
    ASSERT(m_queueSinkCount == (int)m_queueMap.size());
    map<string, CSQueueSink *>::iterator mapiter;
    for (mapiter = m_queueMap.begin();
         mapiter != m_queueMap.end(); ++mapiter) {
        CSQueueSink *sQueueSink = (*mapiter).second;
        SYSLOG_DEBUG("QSTATS: Sink: Id=%s, Listeners=%d, Contention=%lu",
                     (sQueueSink->GetQueueId()).c_str(),
                     sQueueSink->GetListenerCount(),
                     sQueueSink->GetContention());
    }
    SYSLOG_DEBUG("QSTATS: Sink Total Listeners= %d",
                 m_queueSourceCount);
    SYSLOG_DEBUG("QSTATS: Sink Shared Listeners= %d",
                 m_pipedSourceCount);
    //ASSERT(totalListeners == (m_queueSourceCount + m_pipedSourceCount));

    // List all pipes
    SYSLOG_DEBUG("QSTATS: Pipes Total = %d\n", m_pipeMap.size());
    map<int, CPipe *>::iterator pipeiter;
    for (pipeiter = m_pipeMap.begin();
             pipeiter != m_pipeMap.end(); ++pipeiter) {

        SYSLOG_DEBUG("QSTATS: Pipe %d: %s -> %s",
                     (*pipeiter).first,
                     (*pipeiter).second->m_sinkQueueId.c_str(),
                     (*pipeiter).second->m_sourceQueueId.c_str());
    }


}
