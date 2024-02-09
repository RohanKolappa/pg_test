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
//	BFC.TL.SortArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_SortArray_H_
#define _BFC_TL_SortArray_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.TL.ArrayBase.h"
#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A fast and secure sorted array of comparable elements of type \b T.
///
/// \ingroup BFC_TL

template < typename T >
class SortArray : public ArrayBase {

public :

	/// \brief Creates a new SortArray.

	SortArray(
	);

	/// \brief Creates a new SortArray, which is a sorted copy of array
	///	\a other.

	SortArray(
		const	CompArray< T > &	other
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

	/// \brief Returns a reference to element at position pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	T & operator [] (
		const	Uint32			pIdx
	) {
		checkIndex( pIdx, false );
		return getT( pIdx );
	}

	/// \brief Returns a reference to element at position pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	const T & operator [] (
		const	Uint32			pIdx
	) const {
		checkIndex( pIdx, false );
		return getT( pIdx );
	}

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

	/// \brief Removes element whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater than or equal to the size of this object.

	void pop(
		const	Uint32			pIdx
	);

	/// \brief Destroys the content of this object.

	void kill(
	);

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
	///	\a elt, in which case \a pIdx is updated with the index
	///	of this occurrence.

	Bool contains(
		const	T &			elt,
			Uint32 &		pIdx
	) const;

	BASE_CLASS_GEN_EXCEPTION( "BFC.TL.SortArray" );

	CLASS_EXCEPTION( DuplicateElement, "Duplicate element" );
	CLASS_EXCEPTION( NoSuchElement, "No such element" );

protected :

	void doCopy(
		const	SortArray< T > &	other
	);

	T & getT(
		const	Uint32			pIdx
	) {
		return * reinterpret_cast< T * >( ptr[ pIdx ] );
	}

	const T & getT(
		const	Uint32			pIdx
	) const {
		return * reinterpret_cast< const T * >( ptr[ pIdx ] );
	}

	void setT(
		const	Uint32			pIdx,
		const	T &			elt
	) {
		ptr[ pIdx ] = reinterpret_cast< void * >( new T( elt ) );
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

template < typename T >
SortArray< T >::SortArray(
	const	CompArray< T > &	src ) :

	ArrayBase( 0 ) {

	for ( Uint32 i = 0 ; i < src.getSize() ; i++ ) {
		try {
			insert( src[ i ] );
		}
		catch ( DuplicateElement & ) {
		}
	}

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

	Uint32	pIdx = findNearest( elt );

	if ( pIdx < nbr && getT( pIdx ) == elt ) {
		throw DuplicateElement();
	}

	insertHole( pIdx, 1 );

	setT( pIdx, elt );

}

// ============================================================================

template < typename T >
void SortArray< T >::remove(
	const	T &		elt ) {

	Uint32	pIdx = findNearest( elt );

	if ( pIdx >= nbr || getT( pIdx ) != elt ) {
		throw NoSuchElement();
	}

	pop( pIdx );

}

// ============================================================================

template < typename T >
void SortArray< T >::pop(
	const	Uint32		pIdx ) {

	checkIndex( pIdx, false );

	delete reinterpret_cast< T * >( ptr[ pIdx ] );

	deleteHole( pIdx, 1 );

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
		Uint32 &	pIdx ) const {

	pIdx = findNearest( elt );

	if ( pIdx < nbr && getT( pIdx ) == elt ) {
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
		if ( ! ( getT( posMin ) < elt ) ) {
			return posMin;
		}
		posMin++;
		if ( posMin == posMax ) {
			return posMin;
		}
		if ( getT( posMax - 1 ) < elt ) {
			return posMax;
		}
		posMax--;
		if ( posMin == posMax ) {
			return posMin;
		}
		posMed = ( posMin + posMax ) / 2;
		if ( ! ( elt < getT( posMed ) ) ) {
			posMin = posMed;
		}
		else {
			posMax = posMed;
		}
	}

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_SortArray_H_

// ============================================================================

