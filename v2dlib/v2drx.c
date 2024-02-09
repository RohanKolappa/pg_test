#include "net_utils.h"
#include "rtp.h"

#define VIDEO_RCV_SOCK_BUF_LEN 65535*40
#define AUDIO_RCV_SOCK_BUF_LEN 65535*40

#define OUR_PROTOCOL_VERSION_MAJOR 1
#define OUR_PROTOCOL_VERSION_MINOR 2

#define RECONNECT_INTERVAL 5000

#define V2DSENDERROR(s) do {\
   LOG_SYS(V2DLIB_ERROR, "V2DRX: Error code %d:%d set in %s at line %d.\n", \
     ((s)->m_nErrCode & 0xffff0000)>>16, (s)->m_nErrCode & 0xffff, __FILE__, __LINE__); \
   (s)->m_bSendError = TRUE; \
} while (0)


/**
 * Checks all outgoing messages and returns false if the current
 * negotiated protocol version should not allow this message to be
 * transmitted to the server. This is needed to ensure backwards
 * compatibility.
 */
static inline int Rx_CheckProtocolVersion(unsigned long version, int protid) {

    switch(protid) {
    case IDENT_CLIENT_MSABS:
    case IDENT_CLIENT_KBABS:
        if ((V2D_PROTOCOL_VERSION_MAJOR(version) <= 1) &&
            (V2D_PROTOCOL_VERSION_MINOR(version) < 1)) {
            return 0;
        }
        break;

    case IDENT_CLIENT_RFBQUERY:
        if ((V2D_PROTOCOL_VERSION_MAJOR(version) <= 1) &&
            (V2D_PROTOCOL_VERSION_MINOR(version) < 1)) {
            return 0;
        }
        break;
    default:
        break;
    }

    return 1;

}

static inline void Rx_GetTimeOfDay(V2DRx_t *s, struct timeval *timevalp) {
    timevalp->tv_sec = s->m_current_time/1000;
    timevalp->tv_usec = (s->m_current_time % 1000) * 1000;
}

/**
 * Rx_ClearCStat - Resets all variables used to compute client statistics
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns nothing.
 */
static void Rx_ClearCStat(V2DRx_t *s) {

    Rx_GetTimeOfDay(s, &s->m_tLastStatistics);

    s->m_nCurrVideoPkts = s->m_nVideoLastPkts = s->m_nVideoPkts = 0;
    s->m_nCurrVideoBytes = s->m_nVideoLastBytes = s->m_nVideoBytes = 0;
    s->m_nCurrAudioPkts = s->m_nAudioLastPkts = s->m_nAudioPkts = 0;
    s->m_nCurrAudioBytes = s->m_nAudioLastBytes = s->m_nAudioBytes = 0;
    s->m_nCurrVideoLoss = s->m_nVideoLastLoss = s->m_nVideoLoss = 0;
    s->m_nCurrAudioLoss = s->m_nAudioLastLoss = s->m_nAudioLoss = 0;
    s->m_nCurrVideoOutOfOrder = s->m_nVideoLastOutOfOrder =
        s->m_nVideoOutOfOrder= 0;
    s->m_nCurrAudioOutOfOrder = s->m_nAudioLastOutOfOrder =
        s->m_nAudioOutOfOrder= 0;
    s->m_nSerialRcvd = 0;
    s->m_RFBMouseSent = s->m_RFBKeyboardSent = 0;
}


/**
 * Rx_Reset - Resets the RX back to IDLE state
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns nothing.
 */
static void Rx_Reset(V2DRx_t *s)
{
    /* If we have received a disconnect request, we stay disabled so
       that we do not try to connect again */
    if (s->m_nRxState != CLIENT_STATE_DISABLED)
        s->m_nRxState = CLIENT_STATE_IDLE;

    if (s->m_nCtlSock  > 0)
        v2dLibCloseSocket( s->m_nCtlSock );
    s->m_nCtlSock = 0;
    s->m_CtlRcvLength = 0;

    if (s->m_nVideoSock > 0)
        v2dLibCloseSocket(s->m_nVideoSock);
    s->m_nVideoSock = 0;

    if (s->m_nAudioSock > 0)
        v2dLibCloseSocket(s->m_nAudioSock);
    s->m_nAudioSock = 0;

    s->m_bSendNotify=TRUE;
    s->m_version = V2D_PROTOCOL_VERSION(1,0);
    s->m_nSessionId = v2dLibGetRandomNumber();
    s->m_nQueryId = 1;
    s->m_bRFBStatus = FALSE;
    s->m_nCtlFlags |= RX_SEND_RFB_QUERY;

    (void) Rx_ClearCStat(s);

    // Note: We do not touch the m_statuscode. We want it to reflect
    // the last status, till it reconnects again

    s->m_nResetCount++;
}


/**
 * Rx_CheckCtlConnect - Checks if the control connection for the V2DRx_t
 * has failed, set the state to CONNECTED if it hasn't.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct to be checked
 *
 * Returns 0 if connection hasn't failed, non-zero error code if failed
 */
static int Rx_CheckCtlConnect(V2DRx_t *s)
{
    int retval = 0;

#ifdef WINDOWS
    fd_set write_fdset, except_fdset;
    struct timeval timeout;
    int iReady = 0;

    if (s->m_nPureRTPMode)
        return 0;

    FD_ZERO( &write_fdset );
    FD_ZERO( &except_fdset );

    timeout.tv_sec =  0;
    timeout.tv_usec = 0;

    FD_SET((unsigned int) s->m_nCtlSock, &write_fdset );
    FD_SET((unsigned int) s->m_nCtlSock, &except_fdset );

    // on windows, when connect() is called on non-blocking call
    // connection succeeded if the socket is ready for writing

    iReady = select( 0, NULL, &write_fdset, &except_fdset, &timeout );

    // connection failed
    if ( FD_ISSET( s->m_nCtlSock, &except_fdset ) ) {
        Rx_Reset( s );
        Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
        retval = RETCODE( RC_CONNECT_ERR, WSAGetLastError() );
    }

    // succesful connection
    else if ( FD_ISSET( s->m_nCtlSock, &write_fdset ) ) {
        if (s->m_bUseHTTP) {
            s->m_nRxState = CLIENT_STATE_SENDHTTPREQ;
        }
        else {
            s->m_nRxState = CLIENT_STATE_CONNECTED;
	        s->m_statuscode = STATUSCODE_AUTHENTICATING;
            s->m_bSendNotify=TRUE;
        }
    }

#else
    int sockoptval = 0;
    int sockoptlen = sizeof(sockoptval);

    if (s->m_nPureRTPMode)
        return 0;

    retval = getsockopt( s->m_nCtlSock, SOL_SOCKET, SO_ERROR,
                         &sockoptval, (socklen_t *)&sockoptlen );

    if( retval < 0 ) {

        retval = RETCODE( RC_SOCKERR, errno );
    }
    else if (sockoptval == 0) {
        if (s->m_bUseHTTP) {
            s->m_nRxState = CLIENT_STATE_SENDHTTPREQ;
        }
        else {
            s->m_nRxState = CLIENT_STATE_CONNECTED;
            s->m_statuscode = STATUSCODE_AUTHENTICATING;
            s->m_bSendNotify=TRUE;
        }
    }
    else  {

        switch (sockoptval) {
        case ENETUNREACH:
            s->m_statuscode = STATUSCODE_UNREACHABLE;
            break;
        case ECONNREFUSED:
            s->m_statuscode = STATUSCODE_REFUSED;
            break;
        case EHOSTUNREACH:
            s->m_statuscode = STATUSCODE_HOSTUNREACHABLE;
            break;
        case ETIMEDOUT:
            s->m_statuscode = STATUSCODE_TIMEDOUT;
            break;
        default:
            s->m_statuscode = STATUSCODE_UNKNOWN;
            break;
        }

        Rx_Reset(s);

        Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
        retval = RETCODE(RC_CONNECT_ERR, 0);
    }
#endif

    return retval;
}

/**
 * Rx_CtlConnect - Initiate connection to the TX on the control socket
 * (if the last connect attempt was more than 1 sec ago)
 *
 * Input:
 *   s - pointer to the V2DRx_t to initiate the connection
 *
 * Returns 1 if success, 0 if not time to try to connect yet,
 * and non-zero error code if failure
 */
static int Rx_CtlConnect(V2DRx_t *s)
{
    int retval = 0;
    unsigned long delta;

    if (s->m_nPureRTPMode)
        return 0;

    Rx_GetTimeOfDay(s, &s->m_tLastCtlConnect);
    GETTIMEDIFF(s->m_tLastCtlConnect, s->m_tLastFailedConnect, delta);

    /* Exponential backoff in connect attempts */
    if (delta < s->m_nReconnectPeriod)
        goto EXIT_LABEL;
    s->m_nReconnectPeriod = s->m_nReconnectPeriod * 2;
    if (s->m_nReconnectPeriod > RECONNECT_INTERVAL)
        s->m_nReconnectPeriod = RECONNECT_INTERVAL;

#ifdef WINDOWS
    {
        DWORD dwNonBlocking = 1;
        retval = ioctlsocket( s->m_nCtlSock, FIONBIO, (u_long FAR*)&dwNonBlocking );
    }
#else
    retval = fcntl(s->m_nCtlSock, F_SETFL, O_NONBLOCK);
#endif

    if ( retval < 0) {
        retval = getReturnCode( RC_SOCKERR );
        goto EXIT_LABEL;
    }

    retval = connect(s->m_nCtlSock, (struct sockaddr *) &s->m_Destination,
                     sizeof(s->m_Destination));

    if ( retval )
    {
#ifdef WINDOWS
        DWORD dwErr = WSAGetLastError();

        // non blocking socket should return one of these errors
        // typically it would return WSAEWOULDBLOCK on first connect()
        // and on subsequent connect() it would return either
        // WSAEALREADY or WSAEISCONN.
        // We will call connect() just once and then call select()
        // on control socket to check connection status.
        if (  dwErr != WSAEWOULDBLOCK
              && dwErr != WSAEINPROGRESS
              && dwErr != WSAEALREADY
              && dwErr != WSAEISCONN
            )
        {
            retval = RETCODE( RC_CONNECT_ERR, dwErr );
        }
#else
        /* Non blocking so could be in progress */
        if (errno != EINPROGRESS)
        {
            retval=RETCODE(RC_CONNECT_ERR, errno);
        }
#endif
        else
        {
            retval = 0;
            s->m_nRxState = CLIENT_STATE_CONNECTING;
	        s->m_statuscode = STATUSCODE_CONNECTING;
            s->m_bSendNotify = TRUE;
            Rx_GetTimeOfDay(s, &s->m_tLastCtlRead);
        }
    }


 EXIT_LABEL:
    return retval;
}

int Rx_AddMcastReceiveInterface(V2DRx_t *s, struct in_addr interface_addr)
{
    if (s->m_recvInterfaceCount == -1 ||
            s->m_recvInterfaceCount >= MAX_RECV_INTERFACES)
        return -1;

    s->m_recvInterfaces[s->m_recvInterfaceCount] = interface_addr;
    s->m_recvInterfaceCount++;

    return 0;
}

void Rx_UseMcastRecvInterfaceList(V2DRx_t *s)
{
    s->m_recvInterfaceCount = 0;
}

static int Rx_JoinMulticastAtInterfaces(V2DRx_t *s, int socket,
        struct ip_mreq *mreqptr)
{
    int result = 0;
    if (s->m_recvInterfaceCount == -1) {
        // Join on a single default port (legacy behaviour)

        mreqptr->imr_interface.s_addr = INADDR_ANY;

        result = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) mreqptr,
                sizeof(struct ip_mreq));
    }
    else {
        // Join on the configured list of interfaces
        int i;
        for (i = 0; i < s->m_recvInterfaceCount; i++) {
            mreqptr->imr_interface.s_addr = s->m_recvInterfaces[i].s_addr;

            result = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) mreqptr,
                    sizeof(struct ip_mreq));
            if (result < 0)
                break;
        }

    }
    return result;
}

static int Rx_SetupMulticast(V2DRx_t *s, int sock, struct sockaddr_in * sa)
{
    int retval = 0;
    int copt = 0;
    int nOptLen = 1;
    int iResult = 0;
    int foundMcastAddr = 0;
    struct ip_mreq mreq;
    int m_Iter;

    // Set multicast for no loopback
#ifdef WINDOWS
    iResult = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
                          (char*)&copt, nOptLen);
#else
    iResult = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
            &copt, nOptLen);
