// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::Dyn".
//
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.Dyn.Var.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Dyn_Var_H_
#define _BFC_Dyn_Var_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VarPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Cloneable.h"
#include "BFC.Base.UUId.h"

#include "BFC.Dyn.TypePtr.h"

#include "BFC.IO.Serializable.h"

#include "BFC.Msg.Invokable.h"
#include "BFC.Msg.Observable.h"
#include "BFC.Msg.Observer.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Abstract base class of all direct BFC "dynamic variables".
///
/// A dynamic variable is an object that works like an ordinary variable, but
/// that also implements some access patterns like BFC::Cloneable or
/// BFC::Msg::Invokable.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL Var :

	virtual public Cloneable,
	virtual public IO::Serializable,
	virtual public Msg::Invokable,
	virtual public Msg::Observable,
	virtual public Msg::Observer {

protected :

	Var(
			TypeCPtr	pType,
		const	Buffer &	pName = Buffer()
	);

	/// \brief Copy constructor.

	Var(
		const	Var &		pOther
	);

	/// \brief Copy operator.
	///
	/// The default implementation barely checks this Var is not locked and
	///	the types match.

	Var & operator = (
		const	Var &		pOther
	);

public :

	const TypeCPtr & getType(
	) const {
		return type;
	}

	/// \brief Returns the child element selected by \a pTag (if any).

	virtual VarPtr sub(
		const	Buffer &	pTag
	);

	/// \brief Sets the value of the child element selected by \a pTag
	///	to \a pVal.

	virtual void set(
		const	Buffer &	pTag,
		const	Buffer &	pVal
	);

	/// \brief Returns the value of the child element selected by \a pTag.

	virtual Buffer get(
		const	Buffer &	pTag
	) const;

	/// \brief Gives this Var the name \a pName.

	void setName(
		const	Buffer &	pName
	) {
		name = pName;
	}

	/// \brief Returns the name of this Var.
	///
	/// The default name is an empty string.

	const Buffer & getName(
	) const {
		return name;
	}

	virtual Msg::ResultPtr handleRequest(
		const	Msg::RequestCPtr &	pRequest
	);

	virtual void doSerialize(
			IO::ByteOutputStreamPtr	pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr	pIStream
	);

	/// \brief Returns a human-readable representation of the value of this
	///	object.
	///
	/// The output of this method doesn't loose any precision, so that
	/// reinjecting it in the fromBuffer() method recreates the exact
	/// original value.
	
	virtual Buffer toBuffer(
	) const = 0;
	
	/// \brief Converts the human-readable string \a pBuffer into the
	///	internal representation.
	
	virtual void fromBuffer(
		const	Buffer &	pBuffer
	) = 0;


protected :

	static void splitTag(
		const	Buffer &	pTag,
			Buffer &	pPrefix,
			Buffer &	pSuffix
	);

	static void splitCompact(
		const	Buffer &	pCompact,
			BufferArray &	pExpanded
	);

	static void skipWS(
		const	Buffer &	pBuffer,
			Uint32 &	pIndex
	);

	static Uint32 findClosingChar(
		const	Buffer &	b1,
		const	Uint32		start
	);
	
	static Uint32 findClosingQuote(
		const	Buffer &	b1,
		const	Uint32		start
	);

private :

	TypeCPtr	type;		///< The type of this variable (ex. TInt).
	Buffer		name;		///< The name of this variable (ex. "bitrate").

	/// \brief Forbidden default constructor.

	Var(
	);

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_Var_H_

// ============================================================================

