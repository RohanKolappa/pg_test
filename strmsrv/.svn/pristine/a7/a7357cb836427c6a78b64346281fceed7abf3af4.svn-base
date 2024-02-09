#ifndef __HH_RtpRX_H__
#define __HH_RtpRX_H__

#include "RTPSession.hh"
#include "receiver.hh"


class CRtpRx : public CReceiver
{
private:
    RTPSession *            _session;

    std::string m_strSrcIP;
    std::string m_strAudioSrcIP;
    std::string m_strRemoteIP;

    int    m_nAudioPort;
    int    m_nVideoPort;

    int updateRxFrame(char *pData,
                      int nLen,
                      unsigned long nFrameType);

public:
    CRtpRx(int nId,
           char *ip,
           char *audioip,
           int videoPort,
           int audioPort,
           char *videoCodec,
           char *audioCodec,
           intlist & a_interfaceList);
    ~CRtpRx();

    int ProcessStream ();

    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);

};

#endif /*__HH_RtpRX_H__*/
