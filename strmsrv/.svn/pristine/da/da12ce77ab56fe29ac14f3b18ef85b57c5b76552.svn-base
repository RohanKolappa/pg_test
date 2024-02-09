
#include "strmsrv_compatibility.h"

#include "socket.hh"
#include "common.hh"

#ifdef WINDOWS
#define SOCKOPTCAST (char *)
#else
#define SOCKOPTCAST
#endif

// CSocket class

using namespace std;

CSocket::CSocket() :
            m_fd(INVALID_SOCKET_FD)
{
}


CSocket::CSocket(int a_domain, int a_type, int a_protocol) :
            m_fd(-1)
{
    m_errstring = "No Error";
    m_fd = socket(a_domain, a_type, a_protocol);
    if (m_fd <= 0) {
        SetSocketError("Failed creating socket");
    }
}

int
CSocket:: CreateSocket(int a_domain, int a_type, int a_protocol)
{
    Close();
    m_fd = socket(a_domain, a_type, a_protocol);
    if (m_fd <= 0) {
        SetSocketError("Failed creating socket");
    }

    return m_fd;
}

CSocket::~CSocket()
{
    Close();
}

void CSocket::Close()
{
    if (m_fd > 0) {
#ifdef WINDOWS
        closesocket(m_fd);
#else
        close(m_fd);
#endif
    }
    m_fd = -1;
}

/// Setup socket non-blocking mode
int
CSocket::SetNonBlocking(bool a_nonblock)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

#ifdef WINDOWS
    DWORD dwNonBlocking = a_nonblock ? 1 : 0;
    if (ioctlsocket(m_fd, FIONBIO, (u_long FAR*)&dwNonBlocking)) {
        SetSocketError("Failed setting non-blocking mode");
        return -1;
    }
#else
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (a_nonblock)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    if (fcntl(m_fd, F_SETFL, flags) < 0) {
        SetSocketError("Failed setting non-blocking mode");
        return -1;
    }
#endif
    return 0;
}

/// Set up Reuse Address socket option
int
CSocket::SetReuseAddress(bool a_reuse)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;


    int copt = 0;
    if (a_reuse)
        copt = 1;
    int nOptLen = sizeof(copt);
    int rc = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, SOCKOPTCAST &copt, nOptLen);
    if (rc < 0) {
        SetSocketError("Failed setting reuse address socket option");
        return -1;
    }
    return 0;
}

/// Set up Reuse Address socket option
int
CSocket::SetReceiveBufferSize(int a_buffersize)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    int buffer_size = a_buffersize;
    if (buffer_size < 65535)
        buffer_size = 65535;
    int nOptLen = sizeof(buffer_size);

    int rc = setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, SOCKOPTCAST &buffer_size, nOptLen);
    if (rc < 0) {
        SetSocketError("Failed setting socket receive buffer size socket option");
        return -1;
    }

    return 0;
}

/// Set up Multicast TTL
int
CSocket::SetMcastTTL(int a_ttl)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    char ttl = (unsigned char) a_ttl;
    if (setsockopt(m_fd, IPPROTO_IP, IP_MULTICAST_TTL,
            &ttl, sizeof(ttl)) == -1) {
        SetSocketError("Failed setting Multicast TTL");
        return -1;
    }

    return 0;
}

/// Set up Multicast Loopback
int
CSocket::SetMcastLoopback(bool a_loopback)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    unsigned char loopback = 0;
    if (a_loopback)
        loopback = 1;
    int nOptLen = sizeof(loopback);
    int rc = setsockopt(m_fd, IPPROTO_IP, IP_MULTICAST_LOOP, SOCKOPTCAST &loopback, nOptLen);
    if (rc < 0) {
        SetSocketError("Failed setting Multicast Loopback");
        return -1;
    }

    return 0;
}

/// Sets up the error string for clients to retrieve the last error string
void
CSocket::SetSocketError(const char * a_errormsg)
{
    if (m_fd != INVALID_SOCKET_FD)
        Close();
    m_fd = INVALID_SOCKET_FD;
    if (a_errormsg != NULL)
        m_errstring = a_errormsg;

}



// CSocketDgram class

/// Constructor. Creates a new DGRAM (udp) socket.
CSocketDgram::CSocketDgram(bool a_reuseAddress,
        bool a_nonBlocking) :
        m_flags(0),
        m_isBound(false)
{

    // Create the socket by calling base class
    CSocket::CreateSocket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_fd == INVALID_SOCKET_FD)
        return;

    // Setup requested socket options
    CSocket::SetNonBlocking(a_nonBlocking);
    CSocket::SetReuseAddress(a_reuseAddress);

    // List of Interfaces for Multicast
    m_mcastInterfaceCount = 0;
    m_useDefaultInterface = true;
    m_useDefaultSendInterface = true;

}

