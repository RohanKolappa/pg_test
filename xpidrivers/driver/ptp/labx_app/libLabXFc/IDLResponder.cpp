// File : IDLResponder.cpp
// Author : Eldridge M. Mount IV
// Description : Class abstracting an Interface Description Language (IDL)
//               Responder interface.
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.

// STL headers
#include <algorithm>
#include <iostream>
#include <sstream>

// #define _LABXDEBUG

// Library headers
#include "Debug.h"
#include "IDLResponder.h"
#include "LabX.h"

// Namespace using directives
using LabX::IDLResponder;
using LabX::toHexString;
using std::cerr;
using std::endl;
using std::find;
using std::string;
using std::stringstream;
using std::vector;

// Static member initialization

LabX::Semaphore IDLResponder::registrySem;

vector<IDLResponder::ResponderEntry*> IDLResponder::responderRegistry;

vector<IDLResponder::ClientInterface*> IDLResponder::clientRegistry;

// Constructor / Destructor

IDLResponder::IDLResponder(const string &name) :
  Lockable(),
  name(name) {
}

IDLResponder::~IDLResponder(void) {
}

// Static application interface

void IDLResponder::registerResponder(uint16_t firstClassCode,
                                     uint16_t numClassCodes,
                                     IDLResponder &responder) {
  try {
    // Atomically register the instance
    registrySem.wait();

    // First iterate through the existing registrations looking for any
    // overlap of class codes, which is not permitted.
    uint32_t newRangeStart = static_cast<uint32_t>(firstClassCode);
    uint32_t newRangeEnd = (newRangeStart + static_cast<uint32_t>(numClassCodes) - 1);
    for(vector<ResponderEntry*>::const_iterator vecIter = responderRegistry.begin();
        vecIter != responderRegistry.end();
        ++vecIter) {
      // Test for range overlap
      if(((newRangeStart >= (*vecIter)->firstClassCode) & (newRangeStart <= (*vecIter)->lastClassCode)) |
         ((newRangeEnd >= (*vecIter)->firstClassCode) & (newRangeEnd <= (*vecIter)->lastClassCode))) {
        stringstream excStream;
        excStream << "IDL class code range ["
                  << newRangeStart
                  << ", "
                  << newRangeEnd
                  << "] for responder \""
                  << responder.getName()
                  << "\" overlaps with that of responder \""
                  << (*vecIter)->responder->getName()
                  << "\"";
        throw(new Exception(excStream.str()));
      }

      // Test for multiple-registration
      if((*vecIter)->responder->getName() == responder.getName()) {
        stringstream excStream;

        excStream << "IDL responder \""
                  << responder.getName()
                  << "\" is already registered";
        throw(new Exception(excStream.str()));
      }
    }

    // If we made it to here, then insertion is okay
    ResponderEntry *newEntry = new ResponderEntry();
    newEntry->firstClassCode = newRangeStart;
    newEntry->lastClassCode = newRangeEnd;
    newEntry->responder = &responder;
    responderRegistry.push_back(newEntry);

    // Ensure that any clients that have already registered are added
    // to the new responder.
    for(uint32_t clientIndex = 0; clientIndex < clientRegistry.size(); clientIndex++) {
      responder.addClient(clientRegistry[clientIndex]);
    }

    registrySem.post();
  } catch(Exception *exc) {
    // Unlock the registry before re-throwing
    registrySem.post();

    stringstream excStream;
    excStream << "Exception caught registering IDL responder \""
              << responder.getName()
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
}

void IDLResponder::unregisterResponder(IDLResponder &responder) {
  try {
    // Atomically unregister the instance
    registrySem.wait();

    // Iterate through the existing registrations looking for the instance.
    // If it isn't found, just quietly return.
    for(vector<ResponderEntry*>::iterator vecIter = responderRegistry.begin();
        vecIter != responderRegistry.end();
        ++vecIter) {
      if((*vecIter)->responder->getName() == responder.getName()) {
        // Destroy the registration and erase its entry from the vector
        delete(*vecIter);
        responderRegistry.erase(vecIter);
        break;
      }
    }

    // Also remove any clients we have added to the responder
    for(uint32_t clientIndex = 0; clientIndex < clientRegistry.size(); clientIndex++) {
      responder.removeClient(clientRegistry[clientIndex]);
    }

    registrySem.post();
  } catch(Exception *exc) {
    // Unlock the registry before re-throwing
    registrySem.post();

    stringstream excStream;
    excStream << "Exception caught unregistering IDL responder \""
              << responder.getName()
              << "\"";
    throw(new Exception(excStream.str(), exc));
  }
}

void IDLResponder::registerClient(ClientInterface *client) {
  bool multipleCall = false;

  // Guard against multiple registration
  registrySem.wait();

  vector<ClientInterface*>::iterator findIter = 
    find(clientRegistry.begin(), clientRegistry.end(), client);
  if(findIter == clientRegistry.end()) {
    // Add the client to the registry and inform all registered responders
    clientRegistry.push_back(client);

    for(vector<ResponderEntry*>::iterator vecIter = responderRegistry.begin();
        vecIter != responderRegistry.end();
        ++vecIter) {
      IDLResponder &responder(*(*vecIter)->responder);
      responder.lock();
      responder.addClient(client);
      responder.unlock();
    }
  } else multipleCall = true;

  registrySem.post();

  DebugNoteVa("Registered client \"%s\"", client->getName().c_str());

  if(multipleCall) {
    stringstream excStream;
    excStream << "IDLResponder::registerClient() : "
              << "multiple call for client from \""
              << client->getName()
              << "\"";
    throw(new Exception(excStream.str()));
  }
}

void IDLResponder::unregisterClient(ClientInterface *client) {
  registrySem.wait();

  // Silently ignore if the client is not registered to begin with
  vector<ClientInterface*>::iterator findIter = 
    find(clientRegistry.begin(), clientRegistry.end(), client);
  if(findIter != clientRegistry.end()) {
    clientRegistry.erase(findIter);

    for(vector<ResponderEntry*>::iterator vecIter = responderRegistry.begin();
        vecIter != responderRegistry.end();
        ++vecIter) {
      IDLResponder &responder(*(*vecIter)->responder);
      responder.lock();
      responder.removeClient(client);
      responder.unlock();
    }
  }
  registrySem.post();
}

void IDLResponder::processMessage(ClientInterface *client,
                                  IDLRequestMessageBuffer &request,
                                  IDLResponseMessageBuffer &response) {
  try {
    // Atomically search through the registry of responders, looking for one
    // which matches the class code of the request
    uint32_t firstClassCode = 0;
    IDLResponder *responder = NULL;
    registrySem.wait();
    for(vector<ResponderEntry*>::iterator vecIter = responderRegistry.begin();
        vecIter != responderRegistry.end();
        ++vecIter) {
      if((request.getClassCode() >= (*vecIter)->firstClassCode) &
         (request.getClassCode() <= (*vecIter)->lastClassCode)) {
        firstClassCode = (*vecIter)->firstClassCode;
        responder = (*vecIter)->responder;
      }
    }
    registrySem.post();

    // Throw an exception if no responder accepts the class code
    if(responder == NULL) {
      stringstream excStream;

      DebugWarningVa("Unable to locate IDLResponder for class code %d",
                     request.getClassCode());

      excStream << "Unable to locate IDLResponder for class code "
                << request.getClassCode();
      throw(new Exception(excStream.str()));
    }

    DebugNoteVa("Processing %d-byte request:", request.getLength());
#ifdef _LABXDEBUG
    for(uint32_t byteIndex = 0; byteIndex < request.getLength(); byteIndex++) {
      cerr << toHexString(request.get_uint8_t(byteIndex), 2, false) << " ";
      if((byteIndex % 16) == 15) cerr << endl;
    }
    cerr << endl;
#endif

    // Dispatch the message to the located responder instance.  Normalize the class
    // code by subtracting out the base class code assigned by the application.  This
    // permits each derived class to maintain its own insulated, zero-based enumeration
    // for class codes.  Client applications are responsible for mapping them along with
    // other class codes within the overall code space without causing intersections.
    uint32_t normalizedClassCode = (request.getClassCode() - firstClassCode);
    request.setClassCode(normalizedClassCode);

    // Atomically invoke the message dispatch upon the responder.  This helps to avoid
    // reentrancy through this interface; in addition, responders may interact asynchronously
    // with their respective systems.  If such asynchronous interfaces on the derived
    // class also lock, then race condition hazards are avoided.
    responder->lock();
    responder->dispatchMessage(client, request, response);
    responder->unlock();

    DebugNoteVa("Returning %d-byte response:", response.getLength());
#ifdef _LABXDEBUG
    for(uint32_t byteIndex = 0; byteIndex < response.getLength(); byteIndex++) {
      cerr << toHexString(response.get_uint8_t(byteIndex), 2, false) << " ";
      if((byteIndex % 16) == 15) cerr << endl;
    }
    cerr << endl;
#endif
  } catch(Exception *exc) {
    registrySem.post();

    throw(new Exception("Exception caught processing IDL message", exc));
  }
}
