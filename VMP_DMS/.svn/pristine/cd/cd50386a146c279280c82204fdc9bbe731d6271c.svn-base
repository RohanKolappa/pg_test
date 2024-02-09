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
//	TBFC.TL.Map.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_Map_H_
#define _TBFC_TL_Map_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.TL.Array.h"
#include "TBFC.TL.SortArray.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Node type for Map class.
///
/// \ingroup TBFC_TL

template < typename K, typename T >
class MapNode {

public :

	MapNode() {
	}

//	MapNode(
//		const	K &		_key
//	) {
//		key = _key;
//	}

	MapNode(
		const	K &		_key,
		const	T &		_val
	) {
		key = _key;
		val = _val;
	}

	MapNode(
		const	MapNode< K, T >	& c
	) {
		key = c.key;
		val = c.val;
	}

	MapNode< K, T > & operator = (
		const	MapNode< K, T >	& c
	) {
		key = c.key;
		val = c.val;
		return *this;
	}

//	Bool operator == (
//		const	MapNode< K, T >	& n
//	) const {
//		return ( key == n.key && val == n.val );
//	}

//	Bool operator != (
//		const	MapNode< K, T >	& n
//	) const {
//		return ( key != n.key || val != n.val );
//	}

	const K & getKey(
	) const {
		return key;
	}

	const T & getValue(
	) const {
		return val;
	}

private :

	K	key;
	T	val;

};

// ============================================================================

/// \brief A simple templated 1 to 1 map.
///
/// \ingroup TBFC_TL

template < typename K, typename T >
class Map {

public :

	/// \brief Type of associations contained within this map.

	typedef MapNode< K, T >  NodeType;

	/// \brief Array of keys type.

	typedef SortArray< K > KeyArrayType;

	/// \brief Array of Values type.

	typedef Array< T > ValArrayType;

	/// \brief Creates a new empty map.

	Map(
	) {
	}

	/// \brief Creates a new map, copied from \a other.

	Map(
		const	Map< K, T > &	other
	) {
		keys = other.keys;
		vals = other.vals;
	}

	/// \brief Destroys this object.

	/* virtual */ ~Map(
	) {
	}

	/// \brief Replaces the content of this map by a copy of \a other.

	Map< K, T > & operator = (
		const	Map< K, T> &	other
	) {
		if ( this != & other ) {
			keys = other.keys;
			vals = other.vals;
		}
		return * this;
	}

	/// \brief Returns a read-only reference to the value associated to
	///	key \a key.
	///
	/// \throws NoSuchKey
	///	If key \a key doesn't exist yet.

	const T & operator [] (
		const	K &		key
	) const;

	/// \brief Returns a writeable reference to the value associated to
	///	key \a key.
	///
	/// \throws NoSuchKey
	///	If key \a key doesn't exist yet.

	T & operator [] (
		const	K &		key
	);

	/// \brief Returns a writeable reference to the value associated to
	///	key \a key.
	///
	/// If the key doesn't exist yet, it is first added.

	T & getOrAdd(
		const	K &		key
	);

	/// \brief Returns the number or pairs (key, value) contained in
	///	this map.

	Uint32 getSize(
	) const {
		return keys.getSize();
	}

	/// \brief Synonym of getSize().

	Uint32 size(
	) const {
		return getSize();
	}

	/// \brief Returns all the keys of this map.

	const KeyArrayType & getAllKeys(
	) const {
		return keys;
	}

	/// \brief Returns all the values of this map.

	const ValArrayType & getAllValues(
	) const {
		return vals;
	}

	/// \brief Returns the key whose index is \a index.
	///
	/// \throws OutOfRange
	///	If \a index is greater or equal to the size of this map
	///	(see getSize()).

	const K & getKey(
		const	Uint32		index
	) const;

	/// \brief Synonym of getKey().

	const K & key(
		const	Uint32		index
	) const {
		return getKey( index );
	}

	/// \brief Returns the value whose index is \a index.
	///
	/// \throws OutOfRange
	///	If \a index is greater or equal to the size of this map
	///	(see getSize()).

	const T & getValue(
		const	Uint32		index
	) const;

	/// \brief Returns the value whose index is \a index.
	///
	/// \throws OutOfRange
	///	If \a index is greater or equal to the size of this map
	///	(see getSize()).

	T & getValue(
		const	Uint32		index
	);

	/// \brief Synonym of getValue().

	const T & value(
		const	Uint32		index
	) const {
		return getValue( index );
	}

	/// \brief Returns the first key whose associated value is
	///	\a value.
	///
	/// \throws NoSuchValue
	///	If no key is associated to this value.

	const K & keyOf(
		const	T &		value
	) const;

	/// \brief Returns true iff this map doesn't contain any pair yet.

	Bool isEmpty(
	) const {
		return keys.isEmpty();
	}

	/// \brief Returns true iff this map contains an association for
	///	key \a k.

	Bool contains(
		const	K &		k
	) const {
		return keys.contains( k );
	}

	/// \brief Returns true iff this map contains an association for
	///	key \a key. If this is the case, \a pos is set to the index
	///	of this association.

	Bool contains(
		const	K &		key,
			Uint32 &	pos
	) const {
		return keys.contains( key, pos );
	}

	/// \brief Synonym of contains().

	Bool has(
		const	K &		k
	) const {
		return contains( k );
	}

	/// \brief Synonym of contains().

	Bool exists(
		const	K &		k
	) const {
		return contains( k );
	}

	/// \brief Returns true iff this map contains an association for
	///	key \a key. If this is the case, \a pos is set to the index
	///	of this association.

