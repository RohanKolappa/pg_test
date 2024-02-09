// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "TBFC::Light".
//
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	TBFC.Base.Types.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _TBFC_Base_Types_H_
#define _TBFC_Base_Types_H_

// ============================================================================

#if ! defined( PLATFORM_LINUX ) && ! defined( PLATFORM_CYGWIN ) && ! defined( PLATFORM_WIN32 ) && !defined( PLATFORM_DARWIN )
#error !!! No platform specified !!!
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
/// \ingroup TBFC_Base

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
/// \ingroup TBFC_Base

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
/// \ingroup TBFC_Base

#if defined( PLATFORM_WIN32 )
	#define JBS_DLL_IMPORT __declspec( dllimport )
#else
	#define JBS_DLL_IMPORT
#endif

#endif

#define NO_DLL

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

	// Disable warning C4251:
	//	'<...>' : class '<...>' needs to have
	//	dll-interface to be used by clients of
	//	class '<...>'

	#pragma warning ( disable : 4251 )

	// Disable warning C4706:
	//	'assignment within conditional expression'
	//	('=' instead of '==' already caught by g++!)

	#pragma warning ( disable : 4706 )

	// Disable warning C4786:
	//	'std::rb_tree<CAiSpanningTree< ... > >' : identifier was
	//	truncated to '255' characters in the debug information

	#pragma warning ( disable : 4786 )

	// Disable warning C4127:
	//	'conditional expression is constant'
	
	#pragma warning ( disable : 4127 )

#endif

// ============================================================================

#ifndef byte

/// \brief A byte (8 bits).
///
/// \ingroup TBFC_Base

typedef unsigned char byte;

#endif

#ifndef uchar

/// \brief An unsigned char.
///
/// \ingroup TBFC_Base

typedef unsigned char uchar;

#endif

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Boolean value
///
/// \ingroup TBFC_Base

typedef	bool			Bool;

/// \brief Real-value (as a float).
///
/// \ingroup TBFC_Base

typedef	float			Float;

/// \brief Real-value (as a double).
///
/// \ingroup TBFC_Base

typedef	double			Double;

/// \brief A byte.
///
/// \ingroup TBFC_Base

typedef unsigned char		Byte;

/// \brief 1-byte (8 bits) unsigned character
///
/// \ingroup TBFC_Base

typedef unsigned char		Uchar;

/// \brief 1-byte (8 bits) signed character
///
/// \ingroup TBFC_Base

typedef char			Char;

/// \brief 1-byte (8 bits) unsigned integer
///
/// \ingroup TBFC_Base

typedef unsigned char		Uint8;

/// \brief 1-byte (8 bits) signed integer
///
/// \ingroup TBFC_Base

typedef char			Int8;

/// \brief 2-bytes (16 bits) unsigned integer
///
/// \ingroup TBFC_Base

typedef unsigned short		Uint16;

/// \brief 2-bytes (16 bits) signed integer
///
/// \ingroup TBFC_Base

typedef short			Int16;

/// \brief 4-bytes (32 bits) unsigned integer
///
/// \ingroup TBFC_Base

typedef unsigned int		Uint32;

/// \brief 4-bytes (32 bits) signed integer
///
/// \ingroup TBFC_Base

typedef int			Int32;

/// \brief 8-bytes (64 bits) unsigned integer
///
/// \ingroup TBFC_Base

#if defined( PLATFORM_WIN32 )
	typedef	unsigned __int64	Uint64;
#else
	typedef unsigned long long	Uint64;
#endif

/// \brief 8-bytes (64 bits) signed integer
///
/// \ingroup TBFC_Base

#if defined( PLATFORM_WIN32 )
	typedef __int64			Int64;
#else
	typedef long long		Int64;
#endif

/// \brief unsigned type large enough to store a pointer
///
/// \ingroup TBFC_Base

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
///
/// \ingroup TBFC_Base

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

/// \brief Maximum value that can be stored in a Uint8.
///
/// \ingroup TBFC_Base

const	Uint8	Uint8Max	=	255U;

/// \brief Maximum value that can be stored in an Int8.
///
/// \ingroup TBFC_Base

const	Int8	Int8Max		=	127;

/// \brief Minimum value that can be stored in an Int8.
///
/// \ingroup TBFC_Base

const	Int8	Int8Min		=	-128;

/// \brief Maximum value that can be stored in a Uint16.
///
/// \ingroup TBFC_Base

const	Uint16	Uint16Max	=	0xFFFF;

/// \brief Maximum value that can be stored in an Int16.
///
/// \ingroup TBFC_Base

const	Int16	Int16Max	=	0x7FFF;

/// \brief Minimum value that can be stored in an Int16.
///
/// \ingroup TBFC_Base

const	Int16	Int16Min	=	- Int16Max - 1;

/// \brief Maximum value that can be stored in a Uint32.
///
/// \ingroup TBFC_Base

const	Uint32	Uint32Max	=	0xFFFFFFFF;

/// \brief Maximum value that can be stored in an Int32.
///
/// \ingroup TBFC_Base

const	Int32	Int32Max	=	0x7FFFFFFF;

/// \brief Minimum value that can be stored in an Int32.
///
/// \ingroup TBFC_Base

const	Int32	Int32Min	=	- Int32Max - 1;

/// \brief Maximum value that can be stored in a Uint64.
///
/// \ingroup TBFC_Base

const	Uint64	Uint64Max	=	( (Uint64)0 - (Uint64)1 );

/// \brief Maximum value that can be stored in an Int64.
///
/// \ingroup TBFC_Base

const	Int64	Int64Max	=	(Int64)(Uint64Max >> 1);

/// \brief Minimum value that can be stored in an Int64.
///
/// \ingroup TBFC_Base

const	Int64	Int64Min	=	- Int64Max - 1;

// ============================================================================

/// \brief [SLAS 2006-8-9] Emulation of a little endian 3-bytes (24 bits)
///	unsigned integer.
///
/// \ingroup TBFC_Base

class Uint24 {

public:

	Uint24(
	) : b0( 0 ), b1( 0 ), b2( 0 ) {
	}

	Uint24(
		const	Uint32		a
	) : b0( a ), b1( a >> 8 ), b2( a >> 16 ) {
	}

	Uint24(
		const	Uchar		pb0,
		const	Uchar		pb1,
		const	Uchar		pb2
	) : b0( pb0 ), b1( pb1 ), b2( pb2 ) {
	}

	Uint24(
		const	Uint24 &	other
	) : b0( other.b0 ), b1( other.b1 ), b2( other.b2 ) {
	}

	Uint24 & operator = (
		const	Uint24 &	other
	) {
		b0 = other.b0;
		b1 = other.b1;
		b2 = other.b2;
		return * this;
	}

	operator Uint32 (
	) const {
		return ( ( Uint32 )b0       )
		     | ( ( Uint32 )b1 <<  8 )
		     | ( ( Uint32 )b2 << 16 );
	}

private:

	Uchar	b0; // LSB
	Uchar	b1;
	Uchar	b2; // MSB

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_Types_H_

// ============================================================================

