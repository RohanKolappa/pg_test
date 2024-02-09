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
//	BFC.Crypto.Khazad.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_Khazad_H_
#define _BFC_Crypto_Khazad_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Khazad )

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

class BFC_Crypto_DLL Khazad : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Khazad.

	Khazad(
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

	static void process(
		const	Uchar *		pt,
			Uchar *		ct,
		const	Uint64 *	roundKey
	);

private :

	static const Uint32 R		= 8;
	static const Uint32 KEYSIZE	= 128;
	static const Uint32 KEYSIZEB	= (KEYSIZE/8);
	static const Uint32 BLOCKSIZE	= 64;
	static const Uint32 BLOCKSIZEB	= (BLOCKSIZE/8);

	static const Uint64 T0[ 256 ];
	static const Uint64 T1[ 256 ];
	static const Uint64 T2[ 256 ];
	static const Uint64 T3[ 256 ];
	static const Uint64 T4[ 256 ];
	static const Uint64 T5[ 256 ];
	static const Uint64 T6[ 256 ];
	static const Uint64 T7[ 256 ];

	static const Uint64 c[ R + 1 ];

	Uint64 roundKeyEnc[ 8 + 1 ];
	Uint64 roundKeyDec[ 8 + 1 ];

	/// \brief Forbidden copy constructor.

	Khazad(
		const	Khazad &
	);

	/// \brief Forbidden copy operator.

	Khazad & operator = (
		const	Khazad &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Khazad_H_

// ============================================================================