	Bool exists(
		const	K &		key,
			Uint32 &	pos
	) const {
		return contains( key, pos );
	}

	/// \brief Adds a new association whose key is \a key and whose
	///	value if the default value of type \b T to this map.
	///
	/// \throws KeyAlreadyUsed
	///	If \a key is already used.

	void add(
		const	K &		key
	) {
		add( key, T() );
	}

	/// \brief Adds a new association whose key is \a key and whose
	///	value is \a val to this map.
	///
	/// \throws KeyAlreadyUsed
	///	If \a key is already used and \a multi is false.

	void add(
		const	K &		key,
		const	T &		val,
		const	Bool		multi = false
	);

	/// \brief Sets the value of \a key to \a val. If \a key does not exist,
	///	it is first created.

	void setOrAdd(
		const	K &		key,
		const	T &		val
	);

	/// \brief Removes all the items that have the key \a key from
	///	this map.
	///
	/// \returns The number of items removed which is usually 1, but
	///	can be > 1 if duplicates have been enabled in add().
	///
	/// \throws NoSuchKey
	///	If this map doesn't contain any association whose key is
	///	\a key.

	Uint32 del(
		const	K &		key
	);

	/// \brief Removes the association whose index is \a index from
	///	this map.
	///
	/// \throws OutOfRange
	///	If \a index is greater or equal to the size of this map
	///	(see getSize()).

	void pop(
		const	Uint32		index
	);

	/// \brief Removes all associations from this map.

	void kill(
	) {
		keys.kill();
		vals.kill();
	}

	/// \brief Adds a new association \a s to this map.
	///
	/// \throws KeyAlreadyUsed
	///	If the key of \a s is already used.

	void operator += (
		const	NodeType &	s
	) {
		add( s.getKey(), s.getValue() );
	}

	/// \brief Synonym of del().

	void operator -= (
		const	K &		key
	) {
		del( key );
	}

	BASE_CLASS_GEN_EXCEPTION( "TBFC.TL.Map" );

	CLASS_EXCEPTION( KeyAlreadyUsed, "Key already used" );
	CLASS_EXCEPTION( NoSuchKey, "No such key" );
	CLASS_EXCEPTION( NoSuchValue, "No such value" );
	CLASS_EXCEPTION( OutOfRange, "Out of range" );

protected :

private :

	KeyArrayType	keys;
	ValArrayType	vals;

};

// ============================================================================

template < typename K, typename T >
const T & Map< K, T >::operator [] (
	const	K		& key ) const {

	Uint32 pos;

	if ( ! keys.contains( key, pos ) ) {
		throw NoSuchKey( "Map::operator[] const" );
	}

	return vals[ pos ];

}

template < typename K, typename T >
T & Map< K, T >::operator [] (
	const	K		& key ) {

	Uint32 pos;

	if ( ! keys.contains( key, pos ) ) {
		throw NoSuchKey( "Map::operator[]" );
//		pos = vals.getSize();
//		keys += key;
//		vals += T();
	}

	return vals[ pos ];

}

template < typename K, typename T >
T & Map< K, T >::getOrAdd(
	const	K &		key ) {

	Uint32 pos;

	if ( ! keys.contains( key, pos ) ) {
		pos = vals.getSize();
		keys += key;
		vals += T();
	}

	return vals[ pos ];

}

// ============================================================================

template < typename K, typename T >
const K & Map< K, T >::getKey(
	const	Uint32		index ) const {

	if ( index >= keys.getSize() ) {
		throw OutOfRange();
	}

	return keys[ index ];

}

template < typename K, typename T >
const T & Map< K, T >::getValue(
	const	Uint32		index ) const {

	if ( index >= vals.getSize() ) {
		throw OutOfRange();
	}

	return vals[ index ];

}

template < typename K, typename T >
T & Map< K, T >::getValue(
	const	Uint32		index ) {

	if ( index >= vals.getSize() ) {
		throw OutOfRange();
	}

	return vals[ index ];

}

// ============================================================================

template < typename K, typename T >
const K & Map< K, T >::keyOf(
	const	T		& val ) const {

	Uint32 pos;

	if ( ! vals.contains( val, pos ) ) {
		throw NoSuchValue();
	}

	return keys[ pos ];

}

// ============================================================================

template < typename K, typename T >
void Map< K, T >::add(
	const	K		& key,
	const	T		& val,
	const	Bool		multi ) {

	if ( ! multi && keys.contains( key ) ) {
		throw KeyAlreadyUsed();
	}

	keys += key;
	vals += val;

}

template < typename K, typename T >
void Map< K, T >::setOrAdd(
	const	K		& key,
	const	T		& val ) {

	Uint32 pos;

	if ( keys.contains( key, pos ) ) {
		vals[ pos ] = val;
	}
	else {
		keys += key;
		vals += val;
	}

}

template < typename K, typename T >
Uint32 Map< K, T >::del(
	const	K		& key ) {

	Uint32	pos = 0;
	Uint32	res = 0;

	while ( keys.contains( key, pos ) ) {
		keys.pop( pos );
		vals.pop( pos );
		res++;
	}

	if ( ! res ) {
		throw NoSuchKey( "Map::del()" );
	}

	return res;

}

template < typename K, typename T >
void Map< K, T >::pop(
	const	Uint32		index ) {

	if ( index >= keys.getSize() ) {
		throw OutOfRange();
	}

	keys.pop( index );
	vals.pop( index );

}

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_Map_H_

// ============================================================================

