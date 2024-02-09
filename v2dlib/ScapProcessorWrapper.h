#pragma once
#include "v2dlib_compatibility.h"
#include <string>

struct v2dScapWrapper_t;

#ifdef __cplusplus
extern "C" {
#endif

    v2dScapWrapper_t* createScapProcessorObj(bool isRTPMode = false, int width = 0,
                                             int height = 0, int sliceSize = 0,
                                             int encodeType = 1);
    void destroyScapprocessorObj(v2dScapWrapper_t *a_pV2dScapWrapper);

    void ProcessorScapData(v2dScapWrapper_t* a_pV2dScapWrapper, char *a_pPayLoad);

    bool ResolutionChangeDetected(v2dScapWrapper_t* a_pV2dScapWrapper);
    bool AudioResolutionChangeDetected(v2dScapWrapper_t* a_pV2dScapWrapper);
    bool GetCachedSCAP(v2dScapWrapper_t* a_pV2dScapWrapper, uint8_t * payload);
    void ResolutionChangeString(v2dScapWrapper_t* a_pV2dScapWrapper, std::string & message);
    int GetWidth(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetHeight(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetOrigWidth(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetOrigHeight(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetSliceSize(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetMaxSliceNumber(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetStereoMode(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetVideoCompMode(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetVideoMotionCompMode(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetCodecVersion(v2dScapWrapper_t* a_pV2dScapWrapper);
    bool IsSVC(v2dScapWrapper_t* a_pV2dScapWrapper);
    //eEncodeFormat GetEncodeFormat(v2dScapWrapper_t* a_pV2dScapWrapper);
    uint64_t GetScapProcessedCount(v2dScapWrapper_t* a_pV2dScapWrapper);

    int GetAudioChannels(v2dScapWrapper_t* a_pV2dScapWrapper);
    int GetAudioBitsPerSample(v2dScapWrapper_t* a_pV2dScapWrapper);
    long GetAudioFrequency(v2dScapWrapper_t* a_pV2dScapWrapper);

#ifdef __cplusplus
}
#endif
