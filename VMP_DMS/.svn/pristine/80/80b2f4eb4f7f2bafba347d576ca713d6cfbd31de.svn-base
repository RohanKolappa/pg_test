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
//	BFC.Dyn.TEnum.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_TEnum_H_
#define _BFC_Dyn_TEnum_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.TEnumPtr.h"

// ============================================================================

#include "BFC.Dyn.Type.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Type representing an ordered array of Buffer values.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL TEnum : public Type {

public :

	/// \brief Creates a new empty TEnum.

	TEnum(
	);

	/// \brief Creates a new TEnum containing all strings from \a pTable.

	TEnum(
		const	char *		pTable[]
	);

	/// \brief Creates a copy of \a pOther.

	TEnum(
		const	TEnum &		pOther
	);

	/// \brief Copies \a pOther.

	TEnum & operator = (
		const	TEnum &		pOther
	) {
		Type::operator = ( pOther );
		t = pOther.t;
		return *this;
	}

	virtual SPtr clone(
	) const;

	virtual VarPtr create(
	);

//	virtual Buffer toBuffer(
//	) const;
//
//	virtual void fromBuffer(
//		const	Buffer &	pBuffer
//	);

//	void setValueCB(
//			Msg::EventCPtr	pEvent
//	);

	Uint32 getSize(
	) const {
		return t.getSize();
	}

	const Buffer & getEntry(
		const	Uint32		pIndex
	) const {
		return t[ pIndex ];
	}

	Uint32 getEntryIndex(
		const	Buffer &	pBuffer
	) const {
		return t.indexOf( pBuffer );
	}

	void addEntry(
		const	Buffer &	pBuffer
	);

	void delEntries(
	);

//	virtual void syncObservers(
//	) const;

private :

	BufferArray	t;	///< Array of buffer entries.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_TEnum_H_

// ============================================================================

