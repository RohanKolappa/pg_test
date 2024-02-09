// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Image".
// 
// "BFC::Image" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Image" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Image"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Image.Converter_RGB_RYUV.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Converter_RGB_RYUV_H_
#define _BFC_Image_Converter_RGB_RYUV_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.RawConverter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class BFC_Image_DLL RawRYUVConverter : public RawConverter {

public :

	RawRYUVConverter(
		const	PictureFormat &	iFormat,
		const	PictureFormat &	oFormat
	) :
		RawConverter( iFormat, oFormat ) {
	}

protected :

	static void rgb2ryuv(
		const	Uchar		r,
		const	Uchar		g,
		const	Uchar		b,
			Uint16 &	y,
			Uint16 &	u,
			Uint16 &	v
	) {
		y = ( Uint16 )( ( ( Uint16 )r + ( ( Uint16 )g << 1 ) + ( Uint16 )b ) >> 2 );
		u = ( Uint16 )( ( ( Uint16 )r - ( Uint16 )g ) );
		v = ( Uint16 )( ( ( Uint16 )b - ( Uint16 )g ) );
	}

	static void ryuv2rgb(
		const	Uint16		y,
		const	Uint16		u,
		const	Uint16		v,
			Uchar &		r,
			Uchar &		g,
			Uchar &		b
	) {
		g = y - ( ( u + v ) >> 2 );
		r = ( Uchar )u + g;
		b = ( Uchar )v + g;
	}

};

// ============================================================================

/// \brief RGB <---> RYUV444P converter.

class BFC_Image_DLL Converter_RGB_RYUV444P : public RawRYUVConverter {

public :

	Converter_RGB_RYUV444P(
	);

	virtual void engineForward(
			ReaderArray &	pIPics,
			WriterArray &	pOPics
	);

	virtual void engineBackward(
			ReaderArray &	pIPics,
			WriterArray &	pOPics
	);

};

// ============================================================================

/// \brief RGBA <---> RYUVA444P converter.

class BFC_Image_DLL Converter_RGBA_RYUVA444P : public RawRYUVConverter {

public :

	Converter_RGBA_RYUVA444P(
	);

	virtual void engineForward(
			ReaderArray &	pIPics,
			WriterArray &	pOPics
	);

	virtual void engineBackward(
			ReaderArray &	pIPics,
			WriterArray &	pOPics
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Converter_RGB_RYUV_H_

// ============================================================================

