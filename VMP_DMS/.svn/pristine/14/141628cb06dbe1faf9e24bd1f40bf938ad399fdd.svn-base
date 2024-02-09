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
//	BFC.TL.Hash.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_Hash_H_
#define _BFC_TL_Hash_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include <new>

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Math.Utils.h"

#include "BFC.TL.List.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Uchar		key
) {
	return ( Uint32 )key;
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Char		key
) {
	return ( Uint32 )( Uchar )key;
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Uint16		key
) {
	return ( Uint32 )key;
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Int16		key
) {
	return ( Uint32 )( Uint16 )key;
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Uint32		key
) {
	return key;
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Int32		key
) {
	return ( Uint32 )key;
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Uint64		key
) {
	return ( Uint32 )( ( key >> 32 ) ^ key );
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

inline Uint32 getHash(
	const	Int64		key
) {
	return getHash( ( Uint64 )key );
}

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

BFC_Light_DLL Uint32 getHash(
	const	Buffer &	key
);

/// \brief Returns the hash value for \a key.
///
/// \ingroup BFC_TL

template < class TVal >
inline Uint32 getHash(
	const	TVal *		key
) {
	return ( sizeof( const TVal * ) > sizeof( Uint32 )
		? getHash( static_cast< Uint64 >( key ) )
		: static_cast< Uint32 >( key ) );
}

// ============================================================================

struct BFC_Light_DLL HashData {

	struct Node {
		Node *	next;
		Uint32	h;
	};

	Node *		fakeNext;
	Node **		buckets;
	Uint32		refCount;
	Int32		size;
	Int32		nodeSize;
	Int16		userNumBits;
	Int16		numBits;
	Int32		numBuckets;

	void * allocateNode(
	);

	void freeNode(
			void *		node
	);

	HashData * detach_helper(
			void		( *node_duplicate )( Node *, void * ),
			Int32		nodeSize
	);

	void mightGrow(
	) {
		if ( size >= numBuckets ) {
			rehash( numBits + 1 );
		}
	}

	void hasShrunk(
	) {
		if ( size <= ( numBuckets >> 3 )
		  && numBits > userNumBits ) {
			rehash( Math::getMax(
				( Int32 )( numBits - 2 ),
				( Int32 )( userNumBits ) ) );
		}
	}

	void rehash(
			Int32		hint
	);

	void destroyAndFree(
	);

	Node * firstNode(
	) {
		Node *	e = reinterpret_cast< Node * >( this );
		Node **	bucket = buckets;
		Int32	n = numBuckets;
		while ( n-- ) {
			if ( *bucket != e ) {
				return *bucket;
			}
			++bucket;
		}
		return e;
	}

	static Node * nextNode(
			Node *		node
	);

	static Node * previousNode(
			Node *		node
	);

	static HashData shared_null;

};

// ============================================================================

template < class TKey, class TVal >
struct HashNode {

	HashNode *	next;
	Uint32		h;
	TKey		key;
	TVal		value;

	HashNode(
		const	TKey &		key0
	) :
		key( key0 ) {
	} // ### remove in 5.0

	HashNode(
		const	TKey &		key0,
		const	TVal &		value0
	) :
		key( key0 ),
		value( value0 ) {
	}

	Bool same_key(
		const	Uint32		h0,
		const	TKey &		key0
	) {
		return ( h0 == h && key0 == key );
	}

};

template < class TVal >
struct HashNode< Int16, TVal > {

	HashNode *	next;
	union {
		Uint32		h;
		Int16		key;
	};
	TVal		value;

	HashNode(
		const	Int16		/* key0 */
	) {
	}

	HashNode(
		const	Int16		/* key0 */,
		const	TVal &		value0
	) :
		value( value0 ) {
	}

	Bool same_key(
		const	Uint32		h0,
		const	Int16
	) {
		return ( h0 == h );
	}

};

template < class TVal >
struct HashNode< Uint16, TVal > {

	HashNode *	next;
	union {
		Uint32		h;
		Uint16		key;
	};
	TVal		value;

	HashNode(
		const	Uint16		/* key0 */
	) {
	}

	HashNode(
		const	Uint16		/* key0 */,
		const	TVal &		value0
	) :
		value( value0 ) {
	}

	Bool same_key(
		const	Uint32		h0,
		const	Uint16
	) {
		return ( h0 == h );
	}

};

template < class TVal >
struct HashNode< Int32, TVal > {

	HashNode *	next;
	union {
		Uint32		h;
		Int32		key;
	};
	TVal		value;

	HashNode(
		const	Int32		/* key0 */
	) {
	}

	HashNode(
		const	Int32		/* key0 */,
		const	TVal &		value0
	) :
		value( value0 ) {
	}

	Bool same_key(
		const	Uint32		h0,
		const	Int32
	) {
		return ( h0 == h );
	}

};

template < class TVal >
struct HashNode< Uint32, TVal > {

	HashNode *	next;
	union {
		Uint32		h;
		Uint32		key;
	};
	TVal		value;

	HashNode(
		const	Uint32		/* key0 */
	) {
	}

	HashNode(
		const	Uint32		/* key0 */,
		const	TVal &		value0
	) :
		value( value0 ) {
	}

	Bool same_key(
		const	Uint32		h0,
		const	Uint32
	) {
		return ( h0 == h );
	}

};

// ============================================================================

/// \brief Provides a hash-table-based dictionary.
/// 
/// Hash\<TKey, TVal\> stores (key, value) pairs and provides very fast lookup of
/// the value associated with a key.
/// 
/// Here's an example Hash with Buffer keys and \c Int32 values:
/// \code
/// 	Hash< Buffer, Int32 > hash;
/// \endcode
/// 
/// To insert a (key, value) pair into the hash, you can use operator[]():
/// 
/// \code
/// 	hash["one"] = 1;
/// 	hash["three"] = 3;
/// 	hash["seven"] = 7;
/// \endcode
/// 
/// This inserts the following three (key, value) pairs into the
/// Hash: ("one", 1), ("three", 3), and ("seven", 7). Another way to
/// insert items into the hash is to use insert():
/// 
/// \code
/// 	hash.insert("twelve", 12);
/// \endcode
/// 
/// To look up a value, use operator[]() or getVal():
/// 
/// \code
/// 	Int32 num1 = hash["thirteen"];
/// 	Int32 num2 = hash.getVal("thirteen");
/// \endcode
/// 
/// If there is no item with the specified key in the hash, these
/// functions return a default-constructed value.
/// 
/// If you want to check whether the hash contains a particular key,
/// use contains():
/// 
/// \code
/// 	Int32 timeout = 30;
/// 	if (hash.contains("TIMEOUT"))
/// 		timeout = hash.getVal("TIMEOUT");
/// \endcode
/// 
/// There is also a getVal() overload that uses its second argument as
/// a default value if there is no item with the specified key:
/// 
/// \code
/// 	Int32 timeout = hash.getVal("TIMEOUT", 30);
/// \endcode
/// 
/// In general, we recommend that you use contains() and getVal()
/// rather than operator[]() for looking up a key in a hash. The
/// reason is that operator[]() silently inserts an item into the
/// hash if no item exists with the same key (unless the hash is
/// const). For example, the following code snippet will create 1000
/// items in memory:
/// 
/// \code
/// 	// WRONG
/// 	Hash<Int32, QWidget *> hash;
/// 	...
/// 	for (Int32 i = 0; i < 1000; ++i) {
/// 		if (hash[i] == okButton)
/// 			cout << "Found button at index " << i << endl;
/// 	}
/// \endcode
/// 
/// To avoid this problem, replace \c hash[i] with \c hash.getVal(i)
/// in the code above.
/// 
/// If you want to navigate through all the (key, value) pairs stored
/// in a Hash, you can use an iterator. Hash provides both
/// Java-style iterators (HashIterator and QMutableHashIterator)
/// and STL-style iterators (Hash::CstIterator and
/// Hash::Iterator). Here's how to iterate over a Hash<Buffer,
/// Int32> using a Java-style iterator:
/// 
/// \code
/// 	HashIterator<Buffer, Int32> i(hash);
/// 	while (i.hasNext()) {
/// 		i.next();
/// 		cout << i.getKey() << ": " << i.getVal() << endl;
/// 	}
/// \endcode
/// 
/// Here's the same code, but using an STL-style iterator:
/// 
/// \code
/// 	Hash<Buffer, Int32>::CstIterator i = hash.constBegin();
/// 	while (i != hash.constEnd()) {
/// 		cout << i.getKey() << ": " << i.getVal() << endl;
/// 		++i;
/// 	}
/// \endcode
/// 
/// Hash is unordered, so an iterator's sequence cannot be assumed
/// to be predictable. If ordering by key is required, use a QMap.
/// 
/// Normally, a Hash allows only one value per key. If you call
/// insert() with a key that already exists in the Hash, the
/// previous value is erased. For example:
/// 
/// \code
/// 	hash.insert("plenty", 100);
/// 	hash.insert("plenty", 2000);
/// 	// hash.getVal("plenty") == 2000
/// \endcode
/// 
/// However, you can store multiple values per key by using
/// insertMulti() instead of insert() (or using the convenience
/// subclass MultiHash). If you want to retrieve all
/// the values for a single key, you can use getVals(const TKey &key),
/// which returns a List< TVal >:
/// 
/// \code
/// 	List<Int32> values = hash.getVals("plenty");
/// 	for (Int32 i = 0; i < values.getSize(); ++i)
/// 		cout << values.at(i) << endl;
/// \endcode
/// 
/// The items that share the same key are available from most
/// recently to least recently inserted. A more efficient approach is
/// to call find() to get the iterator for the first item with a key
/// and iterate from there:
/// 
/// \code
/// 	Hash<Buffer, Int32>::Iterator i = hash.find("plenty");
/// 	while (i != hash.end() && i.getKey() == "plenty") {
/// 		cout << i.getVal() << endl;
/// 		++i;
/// 	}
/// \endcode
/// 
/// Items can be removed from the hash in several ways. One way is to
/// call remove(); this will remove any item with the given key.
/// In addition, you can clear the entire hash using clear().
/// 
/// Hash's key and value data types must be assignable data
/// types. In addition, Hash's key type must
/// provide operator==(), and there must also be a global getHash()
/// function that returns a hash value for an argument of the key's
/// type.
/// 
/// The getHash() function computes a numeric value based on a key. It
/// can use any algorithm imaginable, as long as it always returns
/// the same value if given the same argument. In other words, if
/// e1 == e2, then getHash(e1) == getHash(e2) must hold as well.
/// However, to obtain good performance, the getHash() function should
/// attempt to return different hash values for different keys to the
/// largest extent possible.
/// 
/// Internally, Hash uses a hash table to perform lookups. Hash's hash table
/// automatically grows and shrinks to
/// provide fast lookups without wasting too much memory. You can
/// still control the size of the hash table by calling reserve() if
/// you already know approximately how many items the Hash will
/// contain, but this isn't necessary to obtain good performance. You
/// can also call getCapacity() to retrieve the hash table's size.
///
/// \ingroup BFC_Light

template < class TKey, class TVal >
class Hash {

public :

	/// \brief Constructs an empty hash.

	Hash(
	) :
		d( &HashData::shared_null ) {
		d->refCount++;
	}

	/// \brief Constructs a copy of \a other.
	/// 
	/// This operation occurs in constant time, because Hash is
	/// implicitly shared. This makes returning a Hash from a
	/// function very fast. If a shared instance is modified, it will be
	/// copied (copy-on-write), and this takes linear time.

	Hash(
		const	Hash< TKey, TVal > &	other
	) :
		d( other.d ) {
		d->refCount++;
	}

	/// \brief Destroys the hash.
	///
	/// References to the values in the hash and all iterators of this
	/// hash become invalid.

	~Hash(
	) {
		if ( ! --d->refCount )
			freeData(d);
	}

	/// \brief Assigns \a other to this hash and returns a reference to
	///	this hash.

	Hash< TKey, TVal > & operator = (
		const	Hash< TKey, TVal > &	other
	);

	/// \brief Returns true iff \a other is equal to this hash.
	/// 
	/// Two hashes are considered equal if they contain the same (key,
	/// value) pairs.

	Bool operator == (
		const	Hash< TKey, TVal > &	other
	) const;

	/// \brief Returns true iff \a other is not equal to this hash.
	/// 
	/// Two hashes are considered equal if they contain the same (key,
	/// value) pairs.

	Bool operator != (
		const	Hash< TKey, TVal > &	other
	) const {
		return !(*this == other);
	}

	/// \brief Returns the number of items in the hash.

	Int32 getSize(
	) const {
		return d->size;
	}

	/// \brief Returns true iff the hash contains no items.

	Bool isEmpty(
	) const {
		return ( d->size == 0 );
	}

	/// \brief Returns the number of buckets in the Hash's internal hash
	///	table.
	/// 
	/// The sole purpose of this function is to provide a means of fine
	/// tuning Hash's memory usage. In general, you will rarely ever
	/// need to call this function. If you want to know how many items are
	/// in the hash, call getSize().
	/// 
	/// \sa reserve(), squeeze()

	Int32 getCapacity(
	) const {
		return ( d->numBuckets );
	}

	/// \brief Ensures that the Hash's internal hash table consists of at
	///	least \a size buckets.
	/// 
	/// This function is useful for code that needs to build a huge hash
	/// and wants to avoid repeated reallocation. For example:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32> hash;
	/// 	hash.reserve(20000);
	/// 	for (Int32 i = 0; i < 20000; ++i)
	/// 		hash.insert(keys[i], values[i]);
	/// \endcode
	/// 
	/// Ideally, \a size should be slightly more than the maximum number
	/// of items expected in the hash. \a size doesn't have to be prime,
	/// because Hash will use a prime number internally anyway. If \a size
	/// is an underestimate, the worst that will happen is that the Hash
	/// will be a bit slower.
	/// 
	/// In general, you will rarely ever need to call this function.
	/// Hash's internal hash table automatically shrinks or grows to
	/// provide good performance without wasting too much memory.
	/// 
	/// \sa squeeze(), getCapacity()

	void reserve(
		const	Int32		size
	);

	/// \brief Reduces the size of the Hash's internal hash table to save
	///	memory.
	/// 
	/// The sole purpose of this function is to provide a means of fine
	/// tuning Hash's memory usage. In general, you will rarely ever
	/// need to call this function.

	void squeeze(
	) {
		reserve( 1 );
	}

	/// \brief Detaches this hash from any other hashes with which it
	///	may share data.
	/// 
	/// \sa isDetached()

	void detach(
	) {
		if ( d->refCount != 1 ) {
			detach_helper();
		}
	}

	/// \brief Returns true iff the hash's internal data isn't shared with
	///	any other hash object.
	/// 
	/// \sa detach()

	Bool isDetached(
	) const {
		return ( d->refCount == 1 );
	}

	/// \brief Removes all items from the hash.
	/// 
	/// \sa remove()

	void clear(
	);

	/// \brief Removes all the items that have the key \a key from the
	///	hash.
	///
	/// \returns
	///	The number of items removed which is usually 1 but will
	///	be 0 if the key isn't in the hash, or greater than 1 if
	///	insertMulti() has been used with the \a key.
	/// 
	/// \sa clear(), take()

	Int32 remove(
		const	TKey &		key
	);

	/// \brief Removes the item with the key \a key from the hash and
	///	returns the value associated with it.
	/// 
	/// If the item does not exist in the hash, the function simply
	/// returns a default-constructed value. If there are multiple
	/// items for \a key in the hash, only the most recently inserted one
	/// is removed.
	/// 
	/// If you don't use the return value, remove() is more efficient.
	/// 
	/// \sa remove()

	TVal take(
		const	TKey &		key
	);

	/// \brief Returns true iff the hash contains an item with key \a key.
	/// 
	/// \sa getCount()

	Bool contains(
		const	TKey &		key
	) const;

	/// \brief Returns the first key with value \a value.
	/// 
	/// If the hash contains no item with value \a value, the function
	/// returns a default-constructed key.
	/// 
	/// This function can be slow (linear time), because Hash's
	/// internal data structure is optimized for fast lookup by key, not
	/// by value.
	/// 
	/// \sa getVal(), getVals()

	const TKey getKey(
		const	TVal &		value
	) const;

	/// \brief Returns the value associated with the key \a key.
	/// 
	/// If the hash contains no item with key \a key, the function
	/// returns a default-constructed value. If there are multiple
	/// items for \a key in the hash, the value of the most recently
	/// inserted one is returned.
	/// 
	/// \sa getKey(), getVals(), contains(), operator[]()

	const TVal getVal(
		const	TKey &		key
	) const;

	/// \brief Returns the value associated with the key \a key.
	/// 
	/// If the hash contains no item with the given \a key, the function
	/// returns \a defaultValue.
	/// 
	/// \sa getKey(), getVals(), contains(), operator[]()

	const TVal getVal(
		const	TKey &		key,
		const	TVal &		defaultValue
	) const;

	/// \brief Returns the value associated with the key \a key as a
	///	modifiable reference.
	/// 
	/// If the hash contains no item with key \a key, the function inserts
	/// a default-constructed value into the hash with key \a key, and
	/// returns a reference to it. If the hash contains multiple items
	/// with key \a key, this function returns a reference to the most
	/// recently inserted value.
	/// 
	/// \sa insert(), getVal()

	TVal & operator [] (
		const	TKey &		key
	);

	/// \brief Same as getVal().

	const TVal operator [] (
		const	TKey &		key
	) const;

	/// \brief Returns a list containing all the keys in the map in
	///	ascending order.
	///
	/// Keys that occur multiple times in the map (because items
	/// were inserted with insertMulti(), or unite() was used) occur only
	/// once in the returned list.
	/// 
	/// \sa getKeys(), getVals()

	List< TKey > getUniqueKeys(
	) const;

	/// \brief Returns a list containing all the keys in the hash, in an
	///	arbitrary order.
	///
	/// Keys that occur multiple times in the hash
	/// (because items were inserted with insertMulti(), or unite() was
	/// used) also occur multiple times in the list.
	/// 
	/// To obtain a list of unique keys, where each key from the map only
	/// occurs once, use getUniqueKeys().
	/// 
	/// The order is guaranteed to be the same as that used by getVals().
	/// 
	/// \sa getUniqueKeys(), getVals(), getKey()

	List< TKey > getKeys(
	) const;

	/// \brief Returns a list containing all the keys associated with
	///	value \a value, in an arbitrary order.
	/// 
	/// This function can be slow (linear time), because Hash's
	/// internal data structure is optimized for fast lookup by key, not
	/// by value.

	List< TKey > getKeys(
		const	TVal &		value
	) const;

	/// \brief Returns a list containing all the values in the hash, in an
	///	arbitrary order.
	///
	/// If a key is associated multiple values, all of
	/// its values will be in the list, and not just the most recently
	/// inserted one.
	/// 
	/// The order is guaranteed to be the same as that used by getKeys().
	/// 
	/// \sa getKeys()

	List< TVal > getVals(
	) const;

	/// \brief Returns a list of all the values associated with key \a key,
	///	from the most recently inserted to the least recently inserted.
	/// 
	/// \sa getCount(), insertMulti()

	List< TVal > getVals(
		const	TKey &		key
	) const;

	/// \brief Returns the number of items associated with key \a key.
	/// 
	/// \sa contains(), insertMulti()

	Int32 getCount(
		const	TKey &		key
	) const;

	class CstIterator;

	/// \brief Provides an STL-style non-const iterator for Hash and MultiHash.
	/// 
	/// Hash\<TKey, TVal\>::Iterator allows you to iterate over a Hash (or
	/// MultiHash) and to modify the value (but not the key) associated
	/// with a particular key. If you want to iterate over a const Hash,
	/// you should use Hash::CstIterator. It is generally good
	/// practice to use Hash::CstIterator on a non-const Hash as
	/// well, unless you need to change the Hash through the iterator.
	/// Const iterators are slightly faster, and can improve code
	/// readability.
	/// 
	/// The default Hash::Iterator constructor creates an uninitialized
	/// Iterator. You must initialize it using a Hash function like
	/// Hash::begin(), Hash::end(), or Hash::find() before you can
	/// start iterating. Here's a typical loop that prints all the (key,
	/// value) pairs stored in a hash:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32> hash;
	/// 	hash.insert("January", 1);
	/// 	hash.insert("February", 2);
	/// 	...
	/// 	hash.insert("December", 12);
	/// 
	/// 	Hash<Buffer, Int32>::Iterator i;
	/// 	for (i = hash.begin(); i != hash.end(); ++i)
	/// 		cout << i.getKey() << ": " << i.getVal() << endl;
	/// \endcode
	/// 
	/// Let's see a few examples of things we can do with a
	/// Hash::Iterator that we cannot do with a Hash::CstIterator.
	/// Here's an example that increments every value stored in the Hash
	/// by 2:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32>::Iterator i;
	/// 	for (i = hash.begin(); i != hash.end(); ++i)
	/// 		i.getVal() += 2;
	/// \endcode
	/// 
	/// Here's an example that removes all the items whose key is a
	/// string that starts with an underscore character:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32>::Iterator i = hash.begin();
	/// 	while (i != hash.end()) {
	/// 		if (i.getKey().beginsWith("_"))
	/// 			i = hash.erase(i);
	/// 		else
	/// 			++i;
	/// 	}
	/// \endcode
	/// 
	/// The call to Hash::erase() removes the item pointed to by the
	/// Iterator from the hash, and returns an Iterator to the next item.
	/// Here's another way of removing an item while iterating:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32>::Iterator i = hash.begin();
	/// 	while (i != hash.end()) {
	/// 		Hash<Buffer, Int32>::Iterator prev = i;
	/// 		++i;
	/// 		if (prev.getKey().beginsWith("_"))
	/// 			hash.erase(prev);
	/// 	}
	/// \endcode
	/// 
	/// Multiple Iterators can be used on the same hash. However, be
	/// aware that any modification performed directly on the Hash has
	/// the potential of dramatically changing the order in which the
	/// items are stored in the hash, as they might cause Hash to rehash
	/// its internal data structure. There is one notable exception:
	/// Hash::erase(). This function can safely be called while
	/// iterating, and won't affect the order of items in the hash. If you
	/// need to keep Iterators over a long period of time, we recommend
	/// that you use QMap rather than Hash.
	/// 
	/// \sa Hash::CstIterator

	class Iterator {

	public :

		typedef	TVal		ValType;
		typedef	TVal *		PtrType;
		typedef	TVal &		RefType;

		/// \brief Constructs an uninitialized Iterator.
		/// 
		/// Functions like getKey(), getVal(), and operator++() must not be
		/// called on an uninitialized Iterator. Use operator=() to assign a
		/// value to it before using it.
		/// 
		/// \sa Hash::begin() Hash::end()

		Iterator(
		) : i( 0 ) {
		}

		explicit Iterator(
				void *		node
		) : i( reinterpret_cast<HashData::Node *>(node) ) {
		}

		/// \brief Returns the current item's key as a const reference.
		/// 
		/// There is no direct way of changing an item's key through an
		/// Iterator, although it can be done by calling Hash::erase()
		/// followed by Hash::insert() or Hash::insertMulti().
		/// 
		/// \sa getVal()

		const TKey & getKey(
		) const {
			return concrete(i)->key;
		}

		/// \brief Returns a modifiable reference to the current item's value.
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

		TVal & getVal(
		) const {
			return concrete(i)->value;
		}

		/// \brief Returns a modifiable reference to the current item's value.
		/// 
		/// Same as getVal().
		/// 
		/// \sa getKey()

		TVal & operator * (
		) const {
			return concrete(i)->value;
		}

		/// \brief Returns a pointer to the current item's value.
		/// 
		/// \sa getVal()

		TVal * operator -> (
		) const {
			return &concrete(i)->value;
		}

		/// \brief Returns true iff \a other points to the same item as this
		///	Iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	Iterator &	o
		) const {
			return ( i == o.i );
		}

		/// \brief Returns true iff \a other points to a different item than this
		///	Iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	Iterator &	o
		) const {
			return ( i != o.i );
		}

		Bool operator == (
			const	CstIterator &	o
		) const {
			return ( i == reinterpret_cast<const Iterator &>(o).i );
		}

		Bool operator != (
			const	CstIterator &	o
		) const {
			return ( i != reinterpret_cast<const Iterator &>(o).i );
		}

		/// \brief (prefix) Advances the Iterator to the next item in the hash
		///	and returns an Iterator to the new current item.
		/// 
		/// Calling this function on Hash::end() leads to undefined results.
		/// 
		/// \sa operator--()

		Iterator & operator ++ (
		) {
			i = HashData::nextNode(i);
			return *this;
		}

		/// \brief (postfix) Advances the Iterator to the next item in
		///	the hash and returns an Iterator to the previously
		///	current item.

		Iterator operator ++ (
				int
		) {
			Iterator r = *this;
			i = HashData::nextNode(i);
			return r;
		}

		/// \brief (prefix) Makes the preceding item current and
		///	returns an Iterator pointing to the new current item.
		/// 
		/// Calling this function on Hash::begin() leads to undefined
		/// results.
		/// 
		/// \sa operator++()

		Iterator & operator -- (
		) {
			i = HashData::previousNode(i);
			return *this;
		}

		/// \brief (postfix) Makes the preceding item current and
		///	returns an Iterator pointing to the previously
		///	current item.

		Iterator operator -- (
				int
		) {
			Iterator r = *this;
			i = HashData::previousNode(i);
			return r;
		}

		/// \brief Returns an Iterator to the item at \a j positions
		///	forward from this Iterator.
		///
		/// If \a j is negative, the Iterator goes backward.
		/// 
		/// This operation can be slow for large \a j values.
		/// 
		/// \sa operator-()

		Iterator operator + (
				Int32		j
		) const {
			Iterator r = *this;
			if (j > 0)
				while (j--)
					++r;
			else
				while (j++)
					--r;
			return r;
		}

		/// \brief Returns an Iterator to the item at \a j positions
		///	backward from this Iterator.
		///
		/// If \a j is negative, the Iterator goes forward.
		/// 
		/// This operation can be slow for large \a j values.
		/// 
		/// \sa operator+()

		Iterator operator - (
			const	Int32		j
		) const {
			return operator + ( - j );
		}

		/// \brief Advances the Iterator by \a j items.
		///
		/// If \a j is negative, the Iterator goes backward.
		/// 
		/// \sa operator-=(), operator+()

		Iterator & operator += (
			const	Int32		j
		) {
			return ( *this = *this + j );
		}

		/// \brief Makes the Iterator go back by \a j items.
		///
		/// If \a j is negative, the Iterator goes forward.
		/// 

		/// \sa operator+=(), operator-()

		Iterator & operator -= (
			const	Int32		j
		) {
			return ( *this = *this - j );
		}

	private :

		HashData::Node *i;

	};

	friend class Iterator;

	/// \brief Provides an STL-style const iterator for Hash and MultiHash.
	/// 
	/// Hash features both STL-style iterators and Java-style
	/// iterators. The STL-style iterators are more low-level and more
	/// cumbersome to use; on the other hand, they are slightly faster
	/// and, for developers who already know STL, have the advantage of
	/// familiarity.
	/// 
	/// Hash\<TKey, TVal\>::CstIterator allows you to iterate over a
	/// Hash (or a MultiHash). If you want to modify the Hash as you
	/// iterate over it, you must use Hash::Iterator instead. It is
	/// generally good practice to use Hash::CstIterator on a
	/// non-const Hash as well, unless you need to change the Hash
	/// through the iterator. Const iterators are slightly faster, and
	/// can improve code readability.
	/// 
	/// The default Hash::CstIterator constructor creates an
	/// uninitialized iterator. You must initialize it using a Hash
	/// function like Hash::constBegin(), Hash::constEnd(), or
	/// Hash::find() before you can start iterating. Here's a typical
	/// loop that prints all the (key, value) pairs stored in a hash:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32> hash;
	/// 	hash.insert("January", 1);
	/// 	hash.insert("February", 2);
	/// 	...
	/// 	hash.insert("December", 12);
	/// 
	/// 	Hash<Buffer, Int32>::CstIterator i;
	/// 	for (i = hash.constBegin(); i != hash.constEnd(); ++i)
	/// 		cout << i.getKey() << ": " << i.getVal() << endl;
	/// \endcode
	/// 
	/// Multiple iterators can be used on the same hash. However, be aware
	/// that any modification performed directly on the Hash has the
	/// potential of dramatically changing the order in which the items
	/// are stored in the hash, as they might cause Hash to rehash its
	/// internal data structure. If you need to keep iterators over a long
	/// period of time, we recommend that you use QMap rather than Hash.
	/// 
	/// \sa Hash::Iterator

	class CstIterator {

	public :

		typedef TVal		ValType;
		typedef const TVal *	PtrType;
		typedef const TVal &	RefType;

		/// \brief Constructs an uninitialized iterator.
		/// 
		/// Functions like getKey(), getVal(), and operator++() must not be
		/// called on an uninitialized iterator. Use operator=() to assign a
		/// value to it before using it.
		/// 
		/// \sa Hash::constBegin() Hash::constEnd()

		CstIterator(
		) :
			i( 0 ) {
		}

		explicit CstIterator(
				void *		node
		) :
			i(reinterpret_cast<HashData::Node *>(node)) {
		}

		/// \brief Constructs a copy of \a other.

		CstIterator(
			const	Iterator &	o
		) {
			i = reinterpret_cast<const CstIterator &>(o).i;
		}

		/// \brief Constructs a copy of \a other.

		CstIterator(
			const	CstIterator &	o
		) {
			i = o.i;
		}

		/// \brief Returns the current item's key.
		/// 
		/// \sa getVal()

		const TKey & getKey(
		) const {
			return concrete(i)->key;
		}

		/// \brief Returns the current item's value.
		/// 
		/// \sa getKey(), operator*()

		const TVal & getVal(
		) const {
			return concrete(i)->value;
		}

		/// \brief Returns the current item's value.
		/// 
		/// Same as getVal().
		/// 
		/// \sa getKey()

		const TVal & operator * (
		) const {
			return concrete(i)->value;
		}

		/// \brief Returns a pointer to the current item's value.
		/// 
		/// \sa getVal()

		const TVal * operator -> (
		) const {
			return &concrete(i)->value;
		}

		/// \brief Returns true iff \a other points to the same item
		///	as this iterator.
		/// 
		/// \sa operator!=()

		Bool operator == (
			const	CstIterator &	o
		) const {
			return ( i == o.i );
		}

		/// \brief Returns true iff \a other points to a different
		///	item than this iterator.
		/// 
		/// \sa operator==()

		Bool operator != (
			const	CstIterator &	o
		) const {
			return ( i != o.i );
		}

		/// \brief (prefix) Advances the iterator to the next item in
		///	the hash and returns an iterator to the new current
		///	item.
		/// 
		/// Calling this function on Hash::end() leads to undefined results.
		/// 
		/// \sa operator--()

		CstIterator & operator ++ () {
			i = HashData::nextNode(i);
			return *this;
		}

		/// \brief (postfix) Advances the iterator to the next item in
		///	the hash and returns an iterator to the previously
		///	current item.

		CstIterator operator ++ (
				int
		) {
			CstIterator r = *this;
			i = HashData::nextNode(i);
			return r;
		}

		/// \brief (prefix) Makes the preceding item current and
		///	returns an iterator pointing to the new current item.
		/// 
		/// Calling this function on Hash::begin() leads to undefined
		/// results.
		/// 
		/// \sa operator++()

		CstIterator & operator -- (
		) {
			i = HashData::previousNode(i);
			return *this;
		}

		/// \brief (postfix) Makes the preceding item current and
		///	returns an iterator pointing to the previously
		///	current item.

		CstIterator operator -- (
				int
		) {
			CstIterator r = *this;
			i = HashData::previousNode(i);
			return r;
		}

		/// \brief Returns an iterator to the item at \a j positions
		///	forward from this iterator.
		///
		/// If \a j is negative, the iterator goes backward.
		/// 
		/// This operation can be slow for large \a j values.
		/// 
		/// \sa operator-()

		CstIterator operator + (
				Int32		j
		) const {
			CstIterator r = *this;
			if (j > 0)
				while (j--)
					++r;
			else
				while (j++)
					--r;
			return r;
		}

		/// \brief Returns an iterator to the item at \a j positions
		///	backward from this iterator.
		///
		/// If \a j is negative, the iterator goes forward.
		/// 
		/// This operation can be slow for large \a j values.
		/// 
		/// \sa operator+()

		CstIterator operator - (
				Int32		j
		) const {
			return operator+(-j);
		}

		/// \brief Advances the iterator by \a j items.
		///
		/// If \a j is negative, the iterator goes backward.
		/// 
		/// This operation can be slow for large \a j values.
		/// 
		/// \sa operator-=(), operator+()

		CstIterator & operator += (
				Int32		j
		) {
			return ( *this = *this + j );
		}

		/// \brief Makes the iterator go back by \a j items.
		///
		/// If \a j is negative, the iterator goes forward.
		/// 
		/// This operation can be slow for large \a j values.
		/// 
		/// \sa operator+=(), operator-()

		CstIterator & operator -= (
				Int32		j
		) {
			return ( *this = *this - j );
		}

	private :

		HashData::Node *i;

	};

	friend class CstIterator;

	/// \brief Returns an STL-style iterator pointing to the first item in
	///	the hash.
	/// 
	/// \sa constBegin(), end()

	Iterator begin(
	) {
		detach();
		return Iterator(d->firstNode());
	}

	/// \brief Returns a const STL-style iterator pointing to the first item in
	///	the hash.
	/// 
	/// \sa constBegin(), end()

	CstIterator begin(
	) const {
		return CstIterator(d->firstNode());
	}

	/// \brief Returns a const STL-style iterator pointing to the first item
	///	in the hash.
	/// 
	/// \sa begin(), constEnd()

	CstIterator constBegin(
	) const {
		return CstIterator(d->firstNode());
	}

	/// \brief Returns an STL-style iterator pointing to the imaginary item
	///	after the last item in the hash.
	/// 
	/// \sa begin(), constEnd()

	Iterator end() {
		detach();
		return Iterator(e);
	}

	/// \brief Returns a const STL-style iterator pointing to the imaginary item
	///	after the last item in the hash.
	/// 
	/// \sa begin(), constEnd()

	CstIterator end(
	) const {
		return CstIterator(e);
	}

	/// \brief Returns a const STL-style iterator pointing to the imaginary
	///	item after the last item in the hash.
	/// 
	/// \sa constBegin(), end()

	CstIterator constEnd(
	) const {
		return CstIterator(e);
	}

	/// \brief Removes the (key, value) pair associated with the iterator
	///	\a pos from the hash, and returns an iterator to the next item
	///	in the hash.
	/// 
	/// Unlike remove() and take(), this function never causes Hash to
	/// rehash its internal data structure. This means that it can safely
	/// be called while iterating, and won't affect the order of items in
	/// the hash. For example:
	/// 
	/// \code
	/// 	Hash<QObject *, Int32> objectHash;
	/// 	...
	/// 	Hash<QObject *, Int32>::Iterator i = objectHash.find(obj);
	/// 	while (i != objectHash.end() && i.getKey() == obj) {
	/// 		if (i.getVal() == 0) {
	/// 			i = objectHash.erase(i);
	/// 		} else {
	/// 			++i;
	/// 		}
	/// 	}
	/// \endcode
	/// 
	/// \sa remove(), take(), find()

	Iterator erase(
			Iterator	it
	);

	/// \brief Returns an iterator pointing to the item with key \a key in
	///	the hash.
	/// 
	/// If the hash contains no item with key \a key, the function
	/// returns end().
	/// 
	/// If the hash contains multiple items with key \a key, this
	/// function returns an iterator that points to the most recently
	/// inserted value. The other values are accessible by incrementing
	/// the iterator. For example, here's some code that iterates over all
	/// the items with the same key:
	/// 
	/// \code
	/// 	Hash<Buffer, Int32> hash;
	/// 	...
	/// 	Hash<Buffer, Int32>::CstIterator i = hash.find("HDR");
	/// 	while (i != hash.end() && i.getKey() == "HDR") {
	/// 		cout << i.getVal() << endl;
	/// 		++i;
	/// 	}
	/// \endcode
	/// 
	/// \sa getVal(), getVals()

	Iterator find(
		const	TKey &		key
	);

	CstIterator find(
		const	TKey &		key
	) const;

	/// \brief Returns an iterator pointing to the item with key \a key
	///	in the hash.
	/// 
	/// If the hash contains no item with key \a key, the function
	/// returns constEnd().
	/// 
	/// \sa find()

	CstIterator constFind(
		const	TKey &		key
	) const;

	/// \brief Inserts a new item with the key \a key and a value of \a value.
	/// 
	/// If there is already an item with the key \a key, that item's value
	/// is replaced with \a value.
	/// 
	/// If there are multiple items with the key \a key, the most
	/// recently inserted item's value is replaced with \a value.
	/// 
	/// \sa insertMulti()

	Iterator insert(
		const	TKey &		key,
		const	TVal &		value
	);

	/// \brief Inserts a new item with the key \a key and a value of \a value.
	/// 
	/// If there is already an item with the same key in the hash, this
	/// function will simply create a new one. (This behavior is
	/// different from insert(), which overwrites the value of an
	/// existing item.)
	/// 
	/// \sa insert(), getVals()

	Iterator insertMulti(
		const	TKey &		key,
		const	TVal &		value
	);

	/// \brief Inserts all the items in the \a other hash into this hash.
	///
	/// If a key is common to both hashes, the resulting hash will contain
	/// the key multiple times.
	/// 
	/// \sa insertMulti()

	Hash< TKey, TVal > & unite(
		const	Hash< TKey, TVal > &	other
	);

