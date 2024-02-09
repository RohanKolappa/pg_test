#ifndef _REMOTE_IDL_CLIENT_H_
#define _REMOTE_IDL_CLIENT_H_

// Library headers
#include "IDLClient.h"
#include "Semaphore.h"
#include "TcpClientSocket.h"
#include "Thread.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Class abstracting a connection to a remote IDL server, permitting
   * IDL messages and responses to be exchanged over a TCP connection.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class RemoteIDLClient : public IDLClient, protected Thread {

    // Constructor / Destructor
  public:

    /**
     * Default constructor
     */
    RemoteIDLClient(void);

    /**
     * Virtual destructor
     */
    virtual ~RemoteIDLClient(void);

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

    // Shared public overrides
  public:

    /**
     * Returns the name of the instance; this maps to both the interface
     * of LabX::Thread as well as LabX::IDLClient
     */
    virtual const std::string& getName(void) const;

    // Public overrides from class IDLClient
  public:

    /**
     * Sends the passed request message buffer to the responder, returning
     * a response buffer after completion.
     * @param request  - Request buffer to the responder
     * @param response - Response buffer from the responder
     */
    virtual void sendMessage(const IDLRequestMessageBuffer &request, 
                             IDLResponseMessageBuffer &response);

    /**
     * Closes the socket to the remote Responder
     */
    virtual void shutdown(void);

    // Public overrides from class Thread
  public:

    /**
     * Worker thread method used to receive server data asynchronously from
     * the socket
     * @param The return status for the thread
     */
    virtual int32_t run(void);

    // Private type definitions
  private:

    /**
     * Enumerated type to capture parsing state
     */
    typedef enum {
      RX_PARSE_CMD,
      RX_PARSE_VERSION,
      RX_PARSE_IDL_LENGTH,
      RX_PARSE_IDL_DATA,
    } RxParseState;

    // Private helper methods
  private:

    /**
     * Returns a 16-bit unsigned value from the Rx queue.  It is assumed that
     * the caller has already ascertained that there is enough data.
     * @return The 16-bit value
     */
    uint16_t getQueuedUint16(void);

    /**
     * Returns a 32-bit unsigned value from the Rx queue.  It is assumed that
     * the caller has already ascertained that there is enough data.
     * @return The 32-bit value
     */
    uint32_t getQueuedUint32(void);

    /**
     * Validates the remote protocol version being used against the instance's,
     * throwing an exception if there is a mismatch.
     */
    void validateProtocolVersion(void);

    // Private attributes
  private:

    /**
     * TCP client socket instance used for communications
     */
    TcpClientSocket clientSocket;

    /**
     * Transmit buffer
     */
    uint8_t *txBuffer;

    /**
     * Receive buffer
     */
    uint8_t *rxBuffer;

    /**
     * Circular processing queue for Rx data
     */
    uint8_t *rxQueue;

    /**
     * Head pointer for writing data into the Rx queue
     */
    uint32_t rxHeadIndex;

    /**
     * Tail pointer for reading data from the Rx queue
     */
    uint32_t rxTailIndex;

    /**
     * Number of bytes in the Rx queue
     */
    uint32_t rxQueuedBytes;

    /**
     * Rx parsing state
     */
    RxParseState rxParseState;

    /**
     * Server protocol version
     */
    uint32_t serverVersionWord;

    /**
     * Length of an IDL response message
     */
    uint16_t idlResponseLength;

    /**
     * Pointer to an IDL response message buffer to write into
     */
    IDLResponseMessageBuffer *clientResponse;

    /**
     * Semaphore used to interact with a client thread
     */
    Semaphore clientSem;
  };

}

#endif