#endif

    if (iResult < 0) {
        retval = getReturnCode(RC_SOCKERR);
        s->m_statuscode = STATUSCODE_SOCKERR;
        return retval;
    }

    // Add membership to Multicast
    for (m_Iter = 0; m_Iter < s->m_nNumScap; m_Iter++) {
        if (s->m_Scap[m_Iter].name == SCAP_IDENT_MCAST_ADDRESS) {
#ifdef WINDOWS
            mreq.imr_multiaddr.s_addr = inet_addr(s->m_Scap[m_Iter].value);
            if (mreq.imr_multiaddr.s_addr == INADDR_NONE) {
                retval = RETCODE(RC_INTERNAL_ERR, 0);
                return retval;
            }
#else
            if (inet_aton(s->m_Scap[m_Iter].value,
                          &mreq.imr_multiaddr) == 0) {
                retval = RETCODE(RC_INTERNAL_ERR, 0);
                return retval;
            }
#endif
            foundMcastAddr = 1;
        }
    }

    if (s->m_nPureRTPMode) {
#ifdef WINDOWS
        mreq.imr_multiaddr.s_addr = inet_addr(s->m_MulticastAddress);
        if (mreq.imr_multiaddr.s_addr == INADDR_NONE) {
            retval = RETCODE(RC_INTERNAL_ERR, 0);
            return retval;
        }
#else
        if (inet_aton(s->m_MulticastAddress,
                      &mreq.imr_multiaddr) == 0) {
            retval = RETCODE(RC_INTERNAL_ERR, 0);
            return retval;
        }
#endif
        foundMcastAddr = 1;
    }

    if (!foundMcastAddr) {
        return RETCODE(RC_INTERNAL_ERR, 0);
    }
    mreq.imr_interface.s_addr = INADDR_ANY;

    iResult = Rx_JoinMulticastAtInterfaces(s, sock, &mreq);

    if (iResult < 0) {
        retval = getReturnCode(RC_SOCKERR);
        s->m_statuscode = STATUSCODE_SOCKERR;
        return retval;
    }
    sa->sin_addr.s_addr = mreq.imr_multiaddr.s_addr;

    LOG_SYS(V2DLIB_INFO, "V2DRX: Signed on for multicast %s\n",
        inet_ntoa(sa->sin_addr));
    return retval;
}

#ifndef WINDOWS
static int Rx_BindSocketToPort(V2DRx_t *s, int sock, int clientPort,
        int is_multicast, int is_video, int portIndex, struct sockaddr_in * sa)
{
    /* For multicast case, we set SO_REUSEADDR on the socket. This
       allows bind to succeed even if the UDP port is being used by
       some other multicast connection */
    int retval = 0;
    if (is_multicast && !s->m_bUseHTTP) {
        int reuse = 1;
        if (setsockopt(sock, SOL_SOCKET,
                       SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            retval = getReturnCode( RC_SOCKERR );
            s->m_statuscode = STATUSCODE_SOCKERR;
            return retval;
        }
    }

    if (clientPort > 0) { // Only try to bind to requested port
        if (s->m_bUseHTTP)
            sa->sin_port = 0;
        else
            sa->sin_port = htons(clientPort);

        if (is_video)
            s->m_nVideoPort = clientPort;
        else
            s->m_nAudioPort = clientPort;

        if (bind(sock,(struct sockaddr *)sa, sizeof(struct sockaddr_in)) < 0) {
            retval = getReturnCode( RC_SOCKERR );
            s->m_statuscode = STATUSCODE_SOCKERR;
            return retval;
        }
    }
    else { // Try ports starting from default till we get a free port
        int port = 0;
        if (is_video)
            clientPort = s->m_nSvrPort;
        else
            clientPort = s->m_nSvrPort + 1;

        while (port < 500) {
            sa->sin_port = htons(clientPort + port);
            if (is_video)
                s->m_nVideoPort = clientPort + port;
            else
                s->m_nAudioPort = clientPort + port;

            if (bind(sock, (struct sockaddr *)sa, sizeof(struct sockaddr_in)) < 0) {
                port += 2;
            }
            else if (portIndex >= 0) {
                sprintf(s->m_Creq[portIndex].value, "%d",
                        clientPort + port);
                break;
            }
            else {
                retval = RETCODE(RC_INTERNAL_ERR, 0);
                s->m_statuscode = STATUSCODE_SOCKERR;
                return retval;
            }
        }
        if (port >= 500) {
            retval = RETCODE(RC_SOCKERR, errno);
            s->m_statuscode = STATUSCODE_SOCKERR;
            return retval;
        }
    }
    return retval;
}

// Connect on the socket. For UDP this means that we will only accept
// datagrams from the original server.
static int Rx_ConnectSocket(V2DRx_t *s, int sock, int is_video, int is_multicast)
{
    int retval = 0;
    struct sockaddr_in destsa;

    // We do not know the source IP in pure RTP mode so we cannot create a
    // connected socket
    if (s->m_nPureRTPMode)
        return 0;

    // Set destination port to 0 for UDP else to http port
    destsa.sin_family = AF_INET;
    memset(destsa.sin_zero, 0, sizeof (destsa.sin_zero));
    destsa.sin_addr = s->m_Destination.sin_addr;
    if (s->m_bUseHTTP) {
        destsa.sin_port = htons(s->m_nHTTPPort);
        if (is_video)
            s->m_nVideoFlags = AV_HTTP_CONNECT;
        else
            s->m_nAudioFlags = AV_HTTP_CONNECT;
    }
    else {
        destsa.sin_port = 0; /* any port */
    }

    /* Do not create a connected socket if server is local
     * host and we are doing multicast */
    if (!s->m_bUseHTTP && is_multicast &&
        s->m_Destination.sin_addr.s_addr == htonl(0x7F000001))
        return retval;

    if (connect(sock, (struct sockaddr *) &destsa, sizeof(destsa)) < 0) {
        retval = RETCODE(RC_SOCKERR, errno);
        s->m_statuscode = STATUSCODE_SOCKERR;
        return retval;
    }

    return retval;
}
#endif

static int Rx_SetSocketBufferLength(V2DRx_t *s, int sock, int bufferLen)
{
    int nBufferLen = bufferLen;
    int nOptLen = sizeof(nBufferLen);
    int retval = 0;
    int iResult;

#ifdef WINDOWS
    iResult = setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
            (char *)&nBufferLen, nOptLen);
#else
    iResult = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &nBufferLen, nOptLen);
#endif

    if (iResult < 0) {
        retval = getReturnCode(RC_SOCKERR);
        s->m_statuscode = STATUSCODE_SOCKERR;
        return retval;
    }
    return retval;
}

static int Rx_MakeSocketNonBlocking(V2DRx_t *s, int sock)
{
    int retval = 0;

#ifdef WINDOWS
    {
        DWORD dwNonBlocking = 1;
        if (ioctlsocket(sock, FIONBIO, (u_long FAR*)&dwNonBlocking)) {
            retval = RETCODE(RC_SOCKERR, WSAGetLastError());
            s->m_statuscode = STATUSCODE_SOCKERR;
            return retval;
        }
    }
#else
    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0 ) {
        retval = RETCODE(RC_SOCKERR, errno);
        s->m_statuscode = STATUSCODE_SOCKERR;
        return retval;
    }
#endif

    return retval;
}

/**
 * Rx_CreateVideoSocket - Create UDP socket to receive video data
 *
 * Input:
 *   s - pointer to the V2DRx_t for which to create the socket
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Rx_CreateVideoSocket(V2DRx_t *s)
{


    struct sockaddr_in sa;
    int nClientPort = 0;
    int portIndex = -1;
    int nMulticast = 0;
    int retval = 0;
    int iResult = 0;

    int m_Iter;
#ifdef WINDOWS
    struct sockaddr_in destsa;
    unsigned char copt;
    int nOptLen=0;
    int port;
#endif
    socklen_t nSockNameLen = sizeof(struct sockaddr_in);

    for (m_Iter=0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_VIDEO_PORT) {
            nClientPort = atoi(s->m_Creq[m_Iter].value);
            portIndex = m_Iter;
        }
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_MULTICAST) {
            nMulticast = atoi(s->m_Creq[m_Iter].value);
        }
    }

    LOG_SYS(
		V2DLIB_INFO,
		"V2DRX: Create Video Socket called port = %d, multicast=%d\n",
		nClientPort, nMulticast);

    // Create socket for Video stream listener
    if (s->m_nVideoSock > 0) {
        v2dLibCloseSocket(s->m_nVideoSock);
    }
    if (s->m_bUseHTTP) {
        s->m_nVideoSock = socket(AF_INET, SOCK_STREAM, 0);
    }
    else {
        s->m_nVideoSock = socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (s->m_nVideoSock == -1) {
        retval = getReturnCode( RC_SOCKERR );
        s->m_statuscode = STATUSCODE_SOCKERR;
        goto EXIT_LABEL;
    }

    if (s->m_nCtlSock > 0) {
        getsockname (s->m_nCtlSock, (struct sockaddr *)&sa, &nSockNameLen);
    }
    else {
        sa.sin_addr.s_addr = INADDR_ANY;
    }
    sa.sin_family = AF_INET;
    memset(sa.sin_zero, 0, sizeof (sa.sin_zero));

    // On windows, setsockopt for IP_ADD_MEMBERSHIP fails
    // if done before bind. So moving bind code up for windows only
#ifdef WINDOWS
    sa.sin_addr.s_addr = INADDR_ANY;

	if (nMulticast) {
        copt = 1; nOptLen = 1;
        iResult = setsockopt( s->m_nVideoSock, SOL_SOCKET, SO_REUSEADDR,
            (char*)&copt, nOptLen
            );

        if ( iResult < 0 ) {
            retval = getReturnCode( RC_SOCKERR );
            s->m_statuscode = STATUSCODE_SOCKERR;
            goto EXIT_LABEL;
        }
    }

    if ( (nClientPort > 0) && (s->m_bUseHTTP || nMulticast) ) {
        if (s->m_bUseHTTP)
            sa.sin_port = 0;
        else
            sa.sin_port = htons(nClientPort);

        s->m_nVideoPort = nClientPort;

        if (bind(s->m_nVideoSock,(struct sockaddr *)&sa,sizeof(sa)) < 0) {
            retval = getReturnCode( RC_SOCKERR );
            s->m_statuscode = STATUSCODE_SOCKERR;
            goto EXIT_LABEL;
        }
    }
    else { // Try ports starting from default till we get a free port
        nClientPort = s->m_nSvrPort;
        port = 0;
        while (port < 50) {
            sa.sin_port = htons(nClientPort+port);
            s->m_nVideoPort = nClientPort + port;

            if ( bind( s->m_nVideoSock,
                       (struct sockaddr *)&sa,
                       sizeof(sa)
                     ) < 0) {
                         v2dLibCloseSocket(s->m_nVideoSock);
                         s->m_nVideoSock = socket(AF_INET, SOCK_DGRAM, 0);
                         port += 2;
            }
            else if (portIndex >= 0) {
                sprintf(s->m_Creq[portIndex].value, "%d",
                        nClientPort+port);
                break;
            }
            else {
                retval = RETCODE(RC_INTERNAL_ERR, 0);
                goto EXIT_LABEL;
            }
        }
        if (port >= 50) {
            retval = RETCODE(RC_SOCKERR, errno);
            s->m_statuscode = STATUSCODE_SOCKERR;
            goto EXIT_LABEL;
        }
    }
    // Make the UDP socket a "connected" socket so that we only accept
    // datagrams from the original server
    if (s->m_bUseHTTP) {
        destsa.sin_family = AF_INET;
        memset(destsa.sin_zero, 0, sizeof (sa.sin_zero));
        destsa.sin_addr = s->m_Destination.sin_addr;
        destsa.sin_port = htons(s->m_nHTTPPort);
        s->m_nVideoFlags = AV_HTTP_CONNECT;
        if (connect(s->m_nVideoSock, (struct sockaddr *) &destsa,
                    sizeof(destsa)) < 0) {
            LOG_SYS(V2DLIB_ERROR, "V2DRX: Connect failed for Video Socket\n");
            retval = RETCODE(RC_SOCKERR, errno);
            s->m_statuscode = STATUSCODE_SOCKERR;
            goto EXIT_LABEL;
        }
    }

#endif

    // Setup Multicast
    if (nMulticast) {
        iResult = Rx_SetupMulticast(s, s->m_nVideoSock, &sa);
        if (iResult != 0) {
            retval = iResult;
            goto EXIT_LABEL;
        }
        strncpy(s->m_MulticastAddress, inet_ntoa(sa.sin_addr), 31);
    }

#ifndef WINDOWS
    // Bind
    iResult = Rx_BindSocketToPort(s, s->m_nVideoSock, nClientPort,
            nMulticast, 1, portIndex, &sa);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

    // Connect
    iResult = Rx_ConnectSocket(s, s->m_nVideoSock, 1, nMulticast);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

#endif

    // Increase socket buffer size
    iResult = Rx_SetSocketBufferLength(s, s->m_nVideoSock, VIDEO_RCV_SOCK_BUF_LEN);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

    // Set socket to be non blocking
    iResult = Rx_MakeSocketNonBlocking(s, s->m_nVideoSock);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

    LOG_SYS(V2DLIB_INFO, "V2DRX: Created Video Socket %d\n", s->m_nVideoSock);

 EXIT_LABEL:
    return retval;
}

/**
 * Rx_CreateAudioSocket - Create UDP socket to receive audio data
 *
 * Input:
 *   s - pointer to the V2DRx_t for which to create the socket
 *
 * Returns 0 if success, non-zero error code if failure
 */