private :

	typedef HashNode< TKey, TVal > Node;

	union {
		HashData *		d;
		Node *			e;
	};

	static Node * concrete(
			HashData::Node *	node
	) {
		return reinterpret_cast< Node * >( node );
	}

private :

	void detach_helper(
	);

	void freeData(
			HashData *	d
	);

	Node ** findNode(
		const	TKey &		key,
			Uint32 *	hp = 0
	) const;

	Node * createNode(
			Uint32		h,
		const	TKey &		key,
		const	TVal &		value,
			Node **		nextNode
	);

	void deleteNode(
			Node *		node
	);

	static void duplicateNode(
			HashData::Node *	originalNode,
			void *		newNode
	);

};

// ============================================================================

template < class TKey, class TVal >
inline void Hash< TKey, TVal >::deleteNode(Node *node)
{
	node->~Node();
	d->freeNode(node);
}

template < class TKey, class TVal >
inline void Hash< TKey, TVal >::duplicateNode(HashData::Node *node, void *newNode)
{
	Node *concreteNode = concrete(node);
	(void) new (newNode) Node(concreteNode->key, concreteNode->value);
}

template < class TKey, class TVal >
inline typename Hash< TKey, TVal >::Node *
Hash< TKey, TVal >::createNode(Uint32 ah, const TKey &pKey, const TVal &pVal, Node **anextNode)
{
	Node *node;

	node = ::new (d->allocateNode()) Node(pKey, pVal);

	node->h = ah;
	node->next = *anextNode;
	*anextNode = node;
	++d->size;
	return node;
}

