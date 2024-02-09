#include "v2d.h"
#include "net_utils.h"
#include "rtp.h"

#ifndef WINDOWS
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <errno.h>
#endif
#include <string.h>

#define VIDEO_SND_SOCK_BUF_LEN 65535*40
#define AUDIO_SND_SOCK_BUF_LEN 65535*2

#define OUR_PROTOCOL_VERSION_MAJOR 1
#define OUR_PROTOCOL_VERSION_MINOR 2

#define V2DSENDERROR(s) do {\
   LOG_SYS(V2DLIB_ERROR, "V2DTX: Error code %d:%d set in %s at line %d.\n", \
     ((s)->m_nErrCode & 0xffff0000)>>16, (s)->m_nErrCode & 0xffff, __FILE__, __LINE__); \
   (s)->m_bSendError = TRUE; \
   if ((s)->m_errorCallback) (s)->m_errorCallback((s)->m_errorCallbackObject); \
} while (0)

#define V2DSENDNOTIFICATION(s) do {\
   (s)->m_bSendNotify = TRUE; \
   if ((s)->m_notificationCallback) \
      (s)->m_notificationCallback((s)->m_notificationCallbackObject); \
} while (0)

static struct list_head AvListHead = { &AvListHead, &AvListHead };

static int unique_id = 1001;

/* Forward declaration */
static V2DTx_t *Tx_GetControllingTx(V2DAv_t *aventryp);
static void Tx_PreemptConnection(V2DTx_t *s);
static void Tx_KickConnection(V2DTx_t *s);

/**
 * Checks all outgoing messages and returns false if the current
 * negotiated protocol version should not allow this message to be
 * transmitted to the client. This is needed to ensure backwards
 * compatibility.
 */
static inline int Tx_CheckProtocolVersion(unsigned long version, int protid)
{

    switch (protid) {
    case IDENT_SERVER_RFBSTATUS:
        if ((V2D_PROTOCOL_VERSION_MAJOR(version) <= 1) && (V2D_PROTOCOL_VERSION_MINOR(version)
                < 1)) {
            return 0;
        }
        break;
    default:
        break;
    }

    return 1;

}

int Tx_IsHTTPMode(V2DTx_t *s)
{
    if (s->m_nHTTPVideoSock > 0)
        return 1;
    return 0;
}

static inline void Tx_GetTimeOfDay(V2DTx_t *s, struct timeval *timevalp) {
    timevalp->tv_sec = s->m_current_time/1000;
    timevalp->tv_usec = (s->m_current_time % 1000) * 1000;
}

/**
 * Tx_Reset - Resets the TX back to the SERVER_STATE_IDLE state
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns nothing.
 */
static void Tx_Reset(V2DTx_t *s)
{
    struct list_head *tmp;
    V2DAv_t *t;
    V2DTx_t *txentryp;
    unsigned char *cp;

    // Clean up control channel structures for the Tx
    s->m_nSvrState = SERVER_STATE_IDLE;
    if (s->m_nCtlSock)
        v2dLibCloseSocket(s->m_nCtlSock);
    s->m_nCtlSock = 0;
    s->m_nTxState = CLIENT_STATE_DISABLED;
    s->m_nFlag = 0;
    s->m_bSendNotify = 0;
    s->m_nNumCreq = 0;
    s->m_CtlRcvLength = 0;
    s->m_nCtlFlags = 0;
    s->m_nResetCount++;
    s->m_version = V2D_PROTOCOL_VERSION(1,0);
    s->m_nKBMSessionId = v2dLibGetRandomNumber();
    s->m_bSendRTP = FALSE;
    s->m_nSimultaneousConnectionsRefused = 0;

    // Fill default server capabilities
    s->m_bFirstCtrl = TRUE;
    cp = s->m_CtlSnd;
    memset(cp, 0x00, SEND_BUFFER_SIZE);
    SET_PROTOCOL_UNIT_LENGTH(cp, 4);
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_CAP); 
    cp += 4;
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_PROTOCOL_VERSION, s->m_requestVersion, cp);
    cp = s->m_CtlSnd;
    SET_PROTOCOL_PACKET_LENGTH(cp, 10);

    if (s->matched_policy_entry != NULL) {
        free(s->matched_policy_entry);
        s->matched_policy_entry = NULL;
    }

    if (s->m_nHTTPVideoSock)
        v2dLibCloseSocket(s->m_nHTTPVideoSock);
    if (s->m_nHTTPAudioSock)
        v2dLibCloseSocket(s->m_nHTTPAudioSock);
    s->m_nHTTPVideoSock = s->m_nHTTPAudioSock = 0;

    if (s->m_AV != NULL) {

        // Clean up AV channel structures for the Tx
        t = s->m_AV;

        // Find and remove ourself form this V2DAv_t's list of V2DTx_t
        // entries
        list_for_each(tmp, &(t->m_TxHead)) {
            txentryp = list_entry(tmp, V2DTx_t, m_TxList);
            if (s == txentryp) {
                list_del(tmp);
            }
        }

        // If the list of V2DTx_t is now empty, we were the last one
        // so we can delete the structure
        if (list_empty(&(t->m_TxHead))) {
            if (t->m_nVideoSock)
                v2dLibCloseSocket(t->m_nVideoSock);
            if (t->m_nAudioSock)
                v2dLibCloseSocket(t->m_nAudioSock);
            t->m_nVideoSock = t->m_nAudioSock = 0;

            // Remove this V2DAv_t entry from the global list of
            // V2DAv_t's entries (stored in AvListHead)
            list_del(&(t->m_AvList));
            free(t);
            s->m_AV = NULL;
        }
        else {
            // The list was not empty, so some other V2DTx_t structure
            // still holds a pointer to this V2DAv_t structure
            // Promote the next connection in the list to be controlling
            if (s->m_bControlling) {
                txentryp = list_entry(t->m_TxHead.next, V2DTx_t, m_TxList);
                txentryp->m_bControlling = 1;
            }
            s->m_AV = NULL;
        }
    }
    s->m_bControlling = 0;

}

void Tx_SetScapCallback(V2DTx_t *s, void(*scapCallback)(void *object,
        ScapData_t *scap), void *object)
{
    s->m_scapCallback = scapCallback;
    s->m_scapCallbackObject = object;
}

void Tx_SetListenerCallback(V2DTx_t *s, void(*listenerCallback)(void *object,
        int fd), void *object)
{
    s->m_listenerCallback = listenerCallback;
    s->m_listenerCallbackObject = object;
}

void Tx_SetErrorCallback(V2DTx_t *s, void(*errorCallback)(void *object),
        void *object)
{
    s->m_errorCallback = errorCallback;
    s->m_errorCallbackObject = object;
}

void Tx_SetNotificationCallback(V2DTx_t *s, void(*notificationCallback)(
        void *object), void *object)
{
    s->m_notificationCallback = notificationCallback;
    s->m_notificationCallbackObject = object;
}

/**
 * Tx_GetCreqType - Returns CREQ_TYPE_UNICAST, CREQ_TYPE_MULTI_INIT
 * or CREQ_TYPE_MULTI_JOIN
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns type of connection request (unicast or multicast)
 */
int Tx_GetCreqType(V2DTx_t *s)
{
    struct list_head *tmp;
    V2DAv_t *aventryp;
    int nMulti = 0;
    int retval = CREQ_TYPE_UNICAST;
    int m_Iter;

    for (m_Iter = 0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_MULTICAST)
            nMulti = atoi(s->m_Creq[m_Iter].value);
    }
    if (!nMulti)
        goto EXIT_LABEL;

    retval = CREQ_TYPE_MULTI_INIT;

    if (!list_empty(&AvListHead)) {
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (aventryp->m_VideoDestination.sin_addr.s_addr == inet_addr(
                    (char *) s->m_McastAddress)) {
                retval = CREQ_TYPE_MULTI_JOIN;
                break;
            }
        }
    }

  EXIT_LABEL:
    return retval;
}

/* Find if there is already a unicast being served at our port, and
 * return a pointer to that connection entry */
static V2DTx_t * Tx_IsServingUnicast(V2DTx_t *s)
{
    struct list_head *tmp;
    struct list_head *tmpTx;
    V2DAv_t *aventryp;
    V2DTx_t *txentryp;

    /* Check if any existing connection is serving unicast at our port */
    if (!list_empty(&AvListHead)) {
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (!aventryp->m_isMulticast) {
                list_for_each(tmpTx, &(aventryp->m_TxHead)) {
                    txentryp = list_entry(tmpTx, V2DTx_t, m_TxList);
                    if (txentryp->m_nListenPort == s->m_nListenPort) {
                        return txentryp;
                    }
                }
            }
        }
    }

    return NULL;
}


/* Find if there is already a multicast being served at our port, and
 * return a pointer to that connection entry */
static V2DTx_t * Tx_IsServingMulticast(V2DTx_t *s)
{
    struct list_head *tmp;
    struct list_head *tmpTx;
    V2DAv_t *aventryp;
    V2DTx_t *txentryp;

    /* Check if any existing connection is serving multicast at our port */
    if (!list_empty(&AvListHead)) {
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (aventryp->m_isMulticast) {
                list_for_each(tmpTx, &(aventryp->m_TxHead)) {
                    txentryp = list_entry(tmpTx, V2DTx_t, m_TxList);
                    if (txentryp->m_nListenPort == s->m_nListenPort) {
                        return txentryp;
                    }
                }
            }
        }
    }

    return NULL;
}

/* Find a multicast session which is the current controlling client for our port,
 * and return a pointer to that connection entry */
static V2DTx_t * Tx_GetControllingClient(V2DTx_t *s)
{
    struct list_head *tmp;
    struct list_head *tmpTx;
    V2DAv_t *aventryp;
    V2DTx_t *txentryp;

    /* Check if any existing connection is serving multicast at our port */
    if (!list_empty(&AvListHead)) {
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (aventryp->m_isMulticast) {
                list_for_each(tmpTx, &(aventryp->m_TxHead)) {
                    txentryp = list_entry(tmpTx, V2DTx_t, m_TxList);
                    if (txentryp->m_nListenPort == s->m_nListenPort &&
                            txentryp->m_bControlling == 1) {
                        return txentryp;
                    }
                }
            }
        }
    }

    return NULL;
}

int Tx_GetControllingClientIP(V2DTx_t *s, char * buffer, int buffer_len) {

    V2DTx_t *txentryp;
    txentryp = Tx_GetControllingClient(s);
    if (txentryp == NULL) {
        strncpy(buffer, "0.0.0.0",  buffer_len);
        return 0;
    }
    strncpy(buffer, inet_ntoa(txentryp->m_Destination.sin_addr), buffer_len);
    return 1;
}

static int Tx_SetNonBlockingSocket(int socketFD)
{
    int retval = 0;
#ifdef WINDOWS
    {
        DWORD dwNonBlocking = 1;
        retval = ioctlsocket(socketFD, FIONBIO, (u_long FAR*)&dwNonBlocking );
    }
#else
    retval = fcntl(socketFD, F_SETFL, O_NONBLOCK);
#endif
    return retval;
}


static int Tx_SetTCPKeepAlive(int socketFD)
{
    // Not yet enabling this mode
#ifdef USE_TCPKEEPALIVE
    int nOptVal = 1;
    int nOptLen = sizeof(nOptVal);
#ifdef WINDOWS
    if (setsockopt(socketFD, SOL_SOCKET, SO_KEEPALIVE, (char *) &nOptVal, nOptLen) < 0) {
        return -1;
    }
#else
    if (setsockopt(socketFD, SOL_SOCKET, SO_KEEPALIVE, &nOptVal, nOptLen) < 0) {
        return -1;
    }
#endif
#endif
    return 0;
}

/**
 * Tx_CreateCtlSocket - Accepts connection from RX, creates a connected socket
 * to send control messages and receive statistics from the RX
 *
 * Input:
 *   s - pointer to the V2DTx_t for which to create the socket
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_CreateCtlSocket(V2DTx_t *s)
{
    int retval = 0;
    struct sockaddr_in sPeer;
    socklen_t nPeerlen = sizeof(sPeer);
    int fd;

    V2DASSERT(s->m_nSvrSock > 0);

    fd = accept(s->m_nSvrSock, (struct sockaddr *) &sPeer, &nPeerlen);

    if (fd <= 0) {
        return getReturnCode(RC_SOCKERR);
    }

    if (s->m_nCtlSock > 0) {
        // We are already busy handling a client. If a call back
        // function is registered, see if it wants to handle the fd
        if (s->m_listenerCallback != NULL) {
            s->m_listenerCallback(s->m_listenerCallbackObject, fd);
        }
        else {
            // Callback funnction not registered so close any new
            // connections (that come in after the initial client
            // connection) ourselves
            s->m_nSimultaneousConnectionsRefused++;
            close(fd); // todo ... we should send a good reject message here?
        }
        return retval;
    }

    // Else we are handling this fd
    s->m_nCtlSock = fd;

    retval = Tx_SetNonBlockingSocket(s->m_nCtlSock);
    if (retval < 0) {
        retval = getReturnCode(RC_SOCKERR);
    }
    else {
        s->m_Destination.sin_addr.s_addr = sPeer.sin_addr.s_addr;
        s->m_Destination.sin_port = sPeer.sin_port;
        // At this point we need to dup the tx and zero out the SvrSock
    }
    //v2dLibDisableNagling(s->m_nCtlSock);

    Tx_GetTimeOfDay(s, &s->m_tLastCtlRead);

    return retval;
}
/**
 * Tx_SetCtlSocket
 * Set the control socket rather than accept on the SvrSock
 *
 * Input:
 *   s - pointer to the V2DTx_t for which to create the socket
 *
 * Returns 0 if success, non-zero error code if failure
 */
