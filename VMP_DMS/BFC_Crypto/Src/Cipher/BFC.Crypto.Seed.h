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
//	BFC.Crypto.Seed.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_Seed_H_
#define _BFC_Crypto_Seed_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Seed )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Cipher.h"
#include "BFC.Crypto.Functions.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Seed : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Seed.

	Seed(
	);

	virtual Uint32 getKeySize(
		const	Uint32		pKeySize
	) const;

	virtual void setKey(
		const	Buffer &	pKey
	);

	virtual void encrypt(
		const	Buffer &	pPlainText,
			Buffer &	pCipherText
	);

	virtual void decrypt(
		const	Buffer &	pCipherText,
			Buffer &	pPlainText
	);

	virtual void test(
	);

protected :

	static Uint32 G(
		const	Uint32		x
	) {
		return (SS3[((x)>>24)&255] ^ SS2[((x)>>16)&255] ^ SS1[((x)>>8)&255] ^ SS0[(x)&255]);
	}

	static void F(
			Uint32 &	L1,
			Uint32 &	L2,
		const	Uint32		R1,
		const	Uint32		R2,
		const	Uint32		K1,
		const	Uint32		K2
	) {
		Uint32 T2 = G((R1 ^ K1) ^ (R2 ^ K2));
		Uint32 T = G( G(T2 + (R1 ^ K1)) + T2);
		L2 ^= T;
		L1 ^= (T + G(T2 + (R1 ^ K1)));
	}

	static void rounds(Uint32 *P, Uint32 *K);

private :

	static const Uint32 SS0[ 256 ];
	static const Uint32 SS1[ 256 ];
	static const Uint32 SS2[ 256 ];
	static const Uint32 SS3[ 256 ];

	static const Uint32 KCi[ 16 ];

	Uint32 K[32], dK[32];

	/// \brief Forbidden copy constructor.

	Seed(
		const	Seed &
	);

	/// \brief Forbidden copy operator.

	Seed & operator = (
		const	Seed &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Seed_H_

// ============================================================================

