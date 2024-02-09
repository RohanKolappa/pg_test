#ifndef __HH_UDPRX_H__
#define __HH_UDPRX_H__

#include "receiver.hh"
#include "socket.hh"


class CUdpRx : public CReceiver
{
public:
    CUdpRx(int nId, 
           int a_sourcePort,
           const char *a_mcastaddr,
           int nPerfInterval,
           int a_genBW,
           intlist & a_interfaceList);
    
    ~CUdpRx();
    
    int ProcessStream();
    
    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);
    
private:
    unsigned short          m_nPort;
    CInternetAddress        m_source;
    CSocketDgram            m_socket;
    CSBuffer *              m_sBuffer;
    unsigned long           m_readOffset;
    struct SPerfData *      m_perfData;
    struct SPerfData *      m_perfSnapShot;
    int                     m_perfInterval;
    unsigned long           m_generateBw;
    unsigned long long      m_nTotalbits;
    unsigned long long      m_nSequence;
    unsigned char *         m_fillBuffer;
    std::string                  m_sInterfaceInfo;

    static const int        m_BUFFER_SIZE;
    
};
#endif /* __HH_UDPRX_H__ */
