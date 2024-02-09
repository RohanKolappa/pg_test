#ifndef __H_SOCKET__
#define __H_SOCKET__

#include "address.hh"

#define INVALID_SOCKET_FD   (-1)

// CSocket class
class CSocket {

public:

    int CreateSocket(int a_domain, int a_type, int a_protocol);
    void Close();

    int GetFd(void) const { return m_fd; };
    bool IsValid(void) const { return m_fd == INVALID_SOCKET_FD ? false : true; };
    
    int SetNonBlocking(bool a_nonblock);
    int SetReuseAddress(bool a_reuse);
    int SetReceiveBufferSize(int a_buffersize);
    virtual int SetMcastTTL(int a_ttl);
    virtual int SetMcastLoopback(bool a_loopback);
    std::string GetLastError() const { return m_errstring; };
    
    virtual CInternetAddress GetPeerAddress(void) const = 0;
    
protected:
    CSocket();
    CSocket(int a_domain, int a_type, int a_protocol);
    virtual ~CSocket();
    void SetSocketError(const char *a_errormsg);
    int                     m_fd;
    std::string             m_errstring;
};


/// CSocketDgram class
class CSocketDgram : public CSocket {
    
public:
    CSocketDgram(bool a_reuseAddress = true,
                 bool a_nonBlocking = true);
    
    ~CSocketDgram();
    
    int Send(const char *a_buffer,
             int a_len);
    
    int Send(const char *a_buffer, 
             int a_length,
             const CInternetAddress &a_destAddr);
    
    int Receive(char *a_buffer,
                int a_len);
    
    int SetSource(const CInternetAddress &a_source);
    int SetDestination(const CInternetAddress &a_destination);
    int SetMcastTTL(int a_ttl);
    int SetMcastLoopback(bool a_loopback);
    void UseMcastRecvInterfaceList();
    int AddMcastReceiveInterface(const CInternetAddress &a_address);
    void SetMcastSendInterface(const CInternetAddress &a_address);
    const CInternetAddress & GetMcastReceiveInterface(int index);
    int GetMcastReceiveInterfaceCount();
    bool IsMcastSendInterfaceSet();
    const CInternetAddress &  GetMcastSendInterface();
    
    CInternetAddress GetPeerAddress() const {return m_peerAddr;};
    bool IsMulticast();

private:
    int                     m_flags;
    bool                    m_isBound;
    CInternetAddress        m_destAddr;
    CInternetAddress        m_peerAddr;
    CInternetAddress        m_sourceAddr;
    CInternetAddress        m_mcastInterfaceAddr[8];
    CInternetAddress        m_mcastSendInterfaceAddr;
    bool                    m_useDefaultInterface;
    bool                    m_useDefaultSendInterface;
    int                     m_mcastInterfaceCount;

    int JoinMulticastAtInterface(const CInternetAddress &a_interface);
    int LeaveMulticastAtInterface(const CInternetAddress &a_interface);
    int ApplyMulticastSendInterface(const CInternetAddress &a_interface);


    CSocketDgram(const CSocketDgram& rhs);
    CSocketDgram& operator=(const CSocketDgram& rhs);
    

};



#endif /* __H_SOCKET__ */
