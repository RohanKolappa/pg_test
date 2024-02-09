// File        : RemoteIDLServer.cpp
// Author      : Eldridge M. Mount IV
// Description : Class implementing a TCP server for remote IDL-based control
//               of an application.  By creating and starting an instance, the 
//               application permits IDL messages to flow from the remote client 
//               directly into registered IDL responders.  Facilities are provided
//               to enforce access control on a class code basis.
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.

// STL headers
#include <iostream>
#include <sstream>

// #define _LABXDEBUG

// Library headers
#include "Debug.h"
#include "Exception.h"
#include "IDLMessageBuffer.h"
#include "RemoteIDLServer.h"

// Namespace using directives
using LabX::RemoteIDLServer;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

// Size of the partial command buffer maintained for each client
#define CMD_BUFFER_BYTES  (1024)
#define TX_BUFFER_BYTES   (1024)

// Static member initialization

// A single word captures the protocol version: eight bits of major,
// and eight bits of minor.
//
// Present version is 1.0
const uint32_t RemoteIDLServer::ProtocolVersion = 0x0100;

// Method to create the server name
static const string makeServerName(uint16_t listenPort) {
  stringstream nameStream;
  nameStream << "RemoteIDLServer(" << listenPort << ")";
  return(nameStream.str());
}

// Constructor / destructor

RemoteIDLServer::RemoteIDLServer(uint16_t listenPort, uint32_t maxClients) :
  IDLServer(),
  TcpServerSocket(makeServerName(listenPort), listenPort, maxClients),
  stateVector() {
  txBuffer = new uint8_t[TX_BUFFER_BYTES];
}

RemoteIDLServer::~RemoteIDLServer(void) {
  while(!stateVector.empty()) {
    delete(stateVector.back());
    stateVector.pop_back();
  }
  delete[](txBuffer);
}

// Shared public overrides
const string& RemoteIDLServer::getName(void) const {
  // Defer to the implementation from LabX::Thread
  return(Thread::getName());
}

// Public overrides from class IDLServer

void RemoteIDLServer::shutdown(void) {
  try {
    // Stop the TCP server thread and join with it
    stop();
    join();
  } catch(Exception *exc) {
    throw(new Exception("RemoteIDLServer::shutdown() : Caught exception", exc));
  }
}

// Protected overrides from class TcpServerSocket

void RemoteIDLServer::addClient(const ClientEntry &clientEntry) {
  // Create and add a new state structure for the client, creating a unique
  // name for it based upon the source IP and TCP port number
  stringstream clientName;
  clientName << getName()
             << "[";
  for(uint32_t byteIndex = 0; byteIndex < k_IpAddressBytes; byteIndex++) {
    clientName << static_cast<uint32_t>(clientEntry.sourceIpAddress[byteIndex]);
    if(byteIndex < (k_IpAddressBytes - 1)) clientName << ".";
  }
  clientName << ":" << clientEntry.sourceTcpPort << "]";
                                  
  ClientState *newState   = new ClientState(clientEntry, clientName.str());
  newState->parseState    = CLIENT_COMMAND;
  newState->commandLength = 0;
  newState->cachedRxBytes = new uint8_t[CMD_BUFFER_BYTES];
  newState->cachedRxCount = 0;
  stateVector.push_back(newState);

  // Register the client with the IDLResponder framework
  IDLResponder::registerClient(newState);
}

void RemoteIDLServer::removeClient(const ClientEntry &clientEntry) {
  // Find and destroy the client's state
  for(vector<ClientState*>::iterator stateIter = stateVector.begin();
      stateIter != stateVector.end();
      ++stateIter) {
    // First unregister with the IDLResponder framework
    IDLResponder::unregisterClient(*stateIter);

    // Next, destroy all state associated with the client
    if(&((*stateIter)->clientEntry) == &clientEntry) {
      delete[]((*stateIter)->cachedRxBytes);
      delete(*stateIter);
      stateVector.erase(stateIter);
      break;
    }
  }
}

