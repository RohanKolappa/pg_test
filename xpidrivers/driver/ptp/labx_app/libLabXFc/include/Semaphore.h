#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

// System headers
#ifdef __APPLE__
#include <sys/semaphore.h>
#else
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

// Locally provide function prototypes for within Cygwin; the case difference
// between Semaphore.h and semaphore.h will not be distinguished.
#if defined(__CYGWIN__)

extern "C" {
  
typedef struct __sem_t {char __dummy;} *sem_t;

#define SEM_FAILED ((sem_t *) 0)

/* Semaphores */
int sem_init (sem_t *sem, int pshared, unsigned int value);
int sem_destroy (sem_t *sem);
sem_t *sem_open (const char *name, int oflag, ...);
int sem_close (sem_t *sem);
int sem_unlink (const char *name);
int sem_wait (sem_t *sem);
int sem_trywait (sem_t *sem);
int sem_timedwait (sem_t *sem, const struct timespec *abstime);
int sem_post (sem_t *sem);
int sem_getvalue (sem_t *sem, int *sval);

}

#endif

#include <semaphore.h>
#endif

#define __need_NULL
#include <stddef.h>
#include <stdint.h>

// Locate within the "LabX" namespace
namespace LabX {

  // Forward reference to class Thread, since there is a circular dependency
  class Thread;

  /**
   * This class encapsulates a POSIX inter-process semaphore.  Both mutex and 
   * counting semaphores are supported by the interface.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Semaphore {

    // Constructor / Destructor
  public:
    
    /**
     * Default constructor creates a binary semaphore
     * @param semName Name of the semaphore (for debug messages)
     */
    explicit Semaphore(const char* semName = NULL);
    
    /**
     * Creates a semaphore initialized to the passed value
     * @param value Initial value of the semaphore
     * @param semName Name of the semaphore (for debug messages)
     * @param longWait If waits are to take some time - true disables the deadlock
     *                 timeout
     */
    Semaphore(const unsigned int& value, const char* semName = NULL);
    
    /**
     * Destructor
     */
    virtual ~Semaphore(void);
    
    // Public interface methods
  public:
    
    /**
     * Blocks the calling thread until the semaphore value becomes nonzero,
     * then atomically decrement the value and returns.  Will wait indefinitely.
     */
    void wait(void);

    /**
     * Identical to wait(void), with the exception that the method will fail
     * with a thrown Exception instance if the passed timeout elapses before
     * the semaphore reaches a nonzero value.
     * @param timeoutMsec - Timeout, in milliseconds
     */
    void wait(uint32_t timeoutMsec);
    
    /**
     * Attempts to atomically decrement the semaphore's value returning true
     * if successful, false if the semaphore's value is zero.
     */
    const bool tryWait(void);
    
    /**
     * Atomically increments the value of the semaphore
     */
    void post(void);
    
    /**
     * Returns the instantaneous value of the semaphore
     */
    const int getValue(void) const;
    
    // Semaphores do not support copy or assignment semantics!
  private:
    
    /**
     * Copy constructor
     * @param other - Other semaphore instance to (not!) be copied
     */
    Semaphore(const Semaphore& other);
    
    /**
     * Assignment operator
     * @param other - Other semaphore instance to (not!) be assigned from
     */
    const Semaphore& operator=(const Semaphore& rhs);
    
    // Private helper methods
  private:
    
    /**
     * Initializes the passed POSIX semaphore to the passed value
     */
    void initPOSIXSemaphore(sem_t& posixSem, const unsigned int value);
    
    /**
     * Waits for the passed POSIX semaphore to become available
     * @param timeoutMsec - Timeout, in milliseconds, or zero for no timeout
     */
    void waitPOSIXSemaphore(sem_t& posixSem, uint32_t timeoutMsec);
    
    /**
     * Tries to decrement the passed POSIX semaphore in a non-blocking fashion
     */
    const bool tryWaitPOSIXSemaphore(sem_t& posixSem);
    
    /**
     * Posts to the passed POSIX semaphore
     */
    void postPOSIXSemaphore(sem_t& posixSem);
    
    /**
     * Destroys the passed POSIX semaphore
     */
    void destroyPOSIXSemaphore(sem_t& posixSem);
    
    // Private attributes
  private:
    
    /**
     * POSIX semaphore used to act as a mutex for state attributes that are used
     * to determine whether or not to wait on the encapsulated semaphore primitive.
     */
    sem_t mutexSem;
    
    /**
     * A boolean indicating whether this is a mutex semaphore, indicating
     * that multiple successive locks within the same thread are to be allowed
     * after the semValue has reached zero.
     */
    bool isMutex;
    
    /**
     * Counter used to keep track of multiple locks within the same thread of 
     * control
     */
    unsigned int lockCount;
    
    /**
     * Pointer to the thread instance currently locking the instance
     */
    Thread* lockingThread;
    
    /**
     * POSIX semaphore encapsulated by this instance
     */
    sem_t instanceSem;
    
    /**
     * Name of the semaphore (for debug purposes
     */
    const char* name;
  };
}

#endif
