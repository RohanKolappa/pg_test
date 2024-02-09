/*
 * FFRtp.h
 *
 *  Created on: Oct 8, 2012
 *      Author: durga
 */

#ifndef FFRTP_H_
#define FFRTP_H_

#include "FFCodec.hh"
#include "codecdefs.h"

#include <string>

// Forward Declarations
class CSFrame;
class MediaSession;
class MediaSubsession;
class TaskScheduler;
class UsageEnvironment;

class CPTSGenerator;
class CRTPParser;
class CSBuffer;

class CSQueueSource;
class CSQueueSink;

class AVCodecContext;
class AVCodec;
class AVCodecParserContext;
class AVFrame;

class CFrameData {
public:
    CFrameData(uint8_t *a_pData, int a_nBuffLen, bool a_bVideoFrame,
               uint32_t a_nRTPTimestamp, uint64_t a_nTimestamp);
    ~CFrameData();

    uint8_t* GetData();
    uint32_t GetLength();
    bool IsVideoFrame();
    uint64_t GetTimestamp();
    uint32_t GetRTPTimestamp();
    void DeleteData();

private:
    uint32_t m_nBuffLen;
    uint32_t m_nRTPTimestamp;
    uint64_t m_nTimestamp;
    uint8_t *m_pData;
    bool m_bVideoFrame;
};

class CFFRtp: public CFFCodec {
public:
    CFFRtp(int a_nId, std::string a_strSDPFile, bool a_bShowProgress,
            bool a_bLiveMode, bool a_bNativeOutput);
    virtual ~CFFRtp();

    virtual void ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset = 0);
    virtual void StopStream();

    void PrintStats();

    virtual int Run();

private:
    void ParseSDP();
    void ParseSDPURL();
    bool InitializeVideoRTPParser();
    bool InitializeAudioRTPParser();
    bool FindVideoDecoder();
    bool FindAudioDecoder();
    int ParsePacket(uint8_t * packet_buffer, int packet_size);
    bool DecodeFrame(uint8_t *frame_buffer, int frame_size);
    void ConvertToRGB();
    void SetupDestinationImage();
    void CheckFrameDimensions();

    void SaveFrameBuffer(uint8_t *a_pFrameBuffer, int a_nBufferLen,
                         bool a_bFlags = true);

    void HandleVideoPacket(char * a_pData, int nLen);
    void HandleAudioPacket(char * a_pData, int nLen);

    void CacheFrameData(CSFrame *a_pCSFrame);
    void DeleteFrameData();
    void CreateThumbnailsFromBuffer();
    void CreateThumbnails();
    void PrepareForFinalThumbnails();
    void CreatePrimaryTN(CFrameData *a_pCSFrame);
    void CreateStreamTN(CFrameData *a_pCSFrame);
    bool CreateSingleImage(unsigned char * a_pInputBuffer, char * a_pStrFileName,
                           int a_nThumbnailWidth, int a_nThumbnailHeight);
    bool CreateSingleThumbnail(const char *pFileName, int nTNWidth, int nTNHeight);
    void CreateBlackThumbnails();

    void CreateQueues(int nId);
    void DeleteQueues();

    void CreateOutput();

    void CreateFinalThumbnails();

    void CreateStandardMedia();
    void CompleteStandardMedia();

    void Initialize();
    void SetStandardMediaParams();

    void CreatePTSGeneratorObjs();
    void DeletePTSGeneratorObjs();

    void ParseSPSAndPPSValues(uint8_t *a_pBuffer, int a_nBuffSize);
    int FindNALUnit(uint8_t *a_pBuff, int a_nSize,
            int *a_nNAL_Start, int *a_nNAL_End);

    uint64_t GetPTSValue(bool bVideoFrame, uint64_t a_nWallClockTime);
    uint64_t GetVideoPTSValue(uint64_t a_nWallClockTime);
    uint64_t GetAudioPTSValue(uint64_t a_nWallClockTime);

    void HandleSSRCChange(bool bVideoFrame);
    void HandleVideoSSRCChange();
    void HandleAudioSSRCChange();

    void CheckForGaps();

    unsigned long m_nInputVideoSSRC;
    unsigned long m_nInputAudioSSRC;

    std::string m_strSDPURL;
    std::string m_strSDPFilePath;

    int m_nFrameWidth;
    int m_nFrameHeight;

    int m_nDecodeSize;

    ePixelFormat m_eDecodePixelFormat;

    CRTPParser *m_pVideoRTPParser;
    CRTPParser *m_pAudioRTPParser;

    bool m_bNeedsFrameParsing;
    bool m_bInitialized;

    MediaSubsession *m_pVideoSubsession;
    MediaSubsession *m_pAudioSubsession;
    TaskScheduler *m_pTaskScheduler;
    UsageEnvironment *m_pUsageEnv;
    MediaSession *m_pSession;

    AVCodecContext *m_pVideoCodecCtx;
    AVCodecContext *m_pAudioCodecCtx;
    AVCodec *m_pVideoCodec;
    AVCodec *m_pAudioCodec;
    AVCodecParserContext *m_pVideoParserContext;
    AVCodecParserContext *m_pAudioParserContext;
    AVFrame *m_pPicture;
    AVFrame *m_pDestPicture;
    struct SwsContext * m_pSwsCtx ;
    unsigned char *m_pDecodeBuffer;

    timestamp_t m_nCurrentTimestamp;
    timestamp_t m_nBeginTimestamp;

    timestamp_t m_nGapStartTime;

    CPTSGenerator *m_pPTSGeneratorForVideo;
    CPTSGenerator *m_pPTSGeneratorForAudio;

    int m_nIFrameCount;
    int m_nInFrameCount;

    int m_nInAudioFrameCount;

    bool m_bGotResolution;

    CSQueueSource * m_qTNQueueSource;
    CSQueueSink * m_qTNQueueSink;

    uint8_t *m_pSPSData;
    uint8_t *m_pPPSData;

    int m_nSPSData_Size;
    int m_nPPSData_Size;

    bool m_bFoundSPSAndPPS;
    //For standard output
    bool m_bVideoFound;
    bool m_bAudioFound;
    bool m_bWrittenTrailer;

    //Storing the Latest CSFrame data into member
    CFrameData *m_pCachedFrameData;
};

#endif /* FFRTP_H_ */
