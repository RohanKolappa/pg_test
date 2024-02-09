#ifndef __H_WRITER__
#define __H_WRITER__


#include "streamer.hh"
#include "diskiothr.hh"
#include "sbuffer.hh"
#include "averager.hh"

// Forward declarations
class CWriterDiskIOThread;
class CWriterDiskIOFFThread;
class CWriterDiskIOProgress;

/// The Writer Class
class CWriter : public CStreamer {

public:

    CWriter(int nId,
            char *pMediaName,
            char *pMediaDir,
            int nTrimLength,
            int createFFTracks,
            int createHLSMedia,
            char *pStreamType,
            int createPrimaryThumbnails,
            int nStreamThumbnailsFrequency,
            char *pSDPFilePath);

    int ProcessStream();
    int StartStream(int nQueueId);
    int StopStream();
    void DeleteStream();

    void PrintStatistics(std::string& a_stats);
    void PrintError(CNotify *a_notify);
    void PrintNotification(CNotify *a_notify);
    void UpdateCustomStatus(CCommand *a_pCmd);

    char * GetMediaDir() {
        return m_pMediaDir;
    }
    char * GetMediaName() {
        return m_pMediaName;
    }
    int GetTrimLength() {
        return m_nTrimLength;
    }
    uint32_t GetUniqueId() {
        return m_nUniqueId;
    }
    void DecrementPendingWrite(int a_nLength);
    static void ResetGlobalStats();

private:

    CSQueueSink *           m_qDiskIOSink;
    CSQueueSink *           m_qDiskIOFFSink;
    CSQueueSource *         m_qDiskIOSource;
    CSBuffer *              m_sBuffer;
    CWriterDiskIOThread *   m_pWriterDiskIOThread;
    CWriterDiskIOFFThread *   m_pWriterDiskIOFFThread;
    char *                  m_pMediaName;
    char *                  m_pMediaDir;
    char *                  m_pWriteBuffer;
    bool                    m_bDroppingBuffers;
    bool                    m_bEnableFFTracks;
    bool                    m_bCreateFFTracks;
    bool                    m_bEnablePrimaryThumbnail;
    bool                    m_bCreateHLSOutput;
    int                     m_nStreamThumbnailsFrequency;
    char *                  m_pStreamType;
    char *                  m_pSDPFilePath;
    int                     m_nPendingWrites;
    long                    m_nPendingBytes;
    CMutex                  m_DiskIOLock;
    uint32_t                m_nUniqueId;

    timestamp_t             m_nFileStartTime;
    timestamp_t             m_nTrimLength;
    timestamp_t             m_nLastWriteTime;
    timestamp_t             m_nIndexTimestamp;
    timestamp_t             m_nWriteWallClock;
    long long               m_nDiscontinuityOffset;
    int                     m_nDiscontinuityCount;
    unsigned long           m_nIndexOffset;
    unsigned long           m_nLastIndexOffset;
    int                     m_nWriteOffset;
    int                     m_nBufferSize;
    unsigned long           m_ScheduledWriteOperations;
    uint64_t                m_bytesWritten;
    CAverager <unsigned long> * m_pQueueSizeAverager;
    CAverager <unsigned long> * m_pDiskIOQueueSizeAverager;
    CAverager <unsigned long> * m_pChunkSizeAverager;

    int                     m_nDiskIOQueueLimit;
    unsigned long           m_nFramesPerBuffer;
    unsigned long           m_nFramesPerBufferCount;
    unsigned long           m_nBufferDrops;
    unsigned long           m_nConsecutiveBufferDrops;
    CWriterDiskIOProgress * m_savedProgressInfo;
    CWriterDiskIOProgress * m_previousProgressInfo;
    static unsigned long    m_nGlobalBufferDrops;

    int CheckFile();
    int FlushBuffer();
    void StartMedia(timestamp_t nFirstTimeStamp);
    int GetNextFile(timestamp_t a_nTimestamp);
    int ProcessAltStream();
    void SetupFFThread();

    // private destructor
    ~CWriter();

};

#endif /* __H_WRITER__ */
