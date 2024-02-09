/**
 * An interface object for communicating over the Generic Netlink 
 * bus using a given protocol family.
 *
 * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
 * @author Eldridge M. Mount IV
 */

// System headers
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Library headers
#include "Debug.h"
#include "Exception.h"
#include "LabX.h"
#include "NetlinkClient.h"

// Namespace using directives
using LabX::toHexString;
using std::cout;
using std::endl;
using std::find;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

// Maximum number of multicast groups per family
#define GENL_MAX_FAM_GRPS  (32)

// Arbitrary starting point for our message sequence IDs
#define SEQUENCE_ID_START  (32)

// Initial value to indicate a new port ID hasn't yet been created
#define PORT_ID_INVALID (0xFFFFFFFF)

// Bit field to match any multicast address
#define MULTICAST_ANY (0xFFFFFFFF)

// Number of bits to shift the process ID up for creating port IDs
#define PROCESS_ID_SHIFT (22)

// Corresponding mask for instance count in port ID creation
#define INSTANCE_COUNT_MASK (0x03FF)

// Timeout, in milliseconds, to be used for Netlink responses
#define NETLINK_TIMEOUT_MSEC (2500)

// Semaphore used to serialize access to the class' resources
LabX::Semaphore LabX::NetlinkClient::classResourcesSem("NetlinkClientSem");

// Class instance count
uint32_t LabX::NetlinkClient::instanceCount = 0;

// Constructor / Destructor

LabX::NetlinkClient::NetlinkClient(const string &familyName) :
  Thread("NetlinkClient: " + familyName),
  Lockable(),
  familyName(familyName),
  netlinkFd(-1),
  sequenceId(SEQUENCE_ID_START),
  familyId(0),
  portId(PORT_ID_INVALID),
  multicastGroupMap(),
  pendingRequests(),
  collectionSem() {
}

LabX::NetlinkClient::~NetlinkClient(void) {
  // Destroy any pending requests
  while(!pendingRequests.empty()) {
    delete(pendingRequests.back());
    pendingRequests.pop_back();
  }
}

// Public overrides from class Thread

void LabX::NetlinkClient::start(void) {
  try {
    lock();

    // Guard against multiple starts
    if(isRunning()) {
      stringstream excStream;
      excStream << getName() << " : Attempted multiple call to start()";
      throw(new Exception(excStream.str()));
    }

    // First, create a unique port ID for use with the instance.  This is made
    // unique throughout the process and the system by combining the process ID
    // along with an instance count per process.  The instance count is bumped
    // as instances are started, not created, to avoid potentially throwing an
    // exception in the constructor.
    if(portId == PORT_ID_INVALID) {
      portId = (getpid() << PROCESS_ID_SHIFT);
      classResourcesSem.wait();
      portId |= (instanceCount++ & INSTANCE_COUNT_MASK);
      classResourcesSem.post();
    }

    // Create a netlink socket for Generic Netlilnk
    if((netlinkFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC)) == -1) {
      throw(new Exception("Couldn't create netlink socket"));
    }

    // Create a Netlink source address and bind it to the created socket.
    // Configure to receive any multicast address.
    memset(&sourceAddress, 0, sizeof(sourceAddress));
    sourceAddress.nl_family = AF_NETLINK;
    sourceAddress.nl_pid    = portId;
    sourceAddress.nl_groups = MULTICAST_ANY;
    if(bind(netlinkFd, (struct sockaddr*) &sourceAddress, sizeof(sourceAddress)) == -1) {
      close(netlinkFd);
      throw(new Exception("Couldn't bind address to netlink socket"));
    }

    // Invoke our base class' implementation to start the thread; set the family ID to
    // zero first to make sure it gets re-acquired
    familyId = 0;
    Thread::start();

    // Send a message requesting information about the Generic Netlink family; 
    // we may need to listen on multicast groups.
    sendGetFamilyMessage();

    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << getName() << " : Error attempting to start thread";
    throw(new Exception(excStream.str(), exc));
  }
}

