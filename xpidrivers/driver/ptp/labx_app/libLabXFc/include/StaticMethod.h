#ifndef _STATIC_METHOD_H_
#define _STATIC_METHOD_H_

// Project headers
#include "Method.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class encapsulates a static class member or a global function.
   * The method is stored as a regular function pointer.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class StaticMethod : public Method {

  // Constructor / Destructor
 public:

  /**
   * Constructor accepts a funtion pointer
   * @param staticFunc - Static function pointer to be encapsulated
   */
  explicit StaticMethod(void (*staticFunc)(void)) :
    Method(),
      staticFunc(staticFunc) {}
   
    /**
     * Virtual destructor
     */
    virtual ~StaticMethod(void) {}
   
    // Public overrides from class Method
  public:
   
    /**
     * Invokes the stored function pointer.
     */
    virtual void operator()(void) const {
      // Invoke the stored function pointer
      staticFunc();
    }

    /**
     * Uses the stored function pointer as the basis for determining equality.
     * @param other - Method instance to be compared for equality
     */
    virtual const bool equals(const Method& other) const {
      bool retValue = false;
      
      // Compare only to other static method instances
      const StaticMethod* otherStaticMethod = 
        dynamic_cast<const StaticMethod*>(&other);
      if(otherStaticMethod != NULL) {
        retValue = (staticFunc == otherStaticMethod->staticFunc);
      }

      return(retValue);
    }

    // Private attributes
  private:
    
    /**
     * Pointer to the static or global method / function
     */
    void (*staticFunc)(void);
  };
}

#endif