int Tx_SetCtlSocket(V2DTx_t *s, int nSock, int bIsHttp)
{
    char *pResponseString = NULL;
    int retval = 0;
    struct sockaddr_in sPeer;
    struct sockaddr_in sMine;
    socklen_t nlen = sizeof(sPeer);

    // TODO: Reject and close the new socket in non http mode too
    if(s->m_nCtlSock && bIsHttp) {
        s->m_nSimultaneousConnectionsRefused++;
        close(nSock);
        return retval;
    }

    s->m_nCtlSock = nSock;

    // Set non blocking mode for the passed in FD
    retval = Tx_SetNonBlockingSocket(s->m_nCtlSock);
    if (retval < 0) {
        retval = getReturnCode(RC_SOCKERR);
    }

    if (bIsHttp) {

        pResponseString = (char *) malloc(HTTP_RESPONSE_SIZE + 1);
        sprintf(pResponseString, HTTPResponseFmt, "OK", 200);
        doHTTPWrite(nSock, pResponseString, HTTP_RESPONSE_SIZE);
        free(pResponseString);
    }
    else {

        /* Since we are possibly cloned, m_nSvrPort may not be set. We
         need to find our listen port from our socket fd*/
        if (getsockname(s->m_nCtlSock, (struct sockaddr *) &sMine, &nlen) == -1) {
            return 0;
        }
        s->m_nListenPort = ntohs(sMine.sin_port);

    }

    retval = getpeername(s->m_nCtlSock, (struct sockaddr *) &sPeer, &nlen);
    if (retval != -1) {
        s->m_Destination.sin_addr.s_addr = sPeer.sin_addr.s_addr;
        s->m_Destination.sin_port = sPeer.sin_port;
    }
    Tx_GetTimeOfDay(s, &s->m_tLastCtlRead);

    s->m_nSvrState = SERVER_STATE_READY;
    s->m_nTxState = CLIENT_STATE_CONNECTED;
    V2DSENDNOTIFICATION(s);

    return retval;
}
/**
 * Tx_SetVideoSocket
 * Replace the UDP based Video Socket with the HTTP socket
 *
 * Input:
 *   s          - pointer to the V2DTx_t for which to create the socket
 *   HTTPSocket - HTTP connection handle
 * Returns 0 if success, non-zero error code if failure
 */
int Tx_SetVideoSocket(V2DTx_t *s, int nHTTPSock)
{
    char *pResponseString = NULL;

    // If control channel not yet setup, accept and close the http socket
    if (s->m_nCtlSock == 0) {
        close(nHTTPSock);
        return 0;
    }

    s->m_nHTTPVideoSock = nHTTPSock;

    // Set non blocking and keep alive mode for the passed in FD
    Tx_SetNonBlockingSocket(s->m_nHTTPVideoSock);
    Tx_SetTCPKeepAlive(s->m_nHTTPVideoSock);

    pResponseString = (char *) malloc(HTTP_RESPONSE_SIZE + 1);
    sprintf(pResponseString, HTTPResponseFmt, "OK", 200);
    doHTTPWrite(nHTTPSock, pResponseString, HTTP_RESPONSE_SIZE);
    free(pResponseString);

    return 0;
}
/**
 * Tx_SetAudioSocket
 * Replace the UDP based Audio Socket with the HTTP socket
 *
 * Input:
 *   s          - pointer to the V2DTx_t for which to create the socket
 *   HTTPSocket - HTTP connection handle
 * Returns 0 if success, non-zero error code if failure
 */
int Tx_SetAudioSocket(V2DTx_t *s, int nHTTPSock)
{
    char *pResponseString = NULL;

    // If control channel not yet setup, accept and close the http socket
    if (s->m_nCtlSock == 0) {
        close(nHTTPSock);
        return 0;
    }

    s->m_nHTTPAudioSock = nHTTPSock;

    // Set non blocking and keep alive mode for the passed in FD
    Tx_SetNonBlockingSocket(s->m_nHTTPAudioSock);
    Tx_SetTCPKeepAlive(s->m_nHTTPAudioSock);

    pResponseString = (char *) malloc(HTTP_RESPONSE_SIZE + 1);
    sprintf(pResponseString, HTTPResponseFmt, "OK", 200);
    doHTTPWrite(nHTTPSock, pResponseString, HTTP_RESPONSE_SIZE);
    free(pResponseString);

    return 0;
}

void Tx_SetMcastSendInterface(V2DTx_t *s, struct in_addr interface_addr)
{
    s->m_sendInterface = interface_addr;
    s->m_bUseSendInterface = 1;
}

void Tx_SetExpectedClient(V2DTx_t *s, const char * expectedClient)
{
#ifndef WINDOWS
	if (expectedClient != NULL && strlen(expectedClient) > 0) {
		if (inet_aton(expectedClient, &s->m_expectedClient) == 0) {
			// invalid address
			s->m_expectedClient.s_addr = 0;
		}
	}
#endif
}

void Tx_SetAuthToken(V2DTx_t *s, const char * authToken)
{
    if (strlen(authToken) <= 64)
        strcpy(s->m_sAuthToken, authToken);
    else
        strcpy(s->m_sAuthToken, "");
}

/// Set the interface for sending multicast traffic
static int Tx_ApplyMulticastSendInterface(V2DTx_t *s, int socket)
{
    struct ip_mreq mreq;

    if (s->m_bUseSendInterface == 0)
        return 0;

    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = s->m_sendInterface.s_addr;

    if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF, (const char*)&mreq,
        sizeof(mreq)) < 0) {
#ifndef WINDOWS
         syslog(LOG_INFO|LOG_LOCAL5, "%s: setsockopt IP_MULTICAST_IF error %d. %m. addr %x\n", __func__, errno, mreq.imr_multiaddr.s_addr);
#endif
        return -1;
    }
    return 0;
}

static int Tx_CreateSocket()
{
    struct sockaddr_in sa;

    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    // Bind local end
    sa.sin_family = AF_INET;
    sa.sin_port = 0;
    memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        return -1;
    }

    return sock;
}

static int Tx_SetSocketSendBuffer(int sock, int bufferLen)
{
    int nBufferLen = bufferLen;
    int nOptLen = sizeof(nBufferLen);

#ifdef WINDOWS
    return setsockopt(sock, SOL_SOCKET, SO_SNDBUF,
            (char*)&nBufferLen, nOptLen);
#else
    return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &nBufferLen,
            nOptLen);
#endif
}

static int Tx_SetSocketMulticastLoopback(int sock)
{
    unsigned char copt = 1;
    int nOptLen = sizeof(copt);

#ifdef WINDOWS
    return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
            (char*)&copt, nOptLen
    );
#else
    return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
            &copt, nOptLen);
#endif
}

static int Tx_SetSocketMulticastTTL(int sock, int ttl)
{
    unsigned char copt = ttl;
    int nOptLen = sizeof(copt);

#ifdef WINDOWS
    return setsockopt( sock, IPPROTO_IP, IP_MULTICAST_TTL,
            (char*)&copt, nOptLen
    );
#else
    return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &copt, nOptLen);
#endif
}

/**
 * Tx_CreateVideoSocket - Create UDP socket to send video data
 *
 * Input:
 *   s - pointer to the V2DTx_t for which to create the socket
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_CreateVideoSocket(V2DTx_t *s)
{
    int code, value;
    V2DAv_t *t = s->m_AV;
    int retval = 0, iResult = 0;
    int m_Iter;


    t->m_nVideoSock = Tx_CreateSocket();
    if (t->m_nVideoSock < 0) {
        retval = getReturnCode(RC_SOCKERR);
        goto EXIT_LABEL;
    }

    iResult = Tx_SetSocketSendBuffer(t->m_nVideoSock, VIDEO_SND_SOCK_BUF_LEN);
    if (iResult < 0) {
        retval = getReturnCode(RC_SOCKERR);
        goto EXIT_LABEL;
    }

    if (Tx_GetCreqType(s) != CREQ_TYPE_UNICAST) {

        iResult = Tx_SetSocketMulticastLoopback(t->m_nVideoSock);
        if (iResult < 0) {
            retval = getReturnCode(RC_SOCKERR);
            goto EXIT_LABEL;
        }

        iResult = Tx_SetSocketMulticastTTL(t->m_nVideoSock, s->m_nTTL);
        if (iResult < 0) {
            retval = getReturnCode(RC_SOCKERR);
            goto EXIT_LABEL;
        }

        iResult = Tx_ApplyMulticastSendInterface(s, t->m_nVideoSock);
        if (iResult < 0) {
            retval = getReturnCode(RC_SOCKERR);
            goto EXIT_LABEL;
        }

        t->m_isMulticast = 1;

    }

    t->m_VideoDestination.sin_family = AF_INET;
    t->m_VideoDestination.sin_addr.s_addr = s->m_Destination.sin_addr.s_addr;

    for (m_Iter = 0; m_Iter < s->m_nNumCreq; m_Iter++) {
        code = s->m_Creq[m_Iter].name;
        value = atoi(s->m_Creq[m_Iter].value);
        switch (code) {
        case AVREQ_IDENT_AV_MULTICAST:
            if (value)
                t->m_VideoDestination.sin_addr.s_addr =
                        inet_addr((char *) s->m_McastAddress);
            break;
        case AVREQ_IDENT_VIDEO_PORT:
            t->m_VideoDestination.sin_port = htons(value);
            break;
        }
    }

    Tx_GetTimeOfDay(s, &t->m_tLastStatistics);

  EXIT_LABEL:
    return retval;
}

/**
 * Tx_CreateAudioSocket - Create UDP socket to send audio data
 *
 * Input:
 *   s - pointer to the V2DTx_t for which to create the socket
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_CreateAudioSocket(V2DTx_t *s)
{
    int code, value;
    V2DAv_t *t = s->m_AV;
    int retval = 0, iResult = 0;
    int m_Iter;

    t->m_nAudioSock = Tx_CreateSocket();
    if (t->m_nAudioSock < 0) {
        retval = getReturnCode(RC_SOCKERR);
        goto EXIT_LABEL;
    }

    iResult = Tx_SetSocketSendBuffer(t->m_nAudioSock, AUDIO_SND_SOCK_BUF_LEN);
    if (iResult < 0) {
        retval = getReturnCode(RC_SOCKERR);
        goto EXIT_LABEL;
    }

    if (Tx_GetCreqType(s) != CREQ_TYPE_UNICAST) {

        iResult = Tx_SetSocketMulticastLoopback(t->m_nAudioSock);
        if (iResult < 0) {
            retval = getReturnCode(RC_SOCKERR);
            goto EXIT_LABEL;
        }

        iResult = Tx_SetSocketMulticastTTL(t->m_nAudioSock, s->m_nTTL);
        if (iResult < 0) {
            retval = getReturnCode(RC_SOCKERR);
            goto EXIT_LABEL;
        }

        iResult = Tx_ApplyMulticastSendInterface(s, t->m_nAudioSock);
        if (iResult < 0) {
            retval = getReturnCode(RC_SOCKERR);
            goto EXIT_LABEL;
        }
        t->m_isMulticast = 1;
    }

    t->m_AudioDestination.sin_family = AF_INET;
    t->m_AudioDestination.sin_addr.s_addr = s->m_Destination.sin_addr.s_addr;

    for (m_Iter = 0; m_Iter < s->m_nNumCreq; m_Iter++) {
        code = s->m_Creq[m_Iter].name;
        value = atoi(s->m_Creq[m_Iter].value);
        switch (code) {
        case AVREQ_IDENT_AV_MULTICAST:
            if (value)
                t->m_AudioDestination.sin_addr.s_addr = inet_addr(
                        (char *) s->m_McastAddress);
            break;
        case AVREQ_IDENT_VIDEO_PORT:
            t->m_AudioDestination.sin_port = htons(value + 1);
            break;
        }
    }
    for (m_Iter = 0; m_Iter < s->m_nNumCreq; m_Iter++) {
        code = s->m_Creq[m_Iter].name;
        value = atoi(s->m_Creq[m_Iter].value);
        if (code == AVREQ_IDENT_AUDIO_PORT)
            t->m_AudioDestination.sin_port = htons(value);
    }
    Tx_GetTimeOfDay(s, &t->m_tLastStatistics);

  EXIT_LABEL:
    return retval;
}

/**
 * Tx_SetVideoChangedFlag - Sets the value for the VIDEO_CHANGED protocol
 * unit in the buffer pointed to by pPayload to true (1).
 *
 * Input:
 *   pPayload - pointer to the server capabilities message buffer
 *
 * Returns 1 if success, 0 if there's no VIDEO_CHANGED protocol unit in
 * pPayload.
 */
static int Tx_SetVideoChangedFlag(char *pPayload)
{
    unsigned char *cp;
    int i = 4;
    int code = 0;
    int len = 0, plen = 0;
    int retval = 0;

    cp = (unsigned char *) pPayload;
    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
            break;
        }
        switch (code) {
        case SCAP_IDENT_VIDEO_CHANGED:
            FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CHANGED, 1, cp);
            retval = 1;
            break;
        default:
            break;
        }
        cp = &cp[plen];
        i += plen;
    }
    return retval;
}

