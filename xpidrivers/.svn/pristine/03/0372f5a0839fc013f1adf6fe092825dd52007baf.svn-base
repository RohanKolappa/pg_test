#include <unistd.h> 
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 
#include <netpacket/packet.h> 
#include <net/ethernet.h>
#include <netinet/in.h>
#include <sys/types.h> 
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <fcntl.h>
#include <stdio.h>
#include <sstream>

#include <vector>

#include "RawSocket.h"
#include "Exception.h"

namespace LabX {

  using namespace std;

  RawSocket::Packet::Packet(RawSocket &socket, uint16_t etherType, const MacAddress &ourMac) :
    m_dptr(kDataStart), m_refCount(1),  m_socket(socket)
  { 
    m_buf = new uint8_t[kMaxPacketLength];
    if (NULL == m_buf) 
      {
        throw(new Exception("Error allocating packet buffer"));
      }

    // Fill in the ethertype
    m_buf[12] = (etherType>>8) & 0xFF;
    m_buf[13] = (etherType) & 0xFF;

    // Fill in the source MAC
    for (int i=0;i<6;++i) m_buf[i+6] = ourMac[i];
  }

  void RawSocket::Packet::AddRef(void) 
  { 
    ++m_refCount; 
  }

  void RawSocket::Packet::Release(void)
  { 
    --m_refCount; 
    if (m_refCount == 0) m_socket.FreePacket(this);
  }

  void RawSocket::Packet::Clear(void)
  {
    memset(m_buf, 0, kMaxPacketLength);
    m_refCount = 0;
    m_dptr = kDataStart;
  }

  RawSocket::Packet::~Packet(void)
  {
    if (m_buf)
      {
        delete [] m_buf;
        m_buf = NULL;
      }
  }

  bool RawSocket::Packet::WriteBuffer(const uint8_t *data, size_t size)
  {
    if (size > freeSpace()) 
      {
        printf("Rawsocket::Packet::%s: Rejecting write request: size %d, Write Request size %d\n",__func__,(int)freeSpace(),(int)size);
        return false;
      }

    memcpy(&m_buf[m_dptr], data, size);
    m_dptr += size;
    return true;
  }

  void RawSocket::Packet::print(void) 
  { 
    printf("RawSocket::Packet::%s: Size: %d\n",__func__,m_dptr);
    for (int i=0;i<m_dptr;++i) printf("%02x ",m_buf[i]);
    printf("\n");
  }

  void RawSocket::Packet::printData(void)
  {
    printf("RawSocket::Packet::%s: Size: %d\n",__func__,m_dptr);
    for (int i=kDataStart;i<m_dptr;++i) printf("0x%x ",m_buf[i]);
    printf("\n");
  }

  RawSocket::RawSocket(void) : Thread("RawSocket"),m_threadRunning(true),m_socketSem("SocketSem"),m_poolSem("PoolSem")
  {
    m_socketfd = -1;
  }

  RawSocket::~RawSocket(void)
  {
    close(m_socketfd);
    m_socketfd = -1;

    // Delete packets from the free pool
    for(std::vector<Packet *>::iterator i = m_freePool.begin(); i != m_freePool.end(); i++)
      {
        delete (*i);
      }
    m_freePool.clear();
  }

  void RawSocket::Init(uint16_t etherType, int poolSize, const std::string& interfaceName)
  {

    // Close the old socket if we had one
    if (m_socketfd >= 0)
      {
        close(m_socketfd);
      } 

    m_interfaceName = interfaceName;

    m_etherType = etherType;

    m_socketfd = socket(PF_PACKET, SOCK_RAW, htons(m_etherType));
    if (m_socketfd < 0) 
      {
        stringstream excStream;
        excStream << "RawSocket::Init() - Error allocating socket: " << strerror(errno); 
        throw(new Exception(excStream.str()));
      }

    /* Grab our MAC */
    struct ifreq ifr;
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ-1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);

    m_ourMacAddress = MacAddress((uint8_t*)&ifr.ifr_hwaddr.sa_data);

    /* Allocate Pool */
    for (int i=0;i<poolSize;++i)
      {
        try
          {
            Packet *p = new Packet(*this, etherType, m_ourMacAddress);
            if (NULL == p)
              {
                throw(new Exception("Unable to allocate packet"));
              }
            p->Release();
          }
        catch(...)
          {
            throw(new Exception("Unable to allocate packet"));
          }
      }

