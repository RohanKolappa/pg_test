#if !defined __V2D_AUDIO_DECODER_H__
#define __V2D_AUDIO_DECODER_H__

class CAudioDecoder
{
public:
    CAudioDecoder(void);
    ~CAudioDecoder(void);

    bool DecodeSample(const unsigned char* a_pEncodedData,
                      unsigned char* a_pDecodedData,
                      const int a_nLen);

    unsigned int GetDecodedAudioFrames() const; 

private:
    unsigned int m_nAudioFramesDecoded;

    bool ConvertNetworkOrderToPCMSamples(const unsigned char* a_pEncodedData,
                                         unsigned char* a_pDecodedData,
                                         const int a_nDataLen);
};
#endif // __V2D_AUDIO_DECODER_H__
