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
//	BFC.Sound.Format.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_Format_H_
#define _BFC_Sound_Format_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.FormatPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL Format : virtual public SObject {

public :

	enum Type {

		S1 = 0,
		S2,
		S3,
		S4,
		S5,
		S6,
		S7,
		S8,

		S9LE,
		S10LE,
		S11LE,
		S12LE,
		S13LE,
		S14LE,
		S15LE,
		S16LE,

		S17LE,
		S18LE,
		S19LE,
		S20LE,
		S21LE,
		S22LE,
		S23LE,
		S24LE,

		S25LE,
		S26LE,
		S27LE,
		S28LE,
		S29LE,
		S30LE,
		S31LE,
		S32LE,

		F32LE,

		Unknown,

		MinSigned	= S1,
		MaxSigned	= S32LE + 1

	};

	/// \brief Creates a new Format.

	Format(
		const	Uint32		pNbrChans,
		const	Uint32		pSampRate,
		const	Type		pSampType
	);

	/// \brief Creates a new Format.

	Format(
		const	Uint32		pNbrChans,
		const	Uint32		pSampRate,
		const	Uint32		pSampSize
	);

	/// \brief Copy constructor.

	Format(
		const	Format &	pOther
	);

	/// \brief Copy operator.

	Format & operator = (
		const	Format &	pOther
	);

	Bool operator == (
		const	Format &	pOther
	) const;

	Bool operator != (
		const	Format &	pOther
	) const;

	Uint32 getNbrChannels(
	) const {
		return chanName.getSize();
	}

	Uint32 getSampleRate(
	) const {
		return sampRate;
	}

	void setSampleRate(
		const	Uint32		pRate
	) {
		sampRate = pRate;
	}

	Type getSampleType(
	) const {
		return sampType;
	}

	void setSampleType(
		const	Type		pType
	) {
		sampType = pType;
	}

	Uint32 getSampleSize(
		const	Uint32		pInBytes = false
	) const;

	Uint32 getUnusedSize(
	) const;

	const Buffer & getSampleName(
	) const;

	Uint32 getTotalSize(
		const	Uint32		pInBytes = false
	) const {
		return ( getSampleSize( pInBytes ) * getNbrChannels() );
	}

	const Buffer & getChannelName(
		const	Uint32		pIndex
	) const {
		return ( chanName[ pIndex ] );
	}

	void setChannelName(
		const	Uint32		pIndex,
		const	Buffer &	pChanName
	) {
		chanName[ pIndex ] = pChanName;
	}

	Buffer toBuffer(
	) const;

	static Type findSampleType(
		const	Uint32		pSampSize,
		const	Bool		pIsSigned = true
	);

	static Type findSampleType(
		const	Buffer &	pTypeName
	);

	static const Buffer & getTypeName(
		const	Type		pSmplType
	);

protected :

private :

	Uint32		sampRate;	///< Nominal sampling rate.
	Type		sampType;
	BufferArray	chanName;

	static const struct Entry {
		Type		smplType;
		const Buffer	typeName;
	}		ConvTble[];

	/// \brief Forbidden default constructor.

	Format(
	);

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_Format_H_

// ============================================================================

