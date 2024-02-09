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
//	BFC.STL.SMap.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_STL_SMap_H_
#define _BFC_STL_SMap_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.STL.SMapPtr.h"

// ============================================================================

#include "BFC.Base.ComparablePtr.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace STL {

// ============================================================================

/// \brief A container mapping Comparable's onto SObject's.
///
/// \ingroup BFC_STL

class BFC_Light_DLL SMap : virtual public SObject {

public :

	/// \brief Array of keys type.

	typedef Array< ComparableCPtr >	KeyArrayType;

	/// \brief Array of Values type.

	typedef Array< SPtr >		ValArrayType;

	/// \brief Creates a new empty map.

	SMap(
	) {
	}

	/// \brief Returns a const reference to the value associated to key \a
	///	pKey.
	///
	/// \throws InvalidArgument
	///	If key \a pKey doesn't exist yet.

	const SPtr & operator [] (
		const	ComparableCPtr &
					pKey
	) const;

	/// \brief Returns a reference to the value associated to key \a pKey.
	///
	/// \throws InvalidArgument
	///	If key \a pKey doesn't exist yet.

	SPtr & operator [] (
		const	ComparableCPtr &
					pKey
	);

	/// \brief Returns a const reference to the value associated to key \a
	///	pKey, or \a pDft if \a pKey is not associated to a value.

	const SPtr & get(
		const	ComparableCPtr &
					pKey,
		const	SPtr &		pDft
	) const;

	/// \brief Returns a reference to the value associated to key \a pKey.
	///
	/// If the key doesn't exist yet, it is first added.

	SPtr & getOrAdd(
		const	ComparableCPtr &
					pKey
	);

	/// \brief Returns the number or pairs (key, value) contained in this
	///	map.

	Uint32 getSize(
	) const {
		return ( keys.getSize() );
	}

	/// \brief Returns all the keys of this map.

	const KeyArrayType & getAllKeys(
	) const {
		return keys;
	}

	/// \brief Returns all the values of this map.

	const ValArrayType & getAllValues(
	) const {
		return vals;
	}

	/// \brief Returns the key whose index is \a pIndex.
	///
	/// \throws InvalidArgument
	///	If \a pIndex is greater than or equal to the size of this map
	///	(see getSize()).

	const ComparableCPtr & getKey(
		const	Uint32		pIndex
	) const;

	/// \brief Returns a const reference to the value whose index is \a
	///	pIndex.
	///
	/// \throws InvalidArgument
	///	If \a pIndex is greater than or equal to the size of this map
	///	(see getSize()).

	const SPtr & getValue(
		const	Uint32		pIndex
	) const;

	/// \brief Returns a reference to the value whose index is \a pIndex.
	///
	/// \throws InvalidArgument
	///	If \a pIndex is greater than or equal to the size of this map
	///	(see getSize()).

	SPtr & getValue(
		const	Uint32		pIndex
	);

	/// \brief Returns the first key whose associated value is \a pValue.
	///
	/// \throws InvalidArgument
	///	If no key is associated to this value.

	const ComparableCPtr & keyOf(
		const	SPtr &		pValue
	) const;

	/// \brief Returns true iff this map doesn't contain any pair yet.

	Bool isEmpty(
	) const {
		return ( keys.isEmpty() );
	}

	/// \brief Returns true iff this map contains an association for
	///	key \a pKey.

	Bool contains(
		const	ComparableCPtr &
					pKey
	) const {
		Uint32 dummy;
		return ( lookup( pKey, dummy ) );
	}

	/// \brief Returns true iff this map contains an association for
	///	key \a pKey.
	///
	/// If this is the case, \a pPos is set to the index of this
	/// association.

	Bool contains(
		const	ComparableCPtr &
					pKey,
			Uint32 &	pPos
	) const {
		return ( lookup( pKey, pPos ) );
	}

	/// \brief Adds a new association (\a pKey, \a pVal).
	///
	/// \throws InvalidArgument
	///	If \a pKey is already used.

	void add(
		const	ComparableCPtr &
					pKey,
		const	SPtr &		pVal = SPtr()
	);

	/// \brief Sets the value of \a pKey to \a pVal.
	///
	/// If \a pKey does not exist, it is first created.

	void setOrAdd(
		const	ComparableCPtr &
					pKey,
		const	SPtr &		pVal
	);

	/// \brief Removes the association identified by key \a pKey.
	///
	/// \throws InvalidArgument
	///	If there is no association identified by key \a pKey.

	void del(
		const	ComparableCPtr &
					pKey
	);

	/// \brief Removes the association identified by index \a pIndex.
	///
	/// \throws InvalidArgument
	///	If \a pIndex is greater than or equal to the size of this map
	///	(see getSize()).

	void pop(
		const	Uint32		pIndex
	);

	/// \brief Removes all associations from this map.

	void kill(
	) {
		keys.kill();
		vals.kill();
	}

protected :

	/// \brief Finds the association identified by key \a pKey.
	///
	/// \param pKey
	///	The key identifying the association to find.
	///
	/// \param pIndex
	///	The position of the association if found, or the position where
	///	to insert a new association identified by key \a pKey.
	///
	/// \returns
	///	True if an association was found, false otherwise.

	Bool lookup(
		const	ComparableCPtr &
					pKey,
			Uint32 &	pIndex
	) const;

private :

	KeyArrayType	keys;	///< Array of keys.
	ValArrayType	vals;	///< Array of values.

	/// \brief Forbidden copy constructor.

	SMap(
		const	SMap &		pOther
	);

	/// \brief Forbidden copy operator.

	SMap & operator = (
		const	SMap &		pOther
	);

};

// ============================================================================

} // namespace STL
} // namespace BFC

// ============================================================================

#endif // _BFC_STL_SMap_H_

// ============================================================================

