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
//	TBFC.TL.ListTmpl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_ListTmpl_H_
#define _TBFC_TL_ListTmpl_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.TL.ListBase.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A fast and secure doubly linked list of elements of type \b T,
///	without insertion methods.
///
/// \ingroup TBFC_TL

template < typename T >
class ListTmpl : public ListBase {

public :

	/// \brief Creates a new empty list.

	ListTmpl(
	) {
	}

	/// \brief Creates a deep copy of \a __other.

	ListTmpl(
		const	ListTmpl< T >		& __other
	);

	/// \brief Destroys this object.

	virtual ~ListTmpl(
	);

	/// \brief Replaces the content of this object by a deep copy of
	///	\a __other.

	ListTmpl< T > & operator = (
		const	ListTmpl< T >		& __other
	);

	/// \brief Returns true iff this object contains an occurrence of
	///	\a __elt.

	Bool contains(
		const	T			& __elt
	) const;

	/// \brief Destroys the content of this object.

	void kill(
	);

	class CstIterator;

	/// \brief Iterator giving read/write access to the elements of a
	///	ListTmpl.

	class Iterator {

	public :

		Iterator(
		) : ptr( 0 ) {
		}

		Iterator(
			const	Iterator	& o
		) : ptr( o.ptr ) {
		}

		Iterator & operator = (
			const	Iterator	& o
		) {
			ptr = o.ptr;
			return * this;
		}

		~Iterator(
		) {
		}

		operator Bool (
		) const {
			return ( ptr != 0 );
		}

		Bool operator == (
			const	Iterator	& o
		) const {
			return ( ptr == o.ptr );
		}

		Bool operator != (
			const	Iterator	& o
		) const {
			return ( ptr != o.ptr );
		}

		T * operator -> (
		) {
			return ( T * )ptr->data;
		}

		T & operator * (
		) {
			return * ( T * )ptr->data;
		}

		void operator ++ (
				int
		) {
			ptr = ptr->next;
		}

		void operator ++ (
		) {
			ptr = ptr->next;
		}

		void operator -- (
				int
		) {
			ptr = ptr->prev;
		}

		void operator -- (
		) {
			ptr = ptr->prev;
		}

	private :

		friend class ListTmpl< T >;
		friend class ListTmpl< T >::CstIterator;

		ListCell	* ptr; ///< Cell we look at.

		Iterator(
				ListCell	* p
		) : ptr( p ) {
		}

	};

	/// \brief Iterator giving read only access to the elements of a
	///	ListTmpl.

	class CstIterator {

	public :

		CstIterator(
		) : ptr( 0 ) {
		}

		CstIterator(
			const	Iterator	& o
		) : ptr( o.ptr ) {
		}

		CstIterator(
			const	CstIterator	& o
		) : ptr( o.ptr ) {
		}

		~CstIterator(
		) {
		}

		CstIterator & operator = (
			const	CstIterator	& o
		) {
			ptr = o.ptr;
			return * this;
		}

		operator Bool (
		) const {
			return ( ptr != 0 );
		}

		const T * operator -> (
		) {
			return ( const T * )ptr->data;
		}

		const T & operator * (
		) {
			return * ( const T * )ptr->data;
		}

		void operator ++ (
				int
		) {
			ptr = ptr->next;
		}

		void operator ++ (
		) {
			ptr = ptr->next;
		}

		void operator -- (
				int
		) {
			ptr = ptr->prev;
		}

		void operator -- (
		) {
			ptr = ptr->prev;
		}

	private :

		friend class ListTmpl< T >;

		const ListCell	* ptr; ///< Cell we look at.

		CstIterator(
			const	ListCell	* p
		) : ptr( p ) {
		}

	};

	/// \brief Returns a CstIterator pointing to the first element in this
	///	object.

	CstIterator firstElt(
	) const {
		return CstIterator( privGetFirst() );
	}

	/// \brief Returns a CstIterator pointing to the last element in this
	///	object.

	CstIterator lastElt(
	) const {
		return CstIterator( privGetLast() );
	}

