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
//	BFC.Crypto.Mode.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Mode_H_
#define _BFC_Crypto_Mode_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Crypto.ModePtr.h"

// ============================================================================

#include "BFC.Crypto.Cipher.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Descr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Represents a mode of operation.
///
/// In cryptography, block ciphers operate on blocks of fixed length. However,
/// messages may be of any length and therefore must be broken into blocks
/// before encryption. But encrypting the same block under the same key
/// produces the same output and encrypting each block in isolation is
/// inadequate for confidentiality, as discussed in the ECB section below. As
/// a result, modes of operation were invented to address this problem.
///
/// The earliest modes described in literature (ECB, CBC, OFB, and CFB)
/// provide only confidentiality; for message integrity, a separate mechanism
/// (e.g., CBC-MAC) had to be employed. Many recent modes attempt to address
/// both confidentiality and integrity protection, some of which are
/// particularly efficient in that they require only one pass over the data.
/// Examples of such combined modes are CCM, EAX, GCM, IAPM, and OCB.
/// Tweakable narrow-block encryption (LRW) and wide-block encryption (CMC and
/// EME) modes, which securely encrypt disk sectors, are described elsewhere.
///
/// \sa http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation
///
/// Example
///
/// \code
///	#include "BFC.Crypto.Cipher.h"
///	#include "BFC.Crypto.Mode.h"
///	...
///	using namespace BFC;
///	...
///	// Create some Cipher.
///	Crypto::CipherPtr cipher = ...;
///	Buffer	key = ...; // Length must be compatible with cipher.
///	Buffer	iv = ...; // Length must be cipher->getBlockSize().
///	// Create some Mode.
///	Crypto::ModePtr mode = ...;
///	mode->init( cipher, iv, key );
///	// Encrypt data.
///	Buffer res;
///	res = mode->encrypt( ... );
///	...
///	res = mode->encrypt( ... );
///	...
///	// Decrypt data.
///	mode->setiv( iv );
///	res = mode->decrypt( ... );
///	...
///	res = mode->decrypt( ... );
/// \endcode
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Mode : public DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new Mode.

	Mode(
		const	DL::TypeCPtr &	pType
	) :
		DL::Object	( pType ),
		descr		( pType->getData() ) {
	}

public :

	/// \brief Initializes this Mode.
	///
	/// \param pCipher
	///	The Cipher to use.
	///
	/// \param pIV
	///	The initial vector.
	///
	/// \param pKey
	///	The secret key.

	virtual void init(
			CipherPtr	pCipher,
		const	Buffer &	pIV,
		const	Buffer &	pKey
	) = 0;

	/// \brief Finalizes this Mode.

	virtual void done(
	) = 0;

	/// \brief Encrypts some plaintext.
	///
	/// \param pPlainText
	///	The plaintext to encrypt.
	///
	/// \returns
	///	The ciphertext.

	virtual Buffer encrypt(
		const	Buffer &	pPlainText
	) = 0;

	/// \brief Decrypts some ciphertext.
	///
	/// \param pCipherText
	///	The ciphertext to decrypt.
	///
	/// \returns
	///	The plaintext.

	virtual Buffer decrypt(
		const	Buffer &	pCipherText
	) = 0;

	/// \brief Gets the current initial vector.
	///
	/// \returns
	///	The current initial vector.

	virtual Buffer getiv(
	) const = 0;

	/// \brief Sets the initial vector.
	///
	/// \param pIV
	///	The initial vector.

	virtual void setiv(
		const	Buffer &	pIV
	) = 0;

	/// \brief Self-test.

	virtual void test(
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Crypto.Mode" );

	CLASS_EXCEPTION( NotInitialized, "Not initialized" );

protected :

private :

	DL::DescrCPtr	descr;

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Mode_H_

// ============================================================================

