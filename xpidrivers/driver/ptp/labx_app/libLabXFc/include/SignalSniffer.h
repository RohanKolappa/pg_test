#ifndef _SIGNAL_SNIFFER_H_
#define _SIGNAL_SNIFFER_H_

// System headers
#include <signal.h>
#include <string>

// Project headers
#include "Semaphore.h"
#include "Thread.h"

// Macro definitions

// The number of signals supported by the OS and this class
#if defined(__APPLE__) || defined(__CYGWIN__)
#define NUM_OS_SIGNALS NSIG
#else
#define NUM_OS_SIGNALS _NSIG
#endif

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an active thread that "sniffs" for the OS signal passed
   * through its constructor.  Concrete classes that inherit from this class 
   * should only be instantiated once, ideally through a Singleton interface, and
   * should be created before any other worker threads are started (instances may
   * be started at any time, order of creation is the important aspect).
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class SignalSniffer : public Thread {

    // Constructor / Destructor
  public:
  
    /**
     * Creates a new instance to sniff for the passed signal.  Concrete
     * instances of this class must be created and started by the application's
     * main thread, before any other worker threads are created to ensure that
     * the instance is the only possible target for delivery of its signal.
     * Although this class inherits the CSingleton interface, it does not 
     * implement it since it is not a concrete class.  Therefore, it's okay to 
     * have a public constructor!
     * @param name - Name of the sniffer's worker thread
     * @param sniffSignal - Signal to be sniffed
     */
    SignalSniffer(const std::string& name, const int& sniffSignal);
    
    /**
     * Destructor
     */
    virtual ~SignalSniffer(void);
    
    // Public overrides from class Thread
  public:
    
    /**
     * Blocks the instance's signal from the calling thread, then calls the
     * default implementation to start the sniffer thread.
     */
    virtual void start(void);
    
    // Abstract interface inherited from class Thread
  protected:
    
    /**
     * Unblocks the instance's signal for this thread, then begins "sniffing"
     * and responding to caught signals.
     * @param The return status for the thread
     */
    virtual int32_t run(void);
    
    // Protected interface methods
  protected:
    
    /**
     * Performs any configuration that must be performed after the thread's signal
     * mask is set up to receive the instance's signal.  Default implementation does
     * nothing.
     */
    virtual void preSniffConfig(void);
    
    /**
     * Handles the instance's signal in an independent thread of control; must
     * be implemented by the derived class.
     */
    virtual void handleSignal(void) = 0;

    /**
     * Method for other derived class' methods to let us know of signal delivery to
     * a different thread than the one created.
     * @param signalNum - Signal number delivered
     */
    static void signalDelivered(int signalNum);
    
    // Private helper methods
  private:
    
    /**
     * Signal handler for the signal
     * @param signalNum - Number of the caught signal
     */
    static void signalHandler(int signalNum);
    
    /**
     * Validates the passed signal number
     * @param signalNum - Number of the candidate signal
     * @return - True if the signal number is supported by the OS
     */
    static const bool validateSignalNumber(const int& signalNum);
    
    // Private attributes
  private:
    
    /**
     * The signal for the instance to sniff for
     */
    const int sniffSignal;
    
    /**
     * A semaphore for the signal handler to let the worker thread know when
     * a signal has been received
     */
    Semaphore signalSem;
    
    // Private class attributes
  private:
    
    /**
     * Maps a given operating system signal to a signal sniffer instance
     * created to handle it.
     */
    static SignalSniffer* snifferRegistry[NUM_OS_SIGNALS];
  };
}

#endif
