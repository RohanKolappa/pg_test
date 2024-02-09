// File        : AvbPort.cpp
// Author      : Eldridge M. Mount IV
// Description : Class abstracting an Ethernet port within an AVB system.
//
// Copyright (c) 2010 Lab X Technologies.  All rights reserved.

// System headers
// NOTE - stdint.h must be included prior to socket headers
#include <stdint.h>
#include <sys/socket.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <net/labx_avb/packet_engine_defs.h>
#include <sys/ioctl.h>
#include <unistd.h>

// STL headers
#include <algorithm>
#include <iostream>
#include <list>
#include <sstream>
#include <string.h>

// AVB Library headers
#include "AvbPort.h"
#include "EthernetDefinitions.h"
#include "Exception.h"
#include "LabX.h"

// Namespace using directives
using LabX::Exception;
using LabX::toHexString;
using std::dec;
using std::endl;
using std::find;
using std::hex;
using std::istringstream;
using std::list;
using std::string;
using std::stringstream;
using std::vector;

// Static member initialization

uint16_t Avb::AvbPort::instancesCreated = 0;

// Constructor / Destructor

Avb::AvbPort::AvbPort(const string &name) :
  LinkObserver(),
  Lockable(),
  name(name),
  socketHandle(-1),
  macAddress(NULL),
  observers(),
  redundancyPartner(NULL) {
  instancesCreated++;
}

Avb::AvbPort::~AvbPort(void) {
  // Delete each sub-object
  delete(macAddress);
}

// Public interface methods

const string& Avb::AvbPort::getName(void) const {
  return(name);
}

const uint8_t* Avb::AvbPort::getMacAddress(void) const {
  // Return the cached MAC address if it exists
  if(macAddress == NULL) {
    // Issue an ioctl() to get the MAC address of the port we abstract
    struct ifreq ifRequest;
    ifRequest.ifr_addr.sa_family = AF_INET;
    strncpy(ifRequest.ifr_name, name.c_str(), (IFNAMSIZ - 1));
    ifRequest.ifr_name[IFNAMSIZ - 1] = '\0';
    if(ioctl(getRawSocket(), SIOCGIFHWADDR, &ifRequest) < 0) {
      stringstream excStream;
      excStream << "I/O control operation failed for \""
                << name
                << "\"";
      throw(new Exception(excStream.str()));
    }

    // Copy and save the MAC address bytes into a cached location for later
    macAddress = new uint8_t[MAC_ADDRESS_BYTES];
    memcpy(macAddress, ifRequest.ifr_hwaddr.sa_data, MAC_ADDRESS_BYTES);
  }
  return(macAddress);
}

const Avb::AvbPort::PortState Avb::AvbPort::getPortState(void) const {
  PortState portState = PORT_LINK_DOWN;

  // Issue an ioctl() to get the MAC address of the port we abstract
  struct ifreq ifRequest;
  strncpy(ifRequest.ifr_name, name.c_str(), (IFNAMSIZ - 1));
  ifRequest.ifr_name[IFNAMSIZ - 1] = '\0';
  if(ioctl(getRawSocket(), SIOCGIFFLAGS, &ifRequest) < 0) {
    stringstream excStream;
    excStream << "I/O control operation failed for \""
              << name
              << "\"";
    throw(new Exception(excStream.str()));
  }

  // First determine whether the link is up and running at all
  if(((ifRequest.ifr_flags & IFF_UP) != 0) &
     ((ifRequest.ifr_flags & IFF_RUNNING) != 0)) {
    // Interface is up and running, get the link speed
    // TEMPORARY
    portState = PORT_LINK_1_GBIT;
  }

  return(portState);
}

void Avb::AvbPort::subscribe(AvbPortObserver &observer) const {
  // Atomically add the passed observer, avoiding double-subscription
  lock();
  vector<AvbPortObserver*>::iterator vecIter = find(observers.begin(), observers.end(), &observer);
  if(vecIter == observers.end()) observers.push_back(&observer);

  // Having added an observer, subscribe to link status notifications if it
  // was the first one.
  if(observers.size() == 1) {
    // Rationale for using const_cast<>():
    // While our client may not be permitted to modify us directly, we're okay
    // with the LinkMonitor Observer abstraction having a non-const reference to
    // us since all it can possibly do is call our own notification method (which
    // we have absolute control over).
    LinkMonitor::instance().subscribe(getName(), const_cast<AvbPort&>(*this));
  }
  unlock();
}

