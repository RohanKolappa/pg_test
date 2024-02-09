/*
 * AudioRenderer.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

#include <sstream>

//#define USE_ATOMIC

#define HAVE_RESAMPLE

#include "AudioRenderer.hh"

#include "portaudio.h"
#include "pa_ringbuffer.h"
#ifdef HAVE_RESAMPLE
#include <samplerate.h>
#endif

using namespace std;

int CAudioRenderer::m_nQueueId = 0;

#define USE_RINGBUFFER  1

CMutex CAudioRenderer::m_globalAudioMutex;
bool CAudioRenderer::m_bInitialized = false;

CAudioRenderer::CAudioRenderer(int a_nFrequency, int a_nChannels,
        int a_nSamplesPerBuffer, eAudioFormat a_eFormat,
        bool a_bResample, int a_nPlaybackFrequency)
{
    m_bError = false;
    m_bStarted = false;
    m_pStream = NULL;
    m_nDroppedSamples = 0;
    m_nIgnoredSamples = 0;
    m_nRequestSamples = 0;
    m_nRequestBytes = 0;
    m_nLastRequestBytes = 0;
    m_nRenderedBytes = 0;
    m_nLastRenderedBytes = 0;
    m_nSilenceCount = 0;
    m_ringBuffer = NULL;
    m_sampleBuffer = NULL;
    m_ringSampleBuffer = NULL;
    m_avgBufferLength = 0.0;
    m_avgBufferTime = 0.0;
    m_ResampleFactor = 1.0;
    m_srcState = NULL;
    m_nLastSampleDropTime = CTimeKeeper::Now();
    m_nBeginTime = CTimeKeeper::Now();
    m_bDroppingPackets = false;

    SetupObjectId();

    m_nFrequency = a_nFrequency;
    m_nChannels = a_nChannels;
    m_eFormat = a_eFormat;
    m_nInputSamplesPerBuffer = a_nSamplesPerBuffer;
    m_nSamplesPerBuffer = m_nInputSamplesPerBuffer;
    m_nBytesPerSample = ComputeBytesPerSample() * m_nChannels;
    m_pInputFrequencyAverager = new CAverager<double> (12);
    m_pOutputFrequencyAverager = new CAverager<double> (12);
    //atomic_set(&m_atomicVal, 127);

    if (m_eFormat == AFUnsupported) {
        m_bError = true;
    }

    m_bResample = a_bResample;
    m_bResample = true;
#ifndef HAVE_RESAMPLE
    m_bResample = false;
#endif
    SetupResampling(a_nPlaybackFrequency);

    m_nMaxBuffers = 32;
    m_sampleBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * m_nMaxBuffers];
    m_pResampleInBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * m_nMaxBuffers];
    m_pResampleOutBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * m_nMaxBuffers];
    m_pResampleFinalBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * 4];
    m_nRemainingInput = 0;
    m_nRemainingOutput = 0;
    m_offset = 0;

    InitializePortAudio();

    CreateQueue();

    OpenStream();

}

CAudioRenderer::~CAudioRenderer()
{
    CloseStream();

    //Pa_Terminate();

    delete [] m_sampleBuffer;

#ifndef USE_RINGBUFFER
    CSQueueManager::Instance()->ReleaseQueueSink(m_qAudioSink);
    CSQueueManager::Instance()->ReleaseQueueSource(m_qAudioSource);
#else
    delete m_ringBuffer;
#endif

    delete [] m_ringSampleBuffer;
    delete [] m_pResampleInBuffer;
    delete [] m_pResampleOutBuffer;
    delete [] m_pResampleFinalBuffer;

    delete m_pInputFrequencyAverager;
    delete m_pOutputFrequencyAverager;

#ifdef HAVE_RESAMPLE
    if (m_srcState != NULL)
        src_delete(m_srcState);
#endif
}

void CAudioRenderer::InitializePortAudio()
{
    if (m_bInitialized)
        return;
    CScopedLock lock(m_globalAudioMutex);
    if (m_bInitialized) // Double scoped lock
        return;
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        m_bError = true;
    }
    m_bInitialized = true;
}

void CAudioRenderer::CreateQueue()
{
#ifdef USE_RINGBUFFER
    m_ringBuffer = new PaUtilRingBuffer();
    m_ringSampleBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * m_nMaxBuffers];
    int rc = PaUtil_InitializeRingBuffer(m_ringBuffer, m_nSamplesPerBuffer * m_nBytesPerSample,
            m_nMaxBuffers, m_ringSampleBuffer);

    if (rc == -1) {
        m_bError = true;
    }
#else

    ostringstream qid;
    qid << "AudioRenderId_" << m_nId;
    m_qAudioSink = CSQueueManager::Instance()->GetQueueSink(qid.str());
    m_qAudioSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
#endif

}

void CAudioRenderer::SetupObjectId()
{
    CScopedLock lock(m_globalAudioMutex);
    m_nQueueId++;
    m_nId = m_nQueueId;
}

int CAudioRenderer::ComputeBytesPerSample()
{
    switch (m_eFormat) {
    case AFSigned32:
    case AFFloat:
        return 4;
    case AFSigned16:
    case AFUnsigned16:
        return 2;
    default:
        return 1;
    }
    return 2;
}

PaSampleFormat CAudioRenderer::AudioFormatToPAFormat()
{
    switch (m_eFormat) {
    case AFSigned16:
        return paInt16;
    case AFUnsigned16:
        return paInt16; // TBD: Translation??
    case AFSigned8:
        return paInt8;
    case AFUnsigned8:
        return paUInt8;
    case AFSigned32:
        return paInt32;
    case AFFloat:
        return paFloat32;
    case AFUnsupported:
        return 0;
    }
    return 0;
}

int CAudioRenderer::AudioCallback(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData)
{
    CAudioRenderer * ourObject = static_cast<CAudioRenderer *> (userData);
    if (ourObject != NULL) {
        return ourObject->AudioCallbackHandler(inputBuffer, outputBuffer,
                framesPerBuffer, timeInfo, statusFlags);
    }
    return paContinue;
}

int CAudioRenderer::AudioCallbackHandler(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags)
{
    // Drain our queue till we collect at least the needed amount of data
    int nLen = framesPerBuffer * m_nBytesPerSample;
    m_nRenderedBytes += nLen;
//    printf("%d: Playing sample %"PRIu64" of length = %d\n", m_nId,
//            GetRenderedSamples(), nLen);
    while (true) {
        if (m_offset >= nLen)
            break;
#ifdef USE_RINGBUFFER
        int availCount = PaUtil_GetRingBufferReadAvailable(m_ringBuffer);
        if (availCount <= 0)
            break;

//        printf("Copying %d at offset %d @ %"PRIu64" \n",
//                (int)m_ringBuffer->elementSizeBytes, m_offset, CTimeKeeper::GetTime());
        PaUtil_ReadRingBuffer(m_ringBuffer, m_sampleBuffer + m_offset, 1);
        //atomic_decr(&m_atomicVal);
        m_offset += m_ringBuffer->elementSizeBytes;
#else
        CSFrame *sFrame = m_qAudioSource->GetFrame();
        if (sFrame == NULL)
            break;
        //printf("Copying %d at offset %d\n", sFrame->GetLength(), m_offset);
        memcpy(m_sampleBuffer + m_offset, sFrame->GetData(), sFrame->GetLength());
        m_offset += sFrame->GetLength();
        m_qAudioSource->ReleaseFrame(sFrame);
#endif
    }


    // Do we have enough for one sample ?
    if (m_offset >= nLen) {
        // Copy a_nLen worth of data into SDL's buffer
        // and move the remaining up into our sample buffer
        //printf("Consuming bytes from m_sampleBuffer %d @ %"PRIu64" \n",
        //        nLen, CTimeKeeper::GetTime());
        memcpy(outputBuffer, m_sampleBuffer, nLen);
        memmove(m_sampleBuffer, m_sampleBuffer + nLen, m_offset - nLen);
        m_offset = m_offset - nLen;
    } else {
        // Insert silence
        m_nSilenceCount++;
//        printf("%d: Inserting silence of len %d at %d @ %"PRIu64"\n",
//                (int)m_nSilenceCount, nLen, (int)m_nRenderedBytes / nLen, CTimeKeeper::GetTime());
        memset(outputBuffer, 0x00, nLen);
    }

    return paContinue;
}

void CAudioRenderer::EnumerateDevices()
{
    int defIdx = Pa_GetDefaultHostApi();
    printf("Default host api index = %d\n", defIdx);
    int hapiCount = Pa_GetHostApiCount();
    printf("Host api count = %d\n", hapiCount);
    for (int idx = 0; idx < hapiCount; idx++) {
        const PaHostApiInfo * hAPIInfo =
                Pa_GetHostApiInfo(idx);
        printf("Host API %d = %s\n", idx, hAPIInfo->name);
    }

    int numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        printf( "ERROR: Pa_GetDeviceCount returned 0x%x\n", numDevices );
        exit (0);
    }

    printf( "Number of devices = %d\n", numDevices );
    int defaultDisplayed;
    for(int i=0; i<numDevices; i++ )
    {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo( i );
        printf( "--------------------------------------- device #%d\n", i );

    /* Mark global and API specific default devices */
        defaultDisplayed = 0;
        if( i == Pa_GetDefaultInputDevice() )
        {
            printf( "[ Default Input" );
            defaultDisplayed = 1;
        }
        else if( i == Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultInputDevice )
        {
            const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );
            printf( "[ Default %s Input", hostInfo->name );
            defaultDisplayed = 1;
        }

        if( i == Pa_GetDefaultOutputDevice() )
        {
            printf( (defaultDisplayed ? "," : "[") );
            printf( " Default Output" );
            defaultDisplayed = 1;
        }
        else if( i == Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultOutputDevice )
        {
            const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );
            printf( (defaultDisplayed ? "," : "[") );
            printf( " Default %s Output", hostInfo->name );
            defaultDisplayed = 1;
        }

        if( defaultDisplayed )
            printf( " ]\n" );

    /* print device info fields */
        printf( "Name                        = %s\n", deviceInfo->name );
        printf( "Host API                    = %s\n",  Pa_GetHostApiInfo( deviceInfo->hostApi )->name );
        printf( "Max inputs = %d", deviceInfo->maxInputChannels  );
        printf( ", Max outputs = %d\n", deviceInfo->maxOutputChannels  );

        printf( "Default low input latency   = %8.3f\n", deviceInfo->defaultLowInputLatency  );
        printf( "Default low output latency  = %8.3f\n", deviceInfo->defaultLowOutputLatency  );
        printf( "Default high input latency  = %8.3f\n", deviceInfo->defaultHighInputLatency  );
        printf( "Default high output latency = %8.3f\n", deviceInfo->defaultHighOutputLatency  );

    }
}