static int Rx_CreateAudioSocket(V2DRx_t *s)
{
    struct sockaddr_in sa;
    int nClientPort = 0;
    int port = 0;

    int portIndex = -1;
    int nMulticast = 0;
    int retval = 0;
    int iResult = 0;
    int m_Iter;

#ifdef WINDOWS
    struct sockaddr_in destsa;
    unsigned char copt;
    int nOptLen=0;
#endif

    socklen_t nSockNameLen = sizeof(struct sockaddr_in);



    for (m_Iter=0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_MULTICAST) {
            nMulticast = atoi(s->m_Creq[m_Iter].value);
        }
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_VIDEO_PORT) {
            port = atoi(s->m_Creq[m_Iter].value);
            if (port > 0)
                nClientPort = port + 1;
        }
    }
    for (m_Iter=0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AUDIO_PORT) {
            port = atoi(s->m_Creq[m_Iter].value);
            nClientPort = port;
            portIndex = m_Iter;
        }
    }

    LOG_SYS(V2DLIB_INFO,
	"V2DRX: Create Audio Socket called port = %d, multicast=%d\n",
	port, nMulticast);

    // Create socket for Audio stream listener
    if (s->m_nAudioSock > 0) {
        v2dLibCloseSocket(s->m_nAudioSock);
    }
    if (s->m_bUseHTTP) {
        s->m_nAudioSock = socket(AF_INET, SOCK_STREAM, 0);
    }
    else {
        s->m_nAudioSock = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (s->m_nAudioSock == -1)
    {
        retval = getReturnCode( RC_SOCKERR );
        s->m_statuscode = STATUSCODE_SOCKERR;
        goto EXIT_LABEL;
    }

    if (s->m_nCtlSock > 0) {
        getsockname (s->m_nCtlSock, (struct sockaddr *)&sa, &nSockNameLen);
    }
    else {
        sa.sin_addr.s_addr = INADDR_ANY;
    }
    sa.sin_family = AF_INET;
    memset(sa.sin_zero, 0, sizeof (sa.sin_zero));

    // On windows, setsockopt for IP_ADD_MEMBERSHIP fails
    // if done before bind. So moving bind code up for windows only
#ifdef WINDOWS
    sa.sin_addr.s_addr = INADDR_ANY;

	if(nMulticast) {
		copt = 1; nOptLen = 1;
		iResult = setsockopt( s->m_nAudioSock, SOL_SOCKET, SO_REUSEADDR,
			(char*)&copt, nOptLen
			);

		if ( iResult < 0 )
		{
			retval = getReturnCode( RC_SOCKERR );
			s->m_statuscode = STATUSCODE_SOCKERR;
			goto EXIT_LABEL;
		}
	}

    if ( (nClientPort > 0) && (s->m_bUseHTTP || nMulticast) ) {

        if (s->m_bUseHTTP)
            sa.sin_port = 0;
        else
            sa.sin_port = htons(nClientPort);

        s->m_nAudioPort = nClientPort;

        if (bind(s->m_nAudioSock,(struct sockaddr *)&sa,sizeof(sa)) < 0) {
            retval = getReturnCode( RC_SOCKERR );
            s->m_statuscode = STATUSCODE_SOCKERR;
            goto EXIT_LABEL;
        }
    }
    else { // Try ports starting from default till we get a free port
        nClientPort = s->m_nSvrPort + 1;
        port = 0;
        while (port < 50) {
            sa.sin_port = htons(nClientPort + port);
            s->m_nAudioPort = nClientPort + port;

            if (bind(s->m_nAudioSock,
                (struct sockaddr *)&sa,sizeof(sa))<0) {
                    v2dLibCloseSocket(s->m_nAudioSock);
                    s->m_nAudioSock = socket(AF_INET, SOCK_DGRAM, 0);
                    port += 2;
            }
            else if (portIndex >= 0) {
                sprintf(s->m_Creq[portIndex].value, "%d",
                        nClientPort+port);
                break;
            }
            else {
                retval = RETCODE(RC_INTERNAL_ERR, 0);
                goto EXIT_LABEL;
            }
        }
        if (port >= 50) {
            retval = RETCODE(RC_SOCKERR, errno);
            s->m_statuscode = STATUSCODE_SOCKERR;
            goto EXIT_LABEL;
        }
    }

    // Make the UDP socket a "connected" socket so that we only accept
    // datagrams from the original server
    if (s->m_bUseHTTP) {
		destsa.sin_family = AF_INET;
		memset(destsa.sin_zero, 0, sizeof (sa.sin_zero));
		destsa.sin_addr = s->m_Destination.sin_addr;
		destsa.sin_port = htons(s->m_nHTTPPort);
        s->m_nAudioFlags = AV_HTTP_CONNECT;
		if (connect(s->m_nAudioSock, (struct sockaddr *) &destsa,
                sizeof(destsa)) < 0) {
			LOG_SYS(V2DLIB_ERROR, "V2DRX: Connect failed for Audio Socket\n");
			retval = RETCODE(RC_SOCKERR, errno);
                        s->m_statuscode = STATUSCODE_SOCKERR;
			goto EXIT_LABEL;
		}
	}

#endif

    if (nMulticast) {
        iResult = Rx_SetupMulticast(s, s->m_nAudioSock, &sa);
        if (iResult != 0) {
            retval = iResult;
            goto EXIT_LABEL;
        }
        strncpy(s->m_MulticastAddress, inet_ntoa(sa.sin_addr), 31);
    }


#ifndef WINDOWS

    // Bind
    iResult = Rx_BindSocketToPort(s, s->m_nAudioSock, nClientPort,
            nMulticast, 0, portIndex, &sa);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

    // Connect
    iResult = Rx_ConnectSocket(s, s->m_nAudioSock, 0, nMulticast);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

#endif

    // Increase socket buffer size
    iResult = Rx_SetSocketBufferLength(s, s->m_nAudioSock, AUDIO_RCV_SOCK_BUF_LEN);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

    // Set socket to be non blocking
    iResult = Rx_MakeSocketNonBlocking(s, s->m_nAudioSock);
    if (iResult != 0) {
        retval = iResult;
        goto EXIT_LABEL;
    }

    LOG_SYS(V2DLIB_INFO, "V2DRX: Created Audio Socket %d\n", s->m_nAudioSock);

 EXIT_LABEL:
    return retval;
}

const char * Rx_GetVideoName(V2DRx_t *s) {
    int m_Iter;
    for (m_Iter=0; m_Iter < s->m_nNumScap; m_Iter++) {
        if (s->m_Scap[m_Iter].name == SCAP_IDENT_VIDEO_NAME) {
            return s->m_Scap[m_Iter].value;
        }
    }
    return "";
}

/**
 * Rx_PopulateScap - Populates the m_Scap array with the server capabilities
 * from the receive buffer. Also overrides the videoport, audioport and audio
 * type parameters in client request with the values for these protocol units
 * if they are present in the received server capabilities buffer.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct to be processed
 *
 * Returns -1 no request found in the receive buffer, else return 0
 */
static int Rx_PopulateScap(V2DRx_t *s)
{
    int len=0, plen=0;
    int i=4, pval;
    int code=0;
    char val[80];
    unsigned char *cp;
    unsigned short protid;
    int retval=0;
    int vIdx = -1, aIdx = -1, pIdx = -1;
    int versionIdx = -1;
    int m_Iter;
    int nMulticast = 0;

    // Find VIDEO_PORT, AUDIO_PORT and AUDIO_TYPE in CREQ
    for (m_Iter = 0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_VIDEO_PORT)
            vIdx = m_Iter;
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AUDIO_PORT)
            aIdx = m_Iter;
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AUDIO_TYPE)
            pIdx = m_Iter;
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_PROTOCOL_VERSION)
            versionIdx = m_Iter;
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_OPTION)
            (void) atoi(s->m_Creq[m_Iter].value);
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_MULTICAST) {
            nMulticast = atoi(s->m_Creq[m_Iter].value);
        }
    }

    LOG_SYS(V2DLIB_VERBOSE, "V2DRX: Processing Server Capabilities\n");

    // Process receive buffer
    cp = s->m_CtlRcv;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_SERVER_CAP)
        return -1;
    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    s->m_nNumScap=0;
    cp += 4;

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
	    s->m_statuscode = STATUSCODE_PROTERR;
            break;
        }
        memset(val, 0, 80);
        switch (code) {
        case SCAP_IDENT_VIDEO_NAME:
        case SCAP_IDENT_MCAST_ADDRESS:
            GET_PROTOCOL_UNIT_VALUE_STR(val, cp, plen);
            break;
        case SCAP_IDENT_VIDEO_PORT:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            sprintf (val, "%d", pval);
            if (vIdx >= 0 && nMulticast) {
                LOG_SYS(V2DLIB_VERBOSE, "V2DRX: Overiding Video Port %s -> %d\n",
                        s->m_Creq[vIdx].value, pval);
                sprintf(s->m_Creq[vIdx].value, "%d", pval);
            }
            break;
        case SCAP_IDENT_AUDIO_PORT:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            sprintf (val, "%d", pval);
            if (aIdx >= 0 && nMulticast) {
                LOG_SYS(V2DLIB_VERBOSE, "V2DRX: Overiding Audio Port %s -> %d\n",
                        s->m_Creq[aIdx].value, pval);
                sprintf(s->m_Creq[aIdx].value, "%d", pval);
            }
            break;
        case SCAP_IDENT_AUDIO_TYPE:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            sprintf (val, "%d", pval);
            if (pIdx >= 0)
                sprintf(s->m_Creq[pIdx].value, "%d", pval);
            break;
        case SCAP_IDENT_PROTOCOL_VERSION:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            if (s->m_nRxState == CLIENT_STATE_CONNECTED) {
                int major, minor;
                // In the intial handshake phase we negotiate protocol version
                LOG_SYS(V2DLIB_INFO, "V2DRX: Remote Protocol Version = %s\n",
                        getV2DProtocolVersion(pval, val, 32));
                if (OUR_PROTOCOL_VERSION_MAJOR > V2D_PROTOCOL_VERSION_MAJOR(pval)) {
                    major = V2D_PROTOCOL_VERSION_MAJOR(pval);
                    minor = V2D_PROTOCOL_VERSION_MINOR(pval);
                }
                else if (OUR_PROTOCOL_VERSION_MAJOR < V2D_PROTOCOL_VERSION_MAJOR(pval)) {
                    major = OUR_PROTOCOL_VERSION_MAJOR;
                    minor = OUR_PROTOCOL_VERSION_MINOR;
                }
                else if (OUR_PROTOCOL_VERSION_MINOR > V2D_PROTOCOL_VERSION_MINOR(pval)) {
                    major = OUR_PROTOCOL_VERSION_MAJOR;
                    minor = V2D_PROTOCOL_VERSION_MINOR(pval);
                }
                else if (OUR_PROTOCOL_VERSION_MINOR < V2D_PROTOCOL_VERSION_MINOR(pval)) {
                    major = OUR_PROTOCOL_VERSION_MAJOR;
                    minor = OUR_PROTOCOL_VERSION_MINOR;
                }
                else {
                    // remote is already at par
                    major = OUR_PROTOCOL_VERSION_MAJOR;
                    minor = OUR_PROTOCOL_VERSION_MINOR;
                }
                s->m_version = V2D_PROTOCOL_VERSION(major, minor);
                LOG_SYS(V2DLIB_INFO,
			"V2DRX: Negotiated Protocol Version = %s\n",
			getV2DProtocolVersion(s->m_version, val, 32));

                s->m_bRecvRTP = (s->m_version >= V2D_PROTOCOL_VERSION(1,2))
                    ? TRUE : FALSE;

                if (versionIdx == -1) {
                    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_PROTOCOL_VERSION;
                    versionIdx = s->m_nNumCreq;
                    s->m_nNumCreq++;
                }
                sprintf(s->m_Creq[versionIdx].value, "%d", s->m_version);
            }
            sprintf (val, "%d", pval);
            break;
        default:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            sprintf (val, "%d", pval);
            break;
        }
        s->m_Scap[s->m_nNumScap].name = code;
        strcpy(s->m_Scap[s->m_nNumScap].value, val);
        if (++s->m_nNumScap >= 50) break;
        cp = &cp[plen];
        i += plen;
    }

    return retval;
}


