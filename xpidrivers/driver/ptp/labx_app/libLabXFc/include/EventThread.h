#ifndef _C_EVENT_THREAD_H_
#define _C_EVENT_THREAD_H_

// System headers
#include <list>
#include <map>
#include <string>
#include <vector>

// Project headers
#include "Event.h"
#include "EventHandler.h"
#include "Lockable.h"
#include "Semaphore.h"
#include "Thread.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an active event handling mechanism, cloning and enqueueing
   * incoming events and handling them polymorphically in FIFO order in an 
   * independent thread of control.  Use of this class is recommended for 
   * computation-intensive event processing to prevent heavyweight event processors
   * from throttling overall system performance.  The class also provides semantics
   * for filtering out events the subclass doesn't care about to prevent unnecessary
   * context switching.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class EventThread : public EventHandler, public Thread, public Lockable
  {
    // Constructor / Destructor
  public:
    
    /**
     * Constructor
     * @param name - Name to be used for debugging, etc.
     */
    explicit EventThread(const string& name);
    
    /**
     * Virtual destructor
     */
    virtual ~EventThread(void);
    
    // Public interface methods
  public:
    
    /**
     * Filter and place the event instance on the event queue in a thread-safe 
     * manner and wake up the message pump if necessary.  Returns true if the event
     * was enqueued; there is no indication at this point whether the instance's 
     * worker thread will ultimately handle the event.
     * @param event - Reference to the Event instance being dispatched
     * @return - True if the event was enqueued, false if ignored
     */
     virtual const bool dispatch(const Event& event);
    
    // Protected overrides from class Thread
  protected:
    
    /**
     * Waits for incoming events to be deposited on the event queue, then
     * dispatches them using the EventHandler dispatching mechanism.
     */
    virtual void run(void);
    
    /**
     * Destroys any remaining events in the queue
     */
    virtual void cleanup(void);
    
    // Protected interface methods
  protected:
    
    /**
     * Registers the instance to receive events of the specified class.  Only events
     * whose classes have been registered will be able to enqueue and awake the 
     * instance.
     * @param eventClass - Name of the event class to register for reception
     */
    void receiveEventClass(const string& eventClass);
    
    /**
     * Limit the number of events of the specified class that can build up in the
     * processing queue.
     * @param eventClass - Name of the event class for which the limit is imposed
     * @param maxNum - Maximum number of event instances which may accrue
     */
    void limitEvents(const string& eventClass, const int maxNum);
    
    // Private attributes
  private:
    
    /**
     * Queue of incoming event instances pending processing
     */
    list<Event*> eventQueue;
    
    /**
     * Vector of event classes that the instance is registered for
     */
    vector<string> eventClasses;
    
    /**
     * Map of event types to maximum limits
     */
    map<string, int> eventLimits;
    
    /**
     * Semaphore to signal to the worker thread that there are new events
     */
    Semaphore eventSem;
  };
}

#endif