int CAudioRenderer::GetOutputDeviceIndex() {
#ifdef WINDOWS
    // Preferably use DirectSound's default output device on windows
    PaHostApiIndex hostApiIndex = Pa_HostApiTypeIdToHostApiIndex(paDirectSound);
    if (hostApiIndex < 0 ||
            Pa_GetHostApiInfo(hostApiIndex)->defaultOutputDevice < 0) {
        printf("Using default output device\n");
        return Pa_GetDefaultOutputDevice();
    }
    printf("Using %s default output device\n",
            Pa_GetHostApiInfo(hostApiIndex)->name);
    return Pa_GetHostApiInfo(hostApiIndex)->defaultOutputDevice;
#else
    printf("Using default output device for Alsa\n");
    return Pa_GetDefaultOutputDevice();
#endif
}

void CAudioRenderer::OpenStream()
{
    CScopedLock lock(m_globalAudioMutex);

    if (m_bError)
        return;

    //EnumerateDevices();


#if 1
    // Determine best output device to use
    int outputDeviceIndex = GetOutputDeviceIndex();
    if (outputDeviceIndex < 0) {
        m_bError = true;
        return;
    }

    // Open the audio stream
    PaStreamParameters outputParameters;
    outputParameters.device = outputDeviceIndex;
    outputParameters.channelCount = m_nChannels;
    outputParameters.sampleFormat = AudioFormatToPAFormat();
    outputParameters.suggestedLatency =
            Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    int nFramesPerBuffer = 0;
    PaError err = Pa_OpenStream(
              &m_pStream,
              NULL, /* no input */
              &outputParameters,
              m_nFrequency,
              nFramesPerBuffer,
              paNoFlag,
              CAudioRenderer::AudioCallback,
              this);
#else
    PaError err = Pa_OpenDefaultStream(
            &m_pStream, 0, m_nChannels, AudioFormatToPAFormat(),
            m_nFrequency, 256,
            CAudioRenderer::AudioCallback, this);
#endif
    if (err != paNoError) {
        m_bError = true;
        return;
    }

}

