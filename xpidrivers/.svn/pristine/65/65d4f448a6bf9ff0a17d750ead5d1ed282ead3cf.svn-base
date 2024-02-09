#ifndef _IDL_CLIENT_H_
#define _IDL_CLIENT_H_

// Library headers
#include "IDLMessageBuffer.h"
#include "Lockable.h"
#include "Nameable.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Abstract class abstracting an IDL client, permitting IDL messages and 
   * responses to be exchanged in an abstract manner.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class IDLClient : public virtual Nameable, public Lockable {

    // Constructor / Destructor
  public:

    /**
     * Default constructor
     */
    IDLClient(void) : Lockable() {}

    /**
     * Virtual destructor
     */
    virtual ~IDLClient(void) {}

    // Public interface methods
  public:

    /**
     * Sends the passed request message buffer to the Responder, returning
     * a response buffer after completion.
     * @param request  - Request buffer to the Responder
     * @param response - Response buffer from the Responder
     */
    virtual void sendMessage(const IDLRequestMessageBuffer &request, 
                             IDLResponseMessageBuffer &response) = 0;

    /**
     * Polymorphically shuts down the client, severing the connection to
     * the remote Responder
     */
    virtual void shutdown(void) = 0;
  };

}

#endif
