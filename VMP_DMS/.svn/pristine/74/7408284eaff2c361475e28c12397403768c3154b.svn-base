// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::STL".
// 
// "BFC::STL" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::STL" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::STL"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.STL.SArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_STL_SArray_H_
#define _BFC_STL_SArray_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.STL.SArrayPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace STL {

// ============================================================================

/// \brief A fast and secure array of SObject/SPtr.
///
/// \ingroup BFC_STL

class BFC_Light_DLL SArray : virtual public SObject {

public :

	/// \brief Creates a new SArray of size \a pSize
	///	(default: 0).

	SArray(
		const	Uint32		pSize = 0
	);

	/// \brief Creates a copy of \a pOther.

	SArray(
		const	SArray &	pOther
	);

	/// \brief Creates a copy of \a pOther.

	SArray(
		const	SArray &	pOther,
		const	Uint32		pStart
	);

	/// \brief Creates a copy of \a pOther.

	SArray(
		const	SArray &	pOther,
		const	Uint32		pStart,
		const	Uint32		pCount
	);

	/// \brief Destroys this object.

	virtual ~SArray(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a pOther.

	SArray & operator = (
		const	SArray &	pOther
	);

	/// \brief Returns true iff this object is empty.

	Bool isEmpty(
	) const {
		return ( log == 0 );
	}

	/// \brief Returns the number of elements in this object.

	Uint32 getSize(
	) const {
		return log;
	}

	/// \brief Returns a subarray of this object.
	///
	/// This method constructs a new SArray object, containing only the
	/// elements of this array starting at position \a pStart.
	///
	/// If \a pStart is outside of the internal buffer, an empty object
	/// is returned.

	SArray operator () (
		const	Uint32		pStart
	) const;

	/// \brief Returns a subarray of this object.
	///
	/// This method constructs a new SArray object, containing only the
	/// elements of this array starting at position \a pStart, and of length
	/// \a pLength.
	///
	/// If \a pStart is outside of the internal buffer, an empty object
	/// is returned.
	///
	/// If there are less than \a pCount elements to be copied,
	/// the new array will contain all elements starting
	/// from position \a pStart, up to the last element in the internal
	/// buffer.

	SArray operator () (
		const	Uint32		pStart,
		const	Uint32		pCount
	) const;

	/// \brief Appends a copy of \a pElt at the end of this object.

	void operator += (
		const	SPtr &		pElt
	) {
		addLast( pElt );
	}

	/// \brief Appends a copy of \a pOther at the end of this object.

	void operator += (
		const	SArray &	pOther
	) {
		insert( log, pOther );
	}

	/// \brief Appends a copy of \a pElt at the end of this object.

	void add(
		const	SPtr &		pElt,
		const	Bool		pAllowDup = true
	) {
		addLast( pElt, pAllowDup );
	}

	/// \brief Inserts a copy of \a pElt at the begin of this object.

	void addFirst(
		const	SPtr &		pElt,
		const	Bool		pAllowDup = true
	) {
		insert( 0, pElt, pAllowDup );
	}

	/// \brief Synonym of add.

	void addLast(
		const	SPtr &		pElt,
		const	Bool		pAllowDup = true
	) {
		insert( log, pElt, pAllowDup );
	}

	/// \brief Inserts a copy of \a pElt at position \a pIdx.
	///
	/// All elements (if any), starting from index \a pIdx, are moved one
	/// step to the "right" (in the direction of increasing indexes), to
	/// make room for the new element.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than the size of this object.

	void insert( 
		const	Uint32		pIdx,
		const	SPtr &		pElt,
		const	Bool		pAllowDup = true
	 );

	/// \brief Inserts a copy of \a pOther at position \a pIdx.
	///
	/// All elements (if any), starting from index \a pIdx, are moved
	/// pOther.getSize()
	/// steps to the "right" (in the direction of increasing indexes), to
	/// make room for the new elements.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than the size of this object.

	void insert(
		const	Uint32		pIdx,
		const	SArray &	pOther,
		const	Bool		pAllowDup = true
	 );

	/// \brief Removes the first (if \a pAll is false, which is the
	///	default) or all (if \a pAll is true) occurrence(s) (if any
	///	occurrence at all) of \a pElt from this object.

	void remove(
		const	SPtr &		pElt,
		const	Bool		pAll = false
	);

	/// \brief Removes element whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	SPtr pop(
		const	Uint32		pIdx
	);

