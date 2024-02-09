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
//	BFC.Image.Converter_YUV420P_JYUV400.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Converter_YUV420P_JYUV400_H_
#define _BFC_Image_Converter_YUV420P_JYUV400_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.RawConverter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief YUV <---> JYUV converter.
///
/// This converter uses the following transform:
/// 
/// Y[YUV] = (220*Y[JYUV]+128)/256+16
/// U = 128
/// V = 128
///
/// Y[JYUV] = (298*(Y[YUV]-16)+128)/256 (+ clipping)


class BFC_Image_DLL Converter_YUV420P_JYUV400 : public RawConverter {

public :

	Converter_YUV420P_JYUV400(
	);

	virtual ~Converter_YUV420P_JYUV400(
	);

	virtual void engineForward(
			ReaderArray	& pIPics,
			WriterArray	& pOPics
	);

	virtual void engineBackward(
			ReaderArray	& pIPics,
			WriterArray	& pOPics
	);

protected :

private :

	// others...

	Converter_YUV420P_JYUV400(
		const	Converter_YUV420P_JYUV400&
	);

	Converter_YUV420P_JYUV400& operator = (
		const	Converter_YUV420P_JYUV400&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Converter_YUV420P_JYUV400_H_

// ============================================================================