static int Rx_HandleRFBStatus(V2DRx_t *s)
{
    int len, plen;
    int i=4, pval;
    int code;
    char val[80];
    unsigned char *cp;
    unsigned short protid;
    int retval=0;
    int count = 0;
    unsigned long sessionId = 0;
    unsigned long queryId = 0;
    int status  = 0;

    // Process receive buffer
    cp = s->m_CtlRcv;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_SERVER_RFBSTATUS)
        return -1;
    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;
    LOG_SYS(V2DLIB_VERBOSE, "V2DRX: Processing RFB Status Message");

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
	    s->m_statuscode = STATUSCODE_PROTERR;
            break;
        }
        memset(val, 0, 80);
        switch (code) {
        case RFBSTAT_IDENT_SESSIONID:
            sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RFBSTAT_IDENT_QUERYID:
            queryId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RFBSTAT_IDENT_STATUS:
            pval = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            status = pval;
            break;
        default:
            break;
        }
        if (++count >= 50) break;
        cp = &cp[plen];
        i += plen;
    }

    // If reply is for us, we honor the status.
    if (s->m_nSessionId == sessionId &&
        s->m_nQueryId == queryId) {
        LOG_SYS(V2DLIB_VERBOSE, "V2DRX: RFB: Got reply %d for %ld:%ld\n",
                status, sessionId, queryId);
        s->m_bRFBStatus = status;
        s->m_nQueryId++;
    }

    // If message was broadcast, we reset the current status and query
    // for the latest status
    else if (sessionId == 0) {
        LOG_SYS(V2DLIB_VERBOSE, "V2DRX: RFB: Scheduling RFB Query\n");
        V2DASSERT(queryId == 0);
        s->m_bRFBStatus = FALSE;
        s->m_nCtlFlags |= RX_SEND_RFB_QUERY;
    }

    return retval;
}

/**
 * Rx_HandleReject - Processes the server reject message in the RX'es
 * receive buffer
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns non-zero code for the reject reason
 */
static int Rx_HandleReject(V2DRx_t *s)
{
    int len=0, plen=0;
    int unit_id, err_code;
    int i=4, retval=0;
    unsigned char *cp;

    cp = s->m_CtlRcv;
    len = GET_PROTOCOL_PACKET_LENGTH(cp);

    if (len < 4) {
        retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
        s->m_statuscode = STATUSCODE_PROTERR;
        goto EXIT_LABEL;
    }
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        unit_id = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
	    s->m_statuscode = STATUSCODE_PROTERR;
            break;
        }
        err_code = GET_PROTOCOL_UNIT_VALUE(cp, plen);
        switch(unit_id) {
        case SREJ_IDENT_AUTHERR:
            switch(err_code) {
            case SREJ_ERR_BAD_NETWORK:
                retval = RETCODE(RC_AUTH_BAD_NETWORK, 0);
                break;
            case SREJ_ERR_NO_UNICAST:
                retval = RETCODE(RC_AUTH_NO_UNICAST, 0);
                break;
            case SREJ_ERR_NO_MULTICAST_INIT:
                retval = RETCODE(RC_AUTH_NO_MULTICAST_INIT, 0);
                break;
            case SREJ_ERR_NO_MULTICAST_PART:
                retval = RETCODE(RC_AUTH_NO_MULTICAST_PART, 0);
                break;
            default:
                retval = RETCODE(RC_AUTH_ERR, 0);
                break;
            }
            s->m_statuscode = STATUSCODE_AUTHERR;
            break;
        case SREJ_IDENT_PROTOERR:
            switch(err_code) {
            case SREJ_ERR_UNEXPECTED_ID:
                retval = RETCODE(RC_PROTO_UNEXPECTED_ID, 0);
                break;
            case SREJ_ERR_MALFORMED_PACKET:
                retval = RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
                break;
            default:
                retval = RETCODE(RC_PROTO_ERR, 0);
                break;
            }
            s->m_statuscode = STATUSCODE_PROTERR;
            break;
        case SREJ_IDENT_SERVERR:
            switch(err_code) {
            case SREJ_ERR_UNKNOWN_ERR:
                retval = RETCODE(RC_SERV_UNKNOWN_ERR, 0);
                s->m_statuscode = STATUSCODE_SERVERERR;
                break;
            case SREJ_ERR_SERVER_DOWN:
                retval = RETCODE(RC_SERV_SERVER_DOWN, 0);
                s->m_statuscode = STATUSCODE_SERVERERR;
                break;
            case SREJ_ERR_CONTROLLER_GONE:
                retval = RETCODE(RC_SERV_CONTROLLER_GONE, 0);
                s->m_statuscode = STATUSCODE_SERVERERR;
                break;
            case SREJ_ERR_SERVER_PREMPTED:
                retval = RETCODE(RC_SERV_SERVER_PREMPTED, 0);
                s->m_statuscode = STATUSCODE_PREMPTED;
                break;
            default:
                retval = RETCODE(RC_SERV_ERR, 0);
                s->m_statuscode = STATUSCODE_SERVERERR;
                break;
            }
            s->m_nReconnectPeriod = RECONNECT_INTERVAL;

            break;
        case SREJ_IDENT_REQERR:
            switch(err_code) {
            case SREJ_ERR_ALREADY_CONNECTED:
                retval = RETCODE(RC_REQ_ALREADY_CONNECTED, 0);
                s->m_statuscode = STATUSCODE_SERVERBUSY;
                break;
            case SREJ_ERR_UNICAST_ONLY:
                retval = RETCODE(RC_REQ_UNICAST_ONLY, 0);
                s->m_statuscode = STATUSCODE_MULTICASTERR;
                break;
            case SREJ_ERR_MULTICAST_ONLY:
                retval = RETCODE(RC_REQ_MULTICAST_ONLY, 0);
                s->m_statuscode = STATUSCODE_UNICASTERR;
                break;
            case SREJ_ERR_MULTICAST_MISMATCH:
                retval = RETCODE(RC_REQ_MULTICAST_MISMATCH, 0);
                s->m_statuscode = STATUSCODE_MISMATCHERR;
                break;
            default:
                retval = RETCODE(RC_REQ_ERR, 0);
                s->m_statuscode = STATUSCODE_UNKNOWN;
                break;
            }
            s->m_nReconnectPeriod = RECONNECT_INTERVAL;
            break;
        default:
            retval = RETCODE(RC_PROTO_ERR, 0);
            break;
        }
        cp = &cp[plen];
        i += len;
    }

 EXIT_LABEL:

    LOG_SYS(V2DLIB_WARN, "V2DRX: Handle Reject Error Code %d",
            (retval&0xffff0000)>>16);

    return retval;
}

/**
 * Rx_CtlWrite - Writes the control message in the RX'es send buffer to
 * the network
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns number of bytes written if success, non-zero code if failure
 */
static int Rx_CtlWrite(V2DRx_t *s)
{
    int len;
    int iSent;
    int protoid;

    if (s->m_nCtlSock <= 0)
        return 0;

    // Send the control packet as long as the current negotiated
    // protocol version allows it
    protoid = GET_PROTOCOL_UNIT_ID(s->m_CtlSnd);
    if (Rx_CheckProtocolVersion(s->m_version, protoid)) {
        len = GET_PROTOCOL_PACKET_LENGTH(s->m_CtlSnd);
        iSent = doTCPWrite(s->m_nCtlSock, (char *)s->m_CtlSnd, len );

        if ( iSent < 0 ) {
            return getReturnCode( RC_WRITEERROR );
        }
        else {
            s->m_nCtrlSndPkts++;
            s->m_nCtrlSndBytes += iSent;
        }
    }
    else {
        // message got filtered
        s->m_nCtlPacketsDropped++;
    }

    return 0;
}
/**
 * Rx_CtlHTTPRequest - Sends an HTTP Request to the server
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 */
static int Rx_CtlHTTPRequest(V2DRx_t *s)
{
    int len;
    char *cp=NULL;
    int retval;

    cp = (char *) malloc (HTTP_REQUEST_SIZE+1);
    sprintf (cp, HTTPRequestFmt, s->m_nSvrPort, "control");


    len = doHTTPWrite( s->m_nCtlSock, (char*)(cp), strlen(cp));
    free(cp);
    if (len == 0) {
        retval = getReturnCode( RC_PROTO_ERR );
    }
    else {
        retval=0;
    }

    return retval;
}
/**
 * Rx_CtlHTTPResponse - Reads an HTTP Response from the server
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Parses the response. Changes the state to connected if the
 * server accepts the connection.
 */
static int Rx_CtlHTTPResponse(V2DRx_t *s)
{
    int len = 0;
    char *cp = (char *)(s->m_CtlRcv) + s->m_CtlRcvLength;
    int retval=0;

	len = doHTTPRead
		(s->m_nCtlSock, (char*)(cp), HTTP_RESPONSE_SIZE - s->m_CtlRcvLength);

    if (len <= 0) {
        retval = getReturnCode( RC_PROTO_ERR );
    }

	if(len > 0)
		s->m_CtlRcvLength += len;

    if (s->m_CtlRcvLength == HTTP_RESPONSE_SIZE){
        s->m_nRxState = CLIENT_STATE_CONNECTED;
        s->m_statuscode = STATUSCODE_AUTHENTICATING;
        s->m_bSendNotify=TRUE;
		s->m_CtlRcvLength = 0;
    }
    else {
        retval = -1;
        // EAGAIN
    }
    if (retval > 0)
        Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);


    return retval;
}
/**
 * Rx_CheckAudioConnect - Checks if the control connection for the V2DRx_t
 * has failed, set the state to CONNECTED if it hasn't.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct to be checked
 *
 * Returns 0 if connection hasn't failed, non-zero error code if failed
 */
static int Rx_CheckAudioConnect(V2DRx_t *s)
{
    int retval=0;

#ifdef WINDOWS
    fd_set write_fdset, except_fdset;
    struct timeval timeout;
    int iReady = 0;

    FD_ZERO( &write_fdset );
    FD_ZERO( &except_fdset );

    timeout.tv_sec =  0;
    timeout.tv_usec = 0;

    FD_SET((unsigned int) s->m_nAudioSock, &write_fdset );
    FD_SET((unsigned int) s->m_nAudioSock, &except_fdset );

    // on windows, when connect() is called on non-blocking call
    // connection succeeded if the socket is ready for writing

    iReady = select( 0, NULL, &write_fdset, &except_fdset, &timeout );

    // connection failed
    if ( FD_ISSET( s->m_nAudioSock, &except_fdset ) ) {
        Rx_Reset( s );
        Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
        retval = RETCODE( RC_CONNECT_ERR, WSAGetLastError() );
    }

    // succesful connection
    else if ( FD_ISSET( s->m_nAudioSock, &write_fdset ) ) {
        s->m_nAudioFlags = AV_HTTP_REQUEST;
    }

#else
    int sockoptval = 0;
    int sockoptlen = sizeof(sockoptval);
    retval = getsockopt( s->m_nAudioSock, SOL_SOCKET, SO_ERROR,
                         &sockoptval, (socklen_t *)&sockoptlen );

    if( retval < 0 ) {
        retval = RETCODE( RC_SOCKERR, errno );
    }
#endif

    return retval;
}
/**
 * Rx_AudioHTTPRequest - Sends an HTTP Request to the server
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 */
static int Rx_AudioHTTPRequest(V2DRx_t *s)
{
    int len;
    char *cp=NULL;
    int retval;

    cp = (char *) malloc (HTTP_REQUEST_SIZE+1);
    sprintf (cp, HTTPRequestFmt, s->m_nSvrPort, "audio");

    len = doHTTPWrite( s->m_nAudioSock, (char*)(cp), strlen(cp));

    free(cp);
    if (len == 0) {
        retval = getReturnCode( RC_PROTO_ERR );
    }
    else {
        retval=0;
    }

    return retval;
}
/**
 * Rx_AudioHTTPResponse - Reads an HTTP Response from the server
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Parses the response.
 */
static int Rx_AudioHTTPResponse(V2DRx_t *s)
{
    int len = 0;
    char *cp = (char *)(s->m_AudioRcv + s->m_nAudioLen);
    int retval = -1;

    len = doHTTPRead
		( s->m_nAudioSock, (char*)(cp), HTTP_RESPONSE_SIZE - s->m_nAudioLen);
    if (len <= 0) {
        retval = getReturnCode( RC_PROTO_ERR );
    }
	else if(len > 0) {
		s->m_nAudioLen += len;
	}

	if(s->m_nAudioLen == HTTP_RESPONSE_SIZE) {
		s->m_nAudioLen = 0;
		retval = 0;
	}
    return retval;
}
/**
 * Rx_CheckVideoConnect - Checks if the control connection for the V2DRx_t
 * has failed, set the state to CONNECTED if it hasn't.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct to be checked
 *
 * Returns 0 if connection hasn't failed, non-zero error code if failed
 */
