// File        : LinkMonitor.cpp
// Author      : Eldridge M. Mount IV
// Description : An active class which monitors link status changes on
//               network interfaces, broadcasting events to interested
//               clients.

// System headers
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <unistd.h>

// STL headers
#include <algorithm>
#include <iostream>

// #define _LABXDEBUG

// Lab X Foundation Class headers
#include "Debug.h"
#include "Exception.h"
#include "LabX.h"

// Lab X AVB library headers
#include "LinkMonitor.h"

// Namespace using directives
using Avb::LinkMonitor;
using LabX::Exception;
using LabX::toHexString;
using std::cerr;
using std::cout;
using std::endl;
using std::find;
using std::map;
using std::string;
using std::vector;

// Static member initialization
LinkMonitor *LinkMonitor::linkMonitorInstance = NULL;

// Message buffer for packet reception
#define NETLINK_BUF_SIZE  (4096)
static uint8_t packetBuffer[NETLINK_BUF_SIZE];

// Destructor

LinkMonitor::~LinkMonitor(void) {
}

// Public overrides from class Thread

void LinkMonitor::start(void) {
  try {
    lock();

    // Guard against multiple starts
    if(netlinkFd != -1) {
      throw(new Exception("Avb::LinkMonitor::start() : Attempt to start running instance"));
    }

    // Create a netlink socket for listening to link-layer datagrams
    if((netlinkFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1) {
      throw(new Exception("Couldn't create netlink socket"));
    }

    // Create a netlink address for link-layer multicast addresses and bind
    // it to the created socket
    struct sockaddr_nl netlinkAddress;
    memset(&netlinkAddress, 0, sizeof(netlinkAddress));
    netlinkAddress.nl_family = AF_NETLINK;
    netlinkAddress.nl_groups = RTMGRP_LINK;
    if(bind(netlinkFd, (struct sockaddr*) &netlinkAddress, sizeof(netlinkAddress)) == -1) {
      close(netlinkFd);
      throw(new Exception("Couldn't bind address to netlink socket"));
    }

    // Call the base class implementation to actually kick off the worker thread
    Thread::start();

    unlock();
  } catch(Exception *exc) {
    unlock();

    throw(new Exception("Avb::LinkMonitor::start() : Caught exception", exc));
  }
}

void LinkMonitor::stop(void) {
  try {
    lock();
    
    // Guard against multiple stops
    if(netlinkFd < 0) throw(new Exception("Attempt to stop() stopped LinkMonitor"));

    // Clear the map of observers, stop the worker thread and close the
    // socket to ensure it is unblocked
    observerMap.clear();
    Thread::stop();
    close(netlinkFd);
    netlinkFd = -1;

    unlock();
  } catch(Exception *exc) {
    unlock();

    throw(new Exception("Avb::LinkMonitor::stop() : Caught exception", exc));
  }
}

int32_t LinkMonitor::run(void) {
  struct iovec ioVector = { 
    (void *) packetBuffer, 
    NETLINK_BUF_SIZE
  };
  struct msghdr messageHeader;
  struct nlmsghdr *netlinkHeader;
  struct sockaddr_nl netlinkAddress;
  ssize_t packetLength;

  try {
    // Worker thread runs continuously until stopped
    while(true) {
      // Continuously receive packets matching link events
      memset(&netlinkAddress, 0, sizeof(netlinkAddress));
      netlinkAddress.nl_family     = AF_NETLINK;
      netlinkAddress.nl_groups     = RTMGRP_LINK;
      messageHeader.msg_name       = &netlinkAddress;
      messageHeader.msg_namelen    = sizeof(netlinkAddress);
      messageHeader.msg_iov        = &ioVector;
      messageHeader.msg_iovlen     = 1;
      messageHeader.msg_control    = NULL;
      messageHeader.msg_controllen = 0;
      messageHeader.msg_flags      = 0;

      // Receive the next packet
      packetLength = recvmsg(netlinkFd, &messageHeader, 0);

      DebugNoteVa("Received %d-byte Netlink packet", packetLength);

      // Packet(s) received, process them
      for(netlinkHeader = reinterpret_cast<struct nlmsghdr*>(packetBuffer);
          NLMSG_OK(netlinkHeader, packetLength);
          netlinkHeader = NLMSG_NEXT(netlinkHeader, packetLength)) {
        // Test for the end of a multipart message
        if (netlinkHeader->nlmsg_type == NLMSG_DONE) break;

        // Process error conditions
        if(netlinkHeader->nlmsg_type == NLMSG_ERROR) {
          struct nlmsgerr *err = 
            reinterpret_cast<struct nlmsgerr*>(NLMSG_DATA(netlinkHeader));
          if(netlinkHeader->nlmsg_len >= NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
            /* Incoherent Netlink header, bail on this iteration */
            DebugWarningVa("Netlink error: %d", err->error);

            break;
          }
        }

        // Switch on the type of link packet
        switch(netlinkHeader->nlmsg_type) {
        case RTM_NEWLINK:
        case RTM_DELLINK:
          handleLinkUpDown(netlinkHeader);
          break;

        default:
          // Unhandled message type
          ;
        }
      }
    } // while(true)
  } catch(...) {
    DebugWarning("LinkMonitor caught unknown exception");
    throw;
  }

  DebugNote("LinkMonitor worker thread exiting");

  return(Thread::k_Success);
}

// Public interface methods

void LinkMonitor::subscribe(const string &portName, LinkObserver &observer) {
  // Atomically add the passed observer, avoiding double-subscription
  lock();
  map<string, vector<LinkObserver*> >::iterator mapIter = observerMap.find(portName);

  // Add a new vector to the map for the new port if one doesn't exist yet
  if(mapIter == observerMap.end()) {
    observerMap.insert(make_pair(portName, vector<LinkObserver*>(1, &observer)));
  } else {
    // Found an existing vector
    vector<LinkObserver*> &portVector(mapIter->second);
    vector<LinkObserver*>::iterator vecIter = find(portVector.begin(), portVector.end(), &observer);
    if(vecIter == portVector.end()) portVector.push_back(&observer);
  }
  unlock();
}

void LinkMonitor::unsubscribe(const string &portName, LinkObserver &observer) {
  // Atomically remove the passed observer, if present
  lock();
  map<string, vector<LinkObserver*> >::iterator mapIter = observerMap.find(portName);
  if(mapIter != observerMap.end()) {
    vector<LinkObserver*> &portVector(mapIter->second);
    vector<LinkObserver*>::iterator vecIter = find(portVector.begin(), portVector.end(), &observer);
    if(vecIter != portVector.end()) portVector.erase(vecIter);
  }
  unlock();
}

// Static Singleton interface

LinkMonitor& LinkMonitor::instance(void) {
  if(linkMonitorInstance == NULL) {
    // Create and start the Singleton
    linkMonitorInstance = new LinkMonitor();
    linkMonitorInstance->start();
  }
  return(*linkMonitorInstance);
}

void LinkMonitor::shutdown(void) {
  // Stop and join with the Singleton's worker thread, then destroy it
  if(linkMonitorInstance != NULL) {
    DebugNote("Shutting down and destroying LinkMonitor Singleton");

    linkMonitorInstance->stop();
    linkMonitorInstance->join();
    delete(linkMonitorInstance);
    linkMonitorInstance = NULL;

    DebugNote("LinkMonitor shut down");
  }
}

// Temporary buffer used to cache interface names
static char nameBuffer[IF_NAMESIZE];

void LinkMonitor::handleLinkUpDown(struct nlmsghdr *netlinkHeader) {
  struct ifinfomsg *infoMessage = 
    reinterpret_cast<struct ifinfomsg*>(NLMSG_DATA(netlinkHeader));
  bool linkUp = ((infoMessage->ifi_flags & IFF_UP) != 0);
  bool linkRunning = ((infoMessage->ifi_flags & IFF_RUNNING) != 0);
  string portName(if_indextoname(infoMessage->ifi_index, nameBuffer));

  DebugNoteVa("Interface \"%s\" up: %s, running: %s",
              portName.c_str(),
              (linkUp ? "true" : "false"),
              (linkRunning ? "true" : "false"));

  // Look for any observers interested in this interface.  Atomically copy
  // the list of observers, if one exists, and then process callbacks outside
  // of the locking scope.
  lock();
  map<string, vector<LinkObserver*> >::iterator mapIter = observerMap.find(portName);
  if(mapIter != observerMap.end()) {
    // Actually make a deep copy of the collection
    vector<LinkObserver*> observerVec(mapIter->second);
    unlock();

    // Iterate through the port's observers, notifying each of them of the change
    for(vector<LinkObserver*>::iterator vecIter = observerVec.begin();
        vecIter != observerVec.end();
        ++vecIter) {
      (*vecIter)->linkChanged(portName, linkUp, linkRunning);
    }
  } else unlock(); // No observers exist for the port
}

// Private constructor

LinkMonitor::LinkMonitor(void) :
  Thread("Link Monitor"),
  Lockable(),
  netlinkFd(-1),
  observerMap() {
}
