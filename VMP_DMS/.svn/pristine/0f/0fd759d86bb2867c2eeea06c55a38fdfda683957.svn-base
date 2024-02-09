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
//	VMP.HLS.Manifest.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.Manifest.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, Manifest )

// ============================================================================

Buffer HLS::Manifest::parseURL(
	const	Buffer &	pPath,
	const	Buffer &	pBase ) {

	Buffer	path = pPath;
	Buffer	base = pBase;

	Buffer	prefix;
	Buffer	suffix;

	if ( path( 0, 7 ) == "http://"
	  || path( 0, 8 ) == "https://" ) {
		return path;
	}

	// Remove query...

	if ( base.findFwd( '?' ) < base.getLength() ) {
		base = base( 0, base.findFwd( '?' ) );
	}

	if ( path( 0, 1 ) == "/" ) {
		prefix = base( 0, base.findFwd( "//" ) + 2 );
		suffix = base( base.findFwd( "//" ) + 2 );
		return prefix + suffix( 0, suffix.findFwd( '/' ) ) + path;
	}

	return base( 0, base.findRev( '/' ) + 1 ) + path;

}

// ============================================================================

const Buffer HLS::Manifest::EXTM3U			= "#EXTM3U";

const Buffer HLS::Manifest::EXTINF			= "#EXTINF:";
const Buffer HLS::Manifest::EXT_X_ALLOW_CACHE		= "#EXT-X-ALLOW-CACHE:";
const Buffer HLS::Manifest::EXT_X_BYTERANGE		= "#EXT-X-BYTERANGE:";
const Buffer HLS::Manifest::EXT_X_ENDLIST		= "#EXT-X-ENDLIST";
const Buffer HLS::Manifest::EXT_X_MEDIA_SEQUENCE	= "#EXT-X-MEDIA-SEQUENCE:";
const Buffer HLS::Manifest::EXT_X_PLAYLIST_TYPE		= "#EXT-X-PLAYLIST-TYPE:";
const Buffer HLS::Manifest::EXT_X_TARGETDURATION	= "#EXT-X-TARGETDURATION:";
const Buffer HLS::Manifest::EXT_X_VERSION		= "#EXT-X-VERSION:";

const Buffer HLS::Manifest::EXT_X_STREAM_INF		= "#EXT-X-STREAM-INF:";
const Buffer HLS::Manifest::EXT_X_MEDIA			= "#EXT-X-MEDIA:";
const Buffer HLS::Manifest::EXT_X_I_FRAME_STREAM_INF	= "#EXT-X-I-FRAME-STREAM-INF:";

// ============================================================================