/**
 * Tx_SetMcastAddress - Sets the value for the MCAST_ADDRESS protocol
 * unit in the buffer pointed to by pPayload to the string sMcastAddr,
 * or append an MCAST_ADDRESS protocol unit with value sMcastAddr if
 * there's no MCAST_ADDRESS protocol unit in pPayload.
 *
 * Also retrieve the video parameters for SRD's sake
 *
 * Input:
 *   pPayload - pointer to the server capabilities message buffer
 *   sMcastAddr - pointer to the string with the desired multicast address
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SetMcastAddress(char *pPayload, const char *sMcastAddr)
{
    unsigned char *cp, *tp;
    unsigned int i = 4;
    int code = 0;
    unsigned int len = 0, plen = 0, newlen = 0, newplen = 0;
    unsigned char tmpbuf[SEND_BUFFER_SIZE + 1];
    int set = 0;
    int retval = 0;

    len = newlen = GET_PROTOCOL_PACKET_LENGTH((unsigned char *) pPayload);
    cp = (unsigned char *) &pPayload[4];

    memcpy(tmpbuf, pPayload, len);
    tp = &tmpbuf[4];

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(tp);
        code = GET_PROTOCOL_UNIT_ID(tp);
        if (plen <= 0) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
            goto EXIT_LABEL;
        }
        switch (code) {
        case SCAP_IDENT_MCAST_ADDRESS:
            newplen = strlen(sMcastAddr);
            SET_PROTOCOL_UNIT_LENGTH(cp, newplen + 5);
            SET_PROTOCOL_UNIT_ID(cp, (unsigned short) code);
            memcpy(&cp[4], sMcastAddr, newplen);
            cp[newplen + 5] = '\0';
            cp = &cp[newplen + 5];
            newlen = len - plen + newplen + 5;
            set = 1;
            break;
        default:
            memcpy(cp, tp, plen);
            cp = &cp[plen];
            break;
        }
        tp = &tp[plen];
        i += plen;
    }
    if (!set && len < SEND_BUFFER_SIZE - plen - 5) {
        cp = (unsigned char *) &pPayload[len];
        plen = strlen(sMcastAddr);
        SET_PROTOCOL_UNIT_LENGTH(cp, plen + 5);
        SET_PROTOCOL_UNIT_ID(cp, (unsigned short) SCAP_IDENT_MCAST_ADDRESS);
        memcpy(&cp[4], sMcastAddr, plen);
        cp[plen + 5] = '\0';
        newlen = len + plen + 5;
    }

    SET_PROTOCOL_PACKET_LENGTH((unsigned char *) pPayload, newlen);

  EXIT_LABEL:
    return retval;
}

/**
 * Tx_SetAVPorts - Sets the value for the VIDEO_PORT and AUDIO_PORT
 * protocol * unit in the buffer pPayload to videoport and audioport.
 *
 * Input:
 *   pPayload - pointer to the server capabilities message buffer
 *   videoport - desired video port number
 *   audioport - desired audio port number
 *
 * Returns 0 if success, non-zero return code if error
 */

static int Tx_SetAVPorts(char *pPayload, unsigned short videoport,
        unsigned short audioport)
{
    unsigned char *cp;
    int i = 4;
    int code = 0;
    int len = 0, plen = 0;
    int retval = 0;
    int vportchanged = 0, aportchanged = 0;

    cp = (unsigned char *) pPayload;
    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;
    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
            break;
        }
        switch (code) {
        case SCAP_IDENT_VIDEO_PORT:
            LOG_SYS(V2DLIB_VERBOSE,
                    "V2DTX: Overiding Video Port %d -> %d",
                    get_protocol_unit_short(cp), videoport);
            FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_PORT, videoport, cp);
            vportchanged = 1;
            break;
        case SCAP_IDENT_AUDIO_PORT:
            LOG_SYS(V2DLIB_VERBOSE,
                    "V2DTX: Overiding Audio Port %d -> %d",
                    get_protocol_unit_short(cp), audioport);
            FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_AUDIO_PORT, audioport, cp);
            aportchanged = 1;
            break;
        default:
            break;
        }
        cp = &cp[plen];
        i += plen;
    }
    if (!vportchanged && len < SEND_BUFFER_SIZE - 6 && videoport > 0) {
        LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Overiding (new) Video Port  %d",
                videoport);
        cp = (unsigned char *) &pPayload[len];
        SET_PROTOCOL_UNIT_LENGTH(cp, 6);
        SET_PROTOCOL_UNIT_ID(cp, (unsigned short) SCAP_IDENT_VIDEO_PORT);
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_PORT, videoport, cp);
        len += 6;
    }
    if (!aportchanged && len < SEND_BUFFER_SIZE - 6 && audioport > 0) {
        LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Overiding (new) Audio Port  %d",
                audioport);
        cp = (unsigned char *) &pPayload[len];
        SET_PROTOCOL_UNIT_LENGTH(cp, 6);
        SET_PROTOCOL_UNIT_ID(cp, (unsigned short) SCAP_IDENT_AUDIO_PORT);
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_AUDIO_PORT, audioport, cp);
        len += 6;
    }
    SET_PROTOCOL_PACKET_LENGTH((unsigned char *) pPayload, len);

    return retval;
}

/**
 * Tx_SendScap - Sends the server capabilities message in the send buffer
 * to the network (with MCAST_ADDRESS set to what it should be).
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SendScap(V2DTx_t *s)
{
    int len = 0, iSent = 0;
    int retval = 0;
    unsigned short protid;
    int m_Iter;
    ScapData_t scap;

    // Make sure that our send buffer indeed has a scap message sitting in it
    protid = GET_PROTOCOL_UNIT_ID(s->m_CtlSnd);
    if (protid != IDENT_SERVER_CAP) {
        return RETCODE(RC_INTERNAL_ERR, 0);
    }

    // Change the multicast address to what the caller wants
    if (s->m_McastEnable)
        Tx_SetMcastAddress((char *) s->m_CtlSnd, s->m_McastAddress);
    else
        Tx_SetMcastAddress((char *) s->m_CtlSnd, "226.1.1.1");

    // For non-controlling clients, get the audio and video ports from
    // the controlling V2DTx_t entry, and set that for the new client
    if (s->m_AV && !s->m_bControlling) {
        V2DTx_t *txentryp = Tx_GetControllingTx(s->m_AV);
        int vport = DEFAULTVIDEOPORT;
        int aport = DEFAULTAUDIOPORT;

        if (txentryp) {
            for (m_Iter = 0; m_Iter < txentryp->m_nNumCreq; m_Iter++) {
                if (txentryp->m_Creq[m_Iter].name == AVREQ_IDENT_VIDEO_PORT)
                    vport = atoi(txentryp->m_Creq[m_Iter].value);
                if (txentryp->m_Creq[m_Iter].name == AVREQ_IDENT_AUDIO_PORT)
                    aport = atoi(txentryp->m_Creq[m_Iter].value);
            }
            Tx_SetAVPorts((char *) s->m_CtlSnd, (unsigned short) vport,
                    (unsigned short) aport);
        }
    }

    // Parse scap
    if (v2dLibParseScap(&scap, (char *) s->m_CtlSnd) != -1) {
        if (s->m_scapCallback != NULL)
            s->m_scapCallback(s->m_scapCallbackObject, &scap);

        if (scap.m_videoNameSet) {
            strcpy(s->m_videoName, scap.m_videoName);
        }
    }

    //v2dLibDumpScap((char *)s->m_CtlSnd);

    // Send the Scap buffer to the network
    len = GET_PROTOCOL_PACKET_LENGTH(s->m_CtlSnd);
    if (len != 0) {
        if (s->m_nCtlSock) {
            iSent = doTCPWrite(s->m_nCtlSock, (char *) s->m_CtlSnd, len);
            if (iSent < 0) {
                retval = getReturnCode(RC_WRITEERROR);
            }
            else {
                s->m_nCtrlSndPkts++;
                s->m_nCtrlSndBytes += iSent;
            }
        }
        Tx_GetTimeOfDay(s, &s->m_tLastCtlSend);

    }
    else
        retval = RETCODE(RC_INTERNAL_ERR, 0);

    return retval;
}

/**
 * Tx_CreateReject - Creates a server reject message
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *   error_type - error type for the reject
 *   error_code - error code for the reject
 *
 * Returns length of server reject message
 */
static int Tx_CreateReject(V2DTx_t *s, int error_type, int error_code)
{
    int len = 0;
    unsigned char *cp;
    int attrlen, attrid, attrval;

    memset(s->m_CtlSnd, 0x00, SEND_BUFFER_SIZE);
    cp = s->m_CtlSnd;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_REJ); 
    cp += 4;
    len += 4;

    attrid = error_type;
    attrval = error_code;
    attrlen = 2;
    FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
    len += attrlen + 4;

    cp = s->m_CtlSnd;
    SET_PROTOCOL_PACKET_LENGTH(cp, len); 

    return len;
}

/**
 * Tx_SendReject - Sends a server reject message
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *   error_type - error type for the reject
 *   error_code - error code for the reject
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SendReject(V2DTx_t *s, int error_type, int error_code)
{
    int len, retval = 0;

    len = Tx_CreateReject(s, error_type, error_code);

    if (doTCPWrite(s->m_nCtlSock, (char *) s->m_CtlSnd, len) < 0) {
#ifdef WINDOWS
        retval = RETCODE(RC_WRITEERROR, WSAGetLastError() );
#else
        retval = RETCODE(RC_WRITEERROR, errno);
#endif
    }
    else {
        s->m_nCtrlSndPkts++;
        s->m_nCtrlSndBytes += len;
    }

    return retval;
}

/**
 * Tx_CreateDisconnect - Creates a server disconnect message
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 always
 */
static int Tx_CreateDisconnect(V2DTx_t *s)
{
    int len = 0;
    unsigned char *cp;

    memset(s->m_CtlSnd, 0x00, SEND_BUFFER_SIZE);
    cp = s->m_CtlSnd;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_CLS); 
    len += 4;

    SET_PROTOCOL_PACKET_LENGTH(cp, len); 

    return 0;
}

/**
 * Tx_SendCachedCtlData - Sends the server capabilities that was sent before
 * every CTLSENDINTERVAL seconds.
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SendCachedCtlData(V2DTx_t *s)
{
    struct timeval tCurr;
    unsigned long delta;
    int retval = 0;

    Tx_GetTimeOfDay(s, &tCurr);
    delta = (tCurr.tv_sec - s->m_tLastCtlSend.tv_sec) * 1000 + ((tCurr.tv_usec
            - s->m_tLastCtlSend.tv_usec) / 1000);
    if (delta > CTLSENDINTERVAL + 1000) { // add 1 sec more since the tx sends
        // scap every 5 secs anyway


        retval = Tx_SendScap(s);
        if (retval == 0) {
            // Update the last Ctl sent time
            s->m_tLastCtlSend.tv_sec = tCurr.tv_sec;
            s->m_tLastCtlSend.tv_usec = tCurr.tv_usec;
        }
    }
    // not returning error since there may be something other than scap
    // sitting in the buffer
    return retval;
}

/**
 * Tx_SendCtlData - Writes the control message in the send buffer to
 * the network (with VIDEO_CHANGED set to 1 if it's the first time).
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SendCtlData(V2DTx_t *s)
{
    int len = 0;
    unsigned char *pData;
    int retval = 0;
    unsigned short protid;

    pData = s->m_CtlSnd;
    protid = GET_PROTOCOL_UNIT_ID(pData);
    len = GET_PROTOCOL_UNIT_LENGTH(pData);

    if (protid == IDENT_SERVER_CAP) {
        LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Sending Control data on request\n");

        // If this is the first SCAP from the TX then change
        // The VIDEO CHANGED FLAG to be 1
        if (s->m_bFirstCtrl) {
            // After the first control packet goes out this flag allows
            // video packets to be sent to the client
            s->m_bFirstCtrl = FALSE;

            LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Sending first scap\n");
            (void) Tx_SetVideoChangedFlag((char *) s->m_CtlSnd);
        }

        if ((retval = Tx_SendScap(s)) != 0)
            return retval;
    }
    else {
        // Not SCAP, send it as is, as long as the current negotiated
        // protocol version allows it
        if (Tx_CheckProtocolVersion(s->m_version, protid)) {
            if (doTCPWrite(s->m_nCtlSock, (char *) pData, len) < 0) {
#ifdef WINDOWS
                retval = RETCODE(RC_WRITEERROR, WSAGetLastError() );
#else
                retval = RETCODE(RC_WRITEERROR, errno);
#endif
            }
            else {
                s->m_nCtrlSndPkts++;
                s->m_nCtrlSndBytes += len;
            }
        }
        else {
            // message got filtered
            s->m_nCtlPacketsDropped++;
        }
    }

    return retval;
}

/* Generic routing to change any value in AVREQ*/

