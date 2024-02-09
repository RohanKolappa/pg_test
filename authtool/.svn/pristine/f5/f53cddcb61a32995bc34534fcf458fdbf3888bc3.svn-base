/*
 * AudioResample.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: rkale
 */


#include "IPVSTranscode_compatibility.h"
#include "AudioResampler.hh"

#include <samplerate.h>

CAudioResampler::CAudioResampler(int nChannels, int nBitsPerSample,
        int nSamplesPerBuffer, double resampleFactor, bool fastResample)
{
    m_nChannels = nChannels;
    m_ResampleFactor = resampleFactor;
    m_nBytesPerSample = m_nChannels * nBitsPerSample/8;
    m_nSamplesPerBuffer = nSamplesPerBuffer;

    m_bError = false;
    int error;
    int resampleQuality = SRC_SINC_FASTEST;
    if (fastResample)
        resampleQuality = SRC_LINEAR;
    if ((m_srcState = src_new(resampleQuality, m_nChannels, &error)) == NULL) {
        m_bError = true;
    }

    m_pInBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * m_nChannels * 16];
    m_pOutBuffer = new uint8_t[m_nBytesPerSample * m_nSamplesPerBuffer * m_nChannels * 16];
    m_nRemainingInput = 0;
    m_nRemainingOutput = 0;


}

CAudioResampler::~CAudioResampler()
{
    delete [] m_pInBuffer;
    delete [] m_pOutBuffer;

    if (m_srcState != NULL)
        src_delete(m_srcState);
}


bool CAudioResampler::Resample(uint8_t * a_outBuffer, uint8_t * a_inBuffer, int a_len)
{
    if (m_bError)
        return false;
    const int floatBytesPerSample =  4 * m_nChannels;

    src_short_to_float_array((short *)a_inBuffer,
            (float *)m_pInBuffer + m_nRemainingInput,
            (a_len * m_nChannels)/m_nBytesPerSample);


    int inSize = m_nRemainingInput/floatBytesPerSample + a_len/m_nBytesPerSample;


//    printf("Resample %d: inSize = %d (%d), remOutput = %d, factor = %f\n",
//            (int)m_nRequestSamples, inSize, a_len,
//            m_nRemainingOutput/floatBytesPerSample, m_ResampleFactor);


    SRC_DATA src_data ;
    src_data.data_in = (float *)m_pInBuffer;
    src_data.input_frames = inSize;
    src_data.data_out = (float *)(m_pOutBuffer + m_nRemainingOutput);
    src_data.output_frames = m_nSamplesPerBuffer * 4;
    src_data.src_ratio = m_ResampleFactor;
    src_data.end_of_input = 0;

    src_set_ratio(m_srcState, src_data.src_ratio); // Is this needed?
    int error = src_process(m_srcState, &src_data);
    if (error)
        return false;


    int outSize = src_data.output_frames_gen;
    int consumed = src_data.input_frames_used;

    // Adjust input buffer
    m_nRemainingInput = (inSize - consumed) * floatBytesPerSample;
    if (m_nRemainingInput > 0) {
        ::memmove(m_pInBuffer,
                m_pInBuffer + consumed * floatBytesPerSample,
                m_nRemainingInput);
    }

    m_nRemainingOutput += outSize * floatBytesPerSample;

//    printf("Resample %d: outSize = %d, consumed = %d, inSize = %d remOutput = %d\n",
//            (int)0, outSize, consumed, inSize, m_nRemainingOutput/floatBytesPerSample);

    // Adjust output buffer
    if (m_nRemainingOutput >= m_nSamplesPerBuffer * floatBytesPerSample) {
        // We have enough to send something out
        src_float_to_short_array((float *)m_pOutBuffer,
                (short *)a_outBuffer, m_nSamplesPerBuffer * m_nChannels);
        m_nRemainingOutput -= m_nSamplesPerBuffer * floatBytesPerSample;
        ::memmove(m_pOutBuffer,
                m_pOutBuffer + m_nSamplesPerBuffer * floatBytesPerSample,
                m_nRemainingOutput);
        if (m_nRemainingOutput >= m_nSamplesPerBuffer * floatBytesPerSample) {
            printf("More samples left %d (%d)\n", m_nRemainingOutput,
                    m_nSamplesPerBuffer * floatBytesPerSample);
        }
        return true;
    }
    //printf("Not enough bytes\n");

    return false;
}

void CAudioResampler::SetResampleFactor(double resampleFactor)
{
    m_ResampleFactor = resampleFactor;
}

void CAudioResampler::Reset()
{
    m_nRemainingInput = 0;
    m_nRemainingOutput = 0;
    src_reset(m_srcState);
}
