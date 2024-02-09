#include "ScapProcessorWrapper.h"
#include "v2dlib_compatibility.h"
#include "ScapProcessor.hh"


struct v2dScapWrapper_t {
    CScapProcessor *pScapProcessor;
};


v2dScapWrapper_t * createScapProcessorObj(bool a_bIsRTPMode, int a_nWidth, int a_nHeight,
                                          int a_nSliceSize, int a_nEncodeType)
{
    v2dScapWrapper_t *pV2dScapWrapper = new v2dScapWrapper_t;
    if (pV2dScapWrapper == NULL)
        return NULL;

    if (a_bIsRTPMode == true)
    {
        eEncodeFormat eFormat;
        switch(a_nEncodeType)
        {
        case 1:
            eFormat = eYUV422P;
            break;
        case 2:
            eFormat = eYUV444S;
            break;
        case 3:
            eFormat = eYUV444D;
            break;
        case 4:
            eFormat = eSVC;
            break;
        default:
            eFormat = eYUV422P;
            break;
        }

        pV2dScapWrapper->pScapProcessor = new CScapProcessor(a_nWidth, a_nHeight,
                                              a_nSliceSize, eFormat);
    }
    else
    {
        pV2dScapWrapper->pScapProcessor = new CScapProcessor();
    }

    return pV2dScapWrapper;
}

void destroyScapprocessorObj(v2dScapWrapper_t *a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper != NULL)
    {
        if (a_pV2dScapWrapper->pScapProcessor != NULL)
            delete a_pV2dScapWrapper->pScapProcessor;
        a_pV2dScapWrapper->pScapProcessor = NULL;

        delete a_pV2dScapWrapper;
        a_pV2dScapWrapper = NULL;
    }
}

void ProcessorScapData(v2dScapWrapper_t* a_pV2dScapWrapper, char *a_pPayLoad)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return;

    a_pV2dScapWrapper->pScapProcessor->Process(a_pPayLoad);
}

bool ResolutionChangeDetected(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return false;

    return (a_pV2dScapWrapper->pScapProcessor->ResolutionChangeDetected());
}

bool AudioResolutionChangeDetected(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return false;

    return (a_pV2dScapWrapper->pScapProcessor->AudioResolutionChangeDetected());

}


bool GetCachedSCAP(v2dScapWrapper_t* a_pV2dScapWrapper, uint8_t * a_pPayLoad)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return false;

    return (a_pV2dScapWrapper->pScapProcessor->GetCachedSCAP(a_pPayLoad));
}

void ResolutionChangeString(v2dScapWrapper_t* a_pV2dScapWrapper, std::string & message)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return;

    a_pV2dScapWrapper->pScapProcessor->ResolutionChangeString(message);
}

int GetWidth(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetWidth());
}

int GetHeight(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetHeight());
}

int GetOrigWidth(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetOriginalWidth());
}

int GetOrigHeight(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetOriginalHeight());
}

int GetSliceSize(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetSliceSize());
}

int GetMaxSliceNumber(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetMaxSliceNumber());
}

int GetStereoMode(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetStereoMode());
}

int GetVideoCompMode(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetVideoCompMode());
}

int GetVideoMotionCompMode(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetVideoMotionCompMode());
}

int GetCodecVersion(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetCodecVersion());
}

bool IsSVC(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return false;

    return (a_pV2dScapWrapper->pScapProcessor->IsSVC());
}

uint64_t GetScapProcessedCount(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetScapProcessedCount());
}

int GetAudioChannels(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetAudioChannels());
}

int GetAudioBitsPerSample(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor->GetAudioBitsPerSample());
}

long GetAudioFrequency(v2dScapWrapper_t* a_pV2dScapWrapper)
{
    if (a_pV2dScapWrapper == NULL
            || a_pV2dScapWrapper->pScapProcessor == NULL)
        return 0;

    return (a_pV2dScapWrapper->pScapProcessor-> GetAudioFrequency());
}
