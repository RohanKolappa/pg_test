//
// Copyright 2003 Lab X Technologies, LLC

// Project headers
#include "Debug.h"
#include "XmlHandler.h"

using std::map;
using std::string;

// Implementation of class XmlHandler

// Constructor / Destructor

LabX::XmlHandler::XmlHandler(void) :
  elementHandlers() {
}

LabX::XmlHandler::~XmlHandler(void) {
}

// Public interface methods

void LabX::XmlHandler::startDocument(void) {
}

void LabX::XmlHandler::startElement(const string& element, const PropertyTable& atts) {
  // Search for an element handler for the name
  map<string, ElementHandler*>::iterator mapIter = elementHandlers.find(element);
  if(mapIter != elementHandlers.end()) {
    DebugNoteVa("Invoking handler for element \"%s\"", element.c_str());

    (*mapIter->second)(true, atts);
  } else {
    unregisteredElement(element, atts);
  }
}

void LabX::XmlHandler::characters(const string& chars) {
}

void LabX::XmlHandler::endElement(const string& element) {
  // Search for an element handler for the name
  map<string, ElementHandler*>::iterator mapIter = elementHandlers.find(element);
  if(mapIter != elementHandlers.end()) {
    (*mapIter->second)(false, PropertyTable());
  }
}

void LabX::XmlHandler::unregisteredElement(const string& element, const PropertyTable& atts) {
  DebugNoteVa("No handler found for element %s\n", element.c_str());
}

void LabX::XmlHandler::endDocument(void) {
}

void LabX::XmlHandler::error(const string& message) {
}

void LabX::XmlHandler::fatalError(const string& message) {
}
