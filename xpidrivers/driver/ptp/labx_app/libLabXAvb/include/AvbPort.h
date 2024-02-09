#ifndef _AVB_PORT_H_
#define _AVB_PORT_H_

// System headers
#include <stdint.h>

// STL headers
#include <string>
#include <vector>

// Lab X Foundation Classes headers
#include "Lockable.h"

// AVB library headers
#include "LinkMonitor.h"

// Locate within namespace "Avb"
namespace Avb {

  /**
   * Class abstracting an Ethernet port within an AVB system.
   *
   * Copyright (c) 2010 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class AvbPort : public LinkMonitor::LinkObserver,
                  LabX::Lockable {
     // Public type declarations
  public:

    /**
     * Enumeration for different port states
     */
    typedef enum {
      PORT_LINK_DOWN,
      PORT_LINK_100_MBIT,
      PORT_LINK_1_GBIT,
      PORT_LINK_10_GBIT
    } PortState;

    /**
     * Inner interface class for port status change Observer pattern
     */
    class AvbPortObserver {
      // Public interface
    public:

      /**
       * Default constructor
       */
      AvbPortObserver(void) {};

      /**
       * Virtual destructor
       */
      virtual ~AvbPortObserver(void) {};

      /**
       * Pure virtual interface method for port state change notification
       * @param avbPort   - AVB port instance the change pertains to
       * @param portState - New state for the AVB port
       */
      virtual void stateChanged(AvbPort &avbPort, const PortState portState) = 0;
    };

    // Constructor / Destructor
  public:
    
    /**
     * Constructor
     * @param name - Name of the port's network interface, e.g. "eth0"
     */
    explicit AvbPort(const std::string &name);
    
    /**
     * Virtual destructor
     */
    virtual ~AvbPort(void);
    
    // Public interface methods
  public:
    
    /**
     * Port name accessor
     * @return - Name of the port's network interface
     */
    const std::string& getName(void) const;
    
    /**
     * MAC address accessor
     * @return - The MAC address associated with the port interface, as reported
     *           by the network driver.
     */
    const uint8_t* getMacAddress(void) const;

    /**
     * Port link state accessor
     * @return - The present link state for the port
     */
    const PortState getPortState(void) const;
    
    /**
     * Subscribes the passed observer to port status change event notifications
     * @param observer - Observer to be subscribed
     */
    void subscribe(AvbPortObserver &observer) const;

    /**
     * Unsubscribes the passed observer from port status change event notifications
     * @param observer - Observer to be unsubscribed
     */
    void unsubscribe(AvbPortObserver &observer) const;

    /**
     * Mutator for the redundancy link partner attribute; this may only be
     * set once after construction.
     * @param redundancyPartner - Pointer to another instance which, in conjunction
     *                            with this instance, represents a redundant port pair.
     */
    void setRedundancyPartner(AvbPort *redundancyPartner);

    /**
     * Accessor for the redundancy link partner attribute
     * @return A pointer to the instance's redundancy partner, or NULL if it is not
     *         part of a redundant port pair.
     */
    AvbPort* getRedundancyPartner(void) const;

    /**
     * Converts the passed string to a MAC address
     * @param macString - The string to be parsed and converted
     * @return - The converted MAC address byte array, which must be delete[]-ed
     */
    static uint8_t* convertMacAddress(const std::string &macAddressString);

    /**
     * Compares two MAC addresses for equality
     * @param macAddressA - The first MAC address to be compared
     * @param macAddressB - The second MAC address to be compared
     * @return True if the MAC addresses are equal, false otherwise
     */
    static bool macAddressesEqual(const uint8_t *macAddressA,
                                  const uint8_t *macAddressB);

    /**
     * Converts the passed string into a colon (:) delimited array of bytes
     * @param byteString - The string to be parsed and converted
     * @param numBytes - The number of bytes to be converted
     * @return - The converted byte array, which must be delete[]-ed
     */
    static uint8_t* convertByteString(const std::string &byteString, uint32_t numBytes);

    // Public overrides from class LinkMonitor::LinkObserver
  public:

    /**
     * Pure virtual interface method for link change notification
     */
    virtual void linkChanged(const std::string &portName,
                             const bool linkUp,
                             const bool linkRunning);

    // Private helper methods
  private:
    
    /**
     * Gets a raw socket for making requests
     * @return - File descriptor for the raw socket
     */
    int getRawSocket(void) const;

    // Private attributes
  private:
    
    /**
     * Name of the instance
     */
    const std::string name;
    
    /**
     * Handle to a raw socket
     */
    mutable int socketHandle;
    
    /**
     * Cached MAC address
     */
    mutable uint8_t *macAddress;

    /**
     * Vector of observers subscribed to this port
     */
    mutable std::vector<AvbPortObserver*> observers;

    /**
     * Pointer to another instance which is this port's partner in a
     * redundant link setup
     */
    AvbPort *redundancyPartner;

    /**
     * Monotonically-incrementing instance count for the class
     */
    static uint16_t instancesCreated;

  };
}

#endif
