/*
 * AudioRenderer.hh
 *
 *  Created on: Apr 27, 2011
 *      Author: rkale
 */

#ifndef AUDIORENDERER_HH_
#define AUDIORENDERER_HH_

#include "squeue.hh"

typedef void PaStream;
struct PaStreamCallbackTimeInfo;
typedef unsigned long PaStreamCallbackFlags;
typedef unsigned long PaSampleFormat;

#include "pa_ringbuffer.h"
#include "averager.hh"

struct SRC_STATE_tag;

class CAudioRenderer {
public:
    enum eAudioFormat {
        AFSigned16,
        AFUnsigned16,
        AFSigned8,
        AFUnsigned8,
        AFSigned32,
        AFFloat,
        AFUnsupported,
    };

    CAudioRenderer(
            int a_nFrequency, int a_nChannels,
            int a_nSamplesPerBuffer, eAudioFormat a_eFormat,
            bool a_bResample = false, int a_nPlaybackFrequency = 0);

    ~CAudioRenderer();

    bool StartStreaming();
    bool StopStreaming();
    bool HasRoom();
    bool RenderSample(uint8_t * buffer, int size);
    bool GetErrorStatus() { return m_bError; };
    bool IsStarted() { return m_bStarted; };
    int GetChannels() { return m_nChannels; };
    int GetFrequency() { return m_nFrequency; };
    int GetSampleSize() { return m_nInputSamplesPerBuffer; };
    int GetSampleTime() { return (int)((1000.0 * m_nInputSamplesPerBuffer)/m_nFrequency); };
    int GetPacketSize() { return m_nInputSamplesPerBuffer * m_nBytesPerSample; };

    uint64_t GetRenderedSamples();
    uint64_t GetRenderedBytes();
    uint64_t GetRequestedBytes();
    uint64_t GetDroppedSamples();
    uint64_t GetSilenceCount();
    int GetBufferLength();
    double GetAvgBufferLength();
    double GetAvgBufferTime();
    double GetInputFrequncy();
    double GetResampleAdjust();

private:
    static int AudioCallback(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData);

    int AudioCallbackHandler(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags);

    void SetupObjectId();
    void InitializePortAudio();
    void CreateQueue();
    void FlushQueue(int keep = 0);
    int ComputeBytesPerSample();
    PaSampleFormat AudioFormatToPAFormat();
    void EnumerateDevices();
    int GetOutputDeviceIndex();
    void OpenStream();
    void CloseStream();
    bool NeedToDropSample();
    void SetupResampling(int a_nPlaybackFrequency);
    uint8_t * Resample(uint8_t * a_buffer, int a_len);
    uint8_t * ResampleRemainingBytes();


    uint8_t *m_sampleBuffer;
    int m_offset;
    uint8_t *m_ringSampleBuffer;
    PaUtilRingBuffer *m_ringBuffer;
    double m_avgBufferLength;
    double m_avgBufferTime;
    struct SRC_STATE_tag * m_srcState;

    uint8_t *m_pResampleOutBuffer;
    uint8_t *m_pResampleInBuffer;
    uint8_t *m_pResampleFinalBuffer;
    int m_nRemainingInput;
    int m_nRemainingOutput;

    uint64_t m_nSilenceCount;
    uint64_t m_nRenderedBytes;
    uint64_t m_nDroppedSamples;
    uint64_t m_nRequestSamples;
    uint64_t m_nRequestBytes;
    uint64_t m_nIgnoredSamples;
    timestamp_t m_nLastSampleDropTime;
    timestamp_t m_nBeginTime;
    uint64_t m_nLastRenderedBytes;
    uint64_t m_nLastRequestBytes;

    //atomic_t m_atomicVal;
    bool m_bError;
    bool m_bStarted;
    bool m_bResample;
    bool m_bDroppingPackets;
    int m_nFrequency;
    int m_nInputFrequency;
    eAudioFormat m_eFormat;
    int m_nChannels;
    int m_nInputSamplesPerBuffer;
    int m_nSamplesPerBuffer;
    int m_nBytesPerSample;
    int m_nMaxBuffers;
    double m_ResampleFactor;
    double m_ResampleFactorAdjust;
    CAverager<double> * m_pInputFrequencyAverager;
    CAverager<double> * m_pOutputFrequencyAverager;

    PaStream *m_pStream;
    CSQueueSink *m_qAudioSink;
    CSQueueSource *m_qAudioSource;
    int m_nId;
    static int m_nQueueId;
    static CMutex m_globalAudioMutex;
    static bool m_bInitialized;
};

#endif /* AUDIORENDERER_HH_ */
