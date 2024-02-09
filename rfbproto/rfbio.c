
#include "portability.h"
#include "rfbio.h"


/*
 * Print out the contents of a packet for debugging.
 */

void
PrintInHex(char *buf, int len)
{
    int i, j;
    char c, str[17];

    str[16] = 0;

    fprintf(stderr, "ReadExact: ");

    for (i = 0; i < len; i++) {
        if ((i % 16 == 0) && (i != 0)) {
            fprintf(stderr, "           ");
        }
        c = buf[i];
        str[i % 16] = (((c > 31) && (c < 127)) ? c : '.');
        fprintf(stderr, "%02x ", (unsigned char) c);
        if ((i % 4) == 3)
            fprintf(stderr, " ");
        if ((i % 16) == 15) {
            fprintf(stderr, "%s\n", str);
        }
    }
    if ((i % 16) != 0) {
        for (j = i % 16; j < 16; j++) {
            fprintf(stderr, "   ");
            if ((j % 4) == 3)
                fprintf(stderr, " ");
        }
        str[i % 16] = 0;
        fprintf(stderr, "%s\n", str);
    }

    fflush(stderr);
}


static int
ReadData(RFBClient_t * client)
{

    RFBASSERT(client != NULL);
    RFBASSERT(client->m_fd > 0);
    
    // Check if the socket is readable yet.
    int nfds;
    fd_set read_fdset;
    struct timeval timeout;
    FD_ZERO(&read_fdset);
    FD_SET(client->m_fd, &read_fdset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if ((nfds = select(client->m_fd + 1, &read_fdset, NULL, NULL, &timeout)) == -1) {
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }

    if (nfds == 0) {
        // Socket not yet ready...
        return RFB_CLIENT_RC_WAIT;
    }
    if (!(FD_ISSET(client->m_fd, &read_fdset))) {
        // Socket not yet ready...
        return RFB_CLIENT_RC_WAIT;
    }

    LOG_RFB("RFB: Socket ready for regular read. Buffered = %d, Room = %d\n",
            client->m_buffered, RFB_BUF_SIZE - client->m_buffered );

#ifdef WINDOWS
    int rc = recv(client->m_fd,
    		(char *)(client->m_readBuf + client->m_buffered),
            RFB_BUF_SIZE - client->m_buffered, 0);
#else
    int rc = read(client->m_fd, 
                  client->m_readBuf + client->m_buffered, 
                  RFB_BUF_SIZE - client->m_buffered);
#endif

    if (rc <= 0) {
        if (rc < 0) {
            LOG_RFB("RFB: Read Failed: Bad Select??\n");
            return RFB_CLIENT_RC_NETWORK_ERROR;
        } else {
            LOG_RFB("RFB: Read Failed: closed\n");
            return RFB_CLIENT_RC_REMOTE_CLOSED;
        }
    }
    LOG_RFB("RFB: Finished regular read: Read %d\n",  rc);

#if 0
    // Comment this in to debug incoming messages
    {
        int i;
        char *cp;
        char buf[1024];
        int len;
        cp = client->m_readBuf + client->m_buffered;
        len = sprintf(buf, "RFB: Got: ");
        for (i = 0; i < rc; i++) {
            if (isalnum(*cp))
                len += sprintf(buf+len, "-%c-", *cp);
            else
                len += sprintf(buf+len, "-%02x-", *cp);
            cp++;
            if (len >= 1023)
                break;
        }
        buf[len] = '\n';
        buf[len+1] = 0;
        LOG_RFB(buf);
    }
#endif

    client->m_buffered += rc;
    client->m_bytesRead += rc;
    client->m_packetsRead++;
    return RFB_CLIENT_RC_SUCCESS;
}

int
FlushIO(RFBClient_t * client)
{

    RFBASSERT(client != NULL);
    RFBASSERT(client->m_fd > 0);


    // Check is the socket is ready  yet.
    fd_set read_fdset, write_fdset;
    struct timeval timeout;
    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    // Always check if we have anything to read
    FD_SET (client->m_fd, &read_fdset);

    // If we have anything to write, include fd in write_fdset
    if (client->m_bytesToWrite > 0) {
        FD_SET (client->m_fd, &write_fdset);
    }
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(client->m_fd + 1, &read_fdset, 
               &write_fdset, NULL, &timeout) == -1) {
        return RFB_CLIENT_RC_NETWORK_ERROR;
    }


    if (!(FD_ISSET(client->m_fd, &read_fdset) ||
          FD_ISSET(client->m_fd, &write_fdset))) {
        // Socket not yet ready...
        return RFB_CLIENT_RC_WAIT;
    }

    // If any read pending, we read as long as there is room in our buffer
    if (FD_ISSET(client->m_fd, &read_fdset) &&
            client->m_buffered < RFB_BUF_SIZE) {
        LOG_RFB("RFB: Socket ready for read! Buffered = %d, Room = %d\n",
                client->m_buffered, RFB_BUF_SIZE - client->m_buffered );
#ifdef WINDOWS
    int rc = recv(client->m_fd,
    		(char *)(client->m_readBuf + client->m_buffered),
            RFB_BUF_SIZE - client->m_buffered, 0);
#else
        int rc = read(client->m_fd, 
                      client->m_readBuf + client->m_buffered, 
                      RFB_BUF_SIZE - client->m_buffered);
#endif
        
        LOG_RFB("RFB: Forced to read: Read %d\n", rc);
        
        if (rc <= 0) {
            if (rc < 0) {
                LOG_RFB("RFB: Forced Read Failed: Bad Select??\n");
                return RFB_CLIENT_RC_NETWORK_ERROR;
            } else {
                LOG_RFB("RFB: Forced Read Failed: closed\n");
                return RFB_CLIENT_RC_REMOTE_CLOSED;
            }
        }
        client->m_buffered += rc;
        client->m_bytesRead += rc;
        client->m_packetsRead++;
        return RFB_CLIENT_RC_WAIT;
    }

    if (FD_ISSET(client->m_fd, &write_fdset) && client->m_bytesToWrite > 0) {
#ifdef WINDOWS
    	int rc = send(client->m_fd, (char *)client->m_writePtr, client->m_bytesToWrite, 0);
#else
        int rc = write(client->m_fd, client->m_writePtr, client->m_bytesToWrite);
#endif
        LOG_RFB("RFB: Need to write: %d, Written %d\n", client->m_bytesToWrite, rc);
        
        if (rc <= 0) {
            if (rc < 0) {
                LOG_RFB("RFB: Write Failed: Bad Select??\n");
                return RFB_CLIENT_RC_NETWORK_ERROR;
            } else {
                LOG_RFB("RFB: Write Failed: closed\n");
                return RFB_CLIENT_RC_NETWORK_ERROR;
            }
        }
        client->m_bytesToWrite -= rc;
        client->m_writePtr += rc;
        client->m_bytesWritten += rc;
        return RFB_CLIENT_RC_SUCCESS;
    }

    RFBASSERT(0);
    return RFB_CLIENT_RC_WAIT;
}

