// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.TL.Array.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_Array_H_
#define _TBFC_TL_Array_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.TL.ArrayBase.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A fast and secure array of elements of type \b T.
///
/// \ingroup TBFC_TL

template < typename T >
class Array : public ArrayBase {

public :

	/// \brief Creates a new Array of size \a __size
	///	(default: 0).

	Array(
		const	Uint32		__size = 0
	);

	/// \brief Creates a new Array, which is a deep copy of array
	///	\a __other.

	Array(
		const	Array< T >	& __other
	);

	/// \brief Destroys this object.

	virtual ~Array(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a __other.

	Array< T > & operator = (
		const	Array< T >	& __other
	);

	/// \brief Appends a copy of \a __elt at the end of this object.

	void operator += (
		const	T		& __elt
	) {
		insert( nbr, __elt );
	}

	/// \brief Appends a copy of \a __other at the end of this object.

	void operator += (
		const	Array< T >	& __other
	) {
		insert( nbr, __other );
	}

	/// \brief Inserts a copy of \a __elt at position \a __idx.
	///
	/// All elements (if any), starting from index \a __idx, are moved one
	/// step to the "right" (in the direction of increasing indexes), to
	/// make room for the new element.
	///
	/// \throws OutOfRange
	///	If \a __idx is greater than the size of this object.

	void insert( 
		const	Uint32		__idx,
		const	T		& __elt
	 );

	/// \brief Inserts a copy of \a __other at position \a __idx.
	///
	/// All elements (if any), starting from index \a __idx, are moved
	/// __other.getSize()
	/// steps to the "right" (in the direction of increasing indexes), to
	/// make room for the new elements.
	///
	/// \throws OutOfRange
	///	If \a __idx is greater than the size of this object.

	void insert( 
		const	Uint32		__idx,
		const	Array< T >	& __other
	 );

	/// \brief Removes element whose index is \a __idx.
	///
	/// \throws OutOfRange
	///	If \a __idx is greater than or equal to the size of this object.

	void pop(
		const	Uint32		__idx
	);

	/// \brief Destroys the content of this object.

	void kill(
	);

	/// \brief Resizes this object to new size \a __size.
	///
	/// If `__size' is 0, kills this object.
	///
	/// Otherwise, if this object is empty, creates a new Array that contains
	/// __size elements of type T.
	///
	/// Otherwise, if the actual size of this object is less than __size,
	/// then reallocates memory in order for this object to contain
	/// __size objects (the actual content is NOT destroyed).
	///
	/// Otherwise, reduces the size of this object by deleting the last
	/// elements.

	void resize(
		const	Uint32		__size
	);

	/// \brief Returns a reference to element at position __idx.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a __idx is greater than or equal to the size of this object.

	const T & operator [] (
		const	Uint32		__idx
	) const {
		checkIndex( __idx, false );
		return * ( const T * )( ptr[ __idx ] );
	}

	/// \brief Returns a reference to element at position __idx.
	///
	/// This reference can be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a __idx is greater than or equal to the size of this object.

	T & operator [] (
		const	Uint32		__idx
	) {
		checkIndex( __idx, false );
		return * ( T * )( ptr[ __idx ] );
	}

protected :

	void doCopy(
		const	Array< T >	& __other
	);

private :

};

// ============================================================================

template < typename T >
Array< T >::Array(
	const	Uint32		__nbr )

	: ArrayBase( __nbr ) {

	Uint32	cnt = nbr;

	while ( cnt-- ) {
		ptr[ cnt ] = ( void * )( new T );
	}

}

template < typename T >
Array< T >::Array(
	const	Array< T >	& src )

	: ArrayBase( 0 ) {

	doCopy( src );

}

// ============================================================================

template < typename T >
Array< T >::~Array() {

	kill();

}

// ============================================================================

template < typename T >
Array< T > & Array< T >::operator = (
	const	Array< T >	& src ) {

	if ( this != & src ) {
		doCopy( src );
	}

	return * this;

}

// ============================================================================

template < typename T >
void Array< T >::insert( 
	const	Uint32		index,
	const	T		& value ) {

	checkIndex( index, true );

	Uint32	pos = index;

	insertHole( pos, 1 );

	ptr[ pos ] = ( void * )( new T( value ) );

}

template < typename T >
void Array< T >::insert( 
	const	Uint32		index,
	const	Array< T >	& array ) {

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
void Array< T >::pop(
	const	Uint32		index ) {

	checkIndex( index, false );

	delete ( T * )( ptr[ index ] );

	deleteHole( index, 1 );

}

// ============================================================================

template < typename T >
void Array< T >::kill() {

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
	const	Array< T >	& src ) {

	kill();

	if ( src.nbr ) {
		Uint32	cnt = src.nbr;
		insertHole( 0, cnt );
		while ( cnt-- ) {
			const T * t = ( const T * )( src.ptr[ cnt ] );
			ptr[ cnt ] = ( void * )( new T( * t ) );
		}
	}

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_Array_H_

// ============================================================================

