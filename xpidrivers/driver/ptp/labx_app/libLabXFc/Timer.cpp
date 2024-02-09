// System headers
#include <list>
#include <sys/time.h>

// Project headers
#include "Debug.h"
#include "StaticMethod.h"
#include "Timer.h"

// Namespace using directives
using std::list;
using std::vector;

// Implementation of class Timer

// Static initialization

LabX::Timer* LabX::Timer::timerInstance = NULL;

// Destructor

LabX::Timer::~Timer(void) {}

// Protected overrides from class SignalSniffer

void LabX::Timer::preSniffConfig(void) {
  struct itimerval oldTimer;
  struct itimerval newTimer;

  // Schedule an alarm event to expire every 100 milliseconds; this is
  // ten times the granularity of the linux kernel tick.
  newTimer.it_value.tv_sec = 0;
  newTimer.it_value.tv_usec = (TIMER_RESOLUTION * 1000);
  newTimer.it_interval.tv_sec = 0;
  newTimer.it_interval.tv_usec = (TIMER_RESOLUTION * 1000);
  DebugNote("Calling setitimer");
  if(setitimer(ITIMER_REAL, &newTimer, &oldTimer) < 0) {
    DebugError("Unable to set interval timer!");
  }
}

void LabX::Timer::handleSignal(void) {
  list<TimerRecord> expiredTimers;

  // Obtain exclusive access to the timers
  timerSem.wait();

  // Iterate through the timers, finding expired timers and resetting / 
  // removing timer records
  for(vector<TimerRecord>::iterator timerIter = timers.begin();
      timerIter != timers.end();) {
    if(--(*timerIter).timeout == 0) {
      // The timer has expired, push on to the list to be executed
      expiredTimers.push_back(*timerIter);
        
      // Test for periodicity to see if the timer should be reset or
      // removed
      if((*timerIter).reload == 0) {
        timerIter = timers.erase(timerIter);
      } else {
        (*timerIter).timeout = (*timerIter).reload;
        ++timerIter;
      }
    } else {
      // Timer has not expired, move on to the next record
      ++timerIter;
    }
  }

  // Release exclusive access; executing callbacks may add new timers!
  timerSem.post();

  // Execute each expired timer callback
  while(expiredTimers.empty() == false) {
    (*expiredTimers.front().timerMethod)();
    if(expiredTimers.front().reload == 0) {
      delete(expiredTimers.front().timerMethod);
    }
    expiredTimers.pop_front();
  }
}

// Public singleton interface

LabX::Timer& LabX::Timer::instance(void) {
  // Create the singleton instance if it doesn't exist yet
  if(timerInstance == NULL) timerInstance = new Timer();

  // Return the singleton instance by reference
  return(*timerInstance);
}

void LabX::Timer::destroy(void) {
  // Destroy the singleton instance
  delete(timerInstance);
  timerInstance = NULL;
}

// Public timer interface

void LabX::Timer::addTimer(void (*timerFunc)(void),
                           const uint32_t& timeout,
                           const bool& periodic) {
  // Create and add a new method instance
  addTimer(new StaticMethod(timerFunc), timeout, periodic);
}

void LabX::Timer::addTimer(Method* timerMethod,
                           const uint32_t& timeout,
                           const bool& periodic) {
  // Ensure mutually exclusive access to the timers
  timerSem.wait();

  // Add a new timer record
  timers.push_back(TimerRecord(timerMethod, 
                               timeout, 
                               (periodic == true) ? timeout : 0));

  // Release mutually exclusive access
  timerSem.post();
}

void LabX::Timer::updateTimer(void (*timerFunc)(void),
                              const uint32_t& timeout,
                              const bool& periodic) {
  updateTimer(new StaticMethod(timerFunc), timeout, periodic);
}

void LabX::Timer::updateTimer(Method* timerMethod,
                              const uint32_t& timeout,
                              const bool& periodic) {
  // Ensure mutually exclusive access to the timers
  timerSem.wait();
  
  bool foundTimer = false;

  // Iterate through the timers, updating the first one that was registered with
  // an equivalent callback method.
  for(vector<TimerRecord>::iterator timerIter = timers.begin();
      timerIter != timers.end();
      ++timerIter) {
    // Compare this timer's callback method to the passed method
    if(timerIter->timerMethod->equals(*timerMethod) == true) {
      // Update the timer
      timerIter->timeout = timeout;
      timerIter->reload = ((periodic == true) ? timeout : 0);
      foundTimer = true;
      break;
    }
  }

  if(foundTimer == false) {
    timers.push_back(TimerRecord(timerMethod, 
                                 timeout, 
                                 (periodic == true) ? timeout : 0));
  } else {
    delete(timerMethod);
  }

  // Release mutually exclusive access
  timerSem.post();
}

void LabX::Timer::removeTimers(void (*timerFunc)(void)) {
  // Remove all timer callbacks registered with an equivalent static
  // method instance.
  removeTimers(StaticMethod(timerFunc));
}

void LabX::Timer::removeTimers(const Method& timerMethod) {
  // Ensure mutually exclusive access to the timers
  timerSem.wait();

  // Iterate through the timers, removing any that were registered with
  // an equivalent callback method.
  for(vector<TimerRecord>::iterator timerIter = timers.begin();
      timerIter != timers.end();) {
    // Compare this timer's callback method to the passed method
    if((*timerIter).timerMethod->equals(timerMethod) == true) {
      // Destroy the callback method and erase the timer
      delete(timerIter->timerMethod);
      timerIter = timers.erase(timerIter);
    } else {
      // No match, move to the next timer
      ++timerIter;
    }
  }

  // Release mutually exclusive access
  timerSem.post();
}

void LabX::Timer::clearAllTimers(void) {
  // Ensure mutually exclusive access to the timers
  timerSem.wait();

  // Delete all timer method instances
  while(timers.empty() == false) {
    delete(timers.back().timerMethod);
    timers.pop_back();
  }

  // Release mutually exclusive access
  timerSem.post();
}

// Private constructor

LabX::Timer::Timer(void) :
  SignalSniffer("Application Timer", SIGALRM),
  timers(),
  timerSem() {
  DebugNote("Timer initialized");
}

// Private helper methods

// Implementation of inner struct TimerRecord

LabX::Timer::TimerRecord::TimerRecord(Method* timerMethod,
                                      const uint32_t& timeout,
                                      const uint32_t& reload) :
  timerMethod(timerMethod),
  timeout(timeout),
  reload(reload) {}