static int Tx_ChangeAVREQ(int avreqtype, char *pPayload, unsigned int avreqval,
        int avreqlen)
{
    unsigned char *cp;
    int i = 4;
    int code = 0;
    int len = 0, plen = 0;
    int retval = 0;
    int protid;
    int avreqvalchanged = 0;
    int val = 0;

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overiding avreqtype %d,avreqval %d, avreqlen %d",
            avreqtype,avreqval,avreqlen);
    if ((avreqlen != 8) && (avreqlen != 6)) {
        LOG_SYS(V2DLIB_ERROR, "V2DTX: Unknown Length for data type %d",avreqlen);
        return -1;
    }

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_AVREQ)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;
    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0)
            break;

        if (code == avreqtype) {
            val = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_INFO,
                    "V2DTX: Overiding code %d,val %d -> %d length=%d, reqllen=%d",
                    code,val,avreqval, plen, avreqlen);
            switch (avreqlen) {
            case 8:
                FILL_PROTOCOL_UNIT_LONG(avreqtype,avreqval,cp);
                break;
            case 6:
                FILL_PROTOCOL_UNIT_SHORT(avreqtype,avreqval,cp);
                break;
            default:
                break;
            }

            avreqvalchanged = 1;
            break;
        }

        cp = &cp[plen];
        i += plen;
    }

    if (!avreqvalchanged && len < SEND_BUFFER_SIZE - avreqlen) {
        LOG_SYS(V2DLIB_ALERT,
                "V2DTX: param %d not found in AVREQ, inserting it with value %d",
                avreqtype,avreqval);
        cp = (unsigned char *) &pPayload[len];
        switch (avreqlen) {
        case 8:
            FILL_PROTOCOL_UNIT_LONG(avreqtype,avreqval,cp);
            break;
        case 6:
            FILL_PROTOCOL_UNIT_SHORT(avreqtype,avreqval,cp);
            break;
        default:
            break;
        }
        len += avreqlen;
    }

    (void) val; // unused in debug mode, mask warning

    SET_PROTOCOL_PACKET_LENGTH((unsigned char *) pPayload, len);

    return retval;
}

static int Tx_AddKBMSessionId(V2DTx_t *s)
{
    unsigned char *cp;
    int i = 4;
    int code = 0;
    int len = 0, plen = 0;
    int protid;
    int found_sessionid = 0;

    cp = (unsigned char *) s->m_CtlRcv;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (!(protid == IDENT_CLIENT_KBDAT || protid == IDENT_CLIENT_MSDAT))
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;
    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0)
            break;

        if (code == KBDAT_IDENT_SESSIONID || code == MSDAT_IDENT_SESSIONID) {
            found_sessionid = 1;
            break;
        }

        cp = &cp[plen];
        i += plen;
    }

    /* Add sessionid only if not already present */
    if (!found_sessionid && len < SEND_BUFFER_SIZE - 8) {

        cp = (unsigned char *) s->m_CtlRcv + len; // goto end of current packet
        if (protid == IDENT_CLIENT_KBDAT) {
            FILL_PROTOCOL_UNIT_LONG(KBDAT_IDENT_SESSIONID, s->m_nKBMSessionId, cp);
        }
        else {
            FILL_PROTOCOL_UNIT_LONG(MSDAT_IDENT_SESSIONID, s->m_nKBMSessionId, cp);
        }
        len += 8;
    }

    cp = (unsigned char *) s->m_CtlRcv;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    return 0;
}

static int Tx_SetBandWidth(char *pPayload, unsigned int bandwidth)
{

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overiding Bandwidth, incoming bw %d", bandwidth);
    return Tx_ChangeAVREQ(AVREQ_IDENT_AV_BW, pPayload, bandwidth, 8);
}

static int Tx_SetMinCompresssion(char *pPayload, unsigned int minComp)
{

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overiding Min Compression, incoming  %d", minComp);
    return Tx_ChangeAVREQ(AVREQ_IDENT_MIN_COMPRESSION, pPayload, minComp, 8);
}

static int Tx_SetMaxCompresssion(char *pPayload, unsigned int maxComp)
{

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overiding Max Compression, incoming  %d", maxComp);
    return Tx_ChangeAVREQ(AVREQ_IDENT_MAX_COMPRESSION, pPayload, maxComp, 8);
}

static int Tx_SetBurstRatio(char *pPayload, int burstRatio)
{

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overiding Burst Ratio, incoming %d",burstRatio);
    return Tx_ChangeAVREQ(AVREQ_IDENT_BURST_RATIO, pPayload, burstRatio, 8);
}

static int Tx_SetBurstDuration(char *pPayload, unsigned int burstDuration)
{

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overiding Burst Duration, incoming %d",burstDuration);
    return Tx_ChangeAVREQ(AVREQ_IDENT_BURST_DURATION, pPayload, burstDuration,
            8);
}

static int Tx_SetFSRefresh(char *pPayload)
{

    LOG_SYS(V2DLIB_INFO, "V2DTX: Overriding FS Refresh...");
    return Tx_ChangeAVREQ(AVREQ_IDENT_FS_REFRESH, pPayload, 1, 6);
}

/**
 * Tx_SetupAVChannel - Sets up an AV channel for the TX (allocates and
 * initializes a V2DAv_t struct if the AV connection doesn't exist otherwise
 * just sets the m_AV pointer to point to the existing AV struct).
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SetupAVChannel(V2DTx_t *s)
{
    int nAVOption = AVOPTION_VIDEO;
    struct list_head *tmp;
    V2DAv_t *aventryp = NULL;
    V2DTx_t *txentryp;
    int found = 0, retval = 0;
    int m_Iter;
    int TxRequestType;

    s->m_AV = NULL;

    TxRequestType = Tx_GetCreqType(s);

    // Do policy check if we are on a V2D TX hardware
    if (s->m_bPolicyCheck) {

        s->matched_policy_entry = (struct policy_entry *) malloc(sizeof(struct policy_entry));
        if (!s->matched_policy_entry) {
            LOG_SYS(V2DLIB_ERROR,
                    "V2DTX: Out of Memory: Simulating Auth Error\n");
            retval = RETCODE(RC_AUTH_BAD_NETWORK, 0);
            Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_BAD_NETWORK);
            goto EXIT_LABEL;
        }
        else {
            memset(s->matched_policy_entry, 0, sizeof(policy_entry_t));
            if ((retval = v2dLibParsePolicy(s->m_Destination.sin_addr,
                    s->matched_policy_entry, s->m_sPolicyFileName)) < 0) {
                retval = RETCODE(RC_AUTH_BAD_NETWORK, 0);
                Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_BAD_NETWORK);
                goto EXIT_LABEL;
            }
        }

        LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Applying Policies .............\n");

        // If unicast request, send reject if it is not allowed by policy
        if (TxRequestType == CREQ_TYPE_UNICAST
                && !s->matched_policy_entry->unicast) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Unicast not allowed in Policy\n");
            retval = RETCODE(RC_AUTH_NO_UNICAST, 0);
            Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_NO_UNICAST);
            goto EXIT_LABEL;
        }

        // For any request (unicast or mulicast), if there is already
        // a unicast connection going on that is not preemptible, send
        // a reject to this new connection. If the ongoing unicast had
        // a preemtible policy, disconnect that unicast and let this
        // one go through
        if ((txentryp = Tx_IsServingUnicast(s)) != NULL
                && txentryp->matched_policy_entry != NULL) {
            if (!txentryp->matched_policy_entry->unicast_preemptible) {
                LOG_SYS(V2DLIB_WARN,
                        "V2DTX: Rejecting connection: Server busy with another connection\n");
                retval = RETCODE(RC_REQ_ALREADY_CONNECTED, 0);
                Tx_SendReject(s, SREJ_IDENT_REQERR, SREJ_ERR_ALREADY_CONNECTED);
                goto EXIT_LABEL;
            }
            else {
                /* The unicast request is preemptible */
                LOG_SYS(V2DLIB_WARN, "V2DTX: Preemting another connection");
                Tx_PreemptConnection(txentryp);
            }
        }

        // If unicast request, and there is already a multicast
        // connection going on send a reject
        if (TxRequestType == CREQ_TYPE_UNICAST && (txentryp
                = Tx_IsServingMulticast(s)) != NULL) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Unicast request but server busy with a multicast session.\n");
            retval = RETCODE(RC_REQ_ALREADY_CONNECTED, 0);
            Tx_SendReject(s, SREJ_IDENT_REQERR, SREJ_ERR_MULTICAST_ONLY);
            goto EXIT_LABEL;

        }

        // If multicast request and we are initiator, make sure that
        // it is allowed by policy
        if (TxRequestType == CREQ_TYPE_MULTI_INIT
                && !s->matched_policy_entry->multicast_init) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Multicast Init not allowed in Policy\n");
            retval = RETCODE(RC_AUTH_NO_MULTICAST_INIT, 0);
            Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_NO_MULTICAST_INIT);
            goto EXIT_LABEL;
        }

        // If multicast request and we are not initiator, make sure that
        // it is allowed by policy
        if (TxRequestType == CREQ_TYPE_MULTI_JOIN
                && !s->matched_policy_entry->multicast_part) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Multicast part/join not allowed in Policy\n");
            retval = RETCODE(RC_AUTH_NO_MULTICAST_PART, 0);
            Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_NO_MULTICAST_PART);
            goto EXIT_LABEL;
        }

        /* Allow multicast only if we are setup for multicast */
        if ((s->m_McastEnable == 0) && (TxRequestType != CREQ_TYPE_UNICAST)) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Multicast requested for non-multicast profile\n");
            retval = RETCODE(RC_REQ_UNICAST_ONLY, 0);
            Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_UNICAST_ONLY);
            goto EXIT_LABEL;
        }

        // Ok, we have not rejected the connection for any
        // reason. Apply the parameters for the matched policy. If we
        // are Unicast or the initiator for a Multicast, then we set
        // the parameters from the policy, else we just create a Full
        // Screen request
        if (TxRequestType == CREQ_TYPE_UNICAST || TxRequestType
                == CREQ_TYPE_MULTI_INIT) {

            if (s->m_nRequestBandwidth
                    > (unsigned long) s->matched_policy_entry->max_bandwidth) {
                Tx_SetBandWidth((char *) s->m_CtlRcv,
                        (unsigned int) s->matched_policy_entry->max_bandwidth);
            }

            //Setting Compression and Burst values
            Tx_SetMinCompresssion((char *) s->m_CtlRcv,
                    (unsigned int) s->matched_policy_entry->min_compression);
            Tx_SetMaxCompresssion((char *) s->m_CtlRcv,
                    (unsigned int) s->matched_policy_entry->max_compression);
            Tx_SetBurstRatio((char *) s->m_CtlRcv,
                    (unsigned int) (s->matched_policy_entry->burst_ratio
                            * BURST_RATIO_CONVERSION_FACTOR));
            Tx_SetBurstDuration((char *) s->m_CtlRcv,
                    (unsigned int) s->matched_policy_entry->burst_duration);
            Tx_SetFSRefresh((char *) s->m_CtlRcv);

            // Set the I_CTL flag so that the modified AVREQ gets
            // forwarded to the FPGA Rx
            s->m_nFlag |= I_CTL;

        }
        else {
            // We are a Multicast Joiner. Create and forward a Full
            // Screen Refresh message to the FPGA Rx
            Tx_SendFSRefresh(s);
        }

    }
    else {
        // Managed mode
        // Check for IP address or authtoken installed by middleware

    	// Reject client if client filter is configured and
    	// an unexpected client connects
		if (s->m_expectedClient.s_addr != 0) {
			if (s->m_expectedClient.s_addr != s->m_Destination.sin_addr.s_addr) {
	            LOG_SYS(V2DLIB_WARN,
	                    "V2DTX: Rejecting connection: Unexpected Client\n");
				retval = RETCODE(RC_AUTH_BAD_NETWORK, 0);
				Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_BAD_NETWORK);
				goto EXIT_LABEL;
			}
		}

		// Reject client if authentication token is configured
		// and client supplies incorrect token
		if (strlen(s->m_sAuthToken) > 0) {
            if (strcmp(s->m_sAuthToken, Tx_GetClientAuthToken(s)) != 0) {
		        LOG_SYS(V2DLIB_WARN,
		                "V2DTX: Rejecting connection: Client Token Mismatch\n");
		        retval = RETCODE(RC_AUTH_BAD_NETWORK, 0);
		        Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_BAD_NETWORK);
		        goto EXIT_LABEL;
		    }
		}

        /* Allow multicast only if we are setup for multicast */
        if ((s->m_McastEnable == 0) && (TxRequestType != CREQ_TYPE_UNICAST)) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Multicast requested for non-multicast profile\n");
            retval = RETCODE(RC_REQ_UNICAST_ONLY, 0);
            Tx_SendReject(s, SREJ_IDENT_AUTHERR, SREJ_ERR_UNICAST_ONLY);
            goto EXIT_LABEL;
        }

        // For any request (unicast or mulicast), if there is already
        // a unicast connection going on, send a reject to this new connection.
        if ((txentryp = Tx_IsServingUnicast(s)) != NULL) {
                LOG_SYS(V2DLIB_WARN,
                        "V2DTX: Rejecting connection: Server busy with another connection\n");
                retval = RETCODE(RC_REQ_ALREADY_CONNECTED, 0);
                Tx_SendReject(s, SREJ_IDENT_REQERR, SREJ_ERR_ALREADY_CONNECTED);
                goto EXIT_LABEL;
        }


        // If unicast request, and there is already a multicast
        // connection going on send a reject
        if (TxRequestType == CREQ_TYPE_UNICAST &&
                (txentryp = Tx_IsServingMulticast(s)) != NULL) {
            LOG_SYS(V2DLIB_WARN,
                    "V2DTX: Rejecting connection: Unicast request but server busy with a multicast session.\n");
            retval = RETCODE(RC_REQ_ALREADY_CONNECTED, 0);
            Tx_SendReject(s, SREJ_IDENT_REQERR, SREJ_ERR_MULTICAST_ONLY);
            goto EXIT_LABEL;

        }


    }

    /* Does client wish to join a multicast connection ?? */
    if (TxRequestType == CREQ_TYPE_MULTI_JOIN) {
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (aventryp->m_VideoDestination.sin_addr.s_addr == inet_addr(
                    (char *) s->m_McastAddress)) {
                found = 1;
                break;
            }
        }
        // Since this is a join, we should always find our entry
        V2DASSERT(found == 1);
    }

    /* Retrieve Audio/Video option for the client */
    for (m_Iter = 0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_OPTION)
            nAVOption = atoi(s->m_Creq[m_Iter].value);
    }

    if (found) {
        // We have a join request for a multicast

        int nAVOption2 = AVOPTION_VIDEO;

        // Check if the AVOption for the multicast request matches
        // what is being served already
        for (m_Iter = 0; m_Iter < aventryp->m_nNumAVdesc; m_Iter++) {
            if (aventryp->m_AVdesc[m_Iter].name == AVDESC_IDENT_AV_OPTION) {
                nAVOption2 = atoi(aventryp->m_AVdesc[m_Iter].value);
            }
        }
        if (nAVOption != nAVOption2) {
            LOG_SYS(V2DLIB_WARN, "V2DTX: Multicast Mismatch\n");
            retval = RETCODE(RC_REQ_MULTICAST_MISMATCH, 0);
            (void) Tx_SendReject(s, SREJ_IDENT_REQERR,
                    SREJ_ERR_MULTICAST_MISMATCH);
            goto EXIT_LABEL;
        }
        s->m_AV = aventryp; // shared V2DAv_t pointer
        s->m_bControlling = 0; // joined, so not controlling

        // Add this V2DTx_t entry to the controlling V2DAv_t's list
        list_add_tail(&(s->m_TxList), &(aventryp->m_TxHead));

    }
    else {
        // We have a unicast or a multicast initiator. Create a new
        // V2DAv_t object
        s->m_AV = (V2DAv_t *) malloc(sizeof(V2DAv_t));
        if (!s->m_AV) {
            LOG_SYS(V2DLIB_ERROR, "V2DTX: Out of Memory\n");
            retval = getReturnCode(RC_INTERNAL_ERR);
            goto EXIT_LABEL;
        }
        memset(s->m_AV, 0, sizeof(V2DAv_t));

        s->m_AV->m_VideoSndPtr = NULL;
        s->m_AV->m_AudioSndPtr = NULL;

        s->m_AV->m_AVdesc[0].name = AVDESC_IDENT_AV_OPTION;
        sprintf(s->m_AV->m_AVdesc[0].value, "%d", nAVOption);
        s->m_AV->m_nNumAVdesc = 1;

        if (nAVOption != AVOPTION_AUDIO) {
            if ((retval = Tx_CreateVideoSocket(s)) != 0) {
                LOG_SYS(V2DLIB_ERROR, "V2DTX: Failed CreateAudioSocket. %m\n");
                free(s->m_AV);
                s->m_AV = NULL;
                goto EXIT_LABEL;
            }
        }
        if (nAVOption != AVOPTION_VIDEO) {
            if ((retval = Tx_CreateAudioSocket(s)) != 0) {
                LOG_SYS(V2DLIB_ERROR, "V2DTX: Failed CreateAudioSocket. %m\n");
                free(s->m_AV);
                s->m_AV = NULL;
                goto EXIT_LABEL;
            }
        }

        // Add this V2DAv_t entry to the global list of  V2DAv_t's entries
        list_add_tail(&(s->m_AV->m_AvList), &AvListHead);

        // Initialize this V2DAv_t's list of V2DTx_t entries and add
        // ourselves to the list
        INIT_LIST_HEAD(&(s->m_AV->m_TxHead));
        list_add_tail(&(s->m_TxList), &(s->m_AV->m_TxHead));

        // We are the first, so we are controlling
        s->m_bControlling = 1;

        // Debug aid
        s->m_AV->m_nId = unique_id++;
    }

    LOG_SYS(V2DLIB_INFO, "V2DTX: AVChannel Setup succesful\n");
  EXIT_LABEL:
    return retval;

}

