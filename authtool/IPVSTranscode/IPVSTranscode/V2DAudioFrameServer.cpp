/*
 * V2DAudioFrameServer.cpp
 *
 *  Created on: Jan 06, 2012
 *      Author: Durga
 */

#include "IPVSTranscode_compatibility.h"


#include <math.h>

#include "V2DAudioFrameServer.hh"

#include "v2d.h"
#include "rtp.h"
#include "AudioDecoder.h"
#include "ScapProcessor.hh"
#include "AudioResampler.hh"
#include <samplerate.h>

using namespace mediaindex;

#define V2D_AUDIO_FREQUENCY                 16316

// For debugging, use DPRINT() instead of printf() and un-comment one of
// these depending on where  you want to send your output to.
//#define USE_WINDEBUG
//#define USE_PRINTF

#ifdef USE_WINDEBUG
char _DBGPRINTBUF_[256];
#define DPRINTF(...) do { sprintf(_DBGPRINTBUF_, __VA_ARGS__); \
    OutputDebugString(_DBGPRINTBUF_); } while (0)
#else
#ifdef USE_PRINTF
#define DPRINTF printf
#else
#define DPRINTF(...) do {} while (0)
#endif
#endif

#define DPRINTFForce printf


CV2DAudioFrameServer::CV2DAudioFrameServer(std::string & a_strMediaPath)
{
    InitializeVariables();

    m_strMediaPath = a_strMediaPath;
    m_scapProcessor = new CScapProcessor();
    m_nSamplesPerBuffer = 256;

    m_pDecodedBuffer = new uint8_t[8 * 1024];
    m_pAudioBuffer = new uint8_t[8 * 1024];

    m_ResampleFactor = (double) m_nFrequency / (double) V2D_AUDIO_FREQUENCY;
    m_pResampler = new CAudioResampler(m_nChannels, m_nBitsPerSample,
            m_nSamplesPerBuffer, m_ResampleFactor);

}

CV2DAudioFrameServer::~CV2DAudioFrameServer()
{
    delete m_pMediaServer;
    delete m_scapProcessor;

    delete [] m_pDecodedBuffer;
    delete [] m_pAudioBuffer;
    delete m_pResampler;
}

void
CV2DAudioFrameServer::InitializeVariables()
{
    m_nStartTimeOffset = 0;
    m_nTotalCFrames = 0;
    m_bGotAudioRes = false;
    m_nCompleteAudioFrames = 0;
    m_strErrorMsg = "";
    m_pMediaServer = NULL;
    m_nErrorCode = FS_SUCCESS;
    m_nAudioSampleCount  = 0;
    m_nCurrentAudioSampleNumber = -1;
    m_nCurrentAudioSampleCount = 0;
    m_nCurrentTimeOffset = 0;
    m_nPreviousTimeOffset = 0;
    m_bInitialized = false;
    m_scapProcessor = NULL;
    m_bHasAudio = false;
    m_bError = false;
    m_nChannels = 2;
    m_nBitsPerSample = 16;
    m_nFrequency = 16000;
    m_nBytesPerSample = m_nChannels * m_nBitsPerSample/8;
    m_nSamplesAvailable = 0;
    m_pDecodedBuffer = NULL;
    m_nDecodeBufferOffset = 0;
    m_avgDrift = 0.0;
    m_nInputSamples = 0;
    m_avgInputFrequency = V2D_AUDIO_FREQUENCY;

}


int
CV2DAudioFrameServer::Initialize()
{
    if (m_bInitialized) {
        m_strErrorMsg = "Already Initialized.";
        return FS_ALREADY_INITIALIZED;
    }

    if (!CreateMediaServer()) {
        return m_nErrorCode;
    }

    if (!ReadFramesTillAudioResolutionAcquired(60000)) {
        return m_nErrorCode;
    }

    ComputeNumberOfAudioFrames();

    m_bInitialized = true;

    return FS_SUCCESS;
}

void CV2DAudioFrameServer::SetFastResample()
{
    delete m_pResampler;
    m_pResampler = new CAudioResampler(m_nChannels, m_nBitsPerSample,
            m_nSamplesPerBuffer, m_ResampleFactor, true);
}

bool CV2DAudioFrameServer::CreateMediaServer()
{
    m_pMediaServer = CMediaServer::Create(m_strMediaPath.c_str());

    if (m_pMediaServer == NULL) {
        m_strErrorMsg = "Could not create MediaServer object.";
        m_nErrorCode = FS_MEDIASERVER_CREATION_FAILED;
        return false;
    }

    if (m_pMediaServer->Init() != MSRC_OK) {
        m_strErrorMsg = "MediaServer Init Failed: ";
        m_strErrorMsg.append(m_pMediaServer->GetErrorString());
        m_nErrorCode = FS_MEDIASERVER_INIT_FAILED;
        return false;
    }

    if (!StartMediaServerAtOffset(0)) {
        return false;
    }

    return true;
}

