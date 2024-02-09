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
//	VMP.HLS.FragmentInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_FragmentInfo_H_
#define _VMP_HLS_FragmentInfo_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.FragmentInfoPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "VMP.HLS.TimeStampPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL FragmentInfo : virtual public BFC::SObject {

public :

	/// \brief Creates a new FragmentInfo.

	FragmentInfo(
		const	BFC::Buffer &	pFragmUrl,
		const	BFC::Double	pFragDrtn,
		const	BFC::Uint64	pFragSqNo,
		const	BFC::Uint64	pRngeStrt,
		const	BFC::Uint64	pRngeSize
	);

	const BFC::Buffer & getUrl(
	) const {

		return fragmUrl;

	}

	BFC::Double getDuration(
	) const {

		return fragDrtn;

	}

	void setDuration(
		const	BFC::Double	pFragDrtn
	) {

		fragDrtn = pFragDrtn;

	}

	BFC::Uint64 getSeqNo(
	) const {

		return fragSqNo;

	}

	BFC::Bool hasRange(
	) const {

		return ( rngeSize != 0 );

	}

	BFC::Uint64 getRangeStart(
	) const {

		return rngeStrt;

	}

	BFC::Uint64 getRangeSize(
	) const {

		return rngeSize;

	}

	BFC::Double getRelativeStartTime(
	) const {

		return strtTime;

	}

	BFC::Double getRelativeStopTime(
	) const {

		return ( getRelativeStartTime() + getDuration() );

	}

	void setRelativeStartTime(
		const	BFC::Double	pStrtTime
	) {

		strtTime = pStrtTime;

	}

	BFC::Bool hasRealPTS(
	) const {

		return ( mediaPTS );

	}

	TimeStampCPtr getRealPTS(
	) const {

		return mediaPTS;

	}

	void setRealPTS(
			TimeStampCPtr	pMediaPTS
	) {

		mediaPTS = pMediaPTS;

	}

	BFC::Bool sameAs(
			FragmentInfoPtr	pFragInfo
	) const;

	BFC::Buffer toString(
	) const;

protected :

private :

	BFC::Buffer	fragmUrl;	///< URL of this chunk.
	BFC::Double	m3u8Drtn;	///< Duration from manifest file.
	BFC::Double	fragDrtn;	///< Duration after merging PTS information.
	BFC::Uint64	fragSqNo;	///< Sequence number of this chunk.
	BFC::Uint64	rngeStrt;
	BFC::Uint64	rngeSize;
	BFC::Double	strtTime;	///< Start time of this chunk.
	TimeStampCPtr	mediaPTS;	///< Real PTS of physical fragment.

	/// \brief Forbidden copy constructor.

	FragmentInfo(
		const	FragmentInfo &
	);

	/// \brief Forbidden copy operator.

	FragmentInfo & operator = (
		const	FragmentInfo &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_FragmentInfo_H_

// ============================================================================

