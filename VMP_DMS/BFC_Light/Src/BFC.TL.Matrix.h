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
//	BFC.TL.Matrix.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_Matrix_H_
#define _BFC_TL_Matrix_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated matrix.
///
/// \ingroup BFC_TL

template < typename T >
class Matrix {

public :

	/// Creates a \a cols by \a rows matrix.

	Matrix(
		const	Uint32		cols = 0,
		const	Uint32		rows = 0
	);

	/// Copy constructor.

	Matrix(
		const	Matrix< T >	& o
	);

	/// Destructor.

	virtual ~Matrix(
	);

	/// Copy operator.

	Matrix< T > & operator = (
		const	Matrix< T >	& o
	);

	/// Destroys the content of this object, reducing its size to 0.

	void kill(
	);

	void resize(
		const	Uint32		cols,
		const	Uint32		rows
	);

	/// Returns `true' iff this object is empty (cols == rows == 0).

	Bool isEmpty(
	) const;

	Uint32 getCols(
	) const;

	Uint32 getRows(
	) const;

	const T & operator () (
		const	Uint32		col,
		const	Uint32		row
	) const;

	T & operator () (
		const	Uint32		col,
		const	Uint32		row
	);

	// For the exceptions...

	BASE_CLASS_GEN_EXCEPTION("Matrix");

	CLASS_EXCEPTION(IndexOutOfRange, "Index out of range");

protected :

	T		* data;	// pointer to the data (NULL if no data)
	Uint32		elts;	// number of elements pointed to by `data'
	Uint32		cols;
	Uint32		rows;

};

// ============================================================================

template < typename T >
Matrix< T >::Matrix(
	const	Uint32		__cols,
	const	Uint32		__rows) {

	cols = __cols;
	rows = __rows;
	elts = __cols * __rows;

	if ( ! elts ) {
		data = 0;
	}
	else {
		data = new T[elts];
	}

}

template < typename T >
Matrix< T >::Matrix(
	const	Matrix< T >&	o) {

	cols = o.cols;
	rows = o.rows;
	elts = o.elts;

	if ( ! elts ) {
		data = 0;
	}
	else {
		data = new T[elts];
		for ( Uint32 c = 0 ; c < elts ; c++ ) {
			data[c] = o.data[c];
		}
	}

}

template < typename T >
Matrix< T >::~Matrix() {

	if ( elts ) {
		delete[] data;
	}

}

template < typename T >
Matrix< T >& Matrix< T >::operator = (
	const	Matrix< T >&	o) {

	if ( this == &o ) {
		return *this;
	}

	if ( elts ) {
		delete[] data;
	}

	cols = o.cols;
	rows = o.rows;
	elts = o.elts;

	if ( ! elts ) {
		data = 0;
	}
	else {
		data = new T[elts];
		for ( Uint32 c = 0 ; c < elts ; c++ ) {
			data[c] = o.data[c];
		}
	}

	return *this;

}

template < typename T >
void Matrix< T >::kill() {

	if ( elts ) {
		delete[] data;
		data = 0;
		cols = 0;
		rows = 0;
		elts = 0;
	}

}

template < typename T >
void Matrix< T >::resize(
	const	Uint32		newCols,
	const	Uint32		newRows) {

	Uint32	newElts = newCols * newRows;

	if ( ! newElts ) {
		kill();
		return;
	}

	if ( ! elts ) {
		data = new T[newElts];
		cols = newCols;
		rows = newRows;
		elts = newElts;
		return;
	}

	if ( cols == newCols && rows == newRows ) {
		return;
	}

	Uint32	oldCols = cols;
	Uint32	oldRows = rows;

	T *newData = new T[newElts];

	for ( Uint32 i = 0 ; i < oldCols && i < newCols ; i++ )
	for ( Uint32 j = 0 ; j < oldRows && j < newRows ; j++ ) {
		newData[i + j * newCols] = data[i + j * oldCols];
	}

	delete[] data;

	data = newData;
	cols = newCols;
	rows = newRows;
	elts = newElts;

}

template < typename T >
Bool Matrix< T >::isEmpty() const {

	return ( ! elts );

}

template < typename T >
Uint32 Matrix< T >::getCols() const {

	return cols;

}

template < typename T >
Uint32 Matrix< T >::getRows() const {

	return rows;

}

template < typename T >
const T& Matrix< T >::operator () (
	const	Uint32		col,
	const	Uint32		row) const {

	if ( col >= cols || row >= rows ) {
		throw IndexOutOfRange();
	}

	return data[col + row * cols];

}

template < typename T >
T& Matrix< T >::operator () (
	const	Uint32		col,
	const	Uint32		row) {

	if ( col >= cols || row >= rows ) {
		throw IndexOutOfRange();
	}

	return data[col + row * cols];

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_Matrix_H_

// ============================================================================

