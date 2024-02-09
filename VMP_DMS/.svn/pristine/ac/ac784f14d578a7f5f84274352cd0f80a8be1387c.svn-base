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
//	BFC.Crypto.Kasumi.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Kasumi_H_
#define _BFC_Crypto_Kasumi_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Kasumi )

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

class BFC_Crypto_DLL Kasumi : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Kasumi.

	Kasumi(
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

	static Uint32 ROL16(
		const	Uint16		x,
		const	Uint32		y
	) {
		return ( ( ( x << y ) | ( x >> ( 16 - y ) ) ) & 0xFFFF );
	}

	Uint16 FI( Uint16 in, Uint16 subkey ) const;
	Uint32 FO( Uint32 in, Uint32 round_no ) const;
	Uint32 FL( Uint32 in, Uint32 round_no ) const;

private :

	Uint32	KLi1[8];
	Uint32	KLi2[8];
	Uint32	KOi1[8];
	Uint32	KOi2[8];
	Uint32	KOi3[8];
	Uint32	KIi1[8];
	Uint32	KIi2[8];
	Uint32	KIi3[8];

	/// \brief Forbidden copy constructor.

	Kasumi(
		const	Kasumi &
	);

	/// \brief Forbidden copy operator.

	Kasumi & operator = (
		const	Kasumi &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Kasumi_H_

// ============================================================================

