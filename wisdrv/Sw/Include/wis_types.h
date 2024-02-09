/******************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
*******************************************************************************
*
*   FILE: 
*
*		wis_types.h
*
*   DESCRIPTION:
* 		OS-Layer (OSL) type definitions.
*
*   $Id: wis_types.h,v 1.1.1.1 2007-01-10 20:52:40 sjain Exp $
*
******************************************************************************/

#ifndef _WIS_TYPES_H
#define _WIS_TYPES_H

/*
 * Define platform-independent types
 */
typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
#if!defined(__KERNEL__)
typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
#endif
typedef int                     sint32;
typedef short                   sint16;
typedef char                    sint8;
typedef int                     bool_t;
typedef int                     status_t;
typedef int  (*funcptr_t) (void);

/* defines to support pacgen */
#define	CONST			        const

typedef uint32                  phys_addr_t;  /* Physical address */
typedef uint8                   mac_addr_t[6];	  /* MAC address */
typedef uint32                  ip_addr_t;		 /* IP Address */

#define MACADDR_STR_LEN	        18      /* Formatted MAC address */
#define	IPADDR_STR_LEN	        16      /* Formatted IP address */
#define	ENET_MAX_SIZE	        (2 * 1024) 

#ifndef TRUE
	#define TRUE   1
	#define FALSE  (!TRUE)
#endif

#define VOL			    volatile
#define STATIC			static
#define INT_TO_PTR(x)    (((void*)((uint32)(x))))
/*
 * OSL Specific types.
 */
#ifdef VXWORKS
typedef void                    *osl_thread_t;     
typedef void			*OSL_THREAD_t;
#elif __linux__

#endif

/* native thread*/
typedef void (*osl_dpc_fn_t)(void*, void*, void*, void*, void*);
/* returned by thread routines */
typedef void (*osl_dpc_fn)(int, int, int, int, int);	


#endif /* _WIS_TYPES_H */
