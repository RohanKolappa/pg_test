#ifndef __H_WRITERDISKIOTHR__
#define __H_WRITERDISKIOTHR__


#include "appthread.hh"
#include "writer.hh"
#include "averager.hh"

// Forward Declarations
class CWriter;

namespace mediaindex {
    class DestIndexInterface;
}

class CWriterDiskIOThread : public CAppThread {

public:
    CWriterDiskIOThread(CWriter *a_writer);
    ~CWriterDiskIOThread();
    int Run();

    unsigned long GetRemainingLength() {
        return m_nFileRemainingLength;
    }
    unsigned long GetWriteOperations() {
        return m_nWriteOperations;
    }
    double GetAverageQueueSize();

    char * GetFileBaseName() {
        return m_pFileBaseName;
    }
    int GetFileIndex() {
        return m_nFileIndex;
    }
    unsigned long GetWriteLatency() {
        return m_nWriteLatency;
    }
    double GetExpAvgWriteLatency() {
        return m_pLatencyAverager->GetExponentialAverage();
    }
    double GetMovingAvgWriteLatency() {
        return m_pLatencyAverager->GetMovingAverage();
    }

    unsigned long GetWriteLatencyPercentile(int a_percent) {
        return m_pLatencyAverager->GetPercentile(a_percent);
    }

    unsigned long GetMaxWriteLatency() {
        return m_pLatencyAverager->GetMaxValue();
    }
    unsigned long GetMinWriteLatency() {
        return m_pLatencyAverager->GetMinValue();
    }
    void ResetLatencyStats() {
        m_pLatencyAverager->ResetStats();
    }

private:
    CWriter * m_pWriter;
    mediaindex::DestIndexInterface * m_pDestinationIndex;
    int m_nFileFD;
    int m_nIndexFileFD;
    bool m_bError;
    int m_nSync;

    CSQueueSource * m_qDiskIOSource;
    CSQueueSink * m_qDiskIOSink;
    char * m_pFileName;
    char * m_pIndexFileName;
    char * m_pFileBaseName;
    int m_nFileIndex;

    unsigned long m_nFileRemainingLength;

    uint64_t m_nWriteOperations;
    uint64_t m_nBytesWritten;
    unsigned long m_nWriteLatency;
    CAverager<unsigned long> * m_pLatencyAverager;

    void CloseFiles();
    int GetNextFile(timestamp_t a_nTimestamp);
    int DiskIOProcessor();
    int DiskIOInit();
    void SendProgressMessage();

};

#endif // __H_WRITERDISKIOTHR__
