#ifndef __HH_RTPTX_H__
#define __HH_RTPTX_H__



#include "sender.hh"
#include "wis.hh"
#include "rtp.hh"
#include "socket.hh"

class CRtpTx : public CSender
{
public:
    CRtpTx(int nId,
           char *pDestination,
           int nDestinationPort,
           int nTTL, 
           int videoTs, 
           int audioTs,
           unsigned long videoFrameRate,
           unsigned long videoFrameRateDivisor,
           int nInterface,
           int bAudioOnly);

    ~CRtpTx();

    int ProcessStream();

    void UpdateCustomStatus(CCommand *a_pCmd);

    virtual int StartSenderStream(int nQueueId);

private:
    CSocketDgram            m_socket;
    CInternetAddress        m_AudioDestination;
    CInternetAddress        m_VideoDestination;
    int                     m_nAudioSeq;
    int                     m_nVideoSeq;
    int                     m_nOffset;
    unsigned long           m_nVideoTs;
    unsigned long           m_nAudioTs;
    char *                  m_pRtpBuffer;
    CSFrame *               m_sFrame;
    unsigned long m_nAudioSsrc;
    unsigned long m_nVideoSsrc;
    unsigned long m_nVideoFrameRate;
    unsigned long m_nVideoFrameRateDivisor;

    unsigned long m_nAudioPktsSend;
    unsigned long m_nAudioBytesSend;
    unsigned long m_nVideoPktsSend;
    unsigned long m_nVideoBytesSend;

    unsigned long m_nWisRawCount;
    unsigned long m_nCodecMP2VCount;
    unsigned long m_nCodecMP4VCount;
    unsigned long m_nCodecMPACount;
    unsigned long m_nCodecAACCount;

    std::string m_sInterfaceInfo;

    char* m_pcModifiedFrame;
    int   m_nModifiedFrameLen;
    
    unsigned long m_nDiscontinuities;
    bool m_bAudioOnly;

    unsigned long m_nInputVideoSsrc;
    unsigned long m_nInputAudioSsrc;

    unsigned long m_nInVideoSsrcCount;
    unsigned long m_nInAudioSsrcCount;

    unsigned long m_nOutVideoSsrcCount;
    unsigned long m_nOutAudioSsrcCount;

    /// Private functions
    int SendAudioAACFrame(char *pData, int nLen, unsigned long ts);
    int SendAudioMPAFrame(char *pData, int nLen, unsigned long ts);
    int SendAudioMP4LATMFrame(char *pData, int nLen, unsigned long ts);
    int SendVideoMP4Frame(char *pData, int nLen, unsigned long ts);

    int Mpeg4FindVopOffset (uint8_t *pBuf, uint32_t buflen);

    void GenerateNewSSRCs();
    bool ModifyFrameSSRC(const char* a_pcFrame, const int a_nFrameLen,
                        const short a_nFrameType);

    void HandleInputSSRCChange(const char *pData, const int a_nFrameLen,
                               const short a_nFrameType);
    void HandleAudioInputSSRCChange(unsigned long a_nCurrentSSRC);
    void HandleVideoInputSSRCChange(unsigned long a_nCurrentSSRC);
};

#define RTPSTATINTERVAL 1000

#define MPEG4_VOSH_START      0xB0
#define MPEG4_VOSH_END        0xB1
#define MPEG4_USER_DATA_START 0xB2
#define MPEG4_GOV_START       0xB3
#define MPEG4_VO_START        0xB5
#define MPEG4_VOP_START       0xB6

#define MIN_VIDEOTS 1000
#define MIN_AUDIOTS 1000

#endif /*__HH_RTPTX_H__*/
