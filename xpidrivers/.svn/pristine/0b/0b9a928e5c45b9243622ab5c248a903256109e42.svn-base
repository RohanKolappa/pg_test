//
// Copyright 2003 Lab X Technologies, LLC

// System headers
#include <algorithm>

// Project headers
#include "Debug.h"
#include "EventNexus.h"

// Namespace using directives
using std::find;
using std::vector;

// Implementation of class EventNexus

// Static initialization

LabX::EventNexus* LabX::EventNexus::eventNexusInstance = NULL;

// Destructor

LabX::EventNexus::~EventNexus(void) {
}

// Public singleton interface

LabX::EventNexus& LabX::EventNexus::instance(void) {
  // Create the singleton instance if it doesn't exist yet
  if(eventNexusInstance == NULL) {
    eventNexusInstance = new EventNexus();
  }

  // Return the singleton instance by reference
  return(*eventNexusInstance);
}

void LabX::EventNexus::destroy(void) {
  // Destroy the singleton instance
  delete(eventNexusInstance);
  eventNexusInstance = NULL;
}

// Public interface methods

void LabX::EventNexus::registerHandler(EventHandler* handler) {
  // Atomically push the handler onto the vector of handlers
  lock();
  if(find(handlers.begin(), handlers.end(), handler) == handlers.end()) {
    handlers.push_back(handler);
  }
  unlock();
}

void LabX::EventNexus::dispatch(Event* event) {
  if(event != NULL)
    {
      // Obtain mutually exclusive access
      lock();
      
      // Increment the event counter
      eventCounter++;
      
      // Potentially generate a debugging message
      if(debugEvents == true) {
        DebugNoteVa("Event[%u]:\n%s\n", eventCounter, event->toString().c_str());
      }
      
      // Dispatch the event to the event handlers.
      for(vector<EventHandler*>::iterator vecIter = handlers.begin();
          vecIter != handlers.end();
          ++vecIter) {
        // Ignore the return value
        (*vecIter)->dispatch(*event);
      }

      // Release exclusive access
      unlock();
      
      // Destroy the event instance now that it has been dispatched
      delete(event);
    }
}

void LabX::EventNexus::setDebugEvents(const bool& debugEvents) {
  // Set the debugging flag atomically
  lock();
  LabX::EventNexus::debugEvents = debugEvents;
  unlock();
}

// Private constructor

LabX::EventNexus::EventNexus(void) :
  Lockable(),
  debugEvents(false),
  handlers(),
  eventCounter(0) {
}

