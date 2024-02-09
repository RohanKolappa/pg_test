#ifndef _C_LOCKABLE_H_
#define _C_LOCKABLE_H_

// Project headers
#include "Semaphore.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class abstracts a lockable object, providing semantics for serializing
   * concurrent access to the instance.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Lockable {

    // Destructor
  public:
    
    /**
     * Virtual destructor
     */
    virtual ~Lockable(void);
    
    // Public interface methods
  public:
    
    /**
     * Locks the instance, blocking until the operation is possible
     */
    void lock(void) const;
    
    /**
     * Unlocks the instance
     */
    void unlock(void) const;
    
    // Protected constructor
  protected:
    
    /**
     * Default constructor
     */
    explicit Lockable(const bool debugLocks = false);
    
    // Private attributes
  private:

    /**
     * Debugging flag
     */
    const bool debugLocks;
    
    /**
     * Mutex semaphore to implement locking semantics
     */
    mutable Semaphore lockSem;
  };
}

#endif
