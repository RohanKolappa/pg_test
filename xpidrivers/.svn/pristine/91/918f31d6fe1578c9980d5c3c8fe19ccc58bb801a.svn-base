// System headers
#include <sstream>

// Library headers
#include "Exception.h"

// Namespace using directives
using std::endl;
using std::string;
using std::stringstream;

// Class name
const string LabX::Exception::CLASS_NAME = "Exception";

// Constructor / Destructor

LabX::Exception::Exception(const string& message) :
  message(message),
  cause(NULL) {
}

LabX::Exception::Exception(const string& message, const Exception *cause) :
  message(message),
  cause(cause) {
}

LabX::Exception::~Exception(void) {
  // Delete the cause, if there is one - this will recursively delete
  // all the exceptions chained
  delete(cause);
}

// Public interface methods

string LabX::Exception::toString(void) const {
  stringstream str;
  str << className() << ": " << message << endl;
  if(cause != NULL) {
    str << "Caused by:" << endl
        << cause->toString();
  }
  return(str.str());
}

const string& LabX::Exception::className(void) const {
  return(CLASS_NAME);
}
