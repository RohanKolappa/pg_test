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
//							Free Software Foundation
//							59 Temple Place, Suite 330
//							Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.TL.Hash.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdlib.h>

#include "BFC.TL.Hash.h"

// ============================================================================

using namespace BFC;

// ============================================================================

// A Hash has initially around pow(2, MinNumBits) buckets. For example, if
// MinNumBits is 4, it has 17 buckets.

static const Int32 MinNumBits = 4;

// ============================================================================

// These functions are based on Peter J. Weinberger's hash function (from the
// Dragon Book). The constant 24 in the original function was replaced with 23
// to produce fewer collisions on input such as "a", "aa", "aaa", "aaaa", ...

Uint32 BFC::getHash(
	const	Buffer &	key ) {

	const Uchar *	p = key.getCstPtr();
	Int32	n = key.getLength();
	Uint32	h = 0;
	Uint32	g;

	while ( n-- ) {
		h = ( h << 4 ) + *p++;
		if ((g = (h & 0xf0000000)) != 0)
			h ^= g >> 23;
		h &= ~g;
	}

	return h;

}

// ============================================================================

// The prime_deltas array is a table of selected prime values, even though it
// doesn't look like one. The primes we are using are 1, 2, 5, 11, 17, 37, 67,
// 131, 257, ..., i.e. primes in the immediate surrounding of a power of two.

// The primeForNumBits() function returns the prime associated to a power of
// two. For example, primeForNumBits(8) returns 257.

static const Uchar prime_deltas[] = {
	0,	0,	1,	3,	1,	5,	3,	3,
	1,	9,	7,	5,	3,	9,	25,	3,
	1,	21,	3,	21,	7,	15,	9,	5,
	3,	29,	15,	0,	0,	0,	0,	0
};

static inline Int32 primeForNumBits(
	const	Int32		numBits ) {

	return ( ( 1 << numBits ) + prime_deltas[ numBits & 0x1F ] );

}

// Returns the smallest integer n such that primeForNumBits(n) >= hint.

static Int32 countBits(
	const	Int32		hint ) {

	Int32 numBits = 0;
	Int32 bits = hint;

	while ( bits > 1 ) {
		bits >>= 1;
		numBits++;
	}

	if ( numBits >= ( Int32 )sizeof( prime_deltas ) ) {
		numBits = sizeof( prime_deltas ) - 1;
	}
	else if ( primeForNumBits( numBits ) < hint ) {
		++numBits;
	}

	return numBits;

}

// ============================================================================
// BFC::TL::HashData
// ============================================================================

HashData HashData::shared_null = {
	0, 0, 1, 0, 0, MinNumBits, 0, 0
};

// ============================================================================

void * HashData::allocateNode() {

	return ::malloc(nodeSize);

}

void HashData::freeNode(void *node)
{
	::free(node);
}

HashData * HashData::detach_helper(
		void (*node_duplicate)(Node *, void *),
		Int32 nodeSize)
{
	union {
		HashData *d;
		Node *e;
	};
	d = new HashData;
	d->fakeNext = 0;
	d->buckets = 0;
	d->refCount = 1;
	d->size = size;
	d->nodeSize = nodeSize;
	d->userNumBits = userNumBits;
	d->numBits = numBits;
	d->numBuckets = numBuckets;

	if (numBuckets) {
		d->buckets = new Node *[numBuckets];
		Node *this_e = reinterpret_cast<Node *>(this);
		for (Int32 i = 0; i < numBuckets; ++i) {
			Node **nextNode = &d->buckets[i];
			Node *oldNode = buckets[i];
			while (oldNode != this_e) {
				Node *dup = static_cast<Node *>(allocateNode());
				node_duplicate(oldNode, dup);
				dup->h = oldNode->h;
				*nextNode = dup;
				nextNode = &dup->next;
				oldNode = oldNode->next;
			}
			*nextNode = e;
		}
	}
	return d;
}

HashData::Node *HashData::nextNode(Node *node)
{
	union {
		Node *next;
		Node *e;
		HashData *d;
	};
	next = node->next;
//	Q_ASSERT_X(next, "Hash", "Iterating beyond end()");
	if (next->next)
		return next;

	Int32 start = (node->h % d->numBuckets) + 1;
	Node **bucket = d->buckets + start;
	Int32 n = d->numBuckets - start;
	while (n--) {
		if (*bucket != e)
			return *bucket;
		++bucket;
	}
	return e;
}

HashData::Node *HashData::previousNode(Node *node)
{
	union {
		Node *e;
		HashData *d;
	};

	e = node;
	while (e->next)
		e = e->next;

	Int32 start;
	if (node == e)
		start = d->numBuckets - 1;
	else
		start = node->h % d->numBuckets;

	Node *sentinel = node;
	Node **bucket = d->buckets + start;
	while (start >= 0) {
		if (*bucket != sentinel) {
			Node *prev = *bucket;
			while (prev->next != sentinel)
				prev = prev->next;
			return prev;
		}

		sentinel = e;
		--bucket;
		--start;
	}
//	Q_ASSERT_X(start >= 0, "Hash", "Iterating backward beyond begin()");
	return e;
}

/*
	If hint is negative, -hint gives the approximate number of
	buckets that should be used for the hash table. If hint is
	nonnegative, (1 << hint) gives the approximate number
	of buckets that should be used.
*/
void HashData::rehash(Int32 hint)
{
	if (hint < 0) {
		hint = countBits(-hint);
		if (hint < MinNumBits)
			hint = MinNumBits;
		userNumBits = ( Int16 )hint;
		while (primeForNumBits(hint) < (size >> 1))
			++hint;
	} else if (hint < MinNumBits) {
		hint = MinNumBits;
	}

	if (numBits != hint) {
		Node *e = reinterpret_cast<Node *>(this);
		Node **oldBuckets = buckets;
		Int32 oldNumBuckets = numBuckets;

		numBits = ( Int16 )hint;
		numBuckets = primeForNumBits(hint);
		buckets = new Node *[numBuckets];
		for (Int32 i = 0; i < numBuckets; ++i)
			buckets[i] = e;

		for (Int32 i = 0; i < oldNumBuckets; ++i) {
			Node *firstNode = oldBuckets[i];
			while (firstNode != e) {
				Uint32 h = firstNode->h;
				Node *lastNode = firstNode;
				while (lastNode->next != e && lastNode->next->h == h)
					lastNode = lastNode->next;

				Node *afterLastNode = lastNode->next;
				Node **beforeFirstNode = &buckets[h % numBuckets];
				while (*beforeFirstNode != e)
					beforeFirstNode = &(*beforeFirstNode)->next;
				lastNode->next = *beforeFirstNode;
				*beforeFirstNode = firstNode;
				firstNode = afterLastNode;
			}
		}
		delete [] oldBuckets;
	}
}

void HashData::destroyAndFree()
{
	delete [] buckets;
	delete this;
}

// ============================================================================

