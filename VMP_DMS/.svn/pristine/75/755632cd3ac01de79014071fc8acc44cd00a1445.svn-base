// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.TL.CompArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_CompArray_H_
#define _BFC_TL_CompArray_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtrTmpl.h"

// ============================================================================

namespace BFC {

BFC_PTR_TMPL( CompArray )

} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.ArrayBase.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A fast and secure array of comparable elements of type \b T.
///
/// \ingroup BFC_TL

template < typename T >
class CompArray : public ArrayBase {

public :

	/// \brief Creates a new CompArray of size \a pSize
	///	(default: 0).

	CompArray(
		const	Uint32		pSize = 0
	);

	/// \brief Creates a new CompArray, which is a deep copy of array
	///	\a pOther.

	CompArray(
		const	CompArray< T > &
					pOther
	);

	/// \brief Destroys this object.

	virtual ~CompArray(
	);

	/// \brief Returns a subarray of this object.
	///
	/// This method constructs a new Array object, containing only the
	/// elements of this array starting at position \a start.
	///
	/// If \a start is outside of the internal buffer, an empty object
	/// is returned.

	CompArray< T > operator () (
		const	Uint32		start
	) const;

	/// \brief Returns a subarray of this object.
	///
	/// This method constructs a new Array object, containing only the
	/// elements of this array starting at position \a start, and of length
	/// \a length.
	///
	/// If \a start is outside of the internal buffer, an empty object
	/// is returned.
	///
	/// If there are less than \a length elements to be copied,
	/// the new array will contain all elements starting
	/// from position \a start, up to the last element in the internal
	/// buffer.

	CompArray< T > operator () (
		const	Uint32		start,
		const	Uint32		length
	) const;

	/// \brief Replaces the content of this object by a deep copy of
	///	\a pOther.

	CompArray< T > & operator = (
		const	CompArray< T > &
					pOther
	);

	/// \brief Returns true iff this object is identical to \a pOther.

	Bool operator == (
		const	CompArray< T > &
					pOther
	) const;

	/// \brief Returns true iff this object is different than \a pOther.

	Bool operator != (
		const	CompArray< T > &
					pOther
	) const;

	/// \brief Appends a copy of \a pElt at the end of this object.

	void add(
		const	T &		pElt
	);

	/// \brief Synonym of add.

	void addLast(
		const	T &		pElt
	);

	/// \brief Appends a copy of \a pOther at the end of this object.

	void add(
		const	CompArray< T > &
					pOther
	);

	/// \brief Appends a copy of \a pElt at the end of this object.

	CompArray< T > & operator += (
		const	T &		pElt
	);

	/// \brief Appends a copy of \a pOther at the end of this object.

	CompArray< T > & operator += (
		const	CompArray< T > &
					pOther
	);

	/// \brief Appends a copy of \a t at the end of this object.

	CompArray< T > & operator << (
		const	T &		t
	);

	/// \brief Inserts a copy of \a pElt at position \a pIndex.
	///
	/// All elements (if any), starting from index \a pIndex, are moved one
	/// step to the "right" (in the direction of increasing indexes), to
	/// make room for the new element.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than the size of this object.

	void insert( 
		const	Uint32		pIndex,
		const	T &		pElt
	);

	/// \brief Inserts a copy of \a pOther at position \a pIndex.
	///
	/// All elements (if any), starting from index \a pIndex, are moved
	/// pOther.getSize()
	/// steps to the "right" (in the direction of increasing indexes), to
	/// make room for the new elements.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than the size of this object.

	void insert( 
		const	Uint32		pIndex,
		const	CompArray< T > &
					pOther
	 );

	/// \brief Removes the first occurrence (if any) of \a pElt from
	///	this object.

	CompArray< T > & operator -= (
		const	T &		pElt
	);

	/// \brief Removes the first (if \a pAll is false, which is the
	///	default) or all (if \a pAll is true) occurrence(s) (if any
	///	occurrence at all) of \a pElt from this object.

	void remove(
		const	T &		pElt,
		const	Bool		pAll = false
	);

	/// \brief Removes element whose index is \a pIndex.
	///
	/// \returns
	///	A copy of the removed element.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than or equal to the size of this object.

	T pop(
		const	Uint32		pIndex
	);

