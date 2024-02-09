// File        : PtpDomain.cpp
// Author      : Eldridge M. Mount IV
// Description : Wrapper class for the Lab X PTP hardware.  This encapsulates
//               device driver calls and clock / PTP profile configuration behind
//               a high-level interface.

// System headers
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/genetlink.h>

// Device driver headers
//#include <net/labx_ptp/labx_ptp_defs.h>
#include "../../../ptp/labx_ptp_defs.h"

// Lab X Foundation Classes headers
#include "Debug.h"
#include "Exception.h"
#include "LabX.h"

// Library headers
#include "AvbPort.h"
#include "PtpDomain.h"

// Namespace using directives
using LabX::EventHandler;
using LabX::Exception;
using LabX::toHexString;
using std::cout;
using std::endl;
using std::make_pair;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

// Application-specific PTP properties - for the moment, these are
// hard-coded in the class
#define PTP_TIME_SOURCE        PTP_SOURCE_HAND_SET

// Constructor / Destructor

Avb::PtpDomain::PtpDomain(const string &domainName,
                          const string &deviceName,
                          const vector<AvbPort*> &avbPorts) :
  Lockable(),
  domainName(domainName),
  deviceName(deviceName),
  deviceFile(NULL),
  avbPorts(avbPorts),
  eventHandlers() {
}

Avb::PtpDomain::~PtpDomain(void) {
}

// Public interface methods

const string& Avb::PtpDomain::getDomainName(void) const {
  return(domainName);
}

