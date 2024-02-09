
#include "strmsrv_compatibility.h"

#include "address.hh"

using namespace std;

// CAddress class

CAddress::CAddress(AddressType a_type, int a_size) :
    m_type(a_type),
    m_size(a_size)
{

}

CAddress::~CAddress() {

}

// CInternetAddress class

CInternetAddress::CInternetAddress() :
    CAddress(InternetAddress, sizeof(sockaddr_in))
{
    SetupDefaultAddress();
}

CInternetAddress::CInternetAddress(int a_portNumber) :
    CAddress(InternetAddress, sizeof(sockaddr_in))
{
    this->SetAddress(a_portNumber);
}

CInternetAddress::CInternetAddress(const char * a_hostName, int a_portNumber) :
    CAddress(InternetAddress, sizeof(sockaddr_in))
{
    this->SetAddress(a_hostName, a_portNumber);
}

CInternetAddress::CInternetAddress(const char * a_hostName) :
    CAddress(InternetAddress, sizeof(sockaddr_in))
{
    this->SetAddress(a_hostName, 0);
}

CInternetAddress::CInternetAddress(const sockaddr_in &a_inetAddr) :
    CAddress(InternetAddress, sizeof(sockaddr_in))
{
    m_addr = a_inetAddr;
}

int
CInternetAddress::SetAddress(int a_portNumber)
{
    SetupDefaultAddress();
    if (a_portNumber < 0 || a_portNumber > 65535)
        return -1;
    unsigned short port = static_cast<unsigned short>(a_portNumber);
    m_addr.sin_port = htons(port);
    return 0;
}

int
CInternetAddress::SetAddress(const char * a_hostName, int a_portNumber)
{
    SetupDefaultAddress();
    if (a_portNumber < 0 || a_portNumber > 65535)
        return -1;

    if (a_hostName == NULL)
        return -1;

    unsigned short port = static_cast<unsigned short>(a_portNumber);
    m_addr.sin_port = htons(port);

    struct addrinfo hints, *result = NULL;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_flags |= AI_NUMERICHOST;

    if (getaddrinfo (a_hostName, 0, &hints, &result) != 0)
        return -1;

    struct addrinfo *r;
    bool valid = false;
    for (r = result; r != NULL; r= result->ai_next) {
        if (r->ai_family == AF_INET) {
            memcpy(&m_addr, r->ai_addr,
                   r->ai_addrlen);
            m_addr.sin_port = htons(port);
            valid = true;
            break;
        }
    }
    freeaddrinfo (result);
    if (valid)
        return 0;
    return -1;
}

CInternetAddress::~CInternetAddress()
{

}

void
CInternetAddress::SetupDefaultAddress()
{
    memset(&m_addr, 0x00, sizeof(sockaddr_in));
    m_addr.sin_port = 0;
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
}

const string
CInternetAddress::GetHost() const
{
    char buf[INET_ADDRSTRLEN];
#ifdef WINDOWS
    strncpy(buf, inet_ntoa(m_addr.sin_addr), INET_ADDRSTRLEN);
#else
    if (inet_ntop(AF_INET, &m_addr.sin_addr, buf, INET_ADDRSTRLEN) == NULL)
        return string("Unknown");
#endif
    return string(buf);
}

int
CInternetAddress::GetPort() const
{
    return static_cast<int> (ntohs(m_addr.sin_port));
}

bool CInternetAddress::IsMulticast() const
{
    unsigned long addr = ntohl(m_addr.sin_addr.s_addr);
    if (addr > 0xE00000FF && addr <= 0xEFFFFFFF)
        return true;
    return false;
}
