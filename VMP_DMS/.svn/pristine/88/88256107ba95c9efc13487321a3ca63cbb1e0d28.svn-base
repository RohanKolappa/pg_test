// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
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

#ifndef _BFC_Types_H_
#define _BFC_Types_H_

// ============================================================================

#if !defined(PLATFORM_LINUX) && !defined(PLATFORM_CYGWIN) && !defined(PLATFORM_WIN32) && !defined(PLATFORM_DARWIN)
#error !!! No platform specified !!!
#endif

// ============================================================================

#include "BFC.Common.h"

// ============================================================================

/// Empty macro.

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
/// \ingroup Common_Base

#define JBS_DLL_NONE

/// \def JBS_DLL_EXPORT
/// \brief Takes a system-dependent value.
///
/// This macro is defined as the system-dependent string that should be
/// used in front of all exported object declarations when creating a
/// Dynamic Library (DLL or SO).
///
/// Typically, it will be "__declspec(dllexport)" for Windows platforms,
/// and the empty string for more clever environments.
///
/// \ingroup Common_Base

/// \def JBS_DLL_IMPORT
/// \brief Takes a system-dependent value.
///
/// This macro is defined as the system-dependent string that should be
/// used in front of all imported object declarations when using a
/// Dynamic Library (DLL or SO).
///
/// Typically, it will be "__declspec(dllimport)" for Windows platforms,
/// and the empty string for more clever environments.
///
/// \ingroup Common_Base

#if defined(PLATFORM_WIN32)

	#define JBS_DLL_EXPORT __declspec(dllexport)
	#define JBS_DLL_IMPORT __declspec(dllimport)

	// Disable warning C4251:
	//	'<...>' : class '<...>' needs to have
	//	dll-interface to be used by clients of
	//	class '<...>'

	#pragma warning ( disable : 4251 )

	// Disable warning C4786:
	//	'std::rb_tree<CAiSpanningTree< ... > >' : identifier was
	//	truncated to '255' characters in the debug information

	#pragma warning ( disable : 4786 )

#else

	#define JBS_DLL_EXPORT
	#define JBS_DLL_IMPORT

#endif

// ============================================================================

// ============================================================================

#ifndef byte

/// A byte (8 bits).

///
/// \ingroup Common_Base

typedef unsigned char byte;

#endif

#ifndef uchar

/// An unsigned char.

///
/// \ingroup Common_Base

typedef unsigned char uchar;

#endif

// ============================================================================

namespace BFC {

// ============================================================================

// -----------------
// Standard types...
// -----------------

/// Boolean value

///
/// \ingroup Common_Base

typedef	bool			Bool;

/// Real-value (as a float).

///
/// \ingroup Common_Base

typedef	float			Float;

/// Real-value (as a double).

///
/// \ingroup Common_Base

typedef	double			Double;

/// \brief A byte.
///
/// \ingroup Common_Base

typedef unsigned char		Byte;

/// \brief 1-byte (8 bits) unsigned character
///
/// \ingroup Common_Base

typedef unsigned char		Uchar;

/// 1-byte (8 bits) signed character

///
/// \ingroup Common_Base

typedef char			Char;

/// 1-byte (8 bits) unsigned integer

///
/// \ingroup Common_Base

typedef unsigned char		Uint8;

/// 1-byte (8 bits) signed integer

///
/// \ingroup Common_Base

typedef char			Int8;

/// 2-bytes (16 bits) unsigned integer

///
/// \ingroup Common_Base

typedef unsigned short		Uint16;

/// 2-bytes (16 bits) signed integer

///
/// \ingroup Common_Base

typedef short			Int16;

/// 4-bytes (32 bits) unsigned integer

///
/// \ingroup Common_Base

typedef unsigned int		Uint32;

/// 4-bytes (32 bits) signed integer

///
/// \ingroup Common_Base

typedef int			Int32;

#if defined(PLATFORM_WIN32)

	/// 8-bytes (64 bits) unsigned integer

	///
	/// \ingroup Common_Base

