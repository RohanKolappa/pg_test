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
//	VMP.VideoStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_VideoStream_H_
#define _VMP_VideoStream_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.VideoStreamPtr.h"

// ============================================================================

#include "BFC.Base.Frac64.h"

#include "VMP.ElementaryStream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL VideoStream : public ElementaryStream {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new VideoStream.

	VideoStream(
	);

	/// \brief Copy constructor.

	VideoStream(
		const	VideoStream &	pOther
	);

	/// \brief Copy operator.

	VideoStream & operator = (
		const	VideoStream &	pOther
	);

	virtual StreamPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			StreamCPtr	pStream
	) const;

	const BFC::Frac64 & getFracRate(
	) const {
		return frmeRate;
	}

	void setFrameRate(
		const	BFC::Frac64 &	pFrameRate
	) {
		frmeRate = pFrameRate;
	}

	void setFrameRate(
		const	BFC::Uint32	i
	) {
		frmeRate.setNum( i );
//		frameRate = i;
	}

	BFC::Uint32 getFrameRate(
	) const {
		return ( BFC::Uint32 )frmeRate.getNum();
//		return frameRate;
	}

	void setFrameScale(
		const	BFC::Uint32	i
	) {
		frmeRate.setDen( i );
//		frameScale = i;
	}

	BFC::Uint32 getFrameScale(
	) const {
		return ( BFC::Uint32 )frmeRate.getDen();
//		return frameScale;
	}

	void setWidth(
		const	BFC::Uint32	i
	) {
		width = i;
	}

	BFC::Uint32 getWidth(
	) const {
		return width;
	}

	void setHeight(
		const	BFC::Uint32	i
	) {
		height = i;
	}

	BFC::Uint32 getHeight(
	) const {
		return height;
	}

	void setFlipped(
		const	BFC::Bool	b
	) {
		flipped = b;
	}

	BFC::Bool isFlipped(
	) const {
		return flipped;
	}

	void setCompression(
		const	VideoCompression &
					c
	) {
		compression = c;
	}

	VideoCompression getCompression(
	) const {
		return compression;
	}

	void setFormat(
		const	BFC::Image::PictureFormat &
					f
	) {
		format = f;
	}

	BFC::Image::PictureFormat getFormat(
	) const {
		return format;
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

private :

	BFC::Frac64			frmeRate;
	BFC::Uint32			width;
	BFC::Uint32			height;
	BFC::Bool			flipped;
	VideoCompression		compression;
	BFC::Image::PictureFormat	format;

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_VideoStream_H_

// ============================================================================

