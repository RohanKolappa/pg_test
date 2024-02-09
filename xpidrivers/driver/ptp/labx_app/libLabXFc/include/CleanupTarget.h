#ifndef _CLEANUP_TARGET_H_
#define _CLEANUP_TARGET_H_

// STL headers
#include <list>

// Library headers
#include "Lockable.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This utility class abstracts an object which will notify
   * registered Observers which need to know when the object is
   * about to be destroyed (e.g. to remove any pointers or handles
   * to the object).
   *
   * Copyright (c) 2011, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class CleanupTarget : public virtual Lockable {

    // Constructor / destructor
  public:

    /**
     * Default constructor
     */
    CleanupTarget(void);

    /**
     * Virtual destructor
     */
    virtual ~CleanupTarget(void);

    // Public type definitions
  public:

    /**
     * Interface class allowing other objects to act as Observers
     * of the target
     */
    class Observer {
      
      // Constructor / destructor
    public:

      /**
       * Constructor
       */
      Observer(void) {}

      /**
       * Virtual destructor
       */
      virtual ~Observer(void) {}

      // Abstract public interface
    public:

      /**
       * Method to notify the implementor when a target is cleaning
       * up in preparation for destruction
       *
       * @param targetInstance - Instance about to be destroyed
       */
      virtual void instanceCleanup(const CleanupTarget *targetInstance) = 0;

    };

    // Public interface methods
  public:

    /**
     * Registers the passed Observer to receive cleanup notification.
     * This is declared const; the instance's list of observers is
     * considered to be not a true attribute of the object.
     *
     * @param observer - Observer instance
     */
    void registerObserver(Observer *observer) const;

    /**
     * Unregisters the passed Observer
     *
     * @param observer - Observer instance
     */
    void unregisterObserver(Observer *observer) const;

    /**
     * Invoked when the passed object is about to be destroyed.  The
     * default implementation simply notifies all Observers of the
     * impending destruction; derived classes may call this before or
     * after their polymorphic cleanup behavior.
     *
     * @param targetInstance - Instance about to be destroyed
     */
    virtual void cleanup(void);

    // Private attributes
  private:

    /**
     * Collection of registered Observers
     */
    mutable std::list<Observer*> observers;
    
  };

}

#endif
