#ifndef _EVENT_HOOK_H_
#define _EVENT_HOOK_H_

// Library headers
#include "Event.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class encapsulates an event processing hook method, which accepts a
   * Event& as its only parameter and returns a boolean value indicating whether
   * the encapsulated method consumed the event.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class EventHook {
    // Constructor / Destructor
  public:
    
    /**
     * Default constructor
     */
    EventHook(void) {}
    
    /**
     * Virtual destructor
     */
    virtual ~EventHook(void) {}
    
    // Abstract public interface
  public:
    
    /**
     * Invokes the method with the passed event.
     * @param event - Reference to the Event instance being dispatched
     * @return - True if the event was handled, false if ignored by the handler
     */
    virtual const bool operator()(const Event& event) const = 0;
  };
}

#endif
