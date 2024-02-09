#ifndef _SHUTDOWN_HANDLER_H_
#define _SHUTDOWN_HANDLER_H_

// System headers
#include <vector>

// Project headers
#include "SignalSniffer.h"
#include "LabX.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an interface for waking up threads when a
   * shutdown event (SIGINT) is caught.
   *
   * Copyright (c) 2011, Lab X Technologies, LLC
   * @author Chris Wulff
   */
  class ShutdownHandler : public SignalSniffer {
    
    // Destructor
  public:
    
    /**
     * Destructor
     */
    virtual ~ShutdownHandler(void);
    
    // Protected overrides from class SignalSniffer
  protected:
    
    /**
     * Wake up anyone waiting on the shutdown condition
     */
    virtual void handleSignal(void);
    
    // Public singleton interface
  public:
    
    /**
     * Returns the singleton instance of ShutdownHandler, creating it if necessary
     * @return - The singleton instance
     */
    static ShutdownHandler& instance(void);
    
    /**
     * Destroys the singleton instance, if it exists.
     */
    static void destroy(void);
    
    // Public interface
  public:

    /**
     * Wait for a shutdown event; invoking this method permits a CTRL-C on
     * the calling thread to also issue the shutdown signal, if it is the
     * console's primary thread of control.
     */
    void awaitShutdown(void);

    // Private constructor
  private:

    /**
     * Creates a new timer object
     */
    ShutdownHandler(void);

    // Private helper methods
  private:

    /**
     * Signal handler for the signal; this may be invoked either by
     * happenstance due to delivery from another process, or by a 
     * CTRL-C issued from a console's primary thread of control.
     *
     * @param signalNum - Number of the caught signal
     */
    static void signalHandler(int signalNum);

    // Private attributes
  private:

    /**
     * A semaphore to serialize access to the timers
     */
    Semaphore timerSem;

    // Private class attributes
  private:

    /**
     * A pointer to the singleton instance
     */
    static ShutdownHandler* shutdownInstance;

    /**
     * Mutex for the wakeupCondition
     */
    pthread_mutex_t wakeupMutex;

    /**
     * Wakeup condition
     */
    pthread_cond_t wakeupCondition;

    /**
     * Shutdown event already received
     */
    bool shutdownInProgress;
  };
}

#endif
