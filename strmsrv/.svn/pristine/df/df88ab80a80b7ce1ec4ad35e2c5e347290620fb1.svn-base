/*
 * writerdiskiofftrhead.hh
 *
 *  Created on: Aug 17, 2012
 *      Author: rkale
 */

#ifndef WRITERDISKIOFFTHREAD_HH_
#define WRITERDISKIOFFTHREAD_HH_

#include "appthread.hh"
#include "writer.hh"

class CWriter;
class CFFMediaProcessor;
class CAdaptiveMediaGenerator;

class CWriterDiskIOFFThread : public CAppThread {

public:
    CWriterDiskIOFFThread(CWriter *a_writer, bool a_bCreateFFTracks,
                          std::string a_strStreamType,
                          bool a_bCreatePrimaryThumbnail,
    		              int a_nStreamThumbnailFrequency,
    		              std::string a_strSDPFilePath,
    		              bool a_bCreateHLSOutput);
    ~CWriterDiskIOFFThread();

    int Run();

    void SetDropBuffers(bool dropBuffers);
    uint64_t GetFramesProcessed();
    uint64_t GetBytesWritten();
    uint64_t GetBytesDeleted();
    uint64_t GetHLSBytesWritten();
    uint64_t GetHLSBytesDeleted();
    uint64_t GetBuffersDropped();
    uint64_t GetFramesDropped();
    uint64_t GetHLSVideoDropCount();
    uint64_t GetHLSAudioDropCount();

private:
    CWriter * m_pWriter;
    CSQueueSource * m_qDiskIOSource;
    CFFMediaProcessor * m_pFFMediaProcessor;
    CAdaptiveMediaGenerator * m_pAdaptiveMediaGenerator;
    bool m_bError;
    int m_nSync;
    uint64_t m_nFramesProcessed;
    bool m_bCreateFFTracks;
    std::string m_strStreamType;
    bool m_bCreatePrimaryThumbnail;
    int m_nStreamThumbnailsFrequency;
    std::string m_strSDPFilePath;

    bool m_bCreateHLSOutput;

    int DiskIOProcessor();
    int DiskIOInit();
    void SetupHLSOutput();
    void SendProgressMessage();
    void SendErrorNotification(std::string errorString);
};

#endif /* WRITERDISKIOFFTHREAD_HH_ */
