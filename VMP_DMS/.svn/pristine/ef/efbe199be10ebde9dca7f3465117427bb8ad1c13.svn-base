// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.TL.List.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_List_H_
#define _BFC_List_H_

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated doubly linked list.
///
/// \ingroup Common_Containers

template <class T>
class List : virtual public SObject {

public :

	List() : first(0), last(0), nbr(0) {}
	virtual ~List() {kill();}

	List<T>& operator = (const List<T>&);

	Bool isEmpty() const {return (nbr == 0);}
	Bool contains( const T& ) const;
	Uint32 size() const {return nbr;}
	void kill();
	List<T>& operator += (const T& t);
	const T& operator [] (const Uint32) const;
	T& operator [] (const Uint32);

	class Cell {
	public :
		// constructor & destructor
		Cell() : prev(0), next(0) {}
		Cell(const T& elt) : data(elt), prev(0), next(0) {}
		~Cell() {}
		// content...
		T	data;
		Cell	*prev;	// previous element in linked list
		Cell	*next;	// next element in linked list
	};

	class Iterator {
	public :
		Iterator() : ptr(0) {}
		Iterator(const Iterator& o) : ptr(o.ptr) {}
		Iterator& operator = (const Iterator& o) {if (this != &o) {ptr = o.ptr;} return *this;}
		operator bool () const {return ptr != 0;}
		T *operator -> () {return &ptr->data;}
		T& operator *  () {return ptr->data;}
		void operator ++ (int) {ptr = ptr->next;}
		void operator ++ () {ptr = ptr->next;}
		void operator -- (int) {ptr = ptr->prev;}
		void operator -- () {ptr = ptr->prev;}
	private :
		Cell *ptr;
		friend class List<T>;
		Iterator(Cell *p) : ptr(p) {}
	};

	Iterator firstElt() const {return Iterator(first);}
	Iterator lastElt() const {return Iterator(last);}

	void pop(Iterator&);
	void insertBefore(Iterator&, const T& t);
	void insertAfter(Iterator&, const T& t);

protected :

private :

	List(const List<T>&);

	// internal items representation...

	Cell		*first;		// OK, ...
	Cell		*last;		// it's a doubly linked list...
	Uint32		nbr;

};

template <class T>
List<T>& List<T>::operator = (
	const	List<T>&	o) {

	if (this == &o) {
		return *this;
	}

	kill();

	Iterator it = o.firstElt();

	while (it) {
		operator += (*it);
		it++;
	}

	return *this;

}

template <class T>
Bool List<T>::contains(
	const	T&		t ) const {

	for ( Iterator it = firstElt() ; it ; it++ ) {
		if ( *it == t ) {
			return true;
		}
	}

	return false;

}

template <class T>
void List<T>::kill() {

	Cell	*ptr = first, *tmp;

	while (ptr) {
		tmp = ptr->next;
		delete ptr;
		ptr = tmp;
	}

	first = last = 0;
	nbr = 0;

}

template <class T>
List<T>& List<T>::operator += (const T &t) {

	Cell *ptr = new Cell(t);

	ptr->prev = last;

	if (!last)
		first = ptr;
	else
		last->next = ptr;
	last = ptr;

	nbr++;

	return *this;

}

template <class T>
const T& List<T>::operator [] (const Uint32 i) const {

	Cell	*ptr = first;

	Uint32 count = i;

	while (count--)
		ptr = ptr->next;

	return ptr->data;

}

template <class T>
T& List<T>::operator [] (const Uint32 i) {

	Cell	*ptr = first;

	Uint32 count = i;

	while (count--)
		ptr = ptr->next;

	return ptr->data;

}

template <class T>
void List<T>::pop(Iterator& it) {

	// assert ptr != 0

	Cell *p = it.ptr;

	if (p->prev)
		p->prev->next = p->next;
	else
		first = p->next;

	if (p->next) {
		it.ptr = p->next;
		p->next->prev = p->prev;
	}
	else {
		it.ptr = p->prev;
		last = p->prev;
	}

	nbr--;

	delete p;

}

template <class T>
void List<T>::insertBefore(Iterator& it, const T& t) {

	// assert ptr != 0

	Cell *cur = it.ptr;
	Cell *ptr = new Cell(t);

	ptr->prev = cur->prev;
	ptr->next = cur;

	if (cur->prev)
		cur->prev->next = ptr;
	else
		first = ptr;

	cur->prev = ptr;

	nbr++;

}

template <class T>
void List<T>::insertAfter(Iterator& it, const T& t) {

	// assert ptr != 0

	Cell *cur = it.ptr;
	Cell *ptr = new Cell(t);

	ptr->prev = cur;
	ptr->next = cur->next;

	if (cur->next)
		cur->next->prev = ptr;
	else
		last = ptr;

	cur->next = ptr;

	nbr++;

}

// ============================================================================

} // namespace BFC

#endif // __LIST_H__

