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
//	VMP.HLS.FragmentData.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_FragmentData_H_
#define _VMP_HLS_FragmentData_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.FragmentDataPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "VMP.HLS.FragmentInfoPtr.h"
#include "VMP.HLS.TimeStampPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL FragmentData : virtual public BFC::SObject {

public :

	/// \brief Creates a new FragmentData.

	FragmentData(
			FragmentInfoPtr	pFragInfo,
//			FLVVectorPtr	pAudTgLst,
//			FLVVectorPtr	pVidTgLst,
		const	BFC::Bool	pFragDisc
	);

	void ensureCloseTo(
			TimeStampCPtr	pTimeStmp
	);

	BFC::Double getRelativeStartTime(
	) const;

	TimeStampCPtr getMinPTS(
	) const {

		return strtTime;

	}

	TimeStampCPtr getMaxPTS(
	) const {

		return stopTime;

	}

	FragmentInfoPtr	fragInfo;		///< Parent info.
//	FLVVectorPtr	audTgLst;		///< Audio tags.
//	FLVVectorPtr	vidTgLst;		///< Video tags.
	BFC::Bool	fragDisc;		///< Discontinuity ?

//	FLVVectorPtr	dataTags;		///< Media tags (audio + video).

protected :

private :

	TimeStampPtr	strtTime;		///< First PTS.
	TimeStampPtr	stopTime;		///< Last PTS.

	/// \brief Forbidden copy constructor.

	FragmentData(
		const	FragmentData &
	);

	/// \brief Forbidden copy operator.

	FragmentData & operator = (
		const	FragmentData &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_FragmentData_H_

// ============================================================================

