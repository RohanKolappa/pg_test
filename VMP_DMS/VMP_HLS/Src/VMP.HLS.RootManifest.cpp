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
//	VMP.HLS.RootManifest.cpp
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

#include "VMP.HLS.LevelInfo.h"
#include "VMP.HLS.RootManifest.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, RootManifest )

// ============================================================================

HLS::RootManifest::RootManifest() {

	setObjectName( "VMP.HLS.RootManifest" );

}

// ============================================================================

void HLS::RootManifest::decode(
	const	Buffer &	pLink,
	const	Buffer &	pData ) {

	msgDbg( "decode" );

	seenDate = Time::now();
	currLink = pLink;
	levlList.kill();

	IO::MemoryInputStreamPtr	iMStream = new IO::MemoryInputStream( pData );
	IO::ByteInputStreamPtr		iBStream = new IO::ByteInputStream( iMStream );

	iBStream->open();

	Buffer	line = iBStream->getLine().trim();

	if ( line != EXTM3U ) {
		throw InternalError( "RootManifest::decode(): "
			"Not an .m3u8 file! First line: " + line );
	}

	LevelInfoPtr	currLevl;
	Bool		needLink = false;		// Next line is link ?

	for (;;) {

		try {
			line = iBStream->getLine().trim();
		}
		catch ( IO::EndOfStream & ) {
			break;
		}

		if ( line.isEmpty() ) {
		}
		else if ( line.beginsWith( EXT_X_MEDIA ) ) {
//#EXT-X-MEDIA:TYPE=AUDIO,GROUP-ID="bipbop_audio",LANGUAGE="eng",NAME="BipBop Audio 1",AUTOSELECT=YES,DEFAULT=YES
//#EXT-X-MEDIA:TYPE=AUDIO,GROUP-ID="bipbop_audio",LANGUAGE="eng",NAME="BipBop Audio 2",AUTOSELECT=NO,DEFAULT=NO,URI="alternate_audio_aac_sinewave/prog_index.m3u8"
//#EXT-X-MEDIA:TYPE=SUBTITLES,GROUP-ID="subs",NAME="English",DEFAULT=YES,AUTOSELECT=YES,FORCED=NO,LANGUAGE="en",URI="subtitles/eng/prog_index.m3u8"
//#EXT-X-MEDIA:TYPE=SUBTITLES,GROUP-ID="subs",NAME="Français",DEFAULT=YES,AUTOSELECT=YES,FORCED=NO,LANGUAGE="fr",URI="subtitles/fra/prog_index.m3u8"
		}
		else if ( line.beginsWith( EXT_X_STREAM_INF ) ) {
			if ( currLevl ) {
				throw InternalError( "RootManifest::decode(): "
						"Missing URL!" );
			}
			currLevl = parseLevel( line( EXT_X_STREAM_INF.getLength() ) );
			needLink = true;
		}
		else if ( line.beginsWith( EXT_X_I_FRAME_STREAM_INF ) ) {
//#EXT-X-I-FRAME-STREAM-INF:BANDWIDTH=669554,CODECS="avc1.4d401f",URI="gear5/iframe_index.m3u8"
		}
		else if ( needLink ) {
			currLevl->setUrl( parseURL( line, currLink ) );
			levlList += currLevl;
			currLevl.kill();
			needLink = false;
		}
		else {
			throw InternalError( "RootManifest::decode(): "
					"\"" + line + "\" syntax not supported!" );
		}

	}

	if ( levlList.isEmpty() ) {
		throw InternalError( "RootManifest::decode(): "
				"No levels found in " + currLink );
	}

}

// ============================================================================

HLS::LevelInfoPtr HLS::RootManifest::parseLevel(
	const	Buffer &	pLineData ) const {

	LevelInfoPtr	tempLevl = new LevelInfo;

	Uint32	pos = 0;
	Uint32	len = pLineData.getLength();

	while ( pos < len ) {

		Uint32	ksp = pos;			// Key start pos.
		Uint32	kep = pLineData.findFwd( '=', ksp );
							// Key end pos.

		Buffer	key = pLineData( ksp, kep - ksp );

		Uint32	vsp = kep + 1;		// Value start pos.
		Uint32	vep = vsp;			// Value end pos.

		while ( vep < len ) {
			if ( pLineData[ vep ] == ',' ) {
				break;
			}
			if ( pLineData[ vep ] == '"' ) {
				vep = pLineData.findFwd( '"', vep + 1 ) + 1;
				if ( vep >= len ) {
					throw InternalError( "RootManifest::parseLevel(): "
						"Missing closing '\"'!" );
				}
				if ( vep < len && pLineData[ vep ] != ',' ) {
					throw InternalError( "RootManifest::parseLevel(): "
						"Junk after closing '\"'!" );
				}
				break;
			}
			vep++;
		}

		Buffer	val = pLineData( vsp, vep - vsp );

		pos = vep + 1;

		if ( key == "PROGRAM-ID" ) {
			tempLevl->setProgramId( val.toUint64() );
		}
		else if ( key == "BANDWIDTH" ) {
			tempLevl->setBandwidth( val.toUint64() );
		}
		else if ( key == "RESOLUTION" ) {
			BufferArray r = BufferParser::tokenize( val, "x" );
			tempLevl->setResolution(
					r[ 0 ].toUint64(),
					r[ 1 ].toUint64() );
		}
		else if ( key == "CODECS" ) {
			tempLevl->setCodecs( val );
		}
//		else if ( key == "AUDIO" ) {
//			tempLevl->setAudioGroup( removeQuotes( val ) );
//		}
//		else if ( key == "SUBTITLES" ) {
//			tempLevl->setSubtitlesGroup( removeQuotes( val ) );
//		}

	}

	return tempLevl;

}

// ============================================================================

