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
//	VMP.HLS.TimeStamp.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_TimeStamp_H_
#define _VMP_HLS_TimeStamp_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.TimeStampPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL TimeStamp : virtual public BFC::SObject {

public :

	static const BFC::Uint64 Mod_2_32 = ( CU64( 1 ) << 32 );
	static const BFC::Uint64 Mod_2_33 = ( CU64( 1 ) << 33 );

	/// \brief Creates a new TimeStamp.

	TimeStamp(
		const	BFC::Int64	pTimeStmp,
		const	BFC::Uint64	pTimeScle,
		const	BFC::Uint64	pRoundMod
	) :

		timeStmp	( pTimeStmp ),
		timeScle	( pTimeScle ),
		roundMod	( pRoundMod ) {

	}

	void ensureCloseTo(
			TimeStampCPtr	pOther
	);

	static TimeStampCPtr add(
			TimeStampCPtr	p1,
			TimeStampCPtr	p2
	);

	static TimeStampCPtr sub(
			TimeStampCPtr	p1,
			TimeStampCPtr	p2
	);

	TimeStampCPtr addNumber(
		const	BFC::Double	p2
	) const;

	TimeStampCPtr subNumber(
		const	BFC::Double	p2
	) const;

	static TimeStampCPtr min(
			TimeStampCPtr	p1,
			TimeStampCPtr	p2
	);

	static TimeStampCPtr max(
			TimeStampCPtr	p1,
			TimeStampCPtr	p2
	);

	BFC::Int64 toMSecs(
	) const;

	BFC::Double toDouble(
	) const;

	BFC::Buffer toString(
	) const;

	static BFC::Uint64 getGCD(
		const	BFC::Uint64	a,
		const	BFC::Uint64	b
	);

	BFC::Int64	timeStmp;
	BFC::Uint64	timeScle;
	BFC::Uint64	roundMod;

protected :

private :

	/// \brief Forbidden default constructor.

	TimeStamp(
	);

	/// \brief Forbidden copy constructor.

	TimeStamp(
		const	TimeStamp &
	);

	/// \brief Forbidden copy operator.

	TimeStamp & operator = (
		const	TimeStamp &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_TimeStamp_H_

// ============================================================================

