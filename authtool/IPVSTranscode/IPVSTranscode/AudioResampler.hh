/*
 * AudioResample.hh
 *
 *  Created on: Feb 13, 2012
 *      Author: rkale
 */

#ifndef AUDIORESAMPLE_HH_
#define AUDIORESAMPLE_HH_

struct SRC_STATE_tag;

class CAudioResampler
{
public:
    CAudioResampler(int nChannels, int nBitsPerSample,
            int nSamplesPerBuffer, double resampleFactor, bool fastResample = false);

    ~CAudioResampler();

    bool Resample(uint8_t * a_outBuffer, uint8_t * a_inBuffer, int a_len);
    void SetResampleFactor(double resampleFactor);
    void Reset();

private:
    struct SRC_STATE_tag * m_srcState;
    double m_ResampleFactor;
    int m_nChannels;
    int m_nBytesPerSample;
    int m_nSamplesPerBuffer;
    uint8_t * m_pOutBuffer;
    uint8_t * m_pInBuffer;
    int m_nRemainingInput;
    int m_nRemainingOutput;
    bool m_bError;
};

#endif /* AUDIORESAMPLE_HH_ */
