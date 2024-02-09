/*
 * ScapProcessor.cpp
 *
 *  Created on: Nov 2, 2011
 *      Author: rkale
 */

#ifndef SCAPPROCESSOR_CPP_
#define SCAPPROCESSOR_CPP_

#include "v2dlib_compatibility.h"
#include <math.h>

#include "ScapProcessor.hh"

CScapProcessor::CScapProcessor()
{
    m_bCodecChanged = false;
    m_bVideoRateChanged = false;
    m_bVideoResolutionChanged = false;
    m_bCompModeChanged = false;
    m_nCodecVersion = CODEC_VERSION_STANDARD;
    m_bIsStereoVideo = false;
    m_bIs444 = false;
    m_bIsMotion444 = false;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nSliceSize = 0;
    m_nMaxSliceNumber = 0;
    m_nScapCount = 0;
    m_cacheBuffer = NULL;
    m_cacheBufferLength = 0;
    m_nAudioChannels = 2;
    m_nAudioBitsPerSample = 16;
    m_bAudioResolutionChanged = false;
    m_nAudioFreqency = LEG_AUDIO_FREQ;
}

CScapProcessor::CScapProcessor(int width, int height, int sliceSize,
        eEncodeFormat encodeFormat)
{
    m_bCodecChanged = false;
    m_bVideoRateChanged = false;
    m_bVideoResolutionChanged = false;
    m_bCompModeChanged = false;
    m_nCodecVersion = CODEC_VERSION_STANDARD;
    m_bIsStereoVideo = false;
    m_bIs444 = false;
    m_bIsMotion444 = false;
    m_nWidth = width;
    m_nHeight = height;
    m_nOrigWidth = width;
    m_nOrigHeight = height;
    m_nSliceSize = sliceSize;
    m_nMaxSliceNumber = 0;
    m_nScapCount = 0;
    m_cacheBuffer = NULL;
    m_cacheBufferLength = 0;
    m_nAudioChannels = 2;
    m_nAudioBitsPerSample = 16;
    m_bAudioResolutionChanged = false;
    m_nAudioFreqency = LEG_AUDIO_FREQ;

    SetEncodeFormat(encodeFormat);

    ComputeMaxSliceNumber();

}

CScapProcessor::~CScapProcessor()
{
    delete [] m_cacheBuffer;
}

bool CScapProcessor::Process(char *payload)
{
    ScapData_t scap;
    int length;
    if ((length = v2dLibParseScap(&scap, payload)) != -1) {
        ProcessSCAP(scap);
        CacheSCAP(payload, length);
        m_nScapCount++;
        return true;
    }
    return false;
}

void CScapProcessor::ProcessSCAP(ScapData_t & scap)
{
    CheckCodecVersion(scap);

    CheckVideoCompMode(scap);

    CheckVideoRateChanged(scap);

    CheckVideoResolution(scap);

    ComputeVideoStereoMode(scap);

    ComputeMaxSliceNumber();

    CheckAudioParams(scap);
    CheckAudioFreq(scap);
}

void CScapProcessor::CheckCodecVersion(ScapData_t & scap)
{
    m_bCodecChanged = false;
    if (scap.m_videoCodecVersionSet &&
            m_nCodecVersion != scap.m_videoCodecVersion) {
        m_nCodecVersion = FIX_CODEC_VERSION(scap.m_videoCodecVersion);
        m_bCodecChanged = true;
    }
    else if (!scap.m_videoCodecVersionSet) {
        if (m_nCodecVersion == CODEC_VERSION_SVC) {
            m_bCodecChanged = true;
        }
        m_nCodecVersion = CODEC_VERSION_STANDARD;
    }
}

void CScapProcessor::CheckVideoCompMode(ScapData_t & scap)
{
    m_bCompModeChanged = false;
    if (scap.m_videoCompmodeSet) {
        if (scap.m_videoCompmode != m_bIs444) {
            m_bCompModeChanged = true;
        }
        m_bIs444 = scap.m_videoCompmode;
    }

    if (scap.m_videoMotionCompmodeSet) {
        if (scap.m_videoMotionCompmode != m_bIsMotion444) {
            m_bCompModeChanged = true;
        }
        m_bIsMotion444 = scap.m_videoMotionCompmode;
    }
}

