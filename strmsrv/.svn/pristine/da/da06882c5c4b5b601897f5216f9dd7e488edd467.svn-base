/*
 * ffv2d.hh
 *
 *  Created on: Jun 24, 2010
 *      Author: rkale
 */

#ifndef FFV2D_HH_
#define FFV2D_HH_

#include <vector>

#include "FFCodec.hh"

#include "FFV2dTrack.hh"
#include "codecdefs.h"


// todo: take these out once done
#include "MediaServer.hh"
#include "MediaWriter.hh"

class CSQueueSource;
class CSQueueSink;
class CScapProcessor;

class CVideoFrameDecoder;

class CScapDataForDec {
public:
    CScapDataForDec(int a_nWidth, int a_nHeight, int a_nSliceSize,
                    eEncodeFormat a_eEncoderFormat, ePixelFormat a_eDecodePixFormat);
    ~CScapDataForDec();

    int GetWidth();
    int GetHeight();
    int GetSliceSize();
    eEncodeFormat GetEncodeFormat();
    ePixelFormat GetDecodeFormat();

private:
    int m_nWidth;
    int m_nHeight;
    int m_nSliceSize;
    eEncodeFormat m_eEncodeFormat;
    ePixelFormat  m_eDecodePixelFormat;
};

class CFFV2d : public CFFCodec {
public:
    CFFV2d(int nId, std::vector<int> & trackSpeeds, bool bShowProgress,
           bool bLiveMode, bool bNativeOutput, bool bCreateTracks = true);
    ~CFFV2d();

    void ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset = 0);
    void StopStream();
    void PrintResults();
    timestamp_t GetCurrentTimeStamp();
    sliceRedData_t * GetSRD();
    CSQueueSink * GetFFQueueSinkAt(int index);
    int GetTrackSpeed(int index);
    uint8_t * GetScapBuffer();
    int GetScapBufferSize();
    uint64_t GetThumbnailPublishErrors();
    std::string GetPublishError();

    virtual int Run();

private:

    void CreateQueues(int nId);
    void DeleteQueues();
    void CreateTrackHandlers();
    void DeleteTrackHandlers();
    void ProcessControlFrame(CSFrame *sFrame);
    void ProcessVideoFrame(CSFrame *sFrame);
    void ProcessAudioFrame(CSFrame *sFrame);
    void ProcessSCAP(CSFrame *sFrame);
    void EnableSRD();
    void DisableSRD();
    void CacheSCAP(CSFrame *sFrame);
    int SendFrameToSRD(CSFrame *sFrame);
    void PrintStats();
    void InitThumbnails();
    void CreateThumbnails();
    void PrepareForFinalThumbnails();
    bool CreateSingleImage(unsigned char* a_pFrameData, int a_nFrameSize,
                           char * a_pStrFileName, int a_nThumbnailWidth,
                           int a_nThumbnailHeight);
    bool CreateImage(int a_nInputWidth, int a_nInputHeight,
                     int a_nOutputWidth, int a_nOutputHeight,
                     char * a_pStrFileName);
    void CreateBlackThumbnails();

    void SetScapDataForTN(CSFrame *a_pCSFrame);

    void PutScapInQueueForTN();
    void PutFrameInQueueForTN(bool bStopped = false);
    bool NeedToQueueFrameForThumbnail();


    void CreatePrimaryTN(CSFrame *a_pCSFrame);
    void CreateStreamTN(CSFrame *a_pCSFrame);

    bool InitializeDecoder();
    void PublishThumbnails();
    void UploadThumbnail(const char * tnFilePath, const char * tnFileName, const char *type);
    bool NeedToPublishThumbnails();

    timestamp_t m_nCurrentTimestamp;
    timestamp_t m_nBeginTimestamp;
    timestamp_t m_nEndTimestamp;

    CSQueueSink * m_qFFQueueSink[MAX_TRACKS];
    CFFV2dTrack * m_pV2DTracks[MAX_TRACKS];
    int m_nNumbTracks;

    std::vector<int> m_trackSpeeds;
    bool m_bDone;
    bool m_bStarted;
    bool m_bCreateTracks;
    sliceRedData_t * m_srd;
    CScapProcessor * m_scapProcessor;
    mediaindex::CMediaWriter * m_pMediaWriter;

    uint64_t m_nControlFrames;
    uint64_t m_nVideoFrames;
    uint64_t m_nAudioFrames;
    uint64_t m_nResolutionChangedCount;
    uint64_t m_nThumbnailPublishErrors;
    bool m_bIsVideoSrcRTP;


    CVideoFrameDecoder *m_pVFrameDecoder;
    ePixelFormat m_eDecodePixelFormat;

    int m_nDecodeSize;
    unsigned char * m_pDecodeBuffer;

    CSQueueSource * m_qTNQueueSource;
    CSQueueSink * m_qTNQueueSink;

    CScapDataForDec *m_pScapDataForTN;

    FILE *m_pTimeStampFile;

    bool m_bQueuedFrameForPrimaryTN;
};
#endif /* FFV2D_HH_ */
