#if !defined __V2D_AUDIO_FRAME_PARSER_HH__
#define __V2D_AUDIO_FRAME_PARSER_HH__

class CAudioFrameParser
{
public:
    CAudioFrameParser(void);
    ~CAudioFrameParser(void);

    bool GetAudioSofEofIndex(const unsigned char* a_pInputData, 
                             const unsigned int& a_nLen, 
                             const bool& a_bSrcRTP,
                             unsigned int& a_nSof, 
                             unsigned int& a_nEof );
};

#endif // __V2D_AUDIO_FRAME_PARSER_HH__
