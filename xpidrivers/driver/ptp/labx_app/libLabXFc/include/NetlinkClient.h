#ifndef _NETLINK_CLIENT_H_
#define _NETLINK_CLIENT_H_

// System headers
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/genetlink.h>

// STL headers
#include <map>
#include <string>
#include <vector>

// Library headers
#include "Lockable.h"
#include "Semaphore.h"
#include "Thread.h"

// Maximum size of a NetLink packet payload, in bytes
#define MAX_NL_PAYLOAD (4096)

// Port ID for the kernel
#define NL_KERNEL_PORT  (0)

// Constant indicating no multicast groups
#define NL_MCAST_NONE  (0)

// Macro for parsing nested attributes
#define parse_rtattr_nested(tb, max, rta) \
  (parse_rtattr((tb), (max), (struct rtattr*) RTA_DATA(rta), RTA_PAYLOAD(rta)))

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * An interface object for communicating over the Generic Netlink 
   * bus using a given protocol family.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class NetlinkClient : public Thread, public Lockable {
    // Constructor / Destructor
  public:

    /**
     * Creates an instance for listening to messages on the specified
     * protocol family
     * @param familyName - Name of the Generic Netlink family
     */
    explicit NetlinkClient(const std::string &familyName);

    /**
     * Destructor
     */
    virtual ~NetlinkClient(void);

    // Public overrides from class Thread
  public:

    /**
     * Sets up the Netlink socket and the worker thread
     */
    virtual void start(void);

    /**
     * Worker thread method to listen for received Netlink packets
     * @param The return status for the thread
     */
    virtual int32_t run(void);

    // Protected helper methods
  protected:

    /**
     * Initializes the shared request structure for a request to the Generic
     * Netlink Controller.  Client code must ensure mutually-exclusive access 
     * while it is using the structure by lock()-ing the instance.
     * @param command - Command for the Generic Netlink Controller
     * @return        - A pointer to the Netlink message header for client use
     */
    struct nlmsghdr* initCommandRequest(uint8_t command);

    /**
     * Initializes the shared request structure for a new message.  Client
     * code must ensure mutually-exclusive access while it is using the 
     * structure by lock()-ing the instance.
     * @param messageType - Generic Netlink message type enumeration
     * @return            - A pointer to the Netlink message header for client use
     */
    struct nlmsghdr* initRequestMessage(uint16_t messageType);

    /**
     * Unicasts the passed Generic Netlink message to the socket.  This method
     * blocks until a response is received.  Client code must ensure mutually-
     * exclusive access while it is using the structure by lock()-ing the instance.
     * @param nlh      - Pointer to the Netlink header of the message
     * @param destPort - Netlink destination port ID to send to
     */
    void sendUnicast(struct nlmsghdr *nlh, uint32_t destPort);

    /**
     * Handler for messages directed to the family
     * @param nlh            - Pointer to the Netlink header of the message
     * @param multicastGroup - Name of the multicast group, if any
     */
    virtual void handleFamilyMessage(struct nlmsghdr *nlh, 
                                     const std::string& multicastGroup) = 0;


    /**
     * Utility method for parsing an attribute table contained within a
     * Netlink packet
     * @param tb - Table to populate with pointers
     * @param max - Maximum number of distinct attributes
     * @param rta - Pointer to the attribute table within the packet
     * @param len - Length of the packet
     * @return Zero if successful, negative otherwise
     */
    int32_t parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len);

    // Private inner classes
  private:

    /**
     * Inner class used to track pending unicast requests that are waiting for an
     * acknowledgment from the kernel or another client.
     */
    class PendingRequest {
    public:

      /**
       * Creates an instance with the passed sequence ID
       */
      explicit PendingRequest(uint32_t sequenceId);
      
      /**
       * Waits until the request's acknowledgment comes in, potentially
       * with some response data.
       */
      void await(void);

      /**
       * Returns the sequence ID
       */
      uint32_t getSequenceId(void) const;

      /**
       * Awakens the thread sleeping on the wait semaphore
       */
      void awaken(void);

    private:

      /**
       * Sequence ID from the pending request
       */
      uint32_t sequenceId;

      /**
       * Semaphore to wake up the instance
       */
      Semaphore waitSem;
    };

    // Private helper methods
    //
    // Some of these have been adapted from the libnetlink code in iproute2, and
    // retain their original names for traceability.
  private:

    /**
     * Sends a message requesting information about the Generic Netlink family
     */
    void sendGetFamilyMessage(void);

    /**
     * Handler for a Generic Netlink control message
     * @param nlh - Pointer to the Netlink message header
     */
    void handleControlMessage(struct nlmsghdr *nlh);

    /**
     * Handler for a "NEW_FAMILY" message
     * @param nlh       - Pointer to the Netlink message header
     * @param attrTable - Netlink message attributes table
     */
    void handleNewFamilyMessage(struct nlmsghdr *nlh, struct rtattr **attrTable);

    /**
     * Handler for a Generic Netlink error message
     * @param nlh - Pointer to the Netlink message header
     */
    void handleErrorMessage(struct nlmsghdr *nlh);

    /**
     * Parses a multicast group from a table of attributes and adds it to the
     * list maintained by the instance.
     * @param mcastGroup - Pointer to the attributes table to be parsed
     */
    void parseMcastGroup(struct rtattr *mcastGroup);

    /**
     * Utility method for adding an attribute to a Netlink packet
     * @param n      - Netlink message header
     * @param maxlen - Maximum length to copy from the data, after alignment
     * @param type   - Attribute type
     * @param data   - Attribute data to be copied
     * @param alen   - Attribute data length
     * @return Zero if successful, negative otherwise
     */
    static int32_t addattr_l(struct nlmsghdr *n, 
                             uint32_t maxlen, 
                             uint32_t type, 
                             const void *data, 
                             uint32_t alen);

    // Private attributes
  private:
    
    /**
     * Name of the protocol family
     */
    const std::string familyName;

    /**
     * Netlink socket descriptor
     */
    int netlinkFd;

    /**
     * Source address bound to the socket
     */
    struct sockaddr_nl sourceAddress;

    /**
     * Netlink request packet structure
     */
    struct {
      struct nlmsghdr n;
      uint8_t buf[MAX_NL_PAYLOAD];
    } nlRequest;

    /**
     * Netlink receive buffer
     */
    uint8_t rxBuffer[MAX_NL_PAYLOAD];

    /**
     * Netlink sequence ID
     */
    uint32_t sequenceId;

    /**
     * Netlink family ID
     */
    uint16_t familyId;

    /**
     * Netlink port ID, which is unique per instance
     */
    uint32_t portId;

    /**
     * Map of multicast group names to their IDs
     */
    std::map<std::string, uint32_t> multicastGroupMap;

    /**
     * Vector of pending requests
     */
    std::vector<PendingRequest*> pendingRequests;

    /**
     * Mutex to lock the collections
     */
    Semaphore collectionSem;

    /**
     * Static semaphore to serialize access to the class attributes which follow
     */
    static Semaphore classResourcesSem;

    /**
     * Number of instances, used to generate unique port IDs
     */
    static uint32_t instanceCount;
  };
}

#endif