	typedef	unsigned __int64	Uint64;

	/// 8-bytes (64 bits) signed integer

	///
	/// \ingroup Common_Base

	typedef __int64			Int64;

#else

	/// 8-bytes (64 bits) unsigned integer

	///
	/// \ingroup Common_Base

	typedef unsigned long long	Uint64;

	/// 8-bytes (64 bits) signed integer

	///
	/// \ingroup Common_Base

	typedef long long		Int64;

#endif

/// \brief unsigned type large enough to store a pointer

#if defined( PLATFORM_WIN32 )
#if defined( __WIN64__ )
	typedef	Uint64	UintPtr;
#else
	typedef	Uint32	UintPtr;
#endif
#else
#if defined( _LP64 )
	typedef Uint64	UintPtr;
#else
	typedef Uint32	UintPtr;
#endif
#endif

/// \brief signed type large enough to store a pointer

#if defined( PLATFORM_WIN32 )
#if defined( __WIN64__ )
	typedef	Int64	IntPtr;
#else
	typedef	Int32	IntPtr;
#endif
#else
#if defined( _LP64 )
	typedef Int64	IntPtr;
#else
	typedef Int32	IntPtr;
#endif
#endif

/// Maximum value that can be stored in a Uint8.

///
/// \ingroup Common_Base

const	Uint8	Uint8Max	=	255U;

/// Maximum value that can be stored in an Int8.

///
/// \ingroup Common_Base

const	Int8	Int8Max		=	127;

/// Minimum value that can be stored in an Int8.

///
/// \ingroup Common_Base

const	Int8	Int8Min		=	-128;

/// Maximum value that can be stored in a Uint16.

///
/// \ingroup Common_Base

const	Uint16	Uint16Max	=	0xFFFF;

/// Maximum value that can be stored in an Int16.

///
/// \ingroup Common_Base

const	Int16	Int16Max	=	0x7FFF;

/// Minimum value that can be stored in an Int16.

///
/// \ingroup Common_Base

const	Int16	Int16Min	=	- Int16Max - 1;

/// Maximum value that can be stored in a Uint32.

///
/// \ingroup Common_Base

const	Uint32	Uint32Max	=	0xFFFFFFFF;

/// Maximum value that can be stored in an Int32.

///
/// \ingroup Common_Base

const	Int32	Int32Max	=	0x7FFFFFFF;

/// Minimum value that can be stored in an Int32.

///
/// \ingroup Common_Base

const	Int32	Int32Min	=	- Int32Max - 1;

/// Maximum value that can be stored in a Uint64.

///
/// \ingroup Common_Base

const	Uint64	Uint64Max	=	( ( (Uint64)0 ) - 1 );

/// Maximum value that can be stored in an Int64.

///
/// \ingroup Common_Base

const	Int64	Int64Max	=	(Uint64Max >> 1);

/// Minimum value that can be stored in an Int64.

///
/// \ingroup Common_Base

const	Int64	Int64Min	=	- Int64Max - 1;

// --------------------------------------------------
// Some memory [de]allocation operators for C++ code.
// --------------------------------------------------

#if 0

#include <stdlib.h>

///	Memory allocator (wrapper).

///	This method reimplements the traditional `new' operator for debugging
///	purposes.

void *operator new (size_t);

///	Memory allocator (wrapper).

///	This method reimplements the traditional `new[]' operator for debugging
///	purposes.

void *operator new[] (size_t);

///	Memory deallocator (wrapper).

///	This method reimplements the traditional `delete' operator for
///	debugging purposes.

void operator delete (void *);

///	Memory deallocator (wrapper).

///	This method reimplements the traditional `delete[]' operator for
///	debugging purposes.

void operator delete[] (void *);

///	A simple allocated memory counter.

///	This function returns the total amount of mmory (in bytes) that was
///	allocated using one of the reimplemented operators (`new' or `new[]').

Uint64 getAllocatedMemory();

#endif

// ============================================================================

} // namespace BFC

// ============================================================================


#endif  // _BFC_Types_H_


// ============================================================================


