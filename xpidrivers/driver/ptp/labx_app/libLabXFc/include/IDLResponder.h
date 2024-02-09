#ifndef _IDL_RESPONDER_H_
#define _IDL_RESPONDER_H_

// System headers
#include <stdint.h>
#include <string>
#include <vector>

// Library headers
#include "Lockable.h"
#include "Nameable.h"
#include "IDLMessageBuffer.h"
#include "Semaphore.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Class abstracting an Interface Description Language (IDL) Responder
   * interface.  Each responder has a unique range of class codes which are
   * assigned by the client application.  Responders register by way of
   * a "friendly" name, allowing other libraries to define responder 
   * interfaces without requiring knowledge of the overall application's
   * scheme for assigning IDL class codes.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class IDLResponder : public virtual Nameable, public Lockable {

    // Public type definitions
  public:

    /**
     * Interface class used by clients of the IDLResponder mechanism.
     * The class makes use of this abstraction as a way to keep track
     * of persistent per-client data structures, deliver events, etc.
     */
    class ClientInterface : public virtual Nameable {

      // Constructor / Destructor
    public:

      /**
       * Default constructor
       */
      ClientInterface(void) : Nameable() {}

      /**
       * Virtual destructor
       */
      virtual ~ClientInterface(void) {}

    };

    // Constructor / Destructor
  public:

    /**
     * Creates a responder instance
     * @param name - Name for the instance
     */
    explicit IDLResponder(const std::string &name);

    /**
     * Virtual destructor
     */
    virtual ~IDLResponder(void);

    // Public overrides from class Nameable
  public:

    /**
     * Returns the name of the instance
     */
    virtual const std::string& getName(void) const { return(name); }

    // Static application interface
  public:

    /**
     * Registers the passed instance with the class, for the specified
     * range of class codes.  An exception is thrown if the requested
     * range overlaps any existing registrations.
     * @param firstClassCode - First class code recognized for the instance
     * @param numClassCodes  - Total number of contiguous class codes
     * @param responder      - A reference to the responder instance
     */
    static void registerResponder(uint16_t firstClassCode,
                                  uint16_t numClassCodes,
                                  IDLResponder &responder);

    /**
     * Unregisters the passed instance with the class, such that it no longer
     * receives dispatched messages after the call.
     */
    static void unregisterResponder(IDLResponder &responder);

    /**
     * Registers the passed client of the IDLResponder class mechanisms.
     * Clients must register before they may process messages or receive 
     * asynchronous events.
     *
     * All responders must be registered before any clients are registered,
     * or some responders may not recognize clients due to the race condition.
     * @param client - Client instance to register
     */
    static void registerClient(ClientInterface *client);

    /**
     * Unregisters the passed client from the class
     * @param client - Client instance to unregister
     */
    static void unregisterClient(ClientInterface *client);

    /**
     * Processes a message for the the application as an entity.  The
     * class code of the message is used to dispatch it to the appropriate
     * responder instance, if one exists.  If no responder has registered
     * and been associated with the class code, an exception is thrown.
     *
     * When the located responder instance is invoked to process the
     * message, it receives a "normalized" class code which is zero-based.
     * The "firstClassCode" value which the application registered the instance
     * with is first subtracted from the actual received class code.
     *
     * This method is safe from the perspective of re-entrancy; if multiple
     * threads make calls to this, they are serialized, with each invocation
     * blocking its calling thread until the message is processed and a
     * responder-defined response generated.
     *
     * @param client   - The client interface making the request
     * @param request  - The IDL request message buffer
     * @param response - The IDL response message buffer
     */
    static void processMessage(ClientInterface *client,
                               IDLRequestMessageBuffer &request,
                               IDLResponseMessageBuffer &response);

    // Protected interface methods
  protected:

    /**
     * Adds a client to the instance, initializing any per-client state or
     * data necessary for the derived class
     * This is invoked with the instance already locked to ensure mutually-exclusive
     * access.
     * @param client - The client to be added
     */
    virtual void addClient(ClientInterface *client) = 0;

    /**
     * Removes a client to the instance, cleaning up any per-client state or
     * data necessary for the derived class
     * This is invoked with the instance already locked to ensure mutually-exclusive
     * access.
     * @param client - The client to be removed
     */
    virtual void removeClient(ClientInterface *client) = 0;

    /**
     * Pure virtual method for processing an IDL message intended for the instance.
     * This is invoked with the instance already locked to ensure mutually-exclusive
     * access.
     * @param client   - The client making the request
     * @param request  - The IDL request message buffer
     * @param response - The IDL response message buffer
     */
    virtual void dispatchMessage(ClientInterface *client,
                                 const IDLRequestMessageBuffer &request,
                                 IDLResponseMessageBuffer &response) = 0;

    // Private type definitions
  private:

    /**
     * Structure to encapsulate all the details of a registered subclass
     * instance for dispatching to
     */
    typedef struct {
      uint32_t firstClassCode;
      uint32_t lastClassCode;
      IDLResponder *responder;
    } ResponderEntry;

    // Private attributes
  private:

    /**
     * Name for the instance
     */
    const std::string name;

    /**
     * Mutex semaphore to lock class resources
     */
    static Semaphore registrySem;

    /**
     * Vector of registered responders and their metadata
     */
    static std::vector<ResponderEntry*> responderRegistry;

    /**
     * Vector of registered clients
     */
    static std::vector<ClientInterface*> clientRegistry;
  };
}

#endif
