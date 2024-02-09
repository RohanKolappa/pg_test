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
//	BFC.Image.Converter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter.h"
#include "BFC.Image.ConverterManager.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Converter::Converter(
	const	PictureFormat &	pIFormat,
	const	PictureFormat &	pOFormat,
	const	Bool		pPreferred ) :

	iformat( pIFormat ),
	oformat( pOFormat ) {

	ConverterManager::instance()->addConverter( this, pPreferred );

}

// ============================================================================

#if 0

const Image::Converter::V4SI	Image::Converter::fv1 = { 16829,  -9714, 28784,     0 };
const Image::Converter::V4SI	Image::Converter::fv2 = { 33039, -19070, 28784,     0 };
const Image::Converter::V4SI	Image::Converter::fv3 = {  6416,  28784, -4681,     0 };
const Image::Converter::V4SI	Image::Converter::fv4 = {     0,      0,     0, 65536 };

const Image::Converter::V4SI	Image::Converter::fc = { 1081344, 8421376, 8421376, 0 };

const Image::Converter::V4SI	Image::Converter::bv1 = { 76309,      0, 104597,     0 };
const Image::Converter::V4SI	Image::Converter::bv2 = { 76309, -25674, -53279,     0 };
const Image::Converter::V4SI	Image::Converter::bv3 = { 76309, 132202,      0,     0 };
const Image::Converter::V4SI	Image::Converter::bv4 = {     0,      0,      0, 65536 };

const Image::Converter::V4SI	Image::Converter::bc = { -14576592, 8917808, -18110032, 0 };

#endif

//void Image::Converter::transformRGB2YUV(
//	const	Int32		R,
//	const	Int32		G,
//	const	Int32		B,
//		Int32 &		Y,
//		Int32 &		U,
//		Int32 &		V ) {
//
//#if 0
//
//	V4SI	r = { R, R, R, R };
//	V4SI	g = { G, G, G, G };
//	V4SI	b = { B, B, B, B };
//	V4SI	a = { 0, 0, 0, 0 };
//
//	UV4SI	res;
//
//	res.v = v1 * r + v2 * g + v3 * b + v4 * a + c;
//
//	Y = ( res.i[ 0 ] >> 16 );
//	U = ( res.i[ 1 ] >> 16 );
//	V = ( res.i[ 2 ] >> 16 );
//
//#else
//
//	Y = ( ( 16829 * R + 33039 * G +  6416 * B + 1081344 ) >> 16 );
//	U = ( ( -9714 * R - 19070 * G + 28784 * B + 8421376 ) >> 16 );
//	V = ( ( 28784 * R - 24103 * G -  4681 * B + 8421376 ) >> 16 );
//
//#endif
//
//}
//
//void Image::Converter::transformYUV2RGB(
//	const	Int32		Y,
//	const	Int32		U,
//	const	Int32		V,
//		Int32 &		R,
//		Int32 &		G,
//		Int32 &		B
//) {
//	R = ( ( 76309 * Y              + 104597 * V - 14576592 ) >> 16 );
//	G = ( ( 76309 * Y -  25674 * U -  53279 * V +  8917808 ) >> 16 );
//	B = ( ( 76309 * Y + 132202 * U              - 18110032 ) >> 16 );
//	if ( R < 0 ) R = 0; else if ( R > 255 ) R = 255;
//	if ( G < 0 ) G = 0; else if ( G > 255 ) G = 255;
//	if ( B < 0 ) B = 0; else if ( B > 255 ) B = 255;
//}

// ============================================================================

