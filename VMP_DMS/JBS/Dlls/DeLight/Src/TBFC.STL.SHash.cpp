// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC".
// 
// "TBFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC"; if not, write to:
//							Free Software Foundation
//							59 Temple Place, Suite 330
//							Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.STL.SHash.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.STL.SHash.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL( SHash )

// ============================================================================

SHash::SHash() :

	ptrBuckets	( 0 ),
	nbrEntries	( 0 ),
	usrNbrBits	( MinNumBits ),
	curNbrBits	( 0 ),
	nbrBuckets	( 0 ) {

	reserve( 1 );

}

SHash::SHash(
	const	SHash &		pOther ) :

	SObject(),

	ptrBuckets	( 0 ),
	nbrEntries	( pOther.nbrEntries ),
	usrNbrBits	( pOther.usrNbrBits ),
	curNbrBits	( pOther.curNbrBits ),
	nbrBuckets	( pOther.nbrBuckets ) {

	if ( nbrBuckets ) {
		ptrBuckets = new Node * [ nbrBuckets ];
		for ( Uint32 i = 0 ; i < nbrBuckets ; ++i ) {
			Node **	nxtNode = &ptrBuckets[ i ];
			Node *	oldNode = pOther.ptrBuckets[ i ];
			while ( oldNode ) {
				Node * dup = new Node( *oldNode );
				*nxtNode = dup;
				nxtNode = &dup->next;
				oldNode = oldNode->next;
			}
			*nxtNode = 0;
		}
	}

}

SHash::~SHash() {

	kill();

}

// ============================================================================

Bool SHash::operator == (
	const	SHash &		pOther ) const {

	if ( this == &pOther ) {
		return true;
	}

	if ( getSize() != pOther.getSize() ) {
		return false;
	}

	CstIterator it = begin();

	while (it != end()) {
		KeyT key = it.getKey();
		CstIterator it2 = pOther.find( key );
		do {
			if ( it2 == pOther.end() || !( it2.getKey() == key ) )
				return false;
			if ( !( it.getVal() == it2.getVal() ) )
				return false;
			++it;
			++it2;
		} while ( it != end() && it.getKey() == key );
	}

	return true;

}

// ============================================================================

void SHash::reserve(
	const	Uint32		pSize ) {

	Uint32	bits = 0;
	Uint32	size = pSize;

	while ( size ) {
		size >>= 1;
		bits++;
	}

	rehash( bits );

}

void SHash::kill() {

	if ( nbrBuckets ) {
	
		for ( Uint32 i = 0 ; i < nbrBuckets ; i++ ) {
			Node * cur = ptrBuckets[ i ];
			while ( cur ) {
				Node * next = cur->next;
				delete cur;
				cur = next;
			}
		}
	
		delete [] ptrBuckets;

		ptrBuckets = 0;
		nbrBuckets = 0;
		nbrEntries = 0;
		curNbrBits = 0;

	}

}

// ============================================================================

Uint32 SHash::remove(
	const	KeyT		pKey ) {

	Uint32	removed = 0;
	Node **	node = findNodeRef( pKey );

	while ( *node && (*node)->key == pKey ) {
		Node * next = (*node)->next;
		delete *node;
		*node = next;
		removed++;
	}

	if ( removed ) {
		nbrEntries -= removed;
//		hasShrunk();
	}

	return removed;

}

SHash::ValT SHash::take(
	const	KeyT		pKey ) {

	Node **	node = findNodeRef( pKey );
	ValT	res;

	if ( *node ) {
		res = (*node)->val;
		Node * next = (*node)->next;
		delete *node;
		*node = next;
		nbrEntries--;
//		hasShrunk();
	}

	return res;

}

// ============================================================================

SHash::KeyT SHash::getKey(
	const	ValT &		pVal ) const {

	CstIterator i = begin();

	while ( i != end() ) {
		if ( i.getVal() == pVal )
			return i.getKey();
		++i;
	}

	throw NoSuchValue();

}

// ============================================================================

SHash::KeyList SHash::getUniqueKeys() const {

	KeyList	res;

	for ( CstIterator i = begin() ; i != end() ; ) {
		KeyT key = i.getKey();
		res += key;
		i++;
		while ( i != end() && i.getKey() == key ) {
			i++;
		}
	}

	return res;

}

