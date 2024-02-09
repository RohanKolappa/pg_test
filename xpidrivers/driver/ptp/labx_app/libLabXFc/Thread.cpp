// System headers
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

// STL headers
#include <iostream>

// #define _LABXDEBUG

// Project headers
#include "Debug.h"
#include "Exception.h"
#include "Thread.h"

// Namespace using directives
using std::cerr;
using std::cout;
using std::endl;
using std::make_pair;
using std::map;
using std::string;

// Implementation of class Thread

// Static member initialization
map<pthread_t, LabX::Thread*> LabX::Thread::threadRegistry;

// The thread registry semaphore is used to *build* 
// the registry of running threads; we definitely need to disable multiple locks,
// or Semaphore will be looking for a registry entry for the thread we're
// trying to create an entry for! :)  To do this, we create the semaphore
// with an explicit initial value of 1.
LabX::Semaphore LabX::Thread::threadRegistrySem(1, "threadRegistrySem");

// Constructors / Destructor

LabX::Thread::Thread(const string& name) :
  name(name),
  threadHandle(0),
  threadRunningSem(0, "ThreadRunning"),
  schedulePriority(0),
  schedulePolicy(SCHED_OTHER),
  threadCancelled(false),
  threadCleanedUp(false) {}

LabX::Thread::~Thread(void) {
}

// Public overrides from class Nameable

const string& LabX::Thread::getName(void) const {
  return(name);
}

// Public interface methods

int32_t LabX::Thread::attachCurrent(void) {
  (void) dispatchRun(reinterpret_cast<void*>(this));
  return(returnCode);
}

void LabX::Thread::start(void) {
  pthread_attr_t attributes;
  sched_param param;

  // Ensure that this object is not already active, silently ignore if it is.
  if(!isRunning()) {
    DebugNoteVa("Starting thread \"%s\"", name.c_str());
    
    // Set up the thread's attributes to our liking
    if(pthread_attr_init(&attributes) != 0) {
      DebugError("Error from pthread_attr_init()!");
    }

    // Set a new scheduling policy
    pthread_attr_setschedpolicy(&attributes, schedulePolicy);
    
    // Set a new priority (if one has been assigned
    pthread_attr_getschedparam(&attributes, &param);
    param.sched_priority = schedulePriority;
    pthread_attr_setschedparam (&attributes, &param);
    
    // Start a new POSIX thread, passing this instance to the dispatch() method
    pthread_create(&threadHandle, &attributes, dispatchRun, reinterpret_cast<void*>(this));
    
    // Block until the new thread is actually running, then restore the
    // running semaphore
    threadRunningSem.wait();
    threadRunningSem.post();
    
    DebugNoteVa("Thread %s started!", name.c_str());
  } else {
    DebugWarningVa("Thread %s already running!", name.c_str());
  }
}

void LabX::Thread::stop(void) {
  DebugNoteVa("Cancelling thread \"%s\"", name.c_str());

  threadCancelled = true;

  // Cancel the thread.  Will return a value of ESRCH if no corresponding thread.
  if((threadHandle == 0) || (pthread_cancel(threadHandle) == ESRCH)) {
    DebugWarningVa("Thread \"%s\" not running!", name.c_str());
  }
}  

int32_t LabX::Thread::join(void) {
  // Check to see if the thread is even running at all, quietly exit if not
  if(threadRunningSem.getValue() > 0) {
    DebugNoteVa("Joining with thread %s", name.c_str());

    if (threadHandle == 0) {
      DebugWarningVa("Thread \"%s\" is being joined but was not started\n", name.c_str());
    } else {
      // Join with the encapsulated thread
      pthread_join(threadHandle, NULL);
    }
    
    // Indicate that the thread is no longer running.  This *requires*
    // a client to join on a thread if it wants to re-start it in the future,
    // but eliminates a deadlock condition between start() and cleanup() that
    // would exist if this were performed in cleanup().
    threadRunningSem.tryWait();
  }
  return(returnCode);
}

