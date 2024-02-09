#ifndef __NET_UTILS_H__
#define __NET_UTILS_H__
#include "v2dcommon.h"

int doUDPRead   (int nSockfd, char *pData, int nBytes);
int doUDPWrite  (int nSockfd, char *pData, int nBytes,
                         struct sockaddr *pSock, int nSockLen);
int doTCPRead   (int nSockfd, char *pData, int nBytes);
int doTCPWrite  (int nSockfd, char *pData, int nBytes);
int doHTTPWrite (int nSockfd, char *pData, int nBytes);
int doHTTPRead  (int nSockfd, char *pData, int nBytes);

#endif
