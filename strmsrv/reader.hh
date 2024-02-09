#ifndef __H_READER__
#define __H_READER__

#include "diskio.hh"
#include "MediaIndexUtils.hh"
#include "squeue.hh"
#include "streamer.hh"
#include "diskiothr.hh"

class SStartCounter {
private:
    int                     m_refCount;
    int                     m_readyCount;
protected:
    ~SStartCounter() {};
public:
    SStartCounter(int a_refcout) :
            m_refCount(a_refcout), m_readyCount(a_refcout) {};
    void Ready() {
        m_readyCount--;
    };
    int IsReady() {
        return m_readyCount <= 0;
    }
    void Release() {
        if (--m_refCount <= 0) delete this;
    }; // self destruct
};

// Forward declarations
class CReaderDiskIOThread;

/// The Reader  Class
class CReader : public CStreamer {
public:
    CReader(int nId,
            const char *pMedianame,
            const char *pMediaDir,
            timestamp_t nTimecode,
            timestamp_t nOffSet,
            const char * speed);

    int ProcessStream();
    int StartStream(int nQueueId);
    int StopStream();
    void DeleteStream();

    void PrintStatistics(std::string& a_stats);
    void PrintError(CNotify *a_notify);
    void PrintNotification(CNotify *a_notify);

    int InitializeDiskIO();

    void setStartCounter(SStartCounter * a_startCounter);
    void UpdateCustomStatus(CCommand *a_pCmd);
    static void GetTimecodeAtOffset(CCommand *a_pCmd,
                                    char * pMedianame,
                                    char * pMediaDir,
                                    timestamp_t nOffSet);
    static void GetOffsetAtTimecode(CCommand *a_pCmd,
                                    char * pMedianame,
                                    char * pMediaDir,
                                    timestamp_t nTimecode);

    unsigned long GetQueueSourceByteCount() {
        return m_qDiskIOSource->ByteCount();
    }

private:
    CSQueueSource * m_qDiskIOSource;
    CReaderDiskIOThread * m_pReaderDiskIOThread;
    SStartCounter * m_startCounter;
    bool m_ready;
    timestamp_t m_nStartTime;
    timestamp_t m_nSessionStartTime;
    timestamp_t m_nGapAdjust;
    int m_nDiscontinuity;
    int m_drainCount;
    int m_maxQueueDrain;
    int m_minQueueSize;
    int m_maxQueueSize;
    int m_savedMaxQueueDrain;
    int m_savedMinQueueSize;
    int m_savedMaxQueueSize;
    unsigned long m_nGapServedCount;

    bool ReadytoStart();
    void ConfigureSpeed(const char * speed);

    ~CReader();

};


#endif /* __H_READER__ */
