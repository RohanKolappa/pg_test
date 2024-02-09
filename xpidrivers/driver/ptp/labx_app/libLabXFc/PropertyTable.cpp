// Project headers
#include "PropertyTable.h"

using std::map;
using std::string;
using std::vector;

// Constructor / Destructor

LabX::PropertyTable::PropertyTable(void) :
  properties() {
}

LabX::PropertyTable::PropertyTable(const string& property, const string& value) :
  properties() {
  // Add the single passed property
  setProperty(property, value);
}

LabX::PropertyTable::PropertyTable(const PropertyTable& other) :
  properties(other.properties) {
}

LabX::PropertyTable::~PropertyTable(void) {
}

// Public interface methods

const vector<string> LabX::PropertyTable::getPropertyKeys(void) const {
  vector<string> retValue;

  for(map<string, string>::const_iterator iter = properties.begin();
      iter != properties.end();
      ++iter) {
    retValue.push_back(iter->first);
  }

  return(retValue);
}

const bool LabX::PropertyTable::hasProperty(const string& property) const {
  return(properties.find(property) != properties.end());
}

const string LabX::PropertyTable::getProperty(const string& property) const {
  string retValue;
  map<string, string>::const_iterator iter = properties.find(property);

  if(iter != properties.end()) retValue = iter->second;

  return(retValue);
}

void LabX::PropertyTable::setProperty(const string& property, const string& value) {
  map<string, string>::iterator iter = properties.find(property);

  if(iter != properties.end()) {
    iter->second = value;
  } else {
    properties.insert(make_pair(property, value));
  }
}

void LabX::PropertyTable::clear(void) {
  // Clear all properties
  properties.clear();
}

const bool LabX::PropertyTable::isEmpty(void) const {
  return(properties.empty());
}

// Public overloaded operators

const LabX::PropertyTable& LabX::PropertyTable::operator=(const PropertyTable& rhs) {
  properties = rhs.properties;
  return(*this);
}