static int Rx_CheckVideoConnect(V2DRx_t *s)
{
    int retval=0;

#ifdef WINDOWS
    fd_set write_fdset, except_fdset;
    struct timeval timeout;
    int iReady = 0;

    FD_ZERO( &write_fdset );
    FD_ZERO( &except_fdset );

    timeout.tv_sec =  0;
    timeout.tv_usec = 0;

    FD_SET((unsigned int) s->m_nVideoSock, &write_fdset );
    FD_SET((unsigned int) s->m_nVideoSock, &except_fdset );

    // on windows, when connect() is called on non-blocking call
    // connection succeeded if the socket is ready for writing

    iReady = select( 0, NULL, &write_fdset, &except_fdset, &timeout );

    // connection failed
    if ( FD_ISSET( s->m_nVideoSock, &except_fdset ) ) {
        Rx_Reset( s );
        Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
        retval = RETCODE( RC_CONNECT_ERR, WSAGetLastError() );
    }

    // succesful connection
    else if ( FD_ISSET( s->m_nVideoSock, &write_fdset ) ) {
        s->m_nVideoFlags = AV_HTTP_REQUEST;
    }

#else
    int sockoptval = 0;
    int sockoptlen = sizeof(sockoptval);
    retval = getsockopt( s->m_nVideoSock, SOL_SOCKET, SO_ERROR,
                         &sockoptval, (socklen_t *)&sockoptlen );

    if( retval < 0 ) {
        retval = RETCODE( RC_SOCKERR, errno );
    }
#endif

    return retval;
}
/**
 * Rx_VideoHTTPRequest - Sends an HTTP Request to the server
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 */
static int Rx_VideoHTTPRequest(V2DRx_t *s)
{
    int len;
    char *cp=NULL;
    int retval;


    cp = (char *) malloc (HTTP_REQUEST_SIZE+1);
    sprintf (cp, HTTPRequestFmt, s->m_nSvrPort, "video");


    len = doHTTPWrite (s->m_nVideoSock,  cp, strlen(cp));

    free(cp);
    if (len == 0) {
        retval = getReturnCode( RC_PROTO_ERR );
    }
    else {
        retval=0;
    }

    return retval;
}
/**
 * Rx_VideoHTTPResponse - Reads an HTTP Response from the server
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Parses the response.
 */
static int Rx_VideoHTTPResponse(V2DRx_t *s)
{
    int len = 0;
    char *cp = (char *)
        ( (s->m_bSliceRed == TRUE) ? s->m_RedVideoRcv : s->m_VideoRcv )
		+ s->m_nVideoLen;
    int retval = -1;

    len = doHTTPRead
		( s->m_nVideoSock, (char*)(cp), HTTP_RESPONSE_SIZE - s->m_nVideoLen);
    if (len <= 0) {
        retval = getReturnCode( RC_PROTO_ERR );
    }
	else if(len > 0) {
		s->m_nVideoLen += len;
	}

	if(s->m_nVideoLen == HTTP_RESPONSE_SIZE) {
		s->m_nVideoLen = 0;
		retval = 0;
	}
    return retval;
}

/**
 * Rx_CtlRead - Reads a control message into the RX'es send buffer from
 * the network
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns bytes read if success, non-zero code on error, or 0 if more bytes
 * are needed to read a complete PDU packet
 */
static int Rx_CtlRead(V2DRx_t *s)
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
        len = doTCPRead (s->m_nCtlSock,
                         (char *)s->m_CtlRcv + s->m_CtlRcvLength,
                         2 - s->m_CtlRcvLength);
        if (len <= 0) {
#ifndef WINDOWS
            if (errno == EWOULDBLOCK)
                return 0;
#endif
            return -1; // error on socket
        }

/*         s->m_nCtrlRcvPkts++; */
        s->m_nCtrlRcvBytes += len;

        s->m_CtlRcvLength += len;
        readcount++;;
        if (s->m_CtlRcvLength < 2)
            return 0; // need to wait

    }

    /* we have the first two bytes, so can find the length we need to read */
    cp = s->m_CtlRcv;
    plen = GET_PROTOCOL_PACKET_LENGTH(cp);

    if ((plen < 4) || (plen > (SEND_BUFFER_SIZE - 2))) {
        return -2; // malformed packet
    }

    Rx_GetTimeOfDay(s, &s->m_tLastCtlRead);

    /* Try to read the remaining bytes. */
    if (s->m_CtlRcvLength < plen) {
        len = doTCPRead (s->m_nCtlSock,
                         (char *)s->m_CtlRcv + s->m_CtlRcvLength,
                         plen - s->m_CtlRcvLength);
        if ((len <= 0 && readcount == 0) || (len < 0)) {
#ifndef WINDOWS
            if (errno == EWOULDBLOCK)
                return 0;
#endif
            return -1; // error on socket
        }

        s->m_nCtrlRcvPkts++;
        s->m_nCtrlRcvBytes += len;

        s->m_CtlRcvLength += len;

        if (s->m_CtlRcvLength < plen)
            return 0; // need to wait

    }

    // we have read the full PDU, and can reset the read pointer
    s->m_CtlRcvLength = 0;

    return plen;
}

/**
 * Rx_CtlProcess - Reads a control message into the RX'es send buffer from
 * the network
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns 0 if success, non-zero code if the connection should be
 * closed for some reason
 */
static int Rx_CtlProcess(V2DRx_t *s)
{
    int plen;
    unsigned char *cp;
    int protid;
    int retval=0;

    cp = s->m_CtlRcv;
    protid = GET_PROTOCOL_UNIT_ID(cp);

    switch (protid) {
    case  IDENT_SERVER_CAP:
        s->m_nFlag |= I_CTL;
        Rx_PopulateScap(s);
        break;
    case IDENT_SERVER_RFBSTATUS:
        s->m_nFlag |= I_CTL;
        Rx_HandleRFBStatus(s);
        break;
    case IDENT_SERVER_SERDAT:
        s->m_nFlag |= I_SER;
        plen = GET_PROTOCOL_UNIT_LENGTH(cp + 4);
        s->m_nSerialRcvd += (plen - 4);
        break;
    case  IDENT_SERVER_REJ:
        // We should not set the I_CTL flag since we do not
        // want to forward these messages to the client
        retval = Rx_HandleReject(s);
        break;
    case IDENT_SERVER_CLS:
        // We should not set the I_CTL flag since we do not
        // want to forward these messages to the client
        LOG_SYS(V2DLIB_INFO, "V2DRX: Received Disconnect Request (%d). Disabling client.",
                s->m_nRxState);
        s->m_nRxState = CLIENT_STATE_DISABLED;
        s->m_statuscode = STATUSCODE_DISCONNECTING;
        retval = RETCODE(RC_SERVER_CLS, 0);
        break;
    default:
        retval = RETCODE(RC_PROTO_ERR, 0);
        break;
    }

    if (retval)
        Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);

    return retval;
}

/**
 * Rx_VideoRead - Reads video data into the RX'es data buffer from
 * the network.
 * For non SRD mode: Sets the I_VIDEO flag bit to indicate to the
 * caller that there's data in the video receive buffers.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns 0 if success, non-zero code if read fails
 */

static int Rx_VideoRead(V2DRx_t *s)
{
    int status=0;
    unsigned long nSeqNum;
    unsigned char *rcvbuf;
    int nOffset=8;
    int m_Iter;
	const int nExpectedSize =
		(s->m_bRecvRTP ? RTP_VIDEOBUFF_SIZE : AV_BUFFER_SIZE);

	if(s->m_nVideoLen == nExpectedSize)
		s->m_nVideoLen = 0;

	// Set appropriate buffer for receiving data
    if(s->m_bSliceRed)
        rcvbuf = s->m_RedVideoRcv;
    else
        rcvbuf = s->m_VideoRcv;

    // Determine noffset
    for (m_Iter=0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name == AVREQ_IDENT_AV_OPTION) {
            if (atoi(s->m_Creq[m_Iter].value) != AVOPTION_VIDEO)
                nOffset = 0;
        }
    }

	if(s->m_bRecvRTP || s->m_nVideoLen)
		nOffset = 0;

	if (s->m_bUseHTTP) {
		status = doHTTPRead ( s->m_nVideoSock,
			(char*)(rcvbuf + nOffset + s->m_nVideoLen),
			(nExpectedSize - nOffset - s->m_nVideoLen));
	}
	else {
		status = doUDPRead ( s->m_nVideoSock,
			(char*)(rcvbuf + nOffset + s->m_nVideoLen),
			(nExpectedSize - nOffset - s->m_nVideoLen));
	}

	if (status > 0) {
		s->m_nVideoLen += status + nOffset;
	}

    if (s->m_nVideoLen == nExpectedSize) {

		s->m_nVideoPkts++;
		s->m_nVideoBytes += nExpectedSize;

		if(!s->m_bRecvRTP)
			nSeqNum = V2DGETSEQUENCENUM(rcvbuf);
		else
			nSeqNum = RTPGETSEQUENCENUM(rcvbuf);

 		if (s->m_nVideoSeq && (s->m_nVideoSeq != nSeqNum)) {
			if (s->m_nVideoSeq < nSeqNum) {
				s->m_nVideoLoss += (nSeqNum - s->m_nVideoSeq);
				s->m_nVideoSeq = nSeqNum + 1;
			}
			else if (s->m_nVideoSeq - nSeqNum == 65536) {
				s->m_nVideoSeq = nSeqNum + 1;
			}
			else {
				s->m_nVideoOutOfOrder++;
				if (s->m_nVideoOutOfOrder % 10 == 0)
					s->m_nVideoSeq = 0;
			}
		}
		else {
			s->m_nVideoSeq = nSeqNum + 1;
		}

        // Update the time as it is meant to detect dead TXs.
        // Receiving either control or data means the
        // TX is still alive.
        Rx_GetTimeOfDay(s, &s->m_tLastCtlRead);


        if (s->m_bSliceRed) { // Send packet to slice reduction library
			findFramesinPacket(s->sliceRdata, s->m_RedVideoRcv +
				(s->m_bRecvRTP ? RTP_HEADER_SIZE : V2D_PACKET_HEADER_SIZE));
        }
        else { // Else send it back to client
            s->m_nFlag |= I_VIDEO;
        }
    }

    return 0;
}

/**
 * Rx_AudioRead - Reads audio data into the RX'es data buffer from
 * the network. Sets the I_AUDIO flag bit to indicate to the
 * caller that there's data in the video receive buffers.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns 0 if success, non-zero code if read fails
 */
static int Rx_AudioRead(V2DRx_t *s)
{
    int status=0;
    unsigned long nSeqNum;
    unsigned char *rcvbuf = s->m_AudioRcv;
	const int nExpectedSize =
		(s->m_bRecvRTP ? RTP_AUDIOBUFF_SIZE : AV_BUFFER_SIZE);

	if(s->m_nAudioLen == nExpectedSize)
		s->m_nAudioLen = 0;

	if (s->m_bUseHTTP) {
		status = doHTTPRead ( s->m_nAudioSock,
			(char*)(rcvbuf + s->m_nAudioLen),
			nExpectedSize - s->m_nAudioLen );
	}
	else {
		status = doUDPRead ( s->m_nAudioSock,
			(char*)(rcvbuf + s->m_nAudioLen),
			nExpectedSize - s->m_nAudioLen );
	}

	if(status > 0)
	{
		s->m_nAudioLen += status;
	}

    if (s->m_nAudioLen == nExpectedSize) {

		s->m_nAudioPkts++;
		s->m_nAudioBytes += nExpectedSize;

		if(!s->m_bRecvRTP)
			nSeqNum = V2DGETSEQUENCENUM(rcvbuf);
		else
			nSeqNum = RTPGETSEQUENCENUM(rcvbuf);

		if (s->m_nAudioSeq && (s->m_nAudioSeq != nSeqNum)) {
			if (s->m_nAudioSeq < nSeqNum) {
				s->m_nAudioLoss += (nSeqNum - s->m_nAudioSeq);
				s->m_nAudioSeq = nSeqNum + 1;
			}
			else if (s->m_nAudioSeq - nSeqNum == 65536) {
				s->m_nAudioSeq = nSeqNum + 1;
			}
			else {
				s->m_nAudioOutOfOrder++;
				if (s->m_nAudioOutOfOrder % 10 == 0)
					s->m_nAudioSeq = 0;
			}
		}
		else {
			s->m_nAudioSeq = nSeqNum + 1;
		}

        s->m_nFlag |= I_AUDIO;
        Rx_GetTimeOfDay(s, &s->m_tLastCtlRead);
    }
    return 0;
}