// AVListHead: global list of all V2DAv_t entries created.
// m_AVList: List item used to add V2DAv_t entry to AVListHead

// m_TxHead: A list in each V2DAv_t entry of all V2DTx_t that it is serving.
//     For unicast, there would be only one, i.e. itself
//     For multicast there would be more than one.
// m_TxList: List item used to add V2DTx_t entry to m_TxHead




/**
 * Tx_SendVideoData - Writes the data in the video buffer in the V2DAv_t struct
 * for the TX to the network
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SendVideoData(V2DTx_t *s)
{
    int status = 0;
    int nLen = 0;
    int nOffset = 8;
    char *pData;
    V2DAv_t *t = s->m_AV;
    int m_Iter;

    if (s->m_bFirstCtrl || !s->m_bControlling || t->m_VideoSndPtr == NULL) {
        t->m_VideoSndPtr = NULL;
        s->m_nFlag &= ~O_VIDEO;
        return 0;
    }

    for (m_Iter = 0; m_Iter < t->m_nNumAVdesc; m_Iter++) {
        if (t->m_AVdesc[m_Iter].name == AVDESC_IDENT_AV_OPTION) {
            if (atoi(s->m_AV->m_AVdesc[m_Iter].value) != AVOPTION_VIDEO)
                nOffset = 0;
        }
    }

    if (s->m_bSendRTP)
        nOffset = 0;

    pData = (char *) t->m_VideoSndPtr;
    nLen = t->m_nVideoLen - nOffset;

    if (!s->m_nHTTPVideoSock) {
        status = doUDPWrite(t->m_nVideoSock, &pData[nOffset], nLen,
                (struct sockaddr *) &t->m_VideoDestination,
                sizeof(t->m_VideoDestination));
    }
    else {
        status = doHTTPWrite(s->m_nHTTPVideoSock, &pData[nOffset], nLen);
    }

    if (status != nLen) {
        t->m_nVideoTxError++;
    }

    t->m_nVideoPkts++;
    t->m_nVideoBytes += nLen;

    // Clear the video sent flag
    t->m_VideoSndPtr = NULL;
    s->m_nFlag &= ~O_VIDEO;

    return 0;
}

/**
 * Tx_SendAudioData - Writes the data in the audio buffer in the V2DAv_t struct
 * for the TX to the network
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Tx_SendAudioData(V2DTx_t *s)
{
    int status;
    int nLen = 0;
    char *pData;
    V2DAv_t *t = s->m_AV;

    if (s->m_bFirstCtrl || !s->m_bControlling || t->m_AudioSndPtr == NULL) {
        t->m_AudioSndPtr = NULL;
        s->m_nFlag &= ~O_AUDIO;
        return 0;
    }

    pData = (char *) t->m_AudioSndPtr;
    nLen = t->m_nAudioLen;

    if (!s->m_nHTTPAudioSock) {
        status = doUDPWrite(t->m_nAudioSock, pData, nLen,
                (struct sockaddr *) &t->m_AudioDestination,
                sizeof(t->m_AudioDestination));
    }
    else {
        status = doHTTPWrite(s->m_nHTTPAudioSock, pData, nLen);
    }

    if (status != nLen) {
        t->m_nAudioTxError++;
    }

    t->m_nAudioPkts++;
    t->m_nAudioBytes += nLen;

    // Clear the audio sent flag
    t->m_AudioSndPtr = NULL;
    s->m_nFlag &= ~O_AUDIO;

    return 0;
}

/**
 * Tx_PopulateCreq - Populates the m_Creq array with the client AV request
 * from the receive buffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns -1 no AV request found in the receive buffer, else return 0
 */
static int Tx_PopulateCreq(V2DTx_t *s)
{
    int i = 4, pval;
    int len = 0, plen = 0;
    int code = 0;
    char val[80];
    unsigned char *cp;
    unsigned short protid;
    int retval = 0;

    cp = s->m_CtlRcv;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_AVREQ)
        return -1;
    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;
    s->m_nNumCreq = 0;

    strcpy(s->m_sClientAuthToken, "");

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
            break;
        }
        memset(val, 0, 80);

        switch (code) {
        case AVREQ_IDENT_AUTH_TOKEN:
            GET_PROTOCOL_UNIT_VALUE_STR(val, cp, plen);
            break;
        default:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            sprintf(val, "%d", pval);
            break;
        }

        switch (code) {
        case AVREQ_IDENT_VIDEO_PORT:
            LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Request for video port: %s\n", val);
            break;
        case AVREQ_IDENT_AUDIO_PORT:
            LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Request for audio port: %s\n", val);
            break;
        case AVREQ_IDENT_AV_BW:
            LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Request for Bandwidth: %s\n", val);
            s->m_nRequestBandwidth = atoi(val); // bytes per sec
            break;
        case AVREQ_IDENT_AUTH_TOKEN:
            strcpy(s->m_sClientAuthToken, val);
            break;
        case AVREQ_IDENT_PROTOCOL_VERSION:
            if (s->m_nTxState == CLIENT_STATE_EXP_DATA_REQ) {
                char buff[32];
                s->m_version = atoi(val);
                getV2DProtocolVersion(s->m_version, buff, 31);
                LOG_SYS(V2DLIB_VERBOSE,
                        "V2DTX: Negotiated Protocol Version = %s", buff);

                s->m_bSendRTP
                        = (s->m_version >= V2D_PROTOCOL_VERSION(1,2)) ? TRUE
                            : FALSE;
            }
            break;
        default:
            LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Request code %d, value %s\n",
                    code, val);
        }

        s->m_Creq[s->m_nNumCreq].name = code;
        strcpy(s->m_Creq[s->m_nNumCreq].value, val);
        if (++s->m_nNumCreq >= 10)
            break;
        cp = &cp[plen];
        i += plen;
    }

    return retval;
}

/**
 * Tx_CtlRead - Reads a control message into the RX'es send buffer from
 * the network
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns RC_NORMAL if success, RC_.. code on error, or RC_WAITING if
 * more bytes are needed to read a complete PDU packet
 */
static int Tx_CtlRead(V2DTx_t *s)
{
    int len, plen;
    unsigned char *cp;
    int readcount = 0;

    // Try to read exactly one PDU worth of bytes. On a slow
    // connection we can get fewer than expected bytes. We keep
    // track of how bytes have been read (in m_CtlRcvLength) so
    // that we can retry if needed

    // Read the first two bytes to get the length
    if (s->m_CtlRcvLength < 2) {
        len = doTCPRead(s->m_nCtlSock, (char *) s->m_CtlRcv
                + s->m_CtlRcvLength, 2 - s->m_CtlRcvLength);
        if (len <= 0) {
#ifndef WINDOWS
            if (errno == EWOULDBLOCK)
                return RC_WAITING;
#endif
            return RC_READERROR; // error on socket
        }

        /*         s->m_nCtrlRcvPkts++; */
        s->m_nCtrlRcvBytes += len;

        s->m_CtlRcvLength += len;
        readcount++;
        if (s->m_CtlRcvLength < 2)
            return RC_WAITING; // need to wait

    }

    /* we have the first two bytes, so can find the length we need to read */
    cp = s->m_CtlRcv;
    plen = GET_PROTOCOL_PACKET_LENGTH(cp);

    if ((plen < 4) || (plen > (SEND_BUFFER_SIZE - 2))) {
        return RC_PROTO_MALFORMED_PACKET; // malformed packet
    }

    Tx_GetTimeOfDay(s, &s->m_tLastCtlRead);

    /* Try to read the remaining bytes. */
    if (s->m_CtlRcvLength < plen) {
        len = doTCPRead(s->m_nCtlSock,
                (char *) s->m_CtlRcv + s->m_CtlRcvLength, plen
                        - s->m_CtlRcvLength);
        if (len <= 0 && readcount == 0) {
#ifndef WINDOWS
            if (errno == EWOULDBLOCK)
                return RC_WAITING;
#endif
            return RC_READERROR; // error on socket
        }

        s->m_nCtrlRcvPkts++;
        s->m_nCtrlRcvBytes += len;

        s->m_CtlRcvLength += len;

        if (s->m_CtlRcvLength < plen)
            return RC_WAITING; // need to wait

    }

    // we have read the full PDU, and can reset the read pointer
    s->m_CtlRcvLength = 0;

    return RC_NORMAL;
}

/**
 * Tx_CtlProcess - Processes a control message in the TX'es
 * receive buffer. Sets the I_KB, I_MS or I_SER flag
 * bits if the control message is a keyboard, mouse or serial data unit.
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero code if error
 */
