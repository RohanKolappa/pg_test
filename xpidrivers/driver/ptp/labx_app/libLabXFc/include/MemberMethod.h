#ifndef _MEMBER_METHOD_H_
#define _MEMBER_METHOD_H_

// Project headers
#include "Method.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class encapsulates a instance's member method.  The method is stored as 
   * a pointer-to-member function.  This class is templated on the class to which
   * the member method belongs.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  template<class MC> class MemberMethod : public Method {

    // Constructor / Destructor
  public:
    
    /**
     * Constructor accepts a pointer-to-member function and a pointer to the
     * instance upon which the function will be invoked.
     * @param memberMethod - Method to be invoked
     * @param invocationInstance - Object upon which to invoke the method
     */
  MemberMethod(void (MC::*memberMethod)(void), MC* invocationInstance) :
    Method(),
      memberMethod(memberMethod),
      invocationInstance(invocationInstance) {}
    
    /**
     * Virtual destructor
     */
    virtual ~MemberMethod(void) {}
    
    // Public overrides from class Method
  public:
    
    /**
     * Invokes the encapsulated method
     */
    virtual void operator()(void) const {
      // Invoke the stored pointer to member
      (invocationInstance->*memberMethod)();
    }
    
    /**
     * Uses the stored pointer-to-member method and the invocation instance as
     * the basis for determining equality.
     * @param other - Other instance to compare to
     * @return - True if the instances are equivalent, false if not
     */
    virtual const bool equals(const Method& other) const {
      bool retValue = false;
      
      // Compare only to other member method instances
      const MemberMethod* otherMemberMethod = dynamic_cast<const MemberMethod*>(&other);
      if(otherMemberMethod != NULL) {
        retValue = ((memberMethod == otherMemberMethod->memberMethod) &&
                    (invocationInstance == otherMemberMethod->invocationInstance));
      }
      
      return(retValue);
    }
    
    // Private attributes
  private:
    
    /**
     * Pointer to the member method
     */
    void (MC::*memberMethod)(void);
    
    /**
     * Pointer to the invocation instance
     */
    MC* invocationInstance;
  };
}

#endif
