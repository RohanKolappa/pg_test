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
//	VMP.HLS.LeafManifest.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.MemoryInputStream.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.HLS.FragmentInfo.h"
#include "VMP.HLS.LeafManifest.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, LeafManifest )

// ============================================================================

HLS::LeafManifest::LeafManifest() :

	seenDate	( Time::now() ),
	onDemand	( false ),
	trgtDrtn	( 0.0 ) {

	setObjectName( "VMP.HLS.LeafManifest" );

}

// ============================================================================

void HLS::LeafManifest::decode(
	const	Buffer &	pLink,
	const	Buffer &	pData ) {

	msgDbg( "decode()" );

	currLink = pLink;
	fragList.kill();

	IO::MemoryInputStreamPtr	iMStream = new IO::MemoryInputStream( pData );
	IO::ByteInputStreamPtr		iBStream = new IO::ByteInputStream( iMStream );

	iBStream->open();

	if ( iBStream->getLine().trim() != EXTM3U ) {
		throw InternalError( "Not an .m3u8 file!" );
	}

	Uint64	mediaSeq = 0;		// EXT_X_MEDIA_SEQUENCE value.
	Double	duration = 0.0;		// EXTINF duration.
	Bool	needLink = false;	// Just saw an EXTINF ?
	Bool	hasRange = false;
	Uint64	rngeStrt = 0;
	Uint64	rngeSize = 0;

	for (;;) {

		Buffer line;

		try {
			line = iBStream->getLine().trim();
		}
		catch ( IO::EndOfStream & ) {
			break;
		}

		if ( line.isEmpty()
		  || line == "#" ) {
		}
		else if ( line.beginsWith( EXT_X_MEDIA_SEQUENCE ) ) {
			mediaSeq = line( EXT_X_MEDIA_SEQUENCE.getLength() ).toUint64();
		}
		else if ( line.beginsWith( EXT_X_TARGETDURATION ) ) {
			trgtDrtn = line( EXT_X_TARGETDURATION.getLength() ).toDouble();
		}
		else if ( line.beginsWith( EXT_X_ALLOW_CACHE )
		       || line.beginsWith( EXT_X_PLAYLIST_TYPE )
		       || line.beginsWith( EXT_X_VERSION ) ) {
		}
		else if ( line.beginsWith( EXTINF ) ) {
			Uint32 pos = line.findFwd( ',' );
			duration = ( pos >= line.getLength()
				? line( EXTINF.getLength() ).toDouble()
				: line( EXTINF.getLength(), pos - EXTINF.getLength() ).toDouble() );
			needLink = true;
			hasRange = false;
		}
		else if ( line.beginsWith( EXT_X_BYTERANGE ) ) {
			if ( ! needLink ) {
				throw InternalError( "Misplaced byte range!" );
			}
			BufferArray tokens = BufferParser::tokenize( line( EXT_X_BYTERANGE.getLength() ), "@" );
			if ( tokens.getSize() == 1 ) {
				rngeStrt += rngeSize;
			}
			else {
				rngeStrt = tokens[ 1 ].toUint64();
			}
			rngeSize = tokens[ 0 ].toUint64();
			hasRange = true;
		}
		else if ( line.beginsWith( EXT_X_ENDLIST ) ) {
			onDemand = true;
			break;
		}
		else if ( needLink ) {
			Buffer	url = parseURL( line, pLink );
			fragList += ( hasRange
				? new FragmentInfo( url, duration, mediaSeq++, rngeStrt, rngeSize )
				: new FragmentInfo( url, duration, mediaSeq++, 0, 0 ) );
			needLink = false;
		}
		else {
			throw InternalError( "\"" + line + "\" syntax not supported!" );
		}

	}

	if ( fragList.getSize() == 0 ) {
		msgWrn( "decode(): No TS fragments found in " + pLink );
	}

}

// ============================================================================

Uint64 HLS::LeafManifest::getFirstSeqNo() const {

	return fragList.getFirst()->getSeqNo();

}

Uint64 HLS::LeafManifest::getLastSeqNo() const {

	return fragList.getLast()->getSeqNo();

}

// ============================================================================

