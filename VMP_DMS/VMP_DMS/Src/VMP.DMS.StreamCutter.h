// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::DMS".
// 
// "VMP::DMS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::DMS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::DMS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.DMS.StreamCutter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_DMS_StreamCutter_H_
#define _VMP_DMS_StreamCutter_H_

// ============================================================================

#include "VMP.DMS.DLL.h"

// ============================================================================

#include "VMP.DMS.StreamCutterPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace DMS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_DMS

class VMP_DMS_DLL StreamCutter : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new StreamCutter.

	StreamCutter(
	);

	/// \brief Destroys this object.

	virtual ~StreamCutter(
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	void pause(
		const	BFC::Time::Clock &
					pTimeStmp
	);

	void resume(
		const	BFC::Time::Clock &
					pTimeStmp
	);

protected :

	void adjustTimeStamp(
			FramePtr	pCurrFrme,
		const	BFC::Time::Delta &
					pTimeDlta
	) const;

private :

	struct Entry {
		BFC::Time::Clock	strtTime;	///< Slot start time.
		BFC::Time::Clock	stopTime;	///< Slot stop time (if any).
		BFC::Time::Delta	timeDlta;	///< To subtract in this slot.
	};

	BFC::FastCS		critSect;
	BFC::Array< Entry >	entrList;
	BFC::Bool		lastOpen;
	BFC::Time::Clock	lastStop;
        BFC::Time::Delta        lastDlta;

	/// \brief Forbidden copy constructor.

	StreamCutter(
		const	StreamCutter &
	);

	/// \brief Forbidden copy operator.

	StreamCutter & operator = (
		const	StreamCutter &
	);

};

// ============================================================================

} // namespace DMS
} // namespace VMP

// ============================================================================

#endif // _VMP_DMS_StreamCutter_H_

// ============================================================================

