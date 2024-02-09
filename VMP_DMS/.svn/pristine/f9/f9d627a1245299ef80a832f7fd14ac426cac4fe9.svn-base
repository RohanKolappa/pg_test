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
//	BFC.Net.UUIdGenerator.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_Net_UUIdGenerator_H_
#define _BFC_Net_UUIdGenerator_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, UUIdGenerator )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.Time.Clock.h"

#include "BFC.Net.MACAddress.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Generates random UUId's.
///
/// See http://upnp.org/resources/draft-leach-uuids-guids-00.txt.

class BFC_Net_DLL UUIdGenerator : virtual public SObject {

protected :

	UUIdGenerator(
	);

public :

	static UUIdGeneratorPtr instance(
	);

	/// \brief Generates a new UUId.
	///
	/// This method creates a new universally unique identifier (UUId).
	/// The new UUId can reasonably be considered unique among all UUIDs
	/// created on the local system, and among UUIDs created on other
	/// systems in the past and in the future.

	UUId generate(
	);

protected :

	void getClock(
			Uint32 &	clock_high,
			Uint32 &	clock_low
	);

	MACAddress getNodeId(
	);

private :

	Time::Clock	lastTime;
	Uint16		clockSeq;
	Uint32		clockAdj;
	Buffer		node_id;

	static const Uint32	MAX_ADJUSTMENT;

	/// \brief Forbidden copy constructor.

	UUIdGenerator(
		const	UUIdGenerator &
	);

	/// \brief Forbidden copy operator.

	UUIdGenerator& operator = (
		const	UUIdGenerator &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_UUIdGenerator_H_

// ============================================================================

