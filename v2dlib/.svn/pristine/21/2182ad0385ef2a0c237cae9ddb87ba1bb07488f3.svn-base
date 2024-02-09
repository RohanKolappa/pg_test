#include "v2dlib_compatibility.h"
#include "AudioDecoder.h"


#define V2DLIB_HI_BYTE(x) (0x00FF&((x) >> 8) )
#define V2DLIB_LOW_BYTE(x) (0x00FF& (x))
#define V2DLIB_REVERSE_WORD(x) ((V2DLIB_LOW_BYTE(x)<<8)|(V2DLIB_HI_BYTE(x)))

CAudioDecoder::CAudioDecoder(void)
: m_nAudioFramesDecoded(0)
{
}

CAudioDecoder::~CAudioDecoder(void)
{
}

bool CAudioDecoder::DecodeSample(const unsigned char* a_pEncodedData, 
                                 unsigned char* a_pDecodedData,
                                 const int a_nLen)
{
    m_nAudioFramesDecoded++;

    return ConvertNetworkOrderToPCMSamples(a_pEncodedData, a_pDecodedData, a_nLen);
}

/*
NOTE: "Assumption: The audio input is in big endian (network order) and the playback or 
 output device (sound card or wave file) expects little endian samples. 
 If the output device expects big endian order samples then the macro must be defined to 
 be a no-op."
 E.g. 
#ifdef BIG_ENDIAN
V2D_REVERSE_WORD(x) (x)
#else
V2D_REVERSE_WORD(x) ..... 
#endif
*/
bool CAudioDecoder::ConvertNetworkOrderToPCMSamples(const unsigned char *a_pEncodedData, 
                                                    unsigned char *a_pDecodedData,
                                                    const int a_nDataLen)
{
    for(int i = 0; i < a_nDataLen; i += 2)
	{
        const unsigned short *nSample = reinterpret_cast<const unsigned short *>
                                                (a_pEncodedData + i);
        unsigned short nSampleSwapped = V2DLIB_REVERSE_WORD(*nSample);
		::memcpy(a_pDecodedData + i, &nSampleSwapped, 2);
	}

    return true;
}

unsigned int CAudioDecoder::GetDecodedAudioFrames() const
{
    return m_nAudioFramesDecoded;
}
