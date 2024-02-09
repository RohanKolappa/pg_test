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
//	VMP.HLS.LeafManifest.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_LeafManifest_H_
#define _VMP_HLS_LeafManifest_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.LeafManifestPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"

#include "VMP.HLS.FragmentInfoPtr.h"
#include "VMP.HLS.Manifest.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL LeafManifest : public Manifest {

public :

	/// \brief Creates a new LeafManifest.

	LeafManifest(
	);

	virtual void decode(
		const	BFC::Buffer &	pLink,
		const	BFC::Buffer &	pData
	);

	const BFC::Time::Clock & getSeenDate(
	) const {

		return seenDate;

	}

	const BFC::Buffer & getUrl(
	) const {

		return currLink;

	}

	BFC::Bool hasEndlist(
	) const {

		return onDemand;

	}

	BFC::Double getTargetDuration(
	) const {

		return trgtDrtn;

	}

	BFC::Bool hasFragments(
	) const {

		return ( fragList.getSize() > 0 );

	}

	const BFC::Array< FragmentInfoPtr > & getFragments(
	) const {

		return fragList;

	}

	BFC::Uint64 getFirstSeqNo(
	) const;

	BFC::Uint64 getLastSeqNo(
	) const;

protected :

private :

	BFC::Time::Clock		seenDate;
	BFC::Buffer			currLink;
	BFC::Array< FragmentInfoPtr >	fragList;
	BFC::Bool			onDemand;
	BFC::Double			trgtDrtn;

	/// \brief Forbidden copy constructor.

	LeafManifest(
		const	LeafManifest &
	);

	/// \brief Forbidden copy operator.

	LeafManifest & operator = (
		const	LeafManifest &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_LeafManifest_H_

// ============================================================================

