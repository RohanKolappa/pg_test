// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.MuxedEncoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_MuxedEncoderBackend_H_
#define _VMP_L3_MuxedEncoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, MuxedEncoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.BytesFrame.h"
#include "VMP.BytesSession.h"
#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L3.StreamEncoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all mux encoder backends.
///
/// \ingroup VMP_L3

class VMP_L3_DLL MuxedEncoderBackend : public StreamEncoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new MuxedEncoderBackend.

	MuxedEncoderBackend(
		const	BFC::DL::TypeCPtr &	pType
	) :
		StreamEncoderBackend( pType ) {
	}

public :

	virtual const BFC::DL::TypeCPtr & getInputType(
	) const;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_MuxedEncoderBackend_H_

// ============================================================================

