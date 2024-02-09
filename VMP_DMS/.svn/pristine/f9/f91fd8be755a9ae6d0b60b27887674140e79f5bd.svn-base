// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::STL".
// 
// "BFC::STL" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::STL" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::STL"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.STL.SList.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_STL_SList_H_
#define _BFC_STL_SList_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.STL.SListPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.STL.FunctorPtr.h"

// ============================================================================

namespace BFC {
namespace STL {

// ============================================================================

/// \brief A simple doubly linked list of SPtr objects.
///
/// \ingroup BFC_STL

class BFC_Light_DLL SList : virtual public SObject {

public :

	/// \brief Constructs an empty list.

	SList(
	);

	/// \brief Constructs a deep copy of \a pOther.

	SList(
		const	SList &		pOther
	);

	/// \brief Destroys this object.
	///
	/// References to the values in the list and all iterators of this
	/// list become invalid.

	virtual ~SList(
	);

	/// \brief Replaces the content of this list by a deep copy of \a
	///	pOther.

	SList & operator = (
		const	SList &		pOther
	);

	/// \brief Returns the number of items in this list.

	Uint32 getSize(
	) const {
		return nbr;
	}

	/// \brief Returns true iff this list contains no item.

	Bool isEmpty(
	) const {
		return ( nbr == 0 );
	}

	/// \brief Returns true iff this list contains \a pVal.

	Bool contains(
		const	SPtr &		pVal
	) const;

	/// \brief Removes all items from this list.

	void kill(
	);

	BFC_PTR_DECL( BFC_Light_DLL, Cell )

	class BFC_Light_DLL Cell : virtual public SObject {
	
	public :
	
		Cell(
		) {
		}
	
		Cell(
			const	SPtr&		elt
		) : data( elt ) {
		}
	
		virtual ~Cell(
		) {
		}
	
		SPtr			data;
		CellPtr		prev;
		CellPtr		next;
	
	};

	class CstIterator;

	/// \brief Provides an STL-style non-const iterator for SList.
	///
	/// \sa SList::CstIterator

	class Iterator {

	public :

		/// \brief Constructs an uninitialized Iterator.
		///
		/// \sa SList::firstElt(), SList::lastElt()

		Iterator(
		) {
		}

		/// \brief Constructs a copy of \a pOther.

		Iterator(
			const	Iterator &	pOther
		) :
			ptr( pOther.ptr ) {
		}

		/// \brief Makes this Iterator points to the same item than
		///	\a pOther.

		Iterator & operator = (
			const	Iterator &	pOther
		) {
			if ( this != &pOther ) {
				ptr = pOther.ptr;
			}
			return *this;
		}

		/// \brief Returns true iff the current item is valid.

		operator Bool (
		) const {
			return ( Bool )ptr;
		}

		/// \brief Returns a modifiable reference to the current item's
		///	value.
		///
		/// \throws NullPointer

		SPtr & operator * (
		) {
			return ptr->data;
		}

		/// \brief Returns a pointer to the current item's value.
		///
		/// \throws NullPointer

		SPtr * operator -> (
		) const {
			return & ptr->data;
		}

		/// \brief (prefix) Advances this Iterator to the next item in
		///	the list.
		///
		/// \throws NullPointer

		void operator ++ (
		) {
			ptr = ptr->next;
		}

		/// \brief (postfix) Advances this Iterator to the next item in
		///	the list.
		///
		/// \throws NullPointer

		void operator ++ (
				int
		) {
			ptr = ptr->next;
		}

		/// \brief (prefix) Reverses this Iterator to the previous item
		///	in the list.
		///
		/// \throws NullPointer

		void operator -- (
		) {
			ptr = ptr->prev;
		}

		/// \brief (postfix) Reverses this Iterator to the previous item
		///	in the list.
		///
		/// \throws NullPointer

		void operator -- (
				int
		) {
			ptr = ptr->prev;
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this Iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	Iterator &	pOther
		) const {
			return ( ptr == pOther.ptr );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this Iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	Iterator &	pOther
		) const {
			return ( ptr != pOther.ptr );
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this Iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	CstIterator &	pOther
		) const {
			return ( ptr == pOther.ptr );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this Iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	CstIterator &	pOther
		) const {
			return ( ptr != pOther.ptr );
		}

	private :

		CellPtr	ptr;

		friend class SList;
		friend class SList::CstIterator;

		Iterator(
				CellPtr	p
		) :
			ptr( p ) {
		}

	};

	/// \brief Provides an STL-style const iterator for SList.
	///
	/// \sa SList::Iterator

	class CstIterator {

	public :

		/// \brief Constructs an uninitialized CstIterator.
		///
		/// \sa SList::constFirstElt(), SList::constLastElt()

		CstIterator(
		) {
		}

