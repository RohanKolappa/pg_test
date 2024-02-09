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
//	VMP.RTP.TClockLine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TClockLine_H_
#define _VMP_RTP_TClockLine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TClockLinePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.Time.Clock.h"

#include "VMP.RTP.TClockPointPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TClockLine : virtual public BFC::SObject {

public :

	/// \brief Creates a new TClockLine.

	TClockLine(
			TClockPointCPtr	pFrsPoint
	);

	/// \brief Creates a new TClockLine.

	TClockLine(
			TClockPointCPtr	pFrsPoint,
			TClockPointCPtr	pLstPoint
	);

	TClockPointCPtr getFirstPoint(
	) const {
		return frsPoint;
	}

	TClockPointCPtr getLastPoint(
	) const {
		return lstPoint;
	}

	BFC::Double getClockSpeed(
	) const {
		return clkSpeed;
	}

	BFC::Time::Clock getTargetForSource(
		const	BFC::Time::Clock &
					pSrceTime
	) const;

	BFC::Time::Clock getSourceForTarget(
		const	BFC::Time::Clock &
					pTrgtTime
	) const;

	TClockPointCPtr getPointForSource(
		const	BFC::Time::Clock &
					pSrceTime
	) const;

	TClockPointCPtr getPointForTarget(
		const	BFC::Time::Clock &
					pTrgtTime
	) const;

	BFC::Time::Delta getSourceDelta(
	) const;

	BFC::Time::Delta getTargetDelta(
	) const;

	BFC::Time::Delta getSourceDelta(
			TClockPointCPtr	pCurPoint
	) const;

	BFC::Time::Delta getTargetDelta(
			TClockPointCPtr	pCurPoint
	) const;

	void resetPoints(
		const	BFC::Time::Clock &
					pSrceTime
	);

	void setLastPoint(
			TClockPointCPtr	pLstPoint
	);

	void extendLastPoint(
		const	BFC::Time::Clock &
					pSrceTime
	);

	BFC::Buffer toBuffer(
	) const;

protected :

	void computeSpeed(
	);

private :

	TClockPointCPtr		frsPoint;	///< First sample point.
	TClockPointCPtr		lstPoint;	///< Last sample point.
	BFC::Double		clkSpeed;

	/// \brief Forbidden default constructor.

	TClockLine(
	);

	/// \brief Forbidden copy constructor.

	TClockLine(
		const	TClockLine &
	);

	/// \brief Forbidden copy operator.

	TClockLine & operator = (
		const	TClockLine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TClockLine_H_

// ============================================================================

