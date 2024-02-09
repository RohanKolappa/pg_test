#include "net_utils.h"
#include "v2d.h"

int doUDPRead   (int nSockfd, char *pData, int nBytes)
{   
    int status;
#ifdef WINDOWS
    status = recv (nSockfd, pData, nBytes, 0);
#else
    status = read (nSockfd, pData, nBytes);
#endif
    return status;
}
int doUDPWrite  (int nSockfd, char *pData, int nBytes,
                         struct sockaddr *pSock, int nSockLen)
{   
    int status;
#ifdef WINDOWS
    status = sendto (nSockfd, pData, nBytes, 0, pSock, nSockLen);
#else
    status = sendto (nSockfd, pData, nBytes, MSG_DONTWAIT, pSock, nSockLen);
#endif
    return status;
}
int doTCPRead   (int nSockfd, char *pData, int nBytes)
{   
    int status;
#ifdef WINDOWS
    status = recv( nSockfd, pData, nBytes, 0);
#else
    status = read (nSockfd, pData, nBytes);
#endif
    return status;
}
int doTCPWrite  (int nSockfd, char *pData, int nBytes)
{   
    int status;
#ifdef WINDOWS
    status = send( nSockfd, pData, nBytes, 0);
#else
    status = write (nSockfd, pData, nBytes);
#endif
    return status;
}
int doHTTPWrite (int nSockfd, char *pData, int nBytes)
{   
    int status;
#ifdef WINDOWS
    status = send( nSockfd, pData, nBytes,0);
#else
    status = write (nSockfd, pData, nBytes);
#endif
    return status;
}
int doHTTPRead  (int nSockfd, char *pData, int nBytes)
{   
	return recv (nSockfd, pData, nBytes, 0);
}
