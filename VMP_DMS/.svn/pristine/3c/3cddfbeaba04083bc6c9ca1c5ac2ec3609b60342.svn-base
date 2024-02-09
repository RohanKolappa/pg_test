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
//	TBFC.Base.Exception.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_Exception_H_
#define _TBFC_Base_Exception_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include <exception>

#include "TBFC.Base.Buffer.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Base class of all TBFC C++ exceptions.
///
/// TBFC uses a C++ exception system based on the following requirements
/// and/or observations.
///
/// When an exception is thrown, it is most often because a local error
/// happened (BadArgument, ...), or an exception has been caught after
/// calling some other function.
///
/// In all cases, the exception being thrown reflects, by its name, the
/// error condition that has been encountered (OutOfRange, ...).
/// But, in the case
/// an exception is the result of a failed call to some other
/// function, it should be possible to attach a complementary
/// explanation to the exception being thrown. This is exactly what the
/// TBFC exceptions were developed for.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL Exception : public std::exception {

public :

	Exception(
	);

	/// \brief Creates a new Exception.
	///
	/// \param cn
	///	Class name.
	///
	/// \param en
	///	Error name.
	///
	/// \param pe
	///	Previous exception.
	///
	/// \param am
	///	Additional message.

	Exception(
		const	Char		* cn,
		const	Char		* en,
		const	Exception	* pe,
		const	Buffer		& am
	);

	Exception(
		const	Exception	& src
	);

	virtual ~Exception(
	) throw (
	);

	Exception& operator = (
		const	Exception	& src
	);

	/// \brief Returns a human-readable description of this exception.

	virtual const Char * what(
	) const throw (
	);

	const Char * getClassName(
	) const {
		return className;
	}

	const Char * getErrorName(
	) const {
		return errorName;
	}

	const Char * getAdMessage(
	) const {
		return adMessage;
	}

	const Exception * getPrevExcep(
	) const {
		return prevExcep;
	}

//	static void dumpCallStack(
//	);

protected :

	void kill(
	);

	static Char * strDup(
		const	Char		* src,
		const	Uint32		len = (Uint32)-1
	);

	static Exception * excDup(
		const	Exception	* src
	);

private :

	Char		* className;	///< Class name.
	Char		* errorName;	///< Error name.
	Char		* adMessage;	///< Additional message.
	Exception	* prevExcep;	///< Previous exception.
	mutable Char	* whatCache;	///< "what" message cache.

};

// ============================================================================

/// \brief Macro used to declare and define the inner LocalClassException of a
///	user class.
///
/// \param __CN__
///	The outer class name this LocalClassException class belongs to.
///
/// \ingroup TBFC_Base

#define BASE_CLASS_GEN_EXCEPTION( __CN__ )				\
class LocalClassException : public TBFC::Exception {			\
public :								\
	virtual ~LocalClassException() throw () {}			\
protected :								\
	LocalClassException(						\
		const	TBFC::Char	* en,				\
		const	TBFC::Exception	* pe = 0,			\
		const	TBFC::Buffer	& am = TBFC::Buffer()		\
	) : TBFC::Exception( __CN__, en, pe, am ) {}			\
}

// ============================================================================

/// \brief Macro used to declare and define some inner Exception class of a
///	user class.
///
/// \param __CN__
///	The class name of this Exception.
///
/// \param __EN__
///	A description of this exception.
///
/// \ingroup TBFC_Base

#define CLASS_EXCEPTION( __CN__, __EN__ )				\
class __CN__ : public LocalClassException {				\
public :								\
	virtual ~__CN__() throw () {}					\
	__CN__(								\
	) : LocalClassException( __EN__ ) {}				\
	__CN__(								\
		const	TBFC::Buffer	& am				\
	) : LocalClassException( __EN__, 0, am ) {}			\
	__CN__(								\
		const	TBFC::Exception	& p,				\
		const	TBFC::Buffer	& am = TBFC::Buffer()		\
	) : LocalClassException( __EN__, & p, am ) {}			\
}

// ============================================================================

/// \brief Macro used to declare and define an Exception at the global scope.
///
/// \param __DN__
///	The DLL define that should be used as prefix (ex: TBFC_Light_DLL).
///
/// \param __CN__
///	The class name of this Exception (ex: OutOfRange).
///
/// \param __EN__
///	A description of this exception (ex: "Out of range").
///
/// \ingroup TBFC_Base

#define GLOBAL_CLASS_EXCEPTION( __DN__, __CN__, __EN__ )		\
class __DN__ __CN__ : public TBFC::Exception {				\
public :								\
	__CN__(								\
	) : TBFC::Exception(						\
		"<global>", __EN__, 0, TBFC::Buffer() ) {}		\
	__CN__(								\
		const	TBFC::Buffer	& a				\
	) : TBFC::Exception(						\
		"<global>", __EN__, 0, a ) {}				\
	__CN__(								\
		const	TBFC::Exception	& p,				\
		const	TBFC::Buffer	& a = TBFC::Buffer()		\
	) : TBFC::Exception(						\
		"<global>", __EN__, &p, a ) {}				\
	virtual ~__CN__() throw () {}					\
}

// ============================================================================

/// \brief Predefined "BadCast" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, BadCast, "Bad cast" );

/// \brief Predefined "InternalError" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, InternalError, "Internal error" );

/// \brief Predefined "NotImplemented" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, NotImplemented, "Not implemented" );

/// \brief Predefined "NullPointer" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, NullPointer, "Null pointer" );

/// \brief Predefined "OutOfRange" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, OutOfRange, "Out of range" );

/// \brief Predefined "OutOfMemory" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, OutOfMemory, "Out of virtual memory" );

/// \brief Predefined "ThreadStopped" global exception.
///
/// \ingroup TBFC_Base

GLOBAL_CLASS_EXCEPTION( TBFC_Light_DLL, ThreadStopped, "Thread stopped" );

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_Exception_H_

// ============================================================================