bool CV2DAudioFrameServer::StartMediaServerAtOffset(uint64_t aOffset)
{
    uint64_t nStartTimestamp = m_pMediaServer->GetTimecodeAtOffset(aOffset);
    if (m_pMediaServer->Start(nStartTimestamp) != MSRC_OK) {
        m_strErrorMsg = "MediaServer Start Failed: ";
        m_strErrorMsg.append(m_pMediaServer->GetErrorString());
        m_nErrorCode = FS_MEDIASERVER_START_FAILED;
        return false;
    }
    return true;
}

bool CV2DAudioFrameServer::ReadFramesTillAudioResolutionAcquired(uint64_t aTimeOffsetLimit)
{
    m_nErrorCode = FS_SUCCESS;
    m_bGotAudioRes = false;
    m_nCurrentTimeOffset = 0;
    m_nPreviousTimeOffset = 0;

    while (!(m_bGotAudioRes && m_bHasAudio)) {
        if (!ReadFrame())
            break;
        // Better get audio within a reasonable time
        if (m_nCurrentTimeOffset > aTimeOffsetLimit) {
            break;
        }
    }
    if (!m_bGotAudioRes) {
        if (m_nErrorCode == FS_SUCCESS) {
            m_strErrorMsg = "Media Stream Error";
            m_nErrorCode = FS_STREAM_ERROR;
        }
        return false;
    }

    return true;
}

void CV2DAudioFrameServer::ComputeNumberOfAudioFrames()
{
    if (!m_bHasAudio) {
        m_nAudioSampleCount = 0;
    }
    else {
        uint64_t nMediaDuration =
                m_pMediaServer->GetOffsetFromTimecode(m_pMediaServer->GetMediaEndTimestamp());
        m_nAudioSampleCount = llround((double)(nMediaDuration * m_nFrequency)/1000.0);
    }
}

bool CV2DAudioFrameServer::InitializeMediaToOffset(uint64_t a_nMediaOffset) {

    // printf("Initializing media to offset %llu\n", a_nMediaOffset);
    bool retVal = false;
    if (m_pMediaServer == NULL) {
        return retVal;
    }

    m_pMediaServer->Stop();

    if (StartMediaServerAtOffset(a_nMediaOffset)) {
        retVal = true;
    }
    return retVal;
}


bool CV2DAudioFrameServer::RetrieveNextAudioFrame()
{
    // Keep reading media frames till an audio frame shows up
    uint64_t nCurrentFrameCount = m_nCompleteAudioFrames;
    while (m_nCompleteAudioFrames == nCurrentFrameCount) {
        if (!ReadFrame())
            break;
    }
    if (m_nCompleteAudioFrames > nCurrentFrameCount) {
        return true;
    }
    return false;
}


bool CV2DAudioFrameServer::GetSamples(uint8_t *a_buffer, int64_t a_start, int64_t a_count)
{
    if (m_nCurrentAudioSampleNumber == -1 ||
            a_start != m_nCurrentAudioSampleNumber + m_nCurrentAudioSampleCount) {

        ResetCounters();
        uint64_t mediaOffset = ComputeMediaOffset(a_start);
        if (!InitializeMediaToOffset(mediaOffset)) {
            m_bError = true;
        }
    }

    m_nCurrentAudioSampleNumber = a_start;
    m_nCurrentAudioSampleCount = a_count;

    if (m_bError) {
        FillSilence(a_buffer, a_count);
    }
    else {
        FillNextAudioSamples(a_buffer, a_count);
    }


    if (m_bError)
        return false;
    return true;
}

uint64_t CV2DAudioFrameServer::ComputeMediaOffset(int64_t a_sampleNumber) {
    return llround(((double)a_sampleNumber / m_nFrequency) * 1000.0);
}

void CV2DAudioFrameServer::FillSilence(uint8_t *a_buffer, int64_t a_count) {
    ::memset(a_buffer, 0x00, a_count * m_nBytesPerSample);
}

