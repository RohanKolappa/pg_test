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
//	BFC.Crypto.Noekeon.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_Noekeon_H_
#define _BFC_Crypto_Noekeon_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Noekeon )

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

class BFC_Crypto_DLL Noekeon : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Noekeon.

	Noekeon(
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

	static void kTHETA(
			Uint32 &	a,
			Uint32 &	b,
			Uint32 &	c,
			Uint32 &	d
	) {
		Uint32 temp;
		temp = a^c; temp = temp ^ ROL(temp, 8) ^ ROR(temp, 8);
		b ^= temp; d ^= temp;
		temp = b^d; temp = temp ^ ROL(temp, 8) ^ ROR(temp, 8);
		a ^= temp; c ^= temp;
	}

	static void THETA(
		const	Uint32		k[],
			Uint32 &	a,
			Uint32 &	b,
			Uint32 &	c,
			Uint32 &	d
	) {
		Uint32 temp;
		temp = a^c; temp = temp ^ ROL(temp, 8) ^ ROR(temp, 8);
		b ^= temp ^ k[1]; d ^= temp ^ k[3];
		temp = b^d; temp = temp ^ ROL(temp, 8) ^ ROR(temp, 8);
		a ^= temp ^ k[0]; c ^= temp ^ k[2];
	}

	static void GAMMA(
			Uint32 &	a,
			Uint32 &	b,
			Uint32 &	c,
			Uint32 &	d
	) {
		b ^= ~(d|c);
		a ^= c&b;
		Uint32 temp = d; d = a; a = temp;
		c ^= a ^ b ^ d;
		b ^= ~(d|c);
		a ^= c&b;
	}

	static void PI1(
			Uint32 &	a,
			Uint32 &	/* b */,
			Uint32 &	c,
			Uint32 &	d
	) {
		a = ROL(a, 1);
		c = ROL(c, 5);
		d = ROL(d, 2);
	}

	static void PI2(
			Uint32 &	a,
			Uint32 &	/* b */,
			Uint32 &	c,
			Uint32 &	d
	) {
		a = ROR(a, 1);
		c = ROR(c, 5);
		d = ROR(d, 2);
	}

private :

	static const Uint32 RC[];

	Uint32 K[4], dK[4];

	/// \brief Forbidden copy constructor.

	Noekeon(
		const	Noekeon &
	);

	/// \brief Forbidden copy operator.

	Noekeon & operator = (
		const	Noekeon &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Noekeon_H_

// ============================================================================

