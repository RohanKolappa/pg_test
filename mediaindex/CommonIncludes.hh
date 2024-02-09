#ifndef _COMMON_INCLUDES_HH
#define _COMMON_INCLUDES_HH

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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef WINDOWS

#include <io.h>

#ifdef __MINGW32__
#include <inttypes.h>

#define FTELL ftello64
#define FSEEK fseeko64
#define STRTOULL strtoull

#else

// VC++ in Windows
typedef  __int8  int8_t;
typedef unsigned __int8  uint8_t;
typedef  __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32  int32_t;
typedef unsigned __int32  uint32_t;
typedef __int64  int64_t;
typedef unsigned __int64  uint64_t;

#define PRIu64  "I64u"

#define FSEEK _fseeki64
#define FTELL _ftelli64
#define STRTOULL ::_strtoi64

#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE

#define snprintf _snprintf
#define strcasecmp _stricmp

#endif


#define LSTAT(filename, buffer) stat(filename, buffer)
#define OPEN(filename, flags) _open((filename), (flags))
#define READ(handle, buffer, count) _read((handle), (buffer), (count))
#define CLOSE(handle) _close((handle));

#define FILESEPERATOR "\\"
#define FILESEPERATOR_CHR '\\'


#else // on linux

#define LSTAT(filename, buffer) lstat(filename, buffer)
#define STRTOULL strtoull
#define OPEN(filename, flags) open((filename), (flags))
#define READ(handle, buffer, count) read((handle), (buffer), (count))
#define CLOSE(handle) close((handle));

#define FTELL ftello64
#define FSEEK fseeko64

#include <inttypes.h>
#include <sys/file.h>
#include <unistd.h>
#include <dirent.h>
#include <syslog.h>

#define FILESEPERATOR "/"
#define FILESEPERATOR_CHR '/'
#endif // WINDOWS


#endif
