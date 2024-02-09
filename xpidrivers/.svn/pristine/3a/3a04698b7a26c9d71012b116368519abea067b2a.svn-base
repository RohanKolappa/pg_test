#ifndef _THREAD_H_
#define _THREAD_H_

// System headers
#include <pthread.h>
#include <map>
#include <stdint.h>
#include <string>

// Library headers
#include "Nameable.h"
#include "Semaphore.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an "active object" abstraction, presenting both a public
   * interface for child thread management, as well as a protected interface for
   * concrete subclasses to customize the behavior of their encapsulated worker
   * thread.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Thread : public virtual Nameable {

    // Public type definitions
  public:

    /**
     * Enumeration for common return codes; subclasses may define
     * their own negative-valued enumerations beginning with k_DerivedCodeBase.
     */
    typedef enum {
      k_CaughtException = -2,
      k_BadParameter    = -1,
      k_Success         =  0,
      k_DerivedCodeBase = (k_CaughtException - 1),
    } ReturnCode;

    // Constructors / Destructor
  public:
    
    /**
     * Create a named thread instance
     */
    Thread(const std::string& name);
    
    /**
     * Destroys the instance
     */
    virtual ~Thread(void);

    // Public overrides from class Nameable
  public:

    /**
     * Returns the thread's name
     */
    virtual const std::string& getName(void) const;
    
    // Public interface methods
  public:
    
    /**
     * Attach this thread object to the current thread
     * This does not return until run() exits.
     * @return The return code from the run() method
     */
    virtual int32_t attachCurrent(void);
    
    /**
     * Starts the thread instance
     */
    virtual void start(void);
    
    /**
     * Cancels the thread.  Thread cancellation is deferred, allowing the thread
     * to die only when blocked or at an internal call to checkPoint().
     */
    virtual void stop(void);
    
    /**
     * Blocks the calling thread until this thread exits.
     * @return The return code from the run() method
     */
    virtual int32_t join(void);
    
    /**
     * Set the initial scheduling policy. This must be
     * called before the thread is started.
     */
    void setSchedulingPolicy(int32_t policy) { schedulePolicy = policy; }
    
    /**
     * Set the initial scheduling policy. This must be
     * called before the thread is started.
     */
    void setSchedulingPriority(int32_t priority) { schedulePriority = priority; }

    /**
     * Returns the running status for the instance
     * @return True if the thread is running, false otherwise
     */
    bool isRunning(void) const;
    
    /**
     * Returns a pointer to the Thread instance encapsulating the current thread
     * of control, or NULL if the thread was created outside of a Thread instance.
     */
    static Thread* thisThread(void);
    
    /**
     * Suspends the current thread for the passed number of seconds
     */
    static void sleep(const uint32_t seconds);
    
    // Abstract interface
  protected:
    
    /**
     * Thread begins running in this method, which must be implemented by the
     * derived class
     * @return An appropriate error code, either selected from the ReturnCode
     *         enumeration or a subclass-defined extension of it.
     */
    virtual int32_t run(void) = 0;
    
    /**
     * Called when the thread exits or is killed
     */
    virtual void cleanup(void);
    
    // Protected helper methods
  protected:
    
    /**
     * Tests for thread cancellation
     */
    void checkPoint(void);
    
    // Private helper methods
  private:
    
    /**
     * Static method to dispatch to the polymorphic run() method, invoked
     * upon the correct instance
     */
    static void* dispatchRun(void* clientParams);
    
    /**
     * Static method to dispatch to the polymorphic cleanup() method, invoked
     * upon the correct instance
     */
    static void dispatchCleanup(void* clientParams);
    
    // Private members
  private:
    
    /**
     * Thread name
     */
    const std::string name;
    
    /**
     * Thread handle
     */
    pthread_t threadHandle;
    
    /**
     * Semaphore to indicate whether the thread is running
     */
    Semaphore threadRunningSem;
    
    /**
     * Priority assigned to this thread on creation
     */
    int schedulePriority;
    
    /**
     * Scheduling policy assigned to this thread on creation
     */
    int schedulePolicy;

    /**
     * Return code from the thread's run() method
     */
    int32_t returnCode;

    /**
     * Thread was cancelled
     */
    bool threadCancelled;
    
    /**
     * Thread was cleaned up
     */
    bool threadCleanedUp;
    
    /**
     * Maps thread handles to their encapsulating Thread instances
     */
    static std::map<pthread_t, Thread*> threadRegistry;
    
    /**
     * A semaphore to prevent concurrent access to the the threadRegistry.
     */
    static Semaphore threadRegistrySem;
  };
}

#endif
