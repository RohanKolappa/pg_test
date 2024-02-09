// System headers
#define _XOPEN_SOURCE 600
#include <errno.h>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <stdint.h>
#include <stdio.h>

// Project headers
#include "Debug.h"
#include "Exception.h"
#include "Semaphore.h"
#include "Thread.h"
#include "LabX.h"

// Timeout periods
#define MSECS_PER_SECOND (      1000L)
#define NSECS_PER_MSEC   (   1000000L)
#define NSECS_PER_SECOND (1000000000L)

// Namespace using directives
using std::stringstream;

// Implementation of class Semaphore

// Constructor / Destructor

LabX::Semaphore::Semaphore(const char* semName) :
  mutexSem(),
  isMutex(true),
  lockCount(0),
  lockingThread(NULL),
  instanceSem(),
  name(semName) {
  // Initialize the encapsulated POSIX semaphores as binary semaphores
  initPOSIXSemaphore(mutexSem, 1);
  initPOSIXSemaphore(instanceSem, 1);
}

LabX::Semaphore::Semaphore(const unsigned int& value, const char* semName) :
  mutexSem(),
  isMutex(false),
  lockCount(0),
  lockingThread(NULL),
  instanceSem(),
  name(semName) {
  // Initialize the encapsulated POSIX semaphore to the passed value
  initPOSIXSemaphore(mutexSem, 1);
  initPOSIXSemaphore(instanceSem, value);
}

LabX::Semaphore::~Semaphore(void) {
  // Destroy the encapsulated POSIX semaphores
  destroyPOSIXSemaphore(mutexSem);
  destroyPOSIXSemaphore(instanceSem);
}

// Public interface methods

void LabX::Semaphore::wait(void) {
  // Dispatch to the main implementation with a zero timeout
  wait(0);
}

void LabX::Semaphore::wait(uint32_t timeout) {
  if ((uintptr_t)this < 0x1000) {
    printf("%s: Bad this pointer\n",__func__);
    Thread* attemptThread = Thread::thisThread();
    printf("In thread %p \"%s\")!",
           attemptThread,
           (attemptThread ? attemptThread->getName().c_str() : "<NULL>"));
    while(1);
  }
  if(isMutex){
    Thread* attemptThread = Thread::thisThread();

    // Obtain mutually exclusive access to the semaphore
    waitPOSIXSemaphore(mutexSem, 0);

    // Check for a multiple lock
    if((lockCount == 0) ||
       (lockingThread == NULL) ||
       (lockingThread != attemptThread)) {
      // This is not a multiple lock, wait on the instance semaphore, releasing
      // the mutex first (thread-safe pthreads does this automatically! :( )
      postPOSIXSemaphore(mutexSem);
      waitPOSIXSemaphore(instanceSem, timeout);
      waitPOSIXSemaphore(mutexSem, 0);
      
      // Locked the first time, leave behind our thread as the locker
      lockingThread = attemptThread;
    }

    // Increment the lock count whether we waited or not
    lockCount++;
    
    // Release mutually exclusive access
    postPOSIXSemaphore(mutexSem);
  } else {
    // Not a mutex, wait on the instance semaphore
    waitPOSIXSemaphore(instanceSem, timeout);
  }
}

const bool LabX::Semaphore::tryWait(void) {
  bool retValue = true;
  
  if(isMutex) {
    Thread* attemptThread = Thread::thisThread();
    
    // Obtain mutually exclusive access to the semaphore
    waitPOSIXSemaphore(mutexSem, 0);
    
    // Check for a multiple lock
    if((lockCount == 0) ||
       (lockingThread == NULL) ||
       (lockingThread != attemptThread)) {
      // This is not a multiple lock, trywait on the instance semaphore
      if((retValue = tryWaitPOSIXSemaphore(instanceSem)) == true) {
        // Locked the first time, leave behind our thread as the locker
        lockingThread = attemptThread;
      }
    }
    
    // Increment the lock count upon success whether we waited or not
    if(retValue == true) lockCount++;
    
    // Release mutually exclusive access
    postPOSIXSemaphore(mutexSem);
  } else {
    // Not a mutex, just trywait on the instance semaphore
    retValue = tryWaitPOSIXSemaphore(instanceSem);
  }

  return(retValue);
}

