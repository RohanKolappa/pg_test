// Project headers
#include "Debug.h"
#include "EventHandler.h"
#include "EventNexus.h"


using std::map;
using std::list;
using std::string;

// Implementation of class EventHandler

// Constructor / Destructor

LabX::EventHandler::EventHandler(void) :
  eventHooks() {
}

LabX::EventHandler::~EventHandler(void) {
  // Destroy the event hook methods
  for(map<string, list<EventHook*> >::iterator mapIter = eventHooks.begin();
      mapIter != eventHooks.end();
      ++mapIter) {
    list<EventHook*>& hookList(mapIter->second);
    for(list<EventHook*>::iterator listIter = hookList.begin();
        listIter != hookList.end();
        ++listIter) {
      delete(*listIter);
    }
  }
}

// Public interface methods

const bool LabX::EventHandler::dispatch(const Event& event)
{
  bool retValue(false);

  // Search for a registered hook for this type of event
  map<string, list<EventHook*> >::iterator mapIter = eventHooks.find(event.getClassName());
  if(mapIter != eventHooks.end()) {
    list<EventHook*>& hookList(mapIter->second);
    
    // Invoke the type-specific event hooks front-to-back, saving the return values.
    // Cease invoking if any handler returns true to absorb the event.
    for(list<EventHook*>::iterator listIter = hookList.begin();
        listIter != hookList.end();
        ++listIter) {
      retValue |= (*(*listIter))(event);
      if(retValue == true) break;
    }
  } else {
    // No type-specific hooks found, try the generic one, saving its return value.
    retValue = handle(event);
  }

  return(retValue);
}

// Protected interface methods

const bool LabX::EventHandler::handle(const Event& event) {
  // Default implementation does nothing and returns false
  return(false);
}

