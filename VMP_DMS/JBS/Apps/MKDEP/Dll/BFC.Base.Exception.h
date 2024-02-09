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
//	BFC.Base.Exception.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Exception_H_
#define _BFC_Exception_H_

// ============================================================================

#include <exception>

#include "BFC.Base.Buffer.h"

// ============================================================================

namespace BFC {

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
/// error condition that has been encountered (IndexOutOfRange, ...).
/// But, in the case
/// an exception is the result of a failed call to some other
/// function, it should be possible to attach a complementary
/// explanation to the exception being thrown. This is exactly what the
/// BFC exceptions are for.
///
/// \ingroup Common_Base

class JBS_DLL_NONE Exception : public std::exception {

public :

	Exception(
	);

	Exception(
		const	Char		*cn,	///< class name
		const	Char		*en,	///< error name
		const	Char		*am,	///< additional message
		const	Exception	*p	///< previous exception
	);

	Exception(
		const	Char		*cn,	///< class name
		const	Char		*en,	///< error name
		const	Buffer&		am,	///< additional message
		const	Exception	*p	///< previous exception
	);

	Exception(
		const	Exception&	src
	);

	virtual ~Exception(
	) throw (
	);

	Exception& operator = (
		const	Exception&	src
	);

	/// \brief Returns a human-readable description of this exception.

	virtual const Char *what(
	);

	const Char *getClassName(
	) const {
		return className;
	}

	const Char *getErrorName(
	) const {
		return errorName;
	}

	const Char *getAdMessage(
	) const {
		return adMessage;
	}

	const Exception *getPrevExcep(
	) const {
		return prevExcep;
	}

protected :

	void kill(
	);

	static Char *strDup(
		const	Char		*src,
		const	Uint32		len = (Uint32)-1
	);

	static Exception *excDup(
		const	Exception	*src
	);

private :

	Char		*className;
	Char		*errorName;
	Char		*adMessage;
	Exception	*prevExcep;
	Char		*whatCache;

};

// ============================================================================

#define BASE_CLASS_GEN_EXCEPTION( __CN__ )				\
class LocalClassException : public BFC::Exception {			\
public :								\
	virtual ~LocalClassException() throw () {}			\
protected :								\
	LocalClassException(						\
		const	BFC::Char	*en,				\
		const	BFC::Char	*am,				\
		const	BFC::Exception	*pe				\
	) : BFC::Exception( __CN__, en, am, pe ) {}			\
	LocalClassException(						\
		const	BFC::Char	*en,				\
		const	BFC::Buffer&	am,				\
		const	BFC::Exception	*pe				\
	) : BFC::Exception( __CN__, en, am, pe ) {}			\
}

// ============================================================================

#define CLASS_EXCEPTION( __CN__, __EN__ )				\
class __CN__ : public LocalClassException {				\
public :								\
	virtual ~__CN__() throw () {}					\
	__CN__(								\
	) : LocalClassException( __EN__, ( const BFC::Char * )0, 0 ) {}	\
	__CN__(								\
		const	BFC::Exception&	p				\
	) : LocalClassException( __EN__, ( const BFC::Char * )0, &p ) {}\
	__CN__(								\
		const	BFC::Char	*am				\
	) : LocalClassException( __EN__, am, 0 ) {}			\
	__CN__(								\
		const	BFC::Buffer&	am				\
	) : LocalClassException( __EN__, am, 0 ) {}			\
	__CN__(								\
		const	BFC::Exception&	p,				\
		const	BFC::Char	*am				\
	) : LocalClassException( __EN__, am, &p ) {}			\
	__CN__(								\
		const	BFC::Exception&	p,				\
		const	BFC::Buffer&	am				\
	) : LocalClassException( __EN__, am, &p ) {}			\
}

// ============================================================================

#define GLOBAL_CLASS_EXCEPTION( __DN__, __CN__, __EN__ )			\
class __DN__ __CN__ : public BFC::Exception {				\
public :								\
	__CN__(								\
	) : BFC::Exception(						\
		"<global>", __EN__, ( const BFC::Char * )0, 0 ) {}	\
	__CN__(								\
		const	BFC::Exception&	p				\
	) : BFC::Exception(						\
		"<global>", __EN__, ( const BFC::Char * )0, &p ) {}	\
	__CN__(								\
		const	BFC::Char	*am				\
	) : BFC::Exception(						\
		"<global>", __EN__, am, 0 ) {}				\
	__CN__(								\
		const	BFC::Buffer&	a				\
	) : BFC::Exception(						\
		"<global>", __EN__, a, 0 ) {}				\
	__CN__(								\
		const	BFC::Exception&	p,				\
		const	BFC::Char	*am				\
	) : BFC::Exception(						\
		"<global>", __EN__, am, &p ) {}				\
	__CN__(								\
		const	BFC::Exception&	p,				\
		const	BFC::Buffer&	a				\
	) : BFC::Exception(						\
		"<global>", __EN__, a, &p ) {}				\
	virtual ~__CN__() throw () {}					\
}

// ============================================================================

GLOBAL_CLASS_EXCEPTION( JBS_DLL_NONE, ThreadStopped, "Thread stopped" );

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Exception_H_

// ============================================================================