/// Destructor. If multicast add membership was invoked, the drop membership
/// happens here.  Lets the parent handle the closing of the socket
CSocketDgram::~CSocketDgram()
{
    if (m_sourceAddr.IsMulticast()) {

        if (m_useDefaultInterface) {
            CInternetAddress interface_any;
            LeaveMulticastAtInterface(interface_any);
        }
        else {
            for (int i = 0; i < m_mcastInterfaceCount; i++) {
                LeaveMulticastAtInterface(m_mcastInterfaceAddr[i]);
            }
        }

    }

}


/// Set up the source (local) end-point of a UDP connection.
/// If the source is detected to be multicast, joins the
/// corresponding multicast group
int
CSocketDgram::SetSource(const CInternetAddress &a_source)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

#ifdef WINDOWS
    // On windows if Multicast, we should bind on INADDR_ANY
    // before we can join the multicast group
    CInternetAddress address;
    if (a_source.IsMulticast()) {
        address.SetAddress(a_source.GetPort());
    }
    else {
        address = a_source;
    }
    // Bind local address to the socket
    if (bind(m_fd, (struct sockaddr *) address.GetAddress(),
            address.GetSize()) == -1) {
        char buf[128];
        sprintf(buf, "Failed binding to Windows local address: %s: %d",
                a_source.GetHost().c_str(), a_source.GetPort());
        CSocket::SetSocketError(buf);
        return -1;
    }
#else
    // Bind local address to the socket
    if (bind(m_fd, (struct sockaddr *) a_source.GetAddress(), 
            a_source.GetSize()) == -1) {
        char buf[128];
        sprintf(buf, "Failed binding to local address: %s: %d",
                a_source.GetHost().c_str(), a_source.GetPort());
        CSocket::SetSocketError(buf);
        return -1;
    }
#endif

    m_isBound = true;
    m_sourceAddr = a_source;

    // Join the Multicast group if needed
    if (m_sourceAddr.IsMulticast()) {
        if (m_useDefaultInterface) {
            CInternetAddress interface_any;
            return JoinMulticastAtInterface(interface_any);
        }
        else {
            for (int i = 0; i < m_mcastInterfaceCount; i++) {
                int rc = JoinMulticastAtInterface(m_mcastInterfaceAddr[i]);
                if (rc == -1)
                    return -1;
            }

        }
    }
    return 0;
}

/// Join current Multicast Group on a given interface address
int
CSocketDgram::JoinMulticastAtInterface(const CInternetAddress &a_interface)
{
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    struct sockaddr_in addr =
            *((struct sockaddr_in *) m_sourceAddr.GetAddress());
    mreq.imr_multiaddr.s_addr = addr.sin_addr.s_addr;

    struct sockaddr_in interface_addr =
            *((struct sockaddr_in *) a_interface.GetAddress());
    mreq.imr_interface.s_addr = interface_addr.sin_addr.s_addr;

    int rc = setsockopt(m_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, SOCKOPTCAST &mreq, sizeof(mreq));
    if (rc < 0) {
        CSocket::SetSocketError("Failed joining Multicast group");
        return -1;
    }
    return 0;
}

/// Leave current Multicast Group on a given interface address
int
CSocketDgram::LeaveMulticastAtInterface(const CInternetAddress &a_interface)
{
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    struct sockaddr_in addr =
            *((struct sockaddr_in *) m_sourceAddr.GetAddress());
    mreq.imr_multiaddr.s_addr = addr.sin_addr.s_addr;

    struct sockaddr_in interface_addr =
            *((struct sockaddr_in *) a_interface.GetAddress());
    mreq.imr_interface.s_addr = interface_addr.sin_addr.s_addr;

    int rc = setsockopt(m_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, SOCKOPTCAST &mreq, sizeof(mreq));
    if (rc < 0) {
        CSocket::SetSocketError("Failed leaving Multicast group");
        return -1;
    }
    return 0;
}


/// Set up the destination (remote) end-point of a UDP connection.
/// If the destination is detected to be multicast, sets up the
/// default TTL value to be 1 and disables the multicast loopback
int
CSocketDgram::SetDestination(const CInternetAddress &a_destination)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    if (a_destination.IsNull()) {
        m_errstring = "Null Address specified for destination";
        return -1;
    }

    if (a_destination.GetPort() == 0) {
        m_errstring = "Destination port cannot be 0 (Zero)";
        return -1;
    }

    /* Set the destination for future Send() */
    m_destAddr = a_destination;

    /* bind to INADDR_ANY local address if not already done so */
    if (!m_isBound) {
        CInternetAddress addr;
        if (bind(m_fd, (struct sockaddr *) addr.GetAddress(), 
                addr.GetSize()) == -1) {
            CSocket::SetSocketError("Failed local bind to INADDR_ANY.");
            return -1;
        }
        m_isBound = true;
    }

    SetMcastTTL(1);  // default Multicast TTL
    SetMcastLoopback(0); // disable loopback by default

    if (m_destAddr.IsMulticast()) {
        if (!m_useDefaultSendInterface)
            ApplyMulticastSendInterface(m_mcastSendInterfaceAddr);
    }
    return 0;
}

