// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedIndex.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_MuxedIndex_H_
#define _VMP_MuxedIndex_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, MuxedIndex )

} // namespace VMP

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL MuxedIndex : virtual public BFC::SObject {

public :

	MuxedIndex(
	) {
	}

	MuxedIndex(
		const	MuxedIndex &	pOther
	) :
		BFC::SObject(),
		table( pOther.table ) {
	}

	void setNbrMuxedFrames(
		const	BFC::Uint32	pNbrMuxedFrames
	) {
		table.resize( pNbrMuxedFrames );
	}

	void setMuxedFrameData(
		const	BFC::Uint32	pMuxedFrameIndex,
		const	BFC::Uint32	pSessionIndex,
		const	BFC::Uint64	pSubFrameIndex
	) {
		table[ pMuxedFrameIndex ].sessionIndex = pSessionIndex;
		table[ pMuxedFrameIndex ].subFrameIndex = pSubFrameIndex;
	}

	BFC::Uint32 getSessionIndex(
		const	BFC::Uint32	pMuxedFrameIndex
	) const {
		return table[ pMuxedFrameIndex ].sessionIndex;
	}

	BFC::Uint64 getSubFrameIndex(
		const	BFC::Uint32	pMuxedFrameIndex
	) const {
		return table[ pMuxedFrameIndex ].subFrameIndex;
	}

	BFC::Uint32 lookup(
		const	BFC::Uint32	pSessionIndex,
		const	BFC::Uint64	pSubFrameIndex
	) const;

	BFC::Buffer toBuffer(
		const	BFC::Uint32	pMaxIndex = 0
	) const;

protected :

private :

	struct Entry {
		BFC::Uint32		sessionIndex;
		BFC::Uint64		subFrameIndex;
	};

	BFC::Array< Entry >	table;

	/// \brief Forbidden copy operator.

	MuxedIndex & operator = (
		const	MuxedIndex &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_MuxedIndex_H_

// ============================================================================

