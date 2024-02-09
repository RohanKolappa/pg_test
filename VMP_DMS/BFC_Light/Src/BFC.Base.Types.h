// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::Light".
//
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.Base.Types.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Base_Types_H_
#define _BFC_Base_Types_H_

// ============================================================================

#if ! defined( PLATFORM_LINUX ) && ! defined( PLATFORM_CYGWIN ) && ! defined( PLATFORM_WIN32 ) && !defined( PLATFORM_DARWIN )
#error !!! No platform specified !!!
#endif

#if defined( PLATFORM_WIN32 )
#include <float.h>
#endif

// ============================================================================

#if ! defined( JBS_DLL_DEFS )

#define JBS_DLL_DEFS

/// \brief Empty macro.
///
/// This empty macro is to be used when you want to supply an empty
/// argument to another macro that requires an argument.
///
/// Example:
/// \verbatim
/// ...
/// #define class_header(a,b) class a b
/// ...
/// class_header(,MyClass) {}; // fails to compile with some compilers!
/// class_header(JBS_DLL_NONE,MyClass) {}; // always OK!
/// ...
/// \endverbatim
///
/// \ingroup BFC_Base

#define JBS_DLL_NONE

/// \brief Symbol export helper macro.
///
/// This macro is defined as the system-dependent string that should be
/// used in front of all exported object declarations when creating a
/// Dynamic Library (DLL or SO).
///
/// Typically, it will be "__declspec( dllexport )" for Windows platforms,
/// and the empty string for more clever environments.
///
/// \ingroup BFC_Base

#if defined( PLATFORM_WIN32 )
	#define JBS_DLL_EXPORT __declspec( dllexport )
#else
	#define JBS_DLL_EXPORT
#endif

/// \def JBS_DLL_IMPORT
///
/// \brief Symbol import helper macro.
///
/// This macro is defined as the system-dependent string that should be
/// used in front of all imported object declarations when using a
/// Dynamic Library (DLL or SO).
///
/// Typically, it will be "__declspec( dllimport )" for Windows platforms,
/// and the empty string for more clever environments.
///
/// \ingroup BFC_Base

#if defined( PLATFORM_WIN32 )
	#define JBS_DLL_IMPORT __declspec( dllimport )
#else
	#define JBS_DLL_IMPORT
#endif

#endif

#ifndef NO_DLL
#define NO_DLL
#endif

#if defined( PLATFORM_WIN32 )
	#define DLLEXPORT __declspec( dllexport )
#else
	#define DLLEXPORT
#endif

#if defined( PLATFORM_WIN32 )
	#define DLLIMPORT __declspec( dllimport )
#else
	#define DLLIMPORT
#endif

// ============================================================================

#if defined( PLATFORM_WIN32 )

	// Disable warning C4127:
	//	'conditional expression is constant'
	
//	#pragma warning ( disable : 4127 )

	// Disable warning C4250:
	//	'inheritance via dominance'

	#pragma warning ( disable : 4250 )

	// Disable warning C4251:
	//	'<...>' : class '<...>' needs to have
	//	dll-interface to be used by clients of
	//	class '<...>'

	#pragma warning ( disable : 4251 )

	// Disable warning C4706:
	//	'assignment within conditional expression'
	//	('=' instead of '==' already caught by g++!)

//	#pragma warning ( disable : 4706 )

	// Disable warning C4786:
	//	'std::rb_tree<CAiSpanningTree< ... > >' : identifier was
	//	truncated to '255' characters in the debug information

//	#pragma warning ( disable : 4786 )

	// Disable warning C4800:
	//	'...': forcing value to bool 'true' or 'false' (performance warning).

//	#pragma warning ( disable : 4800 )

	// Disable warning C4996:
	//	'...': The POSIX name for this item is deprecated.

//	#pragma warning ( disable : 4996 )

#endif

// ============================================================================

#ifndef byte

/// \brief A byte (8 bits).
///
/// \ingroup BFC_Base

typedef unsigned char byte;

#endif

#ifndef uchar

/// \brief An unsigned char.
///
/// \ingroup BFC_Base

typedef unsigned char uchar;

#endif

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Boolean value
///
/// \ingroup BFC_Base

typedef	bool				Bool;

/// \brief Real-value (as a float).
///
/// \ingroup BFC_Base

typedef	float				Float;

/// \brief Real-value (as a double).
///
/// \ingroup BFC_Base

typedef	double				Double;

/// \brief Real-value (as a long double).
///
/// \ingroup BFC_Base

typedef long double			LDouble;

/// \brief A byte.
///
/// \ingroup BFC_Base

typedef unsigned char			Byte;

/// \brief 1-byte (8 bits) unsigned character
///
/// \ingroup BFC_Base

typedef unsigned char			Uchar;

/// \brief 1-byte (8 bits) signed character
///
/// \ingroup BFC_Base

typedef char				Char;

/// \brief 1-byte (8 bits) unsigned integer
///
/// \ingroup BFC_Base

typedef unsigned char			Uint8;

/// \brief 1-byte (8 bits) signed integer
///
/// \ingroup BFC_Base

typedef signed char			Int8;

/// \brief 2-bytes (16 bits) unsigned integer
///
/// \ingroup BFC_Base

typedef unsigned short			Uint16;

/// \brief 2-bytes (16 bits) signed integer
///
/// \ingroup BFC_Base

typedef short				Int16;

/// \brief 4-bytes (32 bits) unsigned integer
///
/// \ingroup BFC_Base

typedef unsigned int			Uint32;

/// \brief 4-bytes (32 bits) signed integer
///
/// \ingroup BFC_Base

