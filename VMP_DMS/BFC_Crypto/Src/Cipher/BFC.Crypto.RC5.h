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
//	BFC.Crypto.RC5.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_RC5_H_
#define _BFC_Crypto_RC5_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, RC5 )

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

class BFC_Crypto_DLL RC5 : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new RC5.

	RC5(
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

private :

	static const Uint32 stab[50];

	Uint32	rounds;
	Uint32	K[ 50 ];

	/// \brief Forbidden copy constructor.

	RC5(
		const	RC5 &
	);

	/// \brief Forbidden copy operator.

	RC5 & operator = (
		const	RC5 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_RC5_H_

// ============================================================================

