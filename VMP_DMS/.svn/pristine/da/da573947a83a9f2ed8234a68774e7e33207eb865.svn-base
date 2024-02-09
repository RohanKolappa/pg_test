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
//	VMP.RTP.SOrigin.cpp
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

#include "VMP.RTP.SConnInfo.h"
#include "VMP.RTP.SOrigin.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SOrigin )

// ============================================================================

void RTP::SOrigin::decode(
	const	Buffer &	pLineData ) {

	BufferArray itemList = BufferParser::tokenize( pLineData, " " );

	if ( itemList.getSize() != 6 ) {
		throw InternalError( "Invalid origin: \"" + pLineData + "\"!" );
	}

	userName = itemList[ 0 ];
	sessIdnt = itemList[ 1 ];
	sessVers = itemList[ 2 ];

	SConnInfoPtr	c = new SConnInfo;

	c->decode( itemList[ 3 ], itemList[ 4 ], itemList[ 5 ] );

	connInfo = c;

}

Buffer RTP::SOrigin::encode() const {

	return ( userName + " " + sessIdnt + " " + sessVers + " " + connInfo->encode() );

}

// ============================================================================