int32_t LabX::NetlinkClient::run(void) {
  struct nlmsghdr *nlh;
  struct sockaddr_nl sourceAddress;
  struct iovec iov;
  struct msghdr msg;
  ssize_t rxLength;

  // Loop, receiving messages from the Netlink socket
  while(true) {
    // Set up the IO vector and receive a message
    iov.iov_base = (void*) rxBuffer;
    iov.iov_len  = sizeof(rxBuffer);

    memset(&msg, 0, sizeof(msg));
    msg.msg_name    = (void*) &sourceAddress;
    msg.msg_namelen = sizeof(sourceAddress);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    rxLength = recvmsg(netlinkFd, &msg, 0);

    // Parse the received message payload, handling multi-part messages
    for(nlh = (struct nlmsghdr*) rxBuffer;
        NLMSG_OK(nlh, rxLength);
        nlh = NLMSG_NEXT(nlh, rxLength)) {
      switch(nlh->nlmsg_type) {
      case GENL_ID_CTRL: 
        // Handle the received control message
        handleControlMessage(nlh);
        break;
        
      case NLMSG_ERROR:
        // Handle the error message
        handleErrorMessage(nlh);
        break;

      case NLMSG_DONE:
        // End of a multi-part message, continue
        continue;

      default:
        // See if this message belongs to our family ID, if it is known yet
        if((familyId != 0) & (nlh->nlmsg_type == familyId)) {
          // Determine whether this belongs to any known multicast group
          string multicastGroup("");
          collectionSem.wait();
          for(map<string, uint32_t>::const_iterator groupIter = multicastGroupMap.begin();
              groupIter != multicastGroupMap.end();
              ++groupIter) {
            if((sourceAddress.nl_groups & groupIter->second) != 0) {
              multicastGroup = groupIter->first;
            }
          }
          collectionSem.post();
          handleFamilyMessage(nlh, multicastGroup);
        } else {
          // Unrecognized or error, just issue a warning in debug mode
//          DebugWarningVa("(%s) : Unrecognized NLMSG type: %d",
//                         getName().c_str(), nlh->nlmsg_type);
        }
      }
    } // for(each part of message)
  } // while(true)

  return(Thread::k_Success);
}

// Protected helper methods

struct nlmsghdr* LabX::NetlinkClient::initCommandRequest(uint8_t command) {
  struct nlmsghdr *nlh;
  struct genlmsghdr ghdr;

  // Initialize the Generic Netlink header for the request
  nlh = initRequestMessage(GENL_ID_CTRL);

  // Clear out the generic netlink message header and set up for a command
  // to the Generic Netlink Controller
  memset(&ghdr, 0, sizeof(ghdr));
  ghdr.cmd = command;
  memcpy(NLMSG_DATA(&nlRequest.n), &ghdr, GENL_HDRLEN);

  return(nlh);
}

struct nlmsghdr* LabX::NetlinkClient::initRequestMessage(uint16_t messageType) {
  struct nlmsghdr *nlh;

  // Clear out the request structure
  memset(&nlRequest, 0, sizeof(nlRequest));

  // Set up a pointer to the netlink header in the request, then initialize it
  // for sending a generic netlink control message.  We require a positive 
  // acknowledgment for any request message to ensure we're in sync with the
  // responder.
  //
  // Tag the message with the process ID as the port ID, and bump the sequence
  // number for the instance.
  nlh = &nlRequest.n;
  nlh->nlmsg_len   = NLMSG_LENGTH(GENL_HDRLEN);
  nlh->nlmsg_flags = (NLM_F_REQUEST | NLM_F_ACK);
  nlh->nlmsg_type  = messageType;
  nlh->nlmsg_pid   = portId;
  nlh->nlmsg_seq   = sequenceId++;

  // Return a pointer to the Netlink header for further use in message construction
  return(nlh);
}

void LabX::NetlinkClient::sendUnicast(struct nlmsghdr *nlh, uint32_t destPort) {
  PendingRequest *request = NULL;
  struct iovec iov;
  struct sockaddr_nl destAddress;
  struct msghdr msg;
  int retValue;

  try {
    // Clear out and initialize a unicast destination Netlink address
    memset(&destAddress, 0, sizeof(destAddress));
    destAddress.nl_family = AF_NETLINK;
    destAddress.nl_pid    = destPort;
    destAddress.nl_groups = NL_MCAST_NONE;

    // Initialize an I/O vector for sending
    iov.iov_base = (void*) nlh;
    iov.iov_len  = nlh->nlmsg_len;

    // Create a message header containing the I/O vector
    memset(&msg, 0, sizeof(msg));
    msg.msg_name    = (void*) &destAddress;
    msg.msg_namelen = sizeof(destAddress);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    // Message is ready to send; place the instance onto the pending request queue.
    // Lock access to the vector prior to modifying it.
    PendingRequest *request = new PendingRequest(sequenceId - 1);
    collectionSem.wait();
    pendingRequests.push_back(request);
    collectionSem.post();

    // Actually send the message
    retValue = sendmsg(netlinkFd, &msg, 0);
    if(retValue < 0) {
      stringstream excStream;
      excStream << getName()
                << ": Error in sendmsg(): "
                << errno;
      throw(new Exception(excStream.str()));
    }

    // Wait on the request queue; if the response was already received, the
    // request has already been awakened and this will return immediately.
    request->await();

    // The response has been received, pop and destroy the monitor object.
    // Again, lock the collection first.
    collectionSem.wait();
    vector<PendingRequest*>::iterator requestIter = 
      find(pendingRequests.begin(), pendingRequests.end(), request);
    if(requestIter != pendingRequests.end()) {
      pendingRequests.erase(requestIter);
    }
    collectionSem.post();
    delete(request);
  } catch(Exception *exc) {
    // An exception was thrown; remove the request object if it was already
    // pushed onto the vector and destroy it
    if(request != NULL) {
      collectionSem.wait();
      vector<PendingRequest*>::iterator requestIter = 
        find(pendingRequests.begin(), pendingRequests.end(), request);
      if(requestIter != pendingRequests.end()) {
        pendingRequests.erase(requestIter);
      }
      collectionSem.post();
      delete(request);
    }

    stringstream excStream;
    excStream << getName() << ": Error sending unicast message";
    throw(new Exception(excStream.str()), exc);
  }
}

