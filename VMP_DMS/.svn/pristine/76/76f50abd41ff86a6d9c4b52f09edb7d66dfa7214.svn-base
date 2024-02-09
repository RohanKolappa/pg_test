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
//	VMP.AVC.NALUInputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_NALUInputStream_H_
#define _VMP_AVC_NALUInputStream_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.NALUInputStreamPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.IO.ByteInputStreamPtr.h"

#include "VMP.AVC.NALUPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL NALUInputStream : virtual public BFC::DObject {

public :

	/// \brief Creates a new NALUInputStream.

	NALUInputStream(
			BFC::IO::ByteInputStreamPtr
					pIStream
	);

	void init(
	);

	NALUPtr peekNALU(
	);

	NALUPtr getNALU(
	);

	NALUPtr getNALU(
		const	BFC::Uchar	pType
	);

	void popNALU(
	);

protected :

	void fillNALU(
	);

	static BFC::Uint32 findStartCode(
		const	BFC::Buffer &	pData
	);

private :

	BFC::IO::ByteInputStreamPtr	strm;
	BFC::Buffer			buff;
	NALUPtr				nalu;
	BFC::Bool			done;

	/// \brief Forbidden default constructor.

	NALUInputStream(
	);

	/// \brief Forbidden copy constructor.

	NALUInputStream(
		const	NALUInputStream &
	);

	/// \brief Forbidden copy operator.

	NALUInputStream & operator = (
		const	NALUInputStream &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_NALUInputStream_H_

// ============================================================================

