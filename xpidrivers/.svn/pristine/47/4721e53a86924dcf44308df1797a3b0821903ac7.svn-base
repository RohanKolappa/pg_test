/** @file commontypes.h
		@brief Header file defining types, functions and constants to be used by both the driver and user applications.

		(C) 2007 Copyright Barco NV \n
		All rights are reserved. \n
		Reproduction in whole or in part is prohibited without the written consent \n
		of the copyright owner. \n

		Purpose: Define common types and constants that are portable between Windows and Linux \n

		To compensate for compiler and OS differences, some basic integer types are defined.
		The format of the naming is: [<sign>]Int<width>. E.g. UInt16 represents an unsigned integer that is 16-bits wide.
		When no sign is specified, the type is signed; e.g. Int64 is a signed 64-bit integer.

		Four special cases are defined: Int, UInt, Long and ULong.
		The Int and UInt integer types are native C int types. Currently these types are 32-bit wide on both Windows and Linux 32- and 64-bit.\n
		The Long and ULong integer types follow the native width of the OS.\n
		They are guaranteed to be the same size as a pointer, \n
		i.e. they are 32-bits wide on a 32-bit OS, 64-bit on a 64-bit OS.\n
		These types should be used anywhere where a pointer must be cast to an integer (to do pointer arithmetic for instance).\n

		Note: For buffer sizes, it is recommended to use size_t (this type has a 'natural' size)

		Note: The __w64 keyword, marks OS dependent types, which could generate warnings when compiled on 64-bit.
		Example: ULong i = 5; UInt32 j; j=i; //will generate a warning because on 64-bit the type would be truncated
		To generate portability warnings, on Windows the /Wp64 compiler swith must be used.

		Besides integer types, this header also defines a Char, UChar, Bool, Float and Double type.
		The constants TRUE (1) and FALSE (0) are defined as well, but only if they weren't defined before.

		@var	UInt8	Unsigned 8-bit integer, aka byte
		@var	Int8	Signed 8-bit integer
		@var	UInt16	Unsigned 16-bit integer
		@var	Int16	Signed 16-bit integer
		@var	UInt32	Unsigned 32-bit integer
		@var	Int32	Signed 32-bit integer
		@var	UInt64	Unsigned 64-bit integer
		@var	Int64	Signed 64-bit integer
		@var	Int		Signed integer (same size as "int" type, which is currently 32-bit)
		@var	UInt	Unsigned integer (same size as "unsigned int" type, which is currently 32-bit)
		@var	Long	Signed "native-wide" integer (on 32-bit systems: 32-bit wide; on 64-bit systems: 64-bit wide)
		@var	ULong	Unsigned "native-wide" integer (on 32-bit systems: 32-bit wide; on 64-bit systems: 64-bit wide)
		@var	Char	A signed non-unicode, one-byte character
		@var	UChar	An unsigned single non-unicode, one-byte character
		@var	Bool	A type that can hold TRUE or FALSE; defined as a native integer
		@var	Float	A floating type of single precision
		@var	Double	A floating type of double precision

		Revision history: \n
		See SVN :-)
*/
#ifndef _COMMONTYPES_H
#define _COMMONTYPES_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
typedef int Bool;

#if !defined(FALSE)
	#define FALSE 0
#endif	/* FALSE */
#if !defined (TRUE)
	#define TRUE 1
#endif	/* TRUE */

typedef float			Float;
typedef double			Double;
typedef char			Char;
typedef unsigned char	UChar;

#ifdef __linux__ /* GNU GCC */
#ifdef __KERNEL__
#include <linux/types.h>
	typedef u8			UInt8;
	typedef s8			Int8;

	typedef u16			UInt16;
	typedef s16			Int16;

	typedef u32			UInt32;
	typedef s32			Int32;

	typedef u64			UInt64;
	typedef s64			Int64;
#else	/*NOT __KERNEL__ */
#include <inttypes.h>
	typedef uint8_t		UInt8;
	typedef int8_t			Int8;

	typedef uint16_t		UInt16;
	typedef int16_t			Int16;

	typedef uint32_t		UInt32;
	typedef int32_t			Int32;

	typedef uint64_t		UInt64;
	typedef int64_t			Int64;
#endif	/* __KERNEL__ */

	typedef	unsigned int	UInt;
	typedef   signed int	Int;
	typedef	unsigned long	ULong;
	typedef   signed long	Long;
#endif	/* __LINUX__ */

#if defined(_WIN32) || defined(_WIN64)
	typedef unsigned __int8		UInt8;
	typedef   signed __int8		Int8;

	typedef unsigned __int16	UInt16;
	typedef   signed __int16	Int16;

	typedef unsigned __int32	UInt32;
	typedef   signed __int32	Int32;

	typedef unsigned __int64	UInt64;
	typedef   signed __int64	Int64;

	#ifdef _WIN64
		typedef unsigned __int32	UInt;					//!< 32-bit unsigned integer type
		typedef signed __int32		Int;					//!< 32-bit signed integer type
		typedef unsigned __int64	ULong;					//!< 64-bit unsigned integer type
		typedef signed __int64		Long;					//!< 64-bit signed integer type
	#else		/* _WIN64 */
		typedef unsigned __int32		UInt;		//!< 32-bit unsigned integer type
		typedef signed __int32			Int;		//!< 32-bit signed integer type
//		typedef unsigned __int32 __w64	ULong;		//!< 32-bit unsigned integer type
//		typedef signed __int32 __w64	Long;		//!< 32-bit signed integer type
	#endif	/* _WIN64 */
#endif	/* _WIN32 || _WIN64 */

#if	!defined(IN)
#define IN					/* empty */
#endif	/* IN */
#if !defined(OUT)
#define OUT					/* empty */
#endif	/* OUT */
#if !defined(OPTIONAL)
#define OPTIONAL			/* empty */
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif	/* _COMMONTYPES_H */
