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
//	BFC.Crypto.Safer.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_Safer_H_
#define _BFC_Crypto_Safer_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, SaferK64 )
BFC_PTR_DECL( BFC_Crypto_DLL, SaferSK64 )
BFC_PTR_DECL( BFC_Crypto_DLL, SaferK128 )
BFC_PTR_DECL( BFC_Crypto_DLL, SaferSK128 )

BFC_PTR_DECL( BFC_Crypto_DLL, SaferPlus )

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

class BFC_Crypto_DLL SaferBase : public Cipher {

protected :

	/// \brief Creates a new SaferBase.

	SaferBase(
		const	CipherDescriptorCPtr &
					pDesc
	);

public :

	void setRounds(
		const	Uint32		pRounds
	);

	virtual void encrypt(
		const	Buffer &	pPlainText,
			Buffer &	pCipherText
	);

	virtual void decrypt(
		const	Buffer &	pCipherText,
			Buffer &	pPlainText
	);

	static const Uchar EBox[ 256 ];
	static const Uchar LBox[ 256 ];

protected :

	static const Uint32 MaxRounds	= 13;
	static const Uint32 CBlockLen	= 8;
	static const Uint32 CKeyLen	= ( 1 + CBlockLen * ( 1 + 2 * MaxRounds ) );

	static Uchar ROL8(
		const	Uchar		x,
		const	Uint32		n
	) {
		return ( ( x << n )
		       | ( x >> ( 8 - n ) ) );
	}

	static Uchar EXP(
		const	Uchar		x
	) {
		return EBox[ ( Uint32 )x ];
	}

	static Uchar LOG(
		const	Uchar		x
	) {
		return LBox[ ( Uint32 )x ];
	}

	static void PHT(
			Uchar &		x,
			Uchar &		y
	) {
		y += x; x += y;
	}

	static void IPHT(
			Uchar &		x,
			Uchar &		y
	) {
		x -= y; y -= x;
	}

	void expandKey(
		const	Uchar *		userkey_1,
		const	Uchar *		userkey_2,
		const	Bool		strengthened
	);

