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
//	BFC.TL.Array.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_Array_H_
#define _BFC_TL_Array_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtrTmpl.h"

// ============================================================================

namespace BFC {

BFC_PTR_TMPL( Array )

} // namespace BFC

// ============================================================================

#include "BFC.TL.ArrayBase.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A fast and secure array of elements of type \b T.
///
/// \ingroup BFC_TL

template < typename T >
class Array : public ArrayBase {

public :

	/// \brief Creates a new Array of size \a pSize
	///	(default: 0).

	Array(
		const	Uint32		pSize = 0
	);

	/// \brief Creates a new Array, which is a deep copy of array
	///	\a pOther.

	Array(
		const	Array< T > &	pOther
	);

	/// \brief Destroys this object.

	virtual ~Array(
	);

	/// \brief Returns a subarray of this object.
	///
	/// This method constructs a new Array object, containing only the
	/// elements of this array starting at position \a start.
	///
	/// If \a start is outside of the internal buffer, an empty object
	/// is returned.

	Array< T > operator () (
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

	Array< T > operator () (
		const	Uint32		start,
		const	Uint32		length
	) const;

	/// \brief Replaces the content of this object by a deep copy of
	///	\a pOther.

	Array< T > & operator = (
		const	Array< T > &	pOther
	);

	/// \brief Appends a copy of \a pElt at the end of this object.

	void operator += (
		const	T &		pElt
	) {
		insert( nbr, pElt );
	}

	/// \brief Appends a copy of \a pOther at the end of this object.

	void operator += (
		const	Array< T > &	pOther
	) {
		insert( nbr, pOther );
	}

	/// \brief Appends a copy of \a pElt at the end of this object.

	void add(
		const	T &		pElt
	) {
		insert( nbr, pElt );
	}

	/// \brief Synonym of add.

	void addLast(
		const	T &		pElt
	) {
		add( pElt );
	}

	/// \brief Appends a copy of \a t at the end of this object.

	Array< T > & operator << (
		const	T &		t
	) {
		insert( nbr, t );
		return *this;
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
		const	T &		pElt
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
		const	Array< T > &	pOther
	 );

	/// \brief Removes element whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	T pop(
		const	Uint32		pIdx
	);

	/// \brief Removes first element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	T popFirst(
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

	T popLast(
	) {
		return pop( nbr - 1 );
	}

	/// \brief Removes element whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	void del(
		const	Uint32		pIdx
	);

	/// \brief Removes first element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	void delFirst(
	) {
		del( 0 );
	}

	/// \brief Removes last element.
	///
	/// \throws OutOfRange
	///	If this object is empty.

	void delLast(
	) {
		del( nbr - 1 );
	}

	/// \brief Removes element whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	T removeAt(
		const	Uint32		pIdx
	) {
		return pop( pIdx );
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

	void incSize(
	) {
		resize( getSize() + 1 );
	}

	/// \brief Returns a reference to element at position pIdx.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	const T & operator [] (
		const	Uint32		pIdx
	) const {
		checkIndex( pIdx, false );
		return * ( const T * )( ptr[ pIdx ] );
	}

	/// \brief Returns a reference to element at position pIdx.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	const T & getElt(
		const	Uint32		pIdx
	) const {
		checkIndex( pIdx, false );
		return * ( const T * )( ptr[ pIdx ] );
	}

	/// \brief Sets element \a pIdx to be a copy of \a pElt.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	void setElt(
		const	Uint32		pIdx,
		const	T &		pElt
	) {
		checkIndex( pIdx, false );
		* ( T * )( ptr[ pIdx ] ) = pElt;
	}

	/// \brief Returns a reference to element at position pIdx.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	const T & at(
		const	Uint32		pIdx
	) const {
		checkIndex( pIdx, false );
		return * ( const T * )( ptr[ pIdx ] );
	}

	/// \brief Returns a reference to element at position pIdx.
	///
	/// This reference can be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	T & operator [] (
		const	Uint32		pIdx
	) {
		checkIndex( pIdx, false );
		return * ( T * )( ptr[ pIdx ] );
	}

	const T & getFirst(
	) const {
		checkIndex( 0, false );
		return ( *this )[ 0 ];
	}

	T & getFirst(
	) {
		checkIndex( 0, false );
		return ( *this )[ 0 ];
	}

	const T & getLast(
		const	Uint32		pOffset = 0
	) const {
		checkIndex( pOffset, false );
		return ( *this )[ nbr - 1 - pOffset ];
	}

	T & getLast(
		const	Uint32		pOffset = 0
	) {
		checkIndex( pOffset, false );
		return ( *this )[ nbr - 1 - pOffset ];
	}

	void setLast(
		const	T &		t
	) {
		checkIndex( 0, false );
		( *this )[ nbr - 1 ] = t;
	}