int32_t LabX::NetlinkClient::parse_rtattr(struct rtattr *tb[], 
                                          int max, 
                                          struct rtattr *rta, 
                                          int len) {
  // First clear the table, then sparsely populate it with the attributes
  // which are actually present in the message
  memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
  while (RTA_OK(rta, len)) {
    if (rta->rta_type <= max) tb[rta->rta_type] = rta;
    rta = RTA_NEXT(rta,len);
  }

  if(len) DebugErrorVa("(%s) : Deficit %d, rta_len=%d\n", 
                       getName().c_str(), len, rta->rta_len);

  return(0);
}

// Private helper methods

void LabX::NetlinkClient::sendGetFamilyMessage(void) {
  struct nlmsghdr *nlh;

  try {
    // Ensure mutually-exclusive access to the request buffer & socket
    lock();

    // Initialize a new command request message
    nlh = initCommandRequest(CTRL_CMD_GETFAMILY);

    // The message payload is the family name we're interested in
    addattr_l(nlh, 128, CTRL_ATTR_FAMILY_NAME, familyName.c_str(), (familyName.size() + 1));

    // Unicast send the message to the kernel
    DebugNoteVa("(%s) : Sending GET_FAMILY message, seq_id %d\n",
                getName().c_str(), nlh->nlmsg_seq);
    sendUnicast(nlh, NL_KERNEL_PORT);

    // Release access to the instance
    unlock();
  } catch(Exception *exc) {
    unlock();

    stringstream excStream;
    excStream << getName() << ": Error sending GET_FAMILY message";
    throw(new Exception(excStream.str(), exc));
  }
}

void LabX::NetlinkClient::handleControlMessage(struct nlmsghdr *nlh) {
  struct rtattr *attrTable[CTRL_ATTR_MAX + 1];
  struct rtattr *attrs;
  struct genlmsghdr ghdr;
  uint32_t msgLength;

  DebugNoteVa("(%s) : Received GENL_ID_CTRL message, seq_id %d", 
              getName().c_str(), nlh->nlmsg_seq);

  // Compute the length of the attribute portion and payload, then copy
  // the Generic Netlink header from the buffer into the structure for analysis
  msgLength = (nlh->nlmsg_len - NLMSG_LENGTH(GENL_HDRLEN));
  memcpy(&ghdr, NLMSG_DATA(nlh), GENL_HDRLEN);

  // Read the attribute table from the message into the attrTable array, which
  // merely contains pointers to message attributes which are present just beyond
  // the Generic Netlink header.
  attrs = (struct rtattr*) (((uint8_t*) NLMSG_DATA(nlh)) + GENL_HDRLEN);
  parse_rtattr(attrTable, CTRL_ATTR_MAX, attrs, msgLength);

  // Switch upon the control command, only some of which we do anything with
  switch(ghdr.cmd) {
  case CTRL_CMD_GETFAMILY:
    break;

  case CTRL_CMD_DELFAMILY:
    break;

  case CTRL_CMD_NEWFAMILY:
    // This is received in response to a GET_FAMILY command sent to the kernel;
    // obtain information about our family
    handleNewFamilyMessage(nlh, attrTable);
    break;

  case CTRL_CMD_NEWMCAST_GRP:
    break;

  case CTRL_CMD_DELMCAST_GRP:
    break;

  default:
    cout << "CTRL_CMD_?" << endl;
    ;
  }
}