void CScapProcessor::CheckVideoRateChanged(ScapData_t & scap)
{
    m_bVideoRateChanged = false;
    if (scap.m_videoRateChangedSet && scap.m_videoRateChanged == 1) {
        m_bVideoRateChanged = true;
    }

}

void CScapProcessor::CheckVideoResolution(ScapData_t & scap)
{
    m_bVideoResolutionChanged = false;
    if (scap.m_hResSet) {
        if (scap.m_hRes > 0) {
            if (m_nWidth != scap.m_hRes) {
                m_bVideoResolutionChanged = true;
                m_nOrigWidth = scap.m_hRes;
            }
            m_nWidth = scap.m_hRes;
        }
    }

    if (scap.m_vResSet) {
        if (scap.m_vRes > 0) {
            if (m_nHeight != scap.m_vRes) {
                m_bVideoResolutionChanged = true;
                m_nOrigHeight = scap.m_vRes;
            }
            m_nHeight = scap.m_vRes;
        }
    }


    if (scap.m_origHresSet) {
        if (scap.m_origHres > 0) {
            if (m_nOrigWidth != scap.m_origHres)
                m_bVideoResolutionChanged = true;
            m_nOrigWidth = scap.m_origHres;
        }
    }

    if (scap.m_origVresSet) {
        if (scap.m_origVres > 0) {
            if (m_nOrigHeight != scap.m_origVres)
                m_bVideoResolutionChanged = true;
            m_nOrigHeight = scap.m_origVres;
        }
    }

    if (scap.m_sliceSizeSet) {
        if (scap.m_sliceSize > 0) {
            if (m_nSliceSize != scap.m_sliceSize)
                m_bVideoResolutionChanged = true;
            m_nSliceSize = scap.m_sliceSize;
        }
    }
}

void CScapProcessor::ComputeMaxSliceNumber()
{
    if (m_nCodecVersion == CODEC_VERSION_STANDARD) {
        m_nMaxSliceNumber =
                (int) ceil((m_nWidth * m_nHeight * 1.0) / (m_nSliceSize * 64.0));
    }
    else {
        m_nMaxSliceNumber = 0;
    }
}

void CScapProcessor::CheckAudioParams(ScapData_t & scap)
{
    if (scap.m_audioType & AUDIOTYPE_MONO)
    	m_nAudioChannels = 1;
    else
    	m_nAudioChannels = 2;

    if (scap.m_audioType & AUDIOTYPE_8BIT)
        m_nAudioBitsPerSample = 8;
    else
        m_nAudioBitsPerSample = 16;
}

void CScapProcessor::CheckAudioFreq(ScapData_t & scap)
{
    m_bAudioResolutionChanged = false;
    if (scap.m_nAudioFreqSet) {
       if (m_nAudioFreqency != scap.m_nAudioFreqency ) {
          m_nAudioFreqency = scap.m_nAudioFreqency;
          m_bAudioResolutionChanged = true;
       }
    }
    else if (!scap.m_nAudioFreqSet ) {
       if ( m_nAudioFreqency == NGS_AUDIO_FREQ) {
           m_bAudioResolutionChanged = true;
       }
       m_nAudioFreqency = LEG_AUDIO_FREQ;
    }
}


void CScapProcessor::ComputeVideoStereoMode(ScapData_t & scap)
{
    if (scap.m_videoStereoSet) {
        m_bIsStereoVideo = scap.m_videoStereo;
    }
}

bool CScapProcessor::ResolutionChangeDetected()
{
    if (m_bCodecChanged)
        return true;
    if (m_bVideoRateChanged)
        return true;
    if (m_bCompModeChanged)
        return true;
    if (m_bVideoResolutionChanged)
        return true;
    return false;
}

bool CScapProcessor::AudioResolutionChangeDetected()
{

    if (m_bAudioResolutionChanged)
        return true;

    return false;

}

