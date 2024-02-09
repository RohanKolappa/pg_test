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
//	BFC.Image.ConverterPlaneSwitcher.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ConverterPlaneSwitcher_H_
#define _BFC_Image_ConverterPlaneSwitcher_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.Converter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class BFC_Image_DLL ConverterPlaneSwitcher : public Converter {

public :

	ConverterPlaneSwitcher(
		const	PictureFormat &	pIFormat,
		const	PictureFormat &	pOFormat,
		const	Uint32		srcPlane0,
		const	Uint32		srcPlane1,
		const	Uint32		srcPlane2
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

	Uint32		srcPlane0;
	Uint32		srcPlane1;
	Uint32		srcPlane2;

	// others...

	ConverterPlaneSwitcher(
	);

	ConverterPlaneSwitcher(
		const	ConverterPlaneSwitcher&
	);

	ConverterPlaneSwitcher& operator = (
		const	ConverterPlaneSwitcher&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ConverterPlaneSwitcher_H_

// ============================================================================

