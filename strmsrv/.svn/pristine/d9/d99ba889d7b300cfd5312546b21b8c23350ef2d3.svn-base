#ifndef __HH_MPEGTSTX_H__
#define __HH_MPEGTSTX_H__


#include "sender.hh"
#include "socket.hh"
#include "MPEGTSMuxFilter.hh"
#include "common.hh"
#include "shaper.hh"

class CMpegTsTx : public CSender
{
public:
    CMpegTsTx(int nId,
              char *pDestination,
              int nDestinationPort,
              int nTTL,
              int nInterface);

    ~CMpegTsTx();

    int ProcessStream();
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    CInternetAddress                m_destination;
    CSocketDgram                    m_socket;
    
    MPEGTSMuxFilter                 *m_pMPEGTSMuxFilter;
    unsigned long                   m_framesServed;
    unsigned long                   m_packetsServed;
    int                             m_nOffset;
    
    CShaper                         *m_pTokenBucket;
    std::string                          m_sInterfaceInfo;

    int doSend(char *pData, int nLen);
};

#define MPEGTSPKTSIZE           188

#endif /*__HH_MPEGTSTX_H__*/
