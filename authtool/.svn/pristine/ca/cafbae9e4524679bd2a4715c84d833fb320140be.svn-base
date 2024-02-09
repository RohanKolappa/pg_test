/*
 * IPVSTranscode_compatibility.h
 *
 *  Created on: Sep 19, 2011
 *      Author: rkale
 */

#ifndef IPVSTRANSCODE_COMPATIBILITY_H_
#define IPVSTRANSCODE_COMPATIBILITY_H_


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

#define DLLEXPORT

#ifdef WINDOWS

#ifdef __MINGW32__

#include <sys/time.h>
#include <inttypes.h>

#else
#include "stdafx.h"


#endif



#else  // For linux

#include <inttypes.h>

#endif // WINDOWS


#endif /* IPVSTRANSCODE_COMPATIBILITY_H_ */
