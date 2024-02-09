// ============================================================================
// 
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
//	BFC.Base.Exception.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_Exception_H_
#define _BFC_Base_Exception_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include <exception>

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

class Buffer;

// ============================================================================

/// \brief Base class of all BFC C++ exceptions.
///
/// BFC uses a C++ exception system based on the following requirements
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
/// BFC exceptions were developed for.
///
/// \ingroup BFC_Base

class BFC_Light_DLL Exception : public std::exception {

public :

	Exception(
	);

	Exception(
		const	Char *		cn,
		const	Char *		en,
		const	Exception *	pe
	);

	Exception(
		const	Char *		cn,
		const	Char *		en,
		const	Exception *	pe,
		const	Char *		am
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
		const	Char *		cn,
		const	Char *		en,
		const	Exception *	pe,
		const	Buffer &	am
	);

	Exception(
		const	Exception &	src
	);

	/* virtual */ ~Exception(
	) throw (
	);

	Exception & operator = (
		const	Exception &	src
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

	void dumpCallStack(
	) const;

protected :

	void kill(
	);

	static Char * strDup(
		const	Char *		src,
		const	Uint32		len = (Uint32)-1
	);

	static Exception * excDup(
		const	Exception *	src
	);

private :

	Char *		className;	///< Class name.
	Char *		errorName;	///< Error name.
	Char *		adMessage;	///< Additional message.
	Exception *	prevExcep;	///< Previous exception.
	mutable Char *	whatCache;	///< "what" message cache.

};

// ============================================================================

/// \brief Macro used to declare and define the inner LocalClassException of a
///	user class.
///
/// \param __CN__
///	The outer class name this LocalClassException class belongs to.
///
/// \ingroup BFC_Base

#define BASE_CLASS_GEN_EXCEPTION( __CN__ )				\
									\
class LocalClassException : public BFC::Exception {			\
									\
protected :								\
									\
	LocalClassException(						\
		const	BFC::Char *		en,			\
		const	BFC::Exception *	pe = 0			\
	) : BFC::Exception( __CN__, en, pe ) {}				\
									\
	LocalClassException(						\
		const	BFC::Char *		en,			\
		const	BFC::Exception *	pe,			\
		const	BFC::Char *		am			\
	) : BFC::Exception( __CN__, en, pe, am ) {}			\
									\
	LocalClassException(						\
		const	BFC::Char *		en,			\
		const	BFC::Exception *	pe,			\
		const	BFC::Buffer &		am			\
	) : BFC::Exception( __CN__, en, pe, am ) {}			\
									\
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
/// \ingroup BFC_Base

#define CLASS_EXCEPTION( __CN__, __EN__ )				\
									\
class __CN__ : public LocalClassException {				\
									\
public :								\
									\
	__CN__(								\
	) : LocalClassException( __EN__ ) {}				\
									\
	__CN__(								\
		const	BFC::Char *		am			\
	) : LocalClassException( __EN__, 0, am ) {}			\
									\
	__CN__(								\
		const	BFC::Buffer &		am			\
	) : LocalClassException( __EN__, 0, am ) {}			\
									\
	__CN__(								\
		const	BFC::Exception &	p			\
	) : LocalClassException( __EN__, & p ) {}			\
									\
	__CN__(								\
		const	BFC::Exception &	p,			\
		const	BFC::Buffer &		am			\
	) : LocalClassException( __EN__, & p, am ) {}			\
									\
}

// ============================================================================

/// \brief Macro used to declare and define an Exception at the global scope.
///
/// \param __DN__
///	The DLL define that should be used as prefix (ex: BFC_Light_DLL).
///
/// \param __CN__
///	The class name of this Exception (ex: OutOfRange).
///
/// \ingroup BFC_Base

#define GLOBAL_CLASS_EXCEPTION( __DN__, __CN__ )			\
									\
class __DN__ __CN__ : public BFC::Exception {				\
									\
public :								\
									\
	__CN__(								\
	) : BFC::Exception(						\
		"<global>", #__CN__, 0 ) {}				\
									\
	__CN__(								\
		const	BFC::Char *		a			\
	) : BFC::Exception(						\
		"<global>", #__CN__, 0, a ) {}				\
									\
	__CN__(								\
		const	BFC::Buffer &		a			\
	) : BFC::Exception(						\
		"<global>", #__CN__, 0, a ) {}				\
									\
	__CN__(								\
		const	BFC::Exception &	p			\
	) : BFC::Exception(						\
		"<global>", #__CN__, &p ) {}				\
									\
	__CN__(								\
		const	BFC::Exception &	p,			\
		const	BFC::Buffer &		a			\
	) : BFC::Exception(						\
		"<global>", #__CN__, &p, a ) {}				\
									\
}

// ============================================================================

/// \brief Predefined "AlreadyInitialized" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, AlreadyInitialized );

/// \brief Predefined "AssertionFailed" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, AssertionFailed );

/// \brief Predefined "BadCast" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, BadCast );

/// \brief Predefined "InternalError" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, InternalError );

/// \brief Predefined "InvalidArgument" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, InvalidArgument );

/// \brief Predefined "NotImplemented" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, NotImplemented );

/// \brief Predefined "NotInitialized" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, NotInitialized );

/// \brief Predefined "NullPointer" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, NullPointer );

/// \brief Predefined "OutOfRange" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, OutOfRange );

/// \brief Predefined "OutOfMemory" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, OutOfMemory );

/// \brief Predefined "ThreadStopped" global exception.
///
/// \ingroup BFC_Base

GLOBAL_CLASS_EXCEPTION( BFC_Light_DLL, ThreadStopped );

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Exception_H_

// ============================================================================

