// File        : PtpDomain.cpp
// Author      : Eldridge M. Mount IV
// Description : Event class for the Lab X PTP hardware.  This provides an abstraction 
//               through which the PTP management object can inform interested observers 
//               of events.
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.
// @author Eldridge M. Mount IV

// STL headers
#include <sstream>

// Library headers
#include "PtpEvent.h"

// Namespace using directives
using LabX::Event;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

// Class PtpEvent

// Static member initialization

const string Avb::PtpEvent::className("PtpDomain::PtpEvent");

// Constructor / Destructor

Avb::PtpEvent::PtpEvent(Avb::PtpEvent::EventType eventType,
                        const vector<string> &pairStrings) :
  Event(),
  eventType(eventType),
  properties() {
  // Populate the properties by parsing the key / value pairs
  for(vector<string>::const_iterator pairIter = pairStrings.begin();
      pairIter != pairStrings.end();
      ++pairIter) {
    const string &pairString = *pairIter;
    string::size_type pos = pairString.find_first_of(":");
    if(pos != string::npos) {
      string property(pairString.substr(0, pos++));
      string value(pairString.substr(pos, (pairString.length() - pos)));
      properties.setProperty(property, value);
    }
  }
}

Avb::PtpEvent::PtpEvent(const Avb::PtpEvent &other) :
  Event(other),
  eventType(other.eventType),
  properties(other.properties) {
}

Avb::PtpEvent::~PtpEvent(void) {
}

const string& Avb::PtpEvent::getClassName(void) const {
  return(className);
}

const string Avb::PtpEvent::toString(void) const {
  stringstream returnStream;

  returnStream << "PtpEvent: [";
  switch(eventType) {
  case PTP_EVENT_HEARTBEAT:
    returnStream << "Heartbeat";
    break;

  case PTP_EVENT_GM_CHANGE:
    returnStream << "GrandmasterChange";
    break;

  case PTP_EVENT_RTC_LOCK:
    returnStream << "RtcLock";
    break;

  case PTP_EVENT_RTC_UNLOCK:
    returnStream << "RtcUnlock";
    break;

  default:
    returnStream << "UNKNOWN";
  }
  returnStream << "]" << endl;

  // Print the event's properties, if it has any
  vector<string> propertyKeys = properties.getPropertyKeys();
  if(!propertyKeys.empty()) {
    returnStream << "Event properties:" << endl;
  }
  for(vector<string>::const_iterator propertyIter = propertyKeys.begin();
      propertyIter != propertyKeys.end();
      ++propertyIter) {
    returnStream << *propertyIter 
                 << " = " 
                 << properties.getProperty(*propertyIter) 
                 << endl;
  }
  return(returnStream.str());
}

Avb::PtpEvent::EventType Avb::PtpEvent::getEventType(void) const {
  return(eventType);
}

Event* Avb::PtpEvent::clone(void) const {
  return(new PtpEvent(*this));
}
