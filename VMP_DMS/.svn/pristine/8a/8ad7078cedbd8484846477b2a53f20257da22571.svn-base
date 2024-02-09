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
//	VMP.M2S.EStream.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_EStream_h_
#define _VMP_M2S_EStream_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.EStreamPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

#include "VMP.M2S.DescriptorArray.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

typedef BFC::Array< EStreamPtr >	EStreamArray;

// ============================================================================

class VMP_M2S_DLL EStream : virtual public BFC::SObject {

public :

	static BFC::Buffer getStreamTypeName(
		const	BFC::Uint32		type
	);

	BFC::Buffer getStreamTypeName(
	) const {
		return getStreamTypeName( stream_type );
	}

	BFC::Uint32	stream_type;
	BFC::Uint32	pid;
	DescriptorArray	descriptors;

protected :

private :

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_EStream_h_

// ============================================================================