	Uchar	key[ CKeyLen ];
	Uint32	keylen;
	Uint32	rounds;		///< Number of rounds.

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Safer64 : public SaferBase {

protected :

	/// \brief Creates a new Safer64.

	Safer64(
		const Crypto::CipherDescriptorCPtr &	pDesc
	);

public :

	virtual Uint32 getKeySize(
		const	Uint32		pKeySize
	) const;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL SaferK64 : public Safer64 {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SaferK64.

	SaferK64(
	);

	virtual void setKey(
		const	Buffer &	pKey
	);

	virtual void test(
	);

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL SaferSK64 : public Safer64 {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SaferSK64.

	SaferSK64(
	);

	virtual void setKey(
		const	Buffer &	pKey
	);

	virtual void test(
	);

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Safer128 : public SaferBase {

protected :

	/// \brief Creates a new Safer128.

	Safer128(
		const Crypto::CipherDescriptorCPtr &	pDesc
	);

public :

	virtual Uint32 getKeySize(
		const	Uint32		pKeySize
	) const;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL SaferK128 : public Safer128 {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SaferK128.

	SaferK128(
	);

	virtual void setKey(
		const	Buffer &	pKey
	);

	virtual void test(
	);

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL SaferSK128 : public Safer128 {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SaferSK128

	SaferSK128(
	);

	virtual void setKey(
		const	Buffer &	pKey
	);

	virtual void test(
	);

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL SaferPlus : public Cipher {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SaferPlus.

	SaferPlus(
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

	void ROUND(
			Uchar *		b,
		const	Uint32		i
	) {																								\
		b[0]  = SaferBase::EBox[(b[0] ^ K[i][0]) & 255] + K[i+1][0];
		b[1]  = SaferBase::LBox[(b[1] + K[i][1]) & 255] ^ K[i+1][1];
		b[2]  = SaferBase::LBox[(b[2] + K[i][2]) & 255] ^ K[i+1][2];
		b[3]  = SaferBase::EBox[(b[3] ^ K[i][3]) & 255] + K[i+1][3];
		b[4]  = SaferBase::EBox[(b[4] ^ K[i][4]) & 255] + K[i+1][4];
		b[5]  = SaferBase::LBox[(b[5] + K[i][5]) & 255] ^ K[i+1][5];
		b[6]  = SaferBase::LBox[(b[6] + K[i][6]) & 255] ^ K[i+1][6];
		b[7]  = SaferBase::EBox[(b[7] ^ K[i][7]) & 255] + K[i+1][7];
		b[8]  = SaferBase::EBox[(b[8] ^ K[i][8]) & 255] + K[i+1][8];
		b[9]  = SaferBase::LBox[(b[9] + K[i][9]) & 255] ^ K[i+1][9];
		b[10] = SaferBase::LBox[(b[10] + K[i][10]) & 255] ^ K[i+1][10];
		b[11] = SaferBase::EBox[(b[11] ^ K[i][11]) & 255] + K[i+1][11];
		b[12] = SaferBase::EBox[(b[12] ^ K[i][12]) & 255] + K[i+1][12];
		b[13] = SaferBase::LBox[(b[13] + K[i][13]) & 255] ^ K[i+1][13];
		b[14] = SaferBase::LBox[(b[14] + K[i][14]) & 255] ^ K[i+1][14];
		b[15] = SaferBase::EBox[(b[15] ^ K[i][15]) & 255] + K[i+1][15];
	}

	void iROUND(
			Uchar *		b,
		const	Uint32		i
	) {
		b[0]  = SaferBase::LBox[(b[0] - K[i+1][0]) & 255] ^ K[i][0];
		b[1]  = SaferBase::EBox[(b[1] ^ K[i+1][1]) & 255] - K[i][1];
		b[2]  = SaferBase::EBox[(b[2] ^ K[i+1][2]) & 255] - K[i][2];
		b[3]  = SaferBase::LBox[(b[3] - K[i+1][3]) & 255] ^ K[i][3];
		b[4]  = SaferBase::LBox[(b[4] - K[i+1][4]) & 255] ^ K[i][4];
		b[5]  = SaferBase::EBox[(b[5] ^ K[i+1][5]) & 255] - K[i][5];
		b[6]  = SaferBase::EBox[(b[6] ^ K[i+1][6]) & 255] - K[i][6];
		b[7]  = SaferBase::LBox[(b[7] - K[i+1][7]) & 255] ^ K[i][7];
		b[8]  = SaferBase::LBox[(b[8] - K[i+1][8]) & 255] ^ K[i][8];
		b[9]  = SaferBase::EBox[(b[9] ^ K[i+1][9]) & 255] - K[i][9];
		b[10] = SaferBase::EBox[(b[10] ^ K[i+1][10]) & 255] - K[i][10];
		b[11] = SaferBase::LBox[(b[11] - K[i+1][11]) & 255] ^ K[i][11];
		b[12] = SaferBase::LBox[(b[12] - K[i+1][12]) & 255] ^ K[i][12];
		b[13] = SaferBase::EBox[(b[13] ^ K[i+1][13]) & 255] - K[i][13];
		b[14] = SaferBase::EBox[(b[14] ^ K[i+1][14]) & 255] - K[i][14];
		b[15] = SaferBase::LBox[(b[15] - K[i+1][15]) & 255] ^ K[i][15];
	}

	void PHT(
			Uchar *		b
	) {
		b[0]  = (b[0] + (b[1] = (b[0] + b[1]) & 255)) & 255;
		b[2]  = (b[2] + (b[3] = (b[3] + b[2]) & 255)) & 255;
		b[4]  = (b[4] + (b[5] = (b[5] + b[4]) & 255)) & 255;
		b[6]  = (b[6] + (b[7] = (b[7] + b[6]) & 255)) & 255;
		b[8]  = (b[8] + (b[9] = (b[9] + b[8]) & 255)) & 255;
		b[10] = (b[10] + (b[11] = (b[11] + b[10]) & 255)) & 255;
		b[12] = (b[12] + (b[13] = (b[13] + b[12]) & 255)) & 255;
		b[14] = (b[14] + (b[15] = (b[15] + b[14]) & 255)) & 255;
	}

	void iPHT(
			Uchar *		b
	) {
		b[15] = (b[15] - (b[14] = (b[14] - b[15]) & 255)) & 255;
		b[13] = (b[13] - (b[12] = (b[12] - b[13]) & 255)) & 255;
		b[11] = (b[11] - (b[10] = (b[10] - b[11]) & 255)) & 255;
		b[9]  = (b[9] - (b[8] = (b[8] - b[9]) & 255)) & 255;
		b[7]  = (b[7] - (b[6] = (b[6] - b[7]) & 255)) & 255;
		b[5]  = (b[5] - (b[4] = (b[4] - b[5]) & 255)) & 255;
		b[3]  = (b[3] - (b[2] = (b[2] - b[3]) & 255)) & 255;
		b[1]  = (b[1] - (b[0] = (b[0] - b[1]) & 255)) & 255;
	}

	void SHUF(
		const	Uchar *		b,
			Uchar *		b2
	) {
		b2[0] = b[8]; b2[1] = b[11]; b2[2] = b[12]; b2[3] = b[15];
		b2[4] = b[2]; b2[5] = b[1]; b2[6] = b[6]; b2[7] = b[5];
		b2[8] = b[10]; b2[9] = b[9]; b2[10] = b[14]; b2[11] = b[13];
		b2[12] = b[0]; b2[13] = b[7]; b2[14] = b[4]; b2[15] = b[3];
	}

	void iSHUF(
		const	Uchar *		b,
			Uchar *		b2
	) {
		b2[0] = b[12]; b2[1] = b[5]; b2[2] = b[4]; b2[3] = b[15];
		b2[4] = b[14]; b2[5] = b[7]; b2[6] = b[6]; b2[7] = b[13];
		b2[8] = b[0]; b2[9] = b[9]; b2[10] = b[8]; b2[11] = b[1];
		b2[12] = b[2]; b2[13] = b[11]; b2[14] = b[10]; b2[15] = b[3];
	}

	void LT(
			Uchar *		b,
			Uchar *		b2
	) {
		PHT(b);  SHUF(b, b2);
		PHT(b2); SHUF(b2, b);
		PHT(b);  SHUF(b, b2);
		PHT(b2);
	}

	void iLT(
			Uchar *		b,
			Uchar *		b2
	) {
		iPHT(b);
		iSHUF(b, b2); iPHT(b2);
		iSHUF(b2, b); iPHT(b);
		iSHUF(b, b2); iPHT(b2);
	}

private :

	static const Uchar safer_bias[33][16];

	Uchar	K[33][16];
	Uint32	rounds;

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Safer_H_

// ============================================================================

