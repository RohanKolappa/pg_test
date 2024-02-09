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
//	BFC.Crypto.Hash.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Hash_H_
#define _BFC_Crypto_Hash_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Crypto.HashPtr.h"

// ============================================================================

#include "BFC.Crypto.HashDescriptor.h"
	
#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Represents a cryptographic hash function.
///
/// A cryptographic hash function is a deterministic procedure that takes an
/// arbitrary block of data and returns a fixed-size bit string, the
/// <b>(cryptographic) hash value</b>, such that an accidental or intentional
/// change to the data will change the hash value. The data to be encoded is
/// often called the "message", and the hash value is sometimes called the
/// <b>message digest</b> or simply \b digest.
///
/// The ideal cryptographic hash function has four main or significant
/// properties:
/// -	it is easy to compute the hash value for any given message,
/// -	it is infeasible to find a message that has a given hash,
/// -	it is infeasible to modify a message without changing its hash,
/// -	it is infeasible to find two different messages with the same hash.
///
/// Cryptographic hash functions have many information security applications,
/// notably in digital signatures, message authentication codes (MACs), and
/// other forms of authentication. They can also be used as ordinary hash
/// functions, to index data in hash tables, for fingerprinting, to detect
/// duplicate data or uniquely identify files, and as checksums to detect
/// accidental data corruption. Indeed, in information security contexts,
/// cryptographic hash values are sometimes called <b>(digital)
/// fingerprints</b>, \b checksums, or just <b>hash values</b>, even though all
/// these terms stand for functions with rather different properties and
/// purposes.
///
/// \sa http://en.wikipedia.org/wiki/Cryptographic_hash_function
///
/// Example
///
/// \code
///	using namespace BFC;
///	// Create a new Hash.
///	Crypto::HashPtr hash = ...;
///	// Initialize it before use.
///	hash->init();
///	// Process any amount of data.
///	hash->process( ... );
///	...
///	hash->process( ... );
///	// Generate the digest.
///	Buffer res;
///	hash->done( res );
///	// 'res' contains the digest now.
/// \endcode
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Hash : public DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new Hash.

	Hash(
		const	DL::TypeCPtr &	pType
	) :
		DL::Object	( pType ),
		descr		( pType->getData() ) {
	}

public :

	/// \brief Returns the HashDescriptor associated to this Hash.

	HashDescriptorCPtr getDescr(
	) const {
		return descr;
	}

	/// \brief Returns the digest output size (in bytes).

	Uint32 getHashSize(
	) const {
		return descr->getHashSize();
	}

	/// \brief Returns the block size the hash uses internally (in bytes).

	Uint32 getBlockSize(
	) const {
		return descr->getBlockSize();
	}

	/// \brief Initializes this Hash.
	///
	/// This simply sets up the hash to the default state governed by the
	/// specifications of the hash.

	virtual void init(
	) = 0;

	/// \brief Processes a chunk of data.
	///
	/// The data can be passed in any sized chunks. If the order of the
	/// bytes is the same, then the message digest (hash output) will be
	/// the same. For example, this means that:
	/// \code
	///	hash->process( "hello, " );
	///	hash->process( "world" );
	/// \endcode
	/// Will produce the same message digest as the single call:
	/// \code
	///	hash->process( "hello, world" );
	/// \endcode

	virtual void process(
		const	Buffer &	pData
	) = 0;

	/// \brief Produces the digest and stores it in \a pOutput.

	virtual void done(
			Buffer &	pOutput
	) = 0;

	/// \brief Self-test.

	virtual void test(
	) = 0;

	/// \brief Hashes a block of memory and returns the digest.
	///
	/// \param pIBuffer
	///	[in] The data you wish to hash.
	///
	/// \param pOBuffer
	///	[out] Where to store the digest.

	void hashMemory(
		const	Buffer &	pIBuffer,
			Buffer &	pOBuffer
	);

	Buffer hashMemory(
		const	Buffer &	pIBuffer
	);

private :

	HashDescriptorCPtr	descr;	///< Hash descriptor.

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Hash_H_

// ============================================================================