void CV2DAudioFrameServer::FillNextAudioSamples(uint8_t *a_buffer, int64_t a_count) {

    int64_t samplesToFill = a_count;
    int64_t fillOffset = 0;
    while (samplesToFill > 0) {
        // Fill upto 200 samples at a time
        int64_t fillSize = samplesToFill > 200 ? 200 : samplesToFill;
        while (m_nSamplesAvailable < fillSize) {
            if (!RetrieveNextAudioFrame()) {
                m_bError = true;
            }
            if (m_bError)
                return;

            int64_t drift = m_nCurrentTimeOffset - ComputeMediaOffset(m_nCurrentAudioSampleNumber);

            const double alpha = 0.1;
            m_avgDrift = alpha * drift + (1.0 - alpha) * m_avgDrift;

        }


        //    printf("Copying from offset %d %d samples\n", m_nDecodeBufferOffset, (int)a_count);
        ::memcpy(a_buffer + fillOffset,
                m_pDecodedBuffer + m_nDecodeBufferOffset,
                fillSize * m_nBytesPerSample);
        m_nSamplesAvailable -= fillSize;
        m_nDecodeBufferOffset += fillSize * m_nBytesPerSample;
        fillOffset += fillSize * m_nBytesPerSample;
        samplesToFill -= fillSize;
    }
}

void CV2DAudioFrameServer::ResetCounters()
{
    m_nSamplesAvailable = 0;
    m_nDecodeBufferOffset = 0;
    m_nPreviousTimeOffset = 0;
    m_nInputSamples = 0;
    m_bError = false;
    m_pResampler->Reset();
}


bool
CV2DAudioFrameServer::ReadFrame()
{
    CCFrame cFrame;

    int nRetVal = m_pMediaServer->GetFrame(cFrame);

    if (nRetVal == MSRC_END_OF_MEDIA)
        return false;

    if (nRetVal != MSRC_OK) {
        m_nErrorCode = FF_MEDIASERVER_GETFRAME_FAILED;
        m_strErrorMsg = "MediaServer Get Frame Failed: ";
        m_strErrorMsg.append(m_pMediaServer->GetErrorString());
        return false;
    }
    m_nTotalCFrames++;

    if (cFrame.GetMessageType() != CCFrame::Normal) {
        switch (cFrame.GetMessageType()) {
        case CCFrame::MediaStarted:
            break;

        case CCFrame::MediaStopped:
            break;

        default:
            break;
        }
        return true;
    }

    m_nCurrentTimeOffset = cFrame.GetTimeOffset();
    if (m_nPreviousTimeOffset == 0)
        m_nPreviousTimeOffset = m_nCurrentTimeOffset;
    HandleCFrame(cFrame);
    return true;
}

bool
CV2DAudioFrameServer::HandleCFrame(CCFrame &a_rCFrame)
{
    bool bRetVal = false;

    if (a_rCFrame.GetStreamType(STREAMTYPE_V2D) == STREAMTYPE_V2D) {
        CV2DStreamMediaFlag v2dSMediaFlag(a_rCFrame.GetMediaFlag());
        const short nChannel = v2dSMediaFlag.GetChannel();

        switch (nChannel) {
        case FRAMETYPE_V2D_CHAN_VIDEO:
            // Ignore video frames
            break;

        case FRAMETYPE_V2D_CHAN_AUDIO:
            bRetVal = HandleAudioCFrame(a_rCFrame, v2dSMediaFlag);
            break;

        case FRAMETYPE_V2D_CHAN_CTL:
            bRetVal = HandleControlCFrame(a_rCFrame);
            break;
        }
    }
    else {
        m_nErrorCode = FS_STREAMTYPE_NOTSUPPORTED;
        m_strErrorMsg  = "Found a non V2D frame.";
        bRetVal = false;
    }

    return bRetVal;
}

bool
CV2DAudioFrameServer::HandleControlCFrame(CCFrame &a_rCFrame)
{
    char *pCtlData = reinterpret_cast<char *> (a_rCFrame.GetData());
    if (!m_scapProcessor->Process(pCtlData))
        return true;
    int width = 0;
    int height = 0;

    if (m_scapProcessor->ResolutionChangeDetected()) {

        width = m_scapProcessor->GetWidth();
        height = m_scapProcessor->GetHeight();
    }

    if (width > 0 && height > 0) {
        m_bGotAudioRes = true;
    }

    return true;
}


#define RTP_HEADER_SIZE 12

