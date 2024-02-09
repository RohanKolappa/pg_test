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
//	BFC.Dyn.VEnum.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VEnum_H_
#define _BFC_Dyn_VEnum_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VEnumPtr.h"

// ============================================================================

#include "BFC.Dyn.TEnum.h"
#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Var representing a specific entry in an ordered array of Buffer
///	values.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VEnum : public Var {

public :

	/// \brief Creates a new VEnum.

	VEnum(
	);

	/// \brief Creates a new VEnum.

	VEnum(
		const	Buffer &	pVarName
	);

	/// \brief Creates a new VEnum.

	VEnum(
		const	char *		pTable[]
	);

	/// \brief Creates a new VEnum.

	VEnum(
		const	Buffer &	pVarName,
		const	char *		pTable[]
	);

	/// \brief Creates a new VEnum.

	VEnum(
		const	TEnumCPtr &	pTEnum
	);

	/// \brief Creates a new VEnum.

	VEnum(
		const	Buffer &	pVarName,
		const	TEnumCPtr &	pTEnum
	);

	/// \brief Creates a copy of \a pOther.

	VEnum(
		const	VEnum &		pOther
	);

	virtual ~VEnum(
	);

	/// \brief Copies \a pOther.

	VEnum & operator = (
		const	VEnum &		pOther
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

	Uint32 getValue(
	) const {
		return v;
	}

	operator Uint32() const {
		return v;
	}

	void setValue(
		const	Uint32		pValue
	);

	void setValue(
		const	Buffer &	pBuffer
	);

	VEnum & operator = (
		const	Uint32		pValue
	) {
		setValue( pValue );
		return *this;
	}

	VEnum & operator = (
		const	Buffer &	pBuffer
	) {
		setValue( pBuffer );
		return *this;
	}

	void setValueCB(
			Msg::EventCPtr	pEvent
	);

	Uint32 getSize(
	) const {
		return t->getSize();
	}

	const Buffer & getEntry(
		const	Uint32		pIndex
	) const {
		return t->getEntry( pIndex );
	}

	Uint32 getEntryIndex(
		const	Buffer &	pBuffer
	) const {
		return t->getEntryIndex( pBuffer );
	}

	const Buffer & getBuffer(
	) const {
		return t->getEntry( v );
	}

	virtual void syncObservers(
	) const;

protected :

	void privSetValue(
		const	Uint32		pValue
	);

	void privSetValue(
		const	Buffer &	pBuffer
	);

private :

	TEnumCPtr	t;
	Uint32		v;	///< Value of this variable.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VEnum_H_

// ============================================================================

