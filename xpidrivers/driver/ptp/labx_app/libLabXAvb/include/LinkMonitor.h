#ifndef _LINK_MONITOR_H_
#define _LINK_MONITOR_H_

// STL headers
#include <map>
#include <string>
#include <vector>

// Library headers
#include "Lockable.h"
#include "Thread.h"

// Locate within the "Avb" namespace
namespace Avb {

  /**
   * An active Singleton which monitors link status changes on network
   * interfaces, broadcasting events to interested clients.
   *
   * Copyright (c) 2010 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class LinkMonitor : public LabX::Thread, LabX::Lockable {

    // Public type declarations
  public:

    /**
     * Inner interface class for link change Observer pattern
     */
    class LinkObserver {
      // Public interface
    public:

      /**
       * Default constructor
       */
      LinkObserver(void) {};

      /**
       * Virtual destructor
       */
      virtual ~LinkObserver(void) {};

      /**
       * Pure virtual interface method for link change notification
       */
      virtual void linkChanged(const std::string &portName,
                               const bool linkUp,
                               const bool linkRunning) = 0;
    };

    // Destructor
  public:

    /**
     * Virtual destructor
     */
    virtual ~LinkMonitor(void);

    // Public overrides from class Thread
  public:

    /**
     * Starts the thread instance, first opening and configuring the
     * instance's underlying netlink socket
     */
    virtual void start(void);

    /**
     * Removes any registered Observers and then kills the worker thread
     */
    virtual void stop(void);

    /**
     * Active run method for the object
     * @param The return status for the thread
     */
    virtual int32_t run(void);

    // Public interface methods
  public:

    /**
     * Subscribes the passed observer to link change event notifications
     * @param portName - Name of the port to observe
     * @param observer - Observer to be subscribed
     */
    void subscribe(const std::string &portName, LinkObserver &observer);

    /**
     * Unsubscribes the passed observer from link change event notifications
     * @param portName - Name of the port being observed
     * @param observer - Observer to be unsubscribed
     */
    void unsubscribe(const std::string &portName, LinkObserver &observer);

    // Static Singleton interface
  public:

    /**
     * Returns the Singleton instance, creating it if necessary
     * @return A reference to the Singleton
     */
    static LinkMonitor& instance(void);

    /**
     * Stops the Singleton's worker thread, waiting for it to end, and then
     * destroys the encapsulated Singleton instance
     */
    static void shutdown(void);

    // Private helper methods
  private:

    /**
     * Processes link messages
     * @param netlinkHeader - Netlink message header
     */
    void handleLinkUpDown(struct nlmsghdr *netlinkHeader);

    // Private constructor
  private:
    /**
     * Default constructor
     */
    LinkMonitor(void);

    // Private attributes
  private:

    /**
     * Netlink socket descriptor
     */
    int netlinkFd;

    /**
     * Associative map of interfaces to observer lists
     */
    std::map<std::string, std::vector<LinkObserver*> > observerMap;

    // Private class attributes
  private:

    /**
     * Singleton instance pointer
     */
    static LinkMonitor *linkMonitorInstance;
  };
}

#endif
