//
// Copyright 2003 Lab X Technologies, LLC

// System headers
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// Project headers
#include "Debug.h"
#include "LabX.h"

// Namespace using directives
using LabX::stripFilename;

// Macro definitions
#define FORMAT_BUF_SIZE 2048

// Implementation of class Debug

// Static member initialization

// All debug levels enabled by default
int LabX::Debug::debugMask = (Debug::NOTE | Debug::WARNING | Debug::ERROR);

// Stream semaphore is a mutex
LabX::Semaphore LabX::Debug::streamSem;

// Public static interface

void LabX::Debug::setDebugMask(const int& debugMask) {
  Debug::debugMask = debugMask;
}

void LabX::Debug::debugMessage(const DebugLevel& level,
                               const char* filename,
                               const int line,
                               const char* function,
                               const char* message,
                               ...) {
  static char formattedMessage[FORMAT_BUF_SIZE];

  // Obtain mutually exclusive access to the buffer and stream
  streamSem.wait();

  if((level & debugMask) != 0) {
    va_list varArgs;
    const char* severityString = "?SEVERITY?";
    bool fatal = false;
    
    // Obtain the variable argument list
    va_start(varArgs, message);
    
    // Construct a formatted string from the passed message and variable
    // argument list, truncating if necessary
    vsnprintf(formattedMessage, FORMAT_BUF_SIZE, message, varArgs);
    
    // Stick a null terminator at the end just in case of truncation
    formattedMessage[FORMAT_BUF_SIZE - 1] = '\0';
    
    // Deallocate the variable argument list
    va_end(varArgs);
    
    // Determine what to print for severity & fatality
    switch(level) {
	case NOTE: {
      severityString = "NOTE";
      break;
    }
	case WARNING: {
      severityString = "WARNING";
      break;
    }
	case ERROR: {
      severityString = "ERROR";
      fatal = true;
      break;
    }
	default: {
      break;
    }
	}
      
    // Write to stderr
    fprintf(stderr, "%s:%d:%s:%s: %s\n", stripFilename(filename), line, function,
            severityString, formattedMessage);

    // Hard exit with an error if fatal
    if(fatal == true) {
	  exit(-1);
	}
  }
      
  // Release mutually exclusive access
  streamSem.post();
}