template < class TKey, class TVal >
inline Hash< TKey, TVal > &Hash< TKey, TVal >::unite(const Hash< TKey, TVal > &other)
{
	Hash< TKey, TVal > copy(other);
	CstIterator it = copy.constEnd();
	while (it != copy.constBegin()) {
		--it;
		insertMulti(it.getKey(), it.getVal());
	}
	return *this;
}

template < class TKey, class TVal >
void Hash< TKey, TVal >::freeData(HashData *x)
{
	Node *e_for_x = reinterpret_cast<Node *>(x);
	Node **bucket = reinterpret_cast<Node **>(x->buckets);
	Int32 n = x->numBuckets;
	while (n--) {
		Node *cur = *bucket++;
		while (cur != e_for_x) {
			Node *next = cur->next;
			deleteNode(cur);
			cur = next;
		}
	}
	x->destroyAndFree();
}

template < class TKey, class TVal >
inline void Hash< TKey, TVal >::clear()
{
	*this = Hash<TKey,TVal>();
}

template < class TKey, class TVal >
void Hash< TKey, TVal >::detach_helper()
{
	HashData * newData = d->detach_helper( duplicateNode, sizeof( Node ) );
	HashData * oldData = d;
	d = newData;
	if ( ! --oldData->refCount ) {
		freeData(oldData);
	}
}

