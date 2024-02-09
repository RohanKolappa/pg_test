#ifndef __H_SQUEUE__
#define __H_SQUEUE__


#include <string>
#include <list>
#include <map>

#include "sframe.hh"
#include "mutex.hh"


// EXPERIMENTAL LOCK-FREE Queue implementation
// Work in progress. Enable by commenting in the LOCK_FREE hashdef

//#define LOCK_FREE


/**
 * Base class for queues. Supposed to provide the common interface for
 * CSQueueSource and CSQueueSink classes. Currently does nothing
 * expect declare common member variables.
 */

class CSQueue {

public:
    CSQueue(std::string a_queueId);
    virtual ~CSQueue();

    /// Get queue ID
    std::string GetQueueId() {return m_queueId;};

    virtual void InsertFrame(CSFrame * a_sFrame) = 0;

    unsigned long GetContention() {return m_mutex.GetContention();};

protected:


    /// Id of the queue this belongs to
    std::string                          m_queueId;

    /// Mutex protecting the queue
    CMutex                          m_mutex;

    CSQueue(const CSQueue& rhs);
    CSQueue& operator=(const CSQueue& rhs);

};

class CSQueueSink;

/**
 * Class used to retrieve items from a queue. Clients use GetFrame()
 * and ReleaseFrame() to retrieve messages in the queue.
 */
class CSQueueSource : public CSQueue {
public:
    CSQueueSource(std::string a_queueId,
                  CMutex& a_mutex);
    ~CSQueueSource();

    CSFrame * GetFrame();
    void ReleaseFrame(CSFrame * a_sFrame);
    void InsertFrame(CSFrame * a_sFrame);

    /// Get queue size
    int Size() const {return m_size;};

    /// Set maximum size of the queue (to prevent memory leaks for
    /// runaway streams)
    void SetMaxSize(int a_size) { m_maxSize = a_size; };

    /// Set maximum bytes allowed in the queue (to prevent memory leaks for
    /// runaway streams)
    void SetMaxBytes(unsigned long a_maxBytes) { m_maxBytes = a_maxBytes; };

    /// Get the configured max size
    int GetMaxSize() const { return m_maxSize; };

    /// Get the configured max bytes
    int GetMaxBytes() const { return m_maxBytes; };

    /// Get the number of bytes in this queue 
    unsigned long ByteCount() const { return m_bytes; };

    /// Set the owner. Used for debugging.
    void SetOwner(int a_id) { m_owner = a_id; };

    /// Get the owner. Used for debugging.
    int GetOwner() const { return m_owner; };

    /// Get the average queue length. Used for debugging.
    double AvgSize() const;

    /// Get the average queue size in bytes. Used for debugging.
    double AvgBytes() const;

    /// Get the max queue length. Used for debugging.
    int MaxLength();

    /// Get the total number of incoming frames
    unsigned long FrameCount() const;

    /// Get the number of drops so far
    int DropCount() const { return m_drops; };

    /// Get the queue sink associated with this source
    CSQueueSink *GetSink();

private:
    /// Reference to the sink's mutex
    CMutex&                         m_sinkMutex;

    /// The list of frames
    std::list<CSFrame *>                 m_frameList;

#ifdef LOCK_FREE

private:
    struct FrameNode {
        FrameNode(CSFrame * a_frame) : m_pValue(a_frame), m_pNext(NULL) { };
        CSFrame *m_pValue;
        FrameNode* m_pNext;
    };
    FrameNode * m_pFirst;
    FrameNode * m_pDivider;
    FrameNode * m_pLast;

#endif

    /// Current number of frames in queue
    int                             m_size;

    /// The maximum size of elements allowd in this queue
    int                             m_maxSize;

    /// Current number of bytes in queue
    unsigned long                   m_bytes;

    /// The maximum number of bytes allowd in this queue
    unsigned long                   m_maxBytes;

    /// Drop counter
    int                             m_drops;

    /// Object id of the source. Used for debugging
    int                             m_owner;

    /// Max length of queue. Used for debugging
    int                             m_maxLength;

    /// Average length of queue. Used for debugging
    double                             m_avgLength;

    /// Average Bytes in queue. Used for debugging
    double                             m_avgBytes;

    /// Total frames enqueued
    unsigned long                             m_inCount;

    /// The sink associated with this source
    CSQueueSink *                   m_qQueueSink;

    CSQueueSource();
    CSQueueSource(const CSQueueSource& rhs);
    CSQueueSource& operator=(const CSQueueSource& rhs);

};

