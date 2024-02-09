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
//	BFC.Image.Converter_RGB_JYUV444P.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Converter_RGB_JYUV444P_H_
#define _BFC_Image_Converter_RGB_JYUV444P_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.RawConverter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief RGB <---> JYUV converter.
///
/// This converter uses the following transform:
/// 
/// \f[
/// \left(
/// \begin{array}{c} Y \\ Cr \\ Cb \end{array}
/// \right)
/// =
/// \frac{1}{256}
/// .
/// \left(
/// \left(
/// \begin{array}{ccc}
/// 77 & 150 & 29 \\ -43 & -85 & 128 \\ 128 & -107 & -21
/// \end{array}
/// \right)
/// .
/// \left(
/// \begin{array}{c} R \\ G \\ B \end{array}
/// \right)
/// + 
/// \left(
/// \begin{array}{c} 128 \\ 128 \\ 128 \end{array}
/// \right)
/// \right)
/// +
/// \left(
/// \begin{array}{c}     \\ 128 \\ 128 \end{array}
/// \right)
/// \f]
///
/// and its counterpart:
///
/// \f[
/// \left(
/// \begin{array}{c} R \\ G \\ B \end{array}
/// \right)
/// =
/// \frac{1}{256}
/// .
/// \left(
/// \left(
/// \begin{array}{ccc}
/// 256 & 0 & 359 \\ 256 & -88 & -183 \\ 256 & 454 & 0
/// \end{array}
/// \right)
/// .
/// \left(
/// \left(
/// \begin{array}{c} Y \\ Cb \\ Cr \end{array}
/// \right)
/// -
/// \left(
/// \begin{array}{c}   \\ 128 \\ 128 \end{array}
/// \right)
/// \right)
/// +
/// \left(
/// \begin{array}{c} 128 \\ 128 \\ 128 \end{array}
/// \right)
/// \right)
/// \f]
///
/// Examples of RGB --> YUV --> RGB transforms: ==> FIXMEEEEE
/// -	Black: (0,0,0) ---> (16,128,128) ---> (0,0,0)
/// -	Red: (255,0,0) ---> (82,90,240) ---> (255,1,0)
/// -	Green: (0,255,0) ---> (144,54,34) ---> (0,254,0)
/// -	Blue: (0,0,255) ---> (41,240,110) ---> (0,0,255)
/// -	Yellow: (255,255,0) ---> (210,16,146) ---> (255,255,0)
/// -	Magenta: (255,0,255) ---> (107,202,222) ---> (255,1,255)
/// -	Cyan: (0,255,255) ---> (169,166,16) ---> (0,254,255)
/// -	White: (255,255,255) ---> (235,128,128) ---> (255,255,255)

class BFC_Image_DLL Converter_RGB_JYUV444P : public RawConverter {

public :

	Converter_RGB_JYUV444P(
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

	Converter_RGB_JYUV444P(
		const	Converter_RGB_JYUV444P&
	);

	Converter_RGB_JYUV444P& operator = (
		const	Converter_RGB_JYUV444P&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Converter_RGB_JYUV444P_H_

// ============================================================================

