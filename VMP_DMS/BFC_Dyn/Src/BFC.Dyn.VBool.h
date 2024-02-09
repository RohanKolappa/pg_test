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
//	BFC.Dyn.VBool.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VBool_H_
#define _BFC_Dyn_VBool_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VBoolPtr.h"

// ============================================================================

#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Var taking a Bool value.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VBool : public Var {

public :

	/// \brief Creates a new VBool.

	VBool(
	);

	/// \brief Creates a copy of \a pBool.

	VBool(
		const	Bool		pBool
	);

	/// \brief Creates a copy of \a pBool.

	VBool(
		const	Buffer &	pVarName,
		const	Bool		pBool
	);

	/// \brief Creates a copy of \a pOther.

	VBool(
		const	VBool &		pOther
	);

	/// \brief Destroys this object.

	virtual ~VBool(
	);

	/// \brief Replaces the content of this object by a copy of \a pOther.

	VBool & operator = (
		const	VBool &		pOther
	);

	virtual SPtr clone(
	) const;

	virtual void doSerialize(
			IO::ByteOutputStreamPtr	pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr	pIStream
	);

	virtual Buffer toBuffer(
	) const;

	virtual void fromBuffer(
		const	Buffer &	pBuffer
	);

	Bool getValue(
	) const {
		return v;
	}

	void setValue(
		const	Bool		pValue
	);

	void setValue(
		const	Buffer &	pBuffer
	);

	VBool & operator = (
		const	Bool		pValue
	) {
		setValue( pValue );
		return *this;
	}

	VBool & operator = (
		const	Buffer &	pBuffer
	) {
		setValue( pBuffer );
		return *this;
	}

	operator Bool() {
		return v;
	}

	virtual void syncObservers(
	) const;

protected :

	/// \brief Callback handling Msg::ValueEvent events (id = AnyId).
	///
	/// The Msg::ValueEvent event can be of any subtype (Buffer, Bool, ...).

	void setValueCB(
			Msg::EventCPtr	pEvent
	);

	void privSetValue(
		const	Bool		pValue
	);

	void privSetValue(
		const	Buffer &	pBuffer
	);

private :

	Bool		v;	///< Value of this variable.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VBool_H_

// ============================================================================

