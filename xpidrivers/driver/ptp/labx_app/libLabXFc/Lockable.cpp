// STL headers
#include <iostream>

// Project headers
#include "Lockable.h"

// Namespace using directives
using std::cerr;
using std::endl;

// Implementation of class Lockable

// Destructor

LabX::Lockable::~Lockable(void) {
}

// Public interface methods

void LabX::Lockable::lock(void) const {
  if(debugLocks) cerr << "lock()" << endl;
  lockSem.wait();
}

void LabX::Lockable::unlock(void) const {
  lockSem.post();
  if(debugLocks) cerr << "unlock()" << endl;
}

// Protected constructor

LabX::Lockable::Lockable(const bool debugLocks) :
  debugLocks(debugLocks),
  lockSem() {
}

