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
//	TBFC.TL.CompArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_CompArray_H_
#define _TBFC_TL_CompArray_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.TL.ArrayBase.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A fast and secure array of comparable elements of type \b T.
///
/// \ingroup TBFC_TL

template < typename T >
class CompArray : public ArrayBase {

public :

	/// \brief Creates a new CompArray of size \a __size
	///	(default: 0).

	CompArray(
		const	Uint32		__size = 0
	);

	/// \brief Creates a new CompArray, which is a deep copy of array
	///	\a __other.

	CompArray(
		const	CompArray< T >	& __other
	);

	/// \brief Destroys this object.

	virtual ~CompArray(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a __other.

	CompArray< T > & operator = (
		const	CompArray< T >	& __other
	);

	/// \brief Returns true iff this object is identical to \a __other.

	Bool operator == (
		const	CompArray< T >	& __other
	) const;

	/// \brief Returns true iff this object is different than \a __other.

	Bool operator != (
		const	CompArray< T >	& __other
	) const;

	/// \brief Appends a copy of \a __elt at the end of this object.

	CompArray< T > & operator += (
		const	T		& __elt
	) {
		insert( nbr, __elt );
		return * this;
	}

	/// \brief Appends a copy of \a __other at the end of this object.

	CompArray< T > & operator += (
		const	CompArray< T >	& __other
	) {
		insert( nbr, __other );
		return * this;
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
		const	CompArray< T >	& __other
	 );

	/// \brief Removes the first occurrence (if any) of \a __elt from
	///	this object.

	CompArray< T > & operator -= (
		const	T		& __elt
	) {
		remove( __elt );
		return * this;
	}

	/// \brief Removes the first (if \a __all is false, which is the
	///	default) or all (if \a __all is true) occurrence(s) (if any
	///	occurrence at all) of \a __elt from this object.

	void remove(
		const	T		& __elt,
		const	Bool		__all = false
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

	/// \brief Returns the position of the first occurrence of \a __elt
	///	within this object, or -1 if there is no such occurrence.

	Uint32 indexOf(
		const	T		& __elt
	) const {
		Uint32	pos;
		return ( contains( __elt, pos ) ? pos : ( Uint32 )-1 );
	}

	/// \brief Returns true iff this object contains an occurrence of
	///	\a __elt.

	Bool contains(
		const	T		& __elt
	) const {
		Uint32	dummy;
		return contains( __elt, dummy );
	}

	/// \brief Returns true iff this object contains an occurrennce of
	///	\a __elt, in which case \a __idx is updated with the index
	///	of this occurrence.

	Bool contains(
		const	T		& __elt,
			Uint32		& __idx
	) const;

protected :

	void doCopy(
		const	CompArray< T >	& __other
	);

private :

};

// ============================================================================

template < typename T >
CompArray< T >::CompArray(
	const	Uint32		__nbr )

	: ArrayBase( __nbr ) {

	Uint32	cnt = nbr;

	while ( cnt-- ) {
		ptr[ cnt ] = ( void * )( new T );
	}

}

template < typename T >
CompArray< T >::CompArray(
	const	CompArray< T >	& src )

	: ArrayBase( 0 ) {

	doCopy( src );

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
		doCopy( src );
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
void CompArray< T >::pop(
	const	Uint32		index ) {

	checkIndex( index, false );

	delete ( T * )( ptr[ index ] );

	deleteHole( index, 1 );

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
void CompArray< T >::doCopy(
	const	CompArray< T >	& src ) {

	kill();

	if ( src.nbr ) {
		Uint32	cnt = src.nbr;
		insertHole( 0, cnt );
		while ( cnt-- ) {
			ptr[ cnt ] = ( void * )( new T( * ( const T * )( src.ptr[ cnt ] ) ) );
		}
	}

}

// ============================================================================

template < typename T >
Bool CompArray< T >::contains(
	const	T		& t,
		Uint32		& i ) const {

	for ( i = 0 ; i < nbr ; i++ ) {
		if ( * ( const T * )( ptr[ i ] ) == t ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_CompArray_H_

// ============================================================================

