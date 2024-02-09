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
//	BFC.Crypto.RC6.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_RC6_H_
#define _BFC_Crypto_RC6_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, RC6 )

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

class BFC_Crypto_DLL RC6 : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new RC6.

	RC6(
	);

	virtual Uint32 getKeySize(
		const	Uint32		pKeySize
	) const;

	virtual void setKey(
		const	Buffer &	key
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

	static void encRND(
			Uint32 &	a,
			Uint32 &	b,
			Uint32 &	c,
			Uint32 &	d,
		const	Uint32 * &	K
	) {
		Uint32	t, u;
		t = (b * (b + b + 1)); t = ROL(t, 5);
		u = (d * (d + d + 1)); u = ROL(u, 5);
		a = ROL(a^t,u) + K[0];
		c = ROL(c^u,t) + K[1];
		K += 2;
	}

	static void decRND(
			Uint32 &	a,
			Uint32 &	b,
			Uint32 &	c,
			Uint32 &	d,
		const	Uint32 * &	K
	) {
		Uint32	t, u;
		K -= 2;
		t = (b * (b + b + 1)); t = ROL(t, 5);
		u = (d * (d + d + 1)); u = ROL(u, 5);
		c = ROR(c - K[1], t) ^ u;
		a = ROR(a - K[0], u) ^ t;
	}

private :

	static const Uint32 stab[ 44 ];

	Uint32	K[ 44 ];

	/// \brief Forbidden copy constructor.

	RC6(
		const	RC6 &
	);

	/// \brief Forbidden copy operator.

	RC6 & operator = (
		const	RC6 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_RC6_H_

// ============================================================================

