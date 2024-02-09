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
//	BFC.Sound.Track.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_Track_H_
#define _BFC_Sound_Track_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.TrackPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Debug.DObject.h"

#include "BFC.Mem.Block.h"
#include "BFC.Mem.Reader.h"
#include "BFC.Mem.Writer.h"

#include "BFC.Sound.TrackFormatPtr.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL Track : virtual public DObject {

public :

	/// \brief Creates a new Track.

	Track(
	);

	Track(
			TrackFormatCPtr	pFormat
	);

	Track(
		const	TrackCArray &	pToInterlace
	);

private :

	/// \brief Copy constructor.

	Track(
		const	Track &		pOther
	);

	/// \brief Copy operator.

	Track & operator = (
		const	Track &		pOther
	);

public :

	void kill(
	) {
		trackL = 0;
		trackF.kill();
		trackD.kill();
	}

//	Bool operator == (
//		const	Track &		pOther
//	) const;
//
//	Bool operator != (
//		const	Track &		pOther
//	) const;

	Bool isEmpty(
	) const {
		return ( trackD.getLength() == 0 );
	}

	Uint32 getLength(
	) const {
		return trackL;
	}

	TrackFormatCPtr getFormat(
	) const {
		return trackF;
	}

	Buffer getData(
	) const {
		return trackD.toBuffer();
	}

//	void setFormat(
//			TrackFormatCPtr	pFormat
//	) {
//		trackF = pFormat;
//	}

	void setData(
		const	Buffer &	pData
	);

	void allocate(
		const	Uint32		pLength
	);

	TrackCPtr getUnInterlaced(
		const	Uint32		pIndex
	) const;

	Mem::Reader getReader(
	) const;

	Mem::Writer getWriter(
	);

protected :

private :

	Uint32		trackL;
	TrackFormatCPtr	trackF;
	Mem::Block	trackD;

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_Track_H_

// ============================================================================

#endif // 0

// ============================================================================

