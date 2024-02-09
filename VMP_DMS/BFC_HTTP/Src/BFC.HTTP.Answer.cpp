// ============================================================================
// 
// Copyright (c) 2003-2009 Barco N.V.
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
//	BFC.HTTP.Answer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.HTTP.Answer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( HTTP, Answer )

// ============================================================================

HTTP::Answer::Answer() {

	setObjectName( "BFC.HTTP.Answer" );

	gotHdr = false;
	gotPld = false;

}

// ============================================================================

Bool HTTP::Answer::parse(
	const	Buffer &	partial ) {

	if ( gotPld ) {
		throw Overflow();
	}

	if ( gotHdr ) {
		return parsePayload( partial );
	}

	hdrBuf += partial;

	Uint32	pos = 0;

	if ( ! hdrBuf.findBufferAt( pos, "\r\n\r\n" ) ) {
		return false;
	}

	gotHdr = true;

	Buffer pldTmp = hdrBuf( pos + 4 );

	hdrBuf = hdrBuf( 0, pos + 2 );
	header = BufferParser::tokenize( hdrBuf, "\r\n" );

	Buffer	hdrTmp = getHeader( "content-length" );

	pldLen = ( hdrTmp.hasContent() ? hdrTmp.toUint32() : 0 );
	chunks = ( getHeader( "transfer-encoding" ).toLower() == "chunked" );

	if ( chunks && pldLen ) {
		msgWrn( "parse(): both chunks and content-length!" );
	}

	if ( ! chunks && ! pldLen ) {
		msgWrn( "parse(): can't determine payload length!" );
	}

	return parsePayload( pldTmp );

}

// ============================================================================

Buffer HTTP::Answer::getHeader(
	const	Buffer &	pTag ) const {

	Buffer	tag = pTag.toLower() + ": ";

	for ( Uint32 i = 0 ; i < header.getSize() ; i++ ) {
		if ( header[ i ].toLower().beginsWith( tag ) ) {
			return header[ i ]( tag.getLength() );
		}
	}

	return Buffer();

}

// ============================================================================

Bool HTTP::Answer::parsePayload(
	const	Buffer &	pData ) {

	if ( ! chunks ) {
		pldBuf += pData;
		if ( pldLen && pldBuf.getLength() >= pldLen ) {
			gotPld = true;
		}
		return gotPld;
	}

	chkBuf += pData;

	for (;;) {
		// Try to parse a chunk. First, chunk length.
		Uint32	pos = 0;
		if ( ! chkBuf.findBufferAt( pos, "\r\n" ) ) {
			return false;
		}
//		msgDbg( "chunk: str: \"" + chkBuf( 0, pos ) + "\"" );
		Buffer	tmp = "0x" + chkBuf( 0, pos );
		Uint32	len = tmp.toUint32();
		pos += 2;
//		msgDbg( "chunk: len == " + Buffer( len )
//			+ ", pos == " + Buffer( pos ) );
		// Enough in buffer to read that amount of data ?
		if ( pos + len + 2 > chkBuf.getLength() ) {
//			msgWrn( "chunk: cur: " + Buffer( chkBuf.getLength() ) );
			return false;
		}
		if ( chkBuf( pos + len, 2 ) != "\r\n" ) {
			throw InvalidArgument( "Corrupted chunk!" );
		}
		if ( len ) {
			pldBuf += chkBuf( pos, len );
			pos += len + 2;
			chkBuf = chkBuf( pos );
		}
		else {
			if ( ! pldLen ) {
				pldLen = pldBuf.getLength();
			}
			else if ( pldLen != pldBuf.getLength() ) {
				throw InvalidArgument( "Chunked lengths differ !" );
			}
			gotPld = true;
			return true;
		}
	}

}

// ============================================================================

