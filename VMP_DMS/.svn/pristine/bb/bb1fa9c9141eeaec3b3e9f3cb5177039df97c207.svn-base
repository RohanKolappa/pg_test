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
//	VMP.L3.MultiProcessorBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_MultiProcessorBackend_H_
#define _VMP_L3_MultiProcessorBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, MultiProcessorBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.Frame.h"
#include "VMP.VideoSession.h"

#include "VMP.L3.Backend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L3

class VMP_L3_DLL MultiProcessorBackend : public Backend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new MultiProcessorBackend.

	MultiProcessorBackend(
		const	BFC::DL::TypeCPtr &	pClassType,
		const	BFC::Uint32		pNbrInputs,
		const	BFC::DL::TypeCPtr &	pSessionType
	) :
		Backend		( pClassType ),
		nbrInputs	( pNbrInputs ),
		sessType	( pSessionType ) {
	}

public :

	BFC::Uint32 getNbrInputs(
	) const {
		return nbrInputs;
	}

	const BFC::DL::TypeCPtr & getSessionType(
	) const {
		return sessType;
	}

	virtual BFC::Bool canProcess(
			SessionArray	pISession
	) = 0;

	virtual SessionPtr initSession(
			SessionArray	pISession
	) = 0;

	virtual void exitSession(
	) = 0;

	virtual FramePtr processFrame(
			FrameArray	pIFrame
	) = 0;

	virtual BFC::Uint64 convertOffset(
		const	BFC::Uint64	pOffset
	) const;

protected :

private :

	BFC::Uint32		nbrInputs;
	BFC::DL::TypeCPtr	sessType;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L3

class VMP_L3_DLL VideoMultiProcessorBackend : public MultiProcessorBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new VideoMultiProcessorBackend.

	VideoMultiProcessorBackend(
		const	BFC::DL::TypeCPtr &	pClassType,
		const	BFC::Uint32		pNbrInputs
	) :
		MultiProcessorBackend(
			pClassType,
			pNbrInputs,
			VideoSession::getClassType() ) {
	}

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_MultiProcessorBackend_H_

// ============================================================================

