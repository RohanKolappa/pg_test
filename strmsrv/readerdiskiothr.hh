#ifndef __H_READERDISKIOTHR__
#define __H_READERDISKIOTHR__


#include "appthread.hh"
#include "reader.hh"

// Forward Declarations
class CReader;

class CReaderDiskIOThread : public CAppThread {

public:
    CReaderDiskIOThread(CReader *a_reader,
                        const char *pMedianame,
                        const char *pMediaDir,
                        double dSpeed,
                        timestamp_t nTimecode,
                        timestamp_t nOffSet);
    ~CReaderDiskIOThread();
    int Run();
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    CReader * m_pReader;
    mediaindex::SourceIndexInterface * m_pSourceIndex;
    bool m_bError;

    bool m_buffering;
    unsigned long m_nBufferOverRuns;
    unsigned long m_nBufferUnderRuns;
    bool m_fileError;
    double m_dSpeed;

    CSQueueSink * m_qDiskIOSink;
    CSBuffer * m_sBuffer;
    char * m_pMediaName;
    char * m_pMediaDir;
    char * m_pFileName;
    char * m_pFileBaseName;
    char * m_pReadBuffer;
    bool m_bFileOpen;
    int m_nFile;

    timestamp_t m_nReadUntil;
    timestamp_t m_nReadTimestamp;
    timestamp_t m_nOffsetTimestamp;
    timestamp_t m_nDiskIOStartTime;
    timestamp_t m_nMediaStartOffset;
    long long m_nFilePosition;

    unsigned long m_nReadBufferSize;
    unsigned long m_nReadOffset;
    unsigned long m_nBufferSize;
    int m_nRetries;
    unsigned long m_readOperations;

    int m_fileCount;
    unsigned long m_nGapCount;
    bool m_bNeedNewChunk;
    bool m_bStartOfStreamSent;
    unsigned long m_nFrameErrors;
    int m_nFileIndex;
    int m_nSearchOffset;
    unsigned long m_nQueueTime;
    timestamp_t m_nObjectCreateTime;
    std::string m_trackDir;

    int DiskIOProcessor();
    int DiskIOInit();

    int GetNextFile();
    bool ReadytoStart();
    void GetNewBuffer(bool a_fresh);
    bool CheckFile();
    int ReadChunk();
    std::string MapToTrackFile(const std::string & minuteFilePath);
    unsigned long SearchTimestamp(timestamp_t a_timestamp);
    unsigned long GetNextFrame(timestamp_t & a_timestamp,
                               unsigned long & a_length,
                               unsigned long & a_flags);
    void SendStartOfStream(timestamp_t nTimestamp);
    CSFrame *CreateErrorNotificationFrame(timestamp_t a_timestamp,
            std::ostringstream& os, bool isSystemError);

};

#endif // __H_READERDISKIOTHR__