void LabX::Semaphore::post(void) {
  if(isMutex) {
    bool shouldPost = false;
    
    // Obtain mutually exclusive access to the semaphore
    waitPOSIXSemaphore(mutexSem, 0);
    
    if((lockingThread == Thread::thisThread()) && (--lockCount == 0)) {
      lockingThread = NULL;
      shouldPost = true;
    }
    
    // Release mutually exclusive access to the semaphore
    postPOSIXSemaphore(mutexSem);
    
    // Do the post after releasing the mutex, since any waiting threads will
    // need to immediately re-acquire it to update the lockCount and lockingThread
    // states.
    if(shouldPost) postPOSIXSemaphore(instanceSem);
  } else {
    postPOSIXSemaphore(instanceSem);
  }
}

const int LabX::Semaphore::getValue(void) const {
  int retValue;

  // Const-cast since we know instanceSem will not be modified
  if(sem_getvalue(const_cast<sem_t*>(&instanceSem), &retValue) != 0) {
    // Switch on the error
    switch(errno) {
    case EINVAL: {
      DebugError("Cannot sem_getvalue() on invalid POSIX semaphore!");
      break;
    }
    case ENOSYS: {
      DebugError("OS does not support sem_getvalue()!");
      break;
    }
    default: {
      DebugError("Unidentified error in sem_getvalue()!");
    }
    }
  }

  return(retValue);
}

// Private helper methods

void LabX::Semaphore::initPOSIXSemaphore(sem_t& posixSem, 
                                         const unsigned int value) {
  // Attempt to initialize the passed POSIX semaphore.  All Semaphore instances are
  // strictly intra-process.
  if(sem_init(&posixSem, 0, value) != 0) {
    // Switch on the error
    switch(errno) {
    case EINVAL: {
      DebugErrorVa("Value %u passed to sem_init() exceeds SEM_VALUE_MAX (%u)!",
                   value,
                   SEM_VALUE_MAX);
      break;
    }
    case ENOSPC: {
      DebugError("Insufficient resources to sem_init() POSIX semaphore!");
      break;
    }
    case ENOSYS: {
      DebugError("OS does not support sem_init()!");
      break;
    }
    case EPERM: {
      DebugError("Process lacks privilege to sem_init() POSIX semaphore!");
      break;
    }
    default: {
      DebugError("Unidentified error in sem_init()!");
    }
    } // case(errno)
  }
}