static int Tx_CtlProcess(V2DTx_t *s)
{
    int retval = 0;
    unsigned short protid;
    unsigned short plen;

    switch (s->m_nTxState) {
    case CLIENT_STATE_EXP_DATA_REQ:
        if ((retval = Tx_PopulateCreq(s)) != 0 || (retval
                = Tx_SetupAVChannel(s)) != 0) {
            Tx_Reset(s);
            goto EXIT_LABEL;
        }

        s->m_nTxState = CLIENT_STATE_READY;
        V2DSENDNOTIFICATION(s);
        break;

    case CLIENT_STATE_READY:
        protid = GET_PROTOCOL_UNIT_ID(s->m_CtlRcv);
        plen = GET_PROTOCOL_UNIT_LENGTH(s->m_CtlRcv + 4);
        if ((protid == IDENT_CLIENT_KBDAT) || (protid == IDENT_CLIENT_KBABS)) {
            s->m_nFlag |= I_KB;
            s->m_nKeyboardRcvd += (plen - 4);
            /* On a V2D Tx, add session Id to the raw data forwarded to fpgarx */
            if (s->m_bIsV2DTx && (protid == IDENT_CLIENT_KBDAT)) {
                Tx_AddKBMSessionId(s);
            }
        }
        else if ((protid == IDENT_CLIENT_MSDAT) || (protid == IDENT_CLIENT_MSABS)) {
            s->m_nFlag |= I_MS;
            s->m_nMouseRcvd += (plen - 4);
            /* On a V2D Tx, add session Id to the raw data forwarded to fpgarx */
            if (s->m_bIsV2DTx && (protid == IDENT_CLIENT_MSDAT)) {
                Tx_AddKBMSessionId(s);
            }
        }
        else if (protid == IDENT_CLIENT_SERDAT) {
            s->m_nFlag |= I_SER;
            s->m_nSerialRcvd += (plen - 4);
        }
        else if (protid == IDENT_CLIENT_RFBQUERY) {
            LOG_SYS(V2DLIB_INFO, "V2DTX: Got Query message\n");
            s->m_nFlag |= I_CTL;
        }
        else if (protid == IDENT_CLIENT_AVREQ) {
            /* will get an AVREQ in connected state for FSRefresh packet */
            LOG_SYS(V2DLIB_INFO, "V2DTX: Got AVREQ (FSrefresh)\n");
            s->m_nFlag |= I_CTL;
        }
        else {
            /* most likely a CSTAT which is is not forwarded */
        }
        break;
    default:
        break;
    }

  EXIT_LABEL:
    return retval;
}

/**
 * Tx_SendFSRefresh - Builds a full-screen refreshcontrol message
 * and puts it in the TX'es receive buffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns nothing
 */
void Tx_SendFSRefresh(V2DTx_t *s)
{
    int len = 0;
    unsigned char *cp;
    int attrlen, attrid, attrval;

    memset(s->m_CtlRcv, 0x00, SEND_BUFFER_SIZE);
    cp = s->m_CtlRcv;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_AVREQ);
    cp += 4;
    len += 4;

    attrid = AVREQ_IDENT_FS_REFRESH;
    attrval = 1;
    attrlen = 2;
    FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
    len += attrlen + 4;

    cp = s->m_CtlRcv;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    s->m_nFlag |= I_CTL;
    LOG_SYS(V2DLIB_INFO, "V2DTX: Requesting IFrame");
}

/**
 * Tx_ResetConnection -  Resets the TX back to the SERVER_STATE_IDLE state
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if success, non-zero error code if failure
 */
void Tx_ResetConnection(V2DTx_t *s)
{
    if ((s->m_nTxState == CLIENT_STATE_CONNECTED) || (s->m_nTxState
            == CLIENT_STATE_READY)) {
        Tx_Reset(s);
    }
}

/**
 * Tx_BuildNetHeader - Builds a network header message into the buffer
 * pointed to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *   buffer - pointer to the buffer for the network header message
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Returns 0 if success, non-zero error code if failure
 */
int Tx_BuildNetHeader(V2DTx_t *s, unsigned short mac[], char *pBuffer, int nLen)
{
    int len = 0;
    unsigned char *cp;
    int attrlen, attrid, attrval;
    socklen_t slen;
    struct sockaddr_in sin;
    unsigned short vport, aport;
    V2DAv_t *t = s->m_AV;

    memset(pBuffer, 0x00, nLen);
    cp = (unsigned char *) pBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_NETHDR);
    cp += 4;
    len += 4;

    /* destination MAC address */
    if (t) {
        attrid = NETHDR_IDENT_DST_MAC0;
        attrval = mac[0];
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = NETHDR_IDENT_DST_MAC1;
        attrval = mac[1];
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = NETHDR_IDENT_DST_MAC2;
        attrval = mac[2];
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
    }

    /* source and destination video port and destination IP address */
    if (t && t->m_nVideoSock > 0) {
        attrid = NETHDR_IDENT_DST_VIDEO_ADDR;
        attrval = t->m_VideoDestination.sin_addr.s_addr;
        attrlen = 4;
        FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        slen = sizeof(sin);
        getsockname(t->m_nVideoSock, (struct sockaddr *)&sin, &slen);
        attrid = NETHDR_IDENT_SRC_VIDEO_PORT;
        attrval = vport = htons(sin.sin_port);
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = NETHDR_IDENT_DST_VIDEO_PORT;
        attrval = htons(t->m_VideoDestination.sin_port);
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = NETHDR_IDENT_TIME_TO_LIVE;
        attrval = (t->m_isMulticast) ? s->m_nTTL : 64;
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
    }

    /* source and destination audio port and destination IP address */
    if (t && t->m_nAudioSock > 0) {
        attrid = NETHDR_IDENT_DST_AUDIO_ADDR;
        attrval = t->m_AudioDestination.sin_addr.s_addr;
        attrlen = 4;
        FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        slen = sizeof(sin);
        getsockname(t->m_nAudioSock, (struct sockaddr *)&sin, &slen);
        attrid = NETHDR_IDENT_SRC_AUDIO_PORT;
        attrval = aport = htons(sin.sin_port);
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = NETHDR_IDENT_DST_AUDIO_PORT;
        attrval = htons(t->m_AudioDestination.sin_port);
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = NETHDR_IDENT_TIME_TO_LIVE;
        attrval = (t->m_isMulticast) ? s->m_nTTL : 64;
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
    }

    cp = (unsigned char *) pBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    LOG_SYS(V2DLIB_VERBOSE, "V2DTX: Sending Network Header");

    return 0;
}

/**
 * Tx_BuildCStat - Builds a server statistics control message every second
 * and puts it in the TX'es send buffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns 0 if not yet time to build stat, 1 otherwise
 */
static int Tx_BuildCStat(V2DTx_t *s)
{
    struct timeval tNow;
    unsigned long delta;
    V2DAv_t *t = s->m_AV;

    if (t == NULL)
        return 0;

    Tx_GetTimeOfDay(s, &tNow);
    delta = ((tNow.tv_sec - t->m_tLastStatistics.tv_sec) * 1000)
            + ((tNow.tv_usec - t->m_tLastStatistics.tv_usec) / 1000);
    if (delta < 1000)
        return 0;

    t->m_tLastStatistics.tv_sec = tNow.tv_sec;
    t->m_tLastStatistics.tv_usec = tNow.tv_usec;

    t->m_nCurrVideoBytes = t->m_nVideoBytes - t->m_nVideoLastBytes;
    t->m_nCurrVideoPkts = t->m_nVideoPkts - t->m_nVideoLastPkts;
    t->m_nCurrVideoLoss = t->m_nVideoLoss - t->m_nVideoLastLoss;
    t->m_nCurrVideoOutOfOrder = t->m_nVideoOutOfOrder
            - t->m_nVideoLastOutOfOrder;
    t->m_nCurrVideoRate = (int) (((double) t->m_nCurrVideoBytes * 1000.0 * 8.0
            / delta));

    t->m_nVideoLastPkts = t->m_nVideoPkts;
    t->m_nVideoLastBytes = t->m_nVideoBytes;
    t->m_nVideoLastLoss = t->m_nVideoLoss;
    t->m_nVideoLastOutOfOrder = t->m_nVideoOutOfOrder;

    t->m_nCurrAudioBytes = t->m_nAudioBytes - t->m_nAudioLastBytes;
    t->m_nCurrAudioPkts = t->m_nAudioPkts - t->m_nAudioLastPkts;
    t->m_nCurrAudioLoss = t->m_nAudioLoss - t->m_nAudioLastLoss;
    t->m_nCurrAudioOutOfOrder = t->m_nAudioOutOfOrder
            - t->m_nAudioLastOutOfOrder;
    t->m_nCurrAudioRate = (int) (((double) t->m_nCurrAudioBytes * 1000.0 * 8.0
            / delta));

    t->m_nAudioLastPkts = t->m_nAudioPkts;
    t->m_nAudioLastBytes = t->m_nAudioBytes;
    t->m_nAudioLastLoss = t->m_nAudioLoss;
    t->m_nAudioLastOutOfOrder = t->m_nAudioOutOfOrder;

    return 1;
}

/* Check pending tasks that need some control data to be sent out.
 * If any task is found, create appropriate data and clear the flag
 */
static int Tx_CheckCtlTasks(V2DTx_t *s)
{

    if (s->m_nCtlFlags & TX_SEND_DISCONNECT) {
        LOG_SYS(V2DLIB_INFO, "V2DTX: Creating Disconnect message");
        Tx_CreateDisconnect(s);
        s->m_nCtlFlags &= ~(TX_SEND_DISCONNECT);
        s->m_nCtlFlags |= TX_DISCONNECT_CLIENT;
        return 1;
    }

    if (s->m_nCtlFlags & TX_PREEMPT_CONNECTION) {
        LOG_SYS(V2DLIB_INFO, "V2DTX: Creating Preempt  message");
        Tx_CreateReject(s, SREJ_IDENT_SERVERR, SREJ_ERR_SERVER_PREMPTED);
        s->m_nCtlFlags &= ~(TX_PREEMPT_CONNECTION);
        s->m_nCtlFlags |= TX_DISCONNECT_CLIENT;
        return 1;
    }

    if (s->m_nCtlFlags & TX_KICK_CONNECTION) {
        LOG_SYS(V2DLIB_INFO, "V2DTX: Creating Kick  message");
        Tx_CreateReject(s, SREJ_IDENT_SERVERR, SREJ_ERR_CONTROLLER_GONE);
        s->m_nCtlFlags &= ~(TX_KICK_CONNECTION);
        s->m_nCtlFlags |= TX_DISCONNECT_CLIENT;
        return 1;
    }

    return 0;
}

static void Tx_SendDisconnect(V2DTx_t *s)
{
    // Schedule a disconnect to be sent at the next opportunity
    s->m_nCtlFlags |= TX_SEND_DISCONNECT;
}

static void Tx_PreemptConnection(V2DTx_t *s)
{
    // Schedule a preempt at the next opportunity
    s->m_nCtlFlags |= TX_PREEMPT_CONNECTION;
}

static void Tx_KickConnection(V2DTx_t *s)
{
    // Kick out conncted client at the next opportunity
    s->m_nCtlFlags |= TX_KICK_CONNECTION;
}

/* Send a disconnect message to all connected clients */
void Tx_DisconnectClients(V2DTx_t *s)
{
    struct list_head *tmp;
    struct list_head *tmpTx;
    V2DAv_t *aventryp;
    V2DTx_t *txentryp;

    // Search for connections with our multicast address in the global
    // list of V2D_Av_t's and tell them to disconnect 
    if (!list_empty(&AvListHead)) {
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (aventryp->m_isMulticast && 
                    aventryp->m_VideoDestination.sin_addr.s_addr == 
                    inet_addr((char *) s->m_McastAddress)) {
                list_for_each(tmpTx, &(aventryp->m_TxHead)) {
                    txentryp = list_entry(tmpTx, V2DTx_t, m_TxList);
                    LOG_SYS(V2DLIB_ALERT, "V2DTX: Disconnecting %s", 
                            inet_ntoa(txentryp->m_Destination.sin_addr));
                    Tx_SendDisconnect(txentryp);
                    break;
                }
            } 
        }
    }
}

/**
 * Tx_Create - TX Constructor
 *
 * Input:
 *   nSvrPort - TCP port to listen for connection
 *   nTTL - Time-to-live value if connection is multicast
 *   sMcastAddr - Address for multicast connection
 *   auth - NULL or pointer to function to authorize the connection
 *
 *  Returns a pointer to V2DTx_t struct if success, NULL if failure
 */