template < class TKey, class TVal >
inline Hash< TKey, TVal > &Hash< TKey, TVal >::operator=(const Hash< TKey, TVal > &other)
{
	if (d != other.d) {
		HashData *x = other.d;
		x->refCount++;
		HashData * t = d;
		d = x;
		x = t;
		if ( ! --x->refCount ) {
			freeData(x);
		}
	}
	return *this;
}

template < class TKey, class TVal >
inline const TVal Hash< TKey, TVal >::getVal(const TKey &pKey) const
{
	Node *node = *findNode(pKey);
	if (node == e) {
		return TVal();
	} else {
		return node->value;
	}
}

template < class TKey, class TVal >
inline const TVal Hash< TKey, TVal >::getVal(const TKey &pKey, const TVal &adefaultValue) const
{
	Node *node = *findNode(pKey);
	if (node == e) {
		return adefaultValue;
	} else {
		return node->value;
	}
}

template < class TKey, class TVal >
List< TKey > Hash< TKey, TVal >::getUniqueKeys() const
{
	List< TKey > res;
	CstIterator i = begin();
	if (i != end()) {
		for (;;) {
			const TKey &aKey = i.getKey();
			res.append(aKey);
			do {
				if (++i == end())
					goto break_out_of_outer_loop;
			} while (aKey == i.getKey());
		}
	}
break_out_of_outer_loop:
	return res;
}

