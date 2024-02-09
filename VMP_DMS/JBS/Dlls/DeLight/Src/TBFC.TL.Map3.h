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
//	TBFC.TL.Map3.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_Map3_H_
#define _TBFC_TL_Map3_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.TL.CompArray.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Node type for Map3 class.
///
/// \ingroup TBFC_TL

template <class K1, class K2, class T>
class Map3Node {

public :

	Map3Node() {
	}

	Map3Node(const K1& _key1, const K2& _key2) {
		key1 = _key1;
		key2 = _key2;
	}

	Map3Node(const K1& _key1, const K2& _key2, const T& _val) {
		key1 = _key1;
		key2 = _key2;
		val  = _val;
	}

	Map3Node(const Map3Node<K1,K2,T>& c) {
		key1 = c.key1;
		key2 = c.key2;
		val  = c.val;
	}

	bool operator == (const Map3Node<K1,K2,T>& n) const {
		return (key1 == n.key1 && key2 == n.key2 && val == n.val);
	}

	bool operator != (const Map3Node<K1,K2,T>& n) const {
		return (key1 != n.key1 || key2 != n.key2 || val != n.val);
	}

	K1	key1;
	K2	key2;
	T	val;

};

// ============================================================================

/// \brief Map 2 keys to 1 value.
///
/// \ingroup TBFC_TL

template <class K1, class K2, class T>
class Map3 {

public :

	Map3();

	Map3(const Map3<K1,K2,T>&);

	virtual ~Map3() {}

	Map3<K1,K2,T>& operator = (const Map3<K1,K2,T>&);

	bool operator == (const Map3<K1,K2,T>&) const;

	bool operator != (const Map3<K1,K2,T>&) const;

	T& operator () (const K1&, const K2&);

	const T& operator () (const K1&, const K2&) const;

	const K1& key1(const Uint32) const;

	const K2& key2(const Uint32) const;

	const T& value(const Uint32) const;

	const K1& key1Of(const T&) const;

	const K2& key2Of(const T&) const;

	Uint32 size() const;

	bool isEmpty() const;

	bool exists(const K1&, const K2&) const;

	void add(const K1&, const K2&, const T&);

	void del(const K1&, const K2&);

	void pop(const Uint32);

	void kill();

	typedef Map3Node<K1,K2,T>  NodeType;

	Map3<K1,K2,T>& operator += (const NodeType& s);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("Map3");

	CLASS_EXCEPTION(KeyAlreadyUsed, "Key already used");
	CLASS_EXCEPTION(NoSuchKey, "No such key");
	CLASS_EXCEPTION(NoSuchValue, "No such value");
	CLASS_EXCEPTION(OutOfRange, "Out of range");

protected :

private :

	CompArray<Map3Node<K1,K2,T> >	array;

};

template <class K1, class K2, class T>
Map3<K1,K2,T>::Map3() {

	array.kill();

}

template <class K1, class K2, class T>
Map3<K1,K2,T>::Map3(const Map3<K1,K2,T>& m) {

	array = m.array;

}

template <class K1, class K2, class T>
Map3<K1,K2,T>& Map3<K1,K2,T>::operator = (const Map3<K1,K2,T>& m) {

	array = m.array;
	return *this;

}

template <class K1, class K2, class T>
bool Map3<K1,K2,T>::operator == (const Map3<K1,K2,T>& m) const {

	return (array == m.array);

}

template <class K1, class K2, class T>
bool Map3<K1,K2,T>::operator != (const Map3<K1,K2,T>& m) const {

	return (array != m.array);

}

template <class K1, class K2, class T>
T& Map3<K1,K2,T>::operator () (const K1& k1, const K2& k2) {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key1 == k1
		 && array[i].key2 == k2) {
			return (array[i].val);
		}
	}

	throw NoSuchKey();

}

template <class K1, class K2, class T>
const T& Map3<K1,K2,T>::operator () (const K1& k1, const K2& k2) const {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key1 == k1
		 && array[i].key2 == k2) {
			return (array[i].val);
		}
	}

	throw NoSuchKey();

}

template <class K1, class K2, class T>
const K1& Map3<K1,K2,T>::key1(const Uint32 i) const {

	if (i >= array.size())
		throw OutOfRange();

	return (array[i].key1);

}

template <class K1, class K2, class T>
const K2& Map3<K1,K2,T>::key2(const Uint32 i) const {

	if (i >= array.size())
		throw OutOfRange();

	return (array[i].key2);

}

template <class K1, class K2, class T>
const T& Map3<K1,K2,T>::value(const Uint32 i) const {

	if (i >= array.size())
		throw OutOfRange();

	return (array[i].val);

}

template <class K1, class K2, class T>
const K1& Map3<K1,K2,T>::key1Of(const T& v) const {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].val == v) {
			return (array[i].key1);
		}
	}

	throw NoSuchValue();

}

template <class K1, class K2, class T>
const K2& Map3<K1,K2,T>::key2Of(const T& v) const {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].val == v) {
			return (array[i].key2);
		}
	}

	throw NoSuchValue();

}

template <class K1, class K2, class T>
Uint32 Map3<K1,K2,T>::size() const {

	return array.size();

}

template <class K1, class K2, class T>
bool Map3<K1,K2,T>::isEmpty() const {

	return array.isEmpty();

}

template <class K1, class K2, class T>
bool Map3<K1,K2,T>::exists(const K1& k1, const K2& k2) const {

	if (array.isEmpty())
		return false;

	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key1 == k1
		 && array[i].key2 == k2)
			return true;
	}

	return false;

}

template <class K1, class K2, class T>
void Map3<K1,K2,T>::add(const K1& k1, const K2& k2, const T& v) {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key1 == k1
		 && array[i].key2 == k2) {
			throw KeyAlreadyUsed();
		}
	}
	array += Map3Node<K1,K2,T>(k1, k2, v);

}

template <class K1, class K2, class T>
void Map3<K1,K2,T>::del(const K1& k1, const K2& k2) {

	Uint32 i = 0;
	if (!array.isEmpty())
	for (i = 0 ; i < array.size() ; i++) {
		if (array[i].key1 == k1
		 && array[i].key2 == k2)
			break;
	}
	if (i == array.size())
		throw NoSuchKey();
	array.pop(i);

}

template <class K1, class K2, class T>
void Map3<K1,K2,T>::pop(const Uint32 i) {

	array.pop(i);

}

template <class K1, class K2, class T>
void Map3<K1,K2,T>::kill() {

	array.kill();

}

template <class K1, class K2, class T>
Map3<K1,K2,T>& Map3<K1,K2,T>::operator += (const NodeType& s) {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key1 == s.key1
		 && array[i].key2 == s.key2) {
			throw KeyAlreadyUsed();
		}
	}
	array += s;

	return *this;

}

// ============================================================================

} // namespace TBFC

// ============================================================================


#endif // _TBFC_TL_Map3_H_


// ============================================================================