V2DTx_t *Tx_Create(int nSvrPort,
                   int nTTL,
                   char *sMcastAddr,
                   int isV2DTx,
                   int policyCheck,
                   char *sPolicyFileName,
                   char *sRequestVersion)
{
    int sockoptval = 1, iResult = 0;
    struct sockaddr_in sa;
    struct timeval tNow;
    unsigned char *cp;
    V2DTx_t *s = NULL;
    unsigned short request_version_value = 0;

    if (v2dLibStartup()) {
        s->m_nErrCode = getReturnCode(RC_CRITICAL_ERR);
        V2DSENDERROR(s); // OK (always returns 0 on linux)
    }

    s = (V2DTx_t *) malloc(sizeof(V2DTx_t));
    if (!s)
        goto EXIT_LABEL;

    memset((void *) s, 0, sizeof(V2DTx_t));
    s->m_nSvrState = SERVER_STATE_IDLE;
    s->m_nSvrPort = nSvrPort;
    s->m_nListenPort = 0;
    s->m_nSvrSock = 0;
    s->m_nCtlSock = 0;
    s->m_nTxState = CLIENT_STATE_DISABLED;
    s->m_bControlling = 0;
    s->m_nFlag = 0;
    s->m_bSendNotify = 0;
    s->m_nNumCreq = 0;
    s->m_CtlRcvLength = 0;
    s->m_nResetCount = 0;
    s->m_bIsV2DTx = isV2DTx;
    s->m_nCtlFlags = 0;
    s->m_bPolicyCheck = policyCheck;
    s->m_nKBMSessionId = v2dLibGetRandomNumber();
    s->m_bSendRTP = FALSE;
    s->m_nSimultaneousConnectionsRefused = 0;
    s->m_videoName[0] = 0;

    /* Determine current time and cache it further use */
     
     GETTIMEOFDAY(&tNow, NULL);
     s->m_current_time = ((uint64_t) tNow.tv_sec) * 1000 + (tNow.tv_usec/1000);

    // If not on a Tx, we do not do policycheck
    if (!s->m_bIsV2DTx)
        s->m_bPolicyCheck = 0;

    s->m_sPolicyFileName = NULL;
    if (sPolicyFileName != NULL) {
        s->m_sPolicyFileName = strdup(sPolicyFileName);
    }

    // Create socket to listen for client connections. If nSvrPort is
    // zero, we are a cloned object on a Tx: we do not create a listen
    // socket and will get a control connection dynamically
    if (s->m_nSvrPort > 0) {
        if ((s->m_nSvrSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            s->m_nErrCode = getReturnCode(RC_SOCKERR);
            V2DSENDERROR(s); // OK
            free(s);
            s = NULL;
            goto EXIT_LABEL;
        }

        // Set socket options
#ifdef WINDOWS
        iResult = setsockopt( s->m_nSvrSock, SOL_SOCKET, SO_REUSEADDR,
                (char*)&sockoptval, sizeof(sockoptval)
        );
#else
        iResult = setsockopt(s->m_nSvrSock, SOL_SOCKET, SO_REUSEADDR,
                &sockoptval, sizeof(sockoptval));
#endif

        if (iResult < 0) {
            s->m_nErrCode = getReturnCode(RC_SOCKERR);
            V2DSENDERROR(s); // OK
            v2dLibCloseSocket(s->m_nSvrSock);
            free(s);
            s = NULL;
            goto EXIT_LABEL;
        }

        // Bind to TCP port
        sa.sin_family = AF_INET;
        sa.sin_port = htons(s->m_nSvrPort);
        memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
        sa.sin_addr.s_addr = INADDR_ANY;
        if (bind(s->m_nSvrSock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            s->m_nErrCode = getReturnCode(RC_SOCKERR);
            V2DSENDERROR(s); // OK
            v2dLibCloseSocket(s->m_nSvrSock);
            free(s);
            s = NULL;
            goto EXIT_LABEL;
        }
        listen(s->m_nSvrSock, 64);
        s->m_nListenPort = s->m_nSvrPort;
    }

    // Allocate memory for control buffers
    s->m_CtlSnd = (unsigned char *) malloc(SEND_BUFFER_SIZE);
    s->m_CtlRcv = (unsigned char *) malloc(SEND_BUFFER_SIZE);

    if (!s->m_CtlSnd || !s->m_CtlRcv) {
        s->m_nErrCode = getReturnCode(RC_INTERNAL_ERR);
        V2DSENDERROR(s); // Not OK (need to free s)
        goto EXIT_LABEL;
    }

    // Fill default server capabilities
    s->m_bFirstCtrl = TRUE;
    cp = s->m_CtlSnd;
    memset(cp, 0x00, SEND_BUFFER_SIZE);
    SET_PROTOCOL_UNIT_LENGTH(cp, 4); 
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_CAP); 
    cp += 4;

    // Setup protocol version that we will be requesting
    s->m_version = V2D_PROTOCOL_VERSION(1,0);
    s->m_requestVersion = V2D_PROTOCOL_VERSION(OUR_PROTOCOL_VERSION_MAJOR, OUR_PROTOCOL_VERSION_MINOR);
    if (sRequestVersion != NULL) {
        request_version_value = getV2DProtocolVersionValue(sRequestVersion);
        if (request_version_value > 0 && request_version_value <= s->m_requestVersion)
            s->m_requestVersion = request_version_value;
    }
    FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_PROTOCOL_VERSION, s->m_requestVersion, cp);
    cp = s->m_CtlSnd;
    SET_PROTOCOL_PACKET_LENGTH(cp, 10);

    // Set multicast TTL and address
    if (nTTL <= 0)
        s->m_nTTL = DEFAULTMCASTTTL;
    else
        s->m_nTTL = nTTL;

    strncpy((char *) s->m_McastAddress, (char *) sMcastAddr, 32);
    if (strlen(sMcastAddr) > 0) {

        V2DAv_t *aventryp;
        V2DTx_t *txentryp = NULL;
        struct list_head *tmp;
        int m_Iter;
        int vport = 0, aport = 0;

        s->m_McastEnable = 1;

        /* This session supports multicast. We could potentially end
         * up being a multicast "joiner". If we find an existing
         * connection using this multicast address, send the
         * video/audio ports therein to the client so that it can
         * correctly override the ports */
        list_for_each(tmp, &AvListHead) {
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            if (aventryp->m_VideoDestination.sin_addr.s_addr == inet_addr(
                    (char *) s->m_McastAddress)) {
                txentryp = Tx_GetControllingTx(aventryp);
                break;
            }
        }
        if (txentryp != NULL) {
            for (m_Iter = 0; m_Iter < txentryp->m_nNumCreq; m_Iter++) {
                if (txentryp->m_Creq[m_Iter].name == AVREQ_IDENT_VIDEO_PORT)
                    vport = atoi(txentryp->m_Creq[m_Iter].value);
                if (txentryp->m_Creq[m_Iter].name == AVREQ_IDENT_AUDIO_PORT)
                    aport = atoi(txentryp->m_Creq[m_Iter].value);
            }
            Tx_SetAVPorts((char *) s->m_CtlSnd, (unsigned short) vport,
                    (unsigned short) aport);
        }

    }

  EXIT_LABEL:
    return s;
}

/**
 * Tx_Destroy - TX Destructor
 *
 * Input:
 *   s - pointer to V2DTx_t struct to be destroyed
 *
 * Returns nothing
 */
void Tx_Destroy(V2DTx_t *s)
{
    struct list_head *tmp;
    struct list_head *tmpTx;
    V2DTx_t *txentryp;
    V2DAv_t *aventryp;

    if (s == NULL)
        return;
    
    if (s->m_bControlling) {
        // Search for connections with our multicast address in the global
        // list of V2D_Av_t's and kick them out since we're going away 
        if (!list_empty(&AvListHead)) {
            list_for_each(tmp, &AvListHead) {
                aventryp = list_entry(tmp, V2DAv_t, m_AvList);
                if (aventryp->m_isMulticast && 
                        aventryp->m_VideoDestination.sin_addr.s_addr == 
                        inet_addr((char *) s->m_McastAddress)) {
                    list_for_each(tmpTx, &(aventryp->m_TxHead)) {
                        txentryp = list_entry(tmpTx, V2DTx_t, m_TxList);
                        Tx_KickConnection(txentryp);
                    }
                }
            } 
        }
    }
    Tx_Reset(s);
    if (s->m_CtlSnd)
        free(s->m_CtlSnd);
    if (s->m_CtlRcv)
        free(s->m_CtlRcv);
    if (s->m_nSvrSock > 0) {
        v2dLibCloseSocket(s->m_nSvrSock);
        s->m_nSvrSock = 0;
    }
    if (s->m_sPolicyFileName != NULL)
        free(s->m_sPolicyFileName);

    free(s);

    v2dLibCleanup();
}

#define CLEAR_REASON_INIT         1
#define CLEAR_REASON_NOTREADY     2

static void Tx_ClearVideoPacket(V2DTx_t *s, int reason)
{
    // In HTTP mode we do not clear the last packet sent
    if (Tx_IsHTTPMode(s)) {
        if ((reason == CLEAR_REASON_NOTREADY) && (s->m_AV != NULL))
            s->m_AV->m_nVideoTxThrottled++;
    }
    else {
        if (s->m_AV) {
            s->m_AV->m_VideoSndPtr = NULL;
            if (reason == CLEAR_REASON_NOTREADY)
                s->m_AV->m_nVideoTxError++;
        }
        s->m_nFlag &= ~O_VIDEO;
    }

}

static void Tx_ClearAudioPacket(V2DTx_t *s, int reason)
{
    if (s->m_AV) {
        s->m_AV->m_AudioSndPtr = NULL;
        if (reason == CLEAR_REASON_NOTREADY)
            s->m_AV->m_nAudioTxError++;
    }
    s->m_nFlag &= ~O_AUDIO;
}

/**
 * Tx_Go - State machine for the V2D receiver. It sends and processes
 * control messages to/from the V2D receiver as required, it sends
 * data in the control, video and audio send buffers to the network
 * when the caller sets the O_CTL, O_VIDEO or O_AUDIO flag bit, and
 * sets the I_CTL, I_KB, I_MS or I_SER flag bits if there's control,
 * etc data has been read into the buffer. Tx_Go will clear the O_CTL
 * etc bits once the data has been transmitted. Similarly, the caller
 * should clear the I_CTL etc when the data has been processed. The
 * video and audio buffers are 1040 bytes long with 8 bytes of audio
 * and 8 bytes of video (or 8 bytes of video and 8 bytes of audio)
 * sequence number at the beginning followed by 1024 bytes of data.
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *
 * Returns number of read/write/connect operations performed if no
 * error or non-zero code if error.
 */
