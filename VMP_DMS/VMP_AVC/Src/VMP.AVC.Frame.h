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
//	VMP.AVC.Frame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_Frame_H_
#define _VMP_AVC_Frame_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.FramePtr.h"

// ============================================================================

#include "VMP.BytesFrame.h"

#include "VMP.AVC.NALUPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL Frame : public BytesFrame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Frame.

	Frame(
	);

	/// \brief Creates a new Frame.

	Frame(
			NALUCPtr	pNALU
	);

	/// \brief Creates a new Frame.

	Frame(
			VMP::FrameCPtr	pTmplFrme,
			NALUCPtr	pNALU
	);

	/// \brief Copy constructor.

	Frame(
		const	Frame &	pOther
	);

	virtual BFC::SPtr clone(
	) const;

	NALUCPtr getNALU(
	) const {
		return nalu;
	}

	void setNALU(
			NALUCPtr	pNALU
	);

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	NALUCPtr	nalu;

	/// \brief Forbidden copy operator.

	Frame & operator = (
		const	Frame &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_Frame_H_

// ============================================================================

