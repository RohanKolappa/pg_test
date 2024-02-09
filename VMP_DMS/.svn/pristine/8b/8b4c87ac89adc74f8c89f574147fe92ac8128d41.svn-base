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
//	BFC.Image.Converter_YUV444P_AYUV444.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Converter_YUV444P_AYUV444_H_
#define _BFC_Image_Converter_YUV444P_AYUV444_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.RawConverter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class BFC_Image_DLL Converter_YUV444P_AYUV444 : public RawConverter {

public :

	Converter_YUV444P_AYUV444(
	);

	virtual ~Converter_YUV444P_AYUV444(
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

	Converter_YUV444P_AYUV444(
		const	Converter_YUV444P_AYUV444&
	);

	Converter_YUV444P_AYUV444& operator = (
		const	Converter_YUV444P_AYUV444&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Converter_YUV444P_AYUV444_H_

// ============================================================================

