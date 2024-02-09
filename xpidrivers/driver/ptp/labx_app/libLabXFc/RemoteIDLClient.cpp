// File        : RemoteIDLClient.cpp
// Author      : Eldridge M. Mount IV
// Description : Class abstracting a connection to a remote IDL server, 
//               permitting IDL messages and responses to be exchanged over
//               a TCP connection.
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.

// System headers
#include <arpa/inet.h>

// STL headers
#include <iostream>
#include <sstream>

// Uncomment for class-level debugging messages
// #define _LABXDEBUG

// Library headers
#include "Debug.h"
#include "Exception.h"
#include "LabX.h"
#include "RemoteIDLClient.h"
#include "RemoteIDLServer.h"

// Namespace using directives
using LabX::RemoteIDLClient;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

// Size of a transmit / receive buffer
#define DATA_BUFFER_BYTES (1024)

// Size of the circular processing buffer
#define RX_QUEUE_BYTES (4096)

// Constructor / destructor

RemoteIDLClient::RemoteIDLClient(void) :
  IDLClient(),
  Thread("RemoteIDLClient"),
  clientSocket(),
  rxHeadIndex(0),
  rxTailIndex(0),
  rxQueuedBytes(0),
  clientSem(0, "idlClientSem") {
  txBuffer = new uint8_t[DATA_BUFFER_BYTES];
  rxBuffer = new uint8_t[DATA_BUFFER_BYTES];
  rxQueue = new uint8_t[RX_QUEUE_BYTES];
}

RemoteIDLClient::~RemoteIDLClient(void) {
  delete[](txBuffer);
  delete[](rxBuffer);
  delete[](rxQueue);
}

// Public interface methods

void RemoteIDLClient::connect(const string &remoteHostname,
                              uint16_t remotePort) {
  try {
    lock();

    // Attempt to make the connection with the remote host; this will
    // throw if we're already connected.
    clientSocket.connect(remoteHostname, remotePort);

    // Clear out the receive state
    rxHeadIndex = 0;
    rxTailIndex = 0;
    rxQueuedBytes = 0;
    rxParseState = RX_PARSE_CMD;

    // Start the worker thread to process server responses
    start();

    // Now that the response thread is running, request and validate
    // the version of the protocol the remote server is running with
    validateProtocolVersion();
    DebugNote("Successfully connected to remote IDL server");

    unlock();
  } catch(Exception *exc) {
    clientSocket.disconnect();
    unlock();
    throw(new Exception("Error connecting to remote IDL server", exc));
  }
}

void RemoteIDLClient::disconnect(void) {
  try {
    lock();

    // Schedule the thread for termination and disconnect the client socket,
    // causing an exception to be caught within run().
    stop();
    clientSocket.disconnect();

    // Join with the worker thread prior to returning
    join();

    unlock();
  } catch(Exception *exc) {
    unlock();
    throw(new Exception("Error disconnecting from remote IDL server", exc));
  }
}

// Shared public overrides
const string& RemoteIDLClient::getName(void) const {
  // Dispatch to Thread's implementation
  return(Thread::getName());
}

// Public overrides from class IDLClient

void RemoteIDLClient::sendMessage(const IDLRequestMessageBuffer &request, 
                                  IDLResponseMessageBuffer &response) {
  try {
    // Construct an IDL request command:
    // CMD_IDL_REQUEST (uint32_t)
    // IDL_DATA_LENGTH (uint16_t)
    // IDL_DATA        (uint8_t * IDL_DATA_LENGTH)
    uint8_t *txPointer = txBuffer;
    uint32_t commandWord = RemoteIDLServer::CMD_IDL_REQUEST;
    uint32_t *wordPointer = reinterpret_cast<uint32_t*>(txPointer);
    *wordPointer = htonl(commandWord);
    txPointer += sizeof(commandWord);
  
    uint16_t idlLength = request.getLength();
    uint16_t *shortPointer = reinterpret_cast<uint16_t*>(txPointer);
    *shortPointer = htons(idlLength);
    txPointer += sizeof(idlLength);
  
    const uint8_t *idlBytes = request.get_bytes(0);
    for(uint32_t byteIndex = 0; byteIndex < request.getLength(); byteIndex++) {
      *txPointer++ = *idlBytes++;
    }

    // Leave a pointer to the response buffer for the receive thread to fill in
    clientResponse = &response;
  
    // Send the command, which should remotely invoke the method on the server
    clientSocket.sendData(txBuffer, static_cast<uint32_t>(txPointer - txBuffer));

    // Wait on the client semaphore for the response to come in
    clientSem.wait();
  } catch(Exception *exc) {
    throw(new Exception("Error sending IDL message to server", exc));
  }
}

