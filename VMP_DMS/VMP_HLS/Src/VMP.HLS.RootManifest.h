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
//	VMP.HLS.RootManifest.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_RootManifest_H_
#define _VMP_HLS_RootManifest_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.RootManifestPtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"

#include "VMP.HLS.LevelInfoPtr.h"
#include "VMP.HLS.Manifest.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL RootManifest : public Manifest {

public :

	/// \brief Creates a new RootManifest.

	RootManifest(
	);

	virtual void decode(
		const	BFC::Buffer &	pLink,
		const	BFC::Buffer &	pData
	);

	const BFC::Buffer & getUrl(
	) const {

		return currLink;

	}

	const BFC::Array< LevelInfoPtr > & getLevels(
	) const {

		return levlList;

	}

protected :

	LevelInfoPtr parseLevel(
		const	BFC::Buffer &	pLineData
	) const;

private :

	BFC::Time::Clock		seenDate;
	BFC::Buffer			currLink;
	BFC::Array< LevelInfoPtr >	levlList;

	/// \brief Forbidden copy constructor.

	RootManifest(
		const	RootManifest &
	);

	/// \brief Forbidden copy operator.

	RootManifest & operator = (
		const	RootManifest &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_RootManifest_H_

// ============================================================================