protected :

	void doCopy(
		const	Array< T > &	pOther,
		const	Uint32		start,
		const	Uint32		length
	);

private :

};

// ============================================================================

template < typename T >
Array< T >::Array(
	const	Uint32		pNbr )

	: ArrayBase( pNbr ) {

	Uint32	cnt = nbr;

	while ( cnt-- ) {
		ptr[ cnt ] = ( void * )( new T );
	}

}

template < typename T >
Array< T >::Array(
	const	Array< T > &	src ) :

	SObject(),
	ArrayBase( 0 ) {

	doCopy( src, 0, src.nbr );

}

// ============================================================================

template < typename T >
Array< T >::~Array() {

	kill();

}

// ============================================================================

template < typename T >
Array< T > & Array< T >::operator = (
	const	Array< T > &	src ) {

	if ( this != & src ) {
		doCopy( src, 0, src.nbr );
	}

	return * this;

}

// ============================================================================

template < typename T >
void Array< T >::insert( 
	const	Uint32		index,
	const	T &		value ) {

	checkIndex( index, true );

	Uint32	pos = index;

	insertHole( pos, 1 );

	ptr[ pos ] = ( void * )( new T( value ) );

}

template < typename T >
void Array< T >::insert( 
	const	Uint32		index,
	const	Array< T > &	array ) {

	if ( this == & array ) {

		Array< T > tmp = array;

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
T Array< T >::pop(
	const	Uint32		index ) {

	checkIndex( index, false );

	T res = *( const T * )( ptr[ index ] );

	delete ( T * )( ptr[ index ] );

	deleteHole( index, 1 );

	return res;

}

template < typename T >
void Array< T >::popFirst(
	const	Uint32		pNbr ) {

	if ( ! pNbr ) {
		return;
	}

	checkIndex( pNbr - 1, false );


	for ( Uint32 i = 0 ; i < pNbr ; i++ ) {
		delete ( T * )( ptr[ i ] );
	}

	deleteHole( 0, pNbr );

}

template < typename T >
void Array< T >::del(
	const	Uint32		index ) {

	checkIndex( index, false );

	delete ( T * )( ptr[ index ] );

	deleteHole( index, 1 );

}

// ============================================================================

template < typename T >
void Array< T >::kill() {

	while ( ! isEmpty() ) {
		delLast();
	}

}

// ============================================================================

template < typename T >
void Array< T >::resize(
	const	Uint32		newsize ) {

	if ( ! newsize ) {
		kill();
	}
	else if ( newsize <= nbr ) {
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
void Array< T >::doCopy(
	const	Array< T > &	src,
	const	Uint32		start,
	const	Uint32		length ) {

	kill();

	if ( src.nbr && src.nbr > start ) {
		Uint32	cnt = src.nbr - start;
		if ( cnt > length )
			cnt = length;
		insertHole( 0, cnt );
		while ( cnt-- ) {
			const T * t = ( const T * )( src.ptr[ cnt + start ] );
			ptr[ cnt ] = ( void * )( new T( * t ) );
		}
	}

}

// ============================================================================

template < typename T >
Array< T > Array< T >::operator () (
	const	Uint32		start ) const {

	Array<T> res;
	res.doCopy( *this, start, nbr);

	return res;

}

template < typename T >
Array< T > Array< T >::operator () (
	const	Uint32		start,
	const	Uint32		length ) const {

	Array<T> res;
	res.doCopy( *this, start, length);

	return res;

}

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_Array_H_

// ============================================================================