void LabX::NetlinkClient::handleNewFamilyMessage(struct nlmsghdr *nlh,
                                                 struct rtattr **attrTable) {
  if(attrTable[CTRL_ATTR_FAMILY_ID] != 0) {
    familyId = *reinterpret_cast<uint16_t*>(RTA_DATA(attrTable[CTRL_ATTR_FAMILY_ID]));
    DebugNoteVa("(%s) : Got family ID %d", getName().c_str(), familyId);
  }

  if(attrTable[CTRL_ATTR_MCAST_GROUPS] != 0) {
    struct rtattr *attrTableMcast[GENL_MAX_FAM_GRPS + 1];
    uint32_t groupIndex;

    // Get the multicast group table nestsed within this attribute
    parse_rtattr_nested(attrTableMcast, GENL_MAX_FAM_GRPS, attrTable[CTRL_ATTR_MCAST_GROUPS]);

    for(groupIndex = 0; groupIndex < GENL_MAX_FAM_GRPS; groupIndex++) {
      if(attrTableMcast[groupIndex] != 0) {
        // Parse and add the multicast group
        parseMcastGroup(attrTableMcast[groupIndex]);
      }
    }
  }
}

void LabX::NetlinkClient::handleErrorMessage(struct nlmsghdr *nlh) {
  struct nlmsgerr *errMessage;

  // Point to the error message following the raw Netlink header
  errMessage = reinterpret_cast<struct nlmsgerr*>(NLMSG_DATA(nlh));
  if(nlh->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
    DebugErrorVa("(%s) : Truncated GENL_ID_ERROR message\n", getName().c_str());
    return;
  }

  // Good error message received
  DebugNoteVa("(%s) : Received GENL_ID_ERROR message: errno = %d, seq_id = %d", 
              getName().c_str(), errMessage->error, nlh->nlmsg_seq);

  // Error codes of zero are the ACKs requested in our command messages;
  // if one is received, look to wake up the command thread it's associated with.
  if(errMessage->error == 0) {
    // Lock access to the collection first
    collectionSem.wait();
    vector<PendingRequest*>::iterator requestIter;
    PendingRequest *foundRequest = NULL;
    for(requestIter = pendingRequests.begin();
        requestIter != pendingRequests.end();
        ++requestIter) {
      if((*requestIter)->getSequenceId() == nlh->nlmsg_seq) {
        foundRequest = *requestIter;
        break;
      }
    }
    collectionSem.post();

    if(foundRequest != NULL) {
      // Awaken the sleeping thread; we'll allow *it* to destroy the monitor
      // object since it will still need it temporarily.
      foundRequest->awaken();
    }
  } else {
    // TODO: Invoke a callback for a bona-fide error
  }
}

void LabX::NetlinkClient::parseMcastGroup(struct rtattr *mcastGroup) {
  struct rtattr *mcastAttrs[CTRL_ATTR_MCAST_GRP_MAX + 1];
  char *groupName;
  uint32_t groupId;

  // Nested within the CTRL_ATTR_MCAST_GROUPS attribute are the group's
  // name and numerical bitmask ID
  parse_rtattr_nested(mcastAttrs, CTRL_ATTR_MCAST_GRP_MAX, mcastGroup);

  // Ensure that both name and ID are present
  if((mcastAttrs[CTRL_ATTR_MCAST_GRP_NAME] != 0) &
     (mcastAttrs[CTRL_ATTR_MCAST_GRP_ID] != 0)) {
    // Get the name and ID of the group
    groupName = reinterpret_cast<char*>(RTA_DATA(mcastAttrs[CTRL_ATTR_MCAST_GRP_NAME]));
    groupId   = *reinterpret_cast<uint32_t*>(RTA_DATA(mcastAttrs[CTRL_ATTR_MCAST_GRP_ID]));
    multicastGroupMap.insert(make_pair(string(groupName), groupId));
    DebugNoteVa("(%s) : Discovered multicast group \"%s\" at id 0x%08X", 
                getName().c_str(), groupName, groupId);
  }
}

int32_t LabX::NetlinkClient::addattr_l(struct nlmsghdr *nlh, 
                                       uint32_t maxlen, 
                                       uint32_t type, 
                                       const void *data, 
                                       uint32_t alen) {
  uint32_t len = RTA_LENGTH(alen);
  struct rtattr *rta;

  if((NLMSG_ALIGN(nlh->nlmsg_len) + len) > maxlen) return(-1);
  rta = (struct rtattr*) (((uint8_t*) nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
  rta->rta_type = type;
  rta->rta_len = len;
  memcpy(RTA_DATA(rta), data, alen);
  nlh->nlmsg_len = (NLMSG_ALIGN(nlh->nlmsg_len) + RTA_ALIGN(len));
  return(0);
}

// Private inner classes

LabX::NetlinkClient::PendingRequest::PendingRequest(uint32_t sequenceId) :
  sequenceId(sequenceId),
  waitSem(0, "PendingRequestSem") {
}
      
void LabX::NetlinkClient::PendingRequest::await(void) {
  waitSem.wait(NETLINK_TIMEOUT_MSEC);
}

uint32_t LabX::NetlinkClient::PendingRequest::getSequenceId(void) const {
  return(sequenceId);
}
 
void LabX::NetlinkClient::PendingRequest::awaken(void) {
  waitSem.post();
}
