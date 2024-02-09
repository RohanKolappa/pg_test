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
//	VMP.RTP.SBandwidth.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_SBandwidth_H_
#define _VMP_RTP_SBandwidth_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.SBandwidthPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL SBandwidth : virtual public BFC::SObject {

public :

	/// \brief Creates a new SBandwidth.

	SBandwidth(
	) :
		value	( 0 )
	{
	}

	void decode(
		const	BFC::Buffer &	pLineData
	);

	BFC::Buffer encode(
	) const;

	/// \brief Returns the modifier (e.g. "AS", "CT", ...).

	const BFC::Buffer & getModifier(
	) const {
		return modif;
	}

	/// \brief Returns the bandwidth value (in kbits/second).

	BFC::Uint64 getValue(
	) const {
		return value;
	}

protected :

private :

	BFC::Buffer	modif;
	BFC::Uint64	value;

	/// \brief Forbidden copy constructor.

	SBandwidth(
		const	SBandwidth &
	);

	/// \brief Forbidden copy operator.

	SBandwidth & operator = (
		const	SBandwidth &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_SBandwidth_H_

// ============================================================================

