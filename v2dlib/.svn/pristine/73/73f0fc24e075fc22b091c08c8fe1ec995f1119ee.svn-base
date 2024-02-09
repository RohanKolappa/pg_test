#ifndef __HH_V2DCOMMON_H__
#define __HH_V2DCOMMON_H__

#include "v2dlib_compatibility.h"

// #define DEBUG 1
// #define LOCAL_PRINT 1

// Pushkar Pradhan: Check for memory leaks
#ifdef WINDOWS

#ifdef DO_MEMLEAK_CHECK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)

#ifdef _DEBUG

#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif
#endif
#endif


#ifdef WINDOWS

extern int windows_gettimeofday(struct timeval* tp, int* tz);
#define GETTIMEOFDAY windows_gettimeofday

#else

#define GETTIMEOFDAY(timevalp, tz) gettimeofday((timevalp), (tz))

#endif

extern int _v2d_debug_level;
extern void (*v2dLibLoggerCallback)(int logLevel, char * logMessage);

#define GETTIMEDIFF(tNow, tPrev, tDelta) \
{ \
	if( (tNow.tv_sec - tPrev.tv_sec) < 0) \
	{ \
		tDelta = 0; \
	} \
	else if( ((tNow.tv_sec - tPrev.tv_sec) == 0) && ((tNow.tv_usec - tPrev.tv_usec) <= 0) ) \
	{ \
		tDelta = 0; \
	} \
	else \
	{ \
		tDelta = ((tNow.tv_sec - tPrev.tv_sec) * 1000) + \
			((tNow.tv_usec - tPrev.tv_usec) / 1000); \
	} \
}

#define TRUE            1
#define FALSE           0


// Some recent changes have caused the DEBUG symbol to be defined in Windows
// builds. Thus added the !defined(WINDOWS) guard below until we figure out.
#if defined(DEBUG) && (!defined(WINDOWS) || defined(__MINGW32__))


#if defined(STREAMING_SERVER) && !defined(LOCAL_PRINT)
#define LOG_SYS(DEBUGLEVEL, ...) syslog(LOG_LOCAL5 | LOG_DEBUG, __VA_ARGS__)
#define V2DASSERT(expr) \
        if(! (expr)) \
        { \
                syslog(LOG_LOCAL5 | LOG_DEBUG, "Assertion \"%s\" failed in file %s at line %d in function %s\n", \
                        #expr, __FILE__, __LINE__, __FUNCTION__); \
                abort(); \
        }
#else
#define LOG_SYS(DEBUGLEVEL, ...) printf(__VA_ARGS__)
#define V2DASSERT assert
#endif //STREAMING_SERVER

#elif _DEBUG // WINDOWS DEBUG mode

#define LOG_SYS(DEBUGLEVEL, ...) \
	{\
	if(_v2d_debug_level >= DEBUGLEVEL) \
		_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
	}
#define V2DASSERT(expr) _ASSERT(expr)

#else  // not DEBUG

#define LOG_SYS(DEBUGLEVEL, ...) \
    do { \
       if (v2dLibLoggerCallback != NULL) { \
           char log_string[128]; \
           snprintf(log_string, 127, __VA_ARGS__); \
           v2dLibLoggerCallback(DEBUGLEVEL, log_string); \
       } \
    } while (0)

#define V2DASSERT(expr) do {} while (0)

#endif // DEBUG


#endif // __HH_V2DCOMMON_H__
