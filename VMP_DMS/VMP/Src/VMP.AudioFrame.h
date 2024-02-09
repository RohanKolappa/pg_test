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
//	VMP.AudioFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AudioFrame_H_
#define _VMP_AudioFrame_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.AudioFramePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Sound.BlockPtr.h"
#include "BFC.Sound.FormatPtr.h"

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL AudioFrame : public Frame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new AudioFrame.

	AudioFrame(
	) :
		BFC::DL::Object	( getClassType() )
	{
	}

	AudioFrame(
			BFC::Sound::BlockCPtr
					pBlock
	) :
		BFC::DL::Object	( getClassType() ),
		block		( pBlock )
	{
	}

	AudioFrame(
		const	BFC::Buffer &	pPayload,
			BFC::Sound::FormatCPtr
					pFormat
	) :
		BFC::DL::Object	( getClassType() )
	{
		setData( pPayload, pFormat );
	}

	AudioFrame(
			FrameCPtr	pTmplFrme,
		const	BFC::Buffer &	pPayload,
			BFC::Sound::FormatCPtr
					pFormat
	) :
		BFC::DL::Object	( getClassType() ),
		Frame		( pTmplFrme )
	{
		setData( pPayload, pFormat );
	}

	/// \brief Copy constructor.

	AudioFrame(
		const	AudioFrame &	pOther
	) :
		BFC::SObject		(),
		BFC::Cloneable		(),
		BFC::DObject		(),
		BFC::DL::Object		( pOther ),
		BFC::IO::Serializable	(),
		Frame			( pOther ),
		block			( pOther.block )
	{
	}

	virtual BFC::SPtr clone(
	) const;

	// New API

	void setBlock(
			BFC::Sound::BlockCPtr
					pBlock
	) {
		block = pBlock;
	}

	BFC::Sound::BlockCPtr getBlock(
	) const {
		return block;
	}

	// Old API

	void setData(
		const	BFC::Buffer &	pPayload,
			BFC::Sound::FormatCPtr
					pFormat
	);

	BFC::Buffer getData(
	) const;

	virtual BFC::Uint32 getLength(
	) const;

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	BFC::Sound::BlockCPtr	block;

	/// \brief Forbidden copy operator.

	AudioFrame & operator = (
		const	AudioFrame &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_AudioFrame_H_

// ============================================================================

