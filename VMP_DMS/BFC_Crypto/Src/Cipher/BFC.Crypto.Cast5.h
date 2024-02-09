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
//	BFC.Crypto.Cast5.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Cast5_H_
#define _BFC_Crypto_Cast5_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Cast5 )

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

class BFC_Crypto_DLL Cast5 : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Cast5.

	Cast5(
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

	/* returns the i'th getByte of a variable */
	static Uint32 GB(
		const	Uint32		x[ 4 ],
		const	Uint32		i
	) {
		return (((x[(15-i)>>2]) >> (8*((15-i)&3))) & 255 );
	}

	static Uint32 FI(Uint32 R, Uint32 Km, Uint32 Kr)
	{
		Uint32 I;
		I = (Km + R);
		I = ROL(I, Kr);
		return ( ( S1[getByte(I, 3)] ^ S2[getByte(I,2)] ) - S3[getByte(I,1)]) + S4[getByte(I,0)];
	}
		
	static Uint32 FII(Uint32 R, Uint32 Km, Uint32 Kr)
	{
		Uint32 I;
		I = (Km ^ R);
		I = ROL(I, Kr);
		return ((S1[getByte(I, 3)] - S2[getByte(I,2)]) + S3[getByte(I,1)]) ^ S4[getByte(I,0)];
	}
	
	static Uint32 FIII(Uint32 R, Uint32 Km, Uint32 Kr)
	{
		Uint32 I;
		I = (Km - R);
		I = ROL(I, Kr);
		return ((S1[getByte(I, 3)] + S2[getByte(I,2)]) ^ S3[getByte(I,1)]) - S4[getByte(I,0)];
	}

private :

	static const Uint32 S1[256];
	static const Uint32 S2[256];
	static const Uint32 S3[256];
	static const Uint32 S4[256];
	static const Uint32 S5[256];
	static const Uint32 S6[256];
	static const Uint32 S7[256];
	static const Uint32 S8[256];

	Uint32	K[32];
	Uint32	keylen;

	/// \brief Forbidden copy constructor.

	Cast5(
		const	Cast5 &
	);

	/// \brief Forbidden copy operator.

	Cast5 & operator = (
		const	Cast5 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Cast5_H_

// ============================================================================

