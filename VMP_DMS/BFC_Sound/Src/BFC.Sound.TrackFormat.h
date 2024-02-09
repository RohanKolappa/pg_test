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
//	BFC.Sound.TrackFormat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_TrackFormat_H_
#define _BFC_Sound_TrackFormat_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.TrackFormatPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.Sound.SampleFormatPtr.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL TrackFormat : virtual public SObject {

public :

	/// \brief Creates a new TrackFormat.

	TrackFormat(
	) {
	}

	/// \brief Creates a new TrackFormat.

	TrackFormat(
		const	Uint32		pSampSize,
		const	Uint32		pNbrChans
	);

	/// \brief Creates a new TrackFormat.

	TrackFormat(
			SampleFormatCPtr
					pSampleFormat,
		const	Buffer &	pSampleName
	) {
		addSample( pSampleFormat, pSampleName );
	}

//	Bool operator == (
//		const	TrackFormat &	pOther
//	) const {
//		return ( id == pOther.id );
//	}
//
//	Bool operator != (
//		const	TrackFormat &	pOther
//	) const {
//		return ( id != pOther.id );
//	}

	Uint32 getNbrSamples(
	) const {
		return tbl.getSize();
	}

	const Buffer & getName(
		const	Uint32		pSampleIndex
	) const {
		return tbl[ pSampleIndex ]->nme;
	}

	SampleFormatCPtr getSample(
		const	Uint32		pSampleIndex
	) const {
		return tbl[ pSampleIndex ]->fmt;
	}

	Uint32 getBitOffset(
		const	Uint32		pSampleIndex
	) const {
		return tbl[ pSampleIndex ]->off;
	}

	Uint32 getBitLength(
		const	Uint32		pSampleIndex
	) const {
		return tbl[ pSampleIndex ]->len;
	}

	Uint32 getTotalLength(
	) const;

	Bool isInterlaced(
	) const;

	void addSample(
			SampleFormatCPtr
					pFormat,
		const	Buffer &	pName
	);

	Buffer toBuffer(
	) const;

protected :

private :

	BFC_PTR_DECL( BFC_Sound_DLL, Entry )

	class BFC_Sound_DLL Entry : virtual public SObject {
	public :
		Entry(
				SampleFormatCPtr
						pFormat,
			const	Buffer &	pName,
			const	Uint32		pOffset,
			const	Uint32		pLength
		) :
			fmt	( pFormat ),
			nme	( pName ),
			off	( pOffset ),
			len	( pLength )
		{
		}
		SampleFormatCPtr	fmt;	///< Sample format.
		Buffer			nme;	///< Sample name.
		Uint32			off;	///< Sample position (in bits).
		Uint32			len;	///< Sample length.
	};

	Array< EntryPtr >	tbl;

	/// \brief Forbidden copy constructor.

	TrackFormat(
		const	TrackFormat &
	);

	/// \brief Forbidden copy operator.

	TrackFormat & operator = (
		const	TrackFormat &
	);

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_TrackFormat_H_

// ============================================================================

#endif // 0

// ============================================================================