/**
 * Class used to insert frames into a queue. Clients use InsertFrame()
 * to enqueue messages.
 */
class CSQueueSink : public CSQueue {
public:
    CSQueueSink(std::string a_queueId);
    ~CSQueueSink();

    void InsertFrame(CSFrame * a_sFrame);

    // Create and insert a simple frame into this queue
    void InsertFrame(const char *a_cData, int a_len) {
        CSFrame *sFrame = new CSFrame(a_cData, a_len);
        InsertFrame(sFrame);
    }

    int Size();

    /// Increase reference count. Used by Queue Manager.
    void IncreaseRef() { ++m_refCount; };

    /// Decrease reference count. Used by Queue Manager.
    void DecreaseRef() { --m_refCount; };

    /// Get reference count. Used by Queue Manager.
    int GetRef() { return m_refCount; };

    /// Get number of listeners. Used by Queue Manager.
    int GetListenerCount() { return m_listenerCount; };

    CSQueueSource *CreateListener();
    void RemoveListener(CSQueue *a_queue);
    void AddListener(CSQueue *a_queue);

    /// Get Timestamp of the last frame inserted into this queue
    timestamp_t GetLastFrameTimestamp() {return m_nLastFrameTimestamp; };

    /// Get WallClock when the last frame was inserted into this queue
    timestamp_t  GetLastFrameWallClock() {return m_nLastFrameWallClock; };

private:
    /// The list of active listeners (queue sources).
    std::list<CSQueue *>           m_queueListeners;
    /// The number of active listeners
    int                             m_listenerCount;
    /// The reference count of the number of clients that have a
    /// handle on this queue
    int                             m_refCount;

    /// Timestamp of the last frame inserted into this queue
    timestamp_t m_nLastFrameTimestamp;

    /// WallClock when the last frame was inserted into this queue
    timestamp_t  m_nLastFrameWallClock;

    CSQueueSink();
    CSQueueSink(const CSQueueSink& rhs);
    CSQueueSink& operator=(const CSQueueSink& rhs);

};


// Class CSQueueManager

/**
 * All queues are accessed via the CSQueueManager class. There is only
 * one instance of this in the application and hence this is
 * implemented as a Singleton. Clients use GetQueueSource and
 * GetQueueSink that are factory methods to create the corresponding
 * queue objects given a queue id (any string). Queue objects are
 * destroyed solely via ReleaseQueueSource and ReleaseQueueSink.
 */
class CSQueueManager {
public:
    static CSQueueManager * Instance();
    void Destroy();


    CSQueueSource *GetQueueSource(std::string a_queueId);
    CSQueueSink *GetQueueSink(std::string a_queueId);
    bool HasQueueSink(std::string a_queueId);
    int NumberOfSinks() {return m_queueSinkCount;};
    int NumberOfSources() {return m_queueSourceCount;};
    int NumberOfPipes() {return m_pipedSourceCount;};
    bool CreatePipe(int a_id, std::string a_sinkQueueId, std::string a_sourceQueueId);
    bool DeletePipe(int a_id);
    void ReleaseQueueSource(CSQueueSource * a_queueSource);
    void ReleaseQueueSink(CSQueueSink * a_queueSink);
    bool HasActiveSink(CSQueueSource * a_queueSource);
    CSQueueSink *GetSink(CSQueueSource * a_queueSource);
    void PrintStatistics();

private:
    /// The current number of queue objects being managed
    int m_queueSinkCount;
    int m_queueSourceCount;
    int m_pipedSourceCount;

    class CPipe {
    public:
        std::string m_sinkQueueId;
        std::string m_sourceQueueId;
        CSQueueSink *m_queueSink;
        CSQueueSink *m_queueSourceSink;
    };

    /// Synchronization mutex
    CMutex m_mutex;

    /// The list of queue sinks being managed
    std::map<std::string, CSQueueSink *> m_queueMap;

    /// The list of queue sources being managed
    std::list<CSQueueSource *>           m_queueSources;

    /// The list of pipes being managed
    std::map<int, CPipe *> m_pipeMap;

    /// Singleton instance
    static CSQueueManager *m_queueManager;

    /// Private constructor
    CSQueueManager();

    /// Private destructor
    ~CSQueueManager();

    CSQueueManager(const CSQueueManager& rhs);
    CSQueueManager& operator=(const CSQueueManager& rhs);

};

#endif
