#ifndef _PORTABILITY_H_
#define _PORTABILITY_H_


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

#define srandom(param) srand((param))
#define random() rand()

#endif

#ifndef WINVER
#define WINVER  0x501
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <sys/timeb.h>

#else  // For linux

#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/types.h>

#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#endif // WINDOWS



#ifdef LONG64
typedef unsigned long CARD64;
typedef unsigned int CARD32;
#else
typedef unsigned long CARD32;
#endif
typedef unsigned short CARD16;
typedef unsigned char  CARD8;

typedef char Bool;

#define True  1
#define False 0

#ifdef  DEBUG
#ifdef STREAMING_SERVER
#define LOG_RFB(...) syslog(LOG_LOCAL5 | LOG_DEBUG, __VA_ARGS__)
#define LOG_RFB_TRACE(...) do {} while (0)
#define RFBASSERT(expr) \
        if(! (expr)) \
        { \
                syslog(LOG_LOCAL5 | LOG_DEBUG, "Assertion \"%s\" failed in file %s at line %d in function %s\n", \
                        #expr, __FILE__, __LINE__, __FUNCTION__); \
                abort(); \
        }
#else
#define LOG_RFB printf
#define LOG_RFB_TRACE(...) do {} while (0)
#define RFBASSERT assert
#endif
#else  // not DEBUG
#ifndef LOG_RFB
#define LOG_RFB(...) do {} while (0)
#endif
#define LOG_RFB_TRACE(...) do {} while (0)
#define RFBASSERT(expr) do {} while (0)
#endif


#endif
