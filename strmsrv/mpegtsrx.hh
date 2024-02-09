#ifndef __HH_mpegtsRX_H__
#define __HH_mpegtsRX_H__

#include "receiver.hh"
#include "socket.hh"

class CMpegTsRx : public CReceiver
{

public:
    CMpegTsRx(int nId,
           char* sourceIp,
           char* multicastIp,
           int destPort,
           intlist & a_interfaceList);
    ~CMpegTsRx();

    int ProcessStream ();

    void UpdateCustomStatus(CCommand *a_pCmd);

private:
   unsigned short          m_nPort;
   CInternetAddress        m_source;
   CInternetAddress        m_srcCheck;
   CSocketDgram            m_socket;
   CSBuffer *              m_sBuffer;
   static const int        m_BUFFER_SIZE;
   unsigned long           m_readOffset;
   int                     m_reallocCount;
   int                     m_nonmulterr;
   int                     m_maxPacketSize;
   int                     m_maxPacketLastSec;  
   int                     m_wrongSourceErr;
   char                    m_sourceIp[60];
   char                    m_multicastIp[60];
   bool                    m_bissrcIp;
   std::string             m_sInterfaceInfo;


};
#endif/* __HH_mpegtsRX_H__*/
