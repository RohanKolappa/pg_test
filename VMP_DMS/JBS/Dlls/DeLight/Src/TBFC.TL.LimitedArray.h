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
//	TBFC.TL.LimitedArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================


#ifndef _MY_LIMITED_ARRAY_H
#define _MY_LIMITED_ARRAY_H


// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================


#include "TBFC.TL.Array.h"
#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Types.h"


// ============================================================================

namespace TBFC {

// ============================================================================

/// A simple templated limited-size array.

///
/// \ingroup TBFC_TL

template <class T>
class /* TBFC_Common_DLL */ LimitedArray : public Array<T> {

public :

	LimitedArray(
		const unsigned int maxsize = 1024);

	LimitedArray(
		const LimitedArray<T>&
	);

	LimitedArray& operator = (
		const LimitedArray<T>&
	);

	LimitedArray<T>& operator += (
		const T&
	);

	void resize(
		const unsigned int newsize
	);

protected :

	unsigned int maxSize;

private :

};

// ============================================================================

/// \brief A LimitedArray of Buffer objects.

typedef LimitedArray< Buffer > LimitedBufferArray;

// ============================================================================

template <class T>
LimitedArray<T>::LimitedArray(
	const unsigned int maxsize)
	: Array<T>(0),
	  maxSize(maxsize) {

}

template <class T>
LimitedArray<T>::LimitedArray(const LimitedArray<T>& src) {

	maxSize = src.maxSize;

	if ((elts = src.elts) == 0) {
		data = (T *)0;
		return;
	}

	data = new T[elts];
	if (data == (T *)0) {
		elts = 0;
		return;
	}

	for (unsigned int i = 0 ; i < elts ; i++)
		data[i] = src.data[i];

}

template <class T>
LimitedArray<T>& LimitedArray<T>::operator = (
	const LimitedArray<T>& s) {

	if (this == &s)
		return *this;

	maxSize = s.maxSize;

	if (elts)
		Kill();

	if ((elts = s.elts) == 0)
		return *this;

	data = new T[elts];
	if (data == (T *)0) {
		elts = 0;
		return *this;
	}

	for (unsigned int i = 0 ; i < elts ; i++)
		data[i] = s.data[i];

	return *this;

}

template <class T>
LimitedArray<T>& LimitedArray<T>::operator += (const T& s) {

	if (!elts) {
		data = new T[1];
		if (data == (T *)0) {
			elts = 0;
			return *this;
		}
		data[0] = s;
		elts = 1;
		return *this;
	}

	if (elts == maxSize) {
		for (unsigned int i = 0 ; i < elts-1 ; i++)
			data[i] = data[i+1];
		data[elts-1] = s;
		return *this;
	}

	T *ptr = new T[elts + 1];
	if (ptr == (T *)0) {
		Kill();
		return *this;
	}

	for (unsigned int i = 0 ; i < elts ; i++)
		ptr[i] = data[i];

	ptr[elts] = s;
	elts++;
	delete[] data;
	data = ptr;

	return *this;

}

template <class T>
void LimitedArray<T>::resize(const unsigned int newSize) {

	unsigned int newsize = newSize;

	if (newsize > maxSize)
		newsize = maxSize;

	if (!newsize) {
		kill();
		return;
	}

	if (!elts) {
		data = new T[newsize];
		if (data == (T *)0)
			throw OutOfMemory();
		elts = newsize;
		return;
	}

	if (elts == newsize)
		return;

	T *ptr = new T[newsize];
	if (ptr == (T *)0) {
		kill();
		throw OutOfMemory();
	}

	unsigned int i = 0;
	while (i < elts && i < newsize) {
		ptr[i] = data[i];
		i++;
	}
	delete[] data;
	data = ptr;
	elts = newsize;

}

// ============================================================================

} // namespace TBFC

// ============================================================================


#endif // _MY_LIMITED_ARRAY_H


// ============================================================================


