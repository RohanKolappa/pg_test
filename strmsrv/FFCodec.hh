/*
 * CFFCodec.h
 *
 *  Created on: Oct 8, 2012
 *      Author: durga
 */

#ifndef CFFCODEC_H_
#define CFFCODEC_H_

#include "appthread.hh"
#include "ThumbnailHandler.h"
#include "mutex.hh"

#include <string>
#include <vector>


typedef uint64_t timestamp_t;

#define MAX_TRACKS 8

#define DEFAULT_FRAME_WIDTH 1920
#define DEFAULT_FRAME_HEIGHT 1080

class CThumbnailGenerator;
class CStandardFormatHandler;

struct CSFrame;

class CFFCodec : public CAppThread {
public:
    virtual ~CFFCodec();

    static CFFCodec* CreateObj(std::string a_strStreamType, int nId, std::vector<int> & trackSpeeds,
                               bool bShowProgress, bool bCreateTracks, bool bLiveMode = false,
                               bool bNativeOutput = false, std::string a_strSDPFile = "");
    void DestroyObj();

    virtual void StopStream() = 0;

    virtual void ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset = 0) = 0;

    virtual void SetDuration(timestamp_t nDuration);

    virtual void SetMediaDestDir(std::string a_strMediaDir);
    std::string GetMediaDestDir();

    virtual void SetMediaSrcDir(std::string a_strMediaSrcDir);
    std::string GetMediaSrcDir();

    virtual void SetPublishURL(std::string a_strPublishURL);
    std::string GetPublishURL();
    virtual uint64_t GetThumbnailPublishErrors();
    virtual std::string GetPublishError();

    virtual void PrintStats() = 0;
    virtual int Run();

    std::string GetErrorString();

    int GetNativeWidth();
    int GetNativeHeight();

    int GetTotalFramesInQueue();
    int GetCurrentFramesInQueue();

    int GetTotalScapsInQueue();
    int GetCurrentScapsInQueue();

    uint64_t GetCurrentThumbnailTS();
    uint64_t GetBytesWritten();

    CThumbnailHandler* GetThumbnailHandler();

    CStandardFormatHandler* GetStandardMediaHandler();

    bool ProceedToProcess();
    bool IsThumbnailCreationEnabled();
    bool IsNativeOutpuEnabled();

protected:
    CFFCodec(std::string a_strStreamType, bool a_bShowProgress,
            bool a_bLiveMode = false, bool a_bNativeOutput = false);

    void PrintProgress();

    void SetErrorString(std::string a_strErrorString);

    timestamp_t m_nDuration;

    std::string m_strMediaDestDir;
    std::string m_strMediaSrcDir;
    std::string m_strPublishURL;
    std::string m_strPublishError;

    bool m_bShowProgress;

    timestamp_t m_nElapsed;
    double m_PercentComplete;
    double m_LastPercentComplete;
    uint64_t m_nProcessedBytes;
    uint64_t m_nLastProcessedBytes;

    bool m_bError;

    bool m_bLiveMode;
    bool m_bStopStreamIssued;

    bool m_bNativeOutput;

    //For stats
    int m_nNativeWidth;
    int m_nNativeHeight;

    int m_nTotalFramesInQueue;
    int m_nCurrentFramesInQueue;
    int m_nTotalScapsInQueue;
    int m_nCurrentScapsInQueue;

    uint64_t m_nBytesWritten;
    uint64_t m_nCurrentTNTS;

    CMutex m_Mutex;

private:
    CCodecHandler *m_pCodecHandler;
    std::string m_strErrorString;

    std::string m_strStreamType;

    void CreateCodecHandler();
    void DestroyCodecHandler();
};

#endif /* CFFCODEC_H_ */
