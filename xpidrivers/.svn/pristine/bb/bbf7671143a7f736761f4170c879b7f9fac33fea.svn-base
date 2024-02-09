#ifndef _PTP_EVENT_H_
#define _PTP_EVENT_H_

// System headers
#include <string>

// Library headers
#include "Event.h"
#include "PropertyTable.h"

// Locate within the "Avb" namespace
namespace Avb {
  
  /**
   * Event class for the Lab X PTP hardware.  This provides an abstraction through
   * which the PTP management object can inform interested observers of events.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class PtpEvent : public LabX::Event {

    // Public type definitions
  public:

    /**
     * Enumeration for event types
     */
    typedef enum {
      PTP_EVENT_HEARTBEAT = 0,
      PTP_EVENT_GM_CHANGE,
      PTP_EVENT_RTC_LOCK,
      PTP_EVENT_RTC_UNLOCK,
    } EventType;


    // Constructor / Destructor
  public:

    /**
     * Creates an event instance with the passed type
     * @param eventType   - Type of the event being reported
     * @param pairStrings - Vector of key / value pair strings
     */
    explicit PtpEvent(EventType eventType, const std::vector<std::string> &pairStrings);

    /**
     * Copy constructor
     * @param other - Instance to be copied
     */
    PtpEvent(const PtpEvent &other);

    /**
     * Virtual destructor
     */
    virtual ~PtpEvent(void);

    // Public interface methods
  public:

    /**
     * Returns the name of the event class.  Subclasses should return a public,
     * static string instance, since this is returned by reference!
     * @return - The name of the event class
     */
    virtual const std::string& getClassName(void) const;
    
    /**
     * Returns a string representation of the event
     * @return - A textual rendering of the event
     */
    virtual const std::string toString(void) const;

    /**
     * Returns the PTP event type encapsulated by the instance
     * @return - The event type
     */
    EventType getEventType(void) const;
    
    /**
     * Allocates and returns a clone of the instance.
     * @return - A pointer to a new, dynamically-allocated clone
     */
    virtual Event* clone(void) const;

    // Public class attributes
  public:

    /**
     * Class name string
     */
    static const std::string className;
    
    // Private attributes
  private:

    /**
     * Event type
     */
    EventType eventType;

    /**
     * Property table
     */
    LabX::PropertyTable properties;
  };
}

#endif
