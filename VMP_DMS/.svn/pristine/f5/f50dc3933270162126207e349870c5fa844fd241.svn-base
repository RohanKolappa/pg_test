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
//	VMP.L3.ProcessorBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_ProcessorBackend_H_
#define _VMP_L3_ProcessorBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "VMP.L3.ProcessorBackendPtr.h"

// ============================================================================

#include "VMP.AudioSession.h"
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

class VMP_L3_DLL ProcessorBackend : public Backend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new ProcessorBackend.

	ProcessorBackend(
		const	BFC::DL::TypeCPtr &	pClassType,
		const	BFC::DL::TypeCPtr &	pSessionType
	) :
		Backend	( pClassType ),
		sessType( pSessionType ) {
	}

public :

	const BFC::DL::TypeCPtr & getSessionType(
	) const {
		return sessType;
	}

	virtual BFC::Bool canProcess(
			SessionPtr	pISession
	) = 0;

	virtual SessionPtr initSession(
			SessionPtr	pISession
	) = 0;

	virtual void exitSession(
	) = 0;

	virtual FramePtr processFrame(
			FramePtr	pIFrame
	) = 0;

	virtual BFC::Uint64 convertOffset(
		const	BFC::Uint64	pOffset
	) const;

protected :

private :

	BFC::DL::TypeCPtr	sessType;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L3

class VMP_L3_DLL AudioProcessorBackend : public ProcessorBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new AudioProcessorBackend.

	AudioProcessorBackend(
		const	BFC::DL::TypeCPtr &	pClassType
	) :
		ProcessorBackend(
			pClassType,
			AudioSession::getClassType() ) {
	}

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L3

class VMP_L3_DLL VideoProcessorBackend : public ProcessorBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new VideoProcessorBackend.

	VideoProcessorBackend(
		const	BFC::DL::TypeCPtr &	pClassType
	) :
		ProcessorBackend(
			pClassType,
			VideoSession::getClassType() ) {
	}

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_ProcessorBackend_H_

// ============================================================================

