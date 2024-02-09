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
//	TBFC.STL.SHash.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_STL_SHash_H_
#define _TBFC_STL_SHash_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {

TBFC_PTR_DECL( TBFC_Light_DLL, SHash )

} // namespace TBFC

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"
#include "TBFC.Math.Utils.h"
#include "TBFC.TL.List.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Provides a hash-table-based dictionary, mapping Uint32 to SPtr.
///
/// \ingroup TBFC_STL

class TBFC_Light_DLL SHash : virtual public SObject {

public :

	typedef	Uint32		KeyT;
	typedef	SPtr		ValT;

	typedef	List< KeyT >	KeyList;
	typedef	List< ValT >	ValList;

private :

	struct Node {
	
		Node *		next;
		KeyT		key;
		ValT		val;
	
		Node(
		) {
		}
	
		Node(
			const	Uint32		pKey,
			const	ValT &		pVal
		) :
			key( pKey ),
			val( pVal ) {
		}
	
		Node(
			const	Node &		pOther
		) :
			key( pOther.key ),
			val( pOther.val ) {
		}
	
		Bool same_key(
			const	Uint32		pKey
		) {
			return ( pKey == key );
		}
	
	};

public :

	/// \brief Constructs an empty hash.

	SHash(
	);

	/// \brief Constructs a deep copy of \a pOther.

	SHash(
		const	SHash &		pOther
	);

	/// \brief Destroys the hash.
	///
	/// References to the values in the hash and all iterators of this
	/// hash become invalid.

	virtual ~SHash(
	);

	/// \brief Returns true iff \a pOther is equal to this hash.
	/// 
	/// Two hashes are considered equal if they contain the same (key,
	/// value) pairs.

	Bool operator == (
		const	SHash &		pOther
	) const;

	/// \brief Returns true iff \a pOther is not equal to this hash.
	/// 
	/// Two hashes are considered equal if they contain the same (key,
	/// value) pairs.

	Bool operator != (
		const	SHash &		pOther
	) const {
		return ( ! ( *this == pOther ) );
	}

	/// \brief Returns the number of items in this hash.

	Uint32 getSize(
	) const {
		return nbrEntries;
	}

	/// \brief Returns true iff this hash contains no items.

	Bool isEmpty(
	) const {
		return ( nbrEntries == 0 );
	}

	/// \brief Returns the number of buckets in this SHash's internal hash
	///	table.
	/// 
	/// The sole purpose of this function is to provide a means of fine
	/// tuning SHash's memory usage. In general, you will rarely ever
	/// need to call this function. If you want to know how many items are
	/// in the hash, call getSize().
	/// 
	/// \sa reserve(), squeeze()

	Uint32 getCapacity(
	) const {
		return nbrBuckets;
	}

	/// \brief Ensures that this SHash's internal hash table consists of at
	///	least \a size buckets.
	/// 
	/// This function is useful for code that needs to build a huge hash
	/// and wants to avoid repeated reallocation. For example:
	/// 
	/// \code
	/// 	SHash hash;
	/// 	hash.reserve( 20000 );
	/// 	for ( Uint32 i = 0 ; i < 20000 ; ++i )
	/// 		hash.insert( keys[i], values[i]);
	/// \endcode
	/// 
	/// Ideally, \a size should be slightly more than the maximum number
	/// of items expected in the hash. \a size doesn't have to be prime,
	/// because SHash will use a prime number internally anyway. If \a size
	/// is an underestimate, the worst that will happen is that the SHash
	/// will be a bit slower.
	/// 
	/// In general, you will rarely ever need to call this function.
	/// SHash's internal hash table automatically shrinks or grows to
	/// provide good performance without wasting too much memory.
	/// 
	/// \sa squeeze(), getCapacity()

	void reserve(
		const	Uint32		size
	);

	/// \brief Reduces the size of the SHash's internal hash table to save
	///	memory.
	/// 
	/// The sole purpose of this function is to provide a means of fine
	/// tuning SHash's memory usage. In general, you will rarely ever
	/// need to call this function.

	void squeeze(
	) {
		reserve( 1 );
	}

	/// \brief Removes all items from the hash.
	/// 
	/// \sa remove()

	void kill(
	);

