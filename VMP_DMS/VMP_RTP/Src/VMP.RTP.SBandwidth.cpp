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
//	VMP.RTP.SBandwidth.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.RTP.SBandwidth.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SBandwidth )

// ============================================================================

void RTP::SBandwidth::decode(
	const	Buffer &	pLineData ) {

	BufferArray itemList = BufferParser::tokenize( pLineData, ":" );

	if ( itemList.getSize() != 2 ) {
		throw InternalError( "Invalid bandwidth: \"" + pLineData + "\"!" );
	}

	modif = itemList[ 0 ];
	value = itemList[ 1 ].toUint64() /* * 1024 */;

//	if ( modif != "CT"		// RFC 4566
//	  && modif != "AS"		// RFC 4566
//	  && modif != "RR"		// RFC 3556
//	  && modif != "RS" ) {		// RFC 3556
//		msgWrn( "decode(): unknown bandwidth modifier: \"" + modif + "\"!" );
//	}

}

Buffer RTP::SBandwidth::encode() const {

	return ( modif + ":" + Buffer( value ) );
}

// ============================================================================

