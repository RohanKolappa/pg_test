// File : CleanupTarget.cpp
// Author : Eldridge M. Mount IV
// Description : This utility class abstracts an object which will 
//               notify registered Observers which need to know when
//               the object is about to be destroyed (e.g. to remove
//               any pointers or handles to the object).
//
// Copyright (c) 2011, Lab X Technologies, LLC
// All rights reserved

// STL headers
#include <algorithm>

// Library headers
#include "CleanupTarget.h"

// Namespace using directives
using LabX::CleanupTarget;
using std::find;
using std::list;

// Implementation of class CleanupTarget

// Constructor / destructor

CleanupTarget::CleanupTarget(void) :
  Lockable(),
  observers() {
}

CleanupTarget::~CleanupTarget(void) {
}

// Public interface methods

void CleanupTarget::registerObserver(Observer *observer) const {
  // Guard against multiple registration
  lock();
  list<Observer*>::const_iterator findIter = 
    find(observers.begin(), observers.end(), observer);
  if(findIter == observers.end()) observers.push_back(observer);
  unlock();
}

void CleanupTarget::unregisterObserver(Observer *observer) const {
  lock();
  list<Observer*>::iterator findIter = 
    find(observers.begin(), observers.end(), observer);
  if(findIter != observers.end()) observers.erase(findIter);
  unlock();
}

void CleanupTarget::cleanup(void) {
  // Make a copy of the collection with the instance locked; some Observers
  // may respond to notification by attempting to unregister.
  lock();
  list<Observer*> observersCopy(observers);
  unlock();

  // Notify all Observers in the order in which they registered
  for(list<Observer*>::iterator clientIter = observersCopy.begin();
      clientIter != observersCopy.end();
      ++clientIter) {
    (*clientIter)->instanceCleanup(this);
  }
}