	/// \brief Removes all the items that have the key \a pKey from the
	///	hash.
	///
	/// \returns
	///	The number of items removed which is usually 1 but will
	///	be 0 if the key isn't in the hash, or greater than 1 if
	///	insertMulti() has been used with the \a pKey.
	/// 
	/// \sa clear(), take()

	Uint32 remove(
		const	KeyT		pKey
	);

	/// \brief Removes the item with the key \a pKey from the hash and
	///	returns the value associated with it.
	/// 
	/// If the item does not exist in the hash, the function simply
	/// returns a default-constructed value. If there are multiple
	/// items for \a pKey in the hash, only the most recently inserted one
	/// is removed.
	/// 
	/// If you don't use the return value, remove() is more efficient.
	/// 
	/// \sa remove()

	ValT take(
		const	KeyT		pKey
	);

	/// \brief Returns true iff the hash contains an item with key \a pKey.
	/// 
	/// \sa getCount()

	Bool contains(
		const	KeyT		pKey
	) const {
		return ( findNodePtr( pKey ) != 0 );
	}

	/// \brief Returns the first key with value \a pVal.
	/// 
	/// This function can be slow (linear time), because SHash's
	/// internal data structure is optimized for fast lookup by key, not
	/// by value.
	/// 
	/// \throws NoSuchValue()
	///	If the hash contains no item with value \a pVal.
	/// 
	/// \sa getVal(), getVals()

	KeyT getKey(
		const	ValT &		pVal
	) const;

	/// \brief Returns the value associated with the key \a pKey.
	///
	/// If there are multiple
	/// items for \a pKey in the hash, the value of the most recently
	/// inserted one is returned.
	///
	/// \throws NoSuchKey
	///	If the hash contains no item with key \a pKey.
	/// 
	/// \sa getKey(), getVals(), contains(), operator[]()

	ValT getVal(
		const	KeyT		pKey
	) const {
		Node * node = findNodePtr( pKey );
		if ( node ) {
			return node->val;
		}
		throw NoSuchKey();
	}

	/// \brief Returns the value associated with the key \a pKey.
	/// 
	/// If the hash contains no item with the given \a pKey, the function
	/// returns \a defaultValue.
	/// 
	/// \sa getKey(), getVals(), contains(), operator[]()

	ValT getVal(
		const	KeyT		pKey,
		const	ValT &		defaultValue
	) const {
		Node * node = findNodePtr( pKey );
		if ( node ) {
			return node->val;
		}
		return defaultValue;
	}

	/// \brief Returns the value associated with the key \a pKey as a
	///	modifiable reference.
	/// 
	/// If the hash contains no item with key \a pKey, the function inserts
	/// a default-constructed value into the hash with key \a pKey, and
	/// returns a reference to it. If the hash contains multiple items
	/// with key \a pKey, this function returns a reference to the most
	/// recently inserted value.
	/// 
	/// \sa insert(), getVal()

	ValT & operator [] (
		const	KeyT		pKey
	) {
		mightGrow();
		Node ** node = findNodeRef( pKey );
		if ( ! *node )
			return createNode( pKey, ValT(), node)->val;
		return (*node)->val;
	}

	/// \brief Same as getVal().

	ValT operator [] (
		const	KeyT		pKey
	) const {
		return getVal( pKey );
	}

	/// \brief Returns a list containing all the keys in the map.
	///
	/// Keys that occur multiple times in the hash (because items
	/// were inserted with insertMulti()) occur only
	/// once in the returned list.
	/// 
	/// \sa getKeys(), getVals()

	KeyList getUniqueKeys(
	) const;

	/// \brief Returns a list containing all the keys in the hash.
	///
	/// Keys that occur multiple times in the hash (because items were
	/// inserted with insertMulti()) also occur multiple times in the list.
	/// 
	/// To obtain a list of unique keys, where each key from the map only
	/// occurs once, use getUniqueKeys().
	/// 
	/// \sa getUniqueKeys(), getVals(), getKey()

	KeyList getKeys(
	) const;

	/// \brief Returns a list containing all the keys associated with
	///	value \a pVal.
	/// 
	/// This function can be slow (linear time), because SHash's
	/// internal data structure is optimized for fast lookup by key, not
	/// by value.

	KeyList getKeys(
		const	ValT &		pVal
	) const;

