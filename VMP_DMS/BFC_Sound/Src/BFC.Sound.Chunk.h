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
//	BFC.Sound.Chunk.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_Chunk_H_
#define _BFC_Sound_Chunk_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.ChunkPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Debug.DObject.h"

#include "BFC.Sound.ChunkFormatPtr.h"
#include "BFC.Sound.Track.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief Represents a chunk of audio material, made up of a number of
///	separate audio Track objects.
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL Chunk : public DObject {

public :

	/// \brief Creates a new Chunk.

	Chunk(
			ChunkFormatCPtr	pFormat,
		const	Uint32		pLength = 0
	);

	/// \brief Copy constructor.

	Chunk(
		const	Chunk &		pOther
	);

	/// \brief Copy operator.

	Chunk & operator = (
		const	Chunk &		pOther
	);

	void kill(
	);

	Bool operator == (
		const	Chunk &		pOther
	) const;

	Bool operator != (
		const	Chunk &		pOther
	) const;

	Bool isSimilarTo(
		const	Chunk &		pOther
	) const;

	Bool isEmpty(
	) const;

	Uint32 getLength(
	) const;

	ChunkFormatCPtr getFormat(
	) const;

	Uint32 getNbrTracks(
	) const;

	void setTrack(
		const	Uint32		pIndex,
			TrackCPtr	pTrack
	);

	TrackCPtr getTrack(
		const	Uint32		pIndex
	) const;

//	ChunkPtr doInterlace(
//	) const;
//
//	ChunkPtr unInterlace(
//	) const;

	Bool isInterlaced(
	) const;

	ChunkCPtr getUnInterlaced(
	) const;

	ChunkCPtr getInterlaced(
	) const;

protected :

	/// \brief Throws an InvalidArgument exception if any incoherency
	///	is found between the member variables.

	void checkValidness(
	);

	/// \brief Throws a CantConvert exception if this format is a user
	///	defined format, of if its the predefined Unknown format.

	void testConvertible(
	) const;

private :

	Uint32		chunkL;
	ChunkFormatCPtr	chunkF;
	TrackCArray	chunkT;

	/// \brief Forbidden default constructor.

	Chunk(
	);

};

// ============================================================================

typedef CompArray< ChunkPtr > ChunkArray;

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_Chunk_H_

// ============================================================================

#endif // 0

// ============================================================================

