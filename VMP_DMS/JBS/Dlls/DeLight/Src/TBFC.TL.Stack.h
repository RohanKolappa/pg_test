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
//	TBFC.Stack.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_Stack_H_
#define _TBFC_TL_Stack_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.TL.ListBase.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A simple templated stack.
///
/// \ingroup TBFC_TL

template < typename T >
class Stack : public ListBase {

public :

	Stack(
	) {
	}

	Stack(
		const	Stack< T >	&
	);

	virtual ~Stack(
	);

	Stack< T > & operator = (
		const	Stack< T >	&
	);

	Bool operator == (
		const	Stack< T >	&
	) const;

	Bool operator != (
		const	Stack< T >	&
	) const;

	Stack< T > & operator += (
		const	T		&
	);

	void push(
		const	T		& elt
	);

	T pop(
	);

	const T & top(
	) const;

	T & top(
	);

	void kill(
	);

	BASE_CLASS_GEN_EXCEPTION( "TBFC.TL.Stack" );

	CLASS_EXCEPTION( Underflow, "Underflow" );

};

// ============================================================================

template < typename T >
Stack< T >::Stack(
	const	Stack< T >	& src ) {

	for ( ListCell * ptr = src.privGetFirst() ; ptr ; ptr = ptr->next ) {
		push( * ( const T * )ptr->data );
	}

}

// ============================================================================

template < typename T >
Stack< T >::~Stack() {

	kill();

}

// ============================================================================

template < typename T >
Stack< T > & Stack< T >::operator = (
	const	Stack< T >	& src ) {

	if ( this != & src ) {
		kill();
		for ( ListCell * ptr = src.privGetFirst() ; ptr ; ptr = ptr->next ) {
			push( * ( const T * )ptr->data );
		}
	}

	return *this;

}

// ============================================================================

template < typename T >
Bool Stack< T >::operator == (
	const	Stack< T >	& src ) const {

	if ( this == & src ) {
		return true;
	}

	if ( getSize() != src.getSize() ) {
		return false;
	}

	ListCell	* ptr1;
	ListCell	* ptr2;

	for ( ptr1 = privGetFirst(), ptr2 = src.privGetFirst()
	    ; ptr1 && ptr2
	    ; ptr1 = ptr1->next, ptr2 = ptr2->next ) {
		if ( * ( const T * )ptr1->data != * ( const T * )ptr2->data ) {
			return false;
		}
	}

	return true;

}

template < typename T >
Bool Stack< T >::operator != (
	const	Stack< T >	& src ) const {

	return ( ! ( * this == src ) );

}

// ============================================================================

template < typename T >
Stack< T > & Stack< T >::operator += (
	const	T		& t ) {

	push( t );

	return * this;

}

template < typename T >
void Stack< T >::push(
	const	T		& t ) {

	privPushAfter( privGetLast(), privGetNew( new T( t ) ) );

}

template < typename T >
T Stack< T >::pop() {

	ListCell * tmp = privGetLast();

	if ( ! tmp ) {
		throw Underflow();
	}

	T res = * ( T * )tmp->data;

	privPop( tmp );

	delete ( T * )tmp->data;
	tmp->data = 0;

	privDelete( tmp );

	return res;

}

template < typename T >
const T & Stack< T >::top() const {

	ListCell * tmp = privGetLast();

	if ( ! tmp ) {
		throw Underflow();
	}

	return *( const T * )tmp->data;

}

template < typename T >
T & Stack< T >::top() {

	ListCell * tmp = privGetLast();

	if ( ! tmp ) {
		throw Underflow();
	}

	return *( T * )tmp->data;

}

// ============================================================================

template < typename T >
void Stack< T >::kill() {

	while ( getSize() ) {
		pop();
	}

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_Stack_H_

// ============================================================================

