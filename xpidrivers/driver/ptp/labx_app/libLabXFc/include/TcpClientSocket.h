#ifndef _TCP_CLIENT_SOCKET_H_
#define _TCP_CLIENT_SOCKET_H_

// System headers
#include <stdint.h>
#include <string>
#include <vector>

// Library headers
#include "Lockable.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Class abstracting a TCP client socket, capable of initiating  connections
   * over a TCP / IP socket and sending / receiving data.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class TcpClientSocket : public Lockable {

    // Constructor / Destructor
  public:

    /**
     * Default constructor
     */
    TcpClientSocket(void);

    /**
     * Virtual destructor
     */
    virtual ~TcpClientSocket(void);

    // Public interface methods
  public:

    /**
     * Connects to a remote server at the specified IP address and port
     * @param remoteHostname - IP address string, in "dotted quad" format,
     *                         or host name to resolve
     * @param remotePort     - Remote TCP port to connect to
     */
    virtual void connect(const std::string &remoteHostname,
                         uint16_t remotePort);
                         
    /**
     * Disconnects from the remote server, if connected
     */
    virtual void disconnect(void);
    
    /**
     * Sends a data buffer to the remote connection
     * @param txBuffer   - Buffer containing the data to transmit
     * @param numTxBytes - Number of bytes to transmit
     */
    virtual void sendData(const uint8_t *txBuffer, uint32_t numTxBytes);
    
    /**
     * Receives data from the remote connection, blocking until at least
     * some amount of data hass been received.  Throws an exception if
     * the server closes the socket or an error occurs.
     * @param rxBuffer   - Buffer to receive into
     * @param numRxBytes - Number of bytes to attempt to receive
     * @return The number of bytes actually received
     */
    virtual uint32_t receiveData(uint8_t *rxBuffer, uint32_t numRxBytes);

    // Private attributes
  private:

    /**
     * Remote host name connected to
     */
    std::string remoteHostname;

    /**
     * Remote port connect to
     */
    uint16_t remotePort;

    /**
     * File descriptor of the socket
     */
    int socketFd;
  };

}

#endif
