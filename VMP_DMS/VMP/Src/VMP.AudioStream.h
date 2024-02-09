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
//	VMP.AudioStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AudioStream_H_
#define _VMP_AudioStream_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.AudioStreamPtr.h"

// ============================================================================

#include "BFC.Sound.FormatPtr.h"

#include "VMP.ElementaryStream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL AudioStream : public ElementaryStream {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new AudioStream.

	AudioStream(
	);

	/// \brief Creates a new AudioStream.

	AudioStream(
			BFC::Sound::FormatCPtr
					pFormat
	);

	/// \brief Copy constructor.

	AudioStream(
		const	AudioStream &	pOther
	);

	/// \brief Copy operator.

	AudioStream & operator = (
		const	AudioStream &	pOther
	);

	virtual StreamPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			StreamCPtr	pStream
	) const;

	void setFormat(
			BFC::Sound::FormatCPtr
					pFormat
	);

	BFC::Sound::FormatCPtr getFormat(
	) const {
		return blockFmt;
	}

	void setSampleRate(
		const	BFC::Uint32	pSmplRate
	) {
		sampRate = pSmplRate;
		updateFormat();
	}

	/// \brief Synonym of setSampRate().

	void setSampRate(
		const	BFC::Uint32	pSmplRate
	) {
		setSampleRate( pSmplRate );
	}

	BFC::Uint32 getSampleRate(
	) const {
		return sampRate;
	}

	/// \brief Synonym of getSampleRate().

	BFC::Uint32 getSampRate(
	) const {
		return getSampleRate();
	}

	void setSampleSize(
		const	BFC::Uint32	pSmplSize
	) {
		sampSize = pSmplSize;
		updateFormat();
	}

	/// \brief Synonym of setSampleSize().

	void setSampSize(
		const	BFC::Uint32	pSmplSize
	) {
		setSampleSize( pSmplSize );
	}

	BFC::Uint32 getSampleSize(
	) const {
		return sampSize;
	}

	/// \brief Synonym of getSampleSize().

	BFC::Uint32 getSampSize(
	) const {
		return getSampleSize();
	}

	void setNbrChannels(
		const	BFC::Uint32	pNbrChans
	) {
		channels = pNbrChans;
		updateFormat();
	}

	/// \brief Synonym of setNbrChannels().

	void setChannels(
		const	BFC::Uint32	pNbrChans
	) {
		setNbrChannels( pNbrChans );
	}

	BFC::Uint32 getNbrChannels(
	) const {
		return channels;
	}

	/// \brief Synonym of getNbrChannels().

	BFC::Uint32 getChannels(
	) const {
		return getNbrChannels();
	}

	void setCompression(
		const	AudioCompression &
					c
	) {
		compress = c;
	}

	AudioCompression getCompression(
	) const {
		return compress;
	}

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

	void updateFormat(
	);

private :

	BFC::Uint32		sampRate; // 44100, 48000, ...
	BFC::Uint32		sampSize; // 16 (bits/sample)
	BFC::Uint32		channels;
	BFC::Sound::FormatCPtr	blockFmt;
	AudioCompression	compress;

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_AudioStream_H_

// ============================================================================

