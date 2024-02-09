#ifndef _NAMEABLE_H_
#define _NAMEABLE_H_

// System headers
#include <string>

// Locate within the Lab X namespace
namespace LabX {

  /**
   * Abstraction for any sort of Nameable object.  This allows this common
   * aspect of object behavior to be "mixed in" at all levels of class hierarchy
   * using other abstractions without resulting in ambiguity due to slight 
   * differences of declaration, const-ness, etc.
   *
   * Due to its intended use as a mix-in interface class, subclasses should
   * always inherit this as a virtual base class, e.g.:
   *
   * class MyFoo : public virtual LabX::Nameable {
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class Nameable {

    // Constructor / destructor
  public:

    /**
     * Default constructor
     */
    Nameable(void) {}

    /**
     * Virtual destructor
     */
    virtual ~Nameable(void) {}

    // Public interface
  public:

    /**
     * Returns the name of the instance as a string.  Note the return by 
     * references, subclasses must not return a temporary object!
     *
     * @return name - Name of the instance
     */
    virtual const std::string& getName(void) const = 0;

  };

}

#endif
