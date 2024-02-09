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
//	VMP.M2S.PESFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_PESFrame_H_
#define _VMP_M2S_PESFrame_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.PESFramePtr.h"

// ============================================================================

#include "VMP.BytesFrame.h"

#include "VMP.M2S.PESPacketPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL PESFrame : public BytesFrame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new PESFrame.

	PESFrame(
	);

	/// \brief Creates a new PESFrame.

	PESFrame(
			PESPacketCPtr	pPacket
	);

	/// \brief Copy constructor.

	PESFrame(
		const	PESFrame &
	);

	PESPacketCPtr getPacket(
	) const {
		return pesPackt;
	}

	void setPacket(
			PESPacketCPtr	pPESPackt
	) {
		pesPackt = pPESPackt;
	}

	virtual BFC::SPtr clone(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	PESPacketCPtr	pesPackt;

	/// \brief Forbidden copy operator.

	PESFrame & operator = (
		const	PESFrame &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_PESFrame_H_

// ============================================================================

