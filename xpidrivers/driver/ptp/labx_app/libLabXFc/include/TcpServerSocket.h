#ifndef _TCP_SERVER_SOCKET_H_
#define _TCP_SERVER_SOCKET_H_

// System headers
#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <vector>

// Library headers
#include "Lockable.h"
#include "Thread.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Class abstracting a TCP server socket, capable of accepting connections over a
   * TCP / IP socket and dispatching asynchronously-received stream data to a polymorphic
   * subclass within an independent thread of control
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class TcpServerSocket : public Thread, public Lockable {

    // Public attributes
  public:

    /**
     * Constant defining how many bytes are in each IP address
     * This should ultimately be abstracted by a separate utility class.
     */
    static const uint32_t k_IpAddressBytes = 4;

    // Constructor / Destructor
  public:

    /**
     * Constructor creating by name
     * @param serverName - Text name for the server object
     * @param listenPort - Local TCP port to listen upon
     * @param maxClients - Maximum number of concurrent clients to permit
     */
    TcpServerSocket(const std::string &serverName, 
                    uint16_t listenPort,
                    uint32_t maxClients);

    /**
     * Virtual destructor
     */
    virtual ~TcpServerSocket(void);

    // Public interface methods
  public:

    /**
     * Returns the name of the server
     * @return The string name provided upon construction
     */
    const std::string& getName(void) const;

    // Public overrides from class Thread
  public:

    /**
     * Starts the server on the local socket, listening for new connections.
     */
    virtual void start(void);

    /**
     * Stops and closes the server, killing its thread
     */
    virtual void stop(void);

    /**
     * Method for the server's thread of control
     * @param The return status for the thread
     */
    virtual int32_t run(void);

    // Protected type definitions
  protected:

    /**
     * Inner struct used to keep track of active client connections
     */
    typedef struct {
      int32_t            clientFd;
      uint8_t            sourceIpAddress[k_IpAddressBytes];
      uint16_t           sourceTcpPort;
      struct sockaddr_in clientAddress;
    } ClientEntry;

    // Protected interface methods
  protected:

    /**
     * Pure virtual method invoked when a new client has been accepted.
     * Called within a locked context
     * @param clientEntry - Reference to the client's data structure
     */
    virtual void addClient(const ClientEntry &clientEntry) = 0;

    /**
     * Pure virtual method invoked when a client has been disconnected
     * Called within a locked context
     * @param clientEntry - Reference to the client's data structure
     */
    virtual void removeClient(const ClientEntry &clientEntry) = 0;

    /**
     * Pure virtual method invoked when data is received from a client connection
     * Called within a locked context
     * @param clientEntry - Reference to the client's data structure
     * @param rxBuffer    - Pointer to the received data bytes
     * @param numRxBytes  - Number of valid bytes within rxBuffer
     * @param Returns true if the client should be disconnected
     */
    virtual bool handleClientData(const ClientEntry &clientEntry,
                                  uint8_t *rxBuffer,
                                  uint32_t numRxBytes) = 0;

    /**
     * Pure virtual method invoked when an error occurs on a client connection
     * Called within a locked context; the client is disconnected after this call.
     * @param clientEntry - Reference to the client's data structure
     */
    virtual void handleClientError(const ClientEntry &clientEntry) = 0;

    /**
     * Sends a data buffer to the remote connection
     * @param clientEntry - Reference to the client's data structure
     * @param txBuffer    - Buffer containing the data to transmit
     * @param numTxBytes  - Number of bytes to transmit
     */
    void sendData(const ClientEntry &clientEntry,
                  const uint8_t *txBuffer, 
                  uint32_t numTxBytes);

    // Private attributes
  private:

    /**
     * Name for the instance
     */
    const std::string serverName;

    /**
     * Local port to listen upon
     */
    const uint16_t listenPort;

    /**
     * Maximum number of simultaneous clients to allow
     */
    const uint32_t maxClients;

    /**
     * File descriptor of the listening socket
     */
    int listenFd;

    /**
     * Vector of active clients
     */
    std::vector<ClientEntry*> activeClients;

    /**
     * Receive buffer for the instance
     */
    uint8_t *rxBuffer;
  };

}

#endif
