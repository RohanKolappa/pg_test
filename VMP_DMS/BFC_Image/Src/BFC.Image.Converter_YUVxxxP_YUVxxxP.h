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
//	BFC.Image.Converter_YUVxxxP_YUVxxxP.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Converter_YUVxxxP_YUVxxxP_H_
#define _BFC_Image_Converter_YUVxxxP_YUVxxxP_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.Converter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class BFC_Image_DLL Converter_YUVxxxP_YUVxxxP : public Converter {

public :

	Converter_YUVxxxP_YUVxxxP(
		const	PictureFormat	inFormat,
		const	PictureFormat	outFormat,
		const	Uint32	ixdown,
		const	Uint32	iydown,
		const	Uint32	oxdown,
		const	Uint32	oydown
	);

	virtual Picture convertForward(
		const	Picture &	pIPicture
	);

	virtual void convertForward(
		const	Picture &	pIPicture,
			Picture &	pOPicture
	);

	virtual Picture convertBackward(
		const	Picture &	pIPicture
	);

	virtual void convertBackward(
		const	Picture &	pIPicture,
			Picture &	pOPicture
	);

protected :

private :

	// internal state...

	Uint32		ixdown;
	Uint32		iydown;
	Uint32		oxdown;
	Uint32		oydown;

	// others...

	Converter_YUVxxxP_YUVxxxP(
	);

	Converter_YUVxxxP_YUVxxxP(
		const	Converter_YUVxxxP_YUVxxxP&
	);

	Converter_YUVxxxP_YUVxxxP& operator = (
		const	Converter_YUVxxxP_YUVxxxP&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Converter_YUVxxxP_YUVxxxP_H_

// ============================================================================