bool RemoteIDLServer::handleClientData(const ClientEntry &clientEntry,
                                       uint8_t *rxBuffer,
                                       uint32_t numRxBytes) {
  // Presume we do not want to disconnect the client
  bool disconnectClient = false;

  // First navigate to the client state space
  ClientState *clientState = NULL;
  for(vector<ClientState*>::iterator stateIter = stateVector.begin();
      stateIter != stateVector.end();
      ++stateIter) {
    if((*stateIter)->clientEntry.clientFd == clientEntry.clientFd) {
      clientState = *stateIter;
      break;
    }
  }

  // Bail out if we couldn't find the client
  if(clientState == NULL) {
    DebugWarningVa("Failed to find client state vector for server \"%s\"", getName().c_str());
    return(true);
  }

  // Do not allow exceptions to leak beyond this context; we will simply request
  // that the client be disconnected if any are caught
  try {
    // Loop until all of the Rx bytes have been consumed in one manner or another
    uint8_t *rxBufferPtr = rxBuffer;

    // Switch upon the client socket's parsing state; command data may come in
    // fragmented, meaning we may not have enough data to operate on all at once.
    // However, all interaction is separated by a response, so once a command
    // is fully parsed, there are no "leftover" bytes.
    switch(clientState->parseState) {
    case CLIENT_COMMAND: 
      {
        uint32_t clientCommand;

        // Make sure there are at least enough bytes for the command; otherwise
        // cache them
        if(numRxBytes >= sizeof(clientCommand)) {
          clientState->pendingCommand = ntohl(*reinterpret_cast<uint32_t*>(rxBufferPtr));
          rxBufferPtr += sizeof(clientCommand);
          numRxBytes -= sizeof(clientCommand);

          // Examine the command in order to determine what to do next
          switch(clientState->pendingCommand) {
          case CMD_VERSION:
            handleVersionCommand(clientState);
            break;

          case CMD_IDL_REQUEST:
            handleIDLRequest(clientState, rxBufferPtr, numRxBytes);
            break;

          default:
            DebugWarningVa("Unrecognized client command %d for server \"%s\"",
                           clientState->pendingCommand,
                           getName().c_str());
          }
        } else cacheClientBytes(clientState, rxBufferPtr, numRxBytes);
      }
      break;

    case CLIENT_CMD_PENDING:
      // A command is pending but still requires additional data, reinvoke it.
      // Before doing so, cache the bytes since we now have to work from the cached
      // buffer to begin with
      cacheClientBytes(clientState, rxBufferPtr, numRxBytes);
      switch(clientState->pendingCommand) {
      case CMD_VERSION:
        handleVersionCommand(clientState);
        break;

      case CMD_IDL_REQUEST:
        handleIDLRequest(clientState, clientState->cachedRxBytes, clientState->cachedRxCount);
        break;
        
      default:
        ;
      }
      break;

    default:
      DebugWarningVa("Unrecognized client parse state for server \"%s\"", getName().c_str());
    } // case(client parse state)
  } catch(Exception *exc) {
    DebugWarningVa("Disconnecting client due to exception : %s", exc->toString().c_str());
    delete(exc);
    disconnectClient = true;
  }

  // Return whether we wish to disconnect the client or not
  return(disconnectClient);
}

void RemoteIDLServer::handleClientError(const ClientEntry &clientEntry) {
  cout << "RemoteIDLServer::handleClientError()" << endl;
}

// Private type definitions

// Class ClientState

const string& RemoteIDLServer::ClientState::getName(void) const {
  return(sourceName);
}

// Private helper methods

void RemoteIDLServer::cacheClientBytes(ClientState *clientState, 
                                       uint8_t *rxBufferPtr, 
                                       uint32_t numRxBytes) {
  // Copy the bytes into the client's cache space, using it as a circular
  // buffer and wrapping around.
  clientState->cachedRxCount += numRxBytes;
  uint8_t *cachePtr = (clientState->cachedRxBytes + clientState->cachedRxCount);
  while(numRxBytes-- > 0) {
    *cachePtr++ = *rxBufferPtr++;
  }
}

