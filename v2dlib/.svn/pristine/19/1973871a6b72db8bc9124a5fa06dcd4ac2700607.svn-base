/*
 * v2dlib_compatibility.h
 *
 *  Created on: Nov 18, 2010
 *      Author: rkale
 */

#ifndef V2DLIB_COMPATIBILITY_H_
#define V2DLIB_COMPATIBILITY_H_

#ifdef V2DLIBRARY_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#if (defined(__WIN32__) || defined(_WIN32))
#define WINDOWS
#else
#ifdef WINDOWS
#undef WINDOWS
#endif
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef WINDOWS

#ifdef __MINGW32__

#include <inttypes.h>
#include <getopt.h>
#include <sys/time.h>

#define _CrtSetReportMode(paramX, paramY) do {} while (0)
#define srandom(param) srand((param))
#define random() rand()

#define STRCASECMP(X, Y) strcasecmp((X), (Y))
#define RINT(A) rint((A))

#else
#include <crtdbg.h>

typedef  __int8  int8_t;
typedef unsigned __int8  uint8_t;
typedef  __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32  int32_t;
typedef unsigned __int32  uint32_t;
typedef __int64  int64_t;
typedef unsigned __int64  uint64_t;

#define STRCASECMP(X, Y) stricmp((X), (Y))
#define RINT(A) floor((A)+(((A) < 0) ? -0.5 : 0.5))

#if _MSC_VER < 1700
#define inline __inline
#endif

#ifndef snprintf
#define snprintf _snprintf
#endif

#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <sys/timeb.h>
#include <io.h>

#else  // For linux

#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <sys/types.h>

#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define STRCASECMP(X, Y) strcasecmp((X), (Y))
#define RINT(A) rint((A))

#endif // WINDOWS



#endif /* V2DLIB_COMPATIBILITY_H_ */
