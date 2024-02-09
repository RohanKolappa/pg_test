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
//	BFC.TL.Array.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Array_H_
#define _BFC_Array_H_

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated array.
///
/// \ingroup Common_Containers

template < typename T >
class Array : virtual public SObject {

public :

	//
	//	Array( const Uint32 len = 0 );
	//
	//	Creates an empty array ( if arg == 0 ( default ) ) or an array
	//	that contains `len' elements of type `T' ( uninitialized,
	//	unless the empty constructor for type T does the initiali-
	//	zation ).
	//

	Array( const Uint32 = 0 );

	//
	//	Array( const Array& src );
	//
	//	Creates a new array whose content is copied from `src'.
	//

	Array( const Array<T>& );

	//
	//	~Array();
	//
	//	Destroys the array by deleting its content.
	//

	virtual ~Array();

	//
	//	Array& operator = ( const Array& src );
	//
	//	Deletes the content of this object and replaces it by a
	//	copy of the content of `src'.
	//	Returns a reference to this object.
	//

	virtual Array<T>& operator = ( const Array<T>& );

//	virtual Bool operator == ( const Array<T>& ) const;

//	virtual Bool operator != ( const Array<T>& ) const;

	//
	//	Array& operator += ( const T& elt );
	//
	//	Appends a copy of element `elt' to this object ( at the
	//	end ), thus increasing its size by 1, and returns a reference
	//	to this object.
	//	This object may be empty before this call.
	//

	virtual Array<T>& operator += ( const T& );

	virtual Array<T>& operator += ( const Array<T>& );

//	virtual Array<T>& operator -= ( const T& );

	//
	//	void Pop( const Uint32 index );
	//

	virtual void pop( const Uint32 index );

	//
	//	void insert( const Uint32 index,
	//	            const T&           value );
	//

	virtual void insert( 
		const Uint32 index,
		const T& value
	 );

	virtual void insert( 
		const	Uint32		index,
		const	Array<T>&	array
	 );

	//
	//	void kill();
	//
	//	Destroys the content of this object, reducing its size to 0.
	//

	virtual void kill();

	//
	//	Bool Resize( const Uint32 len );
	//
	//	If `len' is 0, kills this object and returns true.
	//	Else, if this object is empty, creates an array that
	//	contains `len' objects of type `T' ( uninitialized, or initia-
	//	lized by the empty constructor for type `T' ) and returns
	//	`false' only in case of allocation failure.
	//	Else, if the actual size of this object is less than `len',
	//	reallocates memory in order for this object to contain
	//	`len' objects ( the actual content is NOT destroyed and the
	//	new elements are uninitialized,... ), and returns `false'
	//	only in case of allocation failure.
	//	Else, reduces the size of this object by deleting the last
	//	elements and returns `false' only in case of allocation
	//	failure.
	//	

	virtual void resize( const Uint32 );

	//
	//	Bool IsEmpty() const;
	//
	//	Returns `true' iff this object is empty.
	//

	virtual Bool isEmpty() const;

	//
	//	Uint32 Size() const;
	//
	//	Returns the number of elements in this object.
	//

	virtual Uint32 size() const;

	//
	//	T& operator [] ( const Uint32 index );
	//
	//	Returns a reference to element `index' if index is < Size()
	//	or a reference to a dummy object of type `T' otherwise.
	//	This reference can be used as a Lvalue in an assignement.
	//

	virtual T& operator [] ( const Uint32 i ) {return elt( i );}

	virtual const T& operator [] ( const Uint32 i ) const {return elt( i );}

	virtual T& elt( const Uint32 );

	virtual const T& elt( const Uint32 ) const;

//	virtual Uint32 indexOf( const T& ) const;

//	virtual Bool isIn( const T& ) const;

//	virtual Bool isIn( const T&, Uint32& index ) const;

	//
	//	constT& operator [] ( const Uint32 index ) const;
	//
	//	Returns a reference to element `index' if index is < Size()
	//	or a reference to a dummy object of type `T' otherwise.
	//	This reference can only be used as a Rvalue in an expression.
	//

	virtual const T& operator () ( const Uint32 ) const;

	//	For the exceptions...

	BASE_CLASS_GEN_EXCEPTION( "Array" );

	CLASS_EXCEPTION( IndexOutOfRange, "Index out of range" );
	CLASS_EXCEPTION( NoSuchElement, "No such element" );
	CLASS_EXCEPTION( OutOfMemory, "Out of virtual memory" );

protected :

	T	*data;	// pointer to the data ( NULL if no data )
	Uint32	elts;	// number of elements pointed to by `data'

};

// ============================================================================

template <class T>
Array<T>::Array( const Uint32 e ) {

	elts = 0;

	if ( e == 0 ) {
		data = 0;
		return;
	}

	data = new T[e];
	if ( !data )
		throw OutOfMemory();

	elts = e;

}

