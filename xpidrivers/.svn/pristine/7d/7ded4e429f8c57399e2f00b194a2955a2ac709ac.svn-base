#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

// System headers
#include <list>
#include <map>
#include <string>

// Project headers
#include "Debug.h"
#include "Event.h"
#include "MemberEventHook.h"

// Locate within the "LabX" namespace
namespace LabX {

  // Forward references
  class EventHook;

  /**
   * This class defines an abstract interface for handling events.  Event handlers
   * accept instances of incoming events, and look to see if a subclass has 
   * registered a type-specific hook method via the class' protected interface.  
   * If none is found, a generic virtual handling method is invoked.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class EventHandler {
    // Constructor / Destructor
  public:
    
    /**
     * Default constructor
     */
    EventHandler(void);
    
    /**
     * Virtual destructor
     */
    virtual ~EventHandler(void);
    
    // Public interface methods
  public:
    
    /**
     * Dispatch the passed event instance to the appropriate type-specific or 
     * generic hander.
     * @param event - Reference to the Event instance being dispatched
     * @return - True if the event was handled, false if ignored by the handler
     */
    virtual const bool dispatch(const Event& event);
    
    // Protected interface methods
  protected:
    
    /**
     * Handle the passed event generically.  This is only called if no type-specific
     * event hook was registered for the event.  Default implementation does nothing
     * and returns false (event was not handled).
     * @param event - Reference to the Event instance being dispatched
     * @return - True if the event was handled, false if ignored
     */
    virtual const bool handle(const Event& event);
    
    /**
     * Registers the passed hook function to be executed whenever the state receives
     * an event of the specified type.  Templated upon the member class and event 
     * class.  This is not performed in a thread-safe manner; event hooks should be
     * registered from within the sub-class constructor!  Multiple event hooks may 
     * be registered for the same class; they are executed in FILO order, so that 
     * if base and derived classes in the same object each register a hook on the
     * same class, the derived hook is called first.  If any hook returns true, the
     * event is absorbed and will not propagate to hooks registered prior to the 
     * hook, mimicking OO single dispatch semantics (most-derived method decides 
     * whether to invoke base class functionality)
     * @param hookMethod - Member method within the derived class.
     */
    template<class MC, class EC>
      void registerEventHook(const bool (MC::*hookMethod)(const EC&)) {
      // Ensure that the hook method exists within this instance
      MC* invocationInstance = dynamic_cast<MC*>(this);
      if(invocationInstance == NULL) {
        DebugErrorVa("Hook method for \"%s\" events is not a member of the event handler instance!", EC::className.c_str());
      }
      
      // Map the specified event type to the hook method
      EventHook* eventHook = new MemberEventHook<MC, EC>(hookMethod, invocationInstance);
      std::map<std::string, std::list<EventHook*> >::iterator mapIter = eventHooks.find(EC::className);
      if(mapIter == eventHooks.end()) {
        eventHooks.insert(make_pair(EC::className, std::list<EventHook*>(1, eventHook)));
      } else {
        // Push the method onto the front of the existing list
        mapIter->second.push_front(eventHook);
      }
    }

    // Private attributes
  private:
    
    /**
     * Associative map of event type strings to a list of event hooks
     */
    std::map<std::string, std::list<EventHook*> > eventHooks;
  };
}

#endif


