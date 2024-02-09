#ifndef _XML_HANDLER_H_
#define _XML_HANDLER_H_

// System headers
#include <map>
#include <sstream>
#include <string>

// Project headers
#include "Exception.h"
#include "PropertyTable.h"

// Forward declarations
class Method;

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an interface of callbacks to respond to parsed XML
   * events.  Empty default implementations are provided for convenience.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class XmlHandler {
    // Constructor / Destructor
  public:
    
    /**
     * Default constructor
     */
    XmlHandler(void);

    /**
     * Virtual destructor
     */
    virtual ~XmlHandler(void);

    // Public interface methods
  public:

    /**
     * Receive notification of the beginning of the document
     */
    virtual void startDocument(void);

    /**
     * Receive notification of the beginning of an element.  The qualified tag
     * name is passed in element, and any attributes are passed in atts.
     *
     * The default implementation implements an "element handler" pattern, which
     * searches for a methods which is registered for the parsed element, invoking
     * it if found.  If no explicit handler is found, nothing is done.
     * @param element - Name of the lexical element
     * @param atts    - Table of key-value pairs for the element
     */
    virtual void startElement(const std::string& element, const PropertyTable& atts);

    /**
     * Receive notification of character data; raw character data forming element
     * tags are not reported via this method!
     * @param chars - Raw characters
     */
    virtual void characters(const std::string& chars);

    /**
     * Receive notification of the end of an element.
     * @param element - Name of the lexical element
     */
    virtual void endElement(const std::string& element);

    /**
     * Receive notification of the end of the document
     */
    virtual void endDocument(void);

    /**
     * Receive notification of parsing of an unregistered element name
     * @param element - Name of the lexical element
     * @param atts    - Table of key-value pairs for the element
     */
    virtual void unregisteredElement(const std::string& element, const PropertyTable& atts);

    /**
     * Receive notification of a recoverable error
     * @param message - Message describing the error
     */
    virtual void error(const std::string& message);

    /**
     * Receive notificaton of a non-recoverable error
     * @param message - Message describing the error
     */
    virtual void fatalError(const std::string& message);

    // Private inner classes
  private:

    /**
     * Inner class to encapsulate an element handling method
     */
    class ElementHandler {
    public:
      ElementHandler(void) {};
      virtual ~ElementHandler(void) {};
      virtual void operator()(bool tagOpen, const PropertyTable &atts) const = 0;
    };

    /**
     * Inner class to encapsulate an element handling method which is a member
     * method of a derived class
     */
    template<class MC> class MemberElementHandler : public ElementHandler {
    public:
    MemberElementHandler(void (MC::*elementMethod)(bool tagOpen, 
                                                   const PropertyTable &atts), 
                         MC* invocationInstance) :
      ElementHandler(),
        elementMethod(elementMethod),
        invocationInstance(invocationInstance) {}

      virtual void operator()(bool tagOpen, const PropertyTable &atts) const {
        (invocationInstance->*elementMethod)(tagOpen, atts);
      }

    private:
      void (MC::*elementMethod)(bool, const PropertyTable &);
      MC* invocationInstance;
    };

    // Protected interface methods
  protected:

    /**
     * Registers the passed member method as the handler for the passed element.
     * If a handler was previously registered for the element, it is replaced.
     */
    template<class MC> 
      void registerElement(const std::string &element,
                           void (MC::*elementMethod)(bool, const PropertyTable&),
                           MC *invocationInstance) {
      // Map the specified element to the hook method
      ElementHandler *handler = new MemberElementHandler<MC>(elementMethod, invocationInstance);
      std::map<std::string, ElementHandler*>::iterator mapIter = elementHandlers.find(element);
      if(mapIter == elementHandlers.end()) {
        // Insert the new key->value pair
        elementHandlers.insert(make_pair(element, handler));
      } else {
        // Replace the element handler, deleting the old one
        delete(mapIter->second);
        mapIter->second = handler;
      }
    }

    // Private attributes
  private:

    /**
     * Map of XML elements to their registered handler methods
     */
    std::map<std::string, ElementHandler*> elementHandlers;
  };
}

#endif
