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
//	BFC.TL.SortMap.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_SortMap_H_
#define _BFC_TL_SortMap_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.TL.ArrayBase.h"
#include "BFC.TL.Pair.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated 1 to 1 sorted map.
///
/// \ingroup BFC_TL

template < typename K, typename T >
class SortMap : public ArrayBase {

public :

	/// \brief Type of associations contained within this map.

	typedef Pair< K, T >  NodeType;

	/// \brief Creates a new empty map.

	SortMap(
	);

	/// \brief Creates a deep copy of \a pOther.

	SortMap(
		const	SortMap< K, T > &	pOther
	);

	/// \brief Destroys this object.

	virtual ~SortMap(
	);

	/// \brief Replaces the content of this map by a deep copy of
	///	\a pOther.

	SortMap< K, T > & operator = (
		const	SortMap< K, T > &	pOther
	);

	/// \brief Returns true iff this object is identical to \a pOther.

	Bool operator == (
		const	SortMap< K, T > &	pOther
	) const;

	/// \brief Returns true iff this object is different than \a pOther.

	Bool operator != (
		const	SortMap< K, T > &	pOther
	) const;

	/// \brief Returns true iff this map contains an association for
	///	key \a pKey.

	Bool contains(
		const	K &		pKey
	) const;

	/// \brief Returns true iff this map contains an association for
	///	key \a pKey. If this is the case, \a pIdx is set to the index
	///	of this association.

	Bool contains(
		const	K &		pKey,
			Uint32 &	pIdx
	) const;

	/// \brief Returns a reference to the value associated to key \a pKey.
	///
	/// \throws NoSuchKey
	///	If key \a pKey doesn't exist yet.

	T & operator [] (
		const	K &		pKey
	);

	/// \brief Returns a read-only reference to the value associated to
	///	key \a pKey.
	///
	/// \throws NoSuchKey
	///	If key \a pKey doesn't exist yet.

	const T & operator [] (
		const	K &		pKey
	) const;

	/// \brief Returns a read-only reference to the value associated to
	///	key \a pKey, or \a pDfltVal if \a pKey is not associated to a value.

	const T & get(
		const	K &		pKey,
		const	T &		pDfltVal
	) const;

	/// \brief Returns a writeable reference to the value associated to
	///	key \a pKey.
	///
	/// If the key doesn't exist yet, it is first added.

	T & getOrAdd(
		const	K &		pKey
	);

	/// \brief Returns the key whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater or equal to the size of this map
	///	(see getSize()).

	const K & getKey(
		const	Uint32		pIdx
	) const;

	/// \brief Returns the value whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater or equal to the size of this map
	///	(see getSize()).

	const T & getValue(
		const	Uint32		pIdx
	) const;

	/// \brief Returns the value whose index is \a pIdx.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater or equal to the size of this map
	///	(see getSize()).

	T & getValue(
		const	Uint32		pIdx
	);

	/// \brief Adds a new association whose key is \a pKey and whose
	///	value if the default value of type \b T to this map.
	///
	/// \throws KeyAlreadyUsed
	///	If \a pKey is already used.

	void add(
		const	K &		pKey
	) {
		add( pKey, T() );
	}

	/// \brief Adds a new association whose key is \a pKey and whose
	///	value is \a pVal to this map.
	///
	/// \throws KeyAlreadyUsed
	///	If \a pKey is already used.

	void add(
		const	K &		pKey,
		const	T &		pVal
	);

	/// \brief Sets the value of \a pKey to \a pVal. If \a pKey does not exist,
	///	it is first created.

	void setOrAdd(
		const	K &		pKey,
		const	T &		pVal
	);

	/// \brief Removes all the items that have the key \a pKey from
	///	this map.
	///
	/// \returns The number of items removed which is usually 1, but
	///	can be > 1 if duplicates have been enabled in add().
	///
	/// \throws NoSuchKey
	///	If this map doesn't contain any association whose key is
	///	\a pKey.

	Uint32 del(
		const	K &		pKey
	);

	/// \brief Removes the association whose index is \a pIdx from
	///	this map.
	///
	/// \throws OutOfRange
	///	If \a pIdx is greater or equal to the size of this map
	///	(see getSize()).

	void pop(
		const	Uint32		pIdx
	);

	/// \brief Removes all associations from this map.

	void kill(
	);

	/// \brief Adds a new association \a pNode to this map.
	///
	/// \throws KeyAlreadyUsed
	///	If the key of \a pNode is already used.