	/// \brief Returns a list containing all the values in the hash.
	///
	/// If a key is associated multiple values, all of
	/// its values will be in the list, and not just the most recently
	/// inserted one.
	/// 
	/// \sa getKeys()

	ValList getVals(
	) const;

	/// \brief Returns a list of all the values associated with key \a pKey,
	///	from the most recently inserted to the least recently inserted.
	/// 
	/// \sa getCount(), insertMulti()

	ValList getVals(
		const	KeyT		pKey
	) const;

	/// \brief Returns the number of items associated with key \a pKey.
	/// 
	/// \sa contains(), insertMulti()

	Uint32 getCount(
		const	KeyT		pKey
	) const;

	class CstIterator;

	/// \brief Provides an STL-style non-const iterator for SHash.
	/// 
	/// \sa SHash::CstIterator

	class Iterator {

	public :

		/// \brief Constructs an uninitialized Iterator.
		/// 
		/// \sa SHash::begin(), SHash::end()

		Iterator(
		) :
			hash( 0 ),
			node( 0 ) {
		}

		/// \brief Returns the current item's key.
		/// 
		/// \sa getVal()

		KeyT getKey(
		) const {
			if ( node ) {
				return node->key;
			}
			throw NullPointer();
		}

		/// \brief Returns a modifiable reference to the current item's
		///	value.
		/// 
		/// You can change the value of an item by using getVal() on
		/// the left side of an assignment, for example:
		/// 
		/// \code
		/// 	if (i.getKey() == "Hello")
		/// 		i.getVal() = "Bonjour";
		/// \endcode
		/// 
		/// \sa getKey(), operator*()

		ValT & getVal(
		) const {
			if ( node ) {
				return node->val;
			}
			throw NullPointer();
		}

		/// \brief Returns a modifiable reference to the current item's
		///	value.
		/// 
		/// Same as getVal().
		/// 
		/// \sa getKey()

		ValT & operator * (
		) const {
			if ( node ) {
				return node->val;
			}
			throw NullPointer();
		}

		/// \brief Returns a pointer to the current item's value.
		/// 
		/// \sa getVal()

		ValT * operator -> (
		) const {
			if ( node ) {
				return &node->val;
			}
			throw NullPointer();
		}

		/// \brief (prefix) Advances the Iterator to the next item in
		///	the hash.

		void operator ++ (
		) {
			if ( node ) {
				node = hash->getNextNode( node );
			}
			else {
				throw NullPointer();
			}
		}

		/// \brief (postfix) Advances the Iterator to the next item in
		///	the hash.

		void operator ++ (
				int
		) {
			if ( node ) {
				node = hash->getNextNode( node );
			}
			else {
				throw NullPointer();
			}
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	Iterator &	pOther
		) const {
			return ( node == pOther.node );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	Iterator &	pOther
		) const {
			return ( node != pOther.node );
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	CstIterator &	pOther
		) const {
			return ( node == pOther.node );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	CstIterator &	pOther
		) const {
			return ( node != pOther.node );
		}

	private :

		SHash *		hash;
		Node *		node;

		friend class SHash::CstIterator;
		friend class SHash;

		Iterator(
				SHash *		pHash,
				Node *		pNode
		) :
			hash( pHash ),
			node( pNode ) {
		}

	};

	friend class Iterator;

	/// \brief Provides an STL-style const iterator for SHash.
	/// 
	/// \sa SHash::Iterator

	class CstIterator {

	public :

		/// \brief Constructs an uninitialized iterator.
		/// 
		/// \sa SHash::constBegin() SHash::constEnd()

		CstIterator(
		) :
			hash( 0 ),
			node( 0 ) {
		}

		/// \brief Constructs a copy of \a pOther.

		CstIterator(
			const	Iterator &	pOther
		) :
			hash( pOther.hash ),
			node( pOther.node ) {
		}

		/// \brief Constructs a copy of \a pOther.

		CstIterator(
			const	CstIterator &	pOther
		) :
			hash( pOther.hash ),
			node( pOther.node ) {
		}

		/// \brief Returns the current item's key.
		/// 
		/// \sa getVal()

		KeyT getKey(
		) const {
			if ( node ) {
				return node->key;
			}
			throw NullPointer();
		}

		/// \brief Returns the current item's value.
		/// 
		/// \sa getKey(), operator*()

