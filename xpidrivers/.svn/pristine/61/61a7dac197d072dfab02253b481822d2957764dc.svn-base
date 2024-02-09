// File        : TcpServerSocket.cpp
// Author      : Eldridge M. Mount IV
// Description : Class abstracting a TCP server socket, capable of accepting 
//               connections over a TCP / IP socket and dispatching asynchronously-
//               received stream data to a polymorphic subclass within an independent 
//               thread of control
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.

// System headers
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// STL headers
#include <iostream>
#include <sstream>

// #define _LABXDEBUG

// Library headers
#include "Debug.h"
#include "Exception.h"
#include "TcpServerSocket.h"

// Namespace using directives
using std::string;
using std::stringstream;
using std::vector;

// Size, in bytes, of each per-instance Rx buffer
#define RX_BUFFER_SIZE  (4096)

// Static member initialization

// Constructor / destructor

LabX::TcpServerSocket::TcpServerSocket(const string &serverName,
                                       uint16_t listenPort,
                                       uint32_t maxClients) :
  Thread("TcpServer:" + serverName),
  Lockable(),
  serverName(serverName),
  listenPort(listenPort),
  maxClients(maxClients),
  listenFd(-1),
  activeClients() {
  // Allocate a receive buffer
  rxBuffer = new uint8_t[RX_BUFFER_SIZE];
}

LabX::TcpServerSocket::~TcpServerSocket(void) {
  delete[](rxBuffer);
}

// Public interface methods

void LabX::TcpServerSocket::start(void) {
  try {
    lock();

    struct sockaddr_in serverAddress;
    int32_t yes = 1;

    if((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      throw(new Exception("Error creating TCP/IP socket"));
    }

    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int32_t)) == -1) {
      ::close(listenFd);
      listenFd = -1;
      throw(new Exception("Error setting socket options"));
    }

    // Create the server address structure and bind the port to it
    memset(&serverAddress, 0x00, sizeof(struct sockaddr_in));
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons(listenPort);
    memset(&(serverAddress.sin_zero), '\0', sizeof(serverAddress.sin_zero));

    if(bind(listenFd, 
            reinterpret_cast<struct sockaddr*>(&serverAddress), 
            sizeof(serverAddress)) == -1) {
      ::close(listenFd);
      listenFd = -1;

      stringstream excStream;
      excStream << "Error binding to TCP port "
                << listenPort
                << ", errno = "
                << errno;
      throw(new Exception(excStream.str()));
    }

    // Finally, set up to listen on the socket, providing the maximum
    // number of clients as the "backlog" parameter.
    if(listen(listenFd, maxClients) == -1) {
      ::close(listenFd);
      listenFd = -1;

      stringstream excStream;
      excStream << "Error listening on TCP port "
                << listenPort
                << ", errno = "
                << errno;
      throw(new Exception(excStream.str()));
    }

    // Socket has been set up, begin the worker thread
    Thread::start();

    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << "Error opening TCP server socket \""
              << serverName
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
}

void LabX::TcpServerSocket::stop(void) {
  try {
    lock();

    // Guard against multiple stops
    if(listenFd < 0) throw(new Exception("Attempt to stop() stopped TcpServerSocket"));

    // Stop the worker thread, close the socket, and wait for the worker
    Thread::stop();
    ::close(listenFd);
    Thread::join();

    // Clear the listening descriptor and close all clients
    listenFd = -1;
    while(!activeClients.empty()) {
      ::close(activeClients.back()->clientFd);
      delete(activeClients.back());
      activeClients.pop_back();
    }

    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << "Error closing TCP server socket \""
              << serverName
              << "\"";
    throw(new Exception(excStream.str(), exc));
  } 
}

// Public interface methods

const string& LabX::TcpServerSocket::getName(void) const {
  return(serverName);
}

// Public overrides from class Thread