	void operator += (
		const	NodeType &	pNode
	);

	/// \brief Adds the contents of \a pOther to this map
	///
	/// \throws KeyAlreadyUsed
	///	If any of the keys of \a pOther is already used.

	void operator += (
		const	SortMap< K, T > &	pOther
	);

	/// \brief Synonym of del().

	void operator -= (
		const	K &		pKey
	);

	BASE_CLASS_GEN_EXCEPTION( "BFC.TL.SortMap" );

	CLASS_EXCEPTION( KeyAlreadyUsed, "Key already used" );
	CLASS_EXCEPTION( NoSuchKey, "No such key" );

protected :

	void doCopy(
		const	SortMap< K, T > &	pOther
	);

	NodeType & getNode(
		const	Uint32			pIdx
	) {
		return * reinterpret_cast< NodeType * >( ptr[ pIdx ] );
	}

	const NodeType & getNode(
		const	Uint32			pIdx
	) const {
		return * reinterpret_cast< const NodeType * >( ptr[ pIdx ] );
	}

	void setNode(
		const	Uint32			pIdx,
		const	K &			pKey,
		const	T &			pVal
	) {
		ptr[ pIdx ] = reinterpret_cast< void * >(
				new NodeType( pKey, pVal ) );
	}

	void setNode(
		const	Uint32			pIdx,
		const	NodeType &		pNode
	) {
		ptr[ pIdx ] = reinterpret_cast< void * >(
				new NodeType( pNode ) );
	}

	Uint32 findNearest(
		const	K &			pKey
	) const;

private :

};

// ============================================================================

template < typename K, typename T >
SortMap< K, T >::SortMap() :

	ArrayBase( 0 ) {

}

template < typename K, typename T >
SortMap< K, T >::SortMap(
	const	SortMap< K, T > &	pOther ) :

	ArrayBase( 0 ) {

	doCopy( pOther );

}

// ============================================================================

template < typename K, typename T >
SortMap< K, T >::~SortMap() {

	kill();

}

// ============================================================================

template < typename K, typename T >
SortMap< K, T > & SortMap< K, T >::operator = (
	const	SortMap< K, T > &	pOther ) {

	if ( this != & pOther ) {
		doCopy( pOther );
	}

	return * this;

}

// ============================================================================

template < typename K, typename T >
Bool SortMap< K, T >::operator == (
	const	SortMap< K, T > &	pOther ) const {

	if ( nbr != pOther.nbr ) {
		return false;
	}

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( getNode( i ) != pOther.getNode( i ) ) {
			return false;
		}
	}

	return true;

}

template < typename K, typename T >
Bool SortMap< K, T >::operator != (
	const	SortMap< K, T > &	pOther ) const {

	return ( ! ( * this == pOther ) );

}

// ============================================================================

template < typename K, typename T >
Bool SortMap< K, T >::contains(
	const	K &		pKey ) const {

	Uint32 dummy;

	return contains( pKey, dummy );

}

template < typename K, typename T >
Bool SortMap< K, T >::contains(
	const	K &		pKey,
		Uint32 &	pIdx ) const {

	pIdx = findNearest( pKey );

	if ( pIdx < nbr && getNode( pIdx ).left() == pKey ) {
		return true;
	}

	return false;

}

// ============================================================================

template < typename K, typename T >
T & SortMap< K, T >::operator [] (
	const	K &		pKey ) {

	Uint32 pIdx = findNearest( pKey );

	if ( pIdx < nbr && getNode( pIdx ).left() == pKey ) {
		return getNode( pIdx ).right();
	}

	throw NoSuchKey();

}

template < typename K, typename T >
const T & SortMap< K, T >::operator [] (
	const	K &		pKey ) const {

	Uint32 pIdx = findNearest( pKey );

	if ( pIdx < nbr && getNode( pIdx ).left() == pKey ) {
		return getNode( pIdx ).right();
	}

	throw NoSuchKey();

}

// ============================================================================

template < typename K, typename T >
const T & SortMap< K, T >::get(
	const	K &		pKey,
	const	T &		pDfltVal ) const {

	Uint32 pIdx = findNearest( pKey );

	if ( pIdx < nbr && getNode( pIdx ).left() == pKey ) {
		return getNode( pIdx ).right();
	}

	return pDfltVal;

}

