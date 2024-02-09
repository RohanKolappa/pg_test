/*************************************************************************
 *
 * Unless otherwise specified, Copyright (C) 2004 Barco Control Rooms
 *
 * $Id: common.h,v 1.12 2004/09/20 09:54:42 mleeman Exp $
 *
 * $Source: /home/cvs/firmware/ppc/userspace-utils/common.h,v $
 * $Revision: 1.12 $
 * $Author: mleeman $
 * $Date: 2004/09/20 09:54:42 $
 *
 * Last ChangeLog Entry:
 * $Log: common.h,v $
 * Revision 1.12  2004/09/20 09:54:42  mleeman
 * - removed DOS EOLs
 * - fixed compiler warnings (unsigned char unneeded comparisons)
 * - fixed file permissions
 *
 * Revision 1.11  2004/09/14 11:35:43  bvandeve
 * - Made compilation possible for WIN32
 * - Transport Stream:
 *   - Added stream type 0x10 (MPEG4)
 *   - Don't use PMT ES_info_length when > tsPacketSize!
 *
 * Revision 1.10  2004/08/23 11:36:02  mleeman
 * - some modifications that force not printing anything when CGI
 * - adding CGI log
 *
 * Revision 1.1  2004/08/02 11:24:16  mleeman
 * - Update flash code to the new kernel configuration; sync with userspace-utils
 * - added common.h for common configurations; sync with userspace-utils
 *
 * Revision 1.5  2004/07/30 12:05:29  mleeman
 * moved the definition (and inclusion of stdbool) to common.h instead in the kernel interfaces file
 *
 * Revision 1.4  2004/07/30 08:29:01  mleeman
 * added max/min defines
 *
 * Revision 1.3  2004/07/26 11:09:23  mleeman
 * implemented the WIN32 crud
 *
 * Revision 1.2  2004/07/15 09:12:23  mleeman
 * * compilation with -Wall
 * * added stricter prototyping in the header files to break compilation when
 *   something is changed in the parameters of the functions (will otherwise
 *   segfault anyway, but difficult to track).
 * * use of autoheader: the defines are stored in the generated config.h file
 * * renamed main.h to stream2dsp.h
 *  charactersf
 * * inclusion of config.h in all the files
 * * removal of all unused variables
 * * Added standard header with CVS keywords
 *
 *
 ************************************************************************/

//#include <stdio.h>

#ifndef _COMMON_H_
#define _COMMON_H_



#include <stdint.h>
#include <stdbool.h>
#define HAVE_STDBOOL_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <unistd.h>




#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <fcntl.h>

//#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>





#ifdef WIN32
#define inline
#endif

#ifndef HAVE_STDBOOL_H
typedef enum {
	false=0,
	true
} bool;
#else
#include <stdbool.h>
#endif

#ifndef WIN32
#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

#ifdef DEFINED_SILENT_STDOUT
#define PRINT(format, args...)
#define EPRINT(format, args...)
#else
#define PRINT(format, args...) fprintf(stdout, "STDOUT %s(%d): " format, __FILE__, __LINE__ ,## args)
#define EPRINT(format, args...) fprintf(stderr, "STDERR %s(%d): " format, __FILE__, __LINE__ ,## args)
#endif
#endif
#endif
