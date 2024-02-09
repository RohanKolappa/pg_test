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
//	TBFC.TL.List.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_List_H_
#define _TBFC_TL_List_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.TL.Array.h"
#include "TBFC.TL.CompArray.h"
#include "TBFC.TL.ListTmpl.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A fast and secure doubly linked list of elements of type \b T.
///
/// \ingroup TBFC_TL

template < typename T >
class List : public ListTmpl< T > {

public :

	typedef typename ListTmpl< T >::Iterator	Iterator;
	typedef typename ListTmpl< T >::CstIterator	CstIterator;

	/// \brief Creates a new empty list.

	List(
	) {
	}

	/// \brief Creates a deep copy of \a __other.

	List(
		const	List< T >		& __other
	);

	/// \brief Creates a new list containing copies of all the elements of
	///	\a __array, in the same order.

	List(
		const	Array< T >		& __array
	);

	/// \brief Creates a new list containing copies of all the elements of
	///	\a __array, in the same order.

	List(
		const	CompArray< T >		& __array
	);

	/// \brief Destroys this object.

	virtual ~List(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a __other.

	List< T > & operator = (
		const	List< T >		& __other
	);

	/// \brief Appends a copy of \a __elt at the end of this object.

	void operator += (
		const	T			& __elt
	);

	/// \brief Appends a copy of \a __other at the end of this object.

	void operator += (
		const	List< T >		& __other
	);

	/// \brief Inserts a copy of \a __elt at the left side of the element
	///	pointed to by __it.
	///
	/// The iterator \a __it is left untouched.

	void insertBefore(
			Iterator		& __it,
		const	T			& __elt
	) {
		this->privInsertBefore( __it, __elt );
	}

	/// \brief Inserts a copy of \a __elt at the right side of the element
	///	pointed to by __it.
	///
	/// The iterator \a __it is left untouched.

	void insertAfter(
			Iterator		& __it,
		const	T			& __elt
	) {
		this->privInsertAfter( __it, __elt );
	}

	/// \brief Inserts a copy of \a __elt at the beginning of this object.

	void insertFirst(
		const	T			& __elt
	) {
		this->privPushBefore(
			this->privGetFirst(),
			this->privGetNew( new T( __elt ) ) );
	}

	/// \brief Inserts a copy of \a __other at the beginning of this object.

	void insertFirst(
		const	List< T >		& __other
	);

	/// \brief Appends a copy of \a __elt at the end of this object.

	void insertLast(
		const	T			& __elt
	) {
		this->privPushAfter(
			this->privGetLast(),
			this->privGetNew( new T( __elt ) ) );
	}

	/// \brief Appends a copy of \a __other at the end of this object.

	void insertLast(
		const	List< T >		& __other
	);

};

// ============================================================================

template < typename T >
List< T >::List(
	const	List< T >		& o )

	: ListTmpl< T >( o ) {



}

template < typename T >
List< T >::List(
	const	Array< T >		& o ) {

	for ( Uint32 i = 0 ; i < o.size() ; i++ ) {
		insertLast( o[ i ] );
	}

}

template < typename T >
List< T >::List(
	const	CompArray< T >		& o ) {

	for ( Uint32 i = 0 ; i < o.size() ; i++ ) {
		insertLast( o[ i ] );
	}

}

// ============================================================================

template < typename T >
List< T >::~List() {



}

// ============================================================================

template < typename T >
List< T > & List< T >::operator = (
	const	List< T >	& o ) {

	ListTmpl< T >::operator = ( o );

	return * this;

}

// ============================================================================

template < typename T >
void List< T >::operator += (
	const	T		& t ) {

	insertLast( t );

}

template < typename T >
void List< T >::operator += (
	const	List< T >	& t ) {

	Uint32		toCopy = t.getSize();
	CstIterator	it = t.firstElt();

	while ( toCopy-- ) {
		insertLast( * it );
		it++;
	}

}

// ============================================================================

template < typename T >
void List< T >::insertFirst(
	const	List< T >	& list ) {

	Uint32		toCopy = list.getSize();
	CstIterator	it = list.lastElt();

	while ( toCopy-- ) {
		insertFirst( * it );
		it--;
	}

}

template < typename T >
void List< T >::insertLast(
	const	List< T >	& list ) {

	Uint32		toCopy = list.getSize();
	CstIterator	it = list.firstElt();

	while ( toCopy-- ) {
		insertLast( * it );
		it++;
	}

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_List_H_

// ============================================================================