		const ValT & getVal(
		) const {
			if ( node ) {
				return node->val;
			}
			throw NullPointer();
		}

		/// \brief Returns the current item's value.
		/// 
		/// Same as getVal().
		/// 
		/// \sa getKey()

		const ValT & operator * (
		) const {
			if ( node ) {
				return node->val;
			}
			throw NullPointer();
		}

		/// \brief Returns a pointer to the current item's value.
		/// 
		/// \sa getVal()

		const ValT * operator -> (
		) const {
			if ( node ) {
				return &node->val;
			}
			throw NullPointer();
		}

		/// \brief (prefix) Advances the iterator to the next item in
		///	the hash.

		void operator ++ () {
			if ( node ) {
				node = hash->getNextNode( node );
			}
			else {
				throw NullPointer();
			}
		}

		/// \brief (postfix) Advances the iterator to the next item in
		///	the hash.

		void operator ++ (
				int
		) {
			if ( node ) {
				node = hash->getNextNode( node );
			}
			else {
				throw NullPointer();
			}
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	Iterator &	pOther
		) const {
			return ( node == pOther.node );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	Iterator &	pOther
		) const {
			return ( node != pOther.node );
		}

		/// \brief Returns true iff \a pOther points to the same item
		///	as this iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	CstIterator &	pOther
		) const {
			return ( node == pOther.node );
		}

		/// \brief Returns true iff \a pOther points to a different
		///	item than this iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	CstIterator &	pOther
		) const {
			return ( node != pOther.node );
		}

	private :

		const SHash *	hash;
		Node *		node;

		friend class SHash::Iterator;
		friend class SHash;

		CstIterator(
			const	SHash *		pHash,
				Node *		pNode
		) :
			hash( pHash ),
			node( pNode ) {
		}

	};

	friend class CstIterator;

	/// \brief Returns an Iterator pointing to the first item in this hash.
	/// 
	/// \sa constBegin(), end()

	Iterator begin(
	) {
		return Iterator( this, getFirstNode() );
	}

	/// \brief Returns a CstIterator pointing to the first item in this hash.
	/// 
	/// \sa constBegin(), end()

	CstIterator begin(
	) const {
		return CstIterator( this, getFirstNode() );
	}

	/// \brief Returns a CstIterator pointing to the first item in this hash.
	/// 
	/// \sa begin(), constEnd()

	CstIterator constBegin(
	) const {
		return CstIterator( this, getFirstNode() );
	}

	/// \brief Returns an Iterator pointing to the imaginary item after the
	///	last item in the hash.
	/// 
	/// \sa begin(), constEnd()

	Iterator end(
	) {
		return Iterator( this, 0 );
	}

	/// \brief Returns a CstIterator pointing to the imaginary item after the
	///	last item in the hash.
	/// 
	/// \sa begin(), constEnd()

	CstIterator end(
	) const {
		return CstIterator( this, 0 );
	}

	/// \brief Returns a CstIterator pointing to the imaginary item after the
	///	last item in the hash.
	/// 
	/// \sa constBegin(), end()

	CstIterator constEnd(
	) const {
		return CstIterator( this, 0 );
	}

	/// \brief Removes the (key, value) pair associated with the Iterator
	///	\a it from this hash, and returns an Iterator to the next item
	///	in this hash.
	/// 
	/// Unlike remove() and take(), this function never causes SHash to
	/// rehash its internal data structure. This means that it can safely
	/// be called while iterating, and won't affect the order of items in
	/// the hash.
	/// 
	/// \sa remove(), take(), find()

	void erase(
			Iterator &	it
	);

	/// \brief Returns an Iterator pointing to the item with key \a pKey in
	///	the hash.
	/// 
	/// If the hash contains no item with key \a pKey, the function
	/// returns end().
	/// 
	/// If the hash contains multiple items with key \a pKey, this
	/// function returns an iterator that points to the most recently
	/// inserted value. The other values are accessible by incrementing
	/// the iterator.
	/// 
	/// \sa getVal(), getVals()

	Iterator find(
		const	KeyT		pKey
	) {
		return Iterator( this, findNodePtr( pKey ) );
	}

	CstIterator find(
		const	KeyT		pKey
	) const {
		return CstIterator( this, findNodePtr( pKey ) );
	}

	/// \brief Returns a CstIterator pointing to the item with key \a pKey
	///	in the hash.
	/// 
	/// If the hash contains no item with key \a pKey, the function
	/// returns constEnd().
	/// 
	/// \sa find()

	CstIterator constFind(
		const	KeyT		pKey
	) const {
		return CstIterator( this, findNodePtr( pKey ) );
	}

	/// \brief Inserts a new item with the key \a pKey and a value of \a pVal.
	/// 
	/// If there is already an item with the key \a pKey, that item's value
	/// is replaced with \a pVal.
	/// 
	/// If there are multiple items with the key \a pKey, the most
	/// recently inserted item's value is replaced with \a pVal.
	/// 
	/// \sa insertMulti()

	void insert(
		const	KeyT		pKey,
		const	ValT &		pVal
	) {
		mightGrow();
		Node ** node = findNodeRef( pKey );
		if ( ! *node ) {
			createNode( pKey, pVal, node );
		}
		else {
			(*node)->val = pVal;
		}
	}

	/// \brief Inserts a new item with the key \a pKey and a value of \a pVal.
	/// 
	/// If there is already an item with the same key in the hash, this
	/// function will simply create a new one. (This behavior is
	/// different from insert(), which overwrites the value of an
	/// existing item.)
	/// 
	/// \sa insert(), getVals()

	void insertMulti(
		const	KeyT		pKey,
		const	ValT &		pVal
	) {
		mightGrow();
		Node ** nextNode = findNodeRef( pKey );
		createNode( pKey, pVal, nextNode );
	}

 	BASE_CLASS_GEN_EXCEPTION( "TBFC.STL.SHash" );
 
 	CLASS_EXCEPTION( InvalidIterator, "Invalid iterator" );
 	CLASS_EXCEPTION( NoSuchKey, "No such key" );
 	CLASS_EXCEPTION( NoSuchValue, "No such value" );