template < class TKey, class TVal >
List< TKey > Hash< TKey, TVal >::getKeys() const
{
	List< TKey > res;
	CstIterator i = begin();
	while (i != end()) {
		res.append(i.getKey());
		++i;
	}
	return res;
}

template < class TKey, class TVal >
List< TKey > Hash< TKey, TVal >::getKeys(const TVal &pVal) const
{
	List< TKey > res;
	CstIterator i = begin();
	while (i != end()) {
		if (i.getVal() == pVal)
			res.append(i.getKey());
		++i;
	}
	return res;
}

template < class TKey, class TVal >
const TKey Hash< TKey, TVal >::getKey(const TVal &pVal) const
{
	CstIterator i = begin();
	while (i != end()) {
		if (i.getVal() == pVal)
			return i.getKey();
		++i;
	}

	return TKey();
}

template < class TKey, class TVal >
List< TVal > Hash< TKey, TVal >::getVals() const
{
	List< TVal > res;
	CstIterator i = begin();
	while (i != end()) {
		res.append(i.getVal());
		++i;
	}
	return res;
}

template < class TKey, class TVal >
List< TVal > Hash< TKey, TVal >::getVals(const TKey &pKey) const
{
	List< TVal > res;
	Node *node = *findNode(pKey);
	if (node != e) {
		do {
			res.append(node->value);
		} while ((node = node->next) != e && node->key == pKey);
	}
	return res;
}

