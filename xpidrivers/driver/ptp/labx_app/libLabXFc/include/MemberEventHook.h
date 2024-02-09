#ifndef _MEMBER_EVENT_HOOK_H_
#define _MEMBER_EVENT_HOOK_H_

// Project headers
#include "Debug.h"
#include "EventHook.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Class MemberEventHook encapsulates an event processing hook method.  The method
   * is stored as a pointer-to-member function, which accepts a templatized event
   * reference as its only parameter and returns a boolean value indicating whether
   * the encapsulated method consumed the event.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  template<class MC, class EC> class MemberEventHook : public EventHook {

    // Constructor / Destructor
  public:

    /**
     * Constructor accepts a pointer-to-member function and a pointer to the
     * instance upon which the function will be invoked.
     * @param eventHookMethod - Member method to be invoked
     * @param invocationInstance - Instance of the class containing the hook method
     */
  MemberEventHook(const bool (MC::*eventHookMethod)(const EC&), 
                  MC* invocationInstance) :
    EventHook(),
      eventHookMethod(eventHookMethod),
      invocationInstance(invocationInstance) {
      }
    
    /**
     * Virtual destructor
     */
    virtual ~MemberEventHook(void) {
    }
    
    // Public overrides from class CMethod
  public:
    
    /**
     * Invokes the stored function pointer, passing it the event safely cast to the
     * class it expects.
     * @param event - Reference to the Event instance causing the invocation
     * @return - True if the event was handled, false if ignored by the hook method
     */
    virtual const bool operator()(const Event& event) const {
      // Attempt to cast to the event class the method expects
      const EC* castEvent = dynamic_cast<const EC*>(&event);
      if(castEvent == NULL) {
          DebugErrorVa("Attempt to improperly cast a %s", event.getClassName().c_str());
      }
      
      // Invoke the stored pointer to member
      return((invocationInstance->*eventHookMethod)(*castEvent));
    }
    
    // Private attributes
  private:
    
    /**
     * Pointer to the member method
     */
    const bool (MC::*eventHookMethod)(const EC&);
    
    /**
     * Pointer to the invocation instance
     */
    MC* invocationInstance;
  };
}

#endif