int Tx_Go(V2DTx_t *s)
{
    struct timeval timeout;
    struct timeval tNow;
    int nDescriptors, nMaxfd = -1;
    fd_set read_fdset, write_fdset;
    int retval = 0;
    int nAudioSock = 0;
    int nVideoSock = 0;

    if (!s)
        return retval;

    /* Determine current time and cache it further use */
     
     GETTIMEOFDAY(&tNow, NULL);
     s->m_current_time = ((uint64_t) tNow.tv_sec) * 1000 + (tNow.tv_usec/1000);

    /* Make sure client is alive. Disconnect the client if it does not
     send anything on the control channel for 10 seconds */
    if ((s->m_nTxState == CLIENT_STATE_CONNECTED) || (s->m_nTxState
            == CLIENT_STATE_READY)) {
        unsigned long delta;
        Tx_GetTimeOfDay(s, &tNow);
        delta = ((tNow.tv_sec - s->m_tLastCtlRead.tv_sec) * 1000)
                + ((tNow.tv_usec - s->m_tLastCtlRead.tv_usec) / 1000);
        if (delta > 10000) {
            Tx_Reset(s);
            s->m_nErrCode = RETCODE(RC_SERV_CONTROLLER_GONE, 0);
            V2DSENDERROR(s); // OK
            goto EXIT_LABEL;
        }
    }

    // Get ready for select
    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    // Always listen on a Server Socket (if configured) for new
    // connections
    if (s->m_nSvrSock > 0) {
        FD_SET ((unsigned int)s->m_nSvrSock, &read_fdset);
        if (nMaxfd <= s->m_nSvrSock)
            nMaxfd = s->m_nSvrSock + 1;
    }

    // Always check for data on control channel
    if (s->m_nCtlSock > 0) {
        FD_SET ((unsigned int)s->m_nCtlSock, &read_fdset);
        FD_SET ((unsigned int)s->m_nCtlSock, &write_fdset);
        if (nMaxfd <= s->m_nCtlSock)
            nMaxfd = s->m_nCtlSock + 1;
    }

    // Add audio/video sockets to the fdset if a client is connected
    // and ready
    if (s->m_AV && (s->m_AV->m_nVideoSock > 0)) {
        if (s->m_nHTTPVideoSock > 0)
            nVideoSock = s->m_nHTTPVideoSock;
        else
            nVideoSock = s->m_AV->m_nVideoSock;

        FD_SET((unsigned int)nVideoSock, &write_fdset);
        if (nMaxfd <= nVideoSock)
            nMaxfd = nVideoSock + 1;
    }
    else {
        if (s->m_nFlag & O_VIDEO) {
            Tx_ClearVideoPacket(s, CLEAR_REASON_INIT);
        }
    }

    if (s->m_AV && (s->m_AV->m_nAudioSock > 0)) {
        if (s->m_nHTTPAudioSock > 0)
            nAudioSock = s->m_nHTTPAudioSock;
        else
            nAudioSock = s->m_AV->m_nAudioSock;

        FD_SET((unsigned int)nAudioSock, &write_fdset);
        if (nMaxfd <= nAudioSock)
            nMaxfd = nAudioSock + 1;
    }
    else {
        if (s->m_nFlag & O_AUDIO) {
            Tx_ClearAudioPacket(s, CLEAR_REASON_INIT);
        }
    }

    // return if we have nothing to do
    if (nMaxfd < 0) {
        return 0;
    }

    // select on fds
    nDescriptors
            = select(nMaxfd + 1, &read_fdset, &write_fdset, NULL, &timeout);

    if (nDescriptors == 0) {
        // nothing is ready
        goto EXIT_LABEL;
    }

    if (nDescriptors < 0) {
        s->m_nErrCode = getReturnCode(RC_SOCKERR);
        V2DSENDERROR(s); // Not OK
        goto EXIT_LABEL;
    }

    // Check for new client connection
    if (s->m_nSvrSock > 0 && FD_ISSET(s->m_nSvrSock, &read_fdset)) {
        int rc;
        if ((rc = Tx_CreateCtlSocket(s)) != 0) {
            s->m_nErrCode = rc;
            V2DSENDERROR(s); // Not OK
        }
        else {
            if (s->m_nSvrState == SERVER_STATE_IDLE) {
                s->m_nSvrState = SERVER_STATE_READY;
                s->m_nTxState = CLIENT_STATE_CONNECTED;
                V2DSENDNOTIFICATION(s);
            }
            retval++;
        }
        goto EXIT_LABEL;
    }

    // Check for data to or from the client
    switch (s->m_nTxState) {
    int rc;
    case CLIENT_STATE_DISABLED:
        break;
    case CLIENT_STATE_EXP_DATA_REQ:
        if (FD_ISSET(s->m_nCtlSock, &read_fdset)) {
            if ((rc = Tx_CtlRead(s)) == RC_WAITING) {
                // need to wait in the same state
            }
            else if (rc != RC_NORMAL) {
                Tx_Reset(s);
                s->m_nErrCode = RETCODE(rc, 0);
                V2DSENDERROR(s); // OK
            }
            else if ((rc = Tx_CtlProcess(s)) != 0) {
                // Tx_Reset already called for us by Tx_CtlProcess
                s->m_nErrCode = rc;
                V2DSENDERROR(s); // OK
            }
            else {
                if (s->m_bPolicyCheck) {
                    // We are on a V2DTx in standalone mode.
                    // The AvReq may need to be
                    // updated and forwarded to the fpgarx. The
                    // PolicyCheck will decide whether to accept/reject
                    // the connection, and update the AvReq if
                    // needed. No need to do anything here

                }
                else {
                    // On a DMS or Tx manaaged mode, the AvReq is has been processed
                    // locally by Tx_CtlProcess. If every thing is OK,
                    // only a Full screen refresh needs to be
                    // forwarded to the v2drx relay module
                    Tx_SendFSRefresh(s);
                }
                retval++;
            }
        }
        break;
    case CLIENT_STATE_CONNECTED:
        if (FD_ISSET(s->m_nCtlSock, &write_fdset)) {
            if ((rc = Tx_SendScap(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s); // Not OK
            }
            else {
                s->m_nTxState = CLIENT_STATE_EXP_DATA_REQ;
                retval++;
            }
        }
        break;
    case CLIENT_STATE_READY:
        if (s->m_nFlag & O_AUDIO) {
            if ((FD_ISSET(nAudioSock, &write_fdset))) {
                if ((rc = Tx_SendAudioData(s)) != 0) {
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s); // OK (always returns 0)
                }
                else
                    retval++;
            }
            else {
                Tx_ClearAudioPacket(s, CLEAR_REASON_NOTREADY);
            }
        }
        if (s->m_nFlag & O_VIDEO) {
            if (FD_ISSET(nVideoSock, &write_fdset)) {
                if ((rc = Tx_SendVideoData(s)) != 0) {
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s); // OK (always returns 0)
                }
                else
                    retval++;
            }
            else {
                Tx_ClearVideoPacket(s, CLEAR_REASON_NOTREADY);
            }
        }
        if ((s->m_nFlag & (O_CTL | O_KB | O_MS | O_SER))
                && (FD_ISSET(s->m_nCtlSock, &write_fdset))) {
            s->m_nFlag &= ~(O_CTL | O_KB | O_MS | O_SER);
            if ((rc = Tx_SendCtlData(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s); // Not OK
            }
            else
                retval++;
        }
        else if (Tx_CheckCtlTasks(s) == 1) {
            if ((rc = Tx_SendCtlData(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s); // Not OK
                break;
            }
            else {
                retval++;
                if (s->m_nCtlFlags & TX_DISCONNECT_CLIENT) {
                    Tx_Reset(s);
                    s->m_nErrCode = RETCODE(RC_SERV_SERVER_DOWN, 0);
                    V2DSENDERROR(s); // OK
                    break;
                }
            }
        }
        else {
            if ((rc = Tx_SendCachedCtlData(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s); // OK since Tx_SendCachedCtlData alsways returns 0
            }
            else
                retval += Tx_BuildCStat(s);
        }

        if (FD_ISSET(s->m_nCtlSock, &read_fdset)) {
            if ((rc = Tx_CtlRead(s)) == RC_WAITING) {
                // need to wait in the same state
            }
            else if (rc != RC_NORMAL) {
                Tx_Reset(s);
                s->m_nErrCode = RETCODE(rc, 0);
                V2DSENDERROR(s); // OK
            }
            else if ((rc = Tx_CtlProcess(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s); // OK
            }
            else
                retval++;
        }
        break;
    default:
        break;
    }

  EXIT_LABEL:
    return retval;
}

/**
 * Tx_PrintStatistics - Prints a line of server statistics into the buffer
 * pointed to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *   pBuffer: pointer to the buffer for server statistics
 *   nLen: number of bytes in the buffer pointed to by pBuffer
 *
 * Return nothing
 */
void Tx_PrintStatistics(V2DTx_t *s, char *pBuffer, int nLen)
{

    if ((pBuffer != NULL) && (s != NULL)) {

        int len = snprintf(pBuffer, nLen, "State=%d,%d,%d ", s->m_nSvrState,
                s->m_bControlling, s->m_bFirstCtrl);

        if (!s->m_AV) {
            return;
        }
        else {
            V2DAv_t *t = s->m_AV;

            snprintf(pBuffer + len, nLen - len,
                    "\t(V) r:%d b:%d p:%d l:%d o:%d (A) r:%d b:%d p:%d l:%d o:%d\n"
                        "\t(K) b:%d (M) b:%d (S) b:%d (State) %d (Dest) %s",
                    t->m_nCurrVideoRate, t->m_nCurrVideoBytes, t->m_nVideoPkts,
                    t->m_nVideoLoss, t->m_nVideoOutOfOrder,
                    t->m_nCurrAudioRate, t->m_nCurrAudioBytes, t->m_nAudioPkts,
                    t->m_nAudioLoss, t->m_nAudioOutOfOrder, s->m_nKeyboardRcvd,
                    s->m_nMouseRcvd, s->m_nSerialRcvd, s->m_nTxState,
                    inet_ntoa(s->m_Destination.sin_addr));

        }
    }
}

/**
 * Tx_SendNotification - Prints a notification message into the buffer
 * pointed to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *   pBuffer - pointer to the buffer for the notification
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Returns nothing
 */
void Tx_SendNotification(V2DTx_t *s, char *pBuffer, int nLen)
{
    if (s && pBuffer && s->m_bSendNotify) {
        snprintf(pBuffer, nLen, "s:%d", s->m_nTxState);
        s->m_bSendNotify = FALSE;
    }
}

/**
 * Tx_SendError - Prints a error message into the buffer pointed
 * to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DTx_t struct
 *   pBuffer - pointer to the buffer for the notification
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Returns nothing
 */
void Tx_SendError(V2DTx_t *s, char *pBuffer, int nLen)
{
    if (s->m_bSendError) {
        snprintf(pBuffer, nLen, "error %d: code %d, errno %d", s->m_nErrCode,
                (s->m_nErrCode & 0xffff0000) >> 16, s->m_nErrCode & 0xffff);
        s->m_bSendError = FALSE;
    }
}

/**
 * Tx_GetControllingTx - Returns pointer to controlling TX for the
 * AV channel
 *
 * Input: aventryp - pointer to the V2DAv_t struct
 *
 * Returns pointer to the controlling V2DTx_t struct or NULL if there's
 * no controlling TX.
 */
static V2DTx_t *Tx_GetControllingTx(V2DAv_t *aventryp)
{
    struct list_head *tmp;
    V2DTx_t *txentryp;

    list_for_each(tmp, &(aventryp->m_TxHead)) {
        txentryp = list_entry(tmp, V2DTx_t, m_TxList);
        if (txentryp->m_bControlling)
            return txentryp;
    }
    return NULL;
}

/**
 * Tx_DumpAvList - Print the linked list of V2DAv_t structs pointed to by
 * the global AvListHead list pointer
 *
 * Input: nothing
 *
 * Returns nothing.
 */
void Tx_DumpAvList(void)
{
    struct list_head *tmp;
    V2DAv_t *aventryp;
    int i = 0;
    int m_Iter;

    if (list_empty(&AvListHead))
        printf("AvList Empty\n");
    else {
        list_for_each(tmp, &AvListHead) {
            i++;
            aventryp = list_entry(tmp, V2DAv_t, m_AvList);
            printf("AvList #%d: Destination %s, Video Port %d, AVdesc:\n", i,
                    inet_ntoa(aventryp->m_VideoDestination.sin_addr), ntohs(
                            aventryp->m_VideoDestination.sin_port));
            for (m_Iter = 0; m_Iter < aventryp->m_nNumAVdesc; m_Iter++)
                printf("%d: %s\n", aventryp->m_AVdesc[m_Iter].name,
                        aventryp->m_AVdesc[m_Iter].value);
        }
    }
}

/**
 * Tx_DumpTxList - Prints the linked list of V2DTx_t structs pointed to
 * by the m_TxList list pointer in the V2DAv_t struct
 *
 * Input:
 *   s: pointer to the V2DTx_t struct
 *
 * Returns nothing.
 */
void Tx_DumpTxList(V2DAv_t *s)
{
    struct list_head *tmp;
    V2DTx_t *txentryp;
    int i = 0;

    if (list_empty(&(s->m_TxHead)))
        printf("TxList for Video Destination %s Empty\n", inet_ntoa(
                s->m_VideoDestination.sin_addr));
    else {
        printf("TxList for Video Destination %s:\n", inet_ntoa(
                s->m_VideoDestination.sin_addr));
        list_for_each(tmp, &(s->m_TxHead)) {
            i++;
            txentryp = list_entry(tmp, V2DTx_t, m_TxList);
            printf("TxList #%d: SvrPort %d, Destination %s, Controlling %d\n",
                    i, txentryp->m_nSvrPort, inet_ntoa(
                            txentryp->m_Destination.sin_addr),
                    txentryp->m_bControlling);
        }
    }
}

/**
 * Tx_CheckForVideoLoss - Checks for video loss at the Tx source
 * by looking at the sequence numbers of the packets.
 *
 * Input:
 *   s: pointer to the V2DTx_t struct
 *   pBuffer - pointer to the packet buffer
 *   nRTPPayload - RTP or V2D payload
 * Returns nothing.
 */
int Tx_CheckForVideoLoss(V2DTx_t *s, char *pBuffer, const int nRTPPayload)
{
    V2DAv_t* pAV = s->m_AV;
    unsigned int nSeqNum = 0;

    if (pAV == NULL)
        return 0;

    if (!nRTPPayload)
        nSeqNum = V2DGETSEQUENCENUM(pBuffer);
    else
        nSeqNum = RTPGETSEQUENCENUM(pBuffer);

    if (pAV->m_nVideoSeq && (pAV->m_nVideoSeq != nSeqNum)) {
        if (pAV->m_nVideoSeq < nSeqNum) {
            pAV->m_nVideoLoss += (nSeqNum - pAV->m_nVideoSeq);
            pAV->m_nVideoSeq = nSeqNum + 1;
        }
        else if (pAV->m_nVideoSeq - nSeqNum == 65536) {
            pAV->m_nVideoSeq = nSeqNum + 1;
        }
        else {
            pAV->m_nVideoOutOfOrder++;
            if (pAV->m_nVideoOutOfOrder % 10 == 0)
                pAV->m_nVideoSeq = 0;
        }
    }
    else {
        pAV->m_nVideoSeq = nSeqNum + 1;
    }

    return 0;
}

/**
 * Tx_CheckForAudioLoss - Checks for audio loss at the Tx source
 * by looking at the sequence numbers of the packets.
 *
 * Input:
 *   s: pointer to the V2DTx_t struct
 *   pBuffer - pointer to the packet buffer
 *   nRTPPayload - RTP or V2D payload
 * Returns nothing.
 */
int Tx_CheckForAudioLoss(V2DTx_t *s, char *pBuffer, const int nRTPPayload)
{
    V2DAv_t* pAV = s->m_AV;
    unsigned int nSeqNum = 0;

    if (pAV == NULL)
        return 0;

    if (!nRTPPayload)
        nSeqNum = V2DGETSEQUENCENUM(pBuffer);
    else
        nSeqNum = RTPGETSEQUENCENUM(pBuffer);

    if (pAV->m_nAudioSeq && (pAV->m_nAudioSeq != nSeqNum)) {
        if (pAV->m_nAudioSeq < nSeqNum) {
            pAV->m_nAudioLoss += (nSeqNum - pAV->m_nAudioSeq);
            pAV->m_nAudioSeq = nSeqNum + 1;
        }
        else if (pAV->m_nAudioSeq - nSeqNum == 65536) {
            pAV->m_nAudioSeq = nSeqNum + 1;
        }
        else {
            pAV->m_nAudioOutOfOrder++;
            if (pAV->m_nAudioOutOfOrder % 10 == 0)
                pAV->m_nAudioSeq = 0;
        }
    }
    else {
        pAV->m_nAudioSeq = nSeqNum + 1;
    }

    return 0;
}

const char * Tx_GetAuthToken(V2DTx_t *s)
{
    return s->m_sAuthToken;
}

const char * Tx_GetClientAuthToken(V2DTx_t *s)
{
    return s->m_sClientAuthToken;
}
