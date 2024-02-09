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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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
//	BFC.STL.SHash.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.STL.SHash.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( STL, SHash )

// ============================================================================

STL::SHash::SHash() :

	ptrBuckets	( 0 ),
	nbrEntries	( 0 ),
	usrNbrBits	( MinNumBits ),
	curNbrBits	( 0 ),
	nbrBuckets	( 0 ) {

	reserve( 1 );

}

STL::SHash::SHash(
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

STL::SHash::~SHash() {

	kill();

}

// ============================================================================

Bool STL::SHash::operator == (
	const	SHash &		pOther ) const {

	if ( this == &pOther ) {
		return true;
	}

	if ( getSize() != pOther.getSize() ) {
		return false;
	}

	CstIterator it = constFirstElt();

	while ( it ) {
		Uint32 key = it.getKey();
		CstIterator it2 = pOther.constFind( key );
		do {
			if ( ! it2 || ! ( it2.getKey() == key ) )
				return false;
			if ( !( it.getVal() == it2.getVal() ) )
				return false;
			++it;
			++it2;
		} while ( it && it.getKey() == key );
	}

	return true;

}

// ============================================================================

void STL::SHash::reserve(
	const	Uint32		pSize ) {

	Uint32	bits = 0;
	Uint32	size = pSize;

	while ( size ) {
		size >>= 1;
		bits++;
	}

	rehash( bits );

}

void STL::SHash::kill() {

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

Uint32 STL::SHash::remove(
	const	Uint32		pKey ) {

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

SPtr STL::SHash::take(
	const	Uint32		pKey ) {

	Node **	node = findNodeRef( pKey );
	SPtr	res;

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

Uint32 STL::SHash::getKey(
	const	SPtr &		pVal ) const {

	for ( CstIterator i = constFirstElt() ; i ; i++ ) {
		if ( i.getVal() == pVal )
			return i.getKey();
	}

	throw NoSuchValue();

}

// ============================================================================

STL::SHash::KeyList STL::SHash::getUniqueKeys() const {

	KeyList	res;

	for ( CstIterator i = constFirstElt() ; i ; ) {
		Uint32 key = i.getKey();
		res += key;
		i++;
		while ( i && i.getKey() == key ) {
			i++;
		}
	}

	return res;

}

STL::SHash::KeyList STL::SHash::getKeys() const {

	KeyList res;

	for ( CstIterator i = constFirstElt() ; i ; i++ ) {
		res += i.getKey();
	}

	return res;

}

STL::SHash::KeyList STL::SHash::getKeys(
	const	SPtr &		pVal ) const {

	KeyList res;

	for ( CstIterator i = constFirstElt() ; i ; i++ ) {
		if ( i.getVal() == pVal ) {
			res += i.getKey();
		}
	}

	return res;

}

STL::SHash::ValList STL::SHash::getVals() const {

	ValList res;

	for ( CstIterator i = constFirstElt() ; i ; i++ ) {
		res += i.getVal();
	}

	return res;

}

STL::SHash::ValList STL::SHash::getVals(
	const	Uint32		pKey ) const {

	ValList res;

	for ( Node * node = findNodePtr(pKey)
	    ; node && node->key == pKey
	    ; node = node->next ) {
		res += node->val;
	}

	return res;

}

// ============================================================================

Uint32 STL::SHash::getCount(
	const	Uint32		pKey ) const {

	Uint32	cnt = 0;

	for ( Node * node = findNodePtr( pKey )
	    ; node && node->key == pKey
	    ; node = node->next ) {
		cnt++;
	}

	return cnt;

}

// ============================================================================

void STL::SHash::pop(
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

void STL::SHash::rehash(
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

STL::SHash::Node * STL::SHash::getFirstNode() const {

	for ( Uint32 n = 0 ; n < nbrBuckets ; n++ ) {
		if ( ptrBuckets[ n ] ) {
			return ptrBuckets[ n ];
		}
	}

	return 0;

}

STL::SHash::Node * STL::SHash::getNextNode(
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

const Uint32 STL::SHash::MinNumBits = 4;

const Uint32 STL::SHash::prime_deltas[] = {

	0,	0,	1,	3,	1,	5,	3,	3,
	1,	9,	7,	5,	3,	9,	25,	3,
	1,	21,	3,	21,	7,	15,	9,	5,
	3,	29,	15,	0,	0,	0,	0,	0

};

// ============================================================================

