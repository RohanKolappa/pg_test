// ============================================================================

#if defined( SOUND_OLD_API )

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
//	BFC.Sound.ChunkFormat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_ChunkFormat_H_
#define _BFC_Sound_ChunkFormat_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.ChunkFormatPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Frac64.h"
#include "BFC.Base.SObject.h"

#include "BFC.Sound.TrackFormatPtr.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL ChunkFormat : virtual public SObject {

public :

	/// \brief Creates a new ChunkFormat.

	ChunkFormat(
		const	Frac64 &	pRate
	);

	/// \brief Creates a new ChunkFormat.

	ChunkFormat(
		const	Uint32		pNbrChans,
		const	Uint32		pSampRate,
		const	Uint32		pSampSize,
		const	Bool		pInterlaced
	);

private :

	/// \brief Copy constructor.

	ChunkFormat(
		const	ChunkFormat &	pOther
	);

	/// \brief Copy operator.

	ChunkFormat & operator = (
		const	ChunkFormat &	pOther
	);

public :

	void kill(
	) {
		tbl.kill();
		nbr = 0;
	}

	Frac64 getRate(
	) const {
		return rte;
	}

	void setRate(
		const	Frac64 &	pRate
	) {
		rte = pRate;
	}

//	Bool operator == (
//		const	ChunkFormat &	pOther
//	) const;
//
//	Bool operator != (
//		const	ChunkFormat &	pOther
//	) const;

//	Bool canRepresent(
//		const	Uint32		pChunkLength
//	) const;

	Uint32 getNbrTracks(
	) const {
		return tbl.getSize();
	}

	TrackFormatCPtr getTrack(
		const	Uint32		pTrackIndex
	) const {
		return tbl[ pTrackIndex ];
	}

	Uint32 getNbrSamples(
	) const {
		return nbr;
	}

//	Uint32 getLengthPerTrack(
//		const	Uint32		pTrackIndex,
//		const	Uint32		pChunkLength
//	) const;

	void addTrack(
			TrackFormatCPtr	pTrackFormat,
		const	Uint32		pNbrTracks = 1
	);

//	Buffer getName(
//	) const;

	Bool isInterlaced(
	) const;

	ChunkFormatCPtr getUnInterlaced(
	) const;

	Buffer toBuffer(
	) const;

protected :

private :

	Frac64		rte;	///< Nominal sampling rate.
	Array< TrackFormatCPtr >
			tbl;	///< Tracks.
	Uint32		nbr;	///< # samples (all tracks).

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_ChunkFormat_H_

// ============================================================================

#endif // 0

// ============================================================================

