// File        : TcpServerSocket.cpp
// Author      : Eldridge M. Mount IV
// Description : Class abstracting a TCP client socket, capable of 
//               initiating connections over a TCP / IP socket and 
//               sending / receiving data.
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.

// System headers
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// STL headers
#include <iostream>
#include <sstream>

// Library headers
#include "Exception.h"
#include "TcpClientSocket.h"

// Namespace using directives
using std::string;
using std::stringstream;

// Constructor / destructor

LabX::TcpClientSocket::TcpClientSocket(void) :
  Lockable(),
  remoteHostname(""),
  remotePort(0),
  socketFd(-1) {
}

LabX::TcpClientSocket::~TcpClientSocket(void) {
}

// Public interface methods

void LabX::TcpClientSocket::connect(const string &remoteHostname,
                                    uint16_t remotePort) {
  try {
    lock();
    
    struct sockaddr_in serverAddress;
    struct hostent *hostEntry;
  
    // Prevent multiple connects
    if(socketFd > 0) throw(new Exception("Attempt to connect twice"));
  
    // Look up the remote host name
    hostEntry = gethostbyname(remoteHostname.c_str());
    if(hostEntry == NULL) {
      stringstream excStream;
      excStream << "Unable to locate host \""
                << remoteHostname
                << "\"";
      throw(new Exception(excStream.str()));
    }
  
    // Retain the connection parameters
    this->remoteHostname = remoteHostname;
    this->remotePort = remotePort;
  
    // Create a reliable, stream-based socket using TCP
    if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      close(socketFd);
      socketFd = -1;
      throw(new Exception("Error creating TCP client socket"));
    }
  
    serverAddress.sin_family = AF_INET;
    memcpy(&(serverAddress.sin_addr.s_addr), hostEntry->h_addr, hostEntry->h_length);
    serverAddress.sin_port = htons(remotePort);
    int32_t connectValue = ::connect(socketFd, 
                                     reinterpret_cast<struct sockaddr*>(&serverAddress),
                                     sizeof(serverAddress));
    if(connectValue != 0) {
      close(socketFd);
      socketFd = -1;
      throw(new Exception("Error connecting over TCP client socket"));
    }
    
    unlock();
  } catch(Exception *exc) {
    unlock();
    
    throw(new Exception("Error calling connect()", exc));
  }
}

void LabX::TcpClientSocket::disconnect(void) {
  lock();

  // Quietly ignore any repeated disconnect requests, no harm done
  if(socketFd > 0) {
    close(socketFd);
    socketFd = -1;
  }

  unlock();
}

void LabX::TcpClientSocket::sendData(const uint8_t *txBuffer, uint32_t numTxBytes) {
  try {
    lock();
    
    // Make sure we're connected
    if(socketFd < 0) throw(new Exception("Attempted call to sendData() on an unconnected port"));
   
    // Actually send the data to the socket
    if(send(socketFd, txBuffer, numTxBytes, 0) == -1) {
      stringstream excStream;
      excStream << "Error sending data to client, errno = "
                << errno;
      throw(new Exception(excStream.str()));
    }
     
    unlock();
  } catch(Exception *exc) {
    unlock();
    
    throw(new Exception("Error calling sendData()", exc));
  }
}

uint32_t LabX::TcpClientSocket::receiveData(uint8_t *rxBuffer, uint32_t numRxBytes) {
  // Attempt to receive the requested number of bytes, returning once
  // any data at all is received
  int32_t actualRxBytes = recv(socketFd, rxBuffer, numRxBytes, 0);

  // Check for an error, orderly server shutdown, or a delivered signal
  // for the thread (e.g. the thread has been stop()-ped).
  if(actualRxBytes == 0) {
    throw(new Exception("Server performed orderly shutdown"));
  } else if(actualRxBytes < 0) {
    stringstream excStream;
    excStream << "Socket receive error, errno = " << errno;
    throw(new Exception(excStream.str()));
  }

  // No error or shutdown, return the bytes received
  return(static_cast<uint32_t>(actualRxBytes));
}
