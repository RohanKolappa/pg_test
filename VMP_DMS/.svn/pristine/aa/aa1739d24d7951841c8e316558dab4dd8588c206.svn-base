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
//	TBFC.FIFO.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _TBFC_FIFO_H_
#define _TBFC_FIFO_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.FastCS.h"

#include "TBFC.TL.ListBase.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A simple templated FIFO.
///
/// \ingroup TBFC_TL

template < typename T >
class FIFO : public ListBase {

public :

	/// \brief Creates an empty FIFO.
	///
	/// This constructor creates an empty FIFO, suitable to store
	/// elements of type T, and of maximum size given by \a maxSize.
	///
	/// If \a maxSize is 0 (the default), the size of this FIFO will not be
	/// limited.

	FIFO(
		const	Uint32		maxSize = 0
	);

	/// \brief Destroys this FIFO.

	~FIFO(
	);

	/// \brief Returns true iff this FIFO is full.
	///
	/// This method returns true iff a non null maximum size was set using
	/// the constructor, and the number of elements is equal to this limit.

	Bool isFull(
	) const {
		return ( maxSize && getSize() == maxSize );
	}

	/// \brief Destroys all elements in this FIFO.

	void kill(
	);

	/// \brief Appends an element at the end of this FIFO.
	///
	/// \note
	///	The template type T must provide a copy constructor.
	///
	/// \throws Overflow
	///	If this FIFO is already full (see isFull).

	void push(
		const	T		& elt
	);

	/// \brief Returns the element at the beginning of this FIFO.
	///
	/// \throws Underflow
	///	If this FIFO is still empty (see isEmpty).

	const T & top(
	) const;

	/// \brief Removes the first element from this FIFO.
	///
	/// \throws Underflow
	///	If this FIFO is still empty (see isEmpty).

	void pop(
	);

	BASE_CLASS_GEN_EXCEPTION( "TBFC.TL.FIFO" );

	CLASS_EXCEPTION( Overflow, "Overflow" );
	CLASS_EXCEPTION( Underflow, "Underflow" );

protected :

private :

	Uint32		maxSize;	///< Maximum size of this FIFO.

	// others...

	FIFO(
		const	FIFO< T >	&
	);

	FIFO< T > & operator = (
		const	FIFO< T >	& src
	);

};

// ============================================================================

template < typename T >
FIFO< T >::FIFO(
	const	Uint32		__maxSize )
	: maxSize ( __maxSize ) {



}

template < typename T >
FIFO< T >::~FIFO() {

	kill();

}

// ============================================================================

template < typename T >
void FIFO< T >::kill() {

	while ( ! isEmpty() ) {
		pop();
	}

}

// ============================================================================

template < typename T >
void FIFO< T >::push (
	const	T		& elt ) {

	if ( isFull() ) {
		throw Overflow();
	}

	privPushAfter( privGetLast(), privGetNew( new T( elt ) ) );

}

template < typename T >
const T & FIFO< T >::top(
) const {

	if ( isEmpty() ) {
		throw Underflow();
	}

	return * ( const T * )privGetFirst()->data;

}

template < typename T >
void FIFO< T >::pop() {

	if ( isEmpty() ) {
		throw Underflow();
	}

	ListCell * tmp = privGetFirst();

	privPop( tmp );

	delete ( T * )tmp->data;
	tmp->data = 0;

	privDelete( tmp );

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_FIFO_H_

// ============================================================================

