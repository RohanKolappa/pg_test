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
//	VMP.RTP.SConnInfo.cpp
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

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SConnInfo )

// ============================================================================

void RTP::SConnInfo::decode(
	const	Buffer &	pLineData ) {

	BufferArray itemList = BufferParser::tokenize( pLineData, " " );

	if ( itemList.getSize() != 3 ) {
		throw InternalError( "Invalid connection info: \"" + pLineData + "\"!" );
	}

	decode( itemList[ 0 ], itemList[ 1 ], itemList[ 2 ] );

}

void RTP::SConnInfo::decode(
	const	Buffer &	pNwrkType,
	const	Buffer &	pAddrType,
	const	Buffer &	pAddrData ) {

	if ( pNwrkType != "IN" ) {
		throw NotImplemented( "Unsupported network type: \"" + pNwrkType + "\"!" );
	}

	if ( pAddrType != "IP4" ) {
		throw NotImplemented( "Unsupported address type: \"" + pAddrType + "\"!" );
	}

	BufferArray itemList = BufferParser::tokenize( pAddrData, "/" );

	if ( itemList.getSize() > 3 ) {
		throw InternalError( "Unsupported connection info syntax: \"" + pAddrData + "\"!" );
	}

	addrData = itemList.popFirst();

	if ( itemList.hasContent() ) {
		nmbrAddr = itemList.popLast().toUint32();
	}

	if ( itemList.hasContent() ) {
		mcastTTL = itemList.popLast().toUint32();
	}

}

Buffer RTP::SConnInfo::encode() const {

	return ( "IN IP4 " + addrData
		+ ( mcastTTL != ( Uint32 )-1 ? "/" + Buffer( mcastTTL ) : Buffer() )
		+ ( nmbrAddr != ( Uint32 )-1 ? "/" + Buffer( nmbrAddr ) : Buffer() ) );

}

// ============================================================================

