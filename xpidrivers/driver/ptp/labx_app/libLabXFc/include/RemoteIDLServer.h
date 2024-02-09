#ifndef _REMOTE_IDL_SOCKET_H_
#define _REMOTE_IDL_SOCKET_H_

// Library headers
#include "IDLResponder.h"
#include "IDLServer.h"
#include "TcpServerSocket.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Class implementing a TCP server for remote IDL-based control of an
   * application.  By creating and starting an instance, the application
   * permits IDL messages to flow from the remote client directly into
   * registered IDL responders.  Facilities are provided to enforce access
   * control on a class code basis.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class RemoteIDLServer : public IDLServer, public TcpServerSocket {

    // Public type definitions
  public:

    /**
     * Enumeration defining the basic client requests
     */
    typedef enum {
      CMD_VERSION     = 1,
      CMD_IDL_REQUEST = 2,
    } ClientCommand;

    /**
     * Enumeration defining the basic server responses
     */
    typedef enum {
      RESP_VERSION      = 1,
      RESP_IDL_RESPONSE = 2,
      RESP_ASYNC_EVENT  = 3,
    } ServerResponse;

    // Constructor / Destructor
  public:

    /**
     * Creates an instance bound to the specific port and supporting the
     * passed maximum number of concurrent client connections.
     * @param listenPort - Local TCP port to listen upon
     * @param maxClients - Maximum number of concurrent clients to permit
     */
    RemoteIDLServer(uint16_t listenPort, uint32_t maxClients);

    /**
     * Virtual destructor
     */
    virtual ~RemoteIDLServer(void);

    // Shared public overrides
  public:

    /**
     * Accessor for the name; this is shared with the interfaces
     * of both LabX::Thread and LabX::IDLServer.
     *
     * @return - The name inferred at construction
     */
    virtual const std::string& getName(void) const;

    // Public overrides from class IDLServer
  public:

    /**
     * Shuts down the IDL server, closing the sockets and stopping the
     * underlying worker thread
     */
    virtual void shutdown(void);

    // Protected overrides from class TcpServerSocket
  protected:

    /**
     * Method invoked when a new client has been accepted
     * @param clientEntry - Reference to the client's data structure
     */
    virtual void addClient(const ClientEntry &clientEntry);

    /**
     * Method invoked when a client has been disconnected
     * @param clientEntry - Reference to the client's data structure
     */
    virtual void removeClient(const ClientEntry &clientEntry);

    /**
     * Method invoked when data is received from a client connection
     * @param clientEntry - Reference to the client's data structure
     * @param rxBuffer    - Pointer to the received data bytes
     * @param numRxBytes  - Number of valid bytes within rxBuffer
     * @param Returns true if the client should be disconnected
     */
    virtual bool handleClientData(const ClientEntry &clientEntry,
                                  uint8_t *rxBuffer,
                                  uint32_t numRxBytes);

    /**
     * Method invoked when an error occurs on a client connection
     * @param clientEntry - Reference to the client's data structure
     */
    virtual void handleClientError(const ClientEntry &clientEntry);

    // Private type definitions
  private:

    /**
     * Enumerated type capturing the protocol handling state for a client
     */
    typedef enum {
      CLIENT_COMMAND,
      CLIENT_CMD_PENDING,
      CLIENT_IDL_PARSE,
    } ParseState;

    /**
     * Structure encapsulating the state space for each client socket
     */
    class ClientState : public IDLResponder::ClientInterface {

      // Constructor / destructor
    public:

      /**
       * Constructor initializes the only const attribute
       * @param clientEntry - TCP server socket client data
       * @param sourceName  - String describing the source of the control
       */
      ClientState(const ClientEntry &clientEntry,
                  const std::string &sourceName) : 
        IDLResponder::ClientInterface(),
          clientEntry(clientEntry),
          sourceName(sourceName) {}
      
      /**
       * Virtual destructor
       */
      virtual ~ClientState(void) {}
 
      // Public overrides from class IDLResponder::ClientInterface
    public:

      /**
       * Returns a string describing the interface over which the
       * client marshals IDL requests and responses
       *
       * @return A string identifying the client's source
       */
      virtual const std::string& getName(void) const;

      // Public attributes
    public:
      const ClientEntry &clientEntry;
      const std::string sourceName;
      ParseState parseState;
      uint32_t pendingCommand;
      uint32_t commandLength;
      uint8_t *cachedRxBytes;
      uint32_t cachedRxCount;
    };

    // Private helper methods
  private:

    /**
     * Caches received bytes for the client, holding onto them while
     * there is not sufficient data for it to process and advance state.
     * @param clientState - Client state structure
     * @param rxBufferPtr - Pointer into rxBuffer
     * @param numRxBytes  - Remaining bytes in rxBuffer
     */
    void cacheClientBytes(ClientState *clientState, 
                          uint8_t *rxBufferPtr, 
                          uint32_t numRxBytes);

    /**
     * Handles a CMD_VERSION command for the server
     * @param clientState - Client state structure
     */
    void handleVersionCommand(ClientState *clientState);

    /**
     * Handles a CMD_IDL_REQUEST command for the server
     * @param clientState - Client state structure
     * @param rxBufferPtr - Pointer to the received bytes to process
     * @param numRxBytes  - Number of bytes to process
     */
    void handleIDLRequest(ClientState *clientState,
                          uint8_t *rxBufferPtr,
                          uint32_t numRxBytes);

    // Public attributes
  public:

    /**
     * Protocol version employed
     */
    static const uint32_t ProtocolVersion;

    // Private attributes
  private:

    /**
     * Vector of state space entries for each client
     */
    std::vector<ClientState*> stateVector;

    /**
     * Transmit buffer for responding to clients
     */
    uint8_t *txBuffer;
  };

}

#endif