/// Set the interface for sending multicast traffic
int
CSocketDgram::ApplyMulticastSendInterface(const CInternetAddress &a_interface)
{
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));

    struct sockaddr_in interface_addr =
            *((struct sockaddr_in *) a_interface.GetAddress());
    mreq.imr_multiaddr.s_addr = interface_addr.sin_addr.s_addr;

    int rc = setsockopt(m_fd, IPPROTO_IP, IP_MULTICAST_IF, SOCKOPTCAST &mreq, sizeof(mreq));
    if (rc < 0) {
        CSocket::SetSocketError("Failed setting Multicast send interface");
        return -1;
    }
    return 0;
}


/// Send data on the UDP connection. Assumes that the destination has
/// already been setup using SetDestination().
int
CSocketDgram::Send(const char *a_buffer, 
        int a_length)
{
    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    int rc = sendto(m_fd, 
            a_buffer,
            a_length,
            m_flags,
            (struct sockaddr *)m_destAddr.GetAddress(),
            m_destAddr.GetSize());

    return rc;
}

/// Send data on the UDP connection to a specific address.
/// SetDestination() needs to be called beforehand so that bind is
/// invoked and multicast parameters are setup
int
CSocketDgram::Send(const char *a_buffer, 
        int a_length,
        const CInternetAddress &a_destAddr)
{
    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    int rc = sendto(m_fd, 
            a_buffer,
            a_length,
            m_flags,
            (struct sockaddr *)a_destAddr.GetAddress(),
            a_destAddr.GetSize());

    return rc;
}

/// Get data from the UDP connection. Assumes that the source port has been
/// set up using the SetSource() call
int
CSocketDgram::Receive(char *a_buffer, 
        int a_length)
{

    if (m_fd == INVALID_SOCKET_FD)
        return -1;

    sockaddr * pAddr = static_cast <sockaddr *>(m_peerAddr.GetAddress());
    socklen_t peerSize = m_peerAddr.GetSize();
    int rc = recvfrom(m_fd, 
            a_buffer,
            a_length,
            m_flags,
            pAddr,
            &peerSize);

    return rc;
}


/// Set the Multicast TTL of the socket
int
CSocketDgram::SetMcastTTL(int a_ttl)
{
    if (a_ttl <= 0 || a_ttl > 255) {
        SetSocketError("Invalid TTL number");
        return -1;
    }

    if (m_destAddr.IsMulticast()) {
        return CSocket::SetMcastTTL(a_ttl);
    }

    return 0;
}

/// Set the Loopback mode of the socket
int
CSocketDgram::SetMcastLoopback(bool a_loopback)
{

    if (m_destAddr.IsMulticast()) {
        return CSocket::SetMcastLoopback(a_loopback);
    }

    return 0;
}

/// Indicate that a specific list of interfaces are going to be used
/// by a follow up calls to AddMcastReceiveInterface.
/// If no such follow up calls are issued, the list is assumed to be zero
/// and no multicast "joining" is going to happen
/// If this call is not issued, multicast receive happens on default interface
void
CSocketDgram::UseMcastRecvInterfaceList() {
    m_useDefaultInterface = false;
}

/// Add an interface (specified by its IP address) to the list of interface
/// that need to be "joined" for receiving multicast traffic
int
CSocketDgram::AddMcastReceiveInterface(const CInternetAddress &a_address) {

    if (m_mcastInterfaceCount < 8) {
        m_mcastInterfaceAddr[m_mcastInterfaceCount] = a_address;
        m_mcastInterfaceCount++;
    }
    return m_mcastInterfaceCount;
}

/// Return count of the interfaces this object is
/// prepared to join on
int
CSocketDgram::GetMcastReceiveInterfaceCount()
{
    return m_mcastInterfaceCount;
}

const CInternetAddress &
CSocketDgram::GetMcastReceiveInterface(int a_index)
{
    return m_mcastInterfaceAddr[a_index];
}

/// Set a specific multicast send interface
/// If this call is not issued, the socket will send on the default
/// interface.
void
CSocketDgram::SetMcastSendInterface(const CInternetAddress &a_address) {
    m_mcastSendInterfaceAddr = a_address;
    m_useDefaultSendInterface = false;
}

const CInternetAddress &
CSocketDgram::GetMcastSendInterface()
{
    return m_mcastSendInterfaceAddr;
}

bool
CSocketDgram::IsMcastSendInterfaceSet()
{
    if (m_useDefaultSendInterface)
        return false;
    return true;
}

/// Is this socket serving (sending or receiving) multicast traffic?
bool
CSocketDgram::IsMulticast() {
    if (m_sourceAddr.IsMulticast())
        return true;
    if (m_destAddr.IsMulticast())
        return true;
    return false;
}
