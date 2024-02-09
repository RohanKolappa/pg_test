#ifndef __HH_UDPTX_H__
#define __HH_UDPTX_H__

#include "sender.hh"
#include "socket.hh"

class CUdpTx : public CSender
{
public:
    CUdpTx(int nId,
           char *pDest,
           int nPort,
           int nTTL,
           int nPerfInterval,
           int a_interface);
    ~CUdpTx();

    int ProcessStream();

    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);


private:
    CInternetAddress                m_destination;
    CSocketDgram                    m_socket;
    struct SPerfData *              m_perfData;
    struct SPerfData *              m_perfSnapShot;
    int                             m_perfInterval;
    timestamp_t                     m_nProgressTimestamp;
    unsigned short                  m_nSessionId;
    std::string                     m_sInterfaceInfo;

    virtual void HandleFirstTimestamp(timestamp_t nFirstTimeStamp);
    int StopSender();

};
#endif /*__HH_V2DTX_H__*/