bool LabX::Thread::isRunning(void) const {
  return(threadRunningSem.getValue() != 0);
}

LabX::Thread* LabX::Thread::thisThread(void) {
  Thread* retValue = NULL;

  // Atomically search for the thread
  threadRegistrySem.wait();
  map<pthread_t, Thread*>::iterator iter = threadRegistry.find(pthread_self());
  if(iter != threadRegistry.end()) {
    retValue = iter->second;
  }
  threadRegistrySem.post();

  return(retValue);
}

void LabX::Thread::sleep(const uint32_t seconds) {
  // Sleep for the requested interval
  ::sleep(seconds);
}
  
// Abstract interface

void LabX::Thread::cleanup(void) {
  // Default implementation does nothing
}

// Protected helper methods

void LabX::Thread::checkPoint(void) {
  DebugNote("Testing for cancellation of thread (no name)");

  // Test for thread cancellation
  pthread_testcancel();
}

// Private helper methods

void* LabX::Thread::dispatchRun(void* clientParams) {
  // Cast the passed client parameters appropriately
  Thread* threadInstance = reinterpret_cast<Thread*>(clientParams);

  // Push the cancellation cleanup handler onto the cleanup stack, passing
  // the instance along
  pthread_cleanup_push(dispatchCleanup, clientParams);
    
  try {
    // Push the cancellation cleanup handler onto the cleanup stack, passing
    // the instance along. This is done both inside and outside the exception
    // handing to be sure it gets called at least once for both the exception
    // and non-exception versions of pthread cleanup.
    pthread_cleanup_push(dispatchCleanup, clientParams);
    
    // Register this thread instance
    threadRegistrySem.wait();
    threadRegistry.insert(make_pair(pthread_self(), threadInstance));
    threadRegistrySem.post();
    
    // Indicate that the thread is running
    threadInstance->threadRunningSem.post();

    // Invoke the thread's run() method, capturing its return code if it
    // returns normally
    threadInstance->returnCode = threadInstance->run();

    // Set the thread's cancellation state to disabled, pop and execute the cleanup handler.
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_cleanup_pop(true);
  
  } catch(Exception *exc) {
    threadInstance->returnCode = k_CaughtException;
    cerr << "Caught Exception at top of thread \""
         << threadInstance->getName()
         << "\": "
         << endl
         << exc->toString()
         << endl
         << "Exiting..."
         << endl;
    delete(exc);
  } catch(...) {
    if (!threadInstance->threadCancelled) {
      threadInstance->returnCode = k_CaughtException;
      cerr << "Caught unknown exception at top of thread \""
           << threadInstance->getName()
           << "\", exiting..."
           << endl;
    } else {
      if (threadInstance->threadHandle != 0) {
        // Detach the thread to allow its resources to be deallocated
        pthread_detach(threadInstance->threadHandle);
      }

      throw;
    }
  }
  
  // Set the thread's cancellation state to disabled, pop and execute the cleanup handler.
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  pthread_cleanup_pop(true);
  
  if (threadInstance->threadHandle != 0) {
    // Detach the thread to allow its resources to be deallocated
    pthread_detach(threadInstance->threadHandle);
  }

  // Return okay exit status
  return(NULL);
}

void LabX::Thread::dispatchCleanup(void* clientParams) {
  // Cast the passed client parameters appropriately
  Thread* threadInstance = reinterpret_cast<Thread*>(clientParams);

  if (!threadInstance->threadCleanedUp) {
    threadInstance->threadCleanedUp = true;

    // Invoke the polymorphic cleanup() method
    threadInstance->cleanup();

    // Atomically remove the instance from the registry
    threadRegistrySem.wait();
    map<pthread_t, Thread*>::iterator iter = threadRegistry.find(threadInstance->threadHandle);
    if(iter != threadRegistry.end()) {
      threadRegistry.erase(iter);
    }
    threadRegistrySem.post();
  }
}
