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
//	BFC.Map.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Map_H_
#define _BFC_Map_H_

// ============================================================================

#include "BFC.Base.Types.h"
#include "BFC.TL.Array.h"
#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// A simple templated 1 to 1 map.

///
/// \ingroup Common_Containers

template <class K, class T>
class MapNode {

public :

	MapNode() {
	}

	MapNode(const K& _key) {
		key = _key;
	}

	MapNode(const K& _key, const T& _val) {
		key = _key;
		val = _val;
	}

	MapNode(const MapNode<K,T>& c) {
		key = c.key;
		val = c.val;
	}

	Bool operator == (const MapNode<K,T>& n) const {
		return (key == n.key && val == n.val);
	}

	Bool operator != (const MapNode<K,T>& n) const {
		return (key != n.key || val != n.val);
	}

	K	key;
	T	val;

};

// ============================================================================

/*!
 *	\ingroup	CommonLibrary
 */

template <class K, class T>
class Map {

public :

	Map();

	Map(const Map<K,T>&);

	virtual ~Map() {}

	Map<K,T>& operator = (const Map<K,T>&);

//	Bool operator == (const Map<K,T>&) const;

//	Bool operator != (const Map<K,T>&) const;

	T& operator [] (const K&);

	const T& operator [] (const K&) const;

	const K& key(const Uint32) const;

	const T& value(const Uint32) const;

	T& value(const Uint32);

	const K& keyOf(const T&) const;

	Uint32 size() const;

	Bool isEmpty() const;

	Bool contains( const K& k ) const {return exists( k );}
	Bool exists(const K&) const;

	Bool exists(const K&, Uint32& pos) const;

	void add(const K&, const T&);

	void del(const K&);

	void pop(const Uint32);

	void kill();

	typedef MapNode<K,T>  NodeType;

	Map<K,T>& operator += (const NodeType& s);

	Map<K,T>& operator -= (const K& k);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("Map");

	CLASS_EXCEPTION(KeyAlreadyUsed, "Key already used");
	CLASS_EXCEPTION(NoSuchKey, "No such key");
	CLASS_EXCEPTION(NoSuchValue, "No such value");
	CLASS_EXCEPTION(OutOfRange, "Out of range");

protected :

private :

	CompArray<MapNode<K,T> >	array;

};

template <class K, class T>
Map<K,T>::Map() {

	array.kill();

}

template <class K, class T>
Map<K,T>::Map(const Map<K,T>& m) {

	array = m.array;

}

template <class K, class T>
Map<K,T>& Map<K,T>::operator = (const Map<K,T>& m) {

	array = m.array;
	return *this;

}

#if 0

template <class K, class T>
Bool Map<K,T>::operator == (const Map<K,T>& m) const {

	return (array == m.array);

}

template <class K, class T>
Bool Map<K,T>::operator != (const Map<K,T>& m) const {

	return (array != m.array);

}

#endif

template <class K, class T>
T& Map<K,T>::operator [] (const K& k) {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k) {
			return (array[i].val);
		}
	}

	throw NoSuchKey();

}

template <class K, class T>
const T& Map<K,T>::operator [] (const K& k) const {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k) {
			return (array[i].val);
		}
	}

	throw NoSuchKey();

}

template <class K, class T>
const K& Map<K,T>::key(const Uint32 i) const {

	if (i >= array.size())
		throw OutOfRange();

	return (array[i].key);

}

template <class K, class T>
const T& Map<K,T>::value(const Uint32 i) const {

	if (i >= array.size())
		throw OutOfRange();

	return (array[i].val);

}

template <class K, class T>
T& Map<K,T>::value(const Uint32 i) {

	if (i >= array.size())
		throw OutOfRange();

	return (array[i].val);

}

template <class K, class T>
const K& Map<K,T>::keyOf(const T& v) const {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].val == v) {
			return (array[i].key);
		}
	}

	throw NoSuchValue();

}

template <class K, class T>
Uint32 Map<K,T>::size() const {

	return array.size();

}

template <class K, class T>
Bool Map<K,T>::isEmpty() const {

	return array.isEmpty();

}

template <class K, class T>
Bool Map<K,T>::exists(const K& k) const {

	if (array.isEmpty())
		return false;

	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k)
			return true;
	}

	return false;

}

template <class K, class T>
Bool Map<K,T>::exists(const K& k, Uint32& i) const {

	if (array.isEmpty())
		return false;

	for (i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k)
			return true;
	}

	return false;

}

template <class K, class T>
void Map<K,T>::add(const K& k, const T& v) {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k) {
			throw KeyAlreadyUsed();
		}
	}
	array += MapNode<K,T>(k, v);

}

template <class K, class T>
void Map<K,T>::del(const K& k) {

	Uint32 i = 0;
	if (!array.isEmpty())
	for (i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k)
			break;
	}
	if (i == array.size())
		throw NoSuchKey();
	array.pop(i);

}

template <class K, class T>
void Map<K,T>::pop(const Uint32 i) {

	array.pop(i);

}

template <class K, class T>
void Map<K,T>::kill() {

	array.kill();

}

template <class K, class T>
Map<K,T>& Map<K,T>::operator += (const NodeType& s) {

	if (!array.isEmpty())
	for (Uint32 i = 0 ; i < array.size() ; i++) {
		if (array[i].key == s.key) {
			throw KeyAlreadyUsed();
		}
	}
	array += s;

	return *this;

}

template <class K, class T>
Map<K,T>& Map<K,T>::operator -= (const K& k) {

	Uint32 i = 0;
	if (!array.isEmpty())
	for (i = 0 ; i < array.size() ; i++) {
		if (array[i].key == k)
			break;
	}
	if (i == array.size())
		throw NoSuchKey();
	array.pop(i);

	return *this;

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Map_H_

// ============================================================================