int32_t LabX::TcpServerSocket::run(void) {
  fd_set masterSet;
  fd_set readSet;
  fd_set exceptionSet;
  int32_t maxDescriptor;

  // Clear the file descriptor sets and add the listen socket descriptor
  // to the master set.  Initially, the highest descriptor in the set
  // is the listener.
  FD_ZERO(&masterSet);
  FD_ZERO(&readSet);
  FD_ZERO(&exceptionSet);
  FD_SET(listenFd, &masterSet);
  maxDescriptor = listenFd;

  // Loop forever, listening on the main socket and any clients
  while(true) {
    // Copy the master descriptor set in preparation and call select() to
    // multiplex all the descriptors' I/O events
    readSet = masterSet;
    exceptionSet = masterSet;
    if(select((maxDescriptor + 1), &readSet, NULL, &exceptionSet, NULL) == -1) {
      ::close(listenFd);
      listenFd = -1;

      stringstream excStream;
      excStream << "Call to select() failed for TCP server \""
                << serverName
                << "\", errno = "
                << errno;
      throw(new Exception(excStream.str()));
    }

    // See if this is a new connection request on the listen socket
    if(FD_ISSET(listenFd, &readSet)) {
      // Accept the new connection, creating a new socket for the client
      ClientEntry *newClient = new ClientEntry();
      socklen_t addressLength = sizeof(newClient->clientAddress);
      newClient->clientFd = accept(listenFd, 
                                   reinterpret_cast<struct sockaddr*>(&newClient->clientAddress),
                                   &addressLength);
      if(newClient->clientFd == -1) {
        // Accept has failed, destroy the new client and print a debug warning; not
        // sure if this may simply occur due to network errors, so we don't want to
        // throw an exception and kill the thread.
        delete(newClient);
        DebugWarningVa("Error %d in new client accept()", errno);
      } else {
        // Set the TCP_KEEPALIVE option on the socket
        int32_t yes = 1;
        if(setsockopt(newClient->clientFd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int32_t)) == -1) {
          // KEEPALIVE may not be enabled within the kernel, just warn
          DebugWarningVa("Unable to set TCP_KEEPALIVE on server \"%s\"", serverName.c_str());
        }

        // Add the new socket to the master set and see if this represents a
        // new maximum
        FD_SET(newClient->clientFd, &masterSet);
        if(newClient->clientFd > maxDescriptor) maxDescriptor = newClient->clientFd;

        // Extract the port and IP address in the native endian-ness for the
        // client's convenience
        uint32_t ipAddressWord = ntohl(newClient->clientAddress.sin_addr.s_addr);
        uint8_t *bytePtr = reinterpret_cast<uint8_t*>(&ipAddressWord);
        for(uint32_t byteIndex = 0; byteIndex < k_IpAddressBytes; byteIndex++) {
          newClient->sourceIpAddress[byteIndex] = bytePtr[byteIndex];
        }
        newClient->sourceTcpPort = ntohs(newClient->clientAddress.sin_port);

        // Atomically add the new client and inform the derived class
        lock();
        activeClients.push_back(newClient);
        addClient(*newClient);
        unlock();
      } // if(accept failed)
    }
    
    // Iterate through the existing connections, seeing if there is any new data to be
    // read from any of them.  Create a copy of the clients so as to permit modification
    // of the original vector.
    vector<ClientEntry*> activeClientsCopy(activeClients);
    for(vector<ClientEntry*>::iterator clientIter = activeClientsCopy.begin();
        clientIter != activeClientsCopy.end();
        ++clientIter) {
      ClientEntry *clientPtr = *clientIter;

      // See if the client socket is set in the read set
      if(FD_ISSET(clientPtr->clientFd, &readSet)) {
        // Receive data from the socket and presume the client will not be removed
        ssize_t numRxBytes = recv(clientPtr->clientFd, rxBuffer, RX_BUFFER_SIZE, 0);
        bool disconnectClient = false;
        if(numRxBytes == 0) {
          // Client has performed an orderly shutdown, disconnect it.
          disconnectClient = true;
        } else if(numRxBytes < 0) {
          // Error condition encountered upon receiving for the client
          lock();
          handleClientError(*clientPtr);
          unlock();
          disconnectClient = true;
        } else {
          // Valid data bytes received, pass them to the derived class
          lock();
          disconnectClient = handleClientData(*clientPtr, 
                                              rxBuffer, 
                                              static_cast<uint32_t>(numRxBytes));
          unlock();
        }

        // Disconnect the client if required or requested
        if(disconnectClient) {
          DebugNote("Disconnecting client");

          // First inform the derived class, then close and remove the descriptor and
          // destroy the entry.
          lock();
          removeClient(*clientPtr);
          ::close(clientPtr->clientFd);
          FD_CLR(clientPtr->clientFd, &masterSet);

          // Iterate through the "official" vector of clients to remove, and update the
          // "maximum descriptor" as we go
          maxDescriptor = listenFd;
          for(vector<ClientEntry*>::iterator eraseIter = activeClients.begin();
              eraseIter != activeClients.end();) {
            if((*eraseIter)->clientFd == clientPtr->clientFd) {
              // Use the return value from the erase() operation to advance the iterator
              // to the next element
              eraseIter = activeClients.erase(eraseIter);
            } else {
              // This is not the entry to remove; update the maximum descriptor and
              // advance the iterator
              if((*eraseIter)->clientFd > maxDescriptor) maxDescriptor = (*eraseIter)->clientFd;
              ++eraseIter;
            }
          }
          unlock();
        } // if(disconnect client)
      } // if(read set)

      // See if the client socket is set in the exception set
      if(FD_ISSET(clientPtr->clientFd, &exceptionSet)) {
        DebugWarningVa("Client socket exception in server \"%s\"", serverName.c_str());
      }
    }
  } // while(true)

  return(Thread::k_Success);
}

// Protected interface methods

void LabX::TcpServerSocket::sendData(const ClientEntry &clientEntry,
                                     const uint8_t *txBuffer, 
                                     uint32_t numTxBytes) {
  // Actually send the data to the socket
  if(send(clientEntry.clientFd, txBuffer, numTxBytes, 0) == -1) {
    stringstream excStream;
    excStream << "Error sending data to client, errno = "
              << errno;
    throw(new Exception(excStream.str()));
  }
}
