// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TSTableFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSTableFramer_h_
#define _VMP_M2S_TSTableFramer_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSTableFramerPtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.TL.Array.h"

#include "VMP.GPC.ProxyConsumer.h"

#include "VMP.M2S.TSSectionPtr.h"
#include "VMP.M2S.TSTablePtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSTableFramer : public GPC::ProxyConsumer {

public :

	TSTableFramer(
		const	BFC::Bool	useTableIdExt = false
	);

	/// \brief Rebuilds TS Tables from TS Sections.

	virtual void putObject(
			BFC::SPtr	pTSSection
	);

protected :

private :

	BFC::Bool		useExt;
	// If useExt == true...
	struct Entry {
		BFC::Uint32		id_ext;
		TSTablePtr		table;
	};
	BFC::Array< Entry >	entries;
	// If useExt == false...
	TSTablePtr		table;

	/// \brief Forbidden copy constructor.

	TSTableFramer(
		const	TSTableFramer &
	);

	/// \brief Forbidden copy operator.

	TSTableFramer & operator = (
		const	TSTableFramer &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSTableFramer_h_

// ============================================================================

