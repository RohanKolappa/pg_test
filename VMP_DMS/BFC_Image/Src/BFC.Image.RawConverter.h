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
//	BFC.Image.RawConverter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_RawConverter_H_
#define _BFC_Image_RawConverter_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Mem.Block.h"
#include "BFC.Mem.Reader.h"
#include "BFC.Mem.Writer.h"

#include "BFC.Image.Converter.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Virtual base class of some predefined converters.
///
/// \ingroup BFC_Image

class BFC_Image_DLL RawConverter : public Converter {

public :

	RawConverter(
		const	PictureFormat &	pIFormat,
		const	PictureFormat &	pOFormat
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

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION( "BFC.Image.RawConverter" );

	CLASS_EXCEPTION( CantConvertPicture, "Can't convert picture" );

protected :

	typedef Array< Mem::UcharReader >	ReaderArray;
	typedef Array< Mem::UcharWriter >	WriterArray;

	virtual void engineForward(
			ReaderArray &	pIPics,
			WriterArray &	pOPics
	) = 0;

	virtual void engineBackward(
			ReaderArray &	pIPics,
			WriterArray &	pOPics
	) = 0;

private :

	// others...

	RawConverter(
	);

	RawConverter(
		const	RawConverter&
	);

	RawConverter& operator = (
		const	RawConverter&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_RawConverter_H_

// ============================================================================