void
RFBResetIO(RFBClient_t * client)
{
    client->m_buffered = 0;
    client->m_bytesToWrite = 0;
    client->m_bytesToRead = 0;
    client->m_writePtr = client->m_writeArea;
    client->m_bytesWritten = 0;
    client->m_bytesRead = 0;
    client->m_maxBytesToWrite = 0;
}

void
RFBResetReadArea(RFBClient_t * client)
{
    client->m_readArea = client->m_fixedArea;
    client->m_readAreaLength = RFB_BUF_SIZE;
}

void
RFBSetReadArea(RFBClient_t * client, unsigned char *readArea, int readAreaLength)
{
    client->m_readArea = readArea;
    client->m_readAreaLength = readAreaLength;
}

int
RFBReadFromServer(RFBClient_t * client, unsigned int bytes_to_read)
{
    // First call?
    if (client->m_bytesToRead == 0) {
        RFBASSERT(bytes_to_read > 0);
        RFBASSERT(bytes_to_read <= client->m_readAreaLength);
        LOG_RFB("RFB: Read Request: %d, Available %d\n",
                bytes_to_read, client->m_buffered);
        client->m_bytesToRead = bytes_to_read;
        client->m_readPtr = client->m_readArea;
    }

    // If we do not have all the bytes needed, issue a read
    if (client->m_bytesToRead > client->m_buffered &&
            client->m_buffered == 0) {
        //LOG_RFB("RFB: Need to read: %d\n", client->m_bytesToRead);
        int rc = ReadData(client);
        if (rc == RFB_CLIENT_RC_SUCCESS) {

        }
        else {
            return rc; // either wait or error
        }
    }

    // Do we already have enough in our buffer?
    if (client->m_bytesToRead <= client->m_buffered) {
        memcpy(client->m_readPtr, client->m_readBuf, client->m_bytesToRead);
        client->m_buffered -= client->m_bytesToRead;
        memmove(client->m_readBuf, client->m_readBuf + client->m_bytesToRead,
                client->m_buffered);
        client->m_bytesToRead = 0;
        LOG_RFB("RFB: [%ld] Satisfied read request, extra: %d\n",
               client->m_gocount, client->m_buffered);
        return RFB_CLIENT_RC_SUCCESS;
    }
    else {
        // Copy whatever we have and wait for more
        memcpy(client->m_readPtr, client->m_readBuf, client->m_buffered);
        client->m_readPtr += client->m_buffered;
        client->m_bytesToRead -= client->m_buffered;
        client->m_buffered = 0;
        LOG_RFB("RFB: [%ld] Unsatisfied read request, remaining: %d\n",
               client->m_gocount, client->m_bytesToRead);
        return RFB_CLIENT_RC_PARTIAL_IO;
    }

}

int
RFBWriteToServer(RFBClient_t * client, unsigned char * buffer, 
                 unsigned int bytes_to_write)
{

    
    // Move unwritten bytes to the beginning of the buffer
    if (client->m_bytesToRead > 0) {
        LOG_RFB("RFB: [%ld] Moving pending write bytes %d\n",
                client->m_gocount, client->m_bytesToWrite);
        memmove(client->m_writeArea, client->m_writePtr, client->m_bytesToWrite);
    }
    if (buffer != NULL && bytes_to_write > 0) {
        memcpy(client->m_writeArea + client->m_bytesToWrite, 
                buffer, bytes_to_write);
    }
    client->m_writePtr = client->m_writeArea;
    client->m_bytesToWrite += bytes_to_write;

    if (client->m_maxBytesToWrite < client->m_bytesToWrite)
        client->m_maxBytesToWrite = client->m_bytesToWrite;

    if (client->m_bytesToWrite > 0) {
        int rc = FlushIO(client);

        if (rc == RFB_CLIENT_RC_SUCCESS) {
            
        }
        else {
            return rc; // either wait or error
        }
    }

    if (client->m_bytesToWrite > 0) {
        LOG_RFB("RFB: [%ld] Unsatisfied write request, remaining: %d\n",
               client->m_gocount, client->m_bytesToWrite);
        return RFB_CLIENT_RC_WAIT;
    }
    else {
        LOG_RFB_TRACE("RFB: [%ld] Satisfied write request.\n", client->m_gocount);
        return RFB_CLIENT_RC_SUCCESS;
    }

}