SHash::KeyList SHash::getKeys() const {

	KeyList res;

	for ( CstIterator i = begin() ; i != end() ; i++ ) {
		res += i.getKey();
	}

	return res;

}

SHash::KeyList SHash::getKeys(
	const	ValT &		pVal ) const {

	KeyList res;

	for ( CstIterator i = begin() ; i != end() ; i++ ) {
		if ( i.getVal() == pVal ) {
			res += i.getKey();
		}
	}

	return res;

}

SHash::ValList SHash::getVals() const {

	ValList res;

	for ( CstIterator i = begin() ; i != end() ; i++ ) {
		res += i.getVal();
	}

	return res;

}

SHash::ValList SHash::getVals(
	const	KeyT		pKey ) const {

	ValList res;

	for ( Node * node = findNodePtr(pKey)
	    ; node && node->key == pKey
	    ; node = node->next ) {
		res += node->val;
	}

	return res;

}

// ============================================================================

Uint32 SHash::getCount(
	const	KeyT		pKey ) const {

	Uint32	cnt = 0;

	for ( Node * node = findNodePtr( pKey )
	    ; node && node->key == pKey
	    ; node = node->next ) {
		cnt++;
	}

	return cnt;

}

// ============================================================================

void SHash::erase(
		Iterator &	it ) {

	if ( it.hash != this ) {
		throw InvalidIterator();
	}

	Node *	node = it.node;

	if ( ! node ) {
		throw NullPointer();
	}

	it++;

	Node *& node_ptr = ptrBuckets[ node->key % nbrBuckets ];

	while ( node_ptr != node ) {
		node_ptr = node_ptr->next;
	}

	node_ptr = node->next;
	delete node;
	nbrEntries--;

}

// ============================================================================

void SHash::rehash(
	const	Uint32		pHint ) {

	Uint32	hint = pHint;

	if ( hint < MinNumBits ) {
		hint = MinNumBits;
	}

	if ( hint == curNbrBits ) {
		return;
	}

	Node **	oldPtrBuckets = ptrBuckets;
	Uint32	oldNbrBuckets = nbrBuckets;

	curNbrBits = hint;
	nbrBuckets = primeForNumBits( hint );
	ptrBuckets = new Node * [ nbrBuckets ];

	for ( Uint32 i = 0 ; i < nbrBuckets ; ++i ) {
		ptrBuckets[ i ] = 0;
	}

	for ( Uint32 i = 0 ; i < oldNbrBuckets ; ++i ) {
		Node * curNode = oldPtrBuckets[ i ];
		while ( curNode ) {
			// Transfer 'curNode' and all successors with the
			// same key to the new location...
			Uint32 key = curNode->key;
			Node * lstNode = curNode;
			while ( lstNode->next && lstNode->next->key == key ) {
				lstNode = lstNode->next;
			}
			// Transfert 'curNode' to 'lstNode' (incl)...
			Node * nxtCurNode = lstNode->next;
			Node ** insNodeRef = &ptrBuckets[ key % nbrBuckets];
			while ( *insNodeRef ) {
				insNodeRef = &(*insNodeRef)->next;
			}
			lstNode->next = 0;
			*insNodeRef = curNode;
			curNode = nxtCurNode;
		}
	}

	if ( oldNbrBuckets ) {
		delete [] oldPtrBuckets;
	}

}

// ============================================================================

SHash::Node * SHash::getFirstNode() const {

	for ( Uint32 n = 0 ; n < nbrBuckets ; n++ ) {
		if ( ptrBuckets[ n ] ) {
			return ptrBuckets[ n ];
		}
	}

	return 0;

}

SHash::Node * SHash::getNextNode(
		Node *	node ) const {

	if ( node->next ) {
		return node->next;
	}

	for ( Uint32 n = ( node->key % nbrBuckets ) + 1 ; n < nbrBuckets ; n++ ) {
		if ( ptrBuckets[ n ] ) {
			return ptrBuckets[ n ];
		}
	}

	return 0;

}

// ============================================================================

const Uint32 SHash::MinNumBits = 4;

const Uint32 SHash::prime_deltas[] = {

	0,	0,	1,	3,	1,	5,	3,	3,
	1,	9,	7,	5,	3,	9,	25,	3,
	1,	21,	3,	21,	7,	15,	9,	5,
	3,	29,	15,	0,	0,	0,	0,	0

};

// ============================================================================

