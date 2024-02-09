/*
 * FFTrackWriter.hh
 *
 *  Created on: Jul 30, 2012
 *      Author: rkale
 */

#ifndef FFTRACKWRITER_HH_
#define FFTRACKWRITER_HH_

#include "sframe.hh"
#include "squeue.hh"

#include "MediaServer.hh"
#include "MediaWriter.hh"

#include "FFMediaProcessor.hh"

class CFFTrackWriter {

public:
    CFFTrackWriter(CSQueueSource * qFFQueueSource, std::string & trackDir,
            uint64_t trimLength, bool bSyncWrites, bool bAppendMode);
    ~CFFTrackWriter();

    bool Init();
    bool HandleFrames();
    void SetDropBuffers(bool dropBuffers);
    uint64_t GetBytesWritten();
    uint64_t GetBytesDeleted();
    std::string GetErrorString();

private:

    void WriteFrame(CSFrame *sFrame);
    void SetErrorString(std::string errorString);
    void StartWriter(timestamp_t nTimeStamp);
    void StopWriter(timestamp_t nTimeStamp);
    bool CreateTrackDirectory();
    bool DeleteTrackDirectory();

    CSQueueSource * m_qFFQueueSource;
    mediaindex::CMediaWriter *m_pMediaWriter;
    std::string m_trackDirectory;
    std::string m_errorString;
    int m_nId;
    uint64_t m_nTrimLength;
    bool m_bError;
    bool m_bSyncWrites;
    bool m_bAppendMode;
};

#endif /* FFTRACKWRITER_HH_ */
