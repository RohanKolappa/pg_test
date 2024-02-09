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
//	VMP.HLS.LevelInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_LevelInfo_H_
#define _VMP_HLS_LevelInfo_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.LevelInfoPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.TL.Array.h"

#include "VMP.HLS.FragmentDataPtr.h"
#include "VMP.HLS.FragmentInfoPtr.h"
#include "VMP.HLS.LeafManifestPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL LevelInfo : virtual public BFC::DObject {

public :

	static const BFC::Uint64 InvalidSeqNo = ( BFC::Uint64 )-1;

	/// \brief Creates a new LevelInfo.

	LevelInfo(
	);

	/// \brief Returns the URL of this LevelInfo.

	const BFC::Buffer & getUrl(
	) const {

		return levelUrl;

	}

	void setUrl(
		const	BFC::Buffer &	pLevelUrl
	) {

		levelUrl = pLevelUrl;

	}

	void setProgramId(
		const	BFC::Uint64	pProgrmId
	) {

		progrmId = pProgrmId;

	}

	BFC::Uint64 getBandwidth(
	) const {

		return bandwdth;

	}

	void setBandwidth(
		const	BFC::Uint64	pBandwdth
	) {

		bandwdth = pBandwdth;

	}

	void setResolution(
		const	BFC::Uint64	pVdeoWdth,
		const	BFC::Uint64	pVdeoHght
	) {

		vdeoWdth = pVdeoWdth;
		vdeoHght = pVdeoHght;

	}

	void setCodecs(
		const	BFC::Buffer &	pReqCodec
	) {

		reqCodec = pReqCodec;

	}

	/// \brief Returns the target duration.

	BFC::Double getTargetDuration(
	) const {

		return trgtDrtn;

	}

	/// \brief Returns true iff there is at least one fragment available.

	BFC::Bool hasFragments(
	) const {

		return ( fragList.getSize() > 0 );

	}

	/// \brief Returns the first seqno.

	BFC::Uint64 getFirstSeqNo(
	) const {

		return frstSqNo;

	}

	/// \brief Returns the last seqno.

	BFC::Uint64 getLastSeqNo(
	) const {

		return lastSqNo;

	}

	/// \brief Returns the average fragment duration.

	BFC::Double getAverageDuration(
	) const {

		return ( totlDrtn ? totlDrtn / ( BFC::Double )fragList.getSize() : 0.0 );

	}

	/// \brief Returns the total level duration.

	BFC::Double getTotalDuration(
	) const {

		return totlDrtn;

	}

	/// \brief Updates list of fragments.

	void mergeManifest(
			LeafManifestPtr	pManifest
	);

	void updateFragment(
			FragmentInfoPtr	pFragInfo,
			FragmentDataPtr	pFragData
	);

	FragmentInfoPtr getFragmentFromSeqNo(
		const	BFC::Uint64	pFragSqNo
	) const;

	FragmentInfoPtr getFragmentByPosition(
		const	BFC::Uint64	pCrsrOffs
	) const;

	FragmentInfoPtr getFirstFragment(
	) const;

	FragmentInfoPtr getLastFragment(
	) const;

protected :

	BFC::Uint64 getSeqNoBeforePosition(
		const	BFC::Uint64	position
	) const;

	BFC::Uint32 getIndexFromSeqNo(
		const	BFC::Uint64	pFragSqNm
	) const;

	void updateStartTime(
	);

private :

	BFC::Buffer	levelUrl;	///< URL of this level.
	BFC::Double	trgtDrtn;	///< Target duration (from M3U8).
	BFC::Array< FragmentInfoPtr >
			fragList;	///< Level fragments.
	BFC::Uint64	frstSqNo;	///< Min sequence number from M3U8.
	BFC::Uint64	lastSqNo;	///< Max sequence number from M3U8.
	BFC::Double	totlDrtn;	///< Total level duration.

	BFC::Uint64	progrmId;
	BFC::Uint64	bandwdth;
	BFC::Uint64	vdeoWdth;
	BFC::Uint64	vdeoHght;
	BFC::Buffer	reqCodec;

	/// \brief Forbidden copy constructor.

	LevelInfo(
		const	LevelInfo &
	);

	/// \brief Forbidden copy operator.

	LevelInfo & operator = (
		const	LevelInfo &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_LevelInfo_H_

// ============================================================================

