/*
 * strmsrv_compatibility.h
 *
 *  Created on: Nov 22, 2010
 *      Author: rkale
 */

#ifndef STRMSRV_COMPATIBILITY_H_
#define STRMSRV_COMPATIBILITY_H_

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

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
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
#include <unistd.h>

#define _CrtSetReportMode(paramX, paramY) do {} while (0)
#define srandom(param) srand((param))
#define random() rand()

#else

typedef  __int8  int8_t;
typedef unsigned __int8  uint8_t;
typedef  __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32  int32_t;
typedef unsigned __int32  uint32_t;
typedef __int64  int64_t;
typedef unsigned __int64  uint64_t;


#endif

#ifndef WINVER
#define WINVER  0x501
#endif

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <sys/timeb.h>
#define MSG_DONTWAIT 0
#define SOCKOPTCAST (char *)

#else  // For linux

#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/types.h>

#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
//#include <sys/file.h>
#include <dirent.h>

#define SOCKOPTCAST

#define O_BINARY   0

#endif // WINDOWS

#endif /* STRMSRV_COMPATIBILITY_H_ */
