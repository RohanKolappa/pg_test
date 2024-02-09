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
//	BFC.Crypto.Cipher.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Cipher_H_
#define _BFC_Crypto_Cipher_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Crypto.CipherPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.Crypto.CipherDescriptor.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Represents a crptographic block cipher.
///
/// In cryptography, a block cipher is a symmetric key cipher operating on
/// fixed-length groups of bits, called blocks, with an unvarying
/// transformation. A block cipher encryption algorithm might take (for
/// example) a 128-bit block of plaintext as input, and output a corresponding
/// 128-bit block of ciphertext. The exact transformation is controlled using a
/// second input — the secret key. Decryption is similar: the decryption
/// algorithm takes, in this example, a 128-bit block of ciphertext together
/// with the secret key, and yields the original 128-bit block of plaintext.
///
/// To encrypt messages longer than the block size (128 bits in the above
/// example), a mode of operation is used.
///
/// Block ciphers can be contrasted with stream ciphers; a stream cipher
/// operates on individual digits one at a time, and the transformation varies
/// during the encryption. The distinction between the two types is not always
/// clear-cut: a block cipher, when used in certain modes of operation, acts
/// effectively as a stream cipher.
///
/// An early and highly influential block cipher design was the Data Encryption
/// Standard (DES), developed at IBM and published as a standard in 1977. A
/// successor to DES, the Advanced Encryption Standard (AES), was adopted in
/// 2001.
///
/// \sa http://en.wikipedia.org/wiki/Block_cipher
///
/// BFC::Crypto provides several block ciphers with an ECB block mode
/// interface. It is important to note that <b>you should never use the
/// ECB modes directly to encrypt data</b>. Instead you should use the ECB
/// functions to make a chaining mode, or use one of the provided chaining
/// modes.
///
/// Example that encodes a block with Blowfish in ECB mode (don't use this in
/// your code directly - see above):
/// \code
///	#include "BFC.Crypto.Blowfish.h"
///	using namespace BFC;
///	...
///	// Create a key.
///	Buffer key = ...; // must be 8 bytes for Blowfish!
///	// Create the Cipher.
///	Crypto::CipherPtr cipher = new Crypto::Blowfish;
///	cipher->init( key );
///	// Create some plaintext.
///	Buffer pt = ...; // must be 8 bytes too!
///	// Encrypt the block.
///	Buffer ct = cipher->encrypt( pt );
///	// Now ct holds the encrypted version of pt.
///	// Decrypt the block.
///	Buffer pt2 = cipher->decrypt( ct );
///	// Now pt2 is equal to pt.
/// \endcode
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Cipher : public DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new Cipher.

	Cipher(
		const	DL::TypeCPtr &	pType
	) :
		DL::Object	( pType ),
		descr		( pType->getData() ) {
	}

public :

	/// \brief Returns the CipherDescriptor associated to this Cipher.

	CipherDescriptorCPtr getDescr(
	) const {
		return descr;
	}

	/// \brief Returns the minimum key size of this Cipher.

	Uint32 getMinKeySize(
	) const {
		return descr->getMinKeySize();
	}

	/// \brief Returns the maximum key size of this Cipher.

	Uint32 getMaxKeySize(
	) const {
		return descr->getMaxKeySize();
	}

	/// \brief Returns the block size used internally by this Cipher.

	Uint32 getBlockSize(
	) const {
		return descr->getBlockSize();
	}

	/// \brief Determines a key size.
	///
	/// \param pKeySize
	///	The size of the key desired.
	///
	/// \returns
	///	The suggested key size.
	///
	/// \throws InvalidArgument
	///	If \a pKeySize is smaller than the smallest
	///	supported key size.

	virtual Uint32 getKeySize(
		const	Uint32		pKeySize
	) const;

	/// \brief Initializes this Cipher with the symmetric key \a pKey.
	///
	/// \param pKey
	///	The input symmetric key.

	virtual void setKey(
		const	Buffer &	pKey
	) = 0;

	/// \brief Encrypts a block of text.
	///
	/// \param pPlainText
	///	The plaintext.
	///
	/// \returns
	///	The ciphertext.

	Buffer encrypt(
		const	Buffer &	pPlainText
	);

	/// \brief Decrypts a block of text.
	///
	/// \param pCipherText.
	///
	/// \returns
	///	The plaintext.

	Buffer decrypt(
		const	Buffer &	pCipherText
	);

	/// \brief Encrypts a block of text.
	///
	/// \param pPlainText
	///	[in] The plaintext.
	///
	/// \param pCipherText
	///	[out] The ciphertext.

	virtual void encrypt(
		const	Buffer &	pPlainText,
			Buffer &	pCipherText
	) = 0;

	/// \brief Decrypts a block of text.
	///
	/// \param pCipherText
	///	[in] The ciphertext.
	///
	/// \param pPlainText
	///	[out] The plaintext.

	virtual void decrypt(
		const	Buffer &	pCipherText,
			Buffer &	pPlainText
	) = 0;

	/// \brief Self-test.

	virtual void test(
	) = 0;

private :

	CipherDescriptorCPtr	descr;	///< Cipher descriptor.

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Cipher_H_

// ============================================================================

