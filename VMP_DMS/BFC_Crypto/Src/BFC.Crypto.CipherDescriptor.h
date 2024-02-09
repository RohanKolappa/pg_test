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
//	BFC.Crypto.CipherDescriptor.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_CipherDescriptor_H_
#define _BFC_Crypto_CipherDescriptor_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, CipherDescriptor )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Descr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Cipher descriptor.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL CipherDescriptor : public DL::Descr {

public :

	/// \brief Creates a new CipherDescriptor.

	CipherDescriptor(
		const	DL::Descr &	pDescr = DL::Descr(),
		const	Uint32		pMinKeySze = 0,
		const	Uint32		pMaxKeySze = 0,
		const	Uint32		pBlockSize = 0,
		const	Uint32		pDftRounds = 0
	);

	CipherDescriptor(
		const	CipherDescriptor &
					pOther
	);

	virtual DL::DescrPtr clone(
	) const;

	Uint32 getMinKeySize(
	) const {
		return minKeySze;
	}

	Uint32 getMaxKeySize(
	) const {
		return maxKeySze;
	}

	Uint32 getBlockSize(
	) const {
		return blockSize;
	}

	Uint32 getDefaultRounds(
	) const {
		return dftRounds;
	}

private :

	Uint32	minKeySze;	///< Min keysize (bytes).
	Uint32	maxKeySze;	///< Max keysize (bytes).
	Uint32	blockSize;	///< Block size (bytes).
	Uint32	dftRounds;	///< Default number of rounds.

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_CipherDescriptor_H_

// ============================================================================

