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
//	VMP.M2S.DescriptorDecoder.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_DescriptorDecoder_h_
#define _VMP_M2S_DescriptorDecoder_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.DescriptorDecoderPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.M2S.DescriptorArray.h"
#include "VMP.M2S.DescriptorPtr.h"
#include "VMP.M2S.DescriptorRepositoryPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL DescriptorDecoder : virtual public BFC::DObject {

public :

	DescriptorDecoder(
			DescriptorRepositoryPtr
					pDescRepo
	);

	DescriptorArray decode(
		const	BFC::Buffer &	pDescData
	);

protected :

private :

	DescriptorRepositoryPtr	descRepo;

	/// \brief Forbidden default constructor.

	DescriptorDecoder(
	);

	/// \brief Forbidden copy constructor.

	DescriptorDecoder(
		const	DescriptorDecoder &
	);

	/// \brief Forbidden copy operator.

	DescriptorDecoder & operator = (
		const	DescriptorDecoder &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_DescriptorDecoder_h_

// ============================================================================

