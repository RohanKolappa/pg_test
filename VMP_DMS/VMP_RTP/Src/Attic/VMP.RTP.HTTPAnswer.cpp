// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP".
// 
// "VMP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.HTTPAnswer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"
// #include "BFC.Base.Utils.h"

#include "VMP.RTP.HTTPAnswer.h"

// ============================================================================

using namespace BFC;
using namespace VMP::RTP;

// ============================================================================

BFC_PTR_IMPL( HTTPAnswer )

// ============================================================================

HTTPAnswer::HTTPAnswer() {

	setObjectName( "VMP.RTP.HTTPAnswer" );

}

HTTPAnswer::~HTTPAnswer() {



}

// ============================================================================

Bool HTTPAnswer::consume(
	const	Buffer &	buffer ) {

	Uint32	delim = buffer.findFwd( "\r\n\r\n" );
	Buffer	header = buffer( 0, delim );

	// Interpret header...

	options = BufferParser::tokenize( header, "\r\n" );

	if ( options.isEmpty() ) {
		throw InvalidData( "Empty header!" );
	}

	Buffer		ack = options[ 0 ];
	BufferArray	tokens = BufferParser::tokenize( ack, " " );

	if ( tokens.getSize() < 2 ) {
		throw InvalidData( "Ack a bit too small!" );
	}

	protocol = tokens[ 0 ];

	Uint32 dummy = 0;

	status = tokens[ 1 ].getUintAt( dummy );

	if ( dummy != tokens[ 1 ].getLength() ) {
		throw InvalidData( "Invalid Ack! (" + tokens[ 1 ] + ")" );
	}

	options.pop( 0 );

//	Uint32	size = options.getSize();
//
//	if ( size && options[ size - 1 ] == "\r\n" ) {
//		options.pop( size - 1 );
//	}

	// Prepare payload...

	payload = buffer( delim + 4 );

	// Verify payload length...

	for ( Uint32 i = 0 ; i < options.getSize() ; i++ ) {
		if ( options[ i ].toLower().beginsWith( "content-length: " ) ) {
			Buffer line = options[ i ]( 16 );
			dummy = 0;
			Uint32 len = line.getUintAt( dummy );
			if ( dummy != line.getLength() ) {
				throw InvalidData( "Invalid content-length!" );
			}
			if ( len != payload.getLength() ) {
				throw InvalidData( "Invalid payload length!" );
			}
			options.pop( i );
			break;
		}
	}

//	msgDbg( "Payload: " + Utils::toHex( payload ) );
//	msgDbg( "Payload: " + payload );

	return true;

}

// ============================================================================

