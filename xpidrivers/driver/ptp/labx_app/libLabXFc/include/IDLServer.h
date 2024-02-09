#ifndef _IDL_SERVER_H_
#define _IDL_SERVER_H_

// Library headers
#include "Nameable.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * Abstract class abstracting an IDL server, capable of receiving IDL 
   * messages, presenting them to the IDL Responder framework for processing,
   * and delivering responses to clients.
   *
   * Copyright (c) 2011 Lab X Technologies.  All rights reserved.
   * @author Eldridge M. Mount IV
   */
  class IDLServer : public virtual Nameable {

    // Constructor / Destructor
  public:

    /**
     * Default constructor
     */
    IDLServer(void) {}

    /**
     * Virtual destructor
     */
    virtual ~IDLServer(void) {}

    // Public interface methods
  public:

    /**
     * Shuts down the IDL server.  If any worker threads are employed by the
     * server instance, this method must stop them and guarantee that all have
     * exited before it may return.
     */
    virtual void shutdown(void) = 0;
  };

}

#endif