void CAudioRenderer::CloseStream()
{
    CScopedLock lock(m_globalAudioMutex);
    if (m_pStream != NULL) {
        Pa_CloseStream(m_pStream);
    }
}

bool CAudioRenderer::StartStreaming()
{
    if (m_bError)
        return false;

    if (m_bStarted)
        return false;
    PaError err = Pa_StartStream(m_pStream);
    if (err != paNoError) {
        return false;
    }
    m_bStarted = true;
    return true;
}

bool CAudioRenderer::StopStreaming()
{
    m_bStarted = false;
    if (m_bError)
        return false;

    PaError err = Pa_StopStream(m_pStream);
    if (err != paNoError) {
        return false;
    }

    return true;
}

bool CAudioRenderer::HasRoom() {

#ifdef USE_RINGBUFFER
    if (PaUtil_GetRingBufferWriteAvailable(m_ringBuffer) > 0)
        return true;
#else
    if (m_qAudioSource->Size() < m_nMaxBuffers)
        return true;
#endif
    return false;
}

void CAudioRenderer::FlushQueue(int keep) {
#ifdef USE_RINGBUFFER
    uint8_t buffer[m_nBytesPerSample * m_nSamplesPerBuffer];
    while (true) {
        int availCount = PaUtil_GetRingBufferReadAvailable(m_ringBuffer);
        if (availCount <= keep)
            break;
        PaUtil_ReadRingBuffer(m_ringBuffer, buffer, 1);
    }
#else
    // TODO
#endif

}

