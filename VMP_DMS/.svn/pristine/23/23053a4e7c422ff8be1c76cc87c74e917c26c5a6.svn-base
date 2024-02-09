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
//	BFC.Dyn.VArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VArray_H_
#define _BFC_Dyn_VArray_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VArrayPtr.h"

// ============================================================================

#include "BFC.Dyn.TArray.h"
#include "BFC.Dyn.Var.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Represents an array of Var objects.
///
/// The type of the child elements is determined once and for all at
/// construction time. It is not possible to modify it during the lifetime.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VArray : public Var {

public :

	/// \brief Creates an empty VArray of type \a pTArray.
	///
	/// \param pTArray
	///	The type of this VArray.

	VArray(
			TArrayPtr	pTArray
	);

	/// \brief Creates a copy of \a pOther.
	///
	/// \param pOther
	///	The VArray to copy.

	VArray(
		const	VArray &	pOther
	);

	/// \brief Destroys this object.

	virtual ~VArray(
	);

	/// \brief Copy operator.

	VArray & operator = (
		const	VArray &	pOther
	);

	virtual VarPtr sub(
		const	Buffer &	pTag
	);

	virtual void set(
		const	Buffer &	pTag,
		const	Buffer &	pVal
	);

	virtual Buffer get(
		const	Buffer &	pTag
	) const;

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

	Uint32 getSize(
	) const;

	void setSize(
		const	Uint32		pSize
	);

	VarPtr getItem(
		const	Uint32		pIndex
	);

	VarCPtr getItem(
		const	Uint32		pIndex
	) const;

	virtual void syncObservers(
	) const;

protected :

	enum {
		EventCBId	= AnyId,
		ChildCBId
	};

	void eventCB(
			Msg::EventCPtr	pEvent
	);

	void childCB(
			Msg::EventCPtr	pEvent
	);

	void privCopy(
		const	VArray &	pOther
	);

	void privResize(
		const	Uint32		pNewSize
	);

private :

	TypePtr			s;	///< Type of child elements.
	CompArray< VarPtr >	v;	///< Child elements array.
	Bool			r;	///< Busy resizing array ?

	/// \brief Forbidden default constructor.

	VArray(
	);

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VArray_H_

// ============================================================================