void RemoteIDLServer::handleVersionCommand(ClientState *clientState) {
  try {
    uint8_t *txPointer = txBuffer;
    uint32_t *wordPointer = reinterpret_cast<uint32_t*>(txPointer);

    // Begin by declaring the type of response this is
    *wordPointer++ = htonl(RESP_VERSION);
    txPointer += sizeof(uint32_t);

    // Next, write the actual protocol version word
    *wordPointer = htonl(ProtocolVersion);
    txPointer += sizeof(ProtocolVersion);
    sendData(clientState->clientEntry, txBuffer, static_cast<uint32_t>(txPointer - txBuffer));
  } catch(Exception *exc) {
    throw(new Exception("RemoteIDLServer::handleVersionCommand() : Caught exception", exc));
  }
}

void RemoteIDLServer::handleIDLRequest(ClientState *clientState,
                                       uint8_t *rxBufferPtr,
                                       uint32_t numRxBytes) {
  IDLRequestMessageBuffer *request = NULL;
  IDLResponseMessageBuffer *response = NULL;
  try {
    // First see if there is enough data for the IDL request length
    uint16_t requestLength;
    if(numRxBytes < sizeof(requestLength)) {
      cacheClientBytes(clientState, rxBufferPtr, numRxBytes);
      clientState->parseState = CLIENT_CMD_PENDING;
      return;
    }

    // Get the length of the IDL request
    uint32_t rxBytesLeft = numRxBytes;
    uint8_t *nextRxByte = rxBufferPtr;
    uint16_t *rxShortPtr = reinterpret_cast<uint16_t*>(rxBufferPtr);
    requestLength = ntohs(*rxShortPtr);
    nextRxByte += sizeof(requestLength);
    rxBytesLeft -= sizeof(requestLength);

    // See if there are enough bytes for the IDL payload
    if(rxBytesLeft < requestLength) {
      cacheClientBytes(clientState, rxBufferPtr, numRxBytes);
      clientState->parseState = CLIENT_CMD_PENDING;
      return;
    }

    // Extract the IDL request payload from the message, creating a buffer
    // object for dispatching.  Also create a response buffer and dispatch it.
    request = new IDLRequestMessageBuffer(nextRxByte, requestLength);    
    response = new IDLResponseMessageBuffer();
    IDLResponder::processMessage(clientState, *request, *response);

    // A response has been received from the processed request, even if it is
    // just an empty "success" message; ship it back down the port, prefaced by
    // the response type (IDL) and the length.
    uint8_t *txPointer = txBuffer;
    uint32_t *wordPointer = reinterpret_cast<uint32_t*>(txPointer);
    *wordPointer = htonl(RESP_IDL_RESPONSE);
    txPointer += sizeof(uint32_t);

    // Next encode the length of the IDL response
    uint16_t *shortPointer = reinterpret_cast<uint16_t*>(txPointer);
    *shortPointer = htons(response->getLength());
    txPointer += sizeof(uint16_t);

    // Finally, copy the response data into the buffer and transmit it
    const uint8_t *responseBytes = response->get_bytes(0);
    for(uint32_t byteIndex = 0; byteIndex < response->getLength(); byteIndex++) {
      *txPointer++ = responseBytes[byteIndex];
    }
    sendData(clientState->clientEntry, txBuffer, static_cast<uint32_t>(txPointer - txBuffer));

    // Switch to the command state and destroy the buffer objects
    clientState->parseState = CLIENT_COMMAND;
    delete(request);
    delete(response);
  } catch(Exception *exc) {
    delete(request);
    delete(response);
    throw(new Exception("RemoteIDLServer::handleIDLRequest() : Caught exception", exc));
  }
}
