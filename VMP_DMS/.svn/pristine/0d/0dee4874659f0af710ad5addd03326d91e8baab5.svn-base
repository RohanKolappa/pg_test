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
//	VMP.RTP.TMPEG4GenericContext.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TMPEG4GenericContext_H_
#define _VMP_RTP_TMPEG4GenericContext_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TMPEG4GenericContextPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "VMP.M4S.Defines.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TMPEG4GenericContext : virtual public BFC::SObject {

public :

	TMPEG4GenericContext(
	) :
		smplRate	( 0 ),
		nbrChans	( 0 ),
		strmType	( M4S::Forbidden ),
		profLevl	( 0 ),
		sizeLgth	( 0 ),
		indxLgth	( 0 ),
		dltaLgth	( 0 )
	{
	}

	BFC::Uint32	smplRate;
	BFC::Uint32	nbrChans;

	BFC::Uint32	strmType;	///< "streamType=5" (see ISO/IEC 14496-1, Table 9).
	BFC::Uint32	profLevl;	///< "profile-level-id=15" (see ISO/IEC 14496-1, Table 5, ...).
	BFC::Buffer	strmMode;	///< "mode=AAC-hbr".
	BFC::Buffer	codcCnfg;	///< "config=...".
	BFC::Uint32	sizeLgth;
	BFC::Uint32	indxLgth;
	BFC::Uint32	dltaLgth;

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TMPEG4GenericContext_H_

// ============================================================================