template < class TKey, class TVal >
Int32 Hash< TKey, TVal >::getCount(const TKey &pKey) const
{
	Int32 cnt = 0;
	Node *node = *findNode(pKey);
	if (node != e) {
		do {
			++cnt;
		} while ((node = node->next) != e && node->key == pKey);
	}
	return cnt;
}

template < class TKey, class TVal >
inline const TVal Hash< TKey, TVal >::operator[](const TKey &pKey) const
{
	return getVal(pKey);
}

template < class TKey, class TVal >
inline TVal &Hash< TKey, TVal >::operator[](const TKey &pKey)
{
	detach();
	d->mightGrow();

	Uint32 h;
	Node **node = findNode(pKey, &h);
	if (*node == e)
		return createNode(h, pKey, TVal(), node)->value;
	return (*node)->value;
}

template < class TKey, class TVal >
inline typename Hash< TKey, TVal >::Iterator Hash< TKey, TVal >::insert(const TKey &pKey,
																		 const TVal &pVal)
{
	detach();
	d->mightGrow();

	Uint32 h;
	Node **node = findNode(pKey, &h);
	if (*node == e)
		return Iterator(createNode(h, pKey, pVal, node));

	(*node)->value = pVal;
	return Iterator(*node);
}

template < class TKey, class TVal >
inline typename Hash< TKey, TVal >::Iterator Hash< TKey, TVal >::insertMulti(const TKey &pKey,
																			  const TVal &pVal)
{
	detach();
	d->mightGrow();

	Uint32 h;
	Node **nextNode = findNode(pKey, &h);
	return Iterator(createNode(h, pKey, pVal, nextNode));
}

