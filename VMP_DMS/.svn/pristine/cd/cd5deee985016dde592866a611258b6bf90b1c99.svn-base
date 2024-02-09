// ============================================================================

#ifndef _Array_H_
#define _Array_H_

// ============================================================================

#include "Buffer.h"

// ============================================================================

template <class T>
class Array {

public :

	Array(const int = 0);

	Array(const Array<T>&);

	virtual ~Array();

	virtual Array<T>& operator = (const Array<T>&);

	virtual Array<T>& operator += (const T&);

	virtual void kill();

	virtual bool isEmpty() const;

	virtual bool contains( const T& ) const;

	virtual int size() const;

	virtual const T& operator [] (const int i) const {return elt(i);}

	virtual const T& elt(const int) const;

protected :

	T		*data;	// pointer to the data (NULL if no data)
	int		elts;	// number of elements pointed to by `data'

};

// ============================================================================

template <class T>
Array<T>::Array(const int e) {

	elts = 0;

	if (e == 0) {
		data = 0;
		return;
	}

	data = new T[e];

	elts = e;

}

template <class T>
Array<T>::Array(const Array<T>& src) {

	if ((elts = src.elts) == 0) {
		data = 0;
		return;
	}

	data = new T[elts];

	for (int i = 0 ; i < elts ; i++)
		data[i] = src.data[i];

}

template <class T>
Array<T>::~Array() {

	if (elts)
		delete[] data;

}

template <class T>
Array<T>& Array<T>::operator = (const Array<T>& s) {

	if (this == &s)
		return *this;

	if (elts)
		kill();

	if ((elts = s.elts) == 0)
		return *this;

	data = new T[elts];

	for (int i = 0 ; i < elts ; i++)
		data[i] = s.data[i];

	return *this;

}

template <class T>
Array<T>& Array<T>::operator += (const T& s) {

	if (!elts) {
		data = new T[1];
		if (!data) {
			elts = 0;
			return *this;
		}
		data[0] = s;
		elts = 1;
		return *this;
	}

	T *ptr = new T[elts + 1];

	for (int i = 0 ; i < elts ; i++)
		ptr[i] = data[i];

	ptr[elts] = s;
	elts++;
	delete[] data;
	data = ptr;

	return *this;

}

template <class T>
void Array<T>::kill() {

	if (!elts)
		return;

	delete[] data;
	data = 0;
	elts = 0;

}

template <class T>
bool Array<T>::isEmpty() const {

	return (elts == 0);

}

template <class T>
bool Array<T>::contains( const T& t ) const {

	for ( int i = 0 ; i < elts ; i++ ) {
		if ( data[i] == t ) {
			return true;
		}
	}

	return false;

}

template <class T>
int Array<T>::size() const {

	return elts;

}

template <class T>
const T& Array<T>::elt(const int l) const {

	return data[l];

}

// ============================================================================

typedef Array<Buffer> BufferArray;

// ============================================================================


#endif /* _Array_H_ */


// ============================================================================









