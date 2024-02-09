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
//	BFC.Image.Converter_BGR24_YUV422.h
// 
// Description:
//	...
// 
// Author(s):
//	Christophe Herreman
// 
// ============================================================================

#ifndef _BFC_Image_Converter_BGR24_YUV422_H_
#define _BFC_Image_Converter_BGR24_YUV422_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.RawConverter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief BGR24 to/from YUV422 converter.
///
/// This converter uses the transform described in Converter_RGB_YUV444P.
///
/// \ingroup BFC_Image

class BFC_Image_DLL Converter_BGR24_YUV422 : public RawConverter {

public :

	/// \brief Creates a new Converter_BGR24_YUV422.

	Converter_BGR24_YUV422(
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

#endif // _BFC_Image_Converter_BGR24_YUV422_H_

// ============================================================================

