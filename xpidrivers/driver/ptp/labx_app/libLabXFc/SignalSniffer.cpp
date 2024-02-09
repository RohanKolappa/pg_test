// Project headers
#include "Debug.h"
#include "SignalSniffer.h"

// Namespace using directives
using LabX::SignalSniffer;
using std::string;

// Implementation of class SignalSniffer

// Static member initialization

SignalSniffer* SignalSniffer::snifferRegistry[NUM_OS_SIGNALS];

// Constructor / Destructor

SignalSniffer::SignalSniffer(const string& name, 
                             const int& sniffSignal) :
  Thread(name),
  sniffSignal(sniffSignal),
  signalSem(0, "SignalSniffer") {
  // Validate the signal number to sniff for
  if(validateSignalNumber(sniffSignal) == false) {
    DebugErrorVa("Invalid signal number %u!", sniffSignal);
  } else {
    // Register this instance as the handler for the passed signal
    snifferRegistry[sniffSignal] = this;
    DebugNote("Registered signal");
  }
}

SignalSniffer::~SignalSniffer(void) {
  // Unregister the instance
  snifferRegistry[sniffSignal] = NULL;
}

// Public overrides from class Thread

void SignalSniffer::start(void) {
  sigset_t signalVec;

  // Block the calling thread from receiving the instance's signal.  Any
  // child threads created after this call should inherit this signal mask,
  // so this thread should be the only one able to catch it.
  sigemptyset(&signalVec);
  sigaddset(&signalVec, sniffSignal);
  pthread_sigmask(SIG_BLOCK, &signalVec, NULL);

  // Call the default implementation to actually start the worker thread
  Thread::start();
}

// Abstract interface inherited from class Thread

int32_t SignalSniffer::run(void) {
  sigset_t signalVec;
  struct sigaction action;

  DebugNote("SignalSniffer Started");

  // Block all signals for the thread
  sigfillset(&signalVec);
  pthread_sigmask(SIG_BLOCK, &signalVec, NULL);

  // Register a signal handler for the instance's signal and allow this thread
  // to catch it
  action.sa_handler = &SignalSniffer::signalHandler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(sniffSignal, &action, NULL);

  // Now unblock the instance's signal for this thread to make it the de-facto
  // handling thread for the process.
  sigemptyset(&signalVec);
  sigaddset(&signalVec, sniffSignal);
  pthread_sigmask(SIG_UNBLOCK, &signalVec, NULL);

  // Execute any derived class configuration that needs to be done after the
  // signal mask is set
  preSniffConfig();

  // Worker thread runs continuously until stopped
  while(true) {
    // Wait for the signal handler to let us know when the signal has been
    // caught
    signalSem.wait();
    
    // Invoke the polymorphic handle() method
    handleSignal();
  }

  return(Thread::k_Success);
}

// Protected interface methods

void SignalSniffer::preSniffConfig(void) {
  // Default implementation does nothing
}

void SignalSniffer::signalDelivered(int signalNum) {
  // Directly invoke our own handler
  signalHandler(signalNum);
}

// Private helper methods

void SignalSniffer::signalHandler(int signalNum) {
  // Find the sniffer instance associated with this signal and wake up its
  // worker thread
  snifferRegistry[signalNum]->signalSem.post();
}

const bool SignalSniffer::validateSignalNumber(const int& signalNum) {
  // The signal number must be non-negative, and less than the number of
  // signal supported by the OS
  return((signalNum >= 0) && (signalNum < NUM_OS_SIGNALS));
}
