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
//	BFC.Crypto.AES.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_AES_H_
#define _BFC_Crypto_AES_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, AES )

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

class BFC_Crypto_DLL AES : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new AES.

	AES(
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

	static const Uint32 TE0[256];
	static const Uint32 Te4[256];
	static const Uint32 TD0[256];
	static const Uint32 Td4[256];

	inline static Uint32 Te0( const Uint32 x ) { return TE0[x]; }
	inline static Uint32 Te1( const Uint32 x ) { return ROR(TE0[x], 8); }
	inline static Uint32 Te2( const Uint32 x ) { return ROR(TE0[x], 16); }
	inline static Uint32 Te3( const Uint32 x ) { return ROR(TE0[x], 24); }

	static inline Uint32 Td0( const Uint32 x ) { return TD0[x]; }
	static inline Uint32 Td1( const Uint32 x ) { return ROR(TD0[x], 8); }
	static inline Uint32 Td2( const Uint32 x ) { return ROR(TD0[x], 16); }
	static inline Uint32 Td3( const Uint32 x ) { return ROR(TD0[x], 24); }

protected :

	static Uint32 setup_mix(Uint32 temp);
	static Uint32 setup_mix2(Uint32 temp);

private :

	static const Uint32 rcon[];

	Uint32		eK[60];
	Uint32		dK[60];
	Uint32		Nr;

	/// \brief Forbidden copy constructor.

	AES(
		const	AES &
	);

	/// \brief Forbidden copy operator.

	AES & operator = (
		const	AES &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_AES_H_

// ============================================================================