protected :

	/// \brief Returns the prime associated to a power of two.
	///
	/// For example, primeForNumBits(8) returns 257.

	static Uint32 primeForNumBits(
		const	Uint32		curNbrBits
	) {
		return ( 1U << curNbrBits ) + prime_deltas[ curNbrBits ];
	}

	Node ** findNodeRef(
		const	KeyT		pKey
	) const {
		Node **	node = &ptrBuckets[ pKey % nbrBuckets ];
		while ( *node && (*node)->key != pKey )
			node = &(*node)->next;
		return node;
	}

	Node * findNodePtr(
		const	KeyT		pKey
	) const {
		Node *	node = ptrBuckets[ pKey % nbrBuckets ];
		while ( node && node->key != pKey )
			node = node->next;
		return node;
	}

	Node * createNode(
		const	KeyT		pKey,
		const	ValT &		pVal,
			Node **		pNxt
	) {
		Node * node = new Node( pKey, pVal );
		node->next = *pNxt;
		*pNxt = node;
		++nbrEntries;
		return node;
	}

	void mightGrow(
	) {
		if ( nbrEntries >= nbrBuckets ) {
			rehash( curNbrBits + 1 );
		}
	}

	void hasShrunk(
	) {
		if ( nbrEntries <= ( nbrBuckets >> 3 )
		  && curNbrBits > usrNbrBits ) {
			rehash( Math::getMax( curNbrBits - 2, usrNbrBits ) );
		}
	}

	/// \brief Resizes the buckets array to contain a minimum of
	///	(1 << \a pUsrNbrBits) buckets.

	void rehash(
		const	Uint32		pUsrNbrBits
	);

	Node * getFirstNode(
	) const;

	Node * getNextNode(
			Node *		node
	) const;

private :

	Node **		ptrBuckets;	///< Array of buckets.
	Uint32		nbrEntries;	///< Number of entries.
	Uint32		usrNbrBits;	///< Minimum number of bits required by user.
	Uint32		curNbrBits;
	Uint32		nbrBuckets;

	/// \brief Table of selected prime values (coded minus 2 ^^ index).
	///
	/// The primes we are using are 1, 2, 5, 11, 17, 37, 67, 131, 257, ...,
	/// i.e. primes in the immediate surrounding of a power of two.

	static const Uint32 MinNumBits;
	static const Uint32 prime_deltas[];

	SHash & operator = (
		const	SHash &	pOther
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_STL_SHash_H_

// ============================================================================