void RemoteIDLClient::shutdown(void) {
  // Disconnect from the remote Responder
  disconnect();
}

// Public overrides from class Thread

int32_t RemoteIDLClient::run(void) {
  bool goodConnection = true;

  try {
    // Loop forever, receiving server responses and interpreting them as
    // long as they are sane
    while(goodConnection) {
      // Attempt to receive as much data at a time as our Rx buffer will hold
      uint32_t numRxBytes = clientSocket.receiveData(rxBuffer, DATA_BUFFER_BYTES);
      
      // First, place the data into a circular buffer for processing; this allows
      // fragmented reponses to be coherently processed.
      for(uint32_t byteIndex = 0; byteIndex < numRxBytes; byteIndex++) {
        rxQueue[rxHeadIndex++] = rxBuffer[byteIndex];
        if(rxHeadIndex >= RX_QUEUE_BYTES) rxHeadIndex = 0;
        if(rxHeadIndex == rxTailIndex) goodConnection = false;
      }
      rxQueuedBytes += numRxBytes;
      
      // Process bytes out of the circular queue as long as the connection is
      // still able to be trusted.  Consume bytes and update the parsing state
      // as bytes become available.
      bool continueParsing = true;
      while(goodConnection & continueParsing & (rxQueuedBytes > 0)) {
        // Switch upon the present parsing state
        switch(rxParseState) {
        case RX_PARSE_CMD:
          {
            // Ready to parse a command from the client
            if(rxQueuedBytes < sizeof(uint32_t)) {
              continueParsing = false;
              break;
            }

            // Make a state transition based upon the received response
            uint32_t serverResponse = getQueuedUint32();
            switch(serverResponse) {
            case RemoteIDLServer::RESP_VERSION:
              rxParseState = RX_PARSE_VERSION;
              break;

            case RemoteIDLServer::RESP_IDL_RESPONSE:
              rxParseState = RX_PARSE_IDL_LENGTH;
              break;

            case RemoteIDLServer::RESP_ASYNC_EVENT:
              // Not yet implemented, should not be returned by the server
              goodConnection = false;
              break;

            default:
              // Unrecognized response, bad connection
              goodConnection = false;
            } // switch(server response)
          } // case RX_PARSE_CMD
          break;

        case RX_PARSE_VERSION:
          {
            if(rxQueuedBytes < sizeof(uint32_t)) {
              continueParsing = false;
              break;
            }

            // Get the server version and awaken any client
            serverVersionWord = getQueuedUint32();
            clientSem.post();
            rxParseState = RX_PARSE_CMD;
          }
          break;

        case RX_PARSE_IDL_LENGTH:
          {
            // Fetch the length of the IDL response
            if(rxQueuedBytes < sizeof(uint16_t)) {
              continueParsing = false;
              break;
            }

            // Fetch the response length and continue
            idlResponseLength = getQueuedUint16();
            rxParseState = RX_PARSE_IDL_DATA;
          }
          break;

        case RX_PARSE_IDL_DATA:
          {
            // Wait until the entire response is queued up
            if(rxQueuedBytes < idlResponseLength) {
              continueParsing = false;
              break;
            }

            // Fill the client's response buffer in with the data
            for(uint32_t byteIndex = 0; byteIndex < idlResponseLength; byteIndex++) {
              clientResponse->set_uint8_t(byteIndex, rxQueue[rxTailIndex++]);
              if(rxTailIndex >= RX_QUEUE_BYTES) rxTailIndex = 0;
            }
            rxQueuedBytes -= idlResponseLength;
            
            // Inform the client and return to the command parsing state
            clientSem.post();
            rxParseState = RX_PARSE_CMD;
          }
          break;

        default:
          ;
        } // switch(Rx parse state)
      } // if(good connection)
    } // while(good connection)
  } catch(Exception *exc) {
    // An exception is expected whenever the socket is closed by the
    // server or an error occurs.  In any event, we silently exit after
    // destroying the exception object.
    delete(exc);
  }

  // TODO - Let any client call know if an error occurred!
  if(goodConnection == false) {
    lock();
    // TODO...
    unlock();
  }

  return(Thread::k_Success);
}

