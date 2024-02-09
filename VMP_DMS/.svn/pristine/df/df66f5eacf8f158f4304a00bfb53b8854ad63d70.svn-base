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
//	VMP.DMS.DllInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_DMS_DllInfo_H_
#define _VMP_DMS_DllInfo_H_

// ============================================================================

#include "VMP.DMS.DLL.h"

// ============================================================================

#include "VMP.DMS.DllInfoPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Base.Version.h"

#include "BFC.Time.Clock.h"

// ============================================================================

namespace VMP {
namespace DMS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_DMS

class VMP_DMS_DLL DllInfo : virtual public BFC::SObject {

protected :

	/// \brief Creates a new DllInfo.

	DllInfo(
	);

public :

	static DllInfoCPtr instance(
	);

	const BFC::Version & getVersion(
	) const {
		return compVers;
	}

	const BFC::Time::Clock & getBuildTime(
	) const {
		return compTime;
	}

	BFC::Buffer toBuffer(
	) const;

protected :

private :

	BFC::Version		compVers;	///< Component version.
	BFC::Time::Clock	compTime;	///< Compilation time.

	/// \brief Forbidden copy constructor.

	DllInfo(
		const	DllInfo &
	);

	/// \brief Forbidden copy operator.

	DllInfo & operator = (
		const	DllInfo &
	);

};

// ============================================================================

} // namespace DMS
} // namespace VMP

// ============================================================================

#endif // _VMP_DMS_DllInfo_H_

// ============================================================================