	/// \brief Returns an Iterator pointing to the first element in this
	///	object.

	Iterator firstElt(
	) {
		return Iterator( privGetFirst() );
	}

	/// \brief Returns an Iterator pointing to the last element in this
	///	object.

	Iterator lastElt(
	) {
		return Iterator( privGetLast() );
	}

	/// \brief Removes the element pointed to by \a it from this list.
	///
	/// The Iterator \a it is updated to point to the element at the
	/// right side of the element that was just removed, or to nothing
	/// if there is no such element.

	void pop(
			Iterator		& it
	);

	/// \brief Deletes the first element of this object.

	void deleteFirst(
	);

	/// \brief Deletes the last element of this object.

	void deleteLast(
	);

	/// \brief Returns a read only reference to the first element of this
	///	object.

	const T & getFirst(
	) const;

	/// \brief Returns a read only reference to the last element of this
	///	object.

	const T & getLast(
	) const;

	/// \brief Returns the first element of this list, after having
	///	removed it first.

	T popFirst(
	);

	/// \brief Returns the last element of this list, after having
	///	removed it first.

	T popLast(
	);

protected :

	void privInsertBefore(
			Iterator		& __it,
		const	T			& __elt
	) {
		privPushBefore( __it.ptr, privGetNew( new T( __elt ) ) );
	}

	void privInsertAfter(
			Iterator		& __it,
		const	T			& __elt
	) {
		privPushAfter( __it.ptr, privGetNew( new T( __elt ) ) );
	}

};

// ============================================================================

template < typename T >
ListTmpl< T >::ListTmpl(
	const	ListTmpl< T >		& o )

	: ListBase() {

	for ( CstIterator it = o.firstElt() ; it ; it++ ) {
		privPushAfter(
			privGetLast(),
			privGetNew( new T( * it ) ) );
	}

}

// ============================================================================

template < typename T >
ListTmpl< T >::~ListTmpl() {

	kill();

}

// ============================================================================

template < typename T >
ListTmpl< T > & ListTmpl< T >::operator = (
	const	ListTmpl< T >	& o ) {

	if ( this != & o ) {
		kill();
		for ( CstIterator it = o.firstElt() ; it ; it++ ) {
			privPushAfter(
				privGetLast(),
				privGetNew( new T( * it ) ) );
		}
	}

	return * this;

}

// ============================================================================

template < typename T >
Bool ListTmpl< T >::contains(
	const	T		& t ) const {

	for ( CstIterator it = firstElt() ; it ; it++ ) {
		if ( * it == t ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

template < typename T >
void ListTmpl< T >::kill() {

	while ( ! isEmpty() ) {
		deleteFirst();
	}

}

// ============================================================================

template < typename T >
void ListTmpl< T >::pop(
		Iterator	& it ) {

	ListCell * tmp = it.ptr;

	it.ptr = privPop( tmp );

	delete ( T * )tmp->data;
	tmp->data = 0;

	privDelete( tmp );

}

// ============================================================================

template < typename T >
void ListTmpl< T >::deleteLast() {

	ListCell * tmp = privGetLast();

	privPop( tmp );

	delete ( T * )tmp->data;
	tmp->data = 0;

	privDelete( tmp );

}

template < typename T >
void ListTmpl< T >::deleteFirst() {

	ListCell * tmp = privGetFirst();

	privPop( tmp );

	delete ( T * )tmp->data;
	tmp->data = 0;

	privDelete( tmp );

}

// ============================================================================

template < typename T >
const T & ListTmpl< T >::getLast() const {

	return * ( const T * )privGetLast()->data;

}

template < typename T >
const T & ListTmpl< T >::getFirst() const {

	return * ( const T * )privGetFirst()->data;

}

// ============================================================================

template < typename T >
T ListTmpl< T >::popLast() {

	T t = getLast();

	deleteLast();

	return t;

}

template < typename T >
T ListTmpl< T >::popFirst() {

	T t = getFirst();

	deleteFirst();

	return t;

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_ListTmpl_H_

// ============================================================================

