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
//	BFC.Sound.SampleFormat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_SampleFormat_H_
#define _BFC_Sound_SampleFormat_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.SampleFormatPtr.h"

// ============================================================================

#include "BFC.Base.Bool3.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL SampleFormat : virtual public SObject {

public :

	static SampleFormatCPtr U8;
	static SampleFormatCPtr S8;
	static SampleFormatCPtr U16LE;
	static SampleFormatCPtr S16LE;
	static SampleFormatCPtr U16BE;
	static SampleFormatCPtr S16BE;
	static SampleFormatCPtr U24LE;
	static SampleFormatCPtr S24LE;
	static SampleFormatCPtr U24BE;
	static SampleFormatCPtr S24BE;
	static SampleFormatCPtr U32LE;
	static SampleFormatCPtr S32LE;
	static SampleFormatCPtr U32BE;
	static SampleFormatCPtr S32BE;
	static SampleFormatCPtr F32LE;
	static SampleFormatCPtr F32BE;

	static SampleFormatCPtr getLEInteger(
		const	Uint32		pSampSize,
		const	Bool		pSigned = true
	);

	/// \brief Creates a new SampleFormat.

	SampleFormat(
		const	UUId		pid,
		const	Bool		pin,
		const	Bool3		ple,
		const	Bool3		psg,
		const	Uint32		pln,
		const	Buffer &	pnm
	) :
		id	( pid ),
		in	( pin ),
		le	( ple ),
		sg	( psg ),
		ln	( pln ),
		nm	( pnm ) {
	}

	Bool operator == (
		const	SampleFormat &	pOther
	) const {
		return ( id == pOther.id );
	}

	Bool operator != (
		const	SampleFormat &	pOther
	) const {
		return ( id != pOther.id );
	}

	const UUId & getUUId(
	) const {
		return id;
	}

	Bool isInteger(
	) const {
		return in;
	}

	Bool isFloating(
	) const {
		return ( ! in );
	}

	Bool isLittleEndian(
	) const {
		return ( le == Yes );
	}

	Bool isBigEndian(
	) const {
		return ( le == No );
	}

	Bool leIsCompatibleWith(
		const	Bool3		pChoice
	) const {
		return ( le.isCompatibleWith( pChoice ) );
	}

	Bool isSigned(
	) const {
		return ( sg == Yes );
	}

	Bool isUnsigned(
	) const {
		return ( sg == No );
	}

	Bool sgIsCompatibleWith(
		const	Bool3		pChoice
	) const {
		return ( sg.isCompatibleWith( pChoice ) );
	}

	Uint32 getNbrBits(
	) const {
		return ln;
	}

	const Buffer & getName(
	) const {
		return nm;
	}

protected :

private :

	UUId		id;	///< UUId for this manifest.
	Bool		in;	///< Integer (true) or Float.
	Bool3		le;	///< Little Endian (true) or Big Endian.
	Bool3		sg;	///< Signed (true) or Unsigned.
	Uint32		ln;	///< Number of bits.
	Buffer		nm;	///< Name.

	/// \brief Forbidden default constructor.

	SampleFormat(
	);

	/// \brief Forbidden copy constructor.

	SampleFormat(
		const	SampleFormat &
	);

	/// \brief Forbidden copy operator.

	SampleFormat & operator = (
		const	SampleFormat &
	);

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_SampleFormat_H_

// ============================================================================

#endif // 0

// ============================================================================