template <class T>
Array<T>::Array( const Array<T>& src )
	: SObject() {

	if ( ( elts = src.elts ) == 0 ) {
		data = 0;
		return;
	}

	data = new T[elts];
	if ( !data ) {
		elts = 0;
		throw OutOfMemory();
	}

	for ( Uint32 i = 0 ; i < elts ; i++ )
		data[i] = src.data[i];

}

template <class T>
Array<T>::~Array() {

	if ( elts )
		delete[] data;

}

template <class T>
Array<T>& Array<T>::operator = ( const Array<T>& s ) {

	if ( this == &s )
		return *this;

	if ( elts )
		kill();

	if ( ( elts = s.elts ) == 0 )
		return *this;

	data = new T[elts];

	for ( Uint32 i = 0 ; i < elts ; i++ )
		data[i] = s.data[i];

	return *this;

}

template <class T>
Array<T>& Array<T>::operator += ( const T& s ) {

	if ( !elts ) {
		data = new T[1];
		if ( !data ) {
			elts = 0;
			return *this;
		}
		data[0] = s;
		elts = 1;
		return *this;
	}

	T *ptr = new T[elts + 1];
	if ( !ptr ) {
		kill();
		throw OutOfMemory();
	}

	for ( Uint32 i = 0 ; i < elts ; i++ )
		ptr[i] = data[i];

	ptr[elts] = s;
	elts++;
	delete[] data;
	data = ptr;

	return *this;

}

template <class T>
Array<T>& Array<T>::operator += ( const Array<T>& s ) {

	if ( !s.elts ) {
		return *this;
	}

	if ( this == &s ) {
		T *ptr = new T[2 * elts];
		Uint32 i;
		for ( i = 0 ; i < elts ; i++ )
			ptr[i] = ptr[i + elts] = data[i];
		elts *= 2;
		delete[] data;
		data = ptr;
		return *this;
	}

	if ( !elts ) {
		data = new T[s.elts];
		if ( !data ) {
			elts = 0;
			return *this;
		}
		elts = s.elts;
		for ( Uint32 i = 0 ; i < elts ; i++ )
			data[i] = s.data[i];
		return *this;
	}

	T *ptr = new T[elts + s.elts];
	if ( !ptr ) {
		kill();
		throw OutOfMemory();
	}

	Uint32 i;

	for ( i = 0 ; i < elts ; i++ )
		ptr[i] = data[i];

	for ( i = 0 ; i < s.elts ; i++ )
		ptr[i + elts] = s.data[i];

	elts += s.elts;

	delete[] data;
	data = ptr;

	return *this;

}

template <class T>
void Array<T>::pop( const Uint32 index ) {

	if ( index >= elts )
		throw IndexOutOfRange();

	if ( elts == 1 ) {
		kill();
		return;
	}

	T *ptr = new T[elts - 1];
	if ( !ptr ) {
		kill();
		throw OutOfMemory();
	}

	if ( index )
		for ( Uint32 i = 0 ; i < index ; i++ )
			ptr[i] = data[i];

	for ( Uint32 i = index ; i < elts - 1 ; i++ )
		ptr[i] = data[i+1];

	delete[] data;
	data = ptr;
	elts--;

}

template <class T>
void Array<T>::insert( 
	const Uint32 index,
	const T& value ) {

	if ( index > elts )
		throw IndexOutOfRange();

	if ( index == elts ) {
		*this += value;
		return;
	}

	T *ptr = new T[elts + 1];
	if ( !ptr ) {
		kill();
		throw OutOfMemory();
	}

	if ( index )
		for ( Uint32 i = 0 ; i < index ; i++ )
			ptr[i] = data[i];

	ptr[index] = value;

	for ( Uint32 i = index ; i < elts ; i++ )
		ptr[i+1] = data[i];

	delete[] data;
	data = ptr;
	elts++;

}

template <class T>
void Array<T>::insert( 
	const	Uint32		index,
	const	Array<T>&	array ) {

	if ( index > elts )
		throw IndexOutOfRange();

	if ( !array.elts ) {
		return;
	}

	if ( index == elts ) {
		*this += array;
		return;
	}

	T *ptr = new T[elts + array.elts];

	if ( !ptr ) {
		kill();
		throw OutOfMemory();
	}

	Uint32 i = 0;

	while ( i < index ) {
		ptr[i] = data[i];
		i++;
	}

	while ( i < index + array.elts ) {
		ptr[i] = array.data[i - index];
		i++;
	}

	while ( i < elts + array.elts ) {
		ptr[i] = data[i - array.elts];
		i++;
	}

	delete[] data;
	data = ptr;
	elts += array.elts;

}

template <class T>
void Array<T>::kill() {

	if ( !elts )
		return;

	delete[] data;
	data = 0;
	elts = 0;

}

