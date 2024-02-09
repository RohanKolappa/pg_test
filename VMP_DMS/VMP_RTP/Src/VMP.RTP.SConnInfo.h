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
//	VMP.RTP.SConnInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_SConnInfo_H_
#define _VMP_RTP_SConnInfo_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.SConnInfoPtr.h"

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

class VMP_RTP_DLL SConnInfo : virtual public BFC::SObject {

public :

	/// \brief Creates a new SConnInfo.

	SConnInfo(
	) :
		mcastTTL	( ( BFC::Uint32 )-1 ),
		nmbrAddr	( ( BFC::Uint32 )-1 )
	{
	}

	void decode(
		const	BFC::Buffer &	pLineData
	);

	void decode(
		const	BFC::Buffer &	pNwrkType,
		const	BFC::Buffer &	pAddrType,
		const	BFC::Buffer &	pAddrData
	);

	BFC::Buffer encode(
	) const;

	const BFC::Buffer & getAddress(
	) const {
		return addrData;
	}

	BFC::Uint32 getTTL(
		const	BFC::Uint32	pDfltData = 1
	) const {
		return ( mcastTTL != ( BFC::Uint32 )-1 ? mcastTTL : pDfltData );
	}

	BFC::Uint32 getNbrAddresses(
		const	BFC::Uint32	pDfltData = 1
	) const {
		return ( nmbrAddr != ( BFC::Uint32 )-1 ? nmbrAddr : pDfltData );
	}

protected :

private :

	BFC::Buffer	addrData;
	BFC::Uint32	mcastTTL;
	BFC::Uint32	nmbrAddr;

	/// \brief Forbidden copy constructor.

	SConnInfo(
		const	SConnInfo &
	);

	/// \brief Forbidden copy operator.

	SConnInfo & operator = (
		const	SConnInfo &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_SConnInfo_H_

// ============================================================================