bool CV2DAudioFrameServer::HandleAudioCFrame(CCFrame &a_rCFrame,
        const CV2DStreamMediaFlag &a_rV2dSMediaFlag)
{
    m_nCompleteAudioFrames++;
    m_bHasAudio = true;

    const bool bIsAudioSrcRTP = (a_rV2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP);

    int dataLen = 0;
    if (bIsAudioSrcRTP) {
        DecodeAudio(m_pAudioBuffer, (uint8_t *) a_rCFrame.GetData() + RTP_HEADER_SIZE,
                a_rCFrame.GetLength() - RTP_HEADER_SIZE);
        dataLen = a_rCFrame.GetLength() - RTP_HEADER_SIZE; // 1012
    }
    else {
        DecodeAudio(m_pAudioBuffer, (uint8_t *) a_rCFrame.GetData() + (V2D_PACKET_HEADER_SIZE + 8),
                a_rCFrame.GetLength() - (V2D_PACKET_HEADER_SIZE + 8 + 4));
        dataLen = a_rCFrame.GetLength() - (V2D_PACKET_HEADER_SIZE + 8 + 4); // 1012
    }

    if (dataLen != V2D_AUDIO_FRAME_SIZE)
    {
        ::memset(m_pAudioBuffer, 0x00, dataLen);
    }

    if (m_nDecodeBufferOffset > 0) {
        ::memmove(m_pDecodedBuffer, m_pDecodedBuffer + m_nDecodeBufferOffset,
                m_nSamplesAvailable * m_nBytesPerSample);
        m_nDecodeBufferOffset = 0;
    }
    if (m_pResampler->Resample(m_pDecodedBuffer + m_nSamplesAvailable * m_nBytesPerSample,
            m_pAudioBuffer, dataLen)) {
        m_nSamplesAvailable += m_nSamplesPerBuffer;
    }

    m_nInputSamples += dataLen/m_nBytesPerSample;

    if (m_nCompleteAudioFrames % 4000 == 0) {
        uint64_t timeDiff = m_nCurrentTimeOffset - m_nPreviousTimeOffset;
        m_nPreviousTimeOffset = m_nCurrentTimeOffset;
        if (timeDiff > 0) {
            double inputFreq = m_nInputSamples * 1000.0 / timeDiff;
            const double alpha = 0.1;
            m_avgInputFrequency = alpha * inputFreq + (1.0 - alpha) * m_avgInputFrequency;

            DPRINTF("Calculating input freq at %llu = %.1f %.1f\n",
                    timeDiff, inputFreq, m_avgInputFrequency);
        }
        m_nInputSamples = 0;
    }
    if (m_nCompleteAudioFrames % 1000 == 0) {
        DPRINTF("drift = %.1f, offset = %llu\n", m_avgDrift, m_nCurrentTimeOffset);
        ComputeResampleRate();
    }
    return true;
}

void CV2DAudioFrameServer::ComputeResampleRate()
{
    double resampleFactorAdjust = 0.0;
    double lower_margin = 10.0;
    double upper_margin = 25.0;
    if (m_avgDrift > -lower_margin && m_avgDrift < lower_margin) {
        resampleFactorAdjust = 0;
    }
    else {
        if (m_avgDrift > upper_margin) {
            resampleFactorAdjust = 0.001;
        }
        else if (m_avgDrift < -upper_margin) {
            resampleFactorAdjust = -0.001;
        }
        else if (m_avgDrift > lower_margin) {
            resampleFactorAdjust = 0.0002;
        }
        else if (m_avgDrift < -lower_margin) {
            resampleFactorAdjust = -0.0002;
        }
        DPRINTF("*** Adjust = %f ***\n", resampleFactorAdjust);
    }
    m_ResampleFactor = (double) m_nFrequency / m_avgInputFrequency;
    DPRINTF("Setting resampling factor using freq %0.2f\n", m_avgInputFrequency);
    m_pResampler->SetResampleFactor(m_ResampleFactor + resampleFactorAdjust);

}

void CV2DAudioFrameServer::DecodeAudio(uint8_t * outBuffer, uint8_t * inBuffer, int len) {
    // V2D audio decode is just a byte swap
    for (int i = 0; i < len; i += 2) {
        *outBuffer = *(inBuffer + 1);
        *(outBuffer + 1) = *inBuffer;
        inBuffer += 2;
        outBuffer += 2;
    }
}


uint64_t
CV2DAudioFrameServer::GetSampleCount()
{
    return m_nAudioSampleCount;
}

bool
CV2DAudioFrameServer::HasAudio()
{
    return m_bHasAudio;
}

uint16_t CV2DAudioFrameServer::GetChannels() {
    return m_nChannels;
}

uint16_t CV2DAudioFrameServer::GetBitsPerSample() {
    return m_nBitsPerSample;
}

uint16_t CV2DAudioFrameServer::GetFrequency() {
    return m_nFrequency;
}


const std::string &
CV2DAudioFrameServer::GetErrorString()
{
    return m_strErrorMsg;
}
