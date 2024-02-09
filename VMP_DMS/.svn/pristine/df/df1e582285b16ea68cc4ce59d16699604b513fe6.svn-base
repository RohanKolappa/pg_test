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
//	VMP.HLS.ManifestDecoder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.LeafManifest.h"
#include "VMP.HLS.ManifestDecoder.h"
#include "VMP.HLS.RootManifest.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, ManifestDecoder )

// ============================================================================

HLS::ManifestDecoder::ManifestDecoder() {

	setObjectName( "VMP.HLS.ManifestDecoder" );

}

// ============================================================================

HLS::ManifestPtr HLS::ManifestDecoder::decode(
	const	Buffer &	pLink,
	const	Buffer &	pData ) {

	msgDbg( "decode" );

	if ( ! pData.beginsWith( Manifest::EXTM3U ) ) {
		throw InvalidArgument( "Not an .m3u8 file!" );
	}

	ManifestPtr	res;

	if ( pData.contains( Manifest::EXTINF ) ) {
		res = new LeafManifest;
	}
	else if ( pData.contains( Manifest::EXT_X_STREAM_INF ) ) {
		res = new RootManifest;
	}
	else {
		throw InvalidArgument( "Unsupported .m3u8 syntax!" );
	}

	res->decode( pLink, pData );

	return res;

}

// ============================================================================