void LabX::Semaphore::waitPOSIXSemaphore(sem_t& posixSem, uint32_t timeoutMsec) {
  bool doneWaiting = false;

  while(doneWaiting == false) {
    bool bAcquired = false;
    if (sem_trywait(&posixSem) == 0) {
      bAcquired = true;
    } else {
      struct timespec timeoutPoint;

      if(timeoutMsec != 0) {
        uint32_t seconds     = (timeoutMsec / MSECS_PER_SECOND);
        uint32_t nanoseconds = ((timeoutMsec - (seconds * MSECS_PER_SECOND)) * NSECS_PER_MSEC);
        clock_gettime(CLOCK_REALTIME, &timeoutPoint);

        timeoutPoint.tv_sec  += seconds;
        timeoutPoint.tv_nsec += nanoseconds;
        if(timeoutPoint.tv_nsec >= NSECS_PER_SECOND) {
          timeoutPoint.tv_sec++;
          timeoutPoint.tv_nsec -= NSECS_PER_SECOND;
        }
        
        if(sem_timedwait(&posixSem, &timeoutPoint) == 0) {
          // Semaphore has now been acquired
          bAcquired = true;
        }
      } else {
        // Wait indefinitely
        if (0 == sem_wait(&posixSem)) {
          // Semaphore has now been acquired
          bAcquired = true;
        }
      }
    }

    if (bAcquired) {
      // Success, break loop!
      doneWaiting = true;
    } else {
      // Switch on the error
      switch(errno)
        {
        case EINVAL:
          {
            DebugError("Cannot sem_wait() on invalid POSIX semaphore!");
            doneWaiting = true;
            break;
          }
        case ENOSYS:
          {
            DebugError("OS does not support sem_wait()!");
            doneWaiting = true;
            break;
          }
        case EDEADLK:
          {
            DebugErrorVa("Deadlock detected by sem_wait() on %p \"%s\" (locked by thread %p \"%s\")!",
                         this, (name) ? name : "", lockingThread, lockingThread ? lockingThread->getName().c_str() : "");
            doneWaiting = true;
            break;
          }
        case EINTR:
          {
            // Interrupted by signal delivery, quietly resume waiting
            //fprintf(stderr, "S");
            break;
          }
        case EAGAIN:
          {
            // This apparently can be caused by a time discontinuity (system clock change)
            //fprintf(stderr, "T");
            break;
          }
        case ETIMEDOUT:
          {
            stringstream excStream;
            excStream << "Semaphore \""
                      << ((name != NULL) ? name : "(un-named)")
                      << "\" timed out! (locked by thread "
                      << toHexString((uintptr_t)lockingThread, sizeof(uintptr_t))
                      << "\"" << (lockingThread ? lockingThread->getName().c_str() : "") << "\")";
            throw(new Exception(excStream.str()));
          }
        default:
          {
            DebugErrorVa("Unidentified error %d in sem_wait %p \"%s\" (locked by thread %p \"%s\")",
                         errno, this, (name) ? name : "", lockingThread, lockingThread ? lockingThread->getName().c_str() : "");
            break;
          }
        } // switch(errno)
    } // if(!bAcquired)
  } // while(doneWaiting == false)

  return;
}

const bool LabX::Semaphore::tryWaitPOSIXSemaphore(sem_t& posixSem) {
  bool retValue = true;

  // Try to atomically decrement the semaphore
  if(sem_trywait(&posixSem) != 0)
    {
      // Switch on the error
      switch(errno)
        {
        case EAGAIN:
          {
            // The semaphore could not immediately be locked, return false
            retValue = false;
            break;
          }
        case EINVAL:
          {
            DebugError("Cannot sem_trywait() on invalid POSIX semaphore!");
            break;
          }
        case ENOSYS:
          {
            DebugError("OS does not support sem_trywait()!");
            break;
          }
        case EDEADLK:
          {
            DebugError("Deadlock detected by sem_trywait()!");
            break;
          }
        default:
          {
            DebugError("Unidentified error in sem_trywait!");
          }
        } // switch(errno)
    } // if(sem_trywait(&posixSem) != 0)

  return(retValue);
}

void LabX::Semaphore::postPOSIXSemaphore(sem_t& posixSem) {
  // Try to atomically increment the semaphore
  if(sem_post(&posixSem) != 0)
    {
      // Switch on the error
      switch(errno)
        {
        case EINVAL:
          {
            DebugError("Cannot sem_post() on invalid POSIX semaphore!");
            break;
          }
        case ENOSYS:
          {
            DebugError("OS does not support sem_post()!");
            break;
          }
        default:
          {
            DebugError("Unidentified error in sem_post()!");
          }
        }
    }
}

void LabX::Semaphore::destroyPOSIXSemaphore(sem_t& posixSem) {
  if(sem_destroy(&posixSem) != 0)
    {
      // Switch on the error number
      switch(errno)
        {
        case EINVAL:
          {
            DebugError("Cannot destroy invalid POSIX semaphore!");
            break;
          }
        case ENOSYS:
          {
            DebugError("OS does not support sem_destroy()!");
            break;
          }
        case EBUSY:
          {
            DebugError("Cannot destroy POSIX semaphore while in use!");
            break;
          }
        default:
          {
            DebugError("Unidentified error destroying POSIX semaphore!");
          }
        }
    }
}