bool CAudioRenderer::RenderSample(uint8_t * buffer, int len)
{
    m_nRequestSamples++;
    m_nRequestBytes += len;
    if (m_bError) {
        m_nDroppedSamples++;
        return false;
    }
    if (!HasRoom()) {
        m_nDroppedSamples++;
        return false;
    }

#ifdef USE_RINGBUFFER
    if (len > m_nInputSamplesPerBuffer * m_nBytesPerSample) {
        printf("Sample too big %d\n", len);
        m_nDroppedSamples++;
        return false;
    }
//    if (len < m_nInputSamplesPerBuffer * m_nBytesPerSample) {
//        printf("Sample too small %d\n", len);
//        m_nDroppedSamples++;
//        return false;
//    }
    if (Pa_IsStreamStopped(m_pStream)) {
        m_nIgnoredSamples++;
        return false;
    }

    if (NeedToDropSample()) {
        m_nDroppedSamples++;
        return false;
    }

    uint8_t * inBuffer = Resample(buffer, len);
    if (inBuffer != NULL) {
        PaUtil_WriteRingBuffer(m_ringBuffer, inBuffer, 1);
        //atomic_incr(&m_atomicVal);
    }
    while ((inBuffer = ResampleRemainingBytes()) != NULL) {
        if (HasRoom()) {
            PaUtil_WriteRingBuffer(m_ringBuffer, inBuffer, 1);
        }
        else {
            m_nDroppedSamples++;
        }
    }
#else
    CSFrame *sFrame = new CSFrame((char *) buffer, len);
    m_qAudioSink->InsertFrame(sFrame);
#endif


//    if (m_nRequestSamples % 16 == 0) {
//        printf("Rendered = %"PRIu64", Dropped = %"PRIu64", Silence = %"PRIu64"\n",
//                GetRenderedSamples(), GetDroppedSamples(), GetSilenceSamples());
//    }
    return true;
}

