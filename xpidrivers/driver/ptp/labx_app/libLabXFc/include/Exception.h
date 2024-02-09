#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

// System headers
#include <string>

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Simple exception class for deriving others from.  If all that is required is
   * a text message, then throw this as a concretion.
   */
  class Exception {

    // Constructor / Destructor
  public:

    /**
     * Creates an instance with the passed message
     * @param message - Message to be encapsulated
     */
    explicit Exception(const std::string& message);
    
    /**
     * Creates an instance with the passed message and cause
     * @param message - Message to be encapsulated
     * @param cause - Pointer to the preceding exception
     */
    Exception(const std::string& message, const Exception *cause);
    
    /**
     * Destroys the instance and all of its causes
     */
    virtual ~Exception(void);
    
    // Public interface methods
  public:
    
    /**
     * Prints the message, recursing through the causes
     * @return - The encapsulated messages from each exception in the causal chain
     */
    virtual std::string toString(void) const;
    
    /**
     * Returns the name of the class
     * @return - The name of the exception class
     */
    virtual const std::string& className(void) const;
    
    // Private attributes
  private:
    
    /**
     * The encapsulated message
     */
    const std::string message;

    /**
     * Cause of the exception
     */
    const Exception *cause;

    /**
     * Static class name
     */
    static const std::string CLASS_NAME;
  };
}

#endif
