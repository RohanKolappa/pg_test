#ifndef _DEBUG_H_
#define _DEBUG_H_

// System headers
#include <string>

// Project headers
#include "Semaphore.h"

// Macro definitions

#ifdef _LABXDEBUG

/**
 * Fixed-format debug macros
 */
#define DebugNote(message) LabX::Debug::debugMessage(LabX::Debug::NOTE, __FILE__, __LINE__, __PRETTY_FUNCTION__, (message))
#define DebugWarning(message) LabX::Debug::debugMessage(LabX::Debug::WARNING, __FILE__, __LINE__, __PRETTY_FUNCTION__, (message))
#define DebugError(message) LabX::Debug::debugMessage(LabX::Debug::ERROR, __FILE__, __LINE__, __PRETTY_FUNCTION__, (message))

/**
 * Variable-argument list debug macros
 */
#define DebugNoteVa(message, args...) LabX::Debug::debugMessage(LabX::Debug::NOTE, __FILE__, __LINE__, __PRETTY_FUNCTION__, (message), args)
#define DebugWarningVa(message, args...) LabX::Debug::debugMessage(LabX::Debug::WARNING, __FILE__, __LINE__, __PRETTY_FUNCTION__, (message), args)
#define DebugErrorVa(message, args...) LabX::Debug::debugMessage(LabX::Debug::ERROR, __FILE__, __LINE__, __PRETTY_FUNCTION__, (message), args)

#else

/**
 * Empty macro implementations for when debugging is disabled
 */
#define DebugNote(message) (void (NULL))
#define DebugWarning(message) (void (NULL))
#define DebugError(message) (void (NULL))
#define DebugNoteVa(message, args...) (void (NULL))
#define DebugWarningVa(message, args...) (void (NULL))
#define DebugErrorVa(message, args...) (void (NULL))

#endif

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * A fully-static class and compile-time macros for multi-level debugging output.
   *
   * This class provides a static wrapper interface to the stderr output stream.
   * Debug output can take on one of three severity levels:
   * NOTE    - For making a note for sake of debugging
   * WARNING - For warning about something potentially problematic
   * ERROR   - For announcing a "show-stopper"; this will kill the process
   *
   * Each class of debug messages can be selectively masked off, and output is
   * prefaced with the file name, function name, and line number.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class Debug {
    // Public type definitions
  public:
    
    /**
     * Enumerated type to specify debug severity levels
     */
    typedef enum
    {
      NOTE    = 0x01,
      WARNING = 0x02,
      ERROR   = 0x04
    } DebugLevel;

    // Public static interface
  public:
    
    /**
     * Sets the debug level mask, which determines which debugging "levels" are
     * capable of producing output.  Useful for controlling how verbose an execution
     * run will be.
     * @param debugMask - Debug level bit mask
     */
    static void setDebugMask(const int& debugMask);

    /**
     * Format the passed debug message to stderr.  This is typically never called
     * directly, but rather invoked by the debugging macros.
     * @param level - Debugging level of the message
     * @param filename - Name of the source file originating the message
     * @param line - Line within the originating file
     * @param function - Name of the originating function
     * @param message - Descriptive message, followed by a variable argument list
     */
    static void debugMessage(const DebugLevel& level,
                             const char* filename,
                             const int line,
                             const char* function,
                             const char* message,
                             ...);
  
    // Private static members
  private:
    
    /**
     * Mask to enable/disable levels
     */
    static int debugMask;
    
    /**
     * Semaphore to protect the stderr stream from reentrancy
     */
    static Semaphore streamSem;
  };
}

#endif
