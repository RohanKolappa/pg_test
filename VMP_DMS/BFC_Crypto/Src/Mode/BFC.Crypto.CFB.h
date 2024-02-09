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
//	BFC.Crypto.CFB.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_CFB_H_
#define _BFC_Crypto_CFB_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, CFB )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Mode.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief "Cipher FeedBack" (CFB) Mode of Operation
///
/// The cipher feedback (CFB) mode, a close relative of CBC, makes a block
/// cipher into a self-synchronizing stream cipher. Operation is very similar;
/// in particular, CFB decryption is almost identical to CBC encryption
/// performed in reverse:
/*!
 *	\f{eqnarray*}{
 *		C_i & = & E_K (C_{i-1}) \oplus P_i \\
 *		P_i & = & E_K (C_{i-1}) \oplus C_i \\
 *		C_{0} & = & \ \mbox{IV}
 *	\f}
 */
/// Like CBC mode, changes in the plaintext propagate forever in the
/// ciphertext, and encryption cannot be parallelized. Also like CBC,
/// decryption can be parallelized. When decrypting, a one-bit change in the
/// ciphertext affects two plaintext blocks: a one-bit change in the
/// corresponding plaintext block, and complete corruption of the following
/// plaintext block. Later plaintext blocks are decrypted normally.
///
/// CFB shares two advantages over CBC mode with the stream cipher modes OFB
/// and CTR: the block cipher is only ever used in the encrypting direction,
/// and the message does not need to be padded to a multiple of the cipher
/// block size (though ciphertext stealing can also be used to make padding
/// unnecessary).
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL CFB : public Mode {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new CFB.

	CFB(
	);

	virtual void init(
			CipherPtr	pCipher,
		const	Buffer &	pIV,
		const	Buffer &	pKey
	);

	virtual void done(
	);

	virtual Buffer encrypt(
		const	Buffer &	pPlainText
	);

	virtual Buffer decrypt(
		const	Buffer &	pCipherText
	);

	virtual Buffer getiv(
	) const;

	virtual void setiv(
		const	Buffer &	pIV
	);

	virtual void test(
	);

protected :

private :

	CipherPtr	cipher;
	Uint32		blkSze;
	Buffer		IV;
	Buffer		tmpBuf;

	/// \brief Forbidden copy constructor.

	CFB(
		const	CFB &
	);

	/// \brief Forbidden copy operator.

	CFB & operator = (
		const	CFB &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_CFB_H_

// ============================================================================