bool CAudioRenderer::NeedToDropSample()
{
    // Compute average buffer length
    const double alpha = 0.05;
    int bufferLength = PaUtil_GetRingBufferReadAvailable(m_ringBuffer);
    m_avgBufferLength = (double)alpha * bufferLength + (double)(1.0 - alpha) * m_avgBufferLength;

    double bufferTime = (double)(1000.0 * bufferLength * m_nSamplesPerBuffer) / (1.0 * m_nFrequency);
    m_avgBufferTime = (double)alpha * bufferTime + (double)(1.0 - alpha) * m_avgBufferTime;

//    if (m_nRequestSamples % 100 == 0) {
//        printf("Current Level = %d\n", atomic_read(&m_atomicVal));
//    }

    if (m_bResample) {
        timestamp_t now = CTimeKeeper::Now();

        if (m_bDroppingPackets) {
            if (bufferTime <= 64) {
                printf("Done with dropping samples bl = %d, bt = %.02f\n", bufferLength, bufferTime);
                m_bDroppingPackets = false;
            }
            else {
                printf("Still dropping samples bl = %d, bt = %.02f\n", bufferLength, bufferTime);
                return true;
            }
        }

        if (now - m_nLastSampleDropTime < 5000)
            return false;

        double inputRate = ((double)(1000.0 * (m_nRequestBytes - m_nLastRequestBytes))) /
                ((now - m_nLastSampleDropTime) * m_nBytesPerSample);

        if (inputRate < (double)m_nInputFrequency * 0.9) {
            // Too little data. Audio possibly paused.
            // Need to reset resampling state
            printf ("Audio reset\n");
            m_nLastRenderedBytes = m_nRenderedBytes;
            m_nLastRequestBytes = m_nRequestBytes;
            m_nLastSampleDropTime = now;
            return false;
        }

        if (m_pInputFrequencyAverager->GetSampleCount() == 0)
            m_pInputFrequencyAverager->Sample(m_nInputFrequency);
        else
            m_pInputFrequencyAverager->Sample(inputRate);


        //int renderedBytes = m_nRenderedBytes - m_nLastRenderedBytes;
        double outputRate = ((double)(1000.0 * (m_nRenderedBytes - m_nLastRenderedBytes))) /
                ((now - m_nLastSampleDropTime) * m_nBytesPerSample);

        if (m_pOutputFrequencyAverager->GetSampleCount() == 0)
            m_pOutputFrequencyAverager->Sample(m_nFrequency);
        else
            m_pOutputFrequencyAverager->Sample(outputRate);

        double targetFactor = m_pOutputFrequencyAverager->GetMovingAverage()/
                m_pInputFrequencyAverager->GetMovingAverage();

        printf("Input = %f (%f) \nOutput = %f (%f) \nFactor = %f Target = %f \n",
                m_pInputFrequencyAverager->GetMovingAverage(), inputRate,
                m_pOutputFrequencyAverager->GetMovingAverage(), outputRate,
                targetFactor, m_nFrequency/targetFactor);

        double m_TargetBufferTime = 24.0;
        if (m_avgBufferTime > 512) {
            printf("Starting to drop samples bl = %d\n", bufferLength);
            m_bDroppingPackets = true;
            m_ResampleFactorAdjust = - 0.01;
        }
        else if (m_avgBufferTime > m_TargetBufferTime + 96.0 ) {
            m_ResampleFactorAdjust = - 0.01;
//            printf("Pulling Down Very Fast %f @ %"PRIu64"\n", GetInputFrequncy(), now);
        }
        else if (m_avgBufferTime > m_TargetBufferTime  + 48.0) {
            m_ResampleFactorAdjust = - 0.005;
//            printf("Pulling Down Fast %f @ %"PRIu64"\n", GetInputFrequncy(), now);
        }
        else if (m_avgBufferTime > m_TargetBufferTime + 16.0) {
            m_ResampleFactorAdjust = - 0.0005;
//            printf("Pulling Down %f @ %"PRIu64"\n", GetInputFrequncy(), now);
        }
        else if (m_avgBufferTime < m_TargetBufferTime - 16.0) {
            m_ResampleFactorAdjust = + 0.01;
//            printf("Pulling Up Fast %f @ %"PRIu64"\n", GetInputFrequncy(), now);
        }
        else if (m_avgBufferTime < m_TargetBufferTime - 8.0) {
            m_ResampleFactorAdjust = + 0.001;
//            printf("Pulling Up %f @ %"PRIu64"\n", GetInputFrequncy(), now);
        }
        else {
            m_ResampleFactorAdjust = 0.0;
//            printf("Applying avg %f\n", GetInputFrequncy());
        }

        m_ResampleFactor = targetFactor + m_ResampleFactorAdjust;

        m_nLastRenderedBytes = m_nRenderedBytes;
        m_nLastRequestBytes = m_nRequestBytes;
        m_nLastSampleDropTime = now;
        return false;
    }

    if (m_bDroppingPackets) {
        if (bufferTime <= 32) {
            printf("Done with dropping samples bl = %d\n", bufferLength);
            m_bDroppingPackets = false;
            return false;
        }
        printf("Still dropping samples bl = %d\n", bufferLength);
        return true;
    }

    // If average buffer length too large we drop a sample
//    if (m_avgBufferLength < 2)
//        return false;
    if (m_avgBufferTime < 128)
        return false;

    // Drop sample no more than once every 30 seconds
    timestamp_t now = CTimeKeeper::Now();
    if (now - m_nLastSampleDropTime > 5000) {
        printf("Dropping a sample after %d bl = %d\n", (int)(now - m_nLastSampleDropTime), bufferLength);
        m_nLastSampleDropTime = now;
        m_bDroppingPackets = true;
        //FlushQueue(1);
        return true;
    }
    return false;
}