template < class TKey, class TVal >
Int32 Hash< TKey, TVal >::remove(const TKey &pKey)
{
	detach();

	Int32 oldSize = d->size;
	Node **node = findNode(pKey);
	if (*node != e) {
		Bool deleteNext = true;
		do {
			Node *next = (*node)->next;
			deleteNext = (next != e && next->key == (*node)->key);
			deleteNode(*node);
			*node = next;
			--d->size;
		} while (deleteNext);
		d->hasShrunk();
	}
	return oldSize - d->size;
}

template < class TKey, class TVal >
TVal Hash< TKey, TVal >::take(const TKey &pKey)
{
	detach();

	Node **node = findNode(pKey);
	if (*node != e) {
		TVal t = (*node)->value;
		Node *next = (*node)->next;
		deleteNode(*node);
		*node = next;
		--d->size;
		d->hasShrunk();
		return t;
	}
	return TVal();
}

template < class TKey, class TVal >
typename Hash< TKey, TVal >::Iterator Hash< TKey, TVal >::erase(Iterator it)
{
	if (it == Iterator(e))
		return it;

	Iterator ret = it;
	++ret;

	Node *node = it;
	Node **node_ptr = reinterpret_cast<Node **>(&d->buckets[node->h % d->numBuckets]);
	while (*node_ptr != node)
		node_ptr = &(*node_ptr)->next;
	*node_ptr = node->next;
	deleteNode(node);
	--d->size;
	return ret;
}

template < class TKey, class TVal >
inline void Hash< TKey, TVal >::reserve(Int32 asize)
{
	detach();
	d->rehash( - Math::getMax< Int32 >( asize, 1 ) );
}

template < class TKey, class TVal >
inline typename Hash< TKey, TVal >::CstIterator Hash< TKey, TVal >::find(const TKey &pKey) const
{
	return CstIterator(*findNode(pKey));
}