	/// \brief Removes first element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	T popFirst(
	) {
		return pop( 0 );
	}

	/// \brief Removes last element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	T popLast(
	) {
		return pop( nbr - 1 );
	}

	/// \brief Destroys the content of this object.

	void kill(
	);

	/// \brief Resizes this object to new size \a pSize.
	///
	/// If `pSize' is 0, kills this object.
	///
	/// Otherwise, if this object is empty, creates a new Array that contains
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

	/// \brief Returns a reference to element at position pIndex.
	///
	/// This reference can be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than or equal to the size of this object.

	T & operator [] (
		const	Uint32		pIndex
	);

	/// \brief Returns a reference to element at position pIndex.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than or equal to the size of this object.

	const T & operator [] (
		const	Uint32		pIndex
	) const;

	/// \brief Returns a reference to element at position pIndex.
	///
	/// This reference can be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than or equal to the size of this object.

	T & elt(
		const	Uint32		pIndex
	);

	/// \brief Returns a reference to element at position pIndex.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIndex is greater than or equal to the size of this object.

	const T & elt(
		const	Uint32		pIndex
	) const;

	/// \brief Returns a reference to the first element.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If this array is empty.

	const T & getFirst(
	) const;

	/// \brief Returns a reference to the last element.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If this array is empty.

	const T & getLast(
	) const;

	/// \brief Returns the position of the first occurrence of \a pElt
	///	within this object, or -1 if there is no such occurrence.

	Uint32 indexOf(
		const	T &		pElt
	) const;

	/// \brief Returns true iff this object contains an occurrence of
	///	\a pElt.

	Bool contains(
		const	T &		pElt
	) const;

	/// \brief Returns true iff this object contains an occurrennce of
	///	\a pElt, in which case \a pIndex is updated with the index
	///	of this occurrence.

	template < typename S >
	Bool contains(
		const	S &		pElt,
			Uint32 &	pIndex
	) const;

protected :

	void doCopy(
		const	CompArray< T > &
					pOther,
		const	Uint32		start,
		const	Uint32		length
	);

private :

};

// ============================================================================

template < typename T >
CompArray< T >::CompArray(
	const	Uint32		pNbr )

	: ArrayBase( pNbr ) {

	Uint32	cnt = nbr;

	while ( cnt-- ) {
		ptr[ cnt ] = ( void * )( new T );
	}

}

template < typename T >
CompArray< T >::CompArray(
	const	CompArray< T >	& src ) :

	SObject(),
	ArrayBase( 0 ) {

	doCopy( src, 0, src.nbr );

}

// ============================================================================

template < typename T >
CompArray< T >::~CompArray() {

	kill();

}

// ============================================================================

template < typename T >
CompArray< T > & CompArray< T >::operator = (
	const	CompArray< T >	& src ) {

	if ( this != & src ) {
		doCopy( src, 0, src.nbr );
	}

	return * this;

}

// ============================================================================

template < typename T >
Bool CompArray< T >::operator == (
	const	CompArray< T >	& src ) const {

	if ( nbr != src.nbr ) {
		return false;
	}

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( * ( const T * )( ptr[ i ] )
				!= * ( const T * )( src.ptr[ i ] ) )
			return false;
	}

	return true;

}

template < typename T >
Bool CompArray< T >::operator != (
	const	CompArray< T >	& src ) const {

	return ( ! ( * this == src ) );

}

// ============================================================================

template < typename T >
void CompArray< T >::add(
	const	T &		pElt
) {
	insert( nbr, pElt );
}

template < typename T >
void CompArray< T >::addLast(
	const	T &		pElt
) {
	add( pElt );
}

template < typename T >
void CompArray< T >::add(
	const	CompArray< T > &
					pOther
) {
	insert( nbr, pOther );
}

template < typename T >
CompArray< T > & CompArray< T >::operator += (
	const	T &		pElt
) {
	insert( nbr, pElt );
	return * this;
}

template < typename T >
CompArray< T > & CompArray< T >::operator += (
	const	CompArray< T > &
					pOther
) {
	insert( nbr, pOther );
	return * this;
}

template < typename T >
CompArray< T > & CompArray< T >::operator << (
	const	T &		t
) {
	insert( nbr, t );
	return *this;
}

template < typename T >
void CompArray< T >::insert( 
	const	Uint32		index,
	const	T		& value ) {

	checkIndex( index, true );

	Uint32	pos = index;

	insertHole( pos, 1 );

	ptr[ pos ] = ( void * )( new T( value ) );

}

template < typename T >
void CompArray< T >::insert( 
	const	Uint32		index,
	const	CompArray< T >	& array ) {

	if ( this == & array ) {

		CompArray< T > tmp = array;

		insert( index, tmp );

	}
	else {

		checkIndex( index, true );

		Uint32	pos = index;
		Uint32	cnt = array.nbr;
	
		insertHole( pos, cnt );

		while ( cnt-- ) {
			const T * t = ( const T * )( array.ptr[ cnt ] );
			ptr[ pos + cnt ] = ( void * )( new T( * t ) );
		}

	}

}

// ============================================================================

template < typename T >
CompArray< T > & CompArray< T >::operator -= (
	const	T &		pElt
) {
	remove( pElt );
	return * this;
}

template < typename T >
void CompArray< T >::remove(
	const	T		& elt,
	const	Bool		all ) {

	Uint32 i = 0;

	while ( i < nbr ) {
		if ( * ( const T * )( ptr[ i ] ) == elt ) {
			pop( i );
			if ( ! all ) {
				break;
			}
		}
		else {
			i++;
		}
	}

}

