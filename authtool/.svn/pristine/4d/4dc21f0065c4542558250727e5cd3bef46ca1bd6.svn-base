/*
 * V2DAudioFrameServer.hh
 *
 *  Created on: Jan 06, 2012
 *      Author: Durga
 */

#ifndef V2DAUDIO_FRAMESERVER_HH_
#define V2DAUDIO_FRAMESERVER_HH_

#include <string>

#include "MediaFlags.hh"
#include "MediaServer.hh"

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

class CScapProcessor;
class CAudioResampler;

#define USE_EXTERNAL

class CV2DAudioFrameServer
{
public:
    CV2DAudioFrameServer(std::string& a_strMediaPath);

    ~CV2DAudioFrameServer();

    int Initialize();
    bool GetSamples(uint8_t *a_buffer, int64_t a_start, int64_t a_count);
    uint64_t GetSampleCount();
    bool HasAudio();
    void SetFastResample();
    const std::string & GetErrorString();

    uint16_t GetChannels();
    uint16_t GetBitsPerSample();
    uint16_t GetFrequency();

private:
    void InitializeVariables();
    bool CreateMediaServer();
    bool StartMediaServerAtOffset(uint64_t aOffset);
    bool ReadFramesTillAudioResolutionAcquired(uint64_t aTimeOffsetLimit);
    bool ReadFrame();
    bool RetrieveNextAudioFrame();
    bool InitializeMediaToOffset(uint64_t a_nMediaOffset);
    void ComputeNumberOfAudioFrames();
    uint64_t ComputeMediaOffset(int64_t a_sampleNumber);
    void FillSilence(uint8_t *a_buffer, int64_t a_count);
    void FillNextAudioSamples(uint8_t *a_buffer, int64_t a_count);
    void ResetCounters();
    uint8_t * Resample(uint8_t * a_buffer, int a_len);
    void ComputeResampleRate();


    bool HandleCFrame(mediaindex::CCFrame &a_rCFrame);
    bool HandleVideoCFrame(mediaindex::CCFrame &a_rCFrame,
                           const CV2DStreamMediaFlag &a_rV2dSMediaFlag);
    bool HandleAudioCFrame(mediaindex::CCFrame &a_rCFrame,
                           const CV2DStreamMediaFlag &a_rV2dSMediaFlag);
    bool HandleControlCFrame(mediaindex::CCFrame &a_rCFrame);
    void DecodeAudio(uint8_t * outBuffer, uint8_t * inBuffer, int len);


    int m_nSamplesAvailable;
    uint32_t m_nInputSamples;
    uint64_t m_nStartTimeOffset;
    uint64_t m_nAudioSampleCount;

    std::string m_strMediaPath;

    uint64_t m_nTotalCFrames;

    CScapProcessor * m_scapProcessor;

    bool m_bGotAudioRes;
    bool m_bInitialized;
    bool m_bHasAudio;
    bool m_bError;

    int m_nChannels;
    int m_nBitsPerSample;
    int m_nFrequency;
    int m_nBytesPerSample;

    uint64_t m_nCurrentTimeOffset;
    uint64_t m_nPreviousTimeOffset;
    uint64_t m_nCompleteAudioFrames;
    int64_t m_nCurrentAudioSampleNumber;
    int64_t m_nCurrentAudioSampleCount;

    mediaindex::CMediaServer *m_pMediaServer;
    uint8_t * m_pDecodedBuffer;
    uint8_t * m_pAudioBuffer;
    int m_nDecodeBufferOffset;

    double m_ResampleFactor;
    double m_avgInputFrequency;
;
    int m_nSamplesPerBuffer;
    CAudioResampler * m_pResampler;
    double m_avgDrift;

    uint16_t    m_nErrorCode;
    std::string m_strErrorMsg;
};


#endif /* V2DAUDIO_FRAMESERVER_HH_ */
