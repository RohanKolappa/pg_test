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
//	BFC.Crypto.ModeRepository.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_ModeRepository_H_
#define _BFC_Crypto_ModeRepository_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, ModeRepository )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Factory.h"
#include "BFC.DL.Repository.h"

#include "BFC.Crypto.Mode.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief Mode repository.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL ModeRepository : public DL::Repository {

protected :

	/// \brief Creates a new ModeRepository.

	ModeRepository(
	);

public :

	static ModeRepositoryPtr instance(
	);

	DL::FactoryPtr findFactoryFor(
		const	Buffer &	pModeName
	);

	ModePtr getNew(
		const	Buffer &	pModeName
	) {
		return findFactoryFor( pModeName )->makeObject();
	}

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_ModeRepository_H_

// ============================================================================

