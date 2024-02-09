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
//	BFC.Crypto.OMAC.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_OMAC_H_
#define _BFC_Crypto_OMAC_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, OMAC )
BFC_PTR_DECL( BFC_Crypto_DLL, OMAC2 )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Cipher.h"
#include "BFC.Crypto.MAC.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Implements common parts of OMAC and OMAC2.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL OMACBase : public MAC {

protected :

	/// \brief Creates a new OMACBase.

	OMACBase(
		const	DL::DescrCPtr &
					pDescr
	);

public :

	/// \brief Processes data through OMACBase.
	///
	/// \param pData
	///	The data to send through OMACBase.

	virtual void process(
		const	Buffer &	pData
	);

	/// \brief Terminates an OMACBase session.
	///
	/// \param pTag
	///	The destination of the OMACBase authentication tag.

	virtual void done(
			Buffer &	pTag
	);

protected :

	static void shiftL(
			Buffer &	pBuffer
	);

	static void shiftR(
			Buffer &	pBuffer
	);

	CipherPtr	cipher;
	Uint32		blkLen;
	Buffer		buf;
	Buffer		prv;
	Buffer		Lu[ 2 ];	///< L.u and L.u^2.

private :

	/// \brief Forbidden default constructor.

	OMACBase(
	);

	/// \brief Forbidden copy constructor.

	OMACBase(
		const	OMACBase &
	);

	/// \brief Forbidden copy operator.

	OMACBase & operator = (
		const	OMACBase &
	);

};

// ============================================================================

/// \brief One-Key CBC MAC (a.k.a. OMAC1).
///
/// OMAC is a blockcipher-based message authentication code designed and
/// analyzed by Tetsu Iwata and Kaoru Kurosawa.
///
/// OMAC is a simple variant of the CBC MAC (Cipher Block Chaining Message
/// Authentication Code).
///
/// OMAC is secure for messages of any bit length (while the CBC MAC is only
/// secure on messages of one fixed length, and the length must be a multiple
/// of the block length). Also, the efficiency of OMAC is highly optimized. It
/// is almost as efficient as the CBC MAC.
///
/// ``NIST Special Publication 800-38B Recommendation for Block Cipher Modes
/// of Operation: the CMAC Mode for Authentication'' has been finalized on May
/// 18, 2005. This Recommendation specifies CMAC, which is equivalent to OMAC
/// (OMAC1).
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL OMAC : public OMACBase {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new OMAC.

	OMAC(
	);

	/// \brief Initializes this object.
	///
	/// \param pCipher
	///	The cipher you want to use.
	///
	/// \param pKey
	///	The secret key.

	void init(
			CipherPtr	pCipher,
		const	Buffer &	pKey
	);

	/// \brief Performs OMAC self-test.

	virtual void test(
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	OMAC(
		const	OMAC &
	);

	/// \brief Forbidden copy operator.

	OMAC & operator = (
		const	OMAC &
	);

};

// ============================================================================

/// \brief One-Key CBC MAC 2 (a.k.a. OMAC2).
///
/// Variation of OMAC using different vectors.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL OMAC2 : public OMACBase {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new OMAC2.

	OMAC2(
	);

	/// \brief Initializes this object.
	///
	/// \param pCipher
	///	The cipher you want to use.
	///
	/// \param pKey
	///	The secret key.

	void init(
			CipherPtr	pCipher,
		const	Buffer &	pKey
	);

	/// \brief Performs OMAC2 self-test.

	virtual void test(
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	OMAC2(
		const	OMAC2 &
	);

	/// \brief Forbidden copy operator.

	OMAC2 & operator = (
		const	OMAC2 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_OMAC_H_

// ============================================================================

