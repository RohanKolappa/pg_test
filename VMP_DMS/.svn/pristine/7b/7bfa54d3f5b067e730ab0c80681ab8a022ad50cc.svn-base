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
//	VMP.AVC.SEI.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_SEI_H_
#define _VMP_AVC_SEI_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.SEIPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.AVC.BitstreamPtr.h"
#include "VMP.AVC.NALUPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL SEI : virtual public BFC::DObject {

public :

	/// \brief Creates a new SEI.

	SEI(
	);

	void decode(
			NALUCPtr	pNALU
	);

	BFC::Buffer toBuffer(
	) const;

protected :

//	void decodeBufferingPeriod(
//			BitstreamPtr	pIBitStrm,
//		const	BFC::Uint32	pDataSize
//	);

private :

	/// \brief Forbidden copy constructor.

	SEI(
		const	SEI &
	);

	/// \brief Forbidden copy operator.

	SEI & operator = (
		const	SEI &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_SEI_H_

// ============================================================================