/* Build_RtpFilter - Builds an RTP filter into the buffer
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *   pBuffer - pointer to the buffer for the RTP filter message
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Returns 0 if success, non-zero error code if failure
 */
int Rx_BuildRtpFilter(V2DRx_t *s, char *pBuffer, int nLen)
{
    int len = 0;
    unsigned char *cp;
    int attrlen, attrid, attrval;

    if (s == NULL) {
        return -1;
    }

    memset(pBuffer, 0x00, nLen);
    cp = (unsigned char *) pBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_RTPFILTER);
    cp += 4;
    len += 4;

    /* Video Port and AES */
    if (s->m_nAVOption == AVOPTION_VIDEO || 
		s->m_nAVOption == AVOPTION_AUDIO_VIDEO) {
        attrid = RTPFILTER_IDENT_VPORT;
        attrval = s->m_nVideoPort;
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = RTPFILTER_IDENT_VAES;
        attrval = 0;
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
    }

    /* Audio Port and AES */
    if (s->m_nAVOption == AVOPTION_AUDIO || 
		s->m_nAVOption == AVOPTION_AUDIO_VIDEO) {
        attrid = RTPFILTER_IDENT_APORT;
        attrval = s->m_nAudioPort;
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
        attrid = RTPFILTER_IDENT_AAES;
        attrval = 0;
        attrlen = 2;
        FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
        len += attrlen + 4;
        cp += attrlen + 4;
    }

    /* Multicast */
    if (s->m_nMulticast) {
#ifndef WINDOWS
        struct in_addr inaddr;
        if (inet_aton(s->m_MulticastAddress, &inaddr)) {
            attrid = RTPFILTER_IDENT_MULTICASTADDR;
            attrval = htonl(inaddr.s_addr);
            attrlen = 4;
            FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
            len += attrlen + 4;
            cp += attrlen + 4;
        }
#endif
    }

    cp = (unsigned char *) pBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    return 0;
}

/**
 * Rx_BuildAVReq - Builds an AV request control message from the RX'es
 * m_Creq array and puts it in the send buffer
 *
 * Input:
 *   s -  pointer to the V2DRx_t struct
 *
 * Returns nothing
 */
static void Rx_BuildAVReq(V2DRx_t *s)
{
    unsigned char *cp;
    int len=0;
    int attrlen=0;
    int attrid=0;
    int attrval=0;
    int m_Iter;

    memset(s->m_CtlSnd, 0x00, SEND_BUFFER_SIZE);

    cp = s->m_CtlSnd;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_AVREQ);
    cp+=4;
    len+=4;

    for (m_Iter=0; m_Iter < s->m_nNumCreq; m_Iter++) {
        attrid = s->m_Creq[m_Iter].name;
        attrval = atoi(s->m_Creq[m_Iter].value);
        switch(attrid) {
        case AVREQ_IDENT_AV_BW:
            FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
            attrlen=4;
            break;
        case AVREQ_IDENT_VIDEO_PORT:
        case AVREQ_IDENT_AUDIO_PORT:
        case AVREQ_IDENT_AV_MULTICAST:
        case AVREQ_IDENT_AV_OPTION:
        case AVREQ_IDENT_AUDIO_TYPE:
        case AVREQ_IDENT_PROTOCOL_VERSION:
            FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
            attrlen=2;
            break;
        case AVREQ_IDENT_AUTH_TOKEN:
           {
               char * token = s->m_Creq[m_Iter].value;
               FILL_PROTOCOL_UNIT_STRING(strlen(token) + 1, attrid, 
                       token, cp);
               attrlen = strlen(token) + 1;
               break;
           }
        }
        len += attrlen+4;
        cp += attrlen+4;
    }
    cp = s->m_CtlSnd;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);
}

/**
 * Rx_BuildCStat - Builds a client statistics control message every second
 * and puts it in the RX'es send buffer
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns 0 if not yet time to build stat, 1 otherwise
 */
static int Rx_BuildCStat(V2DRx_t *s)
{
    unsigned char *cp;
    int len=0;
    struct timeval tNow;
    unsigned long delta;
    int retval=0;

    Rx_GetTimeOfDay(s, &tNow);
	GETTIMEDIFF(tNow, s->m_tLastStatistics, delta);

    if (delta < 1000)
        goto EXIT_LABEL;

    s->m_tLastStatistics.tv_sec = tNow.tv_sec;
    s->m_tLastStatistics.tv_usec = tNow.tv_usec;

    s->m_nCurrVideoBytes = s->m_nVideoBytes - s->m_nVideoLastBytes;
    s->m_nCurrVideoRate = (int)(((double)s->m_nCurrVideoBytes*1000.0*8.0)/delta);
    s->m_nCurrVideoPkts = s->m_nVideoPkts - s->m_nVideoLastPkts;
    s->m_nCurrVideoLoss = s->m_nVideoLoss - s->m_nVideoLastLoss;
    s->m_nCurrVideoOutOfOrder = s->m_nVideoOutOfOrder - s->m_nVideoLastOutOfOrder;
    s->m_nVideoLastBytes = s->m_nVideoBytes;
    s->m_nVideoLastPkts = s->m_nVideoPkts;
    s->m_nVideoLastLoss = s->m_nVideoLoss;
    s->m_nVideoLastOutOfOrder = s->m_nVideoOutOfOrder;

    s->m_nCurrAudioBytes = s->m_nAudioBytes - s->m_nAudioLastBytes;
    s->m_nCurrAudioRate = (int)(((double)s->m_nCurrAudioBytes*1000.0*8.0)/delta);
    s->m_nCurrAudioPkts = s->m_nAudioPkts - s->m_nAudioLastPkts;
    s->m_nCurrAudioLoss = s->m_nAudioLoss - s->m_nAudioLastLoss;
    s->m_nCurrAudioOutOfOrder = s->m_nAudioOutOfOrder - s->m_nAudioLastOutOfOrder;
    s->m_nAudioLastBytes = s->m_nAudioBytes;
    s->m_nAudioLastPkts = s->m_nAudioPkts;
    s->m_nAudioLastLoss = s->m_nAudioLoss;
    s->m_nAudioLastOutOfOrder = s->m_nAudioOutOfOrder;

    memset(s->m_CtlSnd, 0x00, SEND_BUFFER_SIZE);
    cp = s->m_CtlSnd;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_CSTAT);
    cp+=4;
    len+=4;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_VIDEORATE, s->m_nVideoRate, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_VIDEOBYTES, s->m_nVideoBytes, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_VIDEOPACKETS, s->m_nVideoPkts, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_VIDEOLOSS, s->m_nVideoLoss, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_VIDEOOOO, s->m_nVideoOutOfOrder, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_AUDIORATE, s->m_nAudioRate, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_AUDIOBYTES, s->m_nAudioBytes, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_AUDIOPACKETS, s->m_nAudioPkts, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_AUDIOLOSS, s->m_nAudioLoss, cp);
    len+= 8;
    cp+= 8;

    FILL_PROTOCOL_UNIT_LONG(CSTAT_IDENT_AUDIOOOO, s->m_nAudioOutOfOrder, cp);
    len+= 8;
    cp+= 8;

    cp = s->m_CtlSnd;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);
    retval = 1;
 EXIT_LABEL:
    return retval;
}

static int Rx_CheckCtlTasks(V2DRx_t *s)
{

    if (s->m_nCtlFlags & RX_SEND_RFB_QUERY) {
        LOG_SYS(V2DLIB_VERBOSE,
			"V2DRX: Creating RFB status query  message for %ld:%ld\n",
			s->m_nSessionId, s->m_nQueryId);
        v2dLibCreateRFBStatusQuery(s->m_CtlSnd, s->m_nSessionId, s->m_nQueryId);
        s->m_nCtlFlags &= ~(RX_SEND_RFB_QUERY);
        return 1;
    }

    if (s->m_nCtlFlags & RX_SEND_FS_REFRESH) {
        LOG_SYS(V2DLIB_VERBOSE, "V2DRX: Creating FS Refresh  message \n");
        v2dLibCreateFSRefresh(s->m_CtlSnd);
        s->m_nCtlFlags &= ~(RX_SEND_FS_REFRESH);
        return 1;
    }

    return 0;
}

/* Send a FSRefresh message to connected server */
void Rx_SendFSRefresh(V2DRx_t *s)
{
    // Schedule an FSRefresh to be sent at the next opportunity
    s->m_nCtlFlags |= RX_SEND_FS_REFRESH;
}

/**
 * Rx_Create2 - RX Constructor that extends Rx_Create so that we
 *				can pass extended options
 * Input:
 *   dest - IP address of TX to connect to
 *   svrport - TCP port on TX to connect to
 *   bw - bandwidth (in bits per second) for the connection
 *   mcast - 1 if connection should be multicast, 0 if not
 *   videoport - UDP port number to receive video data on
 *   audioport - UDP port number to receive audio data on
 *   avoption - whether connection should be video, audio or both
 *   audiotype - audio sample size, rate, stereo/mono
 *   bsrdreqd - if TRUE slice reduction is turned on
 *   httpoption - if TRUE video, audio is received on HTTP
 *   httpport   - if httpoption == TRUE, the HTTP port
 * Returns a pointer to V2DRx_t struct if success, NULL if failure
 */
V2DRx_t *Rx_Create2(char *dest,
					int srvport,
					int bandwidth,
					int multicast,
					int videoport,
					int audioport,
					int avoption,
					int audiotype,
					int bsrdreqd,
					int httpoption,
					int httpport)
{
	V2DRx_t *s=NULL;

	s = Rx_Create(dest,
		     	  srvport,
			      bandwidth,
			      multicast,
			      videoport,
			      audioport,
			      avoption,
			      audiotype,
			      bsrdreqd);

	if(s && httpoption)
	{
		Rx_SetHTTPPort(s, httpport);
		LOG_SYS(V2DLIB_INFO, "V2DRx: HTTP option enabled, port is %d\n",
			httpport);
	}

	return s;
}

/**
 * Rx_Create - RX Constructor
 *
 * Input:
 *   dest - IP address of TX to connect to
 *   svrport - TCP port on TX to connect to
 *   bw - bandwidth (in bits per second) for the connection
 *   mcast - 1 if connection should be multicast, 0 if not
 *   videoport - UDP port number to receive video data on
 *   audioport - UDP port number to receive audio data on
 *   avoption - whether connection should be video, audio or both
 *   audiotype - audio sample size, rate, stereo/mono
 *       bsrdreqd - if TRUE slice reduction is turned on
 *
 * Returns a pointer to V2DRx_t struct if success, NULL if failure
 */
