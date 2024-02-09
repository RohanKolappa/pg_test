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
//	BFC.Image.Converter_YUVxxxP_YUVxxxP.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#define BFC_Image_Converter_COMPILE 1

// ============================================================================

#include "BFC.Image.Picture.h"

#include "BFC.Image.ConverterPlaneAdder.cpp"
#include "BFC.Image.ConverterPlaneSwitcher.cpp"

#include "BFC.Image.Converter_BGR24_RGB555.cpp"
#include "BFC.Image.Converter_BGR24_RGB565.cpp"
#include "BFC.Image.Converter_GRAY12_GRAY16.cpp"
#include "BFC.Image.Converter_GRAY8_GRAY12.cpp"
#include "BFC.Image.Converter_GRAY8_GRAY16.cpp"
#include "BFC.Image.Converter_RGB24_BGR24.cpp"
#include "BFC.Image.Converter_RGB24_RGB555.cpp"
#include "BFC.Image.Converter_RGB24_RGB565.cpp"
#include "BFC.Image.Converter_RGB565_RGB555.cpp"
#include "BFC.Image.Converter_RGBA_BGR24.cpp"
#include "BFC.Image.Converter_RGBA_RGB24.cpp"
#include "BFC.Image.Converter_RGBA_RGB555.cpp"
#include "BFC.Image.Converter_RGBA_RGB565.cpp"
#include "BFC.Image.Converter_RGB_BGR24.cpp"
#include "BFC.Image.Converter_RGB_GRAY8.cpp"
#include "BFC.Image.Converter_RGB_RGB24.cpp"
#include "BFC.Image.Converter_RGB_RGB323.cpp"
#include "BFC.Image.Converter_RGB_RGB555.cpp"
#include "BFC.Image.Converter_RGB_RGB565.cpp"
#include "BFC.Image.Converter_RGB_RGBA.cpp"
#include "BFC.Image.Converter_RGB_YUV444P.cpp"
#include "BFC.Image.Converter_YUV400_RGB.cpp"
#include "BFC.Image.Converter_YUV400_YUV444P.cpp"
#include "BFC.Image.Converter_YUV422P_YUV422.cpp"
#include "BFC.Image.Converter_YUV422_YUYV.cpp"
#include "BFC.Image.Converter_YUV444P_AYUV444.cpp"
#include "BFC.Image.Converter_YUV444P_UYV444.cpp"
#include "BFC.Image.Converter_YUVxxxP_YUVxxxP.cpp"
#include "BFC.Image.Converter_YVYU_YUYV.cpp"

#include "BFC.Image.Converter_YUV444P_JYUV444P.cpp"
#include "BFC.Image.Converter_YUV420P_JYUV400.cpp"
#include "BFC.Image.Converter_RGB_JYUV444P.cpp"
// ============================================================================

#undef BFC_Image_Converter_COMPILE

// ============================================================================

