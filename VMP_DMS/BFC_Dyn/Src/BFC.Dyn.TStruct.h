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
//	BFC.Dyn.TStruct.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_TStruct_H_
#define _BFC_Dyn_TStruct_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

BFC_PTR_DECL( BFC_Dyn_DLL, TStruct )

} // namespace Dyn
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Dyn.Type.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Represents a Dynamic Structure.
///
/// A TStruct contains 0, 1, or more items identified by a name or an index.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL TStruct : public Type {

public :

	/// \brief Creates a new unlocked TStruct.

	TStruct(
	);

	/// \brief Creates an unlocked copy of \a pOther.
	///
	/// \param pOther
	///	The TStruct to copy.

	TStruct(
		const	TStruct &	pOther
	);

	/// \brief Copies \a pOther, cloning the type of child elements (if
	///	any).
	///
	/// \param pOther
	///	TStruct to copy.
	///
	/// \throws Locked
	///	If this object is locked.

	TStruct & operator = (
		const	TStruct &	pOther
	);

	virtual SPtr clone(
	) const;

	virtual VarPtr create(
	);

	/// \brief Locks this Type, and all items it contains.

	virtual void setLocked(
	);

	/// \brief Returns the number of items this TStruct contains.

	Uint32 getNbrMembers(
	) const {
		return members.getSize();
	}

	/// \brief Returns the name of the item at position \a pIndex.
	///
	/// \param pIndex
	///	Position of the item to return the name from.
	///
	/// \throws Map::OutOfRange
	///	If \a pIndex is an invalid index.

	const Buffer & getMemberName(
		const	Uint32		pIndex
	) const {
		return members.getKey( pIndex );
	}

	/// \brief Returns the type of the item at position \a pIndex.
	///
	/// \param pIndex
	///	Position of the item to return the type from.
	///
	/// \throws Map::OutOfRange
	///	If \a pIndex is an invalid index.

	TypePtr getMemberType(
		const	Uint32		pIndex
	) const {
		return members.getValue( pIndex );
	}

	/// \brief Returns the type of the item with name \a pMemberName.
	///
	/// \param pMemberName
	///	Name of the item to return the type from.
	///
	/// \throws Map::NoSuchKey
	///	If there is no item with name \a pMemberName.

	TypeCPtr getMember(
		const	Buffer &	pMemberName
	) const {
		return members[ pMemberName ];
	}

	/// \brief Adds a new item to this TStruct.
	///
	/// \param pMemberName
	///	Unique name of this item.
	///
	/// \param pMemberType
	///	Type of this item.
	///
	/// \throws Locked
	///	If this TStruct is locked.
	///
	/// \throws InvalidArgument
	///	If this TStruct already contains an item with name \a
	///	pMemberName.

	void addMember(
		const	Buffer &	pMemberName,
			TypePtr		pMemberType
	);

	/// \brief Modifies an existing item in this TStruct.
	///
	/// \param pMemberName
	///	Unique name of this item.
	///
	/// \param pMemberType
	///	New Type of this item.
	///
	/// \throws Locked
	///	If this TStruct is locked.
	///
	/// \throws InvalidArgument
	///	If this TStruct doesn't already contain an item with name \a
	///	pMemberName.

	void setMember(
		const	Buffer &	pMemberName,
			TypePtr		pMemberType
	);

protected :

private :

	typedef Map< Buffer, TypePtr >	MembersType;

	MembersType	members;	///< Members.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_TStruct_H_

// ============================================================================