#ifdef HAVE_RESAMPLE
void CAudioRenderer::SetupResampling(int a_nPlaybackFrequency)
{
    if (!m_bResample)
        return;
    /* Initialize the sample rate converter. */
    m_nInputFrequency = m_nFrequency;
    m_nFrequency = a_nPlaybackFrequency;
    if (m_nFrequency == 0)
        m_nFrequency = m_nInputFrequency;

    m_ResampleFactor = (double) m_nFrequency / (double) m_nInputFrequency;

    int error;
    if ((m_srcState = src_new(SRC_SINC_FASTEST, m_nChannels, &error)) == NULL) {
        m_bResample = false;
        m_nFrequency = m_nInputFrequency;
        return;
    }
    if (m_nSamplesPerBuffer < 256)
        m_nSamplesPerBuffer = 256;

}

uint8_t * CAudioRenderer::Resample(uint8_t * a_buffer, int a_len)
{
    if (!m_bResample)
        return a_buffer;
//    if (m_nRequestSamples > 10)
//        exit(0);

    const int floatBytesPerSample =  4 * m_nChannels;

    src_short_to_float_array((short *)a_buffer,
            (float *)m_pResampleInBuffer + m_nRemainingInput,
            (a_len * m_nChannels)/m_nBytesPerSample);


    int inSize = m_nRemainingInput/floatBytesPerSample + a_len/m_nBytesPerSample;


//    printf("Resample %d: inSize = %d (%d), remOutput = %d, factor = %f\n",
//            (int)m_nRequestSamples, inSize, a_len,
//            m_nRemainingOutput/floatBytesPerSample, m_ResampleFactor);

    SRC_DATA src_data ;
    src_data.data_in = (float *)m_pResampleInBuffer;
    src_data.input_frames = inSize;
    src_data.data_out = (float *)(m_pResampleOutBuffer + m_nRemainingOutput);
    src_data.output_frames = m_nSamplesPerBuffer * 4;
    src_data.src_ratio = m_ResampleFactor;
    src_data.end_of_input = 0;

    src_set_ratio(m_srcState, src_data.src_ratio); // Is this needed?
    int error = src_process(m_srcState, &src_data);
    if (error)
        return NULL;

    int outSize = src_data.output_frames_gen;
    int consumed = src_data.input_frames_used;

    // Adjust input buffer
    m_nRemainingInput = (inSize - consumed) * floatBytesPerSample;
    if (m_nRemainingInput > 0) {
        memmove(m_pResampleInBuffer,
                m_pResampleInBuffer + consumed * floatBytesPerSample,
                m_nRemainingInput);
    }

    m_nRemainingOutput += outSize * floatBytesPerSample;

//    printf("Resample %d: outSize = %d, consumed = %d, inSize = %d remOutput = %d\n",
//            (int)m_nRequestSamples, outSize, consumed, inSize, m_nRemainingOutput/floatBytesPerSample);

    // Adjust output buffer
    if (m_nRemainingOutput >= m_nSamplesPerBuffer * floatBytesPerSample) {
        // We have enough to send something out
        src_float_to_short_array((float *)m_pResampleOutBuffer,
                (short *)m_pResampleFinalBuffer, m_nSamplesPerBuffer * m_nChannels);
        m_nRemainingOutput -= m_nSamplesPerBuffer * floatBytesPerSample;
        memmove(m_pResampleOutBuffer,
                m_pResampleOutBuffer + m_nSamplesPerBuffer * floatBytesPerSample,
                m_nRemainingOutput);
        if (m_nRemainingOutput >= m_nSamplesPerBuffer * floatBytesPerSample) {
            printf("More samples left %d (%d)\n", m_nRemainingOutput,
                    m_nSamplesPerBuffer * floatBytesPerSample);
        }
        return m_pResampleFinalBuffer;
    }
//    printf("Not enough bytes\n");

    return NULL;
}
#else
void CAudioRenderer::SetupResampling(int a_nPlaybackFrequency)
{
    return;
}
uint8_t * CAudioRenderer::Resample(uint8_t * a_buffer, int a_len)
{
    return a_buffer;
}
#endif

