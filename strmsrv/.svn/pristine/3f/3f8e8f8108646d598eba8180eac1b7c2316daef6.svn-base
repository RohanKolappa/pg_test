/*
 * FFMediaProcessor.hh
 *
 *  Created on: Jul 23, 2012
 *      Author: rkale
 */

#ifndef FFMEDIAPROCESSOR_HH_
#define FFMEDIAPROCESSOR_HH_

#include <string>
#include <vector>

#include "sframe.hh"
#include "squeue.hh"

#include "MediaServer.hh"
#include "MediaWriter.hh"

#include "FFCodec.hh"
#include "MediaProcessorInterface.hh"

const int DEFAULT_TIME_FOR_PRIMARY_TN = 10000;
const int DEFAULT_TIME_INTERVAL_FOR_STREAM_TN = 15000;

const int DEFAULT_PRIMARY_TN_WIDTH = 480;
const int DEFAULT_PRIMARY_TN_HEIGHT = 320;
const int DEFAULT_STREAM_TN_WIDTH = 160;
const int DEFAULT_STREAM_TN_HEIGHT = 160;

class CFFTrackWriter;

class CFFMediaProcessor : public mediaindex::CMediaProcessorInterface {

public:
    CFFMediaProcessor(std::string mediaDir,
            std::string mediaDestDir,
            std::vector<int> trackSpeeds,
            std::string strStreamType,
            int nId = 0,
            bool bLiveMode = false,
            bool bShowProgress = false,
            std::string strSDPFile = "");
    ~CFFMediaProcessor();

    void SetTrimLength(uint64_t trimLength);
    void SetSyncWrites(bool syncWrites);
    void SetAppendMode(bool appendMode);
    void SetDropBuffers(bool dropBuffers);
    bool InitMedia();
    bool ProcessMedia();
    bool Start(timestamp_t nStartTimestamp);
    bool Stop(timestamp_t nStopTimestamp);
    bool PutFrame(CSFrame * sFrame);
    uint64_t GetBytesWritten();
    uint64_t GetBytesDeleted();
    uint64_t GetBuffersDropped();

    std::string GetErrorString();

    void EnableFFTracks(bool a_bEnableFFTracks);

    void EnableLiveTN(bool a_bEnableTNs,
                      bool a_bEnableSnapshot,
                      bool a_bEnableLargeTN,
                      bool a_bEnableSmallTN);
    bool IsLiveTNEnabled();
    bool IsSnapshotEnabled();
    bool IsLargeTNEnabled();
    bool IsSmallTNEnabled();

    void EnablePrimaryTN(bool a_bEnable);
    bool IsPrimaryTNEnabled();

    bool IsPrimaryTNCreated();

    void SetPrimaryTNResolution(int a_nWidth, int a_nHeight);
    int GetPrimaryTNWidth();
    int GetPrimaryTNHeight();

    void SetPrimaryTNOffset(uint64_t a_nOffset);
    uint64_t GetPrimaryTNOffset();

    void EnableStreamTN(bool a_bEnable);
    bool IsStreamTNEnabled();

    void SetStreamTNResolution(int a_nWidth, int a_nHeight);
    int GetStreamTNWidth();
    int GetStreamTNHeight();

    void SetStreamTNTimeInterval(uint64_t a_nInterval);
    uint64_t GetStreamTNTimeInterval();

    void SetStreamType(std::string a_strStreamType);
    std::string GetStreamType();

    std::string GetMediaDestDir();

    void SetPublishURL(std::string a_strPublishURL);
    std::string GetPublishURL();
    uint64_t GetPublishErrors();
    std::string GetPublishError();

    int GetNativeWidth();
    int GetNativeHeight();

    int GetTotalThumbnailCount();

    int GetTotalFramesInQueue();
    int GetCurrentFramesInQueue();

    int GetTotalScapsInQueue();
    int GetCurrentScapsInQueue();

    uint64_t GetCurrentTNTimestamp();
    uint64_t GetBytesWrittenForTN();

    void SetNativeOutputMode();
    void SetNativeOutputFileName(std::string a_strFileName);

    //CMediaProcessorInterface's methods.
    void StartMedia(uint64_t a_nStartTime);
    void StopMedia(uint64_t a_nStopTime);
    void ProcessMediaFrame(mediaindex::CCFrame & a_rFrame);

private:
    bool CreateFFProcessor();
    bool CreateFFCodec();
    void CreateQueues();
    void DeleteQueues();
    bool GetMediaServerFrame(mediaindex::CCFrame & cframe);
    bool ProcessMediaServerFrame(mediaindex::CCFrame & cframe);
    void ProcessTrackWriters();
    bool CreateMediaServer();
    bool VerifyDestinationDirectory();
    bool CreateTrackWriters();
    void DeleteTrackWriters();
    void SetErrorString(std::string errorString);
    bool DirectoryExists(std::string dirName);

    bool SetThumbnailProperties();
    bool SetStandardMediaProperties();
    void SetupThumbnailParamsIfNeeded();

    std::string m_mediaDir;
    std::string m_mediaDestDir;
    std::string m_errorString;
    int m_nId;
    bool m_bError;
    bool m_bShowProgress;
    std::vector<int> m_trackSpeeds;
    mediaindex::CMediaServer *m_pMediaServer;
    CFFCodec *m_pFFCodec;
    CSQueueSource * m_qFFQueueSource[MAX_TRACKS];
    CFFTrackWriter * m_pTrackWriters[MAX_TRACKS];
    int m_nNumbTracks;
    uint64_t m_nTrimLength;
    bool m_bSyncWrites;
    bool m_bAppendMode;

    bool m_bCreateFFTracks;

    bool m_bEnableLiveTN;
    bool m_bEnableSnapshot;
    bool m_bEnableLargeTN;
    bool m_bEnableSmallTN;
    uint64_t m_nLiveTNInterval;
    bool m_bEnablePrimaryTN;
    int  m_nPrimaryTNWidth;
    int  m_nPrimaryTNHeight;
    uint64_t m_nPrimaryTNOffset;
    bool m_bEnableStreamTN;
    bool m_bThumbnailInitializationNeeded;
    int  m_nStreamTNWidth;
    int  m_nStreamTNHeight;

    std::string m_strStreamType;

    uint64_t m_nThumbnailInterval;

    bool m_bLiveMode;

    std::string m_strSDPFile;

    bool m_bNativeOutput;
    std::string m_strNativeOutputFile;
    std::string m_strPublishURL;

};

#endif /* FFMEDIAPROCESSOR_HH_ */