	/// \brief Removes first element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	SPtr popFirst(
	) {
		return pop( 0 );
	}

	/// \brief Removes first \a pNbr elements.

	void popFirst(
		const	Uint32		pNbr
	);

	/// \brief Removes last element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	SPtr popLast(
	) {
		return pop( log - 1 );
	}

	/// \brief Destroys the content of this object.

	void kill(
	);

	/// \brief Resizes this object to new size \a pSize.
	///
	/// If `pSize' is 0, kills this object.
	///
	/// Otherwise, if this object is empty, creates a new SArray that contains
	/// pSize elements of type T.
	///
	/// Otherwise, if the actual size of this object is less than pSize,
	/// then reallocates memory in order for this object to contain
	/// pSize objects (the actual content is NOT destroyed).
	///
	/// Otherwise, reduces the size of this object by deleting the last
	/// elements.

	void resize(
		const	Uint32		pSize
	);

	/// \brief Returns a reference to element at position pIdx.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	const SPtr & operator [] (
		const	Uint32		pIdx
	) const {
		checkIndex( pIdx );
		return ptr[ pIdx ];
	}

	/// \brief Returns a reference to element at position pIdx.
	///
	/// This reference can be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	SPtr & operator [] (
		const	Uint32		pIdx
	) {
		checkIndex( pIdx );
		return ptr[ pIdx ];
	}

	const SPtr & get(
		const	Uint32		pIdx
	) const {
		checkIndex( pIdx );
		return ptr[ pIdx ];
	}

	SPtr & get(
		const	Uint32		pIdx
	) {
		checkIndex( pIdx );
		return ptr[ pIdx ];
	}

	const SPtr & getFirst(
	) const {
		checkIndex( 0 );
		return ptr[ 0 ];
	}

	SPtr & getFirst(
	) {
		checkIndex( 0 );
		return ptr[ 0 ];
	}

	const SPtr & getLast(
	) const {
		checkIndex( log - 1 );
		return ptr[ log - 1 ];
	}

	SPtr & getLast(
	) {
		checkIndex( log - 1 );
		return ptr[ log - 1 ];
	}

	/// \brief Returns the position of the first occurrence of \a pElt
	///	within this object, or -1 if there is no such occurrence.

	Uint32 indexOf(
		const	SPtr &		pElt
	) const {
		Uint32	pos;
		return ( contains( pElt, pos ) ? pos : ( Uint32 )-1 );
	}

	/// \brief Returns true iff this object contains an occurrence of
	///	\a pElt.

	Bool contains(
		const	SPtr &		pElt
	) const {
		Uint32	dummy;
		return contains( pElt, dummy );
	}

	/// \brief Returns true iff this object contains an occurrennce of
	///	\a pElt, in which case \a pIndex is updated with the index
	///	of this occurrence.

	Bool contains(
		const	SPtr &		pElt,
			Uint32 &	pIndex
	) const;

protected :

	void checkIndex(
		const	Uint32		pIndex
	) const {
		if ( pIndex >= log ) {
			throwOutOfRange( pIndex );
		}
	}

	static Uint32 getLen(
		const	Uint32		pLength
	);

	void throwOutOfRange(
		const	Uint32		pIndex
	) const;

	void throwDuplicate(
	) const;

	/// \brief [Re]initializes this object by copying at most \a pCount
	///	elements starting at position \a pStart from \a pOther.

	void doCopy(
		const	SArray &	pOther,
		const	Uint32		pStart,
		const	Uint32		pCount
	);

	/// \brief Inserts \a pCount elements at position \a pStart.

	void insertHole(
		const	Uint32		pStart,
		const	Uint32		pCount
	);

	/// \brief Removes \a pCount elements from position \a pStart.

	void deleteHole(
		const	Uint32		pStart,
		const	Uint32		pCount
	);

private :

	SPtr *		ptr;	///< Pointers array.
	Uint32		log;	///< Logically used.
	Uint32		phy;	///< Physically allocated.

};

// ============================================================================

} // namespace STL
} // namespace BFC

// ============================================================================

#endif // _BFC_STL_SArray_H_

// ============================================================================