uint8_t * CAudioRenderer::ResampleRemainingBytes() {
    const int floatBytesPerSample =  4 * m_nChannels;
    if (m_nRemainingOutput < m_nSamplesPerBuffer * floatBytesPerSample)
        return NULL;

    src_float_to_short_array((float *)m_pResampleOutBuffer,
            (short *)m_pResampleFinalBuffer, m_nSamplesPerBuffer * m_nChannels);
    m_nRemainingOutput -= m_nSamplesPerBuffer * floatBytesPerSample;
    memmove(m_pResampleOutBuffer,
            m_pResampleOutBuffer + m_nSamplesPerBuffer * floatBytesPerSample,
            m_nRemainingOutput);
    if (m_nRemainingOutput >= m_nSamplesPerBuffer * floatBytesPerSample) {
        printf("Still more to drain %d (%d)\n", m_nRemainingOutput,
                m_nSamplesPerBuffer * floatBytesPerSample);
    }
    return m_pResampleFinalBuffer;
}

uint64_t CAudioRenderer::GetRenderedSamples() {
    return m_nRenderedBytes/(m_nBytesPerSample * m_nSamplesPerBuffer);
}

uint64_t CAudioRenderer::GetRenderedBytes() {
    return m_nRenderedBytes;
}

uint64_t CAudioRenderer::GetRequestedBytes() {
    return m_nRequestBytes;
}

uint64_t CAudioRenderer::GetDroppedSamples() {
    return m_nDroppedSamples;
}

uint64_t CAudioRenderer::GetSilenceCount()
{
    return m_nSilenceCount;
}

int CAudioRenderer::GetBufferLength()
{
    return PaUtil_GetRingBufferReadAvailable(m_ringBuffer);
}

double CAudioRenderer::GetAvgBufferLength()
{
    return m_avgBufferLength;
}

double CAudioRenderer::GetAvgBufferTime()
{
    return m_avgBufferTime;
}

double CAudioRenderer::GetInputFrequncy()
{
    return m_nFrequency / m_ResampleFactor;
}

double CAudioRenderer::GetResampleAdjust()
{
    return m_ResampleFactorAdjust;
}
