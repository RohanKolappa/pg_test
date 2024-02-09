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
//	BFC.Stack.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Stack_H_
#define _BFC_Stack_H_

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated stack.
///
/// \ingroup Common_Containers

template <class T>
class Stack {

public :

	Stack() : data(0), elts(0) {}
	Stack(const Stack&);
	virtual ~Stack();

	virtual Stack& operator = (const Stack&);

	virtual Bool operator == (const Stack<T>&) const;
	virtual Bool operator != (const Stack<T>&) const;

	virtual Stack<T>& operator += (const T&);
	virtual void push(const T& elt);
	virtual void pop();
	virtual const T& top() const;
	virtual T& top();

	virtual void kill();

	virtual Bool isEmpty() const;
	virtual Uint32 size() const;

	BASE_CLASS_GEN_EXCEPTION("BFC.Stack");
	CLASS_EXCEPTION(NoContent, "No content");

protected :

	struct elt {

		elt() : n(0) {}
		elt(const T& __t) : t(__t), n(0) {}
		elt(const T& __t, elt *__n) : t(__t), n(__n) {}
		T	t;
		elt	*n;

	}	*data;	// pointer to the data (NULL if no data)
	Uint32	elts;	// number of elements pointed to by `data'

};

// ============================================================================

// typedef Stack<Buffer> BufferStack;

// ============================================================================

template <class T>
Stack<T>::Stack(const Stack& src) {

	data = 0;

	if ((elts = src.elts) == 0)
		return;

	T	*srcData = src.data,
		*temp;
	elt	*prev = 0;

	while (srcData) {
		temp = new elt(srcData->t);
		if (!prev)
			data = temp;
		else
			prev->n = temp;
		prev = temp;
	}

}

template <class T>
Stack<T>::~Stack() {

	elt *temp;

	while (data) {
		elts--;
		temp = data->n;
		delete data;
		data = temp;
	}

}

template <class T>
Stack<T>& Stack<T>::operator = (const Stack<T>& s) {

	if (this == &s)
		return *this;

	if (elts)
		kill();

	if ((elts = s.elts) == 0)
		return *this;

	elt	*srcData = s.data,
		*prev = 0,
		*temp;

	while (srcData) {
		temp = new elt(srcData->t);
		if (!prev)
			data = temp;
		else
			prev->n = temp;
		prev = temp;
	}

	return *this;

}

template <class T>
Bool Stack<T>::operator == (const Stack<T>& src) const {

	if (elts != src.elts)
		return false;

	if (!elts)
		return true;

	elt	*aPtr = data,
		*bPtr = src.data;

	while (aPtr) {
		if (aPtr->t != bPtr->t)
			return false;
		aPtr = aPtr->n;
		bPtr = bPtr->n;
	}

	return true;

}

template <class T>
Bool Stack<T>::operator != (const Stack<T>& src) const {

	return (!(*this == src));

}

template <class T>
Stack<T>& Stack<T>::operator += (const T& t) {

	push(t);

	return *this;

}

template <class T>
void Stack<T>::push(const T& t) {

	data = new elt( t, data );

	elts++;

}

template <class T>
void Stack<T>::pop() {

	if ( ! elts )
		return;

	elts--;

	elt *p = data->n;
	delete data;
	data = p;

}

template <class T>
const T& Stack<T>::top() const {

	if ( ! data )
		throw NoContent();

	return data->t;

}

template <class T>
T& Stack<T>::top() {

	if ( ! data )
		throw NoContent();

	return data->t;

}

template <class T>
void Stack<T>::kill() {

	elt *temp;

	while (data) {
		elts--;
		temp = data->n;
		delete data;
		data = temp;
	}

}

template <class T>
Bool Stack<T>::isEmpty() const {

	return (elts == 0);

}

template <class T>
Uint32 Stack<T>::size() const {

	return elts;

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Stack_H_

// ============================================================================

