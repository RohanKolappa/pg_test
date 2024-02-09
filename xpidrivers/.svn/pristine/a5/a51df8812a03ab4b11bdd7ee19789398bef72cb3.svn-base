#ifndef _TIMER_H_
#define _TIMER_H_

// System headers
#include <vector>

// Project headers
#include "MemberMethod.h"
#include "MemberMethod1Param.h"
#include "Method.h"
#include "SignalSniffer.h"
#include "LabX.h"

// Timer resolution, in milliseconds.  Must be an integer multiple of 10.
#define TIMER_RESOLUTION 100

// Macro to convert a time value expressed in seconds to timer ticks.
// We rely on the compiler to perform constant folding, including the integer cast!
#define SECOND_TICKS(seconds) ((uint32_t) ((seconds) * 1000 / TIMER_RESOLUTION))

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an interface for registering timer callbacks, providing
   * both periodic and "one-shot" semantics.  This class is a Singleton, and must
   * be started (descends from CThread).
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Timer : public SignalSniffer {
    
    // Destructor
  public:
    
    /**
     * Destructor
     */
    virtual ~Timer(void);
    
    // Protected overrides from class SignalSniffer
  protected:
    
    /**
     * Sets up a periodic interval timer, now that the instance is primed to
     * receive SIGALRM.
     */
    virtual void preSniffConfig(void);
    
    /**
     * Services registered timer callbacks on timer expiry
     */
    virtual void handleSignal(void);
    
    // Public singleton interface
  public:
    
    /**
     * Returns the singleton instance of Timer, creating it if necessary
     * @return - The singleton instance
     */
    static Timer& instance(void);
    
    /**
     * Destroys the singleton instance, if it exists.
     */
    static void destroy(void);
    
    // Public timer interface
  public:
    
    /**
     * Adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * @param timerFunc - Pointer to the static function pointer
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    void addTimer(void (*timerFunc)(void), 
                  const uint32_t& timeout,
                  const bool& periodic);
    
    /**
     * Adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is a pointer-to member function.
     * @param timerMethod - Pointer to the member method pointer
     * @param invocationInstance - Instance upon which to invoke the method
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    template<class MC>
      void addTimer(void (MC::*timerMethod)(void), 
                    MC* invocationInstance,
                    const uint32_t& timeout,
                    const bool& periodic) {
      // Add a new timer callback with a new member method instance
      addTimer(new MemberMethod<MC>(timerMethod, invocationInstance),
               timeout,
               periodic);
    }
    
    /**
     * Adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is a pointer-to member function that will be passed a free-form
     * parameter.
     * @param timerMethod - Pointer to the member method pointer
     * @param parameter - Parameter to be passed upon invocation
     * @param invocationInstance - Instance upon which to invoke the method
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    template<class MC, class PC>
      void addTimer(void (MC::*timerMethod)(PC), 
                    PC parameter,
                    MC* invocationInstance,
                    const uint32_t& timeout,
                    const bool& periodic) {
      // Add a new timer callback with a new member method instance
      addTimer(new MemberMethod1Param<MC, PC>(timerMethod, parameter, invocationInstance),
               timeout,
               periodic);
    }
    
    /**
     * Adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is an instance of Method* whose lifetime becomes tied to the
     * lifetime of the Timer instance.
     * @param timerMethod - Pointer to the timer method instance
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    void addTimer(Method* timerMethod, 
                  const uint32_t& timeout,
                  const bool& periodic);
    
    /**
     * Updates or adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is a static member or global function pointer.
     * @param timerFunc - Pointer to the static function pointer
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    void updateTimer(void (*timerFunc)(void), 
                     const uint32_t& timeout,
                     const bool& periodic);
    
    /**
     * Updates or adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is a pointer-to member function.
     * @param timerMethod - Pointer to the member method pointer
     * @param invocationInstance - Instance upon which to invoke the method
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    template<class MC>
      void updateTimer(void (MC::*timerMethod)(void), 
                       MC* invocationInstance,
                       const uint32_t& timeout,
                       const bool& periodic) {
      // Update the callback with a new member method instance
      updateTimer(new MemberMethod<MC>(timerMethod, invocationInstance),
                  timeout, periodic);
    }
    
    /**
     * Updates or adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is a pointer-to member function that will be passed a free-form
     * parameter.
     * @param timerMethod - Pointer to the member method pointer
     * @param parameter - Parameter to be passed upon invocation
     * @param invocationInstance - Instance upon which to invoke the method
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    template<class MC, class PC>
      void updateTimer(void (MC::*timerMethod)(PC), 
                       PC parameter,
                       MC* invocationInstance,
                       const uint32_t& timeout,
                       const bool& periodic) {
      // Add a new timer callback with a new member method instance
      updateTimer(new MemberMethod1Param<MC, PC>(timerMethod, parameter, invocationInstance),
                  timeout, periodic);
    }

    /**
     * Updates or adds a timer callback set to expire in time increments of 100 ms.
     * If periodic is true, the callback will be called periodically.
     * Callback is an instance of Method* whose lifetime becomes tied to the
     * lifetime of the Timer instance.
     * @param timerMethod - Pointer to the timer method instance
     * @param timeout - Timeout interval
     * @param periodic - True if the callback is periodic, false for one-shot
     */
    void updateTimer(Method* timerMethod, 
                     const uint32_t& timeout,
                     const bool& periodic);

    /**
     * Removes any timers registered with the passed callback.
     * @param timerFunc - Pointer to the static function pointer
     */
    void removeTimers(void (*timerFunc)(void));

    /**
     * Removes any timers registered with the passed callback and invocation 
     * instance.
     * @param timerMethod - Pointer to the member method pointer
     * @param invocationInstance - Instance upon which to invoke the method
     */
    template<class MC>
      void removeTimers(void (MC::*timerMethod)(void), 
                        MC* invocationInstance) {
      // Remove all timer callbacks registered with an equivalent member
      // method instance.
      removeTimers(MemberMethod<MC>(timerMethod, invocationInstance));
    }

    /**
     * Removes any timers registered with the passed callback, parameter and
     * invocation instance.  Callback is a pointer-to member function that takes a 
     * free-form parameter.
     * @param timerMethod - Pointer to the timer method pointer
     * @param parameter - Parameter to be passed upon invocation
     * @param invocationInstance - Instance upon which to invoke the method
     */
    template<class MC, class PC>
      void removeTimers(void (MC::*timerMethod)(PC),
                        PC parameter,
                        MC* invocationInstance) {
      // Remove all timer callbacks registered with an equivalent member method
      // instance
      removeTimers(MemberMethod1Param<MC, PC>(timerMethod, parameter, invocationInstance));
    }

    /**
     * Removes any timers registered with the passed callback.
     * @param timerMethod - Pointer to the timer method instance
     */
    void removeTimers(const Method& timerMethod);

    /**
     * Clears all timers
     */
    void clearAllTimers(void);

    // Private type definitions
  private:

    /**
     * A structure type to store timer records
     */
    struct TimerRecord
    {
      /**
       * Constructor
       * @param timerMethod - Pointer to the timer method instance
       * @param timeout - Timeout interval
       * @param reload - True if timer is to be reloaded
       */
      TimerRecord(Method* timerMethod,
                  const uint32_t& timeout,
                  const uint32_t& reload);

      /**
       * Timeout method to be invoked upon expiry
       */
      Method* timerMethod;

      /**
       * Timeout counter
       */
      uint32_t timeout;

      /**
       * Reload value for periodic timers
       */
      uint32_t reload;
    };

    // Private constructor
  private:

    /**
     * Creates a new timer object
     */
    Timer(void);

    // Private attributes
  private:

    /**
     * A vector of timer records
     */
    std::vector<TimerRecord> timers;

    /**
     * A semaphore to serialize access to the timers
     */
    Semaphore timerSem;

    // Private class attributes
  private:

    /**
     * A pointer to the singleton instance
     */
    static Timer* timerInstance;
  };
}

#endif