V2DRx_t *Rx_Create (char *dest,
                    int svrport,
                    int bw,
                    int mcast,
                    int videoport,
                    int audioport,
                    int avoption,
                    int audiotype,
                    int bsrdreqd)
{
    V2DRx_t *s=NULL;
    char sBandwidth[80];
    char sMulticast[80];
    char sVideoPort[80];
    char sAudioPort[80];
    char sAVOption[80];
    char sAudioType[80];
    struct timeval tNow;


    s = (struct V2DRx *)malloc (sizeof(struct V2DRx));
    if (!s) return NULL;

    memset((void *)s, 0, sizeof(V2DRx_t));

    s->m_nRxState = CLIENT_STATE_IDLE;

    sprintf(sBandwidth, "%d", bw/8);
    sprintf(sMulticast, "%d", mcast);
    sprintf(sVideoPort, "%u", videoport);
    sprintf(sAudioPort, "%u", audioport);
    sprintf(sAVOption, "%d", avoption);
    sprintf(sAudioType, "%d", audiotype);

    s->m_nBandwidth = bw;
    s->m_nMulticast = mcast;
    s->m_nVideoPort = videoport;
    s->m_nAudioPort = audioport;
    s->m_nAVOption = avoption;
    s->m_nAudioType = audiotype;

    s->m_nSvrPort = svrport;
    s->m_nHTTPPort=0;
    s->m_bUseHTTP=0;

    s->m_recvInterfaceCount = -1;

    /* Determine current time and cache it further use */
     
     GETTIMEOFDAY(&tNow, NULL);
     s->m_current_time = ((uint64_t) tNow.tv_sec) * 1000 + (tNow.tv_usec/1000);


    LOG_SYS(
		V2DLIB_INFO,
		"V2DRX: Creating Rx: bw=%d, mcast=%d, videoport=%u, audioport=%u, \
		avoption=%d, audiotype=%d\n",
		bw, mcast, videoport, audioport, avoption, audiotype);

    s->m_Destination.sin_family = AF_INET;
    s->m_Destination.sin_addr.s_addr=inet_addr(dest);
    s->m_Destination.sin_port = htons(svrport);

    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_AV_BW;
    strcpy (s->m_Creq[s->m_nNumCreq].value, sBandwidth);
    s->m_nNumCreq++;
    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_VIDEO_PORT;
    strcpy (s->m_Creq[s->m_nNumCreq].value, sVideoPort);
    s->m_nNumCreq++;
    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_AUDIO_PORT;
    strcpy (s->m_Creq[s->m_nNumCreq].value, sAudioPort);
    s->m_nNumCreq++;
    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_AV_MULTICAST;
    strcpy (s->m_Creq[s->m_nNumCreq].value, sMulticast);
    s->m_nNumCreq++;
    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_AV_OPTION;
    strcpy (s->m_Creq[s->m_nNumCreq].value, sAVOption);
    s->m_nNumCreq++;
    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_AUDIO_TYPE;
    strcpy (s->m_Creq[s->m_nNumCreq].value, sAudioType);
    s->m_nNumCreq++;

    s->m_CtlSnd = (unsigned char *) malloc(SEND_BUFFER_SIZE);
    s->m_CtlRcv = (unsigned char *) malloc(SEND_BUFFER_SIZE);
    s->m_VideoRcv = (unsigned char *) malloc(200*AV_BUFFER_SIZE);
    s->m_AudioRcv = (unsigned char *) malloc(200*AV_BUFFER_SIZE);
    s->m_RedVideoRcv = NULL;
    s->sliceRdata = NULL;
    s->m_CtlRcvLength = 0;
    s->m_nResetCount = 0;
    s->m_version = V2D_PROTOCOL_VERSION(1,0);
    s->m_nSessionId = v2dLibGetRandomNumber();
    s->m_nQueryId = 1;
    s->m_bRFBStatus = FALSE;
    s->m_nCtlFlags |= RX_SEND_RFB_QUERY;
    s->m_statuscode = STATUSCODE_CLEAR;
    s->m_nReconnectPeriod = 125;

    if(bsrdreqd)
    {
        s->m_bSliceRed = bsrdreqd;
        s->m_RedVideoRcv = (unsigned char *) malloc(200*AV_BUFFER_SIZE);
        s->sliceRdata = (sliceRedData_t *)malloc(sizeof(sliceRedData_t));
        if (s->sliceRdata != NULL) {
            if (initSRD(s->sliceRdata) < 0) {
                destroySRD(s->sliceRdata);
                free(s->sliceRdata);
                s->sliceRdata = NULL;
            }
        }
    }

    Rx_GetTimeOfDay(s, &s->m_tLastStatistics);
    Rx_GetTimeOfDay(s, &s->m_tLastCtlRead);

    s->m_bRecvRTP = FALSE;

    if ( v2dLibStartup() )
    {
        s->m_nErrCode   = getReturnCode( RC_CRITICAL_ERR );
        V2DSENDERROR(s);
    }


    return s;
}

/**
 * Rx_Destroy - RX Destructor
 *
 * Input:
 *   s - pointer to V2DRx_t struct to be destroyed
 *
 * Returns nothing
 */
void Rx_Destroy(V2DRx_t *s)
{

    if ( s->m_nVideoSock > 0 )
        v2dLibCloseSocket( s->m_nVideoSock );

    if ( s->m_nAudioSock > 0 )
        v2dLibCloseSocket( s->m_nAudioSock );

    if ( s->m_nCtlSock > 0 )
        v2dLibCloseSocket( s->m_nCtlSock );

    s->m_nCtlSock = s->m_nVideoSock = s->m_nAudioSock = 0;


    if (s->m_CtlSnd) free(s->m_CtlSnd);
    if (s->m_CtlRcv) free(s->m_CtlRcv);
    if (s->m_VideoRcv) free(s->m_VideoRcv);
    if (s->m_AudioRcv) free(s->m_AudioRcv);
    if (s->m_RedVideoRcv) free(s->m_RedVideoRcv);

    s->m_CtlSnd = s->m_CtlRcv = s->m_VideoRcv = s->m_AudioRcv = (s->m_RedVideoRcv) = NULL;

    if (s->sliceRdata != NULL) {
        destroySRD(s->sliceRdata);
        free(s->sliceRdata);
    }
    free(s);

    v2dLibCleanup();

}

int Rx_SetHTTPPort(V2DRx_t *s, int nHTTPPort)
{
    s->m_nHTTPPort = nHTTPPort;
    s->m_bUseHTTP=1;
    s->m_Destination.sin_port = htons(nHTTPPort);
    return 0;
}

int Rx_SetRTPMode(V2DRx_t *s, const char * multicastAddress)
{
    unsigned long dstaddr = 0;
    struct in_addr addr;

    if (multicastAddress == NULL || strlen(multicastAddress) <= 0)
        return -1;

    // If session was created multicast, then IP address better be multicast
#ifdef WINDOWS
    addr.s_addr = inet_addr(multicastAddress);
#else
    if (inet_aton(multicastAddress, &addr) == 0)
        return -1;
#endif

    dstaddr = ntohl(addr.s_addr);

    if (dstaddr > 0xE0000000 && dstaddr <= 0xEFFFFFFF) {
        if (s->m_nMulticast == 1)
            strncpy(s->m_MulticastAddress, multicastAddress, 31);
    }
    else {
        if (s->m_nMulticast == 1)
            return -1;
    }
    s->m_nPureRTPMode = 1;
    s->m_bRecvRTP = TRUE;

    return 0;
}

/**
 * Rx_Go - State machine for the V2D receiver. It sends and processes
 * control messages to/from the V2D transmitter as appropriate for the
 * state it's in. It sets the I_CTL, I_VIDEO or I_AUDIO flag bits when
 * data has been read into the control, video or audio receive buffers,
 * it also writes the data in the control send buffer to the network
 * if the O_CTL, O_KB, O_MS or O_SER bit is set by the caller. Rx_Go
 * will clear the O_CTL etc bits once the data has been transmitted.
 * Similarly, the caller should clear the I_CTL etc bits when the data
 * has been processed. The video and audio data should be 1040 bytes
 * long starting with 8 bytes of audio and 8 bytes of video (or 8 bytes
 * of video and 8 bytes of audio) sequence number followed by 1024
 * bytes of actual data.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns number of read/write/connect operations performed if no
 * error or non-zero code if error or a server reject is received.
 */