// ============================================================================

template < typename T >
T CompArray< T >::pop(
	const	Uint32		index ) {

	checkIndex( index, false );

	T res = *( const T * )( ptr[ index ] );

	delete ( T * )( ptr[ index ] );

	deleteHole( index, 1 );

	return res;

}

// ============================================================================

template < typename T >
void CompArray< T >::kill() {

	if ( nbr ) {
		Uint32	cnt = nbr;
		while ( cnt-- ) {
			delete ( T * )( ptr[ cnt ] );
		}
		deleteHole( 0, nbr );
	}

}

// ============================================================================

template < typename T >
void CompArray< T >::resize(
	const	Uint32		newsize ) {

	if ( newsize == nbr ) {
		return;
	}

	if ( ! newsize ) {
		kill();
	}
	else if ( newsize < nbr ) {
		while ( newsize < nbr ) {
			nbr--;
			delete ( T * )( ptr[ nbr ] );
		}
	}
	else {
		Uint32	pos = nbr;
		Uint32	cnt = newsize - nbr;
		insertHole( pos, cnt );
		while ( cnt-- ) {
			ptr[ pos + cnt ] = ( void * )( new T );
		}
	}

}

// ============================================================================

template < typename T >
void CompArray< T >::doCopy(
	const	CompArray< T >	& src,
	const	Uint32		start,
	const	Uint32		length ) {

	kill();

	if ( src.nbr && src.nbr > start ) {
		Uint32	cnt = src.nbr - start;
		if ( cnt > length )
			cnt = length;
		insertHole( 0, cnt );
		while ( cnt-- ) {
			ptr[ cnt ] = ( void * )( new T( * ( const T * )( src.ptr[ cnt + start ] ) ) );
		}
	}

}

// ============================================================================

template < typename T >
T & CompArray< T >::operator [] (
	const	Uint32		pIndex
) {
	checkIndex( pIndex, false );
	return * ( T * )( ptr[ pIndex ] );
}

template < typename T >
const T & CompArray< T >::operator [] (
	const	Uint32		pIndex
) const {
	checkIndex( pIndex, false );
	return * ( const T * )( ptr[ pIndex ] );
}

template < typename T >
T & CompArray< T >::elt(
	const	Uint32		pIndex
) {
	checkIndex( pIndex, false );
	return * ( T * )( ptr[ pIndex ] );
}

template < typename T >
const T & CompArray< T >::elt(
	const	Uint32		pIndex
) const {
	checkIndex( pIndex, false );
	return * ( const T * )( ptr[ pIndex ] );
}

template < typename T >
const T & CompArray< T >::getFirst(
) const {
	checkIndex( 0, false );
	return * ( const T * )( ptr[ 0 ] );
}

template < typename T >
const T & CompArray< T >::getLast(
) const {
	checkIndex( nbr - 1, false );
	return * ( const T * )( ptr[ nbr - 1 ] );
}

template < typename T >
Uint32 CompArray< T >::indexOf(
	const	T &		pElt
) const {
	Uint32	pos;
	return ( contains( pElt, pos ) ? pos : ( Uint32 )-1 );
}

// ============================================================================

template < typename T >
Bool CompArray< T >::contains(
	const	T &		pElt ) const {

	Uint32	dummy;

	return contains( pElt, dummy );

}

template < typename T >
template < typename S >
Bool CompArray< T >::contains(
	const	S		& t,
		Uint32		& i ) const {

	for ( i = 0 ; i < nbr ; i++ ) {
		if ( * ( const T * )( ptr[ i ] ) == t ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

template < typename T >
CompArray< T > CompArray< T >::operator () (
	const	Uint32		start ) const {

	CompArray<T> res;
	res.doCopy( *this, start, nbr);

	return res;

}

template < typename T >
CompArray< T > CompArray< T >::operator () (
	const	Uint32		start,
	const	Uint32		length ) const {

	CompArray<T> res;
	res.doCopy( *this, start, length);

	return res;

}

// ============================================================================

#if defined( PLATFORM_WIN32 )

// Fix for Win32.
//
// Problem:
//	1° "BufferArray" inherits "CompArray< Buffer >", which implicitely
//	   exports this base class on Win32.
//	2° "Map< K, V >" uses a "CompArray< K >".
//	3° "Dyn::Group" uses a "Map< Buffer, ... >".
//	---> both BFC_Light and BFC_Dyn export "CompArray< Buffer >" :-(
// Solution: we explicitely export "CompArray< Buffer >" in BFC_Light
//	---> BFC_Dyn will import "CompArray< Buffer >" from BFC_Light.
// More info: http://www.codesynthesis.com/~boris/blog/2010/01/18/dll-export-cxx-templates/

template class BFC_Light_DLL CompArray< Buffer >;

#endif

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_CompArray_H_

// ============================================================================

