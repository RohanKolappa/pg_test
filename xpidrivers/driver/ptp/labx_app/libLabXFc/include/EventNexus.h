#ifndef _EVENT_NEXUS_H_
#define _EVENT_NEXUS_H_

// System headers
#include <vector>

// Project headers
#include "Event.h"
#include "EventHandler.h"
#include "Lockable.h"
#include "LabX.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class is a Singleton that clones and dispatches incoming events to
   * registered event handler instances.  All events in the entire system pass
   * through the nexus.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class EventNexus : public Lockable {

    // Destructor
  public:
  
    /**
     * Virtual destructor
     */
    virtual ~EventNexus(void);
    
    // Public singleton interface
  public:
    
    /**
     * Returns the singleton instance of EventNexus, creating it if necessary
     * @return - A reference to the Singleton instance
     */
    static EventNexus& instance(void);
    
    /**
     * Destroys the singleton instance, if it exists.
     */
    static void destroy(void);
    
    // Public interface methods
  public:
    
    /**
     * Registers the passed event handler for all events that pass through the
     * nexus.
     * @param handler - Pointer to the handler to be registered
     */
    void registerHandler(EventHandler* handler);
    
    /**
     * Dispatches the event instance to all registered event handlers, cloning
     * as many times as necessary.  Null events are quietly ignored.  Once the
     * dispatch is completed, the event instance is destroyed.
     * @param event - Pointer to the event to be handled
     */
    void dispatch(Event* event);
    
    /**
     * Enables or disables debugging of events
     * @param debugEvents - True if debugging messages are to be displayed,
                            false otherwise
    */
    void setDebugEvents(const bool& debugEvents);
    
    // Private constructor
  private:
    
    /**
     * Creates a new exit monitor object
     */
    EventNexus(void);
    
    // Private attributes
  private:
    
    /**
     * Boolean controlling whether events are announced with debug messages
     */
    bool debugEvents;
    
    /**
     * A vector of registered event handlers
     */
    std::vector<EventHandler*> handlers;
    
    /**
     * Event counter
     */
    uint32_t eventCounter;
    
    /**
     * A pointer to the singleton instance
     */
    static EventNexus* eventNexusInstance;
  };
}

#endif