void Avb::PtpDomain::open(int16_t currentUtcOffset,
                          uint8_t grandmasterPriority1,
                          uint8_t grandmasterPriority2,
                          uint8_t clockClass,
                          uint8_t clockAccuracy,
                          uint16_t offsetLogVariance,
                          DelayMechanism delayMechanism) {
#ifdef HARDWARE_STUBS
  return;
#else
  try {
    lock();

    // Guard against multiple calls
    if(deviceFile != NULL) {
      stringstream excStream;
      excStream << "Illegal attempt to open PTP hardware device \""
                << deviceName
                << "\" twice";
      throw(new Exception(excStream.str()));
    }
    
    // Device not yet opened, proceed with the attempt
    DebugNoteVa("Opening PTP hardware device \"%s\"", deviceName.c_str());
    if((deviceFile = fopen(deviceName.c_str(), "w")) == NULL) {
      stringstream excStream;
      excStream << "Couldn't open PTP hardware device file "
                << deviceName
                << " for writing";
      throw(new Exception(excStream.str()));
    }
    deviceHandle = fileno(deviceFile);
    
    // Stop the service while we configure it
    deviceIoControl(IOC_PTP_STOP_SERVICE, NULL);
    
    // Set the source MAC address used for PTP packets
    PtpProperties properties;
    
    // Set properties appropriate for the IEEE 802.1AS PTP profile, using
    // the passed parameters.  Presume the first AVB port is the Grandmanster.
    PtpClockQuality *quality = &properties.grandmasterClockQuality;
    const uint8_t *grandmasterMac = avbPorts.front()->getMacAddress();
    properties.domainNumber           = IEEE_802_1_AS_DOMAIN_NUMBER;
    properties.currentUtcOffset       = currentUtcOffset;
    properties.grandmasterPriority1   = grandmasterPriority1;
    properties.grandmasterPriority2   = grandmasterPriority2;
    properties.timeSource             = PTP_TIME_SOURCE;
    quality->clockClass               = clockClass;
    quality->clockAccuracy            = clockAccuracy;
    quality->offsetScaledLogVariance  = offsetLogVariance;
    properties.grandmasterIdentity[0] = grandmasterMac[0];
    properties.grandmasterIdentity[1] = grandmasterMac[1];
    properties.grandmasterIdentity[2] = grandmasterMac[2];
    properties.grandmasterIdentity[3] = 0xFF;
    properties.grandmasterIdentity[4] = 0xFE;
    properties.grandmasterIdentity[5] = grandmasterMac[3];
    properties.grandmasterIdentity[6] = grandmasterMac[4];
    properties.grandmasterIdentity[7] = grandmasterMac[5];

    // Configure the requested delay mechanism - peer-to-peer or end-to-end
    if(delayMechanism == DELAY_MECHANISM_P2P) {
      properties.delayMechanism = PTP_DELAY_MECHANISM_P2P;
    } else {
      properties.delayMechanism = PTP_DELAY_MECHANISM_E2E;
    }

    // Configure lock detection for the RTC using the recommended defaults
    // from the driver interface header file
    properties.lockRangeNsec    = 10000; // TEMPORARY PTP_DEFAULT_LOCK_RANGE;
    properties.lockTimeMsec     = PTP_DEFAULT_LOCK_TIME;
    properties.unlockThreshNsec = PTP_DEFAULT_UNLOCK_THRESH;
    properties.unlockTimeMsec   = PTP_DEFAULT_UNLOCK_TIME; 

    // Set the requested properties
    deviceIoControl(IOC_PTP_SET_PROPERTIES, &properties);

    // Loop, setting the MAC address for each of the ports
    PtpPortProperties portProperties;
    portProperties.portNumber = 0;
    for(vector<AvbPort*>::const_iterator portIter = avbPorts.begin();
        portIter != avbPorts.end();
        ++portIter, portProperties.portNumber++) {
      const uint8_t *portMac = (*portIter)->getMacAddress();
      for(uint32_t byteIndex = 0; byteIndex < MAC_ADDRESS_BYTES; byteIndex++) {
        portProperties.sourceMacAddress[byteIndex] = portMac[byteIndex];
      }
      deviceIoControl(IOC_PTP_SET_PORT_PROPERTIES, &portProperties);
    }
    
    // Restart the service
    deviceIoControl(IOC_PTP_START_SERVICE, NULL);
    
    // Finally, set the proper time, which will be used as long as we remain
    // a master.  For now, just stick some bogus values in for demonstration.
    //
    // TODO: Need a mechanism to associate board-level RTCs with the instance!
    //       This will permit us to maintain a "real" network time across power 
    //       cycles.
    PtpTime setTime;
    setTime.secondsUpper = 0xABCD;
    setTime.secondsLower = 0xEF012345;
    setTime.nanoseconds = 0xD0C0FFEE;
    deviceIoControl(IOC_PTP_SET_TIME, &setTime);

    // Register with the PTP Netlink client to receive callbacks for packets
    // related to this domain.
    PtpNetlink::instance().registerDomain(properties.domainNumber, this);

    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << "Error opening PTP domain \""
              << getDomainName()
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
#endif
}

void Avb::PtpDomain::close(void) {
  // Stop the service, close and null the device if it has been opened
  try {
    lock();

    if(deviceFile != NULL) {
      DebugNoteVa("Stopping PTP domain %s syntonization\n", getDomainName().c_str());
      deviceIoControl(IOC_PTP_STOP_SERVICE, NULL);
      fclose(deviceFile);
      deviceFile = NULL;
    }

    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << "Error closing PTP domain \""
              << getDomainName()
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
}

void Avb::PtpDomain::setCoefficients(int32_t pCoefficient,
                                     int32_t iCoefficient,
                                     int32_t dCoefficient) {
  try {
    lock();

    // Copy the supplied coefficients and issue an ioctl
    PtpCoefficients coefficients;
    coefficients.P = pCoefficient;
    coefficients.I = iCoefficient;
    coefficients.D = dCoefficient;
    deviceIoControl(IOC_PTP_SET_RTC_COEF, reinterpret_cast<void*>(&coefficients));
    
    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << "Error setting coefficients for PTP domain \""
              << getDomainName()
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
}

// Private helper methods

void Avb::PtpDomain::deviceIoControl(uint32_t ioCommand, void *ioParameter) {
#ifdef HARDWARE_STUBS
  return;
#else
  // Ensure the device is open at all
  if(deviceFile == NULL) {
    stringstream excStream;
    excStream << "Attempted I/O control of PTP domain \""
              << getDomainName()
              << "\" prior to call to open()";
    throw(new Exception(excStream.str()));
  }

  // Invoke the system call and check for success
  if(ioctl(deviceHandle, ioCommand, ioParameter) < 0) {
    stringstream excStream;
    excStream << "I/O control command ("
              << ioCommand
              << ") failed for PTP domain \""
              << getDomainName()
              << "\"";
    throw(new Exception(excStream.str()));
  }
#endif
}

#ifdef INCLUDE_DEBUG_INFO
void Avb::PtpDomain::dumpDebugInfo(std::ostream& stream)
{
#ifdef HARDWARE_STUBS
  return;
#else
  try {
    lock();

    PtpAsPortDataSet dataSet;
    PtpAsPortStatistics stats;
    int index = 0;

    dataSet.index = index;
    stats.index = index;

    while ((ioctl(deviceHandle, IOC_PTP_GET_AS_PORT_DATA_SET, &dataSet) >= 0) &&
           (ioctl(deviceHandle, IOC_PTP_GET_AS_PORT_STATISTICS, &stats) >= 0)) {
      stream << "PTP Port Number: " << dataSet.portNumber << std::endl;
      
      stream << "  portRole         : ";
      switch (dataSet.portRole) {
      case PTP_MASTER:
        stream << "master" << std::endl;
        break;

      case PTP_SLAVE:
        stream << "slave" << std::endl;
        break;

      case PTP_PASSIVE:
        stream << "passive" << std::endl;
        break;

      case PTP_DISABLED:
        stream << "disabled" << std::endl;
        break;

      default:
        stream << "invalid" << std::endl;
        break;
      }

      stream << "  pttPortEnabled   : " << ((dataSet.pttPortEnabled != 0) ? "true" : "false") << std::endl;
      stream << "  isMeasuringDelay : " << ((dataSet.isMeasuringDelay != 0) ? "true" : "false") << std::endl;
      stream << "  asCapable        : " << ((dataSet.asCapable != 0) ? "true" : "false") << std::endl;
      stream << "  neighborPropDelay: " << LabX::toHexString(dataSet.neighborPropDelay, 16) << std::endl;
      stream << "  neighborRateRatio: " << LabX::toHexString(dataSet.neighborRateRatio, 16) << std::endl;
      stream << std::endl;
      stream << "  rxSyncCount                            : " << stats.rxSyncCount << std::endl;
      stream << "  rxFollowupCount                        : " << stats.rxFollowupCount << std::endl;
      stream << "  rxPDelayRequestCount                   : " << stats.rxPDelayRequestCount << std::endl;
      stream << "  rxPDelayResponseCount                  : " << stats.rxPDelayResponseCount << std::endl;
      stream << "  rxPDelayResponseFollowupCount          : " << stats.rxPDelayResponseFollowupCount << std::endl;
      stream << "  rxAnnounceCount                        : " << stats.rxAnnounceCount << std::endl;
      stream << "  rxPTPPacketDiscardCount                : " << stats.rxPTPPacketDiscardCount << std::endl;
      stream << "  syncReceiptTimeoutCount                : " << stats.syncReceiptTimeoutCount << std::endl;
      stream << "  announceReceiptTimeoutCount            : " << stats.announceReceiptTimeoutCount << std::endl;
      stream << "  pDelayAllowedLostResponsesExceededCount: " << stats.pDelayAllowedLostResponsesExceededCount << std::endl;
      stream << "  txSyncCount                            : " << stats.txSyncCount << std::endl;
      stream << "  txFollowupCount                        : " << stats.txFollowupCount << std::endl;
      stream << "  txPDelayRequestCount                   : " << stats.txPDelayRequestCount << std::endl;
      stream << "  txPDelayResponseCount                  : " << stats.txPDelayResponseCount << std::endl;
      stream << "  txPDelayResponseFollowupCount          : " << stats.txPDelayResponseFollowupCount << std::endl;
      stream << "  txAnnounceCount                        : " << stats.txAnnounceCount << std::endl;

      index++;
      dataSet.index = index;
      stats.index = index;
    }

    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << "Error dumping debug info for PTP domain \""
              << getDomainName()
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
#endif
}
#endif // INCLUDE_DEBUG_INFO

void Avb::PtpDomain::registerHandler(EventHandler *handler) {
  // Atomically add the passed handler
  lock();
  eventHandlers.push_back(handler);
  unlock();
}

void Avb::PtpDomain::handlePtpEvent(PtpEvent &ptpEvent) {
  DebugNoteVa("PtpDomain \"%s\" handling event: \"%s\"", 
              getDomainName().c_str(), ptpEvent.toString().c_str());

  // Lock the instance and copy the event handlers collection for processing
  lock();
  vector<EventHandler*> handlersCopy(eventHandlers);
  unlock();

  // Iterate through the handlers, dispatching the event
  for(vector<EventHandler*>::iterator handlerIter = handlersCopy.begin();
      handlerIter != handlersCopy.end();
      ++handlerIter) {
    (*handlerIter)->dispatch(ptpEvent);
  }

  // Respond to specific events now that handlers have had a crack at them
  switch(ptpEvent.getEventType()) {
  case PtpEvent::PTP_EVENT_GM_CHANGE:
    // A Grandmaster change has occurred; prior to notifying us, the driver
    // has disabled its ability to slew the PTP hardware until we (userspace)
    // get a chance to disengage anything from the PTP increment / make other
    // changes.  Now that any register handlers have been informed and allowed
    // to respond, we explicitly re-enabled the driver's authority to set and
    // slew the RTC.
    deviceIoControl(IOC_PTP_ACK_GM_CHANGE, NULL);
    break;

  default:
    // Do nothing specific
    ;
  }
}

// Private inner classes

// Class PtpNetlink

// Static member initialization
Avb::PtpDomain::PtpNetlink* Avb::PtpDomain::PtpNetlink::singleton = NULL;

// Destructor
Avb::PtpDomain::PtpNetlink::~PtpNetlink(void) {}

// Public interface methods

Avb::PtpDomain::PtpNetlink& Avb::PtpDomain::PtpNetlink::instance(void) {
  // Create and start the Singleton if necessary
  if(singleton == NULL) {
    singleton = new PtpNetlink();
    singleton->start();
  }
  return(*singleton);
}

void Avb::PtpDomain::PtpNetlink::registerDomain(uint8_t domainNumber, 
                                                PtpDomain *ptpDomain) {
  // Atomically add the instance to the map, guarding against multiple
  // registrations within the same domain number
  lock();
  map<uint8_t, PtpDomain*>::iterator mapIter = domainMap.find(domainNumber);
  if(mapIter != domainMap.end()) {
    unlock();
    
    stringstream excStream;
    excStream << "Attempt to register a second PTP under domain number"
              << toHexString(static_cast<uint32_t>(domainNumber), 2);
    throw(new Exception(excStream.str()));
  }

  // Insert the domain onto the map
  domainMap.insert(make_pair(domainNumber, ptpDomain));
  unlock();
}

// Protected overrides from class NetlinkClient
void Avb::PtpDomain::PtpNetlink::handleFamilyMessage(struct nlmsghdr *nlh, 
                                                     const string& multicastGroup) {
  struct genlmsghdr *ghdr = reinterpret_cast<struct genlmsghdr*>(NLMSG_DATA(nlh));
  struct rtattr *attrTable[PTP_EVENTS_A_MAX + 1];
  struct rtattr *attrs = (struct rtattr*) (((uint8_t*) NLMSG_DATA(nlh)) + GENL_HDRLEN);
  uint32_t msgLength = (nlh->nlmsg_len - NLMSG_LENGTH(GENL_HDRLEN));

  // Parse an attribute table from the message
  parse_rtattr(attrTable, PTP_EVENTS_A_MAX, attrs, msgLength);

  // Regardless of what group this event is from, it must carry a PTP domain number.
  if(attrTable[PTP_EVENTS_A_DOMAIN] == 0) return;
  uint8_t domainNumber = *reinterpret_cast<uint8_t*>(RTA_DATA(attrTable[PTP_EVENTS_A_DOMAIN]));

  // Each event instance can also contain an optional array of key / value pairs
  // encoded as strings.  See if this is required for the instance.
  vector<string> pairStrings;
  if(attrTable[PTP_EVENTS_A_VALUEMAP] != 0) {
    // The key / value map is stored as a nested attribute.  Allocate space for
    // the length as well as the pairs themselves.
    struct rtattr *attrTableValuemap[PTP_VALUEMAP_MAX_PAIRS + 2];
    
    parse_rtattr_nested(attrTableValuemap, 
                        (PTP_VALUEMAP_MAX_PAIRS + 1), 
                        attrTable[PTP_EVENTS_A_VALUEMAP]);
    uint32_t numPairs = 
      *reinterpret_cast<uint32_t*>(RTA_DATA(attrTableValuemap[PTP_VALUEMAP_A_LENGTH]));
    if(numPairs <= PTP_VALUEMAP_MAX_PAIRS) {
      // Valid key / value map, extract the pairs for the event instance
      for(uint32_t pairIndex = 0; pairIndex < numPairs; pairIndex++) {
        pairStrings.push_back(reinterpret_cast<char*>(RTA_DATA(attrTableValuemap[PTP_VALUEMAP_A_PAIRS + pairIndex])));
      }
    }
  }
  
  // Create an event instance to encapsulate the message, mapping from the kernel-defined
  // command enumeration to the one defined by this class
  PtpEvent *ptpEvent = NULL;
  switch(ghdr->cmd) {
  case PTP_EVENTS_C_HEARTBEAT:
    ptpEvent = new PtpEvent(PtpEvent::PTP_EVENT_HEARTBEAT, pairStrings);
    break;

  case PTP_EVENTS_C_GM_CHANGE:
    ptpEvent = new PtpEvent(PtpEvent::PTP_EVENT_GM_CHANGE, pairStrings);
    break;

  case PTP_EVENTS_C_RTC_LOCK:
    ptpEvent = new PtpEvent(PtpEvent::PTP_EVENT_RTC_LOCK, pairStrings);
    break;

  case PTP_EVENTS_C_RTC_UNLOCK:
    ptpEvent = new PtpEvent(PtpEvent::PTP_EVENT_RTC_UNLOCK, pairStrings);
    break;

  default:
    // Unrecognized, just issue a warning in debug mode
    DebugWarningVa("(%s) : Received unknown Netlink command %d", getName().c_str(), ghdr->cmd);
  }

  // Dispatch the event to the event handlers, if one was created
  if(ptpEvent != NULL) {
    // Lock the instance and dispatch the event to the corresponding domain object
    PtpDomain *domainPtr = NULL;
    lock();
    map<uint8_t, PtpDomain*>::iterator mapIter = domainMap.find(domainNumber);
    if(mapIter != domainMap.end()) domainPtr = mapIter->second;
    unlock();

    if(domainPtr != NULL) domainPtr->handlePtpEvent(*ptpEvent);

    // Delete the event instance once complete
    delete(ptpEvent);
  }
}

// Private constructor
Avb::PtpDomain::PtpNetlink::PtpNetlink(void) :
  NetlinkClient(PTP_EVENTS_FAMILY_NAME),
  domainMap() {
}
