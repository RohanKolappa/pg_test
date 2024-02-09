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
//	VMP.M2S.TSPMTFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSPMTFramer_h_
#define _VMP_M2S_TSPMTFramer_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPMTFramerPtr.h"

// ============================================================================

#include "BFC.TL.CompArray.h"

#include "VMP.GPC.ProxyConsumer.h"

#include "VMP.M2S.DescriptorDecoderPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSPMTFramer : public GPC::ProxyConsumer {

public :

	TSPMTFramer(
			DescriptorDecoderPtr
					descrDecoder,
		const	BFC::Uint32	pmtPID
	);

	void addProgId(
		const	BFC::Uint32	pProgId
	) {
		progId += pProgId;
	}

	/// \brief Rebuilds PMTs from TS Tables.

	virtual void putObject(
			BFC::SPtr	pTSTable
	);

protected :

private :

	DescriptorDecoderPtr		descrDecoder;
	BFC::CompArray< BFC::Uint32 >	progId;
	BFC::Uint32			pmtPID;

	/// \brief Forbidden default constructor.

	TSPMTFramer(
	);

	/// \brief Forbidden copy constructor.

	TSPMTFramer(
		const	TSPMTFramer &
	);

	/// \brief Forbidden copy operator.

	TSPMTFramer & operator = (
		const	TSPMTFramer &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPMTFramer_h_

// ============================================================================

