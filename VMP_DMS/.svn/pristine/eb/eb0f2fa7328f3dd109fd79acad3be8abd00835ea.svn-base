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
//	BFC.Crypto.MAC.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_MAC_H_
#define _BFC_Crypto_MAC_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Crypto.MACPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Descr.h"
#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Represents a Message Authentication Code.
///
/// In cryptography, a message authentication code (often MAC) is a short piece
///	of information used to authenticate a message.
///
/// A MAC algorithm, sometimes called a keyed (cryptographic) hash function,
/// accepts as input a secret key and an arbitrary-length message to be
/// authenticated, and outputs a MAC (sometimes known as a tag). The MAC value
/// protects both a message's data integrity as well as its authenticity, by
/// allowing verifiers (who also possess the secret key) to detect any changes
/// to the message content.
///
/// MAC algorithms are designed to produce matching MACs only if the same
/// message, secret key and initialization vector are input to the same
/// algorithm.
///
/// While MAC functions are similar to cryptographic hash functions, they
/// possess different security requirements. To be considered secure, a MAC
/// function must resist existential forgery under chosen-plaintext attacks.
/// This means that even if an attacker has access to an oracle which possesses
/// the secret key and generates MACs for messages of the attacker's choosing,
/// the attacker cannot guess the MAC for other messages without performing
/// unfeasible amounts of computation.
///
/// MACs differ from digital signatures as MAC values are both generated and
/// verified using the same secret key. This implies that the sender and
/// receiver of a message must agree on the same key before initiating
/// communications, as is the case with symmetric encryption. For the same
/// reason, MACs do not provide the property of non-repudiation offered by
/// signatures: any user who can verify a MAC is also capable of generating
/// MACs for other messages. In contrast, a digital signature is generated
/// using the private key of a key pair, which is asymmetric encryption. Since
/// this private key is only accessible to its holder, a digital signature
/// proves that a document was signed by none other than that holder. Thus,
/// digital signatures do offer non-repudiation.
///
/// MAC algorithms can be constructed from other cryptographic primitives, such
/// as cryptographic hash functions (as in the case of HMAC) or from block
/// cipher algorithms (OMAC, CBC-MAC and PMAC). However many of the fastest MAC
/// algorithms are constructed based on universal hashing.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL MAC : public DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new MAC.

	MAC(
		const	DL::TypeCPtr &	pType
	) :
		DL::Object	( pType ),
		descr		( pType->getData() ) {
	}

public :

	/// \brief Returns the Descr associated to this MAC.

	DL::DescrCPtr getDescr(
	) const {
		return descr;
	}

	/// \brief Processes data through MAC.
	///
	/// \param pData
	///	The data to send through MAC.

	virtual void process(
		const	Buffer &	pData
	) = 0;

	/// \brief Terminates a MAC session.
	///
	/// \param pTag
	///	The destination of the MAC authentication tag.

	virtual void done(
			Buffer &	pTag
	) = 0;

	/// \brief Performs MAC self-test.

	virtual void test(
	) = 0;

private :

	DL::DescrCPtr	descr;	///< MAC descriptor.

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_MAC_H_

// ============================================================================

