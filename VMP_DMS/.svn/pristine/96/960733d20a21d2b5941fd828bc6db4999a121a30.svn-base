// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
//
// Filename:
//	VMP.M2S.TimeStamp.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _VMP_M2S_TimeStamp_h_
#define _VMP_M2S_TimeStamp_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TimeStampPtr.h"
// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Time.Clock.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief Represents an MPEG-2 timestamp.
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL TimeStamp : virtual public BFC::SObject {

public :

	TimeStamp(
	) :
		val	( 0 ),
		ext	( 0 ) {
	}

	TimeStamp(
		const	TimeStamp &	pOther
	) :
		BFC::SObject	(),
		val	( pOther.val ),
		ext	( pOther.ext ) {
	}

	TimeStamp & operator = (
		const	TimeStamp &	pOther
	) {
		val = pOther.val;
		ext = pOther.ext;
		return *this;
	}

	TimeStamp operator - (
		const	TimeStamp &	pOther
	) const;

	void decode_33_6_9(
		const	BFC::Uchar *	ptr
	);

	void decode_4_3_1_15_1_15_1(
		const	BFC::Uchar *	ptr
	);

	void encode_4_3_1_15_1_15_1(
			BFC::Uchar *	ptr
	);

	void decode_2_3_1_15_1_15_1_9_1(
		const	BFC::Uchar *	ptr
	);

	BFC::Buffer toBuffer(
	) const;

	void fromClock(
		const	BFC::Time::Clock &
					pClock
	);

	BFC::Time::Clock toClock(
	) const;

	BFC::Uint64 getValue(
	) const {
		return val;
	}

	BFC::Uint64 getExtension(
	) const {
		return ext;
	}

protected :

private :

	BFC::Uint64	val;	// 33 bits
	BFC::Uint32	ext;	// 9 bits

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TimeStamp_h_

// ============================================================================

