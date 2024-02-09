/*
 * ScapProcessor.hh
 *
 *  Created on: Nov 2, 2011
 *      Author: rkale
 */

#ifndef SCAPPROCESSOR_HH_
#define SCAPPROCESSOR_HH_

#include <string>

#include "v2d.h"
#include "codecdefs.h"


class DLLEXPORT CScapProcessor
{
public:
    CScapProcessor();
    CScapProcessor(int width, int height, int sliceSize,
            eEncodeFormat encodeFormat = eYUV422P);
    ~CScapProcessor();

    bool Process(char * payLoad);
    bool ResolutionChangeDetected();
    bool AudioResolutionChangeDetected();
    bool GetCachedSCAP(uint8_t * payload);
    uint8_t * GetCachedSCAPBuffer();
    int GetCachedSCAPLength();
    void ResolutionChangeString(std::string & message);
    int GetWidth();
    int GetHeight();
    int GetOriginalWidth();
    int GetOriginalHeight();
    int GetSliceSize();
    int GetMaxSliceNumber();
    int GetStereoMode();
    int GetVideoCompMode();
    int GetVideoMotionCompMode();
    int GetCodecVersion();
    bool IsSVC();
    eEncodeFormat GetEncodeFormat();
    uint64_t GetScapProcessedCount();

    int GetAudioChannels();
    int GetAudioBitsPerSample();
    long GetAudioFrequency();

private:
    uint8_t *m_cacheBuffer;
    int m_cacheBufferLength;
    bool m_bCodecChanged;
    bool m_bVideoRateChanged;
    bool m_bVideoResolutionChanged;
    bool m_bCompModeChanged;
    bool m_bIsStereoVideo;
    bool m_bIs444;
    bool m_bIsMotion444;
    int m_nWidth;
    int m_nHeight;
    int m_nOrigWidth;
    int m_nOrigHeight;
    int m_nSliceSize;
    int m_nMaxSliceNumber;
    int m_nCodecVersion;
    uint64_t m_nScapCount;
    int m_nAudioChannels;
    int m_nAudioBitsPerSample;
    long m_nAudioFreqency;
    bool m_bAudioResolutionChanged; 

    void ProcessSCAP(ScapData_t & scap);
    bool ScapHasVideoInfo(ScapData_t & scap);
    void CheckCodecVersion(ScapData_t & scap);
    void CheckVideoCompMode(ScapData_t & scap);
    void CheckVideoRateChanged(ScapData_t & scap);
    void CheckVideoResolution(ScapData_t & scap);
    void ComputeVideoStereoMode(ScapData_t & scap);
    void ComputeMaxSliceNumber();
    void CheckAudioParams(ScapData_t & scap);
    void CheckAudioFreq(ScapData_t & scap);
    void CacheSCAP(char * payload, int length);
    void SetEncodeFormat(eEncodeFormat encodeFormat);
};

#endif /* SCAPPROCESSOR_HH_ */