    start();
  }

  bool RawSocket::SubscribeMcast(const MacAddress &mcaddr)
  {
    struct ifreq req; 
    int res;

    memset(&req,0,sizeof(ifreq));

    strcpy(req.ifr_name, m_interfaceName.c_str());

    memcpy(req.ifr_ifru.ifru_addr.sa_data, mcaddr, 6);
    /* req.ifr_flags = IFF_ALLMULTI; | IFF_PROMISC; */

    res = ioctl(m_socketfd, SIOCADDMULTI, &req);
    if(res == -1)
      {
        printf("RawSocket::%s:%s\n",__func__,strerror(errno));
        return false;
      } 

    return true;
  }

  void RawSocket::Attach(RawSocketObserver* o)
  {
    m_observers.push_back(o);
  }

  void RawSocket::Detach(RawSocketObserver *o)
  {
    vector <RawSocketObserver *>::iterator it;
    for (it = m_observers.begin(); it!=m_observers.end(); ++it)
      {
        if ((*it) == o) it = m_observers.erase(it);
        else ++it;
      }
  }

  void RawSocket::Notify(RawSocket::Packet *p)
  {
    vector<RawSocketObserver *>::iterator it;
    for (it = m_observers.begin(); it!=m_observers.end(); ++it)
      {
        (*it)->PacketReceived(p);
      }
  }

  void RawSocket::FreePacket(RawSocket::Packet *p)
  {
    p->Clear();
    m_freePool.push_back(p);
  }

  RawSocket::Packet * RawSocket::GetPacket(void)
  {
    m_poolSem.wait();
    Packet *p;
    if (m_freePool.size() == 0) return NULL;

    /* Note - This works since m_freePool is a vector of pointers */
    p = m_freePool[0];
    m_freePool.erase(m_freePool.begin());
    //  printf("RawSocket::%s: Removing packet %p from Pool. New Pool Size: %d\n",__func__,p,m_freePool.size());
    p->AddRef();
    m_poolSem.post();
    return p;
  }

  bool RawSocket::Send(RawSocket::Packet *packet, const MacAddress &toMac)
  {
    struct ifreq ifreq;
    struct sockaddr_ll sll;

    m_socketSem.wait();

    /*  printf("RawSocket::%s: m_interface: %s\n",__func__,m_interface);
        printf("RawSocket::%s: Dst Address: ",__func__);
    */

    //	packet->print();
    //	for (int i=0;i<6;++i) printf("0x%x:",toMac[i]);
    //	printf("\n");

    for (int i=0;i<6;++i) packet->bufStart()[i] = toMac[i];
    for (int i=6;i<12;++i) packet->bufStart()[i] = m_ourMacAddress[i-6];

    packet->bufStart()[12] = (m_etherType>>8) & 0xFF;
    packet->bufStart()[13] = (m_etherType) & 0xFF;

    memset(&sll, 0, sizeof(struct sockaddr_ll));

    strncpy(ifreq.ifr_ifrn.ifrn_name, m_interfaceName.c_str(), IFNAMSIZ);
    if (ioctl(m_socketfd, SIOCGIFINDEX, &ifreq) < 0) 
      {
        printf("Error Getting Interface Index\n");
        goto error;
      }       

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifreq.ifr_ifru.ifru_ivalue;
    if (sendto(m_socketfd, packet->bufStart(), packet->dataSize()+14, 0, 
               (struct sockaddr *)&sll, sizeof(struct sockaddr_ll)) < 0) 
      {
        printf("Error Sending Packet\n");
        goto error;
      }

    m_socketSem.post();
    return true;

  error:
    m_socketSem.post();
    return false;
  }

  int32_t RawSocket::run(void)
  {
	unsigned char packetBuf[Packet::kMaxPacketLength];
	int n=0;

	fprintf(stderr,"RawSocket::%s: Thread Running !!!!!!!!\n",__func__);

	while (m_threadRunning)
      {
		n = recvfrom(m_socketfd, packetBuf, Packet::kMaxPacketLength, 0, NULL, NULL);
		if (n < 0)
          {
			if (errno == EINTR) continue;
			perror("RawSocket::Read");
          }
		else if (n > 0)
          {

			if (0 != memcmp(&packetBuf[6],&m_ourMacAddress[0],6))
              {
				/* Only care about packets from other MACs. Ignore ones
				   from us due to multicast traffic */
				Packet *p = GetPacket();
				if (NULL == p) printf("RawSocket::%s: Out of packets\n",__func__);

                //				printf("\n");
                //				for (int i=0;i<n;++i) printf("0x%X ",packetBuf[i]);
                //				printf("\n");
				p->rewind(0);
				p->WriteBuffer(&packetBuf[0],n);
				Notify(p);
				p->Release();
              }
          }
      }
    
    return(Thread::k_Success);
  }

} // Namespace LabX

