#include "AudioFrameParser.h"
#include "v2d.h"
#include "rtp.h"

CAudioFrameParser::CAudioFrameParser(void)
{
}

CAudioFrameParser::~CAudioFrameParser(void)
{
}

bool CAudioFrameParser::GetAudioSofEofIndex(const unsigned char *a_pInputBuffer,
                                            const unsigned int& a_rnLen, 
                                            const bool& a_rbIsSrcRTP,
                                            unsigned int& a_rnSof, 
                                            unsigned int& a_rnEof)
{
    if(a_rbIsSrcRTP)
    {
        a_rnSof = RTP_HEADER_SIZE;
        a_rnEof = a_rnLen;
    }
    else
    {
        int iCode = 0;
        
        unsigned int iIncr = 0;
        bool bFindSof = true;
        const unsigned char *pcCode = a_pInputBuffer + iIncr;    

	    int iCodeToLookFor = V2D_AUDIO_SOF_CODE;

        while( iIncr < a_rnLen )
        {
            iCode = pcCode[0];
            iCode <<= 8;
            iCode |= pcCode[1];
            iCode <<= 8;
            iCode |= pcCode[2];        
            iCode <<= 8;
            iCode |= pcCode[3];        

            if ( iCode == iCodeToLookFor )
            {
                if ( bFindSof )
                {
                    a_rnSof = iIncr + 8;

                    iIncr = a_rnSof;
                    iCodeToLookFor =  V2D_AUDIO_EOF_CODE;
                    pcCode = a_pInputBuffer + iIncr;
                    bFindSof = false;

                    continue;
                }
                else
                    a_rnEof = iIncr;

                break;
            }

            // move to next 4 bytes
            iIncr += 4;
            pcCode = a_pInputBuffer + iIncr; 
        } //while
    }

    return true;
}
