// System headers
#include <list>
#include <sys/time.h>

// #define _LABXDEBUG

// Project headers
#include "Debug.h"
#include "ShutdownHandler.h"

// Namespace using directives
using LabX::ShutdownHandler;

// Signal identified as the "shutdown" command from the system
#define SHUTDOWN_SIGNAL (SIGINT)

// Implementation of class ShutdownHandler

// Static initialization

ShutdownHandler* ShutdownHandler::shutdownInstance = NULL;

// Destructor

ShutdownHandler::~ShutdownHandler(void) {}

// Protected overrides from class SignalSniffer

void ShutdownHandler::handleSignal(void) {
  // Wake up all waiters
  pthread_mutex_lock(&wakeupMutex);
  shutdownInProgress = true;
  pthread_cond_broadcast(&wakeupCondition);
  pthread_mutex_unlock(&wakeupMutex);
}

// Public singleton interface

ShutdownHandler& ShutdownHandler::instance(void) {
  // Create the singleton instance if it doesn't exist yet
  if(shutdownInstance == NULL) shutdownInstance = new ShutdownHandler();

  // Return the singleton instance by reference
  return(*shutdownInstance);
}

void ShutdownHandler::destroy(void) {
  // Destroy the singleton instance
  delete(shutdownInstance);
  shutdownInstance = NULL;
}

// Public timer interface

void ShutdownHandler::awaitShutdown(void) {
  sigset_t signalVec;
  struct sigaction action;

  Thread *clientThread = Thread::thisThread();
  DebugNoteVa("LabX::ShutdownHandler::awaitShutdown() invoked by thread \"%s\"",
              ((clientThread != NULL) ? clientThread->getName().c_str() : "<UNKNOWN>"));

  // Unblock the shutdown signal
  sigemptyset(&signalVec);
  sigaddset(&signalVec, SHUTDOWN_SIGNAL);
  pthread_sigmask(SIG_UNBLOCK, &signalVec, NULL);

  // Register a signal handler for the instance's signal and allow this thread
  // to catch it.
  action.sa_handler = &LabX::ShutdownHandler::signalHandler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SHUTDOWN_SIGNAL, &action, NULL);

  // Await the shutdown, whether it comes from this thread or the one originally
  // started for the instance.
  pthread_mutex_lock(&wakeupMutex);

  while (!shutdownInProgress) {
    pthread_cond_wait(&wakeupCondition, &wakeupMutex);
  }

  pthread_mutex_unlock(&wakeupMutex);
}

// Private constructor

ShutdownHandler::ShutdownHandler(void) :
  SignalSniffer("Application ShutdownHandler", SHUTDOWN_SIGNAL),
  shutdownInProgress(false) {

  pthread_mutex_init(&wakeupMutex, NULL);
  pthread_cond_init(&wakeupCondition, NULL);
  
  DebugNote("ShutdownHandler initialized");
}

// Private helper methods

void ShutdownHandler::signalHandler(int signalNum) {
  // Let the base class know of the signal delivery, relying upon it to
  // navigate to the proper instance / thread to perform the shutdown.
  SignalSniffer::signalDelivered(signalNum);
}
