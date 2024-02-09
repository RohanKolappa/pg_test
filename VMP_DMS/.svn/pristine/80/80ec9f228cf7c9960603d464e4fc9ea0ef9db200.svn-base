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
//	VMP.HLS.ServerConfig.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_ServerConfig_H_
#define _VMP_HLS_ServerConfig_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.ServerConfigPtr.h"

// ============================================================================

#include "BFC.Base.Frac64.h"

#include "BFC.Debug.DObject.h"

#include "BFC.TL.BufferArray.h"

#include "BFC.VFS.FSPath.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL ServerConfig : virtual public BFC::DObject {

public :

	/// \brief Creates a new ServerConfig.

	ServerConfig(
	);

	void loadArgs(
		const	BFC::BufferArray &
					pCmndLine
	);

	BFC::Buffer getUsage(
	) const;

	BFC::Buffer		progName;
	BFC::Buffer		pidFName;	///< PID file name (e.g. "streamer.pid").

	BFC::Buffer		iSDPFile;	///< Reference SDP file.
	BFC::Buffer		iVidAddr;	///< RTP video addr (e.g. "239.255.0.10").
	BFC::Uint16		iVidPort;	///< RTP video port (e.g. 50000).
	BFC::Buffer		iAudAddr;	///< RTP audio addr (e.g. "239.255.0.10").
	BFC::Uint16		iAudPort;	///< RTP audio port (e.g. 50002).
	BFC::Bool		vrblSSRC;	///< Allow mutating SSRC in input ?

	BFC::BufferArray	outpFrmt;	///< Output formats.
	BFC::FSPath		outptDir;	///< Output directory (e.g. "/var/www/hls/").
	BFC::Buffer		segmPrfx;	///< Segment file name prefix (e.g. "segment-").
	BFC::Frac64		segmDrtn;	///< Segment duration (in seconds).
	BFC::Buffer		m3u8Name;	///< Playlist file name (e.g. "output.m3u8").
	BFC::Buffer		httpPrfx;	///< Base URL (e.g. "http://IPADDRESS/media/").
	BFC::Uint32		pvrLngth;	///< PVR length (in secs) (e.g. 600).
	BFC::Bool		sncWrite;	///< Sync disk writes ?
	BFC::Bool		fakeClse;	///< Write complete playlists (with ENDLIST flag) ?

protected :

private :

	/// \brief Forbidden copy constructor.

	ServerConfig(
		const	ServerConfig &
	);

	/// \brief Forbidden copy operator.

	ServerConfig & operator = (
		const	ServerConfig &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_ServerConfig_H_

// ============================================================================