		/// \brief Constructs a copy of \a pOther.

		CstIterator(
			const	Iterator &	pOther
		) :
			ptr( pOther.ptr ) {
		}

		/// \brief Constructs a copy of \a pOther.

		CstIterator(
			const	CstIterator &	pOther
		) :
			ptr( pOther.ptr ) {
		}

		/// \brief Makes this Iterator points to the same item than
		///	\a pOther.

		CstIterator & operator = (
			const	CstIterator &	pOther
		) {
			if ( this != & pOther ) {
				ptr = pOther.ptr;
			}
			return *this;
		}

		/// \brief Returns true iff the current item is valid.

		operator Bool (
		) const {
			return ( Bool )ptr;
		}

		/// \brief Returns the current item's value.
		///
		/// \throws NullPointer

		const SPtr & operator * (
		) {
			return ptr->data;
		}

		/// \brief Returns a pointer to the current item's value.
		///
		/// \throws NullPointer

		const SPtr * operator -> (
		) const {
			return & ptr->data;
		}

		/// \brief (prefix) Advances this Iterator to the next item in
		///	the list.
		///
		/// \throws NullPointer

		void operator ++ (
		) {
			ptr = ptr->next;
		}

		/// \brief (postfix) Advances this Iterator to the next item in
		///	the list.
		///
		/// \throws NullPointer

		void operator ++ (
				int
		) {
			ptr = ptr->next;
		}

		/// \brief (prefix) Reverses this Iterator to the previous item
		///	in the list.
		///
		/// \throws NullPointer

		void operator -- (
		) {
			ptr = ptr->prev;
		}

		/// \brief (postfix) Reverses this Iterator to the previous item
		///	in the list.
		///
		/// \throws NullPointer

		void operator -- (
				int
		) {
			ptr = ptr->prev;
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this Iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	Iterator &	pOther
		) const {
			return ( ptr == pOther.ptr );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this Iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	Iterator &	pOther
		) const {
			return ( ptr != pOther.ptr );
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this Iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	CstIterator &	pOther
		) const {
			return ( ptr == pOther.ptr );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this Iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	CstIterator &	pOther
		) const {
			return ( ptr != pOther.ptr );
		}

	private :

		CellCPtr	ptr;

		friend class SList;
		friend class SList::Iterator;

		CstIterator(
				CellCPtr	p
		) :
			ptr( p ) {
		}

	};

	friend class CstIterator;

	/// \brief Returns an Iterator pointing to the first item in this list.
	/// 
	/// \sa constFirstElt(), lastElt()

	Iterator firstElt(
	) {
		return Iterator( first );
	}

	/// \brief Returns a CstIterator pointing to the first item in this list.
	/// 
	/// \sa firstElt(), constLastElt()

	CstIterator constFirstElt(
	) const {
		return CstIterator( CellCPtr( first ) );
	}

	/// \brief Returns an Iterator pointing to the last item in this list.
	/// 
	/// \sa firstElt(), constLastElt()

	Iterator lastElt(
	) {
		return Iterator( last );
	}

	/// \brief Returns an Iterator pointing to the last item in this list.
	/// 
	/// \sa firstElt(), constLastElt()

	CstIterator constLastElt(
	) const {
		return CstIterator( CellCPtr( last ) );
	}

	/// \brief Removes the item pointed to by the Iterator \a pIterator
	///	from this list.

	void pop(
			Iterator &	pIterator
	);

	/// \brief Inserts a copy of \a pValue before the item pointed to by
	///	\a pIterator.

	void insertBefore(
			Iterator &	pIterator,
		const	SPtr &		pValue
	);

	/// \brief Inserts a copy of \a pValue after the item pointed to by
	///	\a pIterator.

	void insertAfter(
			Iterator &	pIterator,
		const	SPtr &		pValue
	);

	/// \brief Inserts a copy of \a pValue at the begin of this list.

	void insertFirst(
		const	SPtr &		pValue
	);

	/// \brief Inserts a copy of \a pValue at the end of this list.

	void insertLast (
		const	SPtr &		pValue
	);

	/// \brief Removes the first item of this list.

	void deleteFirst(
	);

	/// \brief Removes the last item of this list.

	void deleteLast(
	);

	/// \brief Returns the first item of this list.

	SPtr getFirst(
	) const;

	/// \brief Returns the last item of this list.

	SPtr getLast(
	) const;

	void apply(
			FunctorPtr	pFunctor
	);

protected :

private :

	CellPtr	first;		///< First element.
	CellPtr	last;		///< Last element.
	Uint32		nbr;		///< Number of elements.

};

// ============================================================================

} // namespace STL
} // namespace BFC

// ============================================================================

#endif // _BFC_STL_SList_H_

// ============================================================================

