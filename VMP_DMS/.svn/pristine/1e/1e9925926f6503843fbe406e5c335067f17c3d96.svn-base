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
//	BFC.Dyn.VStruct.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VStruct_H_
#define _BFC_Dyn_VStruct_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"
	
// ============================================================================

#include "BFC.Dyn.VStructPtr.h"

// ============================================================================

#include "BFC.Dyn.TStruct.h"
#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Represents a struct of Var objects.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VStruct : public Var {

public :

	/// \brief Creates a new VStruct of type \a pTStruct.
	///
	/// \param pTStruct
	///	The type of this VStruct.

	VStruct(
			TStructPtr	pTStruct
	);

	/// \brief Creates a copy of \a pOther.
	///
	/// \param pOther
	///	The VStruct to copy.

	VStruct(
		const	VStruct &	pOther
	);

	/// \brief Destroys this object.

	virtual ~VStruct(
	);

	/// \brief Copy operator.

	VStruct & operator = (
		const	VStruct &	pOther
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

	void privCreate(
	);

	void privDelete(
	);

	void privCopy(
		const	VStruct &	pOther
	);

private :

	typedef Map< Buffer, VarPtr >	MembersType;

	TStructPtr	t;	///< Type of this VStruct.
	MembersType	v;	///< Child elements array.

	/// \brief Forbidden default constructor.

	VStruct(
	);

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VStruct_H_

// ============================================================================

