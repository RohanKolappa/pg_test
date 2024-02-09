#ifndef _MEMBER_METHOD_1PARAM_H_
#define _MEMBER_METHOD_1PARAM_H_

// Project headers
#include "Method.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class encapsulates a instance's member method.  The method is stored as a
   * pointer-to-member function.  This class is templated on the class to which the
   * member method belongs, as well as on the class of a single parameter which is
   * passed upon invocation.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  template<class MC, class PC> class MemberMethod1Param : public Method {

    // Constructor / Destructor
  public:

    /**
     * Constructor accepts a pointer-to-member function, a pointer to the
     * instance upon which the function will be invoked, and a parameter value.
     * @param memberMethod - Method to be invoked
     * @param invocationInstance - Object upon which to invoke the method
     */
  MemberMethod1Param(void (MC::*memberMethod)(PC),
                     PC parameter,
                     MC* invocationInstance) :
    Method(),
      memberMethod(memberMethod),
      parameter(parameter),
      invocationInstance(invocationInstance) {}

    /**
     * Virtual destructor
     */
    virtual ~MemberMethod1Param(void) {}
  
    // Public overrides from class Method
  public:
    
    /**
     * Invokes the stored function pointer
     */
    virtual void operator()(void) const {
      // Invoke the stored pointer to member, passing the stored parameter
      (invocationInstance->*memberMethod)(parameter);
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
      const MemberMethod1Param* otherMemberMethod = 
        dynamic_cast<const MemberMethod1Param*>(&other);
      if(otherMemberMethod != NULL) {
        retValue = ((memberMethod == otherMemberMethod->memberMethod) &&
                    (invocationInstance == otherMemberMethod->invocationInstance) &&
                    (parameter == otherMemberMethod->parameter));
      }

      return(retValue);
    }

  // Private attributes
 private:

    /**
     * Pointer to the member method
     */
    void (MC::*memberMethod)(PC);

    /**
     * Parameter to be passed upon invocation
     */
    PC parameter;

    /**
     * Pointer to the invocation instance
     */
    MC* invocationInstance;
  };
}

#endif
