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
//	BFC.TL.SortedList.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_SortedList_H_
#define _BFC_TL_SortedList_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.TL.Array.h"
#include "BFC.TL.CompArray.h"
#include "BFC.TL.ListTmpl.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A sorted list of elements of type \b T.
///
/// The sorting algorithm used by this class has complexity O(N)=N.
///
/// \sa BFC::SortedArray
///
/// \ingroup BFC_TL

template < typename T >
class SortedList : public ListTmpl< T > {

public :

	typedef typename ListTmpl< T >::Iterator	Iterator;
	typedef typename ListTmpl< T >::CstIterator	CstIterator;

	/// \brief Creates a new empty list.

	SortedList(
	) {
	}

	/// \brief Creates a deep copy of \a __other.

	SortedList(
		const	SortedList< T >		& __other
	);

	/// \brief Creates a new sorted list containing copies of all the
	///	elements of \a __array.

	SortedList(
		const	Array< T >		& __array
	);

	/// \brief Creates a new sorted list containing copies of all the
	///	elements of \a __array.

	SortedList(
		const	CompArray< T >		& __array
	);

	/// \brief Destroys this object.

	virtual ~SortedList(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a __other.

	SortedList< T > & operator = (
		const	SortedList< T >		& __other
	);

	/// \brief Same as insert( \a __elt ).

	void operator += (
		const	T			& __elt
	) {
		insert( __elt );
	}

	/// \brief Same as insert( \a __other ).

	void operator += (
		const	SortedList< T >		& __other
	) {
		insert( __other );
	}

	/// \brief Inserts a copy of \a __elt in this object.

	void insert(
		const	T			& __elt
	);

	/// \brief Inserts copies of the elements of \a __other in this object.

	void insert(
		const	SortedList< T >		& __other
	);

	/// \brief Same as remove( \a __elt ).

	void operator -= (
		const	T			& __elt
	) {
		remove( __elt );
	}

	/// \brief Same as remove( \a __other ).

	void operator -= (
		const	SortedList< T >		& __other
	) {
		remove( __other );
	}

	/// \brief Removes element \a __elt from this object.

	void remove(
		const	T			& __elt
	);

	/// \brief Removes the elements of \a __other from this object.

	void remove(
		const	SortedList< T >		& __other
	);

	//	For the exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.TL.SortedList" );

	CLASS_EXCEPTION( NoSuchElement, "No such element" );

};

// ============================================================================

template < typename T >
SortedList< T >::SortedList(
	const	SortedList< T >		& o )

	: ListTmpl< T >( o ) {



}

template < typename T >
SortedList< T >::SortedList(
	const	Array< T >		& o ) {

	for ( Uint32 i = 0 ; i < o.size() ; i++ ) {
		insert( o[ i ] );
	}

}

template < typename T >
SortedList< T >::SortedList(
	const	CompArray< T >		& o ) {

	for ( Uint32 i = 0 ; i < o.size() ; i++ ) {
		insert( o[ i ] );
	}

}

// ============================================================================

template < typename T >
SortedList< T >::~SortedList() {



}

// ============================================================================

template < class T >
void SortedList< T >::insert(
	const	T		& t ) {

	for ( Iterator it = this->firstElt() ; it ; it++ ) {

		if ( *it == t ) {
			return;
		}

		if ( *it > t ) {
			privInsertBefore( it, t );
			return;
		}

	}

	privPushAfter(
		this->privGetLast(),
		privGetNew( new T( t ) ) );

}

template < class T >
void SortedList< T >::insert(
	const	SortedList< T >	& l ) {

	for ( CstIterator it = l.firstElt() ; it ; it++ ) {
		insert( * it );
	}

}

// ============================================================================

template < class T >
void SortedList< T >::remove(
	const	T		& t ) {

	for ( Iterator it = this->firstElt() ; it ; it++ ) {

		if ( *it == t ) {
			pop( it );
			return;
		}

	}

	throw NoSuchElement();

}

template < class T >
void SortedList< T >::remove(
	const	SortedList< T >&	l ) {

	for ( Iterator it = l.firstElt() ; it ; it++ ) {
		remove( * it );
	}

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_SortedList_H_

// ============================================================================