void Avb::AvbPort::unsubscribe(AvbPortObserver &observer) const {
  // Atomically remove the passed observer, if present
  lock();
  vector<AvbPortObserver*>::iterator vecIter = find(observers.begin(), observers.end(), &observer);
  if(vecIter != observers.end()) observers.erase(vecIter);

  // We have removed an observer; unsubscribe from link status notifications if
  // there are no others
  if(observers.empty()) {
    // See the const-cast rationale comment in subscribe()...
    LinkMonitor::instance().unsubscribe(getName(), const_cast<AvbPort&>(*this));
  }
  unlock();
}

void Avb::AvbPort::setRedundancyPartner(AvbPort *redundancyPartner) {
  // Test to see whether this has already been set.  No locking is used,
  // since this step should be part of the application initialization
  // prior to lots of threads running around and doing things.
  if(this->redundancyPartner != NULL) {
    stringstream excStream;
    excStream << "Redundancy partner \""
              << this->redundancyPartner->getName()
              << "\" already configured for AVB port \""
              << getName()
              << "\"";
    throw(new Exception(excStream.str()));
  }
  this->redundancyPartner = redundancyPartner;
}

Avb::AvbPort* Avb::AvbPort::getRedundancyPartner(void) const {
  return(redundancyPartner);
}

uint8_t* Avb::AvbPort::convertMacAddress(const string &macAddressString) {
  // Dispatch the call to the more generic mechanism
  try {
    return(convertByteString(macAddressString, MAC_ADDRESS_BYTES));
  } catch(Exception *exc) {
    throw(new Exception("Error parsing MAC address", exc));
  }
}

bool Avb::AvbPort::macAddressesEqual(const uint8_t *macAddressA,
                                     const uint8_t *macAddressB) {
  bool retValue(true);
  for(uint32_t byteIndex = 0; byteIndex < MAC_ADDRESS_BYTES; byteIndex++) {
    retValue &= (macAddressA[byteIndex] == macAddressB[byteIndex]);
  }
  return(retValue);
}

uint8_t* Avb::AvbPort::convertByteString(const string &byteString, 
                                         uint32_t numBytes) {
  uint8_t* byteArray = new uint8_t[numBytes];
  uint32_t tempInteger;
  bool parseFailure = false;

  // First sanity-check the string length against the number of bytes
  if(byteString.length() == ((3 * numBytes) - 1)) {
    // Loop, converting each byte from a ##:##:##:##:##:## hexadecimal format
    uint32_t charIndex = 0;
    for(uint32_t byteIndex = 0; byteIndex < numBytes; byteIndex++, charIndex += 3) {
      istringstream byteStream(byteString.substr(charIndex, 2));
      byteStream >> hex >> tempInteger;
      parseFailure = byteStream.fail();
      if(byteIndex < (numBytes - 1)) parseFailure |= (byteString[charIndex + 2] != ':');
      byteArray[byteIndex] = static_cast<uint8_t>(tempInteger);
    }
  } else parseFailure = true;

  if(parseFailure) {
    delete[] byteArray;
    stringstream excStream;
    excStream << "String \"" << byteString << " is not a "
	      << numBytes << "-byte hex string";
    throw(new Exception(excStream.str()));
  }

  return(byteArray);
}

// Public overrides from class LinkMonitor::LinkObserver
void Avb::AvbPort::linkChanged(const string &portName,
                               const bool linkUp,
                               const bool linkRunning) {
  // TODO: Issue an ioctl() to determine our link speed if it's up and running!
  //       At the moment, we stupidly just assume an active link is 1 Gbit...
  PortState portState = ((linkUp & linkRunning) ? PORT_LINK_1_GBIT : PORT_LINK_DOWN);

  // Atomically make a copy of the observers and notify them
  lock();
  vector<AvbPortObserver*> observersCopy(observers);
  unlock();

  // Iterate through the port's observers, notifying each of them of the change
  for(vector<AvbPortObserver*>::iterator vecIter = observersCopy.begin();
      vecIter != observersCopy.end();
      ++vecIter) {
    (*vecIter)->stateChanged(*this, portState);
  }
}

// Private helper methods

int Avb::AvbPort::getRawSocket(void) const {
  if(socketHandle < 0) {
    // Create a socket through which to get information from the device layer
    socketHandle = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketHandle < 0) {
      stringstream excStream;
      excStream << "Failed to create a raw socket for \"" << name << "\"";
      throw(new Exception(excStream.str()));
    }
  }
  return(socketHandle);
}
