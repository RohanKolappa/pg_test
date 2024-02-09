// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::DMS".
// 
// "VMP::DMS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::DMS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::DMS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.DMS.RTP2HLS.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_DMS_RTP2HLS_H_
#define _VMP_DMS_RTP2HLS_H_

// ============================================================================

namespace VMP {
namespace DMS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_DMS

class RTP2HLS {

public :

	/// \brief Sets the logging level to \a pLogLevel (0:quiet, 4:debug).

	static void setLogLevel(
		const	unsigned int	pLogLevel
	);

	/// \brief Creates a new RTP2HLS.

	RTP2HLS(
	);

	/// \brief Destroys this object.

	virtual ~RTP2HLS(
	);

	/// \brief Initializes this object.
	///
	/// \param pISDPFile
	///	Reference SDP file (e.g. "/tmp/media.sdp").
	///
	/// \param pMultSSRC
	///	Allow mutating SSRC's on the input RTP streams.
	///
	/// \param pOutptDir
	///	Output directory (e.g. "/var/www/hls/").
	///
	/// \param pSegmPrfx
	///	Segment file name prefix (e.g. "segment-").
	///
	/// \param pSegmDrtn
	///	Segment duration (in secs) (e.g. 2, 1.5, ...).
	///
	/// \param pM3u8Name
	///	Playlist file name (e.g. "output.m3u8").
	///
	/// \param pHttpPrfx
	///	Base URL (e.g. "http://IPADDRESS/media/").
	///
	/// \param pPvrLngth
	///	PVR length (in secs) (e.g. 10).
	///
	/// \param pSncWrite
	///	Use synchronous disk writes ?
	///
	/// \param pFakeClse

	void init(
		const	char *		pISDPFile,
		const	bool		pMultSSRC,
		const	char *		pOutptDir,
		const	char *		pSegmPrfx,
		const	double		pSegmDrtn,
		const	char *		pM3u8Name,
		const	char *		pHttpPrfx,
		const	unsigned int	pPvrLngth,
		const	bool		pSncWrite,
		const	bool		pFakeClse
	);

	/// \brief Starts playing.
	///
	/// \throws BFC::NotInitialized
	///
	/// \throws BFC::InternalError

	void play(
	);

	void pause(
		const	unsigned long long
					pWallClck
	);

	void resume(
		const	unsigned long long
					pWallClck
	);

	void stop(
	);

	void putVideoFrame(
		const	char *		pPacktPtr,
		const	unsigned long	pPacktLen,
		const	unsigned long long
					pWallClck
	);

	void putAudioFrame(
		const	char *		pPacktPtr,
		const	unsigned long	pPacktLen,
		const	unsigned long long
					pWallClck
	);

	unsigned long getVideoFifoSize(
	) const;

	unsigned long getAudioFifoSize(
	) const;

	void getDiskStats(
			unsigned long long &
					pNmbrWrtn,
			unsigned long long &
					pNmbrDltd
	);

protected :

	unsigned long long adjustClock(
		const	unsigned long long
					pWallClck
	);

private :

	class Config;

	Config *	cfg;

	class Context;

	Context *	ctx;

	/// \brief Forbidden copy constructor.

	RTP2HLS(
		const	RTP2HLS &
	);

	/// \brief Forbidden copy operator.

	RTP2HLS & operator = (
		const	RTP2HLS &
	);

};

// ============================================================================

} // namespace DMS
} // namespace VMP

// ============================================================================

#endif // _VMP_DMS_RTP2HLS_H_

// ============================================================================

