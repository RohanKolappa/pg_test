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
//	VMP.L3.AudioDecoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L3_AudioDecoderBackend_H_
#define _VMP_L3_AudioDecoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, AudioDecoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.L3.StreamDecoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all audio decoder backends.
///
/// An AudioDecoderBackend is an object capable of decoding a stream of
/// audio frames.
///
/// \ingroup VMP_L3_Audio

class VMP_L3_DLL AudioDecoderBackend : public StreamDecoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new AudioDecoderBackend.

	AudioDecoderBackend(
		const	BFC::DL::TypeCPtr &	pType
	) :
		StreamDecoderBackend( pType ) {
	}

public :

	virtual const BFC::DL::TypeCPtr & getOutputType(
	) const;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_AudioDecoderBackend_H_

// ============================================================================

