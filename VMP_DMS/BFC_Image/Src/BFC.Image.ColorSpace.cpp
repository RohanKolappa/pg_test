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
//	BFC.Image.ColorSpace.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.ColorSpace.h"

// ============================================================================

using namespace BFC;

// ============================================================================

const Image::ColorSpace::DataTblEntry Image::ColorSpace::dataTbl[] = {

	{ "Unknown",	0 },
	{ "RGB",	3 },
	{ "YUV",	3 },
	{ "JYUV",	3 },
	{ "GrayScale",	1 },
	{ "XYZ",	3 },
	{ "LAB",	3 },
	{ "HLS",	3 },
	{ "HSV",	3 },
	{ "RYB",	3 },
	{ "CMYK",	4 },
	{ "RYUV",	3 }

};

// ============================================================================

