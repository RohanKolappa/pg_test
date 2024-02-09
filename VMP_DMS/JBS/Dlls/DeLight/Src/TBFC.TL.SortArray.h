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
//	TBFC.TL.SortArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_SortArray_H_
#define _TBFC_TL_SortArray_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.TL.ArrayBase.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A fast and secure sorted array of comparable elements of type \b T.
///
/// \ingroup TBFC_TL

template < typename T >
class SortArray : public ArrayBase {

public :

	/// \brief Creates a new SortArray.

	SortArray(
	);

	/// \brief Creates a new SortArray, which is a deep copy of array
	///	\a other.

	SortArray(
		const	SortArray< T > &	other
	);

	/// \brief Destroys this object.

	virtual ~SortArray(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a other.

	SortArray< T > & operator = (
		const	SortArray< T > &	other
	);

	/// \brief Returns true iff this object is identical to \a other.

	Bool operator == (
		const	SortArray< T > &	other
	) const;

	/// \brief Returns true iff this object is different than \a other.

	Bool operator != (
		const	SortArray< T > &	other
	) const;

	/// \brief Inserts a copy of \a elt in this object.

	void operator += (
		const	T &			elt
	) {
		insert( elt );
	}

	/// \brief Inserts a copy of \a elt in this object.

	void insert( 
		const	T &			elt
	 );

	/// \brief Removes \a elt from this object.

	void operator -= (
		const	T &			elt
	) {
		remove( elt );
	}

	/// \brief Removes \a elt from this object.

	void remove(
		const	T &			elt
	);

	/// \brief Removes element whose index is \a idx.
	///
	/// \throws OutOfRange
	///	If \a idx is greater than or equal to the size of this object.

	void pop(
		const	Uint32			idx
	);

	/// \brief Destroys the content of this object.

	void kill(
	);

	/// \brief Returns a reference to element at position idx.
	///
	/// This reference can be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a idx is greater than or equal to the size of this object.

	T & operator [] (
		const	Uint32			idx
	) {
		checkIndex( idx, false );
		return getT( idx );
	}

	/// \brief Returns a reference to element at position idx.
	///
	/// This reference cannot be used as an L-value in an assignement.
	///
	/// \throws OutOfRange
	///	If \a idx is greater than or equal to the size of this object.

	const T & operator [] (
		const	Uint32			idx
	) const {
		checkIndex( idx, false );
		return getT( idx );
	}

	/// \brief Returns the position of \a elt
	///	within this object, or -1 if there is no such occurrence.

	Uint32 indexOf(
		const	T &			elt
	) const {
		Uint32	pos;
		return ( contains( elt, pos ) ? pos : ( Uint32 )-1 );
	}

	/// \brief Returns true iff this object contains \a elt.

	Bool contains(
		const	T &			elt
	) const {
		Uint32	dummy;
		return contains( elt, dummy );
	}

	/// \brief Returns true iff this object contains an occurrennce of
	///	\a elt, in which case \a idx is updated with the index
	///	of this occurrence.

	Bool contains(
		const	T &			elt,
			Uint32 &		idx
	) const;

	BASE_CLASS_GEN_EXCEPTION( "TBFC.TL.SortArray" );

	CLASS_EXCEPTION( DuplicateElement, "Duplicate element" );
	CLASS_EXCEPTION( NoSuchElement, "No such element" );

protected :

	void doCopy(
		const	SortArray< T > &	other
	);

	T & getT(
		const	Uint32			idx
	) {
		return * reinterpret_cast< T * >( ptr[ idx ] );
	}

	const T & getT(
		const	Uint32			idx
	) const {
		return * reinterpret_cast< const T * >( ptr[ idx ] );
	}

	void setT(
		const	Uint32			idx,
		const	T &			elt
	) {
		ptr[ idx ] = reinterpret_cast< void * >( new T( elt ) );
	}

	Uint32 findNearest(
		const	T &			elt
	) const;

private :

};

// ============================================================================

template < typename T >
SortArray< T >::SortArray() :

	ArrayBase( 0 ) {

}

template < typename T >
SortArray< T >::SortArray(
	const	SortArray< T > &	src ) :

	ArrayBase( 0 ) {

	doCopy( src );

}

// ============================================================================

template < typename T >
SortArray< T >::~SortArray() {

	kill();

}

// ============================================================================

template < typename T >
SortArray< T > & SortArray< T >::operator = (
	const	SortArray< T > &	src ) {

	if ( this != & src ) {
		doCopy( src );
	}

	return * this;

}

// ============================================================================

template < typename T >
Bool SortArray< T >::operator == (
	const	SortArray< T > &	src ) const {

	if ( nbr != src.nbr ) {
		return false;
	}

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( getT( i ) != src.getT( i ) ) {
			return false;
		}
	}

	return true;

}

template < typename T >
Bool SortArray< T >::operator != (
	const	SortArray< T > &	src ) const {

	return ( ! ( * this == src ) );

}

// ============================================================================

template < typename T >
void SortArray< T >::insert(
	const	T &		elt ) {

	Uint32	idx = findNearest( elt );

	if ( idx < nbr && getT( idx ) == elt ) {
		throw DuplicateElement();
	}

	insertHole( idx, 1 );

	setT( idx, elt );

}

// ============================================================================

template < typename T >
void SortArray< T >::remove(
	const	T &		elt ) {

	Uint32	idx = findNearest( elt );

	if ( idx >= nbr || getT( idx ) != elt ) {
		throw NoSuchElement();
	}

	pop( idx );
}

// ============================================================================

template < typename T >
void SortArray< T >::pop(
	const	Uint32		idx ) {

	checkIndex( idx, false );

	delete reinterpret_cast< T * >( ptr[ idx ] );

	deleteHole( idx, 1 );

}

// ============================================================================

template < typename T >
void SortArray< T >::kill() {

	if ( nbr ) {
		Uint32	cnt = nbr;
		while ( cnt-- ) {
			delete reinterpret_cast< T * >( ptr[ cnt ] );
		}
		deleteHole( 0, nbr );
	}

}

// ============================================================================

template < typename T >
Bool SortArray< T >::contains(
	const	T &		elt,
		Uint32 &	idx ) const {

	idx = findNearest( elt );

	if ( idx < nbr && getT( idx ) == elt ) {
		return true;
	}

	return false;

}

// ============================================================================

template < typename T >
void SortArray< T >::doCopy(
	const	SortArray< T > &	src ) {

	kill();

	if ( src.nbr ) {
		Uint32	cnt = src.nbr;
		insertHole( 0, cnt );
		while ( cnt-- ) {
			setT( cnt, src.getT( cnt ) );
		}
	}

}

// ============================================================================

template < typename T >
Uint32 SortArray< T >::findNearest(
	const	T &			elt ) const {

	Uint32	posMin = 0;
	Uint32	posMax = nbr;
	Uint32	posMed;

	for (;;) {
		if ( posMin == posMax ) {
			return posMin;
		}
		if ( elt <= getT( posMin ) ) {
			return posMin;
		}
		posMin++;
		if ( posMin == posMax ) {
			return posMin;
		}
		if ( elt > getT( posMax - 1 ) ) {
			return posMax;
		}
		posMax--;
		if ( posMin == posMax ) {
			return posMin;
		}
		posMed = ( posMin + posMax ) / 2;
		if ( elt >= getT( posMed ) ) {
			posMin = posMed;
		}
		else {
			posMax = posMed;
		}
	}

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_SortArray_H_

// ============================================================================