// Private helper methods

uint16_t RemoteIDLClient::getQueuedUint16(void) {
  uint16_t retValue;

  // Handle the wraparound case separately
  if(rxTailIndex <= (RX_QUEUE_BYTES - sizeof(uint16_t))) {
    // Enough bytes to handle consecutively
    retValue = ntohs(*(reinterpret_cast<uint16_t*>(rxQueue + rxTailIndex)));
    rxTailIndex += sizeof(uint16_t);
  } else {
    uint8_t tempBuffer[sizeof(uint16_t)];
    for(uint32_t byteIndex = 0; byteIndex < sizeof(uint16_t); byteIndex++) {
      tempBuffer[byteIndex] = rxQueue[rxTailIndex++];
      if(rxTailIndex >= RX_QUEUE_BYTES) rxTailIndex = 0;
    }
    retValue = ntohl(*(reinterpret_cast<uint16_t*>(tempBuffer)));
  }
  rxQueuedBytes -= sizeof(uint16_t);
  return(retValue);
}

uint32_t RemoteIDLClient::getQueuedUint32(void) {
  uint32_t retValue;

  // Handle the wraparound case separately
  if(rxTailIndex <= (RX_QUEUE_BYTES - sizeof(uint32_t))) {
    // Enough bytes to handle consecutively
    retValue = ntohl(*(reinterpret_cast<uint32_t*>(rxQueue + rxTailIndex)));
    rxTailIndex += sizeof(uint32_t);
  } else {
    uint8_t tempBuffer[sizeof(uint32_t)];
    for(uint32_t byteIndex = 0; byteIndex < sizeof(uint32_t); byteIndex++) {
      tempBuffer[byteIndex] = rxQueue[rxTailIndex++];
      if(rxTailIndex >= RX_QUEUE_BYTES) rxTailIndex = 0;
    }
    retValue = ntohl(*(reinterpret_cast<uint32_t*>(tempBuffer)));
  }
  rxQueuedBytes -= sizeof(uint32_t);
  return(retValue);
}

void RemoteIDLClient::validateProtocolVersion(void) {
  try {
    // Construct a "get version" command and transmit it
    uint8_t *txPointer = txBuffer;
    uint32_t commandWord = RemoteIDLServer::CMD_VERSION;
    uint32_t *wordPointer = reinterpret_cast<uint32_t*>(txPointer);
    *wordPointer = htonl(commandWord);
    txPointer += sizeof(commandWord);
    clientSocket.sendData(txBuffer, static_cast<uint32_t>(txPointer - txBuffer));

    // Wait on the client semaphore until the version response has been received
    clientSem.wait();
    if(serverVersionWord != RemoteIDLServer::ProtocolVersion) {
      stringstream excStream;
      excStream << "IDL server protocol version ("
                << (serverVersionWord >> 8)
                << "."
                << (serverVersionWord & 0x0FF)
                << ") doesn't match supported version ("
                << (RemoteIDLServer::ProtocolVersion >> 8)
                << "."
                << (RemoteIDLServer::ProtocolVersion & 0x0FF)
                << ")";
      throw(new Exception(excStream.str()));
    }
    DebugNoteVa("Server protocol version response word 0x%08X good", serverVersionWord);
  } catch(Exception *exc) {
    throw(new Exception("Error validating protocol version", exc));
  }
}
