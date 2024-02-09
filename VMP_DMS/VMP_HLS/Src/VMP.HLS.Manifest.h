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
//	VMP.HLS.Manifest.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_Manifest_H_
#define _VMP_HLS_Manifest_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.ManifestPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL Manifest : virtual public BFC::DObject {

public :

	static const BFC::Buffer EXTM3U;			// "#EXTM3U";

	static const BFC::Buffer EXTINF;			// "#EXTINF:";
	static const BFC::Buffer EXT_X_ALLOW_CACHE;		// "#EXT-X-ALLOW-CACHE:";
	static const BFC::Buffer EXT_X_BYTERANGE;		// "#EXT-X-BYTERANGE:";
	static const BFC::Buffer EXT_X_ENDLIST;			// "#EXT-X-ENDLIST";
	static const BFC::Buffer EXT_X_MEDIA_SEQUENCE;		// "#EXT-X-MEDIA-SEQUENCE:";
	static const BFC::Buffer EXT_X_PLAYLIST_TYPE;		// "#EXT-X-PLAYLIST-TYPE:";
	static const BFC::Buffer EXT_X_TARGETDURATION;		// "#EXT-X-TARGETDURATION:";
	static const BFC::Buffer EXT_X_VERSION;			// "#EXT-X-VERSION:";

	static const BFC::Buffer EXT_X_STREAM_INF;		// "#EXT-X-STREAM-INF:";
	static const BFC::Buffer EXT_X_MEDIA;			// "#EXT-X-MEDIA:";
	static const BFC::Buffer EXT_X_I_FRAME_STREAM_INF;	// "#EXT-X-I-FRAME-STREAM-INF:";

	virtual void decode(
		const	BFC::Buffer &	pLink,
		const	BFC::Buffer &	pData
	) = 0;

protected :

	static BFC::Buffer parseURL(
		const	BFC::Buffer &	path,
		const	BFC::Buffer &	base
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_Manifest_H_

// ============================================================================