int Rx_Go(V2DRx_t *s)
{
    struct timeval timeout;
    int nDescriptors, nMaxfd=-1, nAVOption=AVOPTION_VIDEO;
    fd_set read_fdset, write_fdset;
    int retval=0;
    int m_Iter;

    /* Determine current time and cache it further use */
    struct timeval tNow;
    GETTIMEOFDAY(&tNow, NULL);
    s->m_current_time = ((uint64_t) tNow.tv_sec) * 1000 + (tNow.tv_usec/1000);

    if (s->m_nRxState == CLIENT_STATE_DISABLED)
        return retval;

    /* If we are connected but no control packet has arrived for the
       past 20 seconds, we disconnect and reset */
    if (((s->m_nRxState == CLIENT_STATE_CONNECTED) ||
        (s->m_nRxState == CLIENT_STATE_RECVHTTPRESP) ||
        (s->m_nRxState == CLIENT_STATE_READY)) &&
            s->m_nPureRTPMode == 0) {

        unsigned long delta;

        Rx_GetTimeOfDay(s, &tNow);
        GETTIMEDIFF(tNow, s->m_tLastCtlRead, delta);

        if (delta > 20000) {
            Rx_Reset(s);
            s->m_nErrCode = RETCODE(RC_SERVER_TIMEDOUT, 0);
            V2DSENDERROR(s);
            goto EXIT_LABEL;
        }
    }

    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    timeout.tv_sec =  0;
    timeout.tv_usec = 0;

    if (s->m_nCtlSock > 0) {
        FD_SET ((unsigned int)s->m_nCtlSock, &read_fdset);
        FD_SET ((unsigned int)s->m_nCtlSock, &write_fdset);
        if (nMaxfd <= s->m_nCtlSock)
            nMaxfd = s->m_nCtlSock+1;
    }
    if (s->m_nVideoSock > 0) {
        FD_SET ((unsigned int)s->m_nVideoSock, &read_fdset);
        if (nMaxfd <= s->m_nVideoSock)
            nMaxfd = s->m_nVideoSock+1;
        if (s->m_nVideoFlags == AV_HTTP_REQUEST) {
            FD_SET ((unsigned int)s->m_nVideoSock, &write_fdset);
        }
    }
    if (s->m_nAudioSock > 0) {
        FD_SET ((unsigned int)s->m_nAudioSock, &read_fdset);
        if (nMaxfd <= s->m_nAudioSock)
            nMaxfd = s->m_nAudioSock+1;
        if (s->m_nAudioFlags == AV_HTTP_REQUEST) {
            FD_SET ((unsigned int)s->m_nAudioSock, &write_fdset);
        }
    }
    for (m_Iter=0; m_Iter < s->m_nNumCreq; m_Iter++) {
        if (s->m_Creq[m_Iter].name== AVREQ_IDENT_AV_OPTION) {
            nAVOption = atoi(s->m_Creq[m_Iter].value);
        }
    }
    nDescriptors = select(nMaxfd+1, &read_fdset, &write_fdset,
                          NULL, &timeout);

#ifdef WINDOWS
    // exit if Rx has connected and no socket is ready
    if (  nDescriptors <= 0 && s->m_nPureRTPMode == 0
          && s->m_nRxState != CLIENT_STATE_IDLE
          && s->m_nRxState != CLIENT_STATE_CONNECTING
        )
    {
        WSAGetLastError();
        goto EXIT_LABEL;
    }

    if (nDescriptors <= 0 && s->m_nPureRTPMode == 1) {
        if (!(s->m_nRxState == CLIENT_STATE_IDLE ||
            s->m_nRxState == CLIENT_STATE_CONNECTED))
        {
            WSAGetLastError();
            goto EXIT_LABEL;
        }
    }
#else
    if (nDescriptors <= 0 && s->m_nPureRTPMode == 0)
        goto EXIT_LABEL;

    if (nDescriptors <= 0 && s->m_nPureRTPMode == 1) {
        if (!(s->m_nRxState == CLIENT_STATE_IDLE ||
                s->m_nRxState == CLIENT_STATE_CONNECTED))
            goto EXIT_LABEL;
    }

#endif

    switch (s->m_nRxState) {
        int rc;
    case CLIENT_STATE_IDLE:

        if (s->m_nPureRTPMode) {
            s->m_nRxState = CLIENT_STATE_CONNECTED;
            s->m_statuscode = STATUSCODE_AUTHENTICATING;
            break;
        }

#ifndef WINDOWS
        // on windows, stream socket will not be ready
        // until connected
        if ( FD_ISSET(s->m_nCtlSock, &write_fdset) )
#endif
        {
            if ((rc=Rx_CtlConnect(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s);
                break;
            }
            retval += rc;
        }
        break;
    case CLIENT_STATE_CONNECTING:
        if ((rc=Rx_CheckCtlConnect(s)) != 0) {
            s->m_nErrCode = rc;
            V2DSENDERROR(s);
        }
        break;
    case CLIENT_STATE_SENDHTTPREQ:
        if (FD_ISSET(s->m_nCtlSock, &write_fdset)) {
            if ((rc=Rx_CtlHTTPRequest(s)) != 0) {
                Rx_Reset(s);
                s->m_nErrCode = rc;
                V2DSENDERROR(s);
            }
            else {
                s->m_nRxState = CLIENT_STATE_RECVHTTPRESP;
                retval++;
            }
        }
        break;
    case CLIENT_STATE_RECVHTTPRESP:
        if (FD_ISSET(s->m_nCtlSock, &read_fdset)) {
            rc=Rx_CtlHTTPResponse(s);
            if (rc > 0) {
                Rx_Reset(s);
                s->m_nErrCode = rc;
                V2DSENDERROR(s);
            }
            else if (rc == 0) {
                s->m_nRxState = CLIENT_STATE_CONNECTED;
                s->m_statuscode = STATUSCODE_AUTHENTICATING;
                retval++;
            }
        }
        break;
    case CLIENT_STATE_CONNECTED:
        if (s->m_nPureRTPMode) {
            // In Pure RTP mode we directly create the UDP sockets to receive
            // video and audio.
            if (nAVOption != AVOPTION_AUDIO)
            {
                if ((rc = Rx_CreateVideoSocket(s))!=0) {
                    Rx_Reset(s);
                    Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
            }
            if (nAVOption != AVOPTION_VIDEO)
            {
                if ((rc = Rx_CreateAudioSocket(s))!=0) {
                    Rx_Reset(s);
                    Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
            }

            s->m_nRxState = CLIENT_STATE_READY;
            s->m_statuscode = STATUSCODE_CONNECTED;
            break;
        }
        if (FD_ISSET(s->m_nCtlSock, &read_fdset)) {
            if ((rc = Rx_CtlRead(s)) == 0) {
                // need to wait in the same state
            }
            else if (rc < 0) {
                Rx_Reset(s);
                Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                s->m_nErrCode = rc == -1 ? getReturnCode( RC_READERROR ) :
                    RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
                s->m_statuscode = STATUSCODE_PROTERR;
                V2DSENDERROR(s);
            }
            else if ((rc = Rx_CtlProcess(s)) != 0) {
                Rx_Reset(s);
                Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                s->m_nErrCode = rc;
                V2DSENDERROR(s);
            }
            else {
                s->m_nRxState = CLIENT_STATE_SEND_DATA_REQ;
                retval++;
            }
        }
        break;

    case CLIENT_STATE_SEND_DATA_REQ:
        if (FD_ISSET(s->m_nCtlSock, &write_fdset)) {
            if (nAVOption != AVOPTION_AUDIO)
            {
                if ((rc = Rx_CreateVideoSocket(s))!=0) {
                    Rx_Reset(s);
                    Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
            }
            if (nAVOption != AVOPTION_VIDEO)
            {
                if ((rc = Rx_CreateAudioSocket(s))!=0) {
                    Rx_Reset(s);
                    Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
            }
            Rx_BuildAVReq(s);
            if ((rc = Rx_CtlWrite(s)) != 0) {
                s->m_nErrCode = rc;
                V2DSENDERROR(s);
                break;
            }
            s->m_nRxState = CLIENT_STATE_READY;
			s->m_statuscode = STATUSCODE_CONNECTED;
            retval++;
        }
        break;
    case CLIENT_STATE_READY:
        if (!(s->m_nFlag & (I_CTL|I_SER)) &&
                s->m_nCtlSock > 0 && FD_ISSET(s->m_nCtlSock, &read_fdset)) {
            if ((rc = Rx_CtlRead(s)) == 0) {
                // need to wait in the same state
            }
            else if (rc < 0) {
                Rx_Reset(s);
                Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                s->m_nErrCode = rc == -1 ? getReturnCode( RC_READERROR ) :
                    RETCODE(RC_PROTO_MALFORMED_PACKET, 0);
                s->m_statuscode = STATUSCODE_PROTERR;
                V2DSENDERROR(s);
            }
            else if ((rc = Rx_CtlProcess(s)) != 0)
            {
                Rx_Reset(s);
                Rx_GetTimeOfDay(s, &s->m_tLastFailedConnect);
                s->m_nErrCode = rc;
                V2DSENDERROR(s);
                break;
            }
            else retval++;
        }
        else if (s->m_nVideoFlags)
        {
            switch(s->m_nVideoFlags)
            {
                case AV_HTTP_CONNECT:
                    if (!Rx_CheckVideoConnect(s)) {
                        s->m_nVideoFlags = AV_HTTP_REQUEST;
                    }
                    break;
                case AV_HTTP_REQUEST:
                    if (FD_ISSET(s->m_nVideoSock,&write_fdset)) {
                        if (!Rx_VideoHTTPRequest(s)) {
                            s->m_nVideoFlags = AV_HTTP_RESPONSE;
                        }
                    }
                    break;
                case AV_HTTP_RESPONSE:
                    if (FD_ISSET(s->m_nVideoSock,&read_fdset)) {
                        if (!Rx_VideoHTTPResponse(s)) {
                            s->m_nVideoFlags = 0;
                        }
                    }
            }
        }
        else if (s->m_nAudioFlags)
        {
            switch(s->m_nAudioFlags)
            {
                case AV_HTTP_CONNECT:
                    if (!Rx_CheckAudioConnect(s)) {
                        s->m_nAudioFlags = AV_HTTP_REQUEST;
                    }
                    break;
                case AV_HTTP_REQUEST:
                    if (FD_ISSET(s->m_nAudioSock,&write_fdset)) {
                        if (!Rx_AudioHTTPRequest(s)) {
                            s->m_nAudioFlags = AV_HTTP_RESPONSE;
                        }
                    }
                    break;
                case AV_HTTP_RESPONSE:
                    if (FD_ISSET(s->m_nAudioSock,&read_fdset)) {
                        if (!Rx_AudioHTTPResponse(s)) {
                            s->m_nAudioFlags = 0;
                        }
                    }
            }
        }
        else {
            if (!(s->m_nFlag & I_AUDIO) &&
                FD_ISSET(s->m_nAudioSock,&read_fdset)) {
                if ((rc=Rx_AudioRead(s)) != 0) {
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
                retval++;
            }
            //If slice reduction flag on , just keep on reading, otherwise
            //check for flag.
            else if(s->m_bSliceRed)
            {
                if (FD_ISSET(s->m_nVideoSock,&read_fdset)) {
                    if ((rc=Rx_VideoRead(s)) != 0) {
                        s->m_nErrCode = rc;
                        V2DSENDERROR(s);
                        break;
                    }
                    retval++;
                }

                if (!(s->m_nFlag & I_VIDEO)) {
                    if (s->sliceRdata->IFrameCount > 0
                        || s->sliceRdata->inFrameCount > 1) {
                            if (get_hoFrame(s->sliceRdata) != -1)
                            {
                                s->m_nFlag |= I_VIDEO;
                                retval++;
                            }
                    }
                }
            }
            else
            {

                if (!(s->m_nFlag & I_VIDEO) &&
                    FD_ISSET(s->m_nVideoSock,&read_fdset)) {
                    if ((rc=Rx_VideoRead(s)) != 0) {
                        s->m_nErrCode = rc;
                        V2DSENDERROR(s);
                        break;
                    }
                    retval++;
                }

            }
        }
        if (s->m_nCtlSock > 0 && FD_ISSET(s->m_nCtlSock, &write_fdset)) {
            if (s->m_nFlag & (O_CTL|O_KB|O_MS|O_SER)) {
                s->m_nFlag &= ~(O_CTL|O_KB|O_MS|O_SER);
                if ((rc=Rx_CtlWrite(s)) != 0) {
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
                else {
                    retval++;
                }
            }
            else if (Rx_CheckCtlTasks(s)) {
                if ((rc=Rx_CtlWrite(s)) != 0) {
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
                else {
                    retval++;
                }
            }
            else if (Rx_BuildCStat(s)) {
                if ((rc=Rx_CtlWrite(s)) != 0) {
                    s->m_nErrCode = rc;
                    V2DSENDERROR(s);
                    break;
                }
                retval++;
            }
        }
        if (s->m_nPureRTPMode == 1) {
            Rx_BuildCStat(s);
        }
        break;
    default:
        break;
    }
 EXIT_LABEL:
    return retval;
}

/**
 * Rx_OpenMediaConnection - Create a control socket for the RX so it can
 * start running the RX state machine.
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns 0 if success, non-zero code if failure
 */
int Rx_OpenMediaConnection(V2DRx_t *s) {

    if (s->m_nPureRTPMode) {
        s->m_nCtlSock = 0;
        return 0;
    }

    s->m_nVideoSeq = 0;
    s->m_nVideoPkts = 0;
    s->m_nVideoBytes = 0;
    s->m_nVideoLoss = 0;
    s->m_nVideoRate = 0;
    s->m_nVideoOutOfOrder = 0;
    s->m_nVideoLastBytes = 0;
    s->m_nVideoLastPkts = 0;
    s->m_nVideoLastLoss = 0;
    s->m_nVideoLastRate = 0;
    s->m_nVideoLastOutOfOrder = 0;

    s->m_nAudioSeq = 0;
    s->m_nAudioPkts = 0;
    s->m_nAudioBytes = 0;
    s->m_nAudioLoss = 0;
    s->m_nAudioRate = 0;
    s->m_nAudioOutOfOrder = 0;
    s->m_nAudioLastBytes = 0;
    s->m_nAudioLastPkts = 0;
    s->m_nAudioLastLoss = 0;
    s->m_nAudioLastRate = 0;
    s->m_nAudioLastOutOfOrder = 0;


    s->m_nCtlSock = socket (AF_INET, SOCK_STREAM, 0);
    if (s->m_nCtlSock <= 0)
        return getReturnCode( RC_SOCKERR );

    return 0;
}


/**
 * Rx_CloseMediaConnection - Closes all control and data connections for the RX
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *
 * Returns 0 always
 */
int Rx_CloseMediaConnection(V2DRx_t *s)
{
    if (s->m_nCtlSock > 0) {
        v2dLibCloseSocket(s->m_nCtlSock);
        s->m_nCtlSock = 0;
    }
    if (s->m_nVideoSock > 0) {
        v2dLibCloseSocket(s->m_nVideoSock);
        s->m_nVideoSock = 0;
    }
    if (s->m_nAudioSock > 0) {
        v2dLibCloseSocket(s->m_nAudioSock);
        s->m_nAudioSock = 0;
    }

    return 0;
}

/**
 * Rx_PrintStatistics - Prints a line of client statistics into the buffer
 * pointed to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *   pBuffer - pointer to the buffer for client statistics
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Return nothing
 */
void Rx_PrintStatistics(V2DRx_t *s, char *pBuffer, int nLen)
{
    if (!s) return;

    if (s->m_nRxState != CLIENT_STATE_READY) {
        *pBuffer = '\0';
    }
    else {
        snprintf(pBuffer, nLen,
                 "\t(V) r:%d b:%d p:%d l:%d o:%d (A) r:%d b:%d p:%d l:%d o:%d\n"
                 "\t(S) b:%d (State) %d (Dest) %s",
                 s->m_nCurrVideoRate, s->m_nCurrVideoBytes,
                 s->m_nCurrVideoPkts, s->m_nCurrVideoLoss,
                 s->m_nCurrVideoOutOfOrder, s->m_nCurrAudioRate,
                 s->m_nCurrAudioBytes, s->m_nCurrAudioPkts,
                 s->m_nCurrAudioLoss, s->m_nCurrAudioOutOfOrder,
                 s->m_nSerialRcvd, s->m_nRxState,
                 inet_ntoa(s->m_Destination.sin_addr));
    }
}

/**
 * Rx_SendNotification - Prints a notification message into the buffer
 * pointed to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *   pBuffer - pointer to the buffer for the notification
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Return nothing
 */
void Rx_SendNotification(V2DRx_t *s, char *pBuffer, int nLen)
{
    if (s && s->m_bSendNotify) {
        snprintf(pBuffer, nLen, "s:%d", s->m_nRxState);
        s->m_bSendNotify = FALSE;
    }

}

/**
 * Rx_SendError - Prints an error message into the buffer
 * pointed to by pBuffer
 *
 * Input:
 *   s - pointer to the V2DRx_t struct
 *   pBuffer - pointer to the buffer for the notification
 *   nLen - number of bytes in the buffer pointed to by pBuffer
 *
 * Return nothing
 */
void Rx_SendError(V2DRx_t *s, char *pBuffer, int nLen)
{
    if (s && s->m_bSendError) {
        snprintf(pBuffer, nLen, "error: code %d, errno %d",
                (s->m_nErrCode&0xffff0000)>>16, s->m_nErrCode&0xffff);
        s->m_bSendError = FALSE;
    }

}

int Rx_GetRFBStatus(V2DRx_t *s)
{
    return s->m_bRFBStatus;
}


void Rx_SendMouseEvent(V2DRx_t *s,
                       unsigned short x, unsigned short y,
                       unsigned char buttonmask)
{
    /* Create the packet */
    (void) v2dLibCreateAbsoluteMouse(s->m_CtlSnd, 
                                     s->m_nSessionId, 
                                     x, 
                                     y, 
                                     buttonmask);

    s->m_RFBMouseSent++;
}

void Rx_SendKeyboardEvent(V2DRx_t *s, unsigned long key, int down)
{
    /* Create the packet */
    (void) v2dLibCreateAbsoluteKeyboard(s->m_CtlSnd, 
                                        s->m_nSessionId,
                                        key,
                                        down);
    s->m_RFBKeyboardSent++;

}

void Rx_SetAuthToken(V2DRx_t *s, const char * authToken)
{
    if (strlen(authToken) == 0 || strlen(authToken) > 64) {
        strcpy(s->m_sAuthToken, "");
        return;
    }
    strcpy(s->m_sAuthToken, authToken);
    s->m_Creq[s->m_nNumCreq].name = AVREQ_IDENT_AUTH_TOKEN;
    strcpy (s->m_Creq[s->m_nNumCreq].value, s->m_sAuthToken);
    s->m_nNumCreq++;
}

const char * Rx_GetAuthToken(V2DRx_t *s)
{
    return s->m_sAuthToken;
}