void CScapProcessor::ResolutionChangeString(std::string & message)
{
    message = "";
    if (m_bCodecChanged)
        message.append("Codec Changed, ");
    if (m_bVideoRateChanged)
        message.append("Video Rate Change Signaled, ");
    if (m_bCompModeChanged)
        message.append("Comp Mode Changed, ");
    if (m_bVideoResolutionChanged)
        message.append("Video Resolution Changed, ");
    if (message.length() >= 2) {
        message.erase(message.end() - 2); // remove final comma
    }
}

uint64_t CScapProcessor::GetScapProcessedCount()
{
    return m_nScapCount;
}

int CScapProcessor::GetWidth()
{
    return m_nWidth;
}

int CScapProcessor::GetHeight()
{
    return m_nHeight;
}

int CScapProcessor::GetOriginalWidth()
{
    return m_nOrigWidth;
}

int CScapProcessor::GetOriginalHeight()
{
    return m_nOrigHeight;
}

int CScapProcessor::GetSliceSize()
{
    return m_nSliceSize;
}

int CScapProcessor::GetVideoCompMode()
{
    return (m_bIs444 ? 1 : 0);
}

int CScapProcessor::GetVideoMotionCompMode()
{
    return (m_bIsMotion444 ? 1 : 0);
}

int CScapProcessor::GetStereoMode()
{
    return (m_bIsStereoVideo ? 1 : 0);
}

int CScapProcessor::GetMaxSliceNumber()
{
    return m_nMaxSliceNumber;
}

int CScapProcessor::GetCodecVersion()
{
    return m_nCodecVersion;
}

bool CScapProcessor::IsSVC()
{
    return (m_nCodecVersion == CODEC_VERSION_SVC);
}

eEncodeFormat CScapProcessor::GetEncodeFormat() {
    if (m_nCodecVersion == CODEC_VERSION_SVC) {
        return eSVC;
    }
    if (m_bIs444) {
        if (m_bIsMotion444) {
            return eYUV444D;
        }
        return eYUV444S;
    }
    return eYUV422P;
}

int CScapProcessor::GetAudioChannels()
{
	return m_nAudioChannels;
}

int CScapProcessor::GetAudioBitsPerSample()
{
    return m_nAudioBitsPerSample;
}

long CScapProcessor::GetAudioFrequency()
{
    return m_nAudioFreqency;
}

void CScapProcessor::CacheSCAP(char * payload, int length)
{
    if (m_cacheBuffer == NULL ||
        m_cacheBufferLength != length) {
        delete [] m_cacheBuffer;
        m_cacheBuffer = new uint8_t[length];
    }
    memcpy(m_cacheBuffer, payload, length);
    m_cacheBufferLength = length;
}

bool CScapProcessor::GetCachedSCAP(uint8_t * payload)
{
    if (m_cacheBuffer != NULL && m_cacheBufferLength > 0) {
        memcpy(payload, m_cacheBuffer, m_cacheBufferLength);
        return true;
    }
    return false;
}

uint8_t * CScapProcessor::GetCachedSCAPBuffer()
{
    return m_cacheBuffer;
}

int CScapProcessor::GetCachedSCAPLength()
{
    return m_cacheBufferLength;
}

void CScapProcessor::SetEncodeFormat(eEncodeFormat encodeFormat)
{
    switch(encodeFormat)
    {
    case eYUV422P:
        m_bIs444 = false;
        m_bIsMotion444 = false;
        break;

    case eYUV444S:
        m_bIs444 = true;
        m_bIsMotion444 = false;
        break;

    case eYUV444D:
        m_bIs444 = true;
        m_bIsMotion444 = true;
        break;

    case eSVC:
        m_bIs444 = false;
        m_bIsMotion444 = false;
        m_nCodecVersion = CODEC_VERSION_SVC;
        break;

    default:
        m_bIs444 = false;
        m_bIsMotion444 = false;
        break;
    }
}
#endif /* SCAPPROCESSOR_CPP_ */
