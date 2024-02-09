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
//	VMP.DMS.RTPMultiInjector.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_DMS_RTPMultiInjector_H_
#define _VMP_DMS_RTPMultiInjector_H_

// ============================================================================

#include "VMP.DMS.DLL.h"

// ============================================================================

#include "VMP.DMS.RTPMultiInjectorPtr.h"

// ============================================================================

#include "VMP.L2.PushEngineProxy.h"

#include "VMP.RTP.NFramePtr.h"
#include "VMP.RTP.SProgramPtr.h"

// ============================================================================

namespace VMP {
namespace DMS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_DMS

class VMP_DMS_DLL RTPMultiInjector : public L2::PushEngineProxy {

public :

	/// \brief Creates a new RTPMultiInjector.

	RTPMultiInjector(
	);

	/// \brief Destroys this object.

	virtual ~RTPMultiInjector(
	);

	void init(
			RTP::SProgramPtr
					pSProgram
	);

	void cleanup(
	);

	void putVideoPacket(
			RTP::NFramePtr	pNetFrame
	);

	void putAudioPacket(
			RTP::NFramePtr	pNetFrame
	);

protected :

private :

	BFC::Uint32		vidIndex;
	BFC::Uint32		audIndex;

	/// \brief Forbidden copy constructor.

	RTPMultiInjector(
		const	RTPMultiInjector &
	);

	/// \brief Forbidden copy operator.

	RTPMultiInjector & operator = (
		const	RTPMultiInjector &
	);

};

// ============================================================================

} // namespace DMS
} // namespace VMP

// ============================================================================

#endif // _VMP_DMS_RTPMultiInjector_H_

// ============================================================================

