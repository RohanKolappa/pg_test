

#include "portability.h"

#include "sockets.h"


/*
 * Resolve hostname and create a sockaddr_in structure for use in
 * connect call
 */
static int 
CreateSocketAddress(const char *a_host, int a_port, 
                    struct sockaddr_in *a_addr) {
    
    unsigned short port = (unsigned short) a_port;
    a_addr->sin_port = htons(port);

    struct addrinfo hints, *result = NULL;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_flags |= AI_NUMERICHOST;

    if (getaddrinfo (a_host, 0, &hints, &result) != 0)
        return -1;

    struct addrinfo *r;
    int valid = 0;
    for (r = result; r != NULL; r= result->ai_next) {
        if (r->ai_family == AF_INET) {
            memcpy(a_addr, r->ai_addr,
                   r->ai_addrlen);
            a_addr->sin_port = htons(port);
            valid = 1;
            break;
        }
    }
    freeaddrinfo (result);
    if (valid)
        return 0;
    return -1;

}


/*
 * Initiate a non-blocking TCP connection to the VNC server
 */
int
TCPConnect(RFBClient_t *client)
{
    RFBASSERT(client != NULL);
    RFBASSERT(client->m_fd == -1);
    RFBASSERT(client->m_server != NULL);
    RFBASSERT(client->m_port > 0);

    struct sockaddr_in addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd <= 0) {
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }

    // Make socket non blocking 
#ifdef WINDOWS
    DWORD dwNonBlocking = 1;
    if ( ioctlsocket( sockfd, FIONBIO, (u_long FAR*)&dwNonBlocking ) ) {
        close (sockfd);
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }
#else
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        close (sockfd);
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }
#endif
    // Resolve hostname
    if (CreateSocketAddress(client->m_server, client->m_port, &addr) == -1) {
        close (sockfd);
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }

    // Initiate connection
    int rc = connect(sockfd, (struct sockaddr *) &addr,
                         sizeof(addr));

    // If connect successful
#ifdef WINDOWS
    DWORD dwErr = WSAGetLastError();
    if (rc != -1 || dwErr == WSAEWOULDBLOCK) {
#else
    if (rc != -1 || errno == EINPROGRESS) {
#endif
        // Disable Nagling
        int one = 1;
        if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                       (char *)&one, sizeof(one)) < 0) {
            close(sockfd);
            return -1;
        }
        
        client->m_fd = sockfd;
        return RFB_CLIENT_RC_SUCCESS;
    }

    // If connect failed
    close(sockfd);
    return RFB_CLIENT_RC_NETWORK_ERROR;
}

/*
 * Check if we get connected after a "connect" call on out socket
 */
int
CheckTCPConnection(RFBClient_t *client) {
    RFBASSERT(client != NULL);
    RFBASSERT(client->m_fd > 0);
    
    // Check is the socket is writable yet.
    fd_set write_fdset;
#ifdef WINDOWS
    // On windows, we use except fdset to detect failed connection
    fd_set except_fdset;
#endif
    struct timeval timeout;
    FD_ZERO(&write_fdset);
    FD_SET (client->m_fd, &write_fdset);
#ifdef WINDOWS
    FD_ZERO(&except_fdset);
    FD_SET (client->m_fd, &except_fdset);
#endif
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

#ifdef WINDOWS
    if (select(client->m_fd + 1, NULL, &write_fdset, &except_fdset, &timeout) == -1) {
#else
    if (select(client->m_fd + 1, NULL, &write_fdset, NULL, &timeout) == -1) {
#endif
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }

#ifdef WINDOWS
    if (FD_ISSET(client->m_fd, &except_fdset)) {
        return RFB_CLIENT_RC_SERVER_UNAVAILABLE;
    }
#endif

    if (!(FD_ISSET(client->m_fd, &write_fdset))) {
        // Socket not yet ready... still trying to connect
        return RFB_CLIENT_RC_WAIT;
    }

#ifdef WINDOWS
    return RFB_CLIENT_RC_SUCCESS;
#else
    // Socket is writable, check error condition on socket. If no
    // error, connect succeeded. 
    int sockoptval = 0;
    int sockoptlen = sizeof(sockoptval);
    if ((getsockopt(client->m_fd, SOL_SOCKET, SO_ERROR,
                    &sockoptval, (socklen_t *)&sockoptlen) == 0) &&
        (sockoptval == 0)) {
        
        return RFB_CLIENT_RC_SUCCESS;
    }

    // Server not available (TCP reset)
    return RFB_CLIENT_RC_SERVER_UNAVAILABLE;
#endif
}
