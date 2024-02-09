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
//	BFC.Crypto.HMAC.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_HMAC_H_
#define _BFC_Crypto_HMAC_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, HMAC )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Hash.h"
#include "BFC.Crypto.MAC.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Hash-based Message Authentication Code
///
/// In cryptography, HMAC (Hash-based Message Authentication Code), is a
/// specific construction for calculating a message authentication code (MAC)
/// involving a cryptographic hash function in combination with a secret key.
/// As with any MAC, it may be used to simultaneously verify both the data
/// integrity and the authenticity of a message. Any iterative cryptographic
/// hash function, such as MD5 or SHA-1, may be used in the calculation of an
/// HMAC; the resulting MAC algorithm is termed HMAC-MD5 or HMAC-SHA1
/// accordingly. The cryptographic strength of the HMAC depends upon the
/// cryptographic strength of the underlying hash function, the size of its
/// hash output length in bits and on the size and quality of the
/// cryptographic key.
///
/// An iterative hash function breaks up a message into blocks of a fixed size
/// and iterates over them with a compression function. For example, MD5 and
/// SHA-1 operate on 512-bit blocks. The size of the output of HMAC is the
/// same as that of the underlying hash function (128 or 160 bits in the case
/// of MD5 or SHA-1, respectively), although it can be truncated if desired.
///
/// The definition and analysis of the HMAC construction was first published
/// in 1996 by Mihir Bellare, Ran Canetti, and Hugo Krawczyk, who also wrote
/// RFC 2104. This paper also defined a variant called NMAC that is rarely if
/// ever used. FIPS PUB 198 generalizes and standardizes the use of HMACs.
/// HMAC-SHA-1 and HMAC-MD5 are used within the IPsec and TLS protocols.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL HMAC : public MAC {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new HMAC.

	HMAC(
	);

	/// \brief Initializes this object.
	///
	/// \param pHash
	///	The hash you want to use.
	///
	/// \param pKey
	///	The secret key.

	void init(
			HashPtr		pHash,
		const	Buffer &	pKey
	);

	/// \brief Processes data through HMAC.
	///
	/// \param pData
	///	The data to send through HMAC.

	virtual void process(
		const	Buffer &	pData
	);

	/// \brief Terminates an HMAC session.
	///
	/// \param pTag
	///	The destination of the HMAC authentication tag.

	virtual void done(
			Buffer &	pTag
	);

	/// \brief Performs HMAC self-test.

	virtual void test(
	);

protected :

private :

	HashPtr		usrHash;
	Uint32		hshSize;	///< Hash size.
	Uint32		blkSize;	///< Block size.
	Buffer		key;

	/// \brief Forbidden copy constructor.

	HMAC(
		const	HMAC &
	);

	/// \brief Forbidden copy operator.

	HMAC & operator = (
		const	HMAC &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_HMAC_H_

// ============================================================================

