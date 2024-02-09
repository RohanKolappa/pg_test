// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::HLS".
// 
// "VMP::HLS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::HLS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::HLS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.HLS.FrameRateComputer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_FrameRateComputer_H_
#define _VMP_HLS_FrameRateComputer_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.FrameRateComputerPtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL FrameRateComputer : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new FrameRateComputer.

	FrameRateComputer(
	);

	/// \brief Destroys this object.

	virtual ~FrameRateComputer(
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	virtual void flushSessionCallback(
	);

protected :

private :

	BFC::Bool		gotFRate;
	SessionPtr		inptSess;
	VideoStreamPtr		inptStrm;
	BFC::Array< FramePtr >	inptFrme;
	BFC::Bool		gotKFrme;
	BFC::Uint32		kFrmIndx;

	/// \brief Forbidden copy constructor.

	FrameRateComputer(
		const	FrameRateComputer &
	);

	/// \brief Forbidden copy operator.

	FrameRateComputer & operator = (
		const	FrameRateComputer &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_FrameRateComputer_H_

// ============================================================================

