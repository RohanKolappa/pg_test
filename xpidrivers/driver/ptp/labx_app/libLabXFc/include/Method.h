#ifndef _METHOD_H_
#define _METHOD_H_

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Abstract class Method provides a base class for the two types
   * of methods; static or global methods, and class member functions.  
   * Although the implementation of these two cases are different,
   * this class provides a common base class for storing and invoking them.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Method {
    // Constructor / Destructor
  public:
    
    /**
     * Default Constructor
     */
    Method(void) {}
  
    /**
     * Virtual destructor
     */
    virtual ~Method(void) {}

    // Public interface methods
  public:
    
    /**
     * Invokes the method
     */
    virtual void operator()(void) const = 0;
    
    /**
     * Returns true if the passed method instance is equivalent to the instance.
     */
    virtual const bool equals(const Method& other) const = 0;
  };
}

#endif
