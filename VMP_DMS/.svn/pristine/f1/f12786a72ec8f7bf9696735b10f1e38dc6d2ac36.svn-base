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
//	BFC.Crypto.HashDescriptor.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_HashDescriptor_H_
#define _BFC_Crypto_HashDescriptor_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, HashDescriptor )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Descr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Hash descriptor.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL HashDescriptor : public DL::Descr {

public :

	/// \brief Creates a new HashDescriptor.

	HashDescriptor(
		const	DL::Descr &	pDescr = DL::Descr(),
		const	Uint32		pHashSize = 0,
		const	Uint32		pBlckSize = 0
	);

	HashDescriptor(
		const	HashDescriptor &
					pOther
	);

	virtual DL::DescrPtr clone(
	) const;

	Uint32 getHashSize(
	) const {
		return hashSize;
	}

	Uint32 getBlockSize(
	) const {
		return blckSize;
	}

private :

	Uint32	hashSize;
	Uint32	blckSize;

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_HashDescriptor_H_

// ============================================================================