template <class T>
void Array<T>::resize( const Uint32 newsize ) {

	if ( !newsize ) {
		kill();
		return;
	}

	if ( !elts ) {
		data = new T[newsize];
		if ( !data )
			throw OutOfMemory();
		elts = newsize;
		return;
	}

	if ( elts == newsize )
		return;

	T *ptr = new T[newsize];
	if ( !ptr ) {
		kill();
		throw OutOfMemory();
	}

	Uint32 i = 0;
	while ( i < elts && i < newsize ) {
		ptr[i] = data[i];
		i++;
	}
	delete[] data;
	data = ptr;
	elts = newsize;

}

template <class T>
Bool Array<T>::isEmpty() const {

	return ( elts == 0 );

}

template <class T>
Uint32 Array<T>::size() const {

	return elts;

}

template <class T>
T& Array<T>::elt( const Uint32 l ) {

	if ( l >= elts )
		throw IndexOutOfRange();

	return data[l];

}

template <class T>
const T& Array<T>::elt( const Uint32 l ) const {

	if ( l >= elts )
		throw IndexOutOfRange();

	return data[l];

}

template <class T>
const T& Array<T>::operator () ( const Uint32 l ) const {

	if ( l >= elts )
		throw IndexOutOfRange();

	return data[l];

}

#if 0

template <class T>
Uint32 Array<T>::indexOf( const T& t ) const {

	if ( !elts )
		throw NoSuchElement();

	for ( Uint32 i = 0 ; i < elts ; i++ ) {
		if ( data[i] == t )
			return i;
	}

	throw NoSuchElement();

}

template <class T>
Bool Array<T>::isIn( const T& t ) const {

	if ( !elts )
		return false;

	for ( Uint32 i = 0 ; i < elts ; i++ ) {
		if ( data[i] == t )
			return true;
	}

	return false;

}

template <class T>
Bool Array<T>::isIn( const T& t, Uint32& i ) const {

	if ( !elts )
		return false;

	for ( i = 0 ; i < elts ; i++ ) {
		if ( data[i] == t )
			return true;
	}

	return false;

}

#endif

// ============================================================================

template < typename T >
class CompArray : public Array< T > {

public :

	CompArray( 
		const	Uint32		s = 0
	 ) : Array<T>( s ) {
	}

	CompArray( 
		const	CompArray<T>&	s
	 ) : SObject(), Array<T>( s ) {
	}

	virtual ~CompArray() {}

	CompArray<T>& operator = ( 
		const	CompArray<T>&
	 );

	CompArray<T>& operator -= ( const T& );

	Bool operator == ( const CompArray<T>& ) const;

	Bool operator != ( const CompArray<T>& ) const;

	Uint32 indexOf( const T& ) const;

	Bool isIn( const T& ) const;

	Bool isIn( const T&, Uint32& index ) const;

protected :

private :

};

// ============================================================================

template < typename T >
CompArray<T>& CompArray<T>::operator -= ( const T& s ) {

	if ( ! this->elts )
		throw typename Array<T>::NoSuchElement();

	for ( Uint32 i = 0 ; i < this->elts ; i++ ) {
		if ( this->data[i] == s ) {
			Array<T>::pop( i );
			return *this;
		}
	}

	throw typename Array<T>::NoSuchElement();

}

template < typename T >
Bool CompArray<T>::operator == ( const CompArray<T>& src ) const {

	if ( this->elts != src.elts )
		return false;

	if ( !this->elts )
		return true;

	for ( Uint32 i = 0 ; i < this->elts ; i++ )
		if ( this->data[i] != src.data[i] )
			return false;

	return true;

}

template < typename T >
Bool CompArray<T>::operator != ( const CompArray<T>& src ) const {

	if ( this->elts != src.elts )
		return true;

	if ( !this->elts )
		return false;

	for ( Uint32 i = 0 ; i < this->elts ; i++ )
		if ( this->data[i] != src.data[i] )
			return true;

	return false;

}

template < typename T >
Uint32 CompArray<T>::indexOf( const T& t ) const {

	if ( !this->elts )
		throw Array<T>::NoSuchElement();

	for ( Uint32 i = 0 ; i < this->elts ; i++ ) {
		if ( this->data[i] == t )
			return i;
	}

	throw Array<T>::NoSuchElement();

}

template < typename T >
Bool CompArray<T>::isIn( const T& t ) const {

	if ( !this->elts )
		return false;

	for ( Uint32 i = 0 ; i < this->elts ; i++ ) {
		if ( this->data[i] == t )
			return true;
	}

	return false;

}

template < typename T >
Bool CompArray<T>::isIn( const T& t, Uint32& i ) const {

	if ( !this->elts )
		return false;

	for ( i = 0 ; i < this->elts ; i++ ) {
		if ( this->data[i] == t )
			return true;
	}

	return false;

}

template < typename T >
CompArray<T>& CompArray<T>::operator = ( 
	const	CompArray<T>&	s ) {

	Array<T>::operator = ( s );

	return *this;

}

// ============================================================================

typedef CompArray<Buffer> BufferArray;

// ============================================================================

} // namespace BFC

// ============================================================================


#endif /* _BFC_Array_H_ */


// ============================================================================









