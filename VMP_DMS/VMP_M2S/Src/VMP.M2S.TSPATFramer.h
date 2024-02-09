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
//	VMP.M2S.TSPATFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSPATFramer_h_
#define _VMP_M2S_TSPATFramer_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPATFramerPtr.h"

// ============================================================================

#include "VMP.GPC.ProxyConsumer.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSPATFramer : public GPC::ProxyConsumer {

public :

	TSPATFramer(
	);

	/// \brief Rebuilds PATs from TS Tables.

	virtual void putObject(
			BFC::SPtr	pTSTable
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	TSPATFramer(
		const	TSPATFramer &
	);

	/// \brief Forbidden copy operator.

	TSPATFramer & operator = (
		const	TSPATFramer &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPATFramer_h_

// ============================================================================

