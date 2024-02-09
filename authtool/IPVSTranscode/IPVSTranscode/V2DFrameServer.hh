/*
 * V2DFrameServer.hh
 *
 *  Created on: Sep 19, 2011
 *      Author: rkale
 */

#ifndef V2DFRAMESERVER_HH_
#define V2DFRAMESERVER_HH_

#include <string>

#include "MediaFlags.hh"
#include "MediaServer.hh"
#include "srd.h"


#define FS_SUCCESS                              0
//#define TRC_VIDEOMODE_NOTSUPPORTED              9000
//#define TRC_VIDEORESOLUTION_CHANGED             9001
//#define TRC_FAILED_WRITING_FRAMES_IN_YUV_FILE   9003
//#define TRC_FAILED_WRITING_YUV_HEADER           9004
//#define TRC_WRONG_TARGET_FPS                    9005
#define FS_STREAM_ERROR                        9001
#define FS_VIDEOFRAMEDECODER_CREATION_FAILED   9002
#define FS_MEDIASERVER_CREATION_FAILED         9006
#define FS_MEDIASERVER_INIT_FAILED             9007
#define FS_MEDIASERVER_START_FAILED            9008
#define FF_MEDIASERVER_GETFRAME_FAILED         9009
#define FS_INIT_SRD_FAILED                     9010
#define FS_STREAMTYPE_NOTSUPPORTED             9011
#define FS_ALREADY_INITIALIZED                 9012
//#define TRC_WRITING_WAVE_HEADER_FAILED          9013
//#define TRC_WRITING_WAVE_DATA_FAILED            9014
//#define TRC_AUDIODECODER_DECODESAMPLE_FAILED    9015
//#define TRC_AUDIOPARSER_FINDINGSOFEOF_FAILED    9016
//#define TRC_AUDIOFRAMEDECODER_CREATION_FAILED   9017
//#define TRC_AUDIOFRAMEPARSER_CREATION_FAILED    9018
//#define TRC_WAVEFILE_CREATION_FAILED            9019
//#define TRC_YUV4MPEG2WRITER_CREATION_FAILED     9020

class CVideoFrameDecoder;
class CAudioDecoder;
class CAudioFrameParser;
class CScapProcessor;

struct SwsContext;
struct AVFrame;

class CV2DFrameServer
{
public:
    CV2DFrameServer(std::string& a_strMediaPath, int a_nTargetFPS);

    ~CV2DFrameServer();

    int Initialize();
    int Initialize(int a_nWidth, int a_nHeight);
    uint8_t * GetVideoFrame(int a_nFrameNumber);
    void EnableDebug();
    int GetWidth();
    int GetHeight();
    int GetFrameWidth();
    int GetFrameHeight();
    int GetVideoFrameSize();
    uint64_t GetVideoFrameCount();
    uint64_t GetVideoFramesDecoded();
    uint64_t GetCurrentVideoServedOffset();

    void SkipDecode(bool m_bValue);
    const std::string & GetErrorString();

private:
    void InitializeVariables();
    bool CreateMediaServer();
    bool StartMediaServerAtOffset(uint64_t aOffset);
    bool ReadFramesTillVideoResolutionAcquired(uint64_t aTimeOffsetLimit, uint64_t aFrameCountLimit);
    bool ReadFrame();
    bool RetrieveNextVideoFrame();
    uint64_t GetOffsetAtFrameNumber(int a_nFrameNumber);
    bool InitializeMediaToOffset(uint64_t a_nMediaOffset);
    void ComputeNumberOfVideoFrames();
    void SetupFrameDimensions();
    void DestroySRD();
    void InitializeSRD();
    void CollectFrameFromSRD();
    void CheckScaling();
    void SetupScaler();
    void DestroyScaler();
    void DecodeFrame(uint64_t a_nOffset);
    uint8_t * GetVideoFrameBuffer();


    bool HandleCFrame(mediaindex::CCFrame &a_rCFrame);
    bool HandleVideoCFrame(mediaindex::CCFrame &a_rCFrame,
                           const CV2DStreamMediaFlag &a_rV2dSMediaFlag);
    bool HandleAudioCFrame(mediaindex::CCFrame &a_rCFrame,
                           const CV2DStreamMediaFlag &a_rV2dSMediaFlag);
    bool HandleControlCFrame(mediaindex::CCFrame &a_rCFrame);


    bool CreateFrameDecoderIfNeeded();
    void CreateDecodeBuffer();
    void DestroyDecodeBuffer();
    void DestroyDecoder();

    uint64_t m_nStartTimeOffset;
    uint64_t m_nDurationTime;
    uint64_t m_nVideoFrameCount;

    std::string m_strMediaPath;

    uint64_t m_nTotalCFrames;
    uint64_t m_nVideoCFrames;
    uint64_t m_nAudioCFrames;
    uint64_t m_nControlCFrames;

    sliceRedData_t* m_pSliceRdata;
    CScapProcessor *m_scapProcessor;

    int m_nWidth;
    int m_nHeight;
    int m_nFrameWidth;
    int m_nFrameHeight;
    int m_nScaledWidth;
    int m_nScaledHeight;
    int m_nSliceSize;
    bool m_bGotVideoRes;
    bool m_bSkipDecode;
    bool m_bSRDOnly;
    bool m_bNeedScaling;
    bool m_bServedScaled;
    bool m_bInitialized;
    bool m_bDebugVideo;

    uint64_t m_nCurrentTimeOffset;
    uint64_t m_nPreviousTimeOffset;
    uint64_t m_nTargetTimeOffset;
    uint64_t m_nCurrentVideoServedOffset;
    uint64_t m_nCompleteVideoFrames;
    uint64_t m_nVideoFramesDecoded;
    int m_nCurrentVideoFrameNumber;

    mediaindex::CMediaServer *m_pMediaServer;
    CVideoFrameDecoder *m_pFrameDecoder;
    uint8_t * m_pDecodedBuf;
    int m_nDecodeBufferLen;
    uint8_t * m_pScaledBuf;
    int m_nScaledBufferLen;
    uint8_t * m_pOutFrame;
    int m_nOutFrameBufferLen;
    int m_nOutFrameLen;
    int m_nBytesPerPixel;

    SwsContext * m_pSwsCtx;
    AVFrame * m_pDestPicture;

    double      m_fOutputFramePeriod;
    uint16_t    m_nTargetFps;

    uint16_t    m_nAudioChannels;
    uint16_t    m_nBitsPerSample;
    uint16_t    m_nAudioFrequency;


    uint16_t    m_nErrorCode;
    std::string     m_strErrorMsg;
};


#endif /* V2DFRAMESERVER_HH_ */