typedef int				Int32;

/// \brief 8-bytes (64 bits) unsigned integer
///
/// \ingroup BFC_Base

#if defined( PLATFORM_WIN32 )
	typedef	unsigned __int64	Uint64;
#else
	typedef unsigned long long	Uint64;
#endif

/// \brief 8-bytes (64 bits) signed integer
///
/// \ingroup BFC_Base

#if defined( PLATFORM_WIN32 )
	typedef __int64			Int64;
#else
	typedef long long		Int64;
#endif

#if defined( PLATFORM_WIN32 )
#define CU64( v )	( BFC::Uint64 )( v )
#else
#define CU64( v )	( BFC::Uint64 )( v##ULL )
#endif

#if defined( PLATFORM_WIN32 )
#define CS64( v )	( BFC::Int64 )( v )
#else
#define CS64( v )	( BFC::Int64 )( v##LL )
#endif

/// \brief unsigned type large enough to store a pointer
///
/// \ingroup BFC_Base

#if defined( PLATFORM_WIN32 )
#if defined( __WIN64__ )
	typedef	Uint64			UintPtr;
#else
	typedef	Uint32			UintPtr;
#endif
#else
#if defined( _LP64 )
	typedef Uint64			UintPtr;
#else
	typedef Uint32			UintPtr;
#endif
#endif

/// \brief signed type large enough to store a pointer
///
/// \ingroup BFC_Base

#if defined( PLATFORM_WIN32 )
#if defined( __WIN64__ )
	typedef	Int64			IntPtr;
#else
	typedef	Int32			IntPtr;
#endif
#else
#if defined( _LP64 )
	typedef Int64			IntPtr;
#else
	typedef Int32			IntPtr;
#endif
#endif

/// \brief Maximum positive value that can be stored in a Double.
///
/// \ingroup BFC_Base

#if defined( __DBL_MAX__ )

const	Double	DoubleMax	=	__DBL_MAX__;

#elif defined( DBL_MAX )

const	Double	DoubleMax	=	DBL_MAX;

#else
#error No compiler specific max double value!
#endif

/// \brief Minimum positive value that can be stored in a Double.
///
/// \ingroup BFC_Base

#if defined( __DBL_MIN__ )

const	Double	DoubleMin	=	__DBL_MIN__;

#elif defined( DBL_MIN )

const	Double	DoubleMin	=	DBL_MIN;

#else
#error No compiler specific min double value!
#endif

/// \brief Maximum positive value that can be stored in a Float.
///
/// \ingroup BFC_Base

#if defined( __FLT_MAX__ )

const	Float	FloatMax	=	__FLT_MAX__;

#elif defined( FLT_MAX )

const	Float	FloatMax	=	FLT_MAX;

#else
#error No compiler specific max float value!
#endif

/// \brief Minimum positive value that can be stored in a Float.
///
/// \ingroup BFC_Base

#if defined( __FLT_MIN__ )

const	Float	FloatMin	=	__FLT_MIN__;

#elif defined( FLT_MIN )

const	Float	FloatMin	=	FLT_MIN;

#else
#error No compiler specific min double value!
#endif

/// \brief Maximum value that can be stored in a Uint8.
///
/// \ingroup BFC_Base

const	Uint8	Uint8Max	=	255U;

/// \brief Maximum value that can be stored in an Int8.
///
/// \ingroup BFC_Base

const	Int8	Int8Max		=	127;

/// \brief Minimum value that can be stored in an Int8.
///
/// \ingroup BFC_Base

const	Int8	Int8Min		=	-128;

/// \brief Maximum value that can be stored in a Uint16.
///
/// \ingroup BFC_Base

const	Uint16	Uint16Max	=	0xFFFF;

/// \brief Maximum value that can be stored in an Int16.
///
/// \ingroup BFC_Base

const	Int16	Int16Max	=	0x7FFF;

/// \brief Minimum value that can be stored in an Int16.
///
/// \ingroup BFC_Base

const	Int16	Int16Min	=	- Int16Max - 1;

/// \brief Maximum value that can be stored in a Uint32.
///
/// \ingroup BFC_Base

const	Uint32	Uint32Max	=	0xFFFFFFFF;

/// \brief Maximum value that can be stored in an Int32.
///
/// \ingroup BFC_Base

const	Int32	Int32Max	=	0x7FFFFFFF;

/// \brief Minimum value that can be stored in an Int32.
///
/// \ingroup BFC_Base

const	Int32	Int32Min	=	- Int32Max - 1;

/// \brief Maximum value that can be stored in a Uint64.
///
/// \ingroup BFC_Base

const	Uint64	Uint64Max	=	( (Uint64)0 - (Uint64)1 );

/// \brief Maximum value that can be stored in an Int64.
///
/// \ingroup BFC_Base

const	Int64	Int64Max	=	(Int64)(Uint64Max >> 1);

/// \brief Minimum value that can be stored in an Int64.
///
/// \ingroup BFC_Base

const	Int64	Int64Min	=	- Int64Max - 1;

// ============================================================================

typedef unsigned int	Uint;

// ============================================================================

// typedef Int16	Flt214;
// typedef Int32	Flt266;
// typedef Int32	Flt1616;

// ============================================================================

#if defined( PLATFORM_WIN32 )

// workaround for a bug in the windows compiler
// see http://itrack.barco.com/browse/IT5000-2

#define FIX_FOR_WIN32_COMP_PROBLEM(x) \
	public: \
		virtual const char* __getClassName__() { \
			return #x; \
		}

#else

#define FIX_FOR_WIN32_COMP_PROBLEM(x)

#endif

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Types_H_

// ============================================================================