template < typename K, typename T >
T & SortMap< K, T >::getOrAdd(
	const	K &		pKey ) {

	Uint32 pIdx = findNearest( pKey );

	if ( pIdx >= nbr || getNode( pIdx ).left() != pKey ) {
		insertHole( pIdx, 1 );
		setNode( pIdx, pKey, T() );
	}

	return getNode( pIdx ).right();

}

template < typename K, typename T >
const K & SortMap< K, T >::getKey(
	const	Uint32		pIdx ) const {

	checkIndex( pIdx, false );

	return getNode( pIdx ).left();

}

template < typename K, typename T >
const T & SortMap< K, T >::getValue(
	const	Uint32		pIdx ) const {

	checkIndex( pIdx, false );

	return getNode( pIdx ).right();

}

template < typename K, typename T >
T & SortMap< K, T >::getValue(
	const	Uint32		pIdx ) {

	checkIndex( pIdx, false );

	return getNode( pIdx ).right();

}

// ============================================================================

template < typename K, typename T >
void SortMap< K, T >::add(
	const	K &		pKey,
	const	T &		pVal ) {

	Uint32	pIdx = findNearest( pKey );

	if ( pIdx < nbr && getNode( pIdx ).left() == pKey ) {
		throw KeyAlreadyUsed();
	}

	insertHole( pIdx, 1 );

	setNode( pIdx, pKey, pVal );

}

template < typename K, typename T >
void SortMap< K, T >::setOrAdd(
	const	K &		pKey,
	const	T &		pVal ) {

	Uint32	pIdx = findNearest( pKey );

	if ( pIdx >= nbr || getNode( pIdx ).left() != pKey ) {
		insertHole( pIdx, 1 );
		setNode( pIdx, pKey, pVal );
	}
	else {
		getNode( pIdx ).right() = pVal;
	}

}

template < typename K, typename T >
Uint32 SortMap< K, T >::del(
	const	K &		pKey ) {

	Uint32	pIdx = findNearest( pKey );

	if ( pIdx >= nbr || getNode( pIdx ).left() != pKey ) {
		throw NoSuchKey();
	}

	pop( pIdx );

}

template < typename K, typename T >
void SortMap< K, T >::pop(
	const	Uint32		pIdx ) {

	checkIndex( pIdx, false );

	delete reinterpret_cast< NodeType * >( ptr[ pIdx ] );

	deleteHole( pIdx, 1 );

}

// ============================================================================

template < typename K, typename T >
void SortMap< K, T >::kill() {

	while ( ! isEmpty() ) {
		pop( getSize() - 1 );
	}

}

// ============================================================================

template < typename K, typename T >
void SortMap< K, T >::operator += (
	const	NodeType &	pNode ) {

	add( pNode.left(), pNode.right() );

}

template < typename K, typename T >
void SortMap< K, T >::operator += (
	const	SortMap< K, T > &	pOther ) {

	for ( Uint32 i = 0 ; i < pOther.getSize(); ++i ) {
		add( pOther.getKey(i), pOther.getValue(i) );
	}

}

template < typename K, typename T >
void SortMap< K, T >::operator -= (
	const	K &		pKey ) {

	del( pKey );

}

// ============================================================================

template < typename K, typename T >
void SortMap< K, T >::doCopy(
	const	SortMap< K, T > &	pOther ) {

	kill();

	if ( pOther.nbr ) {
		Uint32	cnt = pOther.nbr;
		insertHole( 0, cnt );
		while ( cnt-- ) {
			setNode( cnt, pOther.getNode( cnt ) );
		}
	}

}

// ============================================================================

template < typename K, typename T >
Uint32 SortMap< K, T >::findNearest(
	const	K &		pKey ) const {

	Uint32	posMin = 0;
	Uint32	posMax = nbr;
	Uint32	posMed;

	for (;;) {
		if ( posMin == posMax ) {
			return posMin;
		}
		if ( ! ( getNode( posMin ).left() < pKey ) ) {
			return posMin;
		}
		posMin++;
		if ( posMin == posMax ) {
			return posMin;
		}
		if ( getNode( posMax - 1 ).left() < pKey ) {
			return posMax;
		}
		posMax--;
		if ( posMin == posMax ) {
			return posMin;
		}
		posMed = ( posMin + posMax ) / 2;
		if ( ! ( pKey < getNode( posMed ).left() ) ) {
			posMin = posMed;
		}
		else {
			posMax = posMed;
		}
	}

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_SortMap_H_

// ============================================================================