template < class TKey, class TVal >
inline typename Hash< TKey, TVal >::CstIterator Hash< TKey, TVal >::constFind(const TKey &pKey) const
{
	return CstIterator(*findNode(pKey));
}

template < class TKey, class TVal >
inline typename Hash< TKey, TVal >::Iterator Hash< TKey, TVal >::find(const TKey &pKey)
{
	detach();
	return Iterator(*findNode(pKey));
}

template < class TKey, class TVal >
inline Bool Hash< TKey, TVal >::contains(const TKey &pKey) const
{
	return *findNode(pKey) != e;
}

template < class TKey, class TVal >
typename Hash< TKey, TVal >::Node **Hash< TKey, TVal >::findNode(
	const	TKey &		pKey,
		Uint32 *	ahp ) const {

	Node **	node;
	Uint32	h = getHash( pKey );

	if (d->numBuckets) {
		node = reinterpret_cast<Node **>(&d->buckets[h % d->numBuckets]);
//		Q_ASSERT(*node == e || (*node)->next);
		while (*node != e && !(*node)->same_key(h, pKey))
			node = &(*node)->next;
	} else {
		node = const_cast<Node **>(reinterpret_cast<const Node * const *>(&e));
	}
	if (ahp)
		*ahp = h;

	return node;

}

template < class TKey, class TVal >
Bool Hash< TKey, TVal >::operator==(const Hash< TKey, TVal > &other) const
{
	if (getSize() != other.getSize())
		return false;
	if (d == other.d)
		return true;

	CstIterator it = begin();

	while (it != end()) {
		const TKey &pKey = it.getKey();

		CstIterator it2 = other.find(pKey);
		do {
			if (it2 == other.end() || !(it2.getKey() == pKey))
				return false;
			if ( !(it.getVal() == it2.getVal()))
				return false;
			++it;
			++it2;
		} while (it != end() && it.getKey() == pKey);
	}
	return true;
}

// ============================================================================

/*! \class MultiHash
	\brief The MultiHash class is a convenience Hash subclass that provides multi-valued hashes.

	\ingroup tools
	\ingroup shared
	\mainclass
	\reentrant

	MultiHash\<TKey, TVal\> is one of Qt's generic \l{container classes}.
	It inherits Hash and extends it with a few convenience functions
	that make it more suitable than Hash for storing multi-valued
	hashes. A multi-valued hash is a hash that allows multiple values
	with the same key; Hash normally doesn't allow that, unless you
	call Hash::insertMulti().

	Because MultiHash inherits Hash, all of Hash's functionality also
	applies to MultiHash. For example, you can use isEmpty() to test
	whether the hash is empty, and you can traverse a MultiHash using
	Hash's iterator classes (for example, HashIterator). But in
	addition, it provides an insert() function that corresponds to
	Hash::insertMulti(), and a replace() function that corresponds to
	Hash::insert(). It also provides convenient operator+() and
	operator+=().

	Example:
	\code
		MultiHash<Buffer, Int32> hash1, hash2, hash3;

		hash1.insert("plenty", 100);
		hash1.insert("plenty", 2000);
		// hash1.getSize() == 2

		hash2.insert("plenty", 5000);
		// hash2.getSize() == 1

		hash3 = hash1 + hash2;
		// hash3.getSize() == 3
	\endcode

	Unlike Hash, MultiHash provides no operator[]. Use getVal() or
	replace() if you want to access the most recently inserted item
	with a certain key.

	If you want to retrieve all the values for a single key, you can
	use getVals(const TKey &key), which returns a List< TVal >:

	\code
		List<Int32> values = hash.getVals("plenty");
		for (Int32 i = 0; i < values.getSize(); ++i)
			cout << values.at(i) << endl;
	\endcode

	The items that share the same key are available from most
	recently to least recently inserted.

	A more efficient approach is to call find() to get
	the STL-style iterator for the first item with a key and iterate from
	there:

	\code
		MultiHash<Buffer, Int32>::Iterator i = hash.find("plenty");
		while (i != hash.end() && i.getKey() == "plenty") {
			cout << i.getVal() << endl;
			++i;
		}
	\endcode

	MultiHash's key and value data types must be \l{assignable data
	types}. You cannot, for example, store a QWidget as a value;
	instead, store a QWidget *. In addition, MultiHash's key type
	must provide operator==(), and there must also be a global
	getHash() function that returns a hash value for an argument of the
	key's type. See the Hash documentation for details.

	\sa Hash, HashIterator, QMutableHashIterator, QMultiMap
*/

template < class TKey, class TVal >
class MultiHash : public Hash< TKey, TVal >
{
public :
/*! \fn MultiHash::MultiHash()

	Constructs an empty hash.
*/

	MultiHash() {}
/*! \fn MultiHash::MultiHash(const Hash< TKey, TVal > &other)

	Constructs a copy of \a other (which can be a Hash or a
	MultiHash).

	\sa operator=()
*/

	MultiHash(const Hash< TKey, TVal > &other) : Hash< TKey, TVal >(other) {}

/*! \fn MultiHash::Iterator MultiHash::replace(const TKey &key, const TVal &value)

	Inserts a new item with the key \a key and a value of \a value.

	If there is already an item with the key \a key, that item's value
	is replaced with \a value.

	If there are multiple items with the key \a key, the most
	recently inserted item's value is replaced with \a value.

	\sa insert()
*/

	inline typename Hash< TKey, TVal >::Iterator replace(const TKey &key, const TVal &value) {
		return Hash< TKey, TVal >::insert(key, value);
	}
/*! \fn MultiHash::Iterator MultiHash::insert(const TKey &key, const TVal &value)

	Inserts a new item with the key \a key and a value of \a value.

	If there is already an item with the same key in the hash, this
	function will simply create a new one. (This behavior is
	different from replace(), which overwrites the value of an
	existing item.)

	\sa replace()
*/

	inline typename Hash< TKey, TVal >::Iterator insert(const TKey &key, const TVal &value) {
		return Hash< TKey, TVal >::insertMulti(key, value);
	}

/*! \fn MultiHash &MultiHash::operator+=(const MultiHash &other)

	Inserts all the items in the \a other hash into this hash
	and returns a reference to this hash.

	\sa insert()
*/

	inline MultiHash &operator+=(const MultiHash &other)
	{ unite(other); return *this; }
/*! \fn MultiHash MultiHash::operator+(const MultiHash &other) const

	Returns a hash that contains all the items in this hash in
	addition to all the items in \a other. If a key is common to both
	hashes, the resulting hash will contain the key multiple times.

	\sa operator+=()
*/

	inline MultiHash operator+(const MultiHash &other) const
	{ MultiHash result = *this; result += other; return result; }

private:
	TVal &operator[](const TKey &key);
	const TVal operator[](const TKey &key) const;
};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_Hash_H_

// ============================================================================

