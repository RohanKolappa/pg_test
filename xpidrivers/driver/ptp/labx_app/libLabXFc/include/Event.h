#ifndef _EVENT_H_
#define _EVENT_H_

// System headers
#include <string>

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides a simple interface for representing events by type
   * and inherits semantics for setting and retrieving property values for
   * string types.  Events that need to carry specialized data may subclass.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Event {
    // Constructor / Destructor
  public:
    
    /**
     * Default constructor
     */
    Event(void);
    
    /**
     * Copy constructor
     */
    Event(const Event& other);
    
    /**
     * Virtual destructor
     */
    virtual ~Event(void);
    
    // Public interface methods
  public:
    
    /**
     * Returns the name of the event class.  Subclasses should return a public,
     * static string instance, since this is returned by reference!
     * @return - The name of the event class
     */
    virtual const std::string& getClassName(void) const = 0;
    
    /**
     * Returns a string representation of the event
     * @return - A textual rendering of the event
     */
    virtual const std::string toString(void) const = 0;
    
    /**
     * Allocates and returns a clone of the instance.
     * @return - A pointer to a new, dynamically-allocated clone
     */
    virtual Event* clone(void) const = 0;
  };
}

#endif

