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
//	BFC.Crypto.Anubis.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Anubis_H_
#define _BFC_Crypto_Anubis_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Anubis )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Cipher.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Anubis : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Anubis.

	Anubis(
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

	void process(
		const	Buffer &	pPlainText,
			Buffer &	pCipherText,
		const	Uint32		pRoundKey[18 + 1][4]
	);

private :

	static const Uint32 MIN_N		= 4;
	static const Uint32 MAX_N		= 10;
	static const Uint32 MIN_ROUNDS		= 8 + MIN_N;
	static const Uint32 MAX_ROUNDS		= 8 + MAX_N;
	static const Uint32 MIN_KEYSIZEB	= 4 * MIN_N;
	static const Uint32 MAX_KEYSIZEB	= 4 * MAX_N;
	static const Uint32 BLOCKSIZE		= 128;
	static const Uint32 BLOCKSIZEB		= BLOCKSIZE / 8;

	static const Uint32 T0[256];
	static const Uint32 T1[256];
	static const Uint32 T2[256];
	static const Uint32 T3[256];
	static const Uint32 T4[256];
	static const Uint32 T5[256];
	static const Uint32 rc[];

	Uint32	keyBits;
	Uint32	R;
	Uint32	roundKeyEnc[18 + 1][4];
	Uint32	roundKeyDec[18 + 1][4];

	/// \brief Forbidden copy constructor.

	Anubis(
		const	Anubis &
	);

	/// \brief Forbidden copy operator.

	Anubis & operator = (
		const	Anubis &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Anubis_H_

// ============================================================================

