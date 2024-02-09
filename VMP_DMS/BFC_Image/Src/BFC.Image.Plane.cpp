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
//	BFC.Image.Plane.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.BufferSerializer.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Image.Plane.h"

#include "BFC.Math.Utils.h"

#include "BFC.Mem.UnivU32Reader.h"
#include "BFC.Mem.UnivU32Writer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Plane::Plane() :

	planeW		( 0 ),
	planeH		( 0 ) {

}

Image::Plane::Plane(
	const	Uint32		pPlaneW,
	const	Uint32		pPlaneH,
	const	Uint32		pBitsPP,
	const	Bool		pClrDat ) :

	planeW		( pPlaneW ),
	planeH		( pPlaneH ),
	planeF		( pBitsPP, ColorChannel::Unknown ),
	planeD		( planeF.getBytesIfPlane( planeW, planeH ) ) {

	if ( pClrDat ) {
		getDataWriter().clearAll();
	}

}

Image::Plane::Plane(
	const	Buffer &	pPlaneD,
	const	Uint32		pPlaneW,
	const	Uint32		pPlaneH,
	const	Uint32		pBitsPP ) :

	planeW		( pPlaneW ),
	planeH		( pPlaneH ),
	planeF		( pBitsPP, ColorChannel::Unknown ),
	planeD		( pPlaneD ) {

	if ( ! planeF.checkBytesIfPlane( planeW, planeH, planeD.getLength() ) ) {
		// Yes! An exception in a constructor :-)
		throw InvalidArgument(
			"Plane(buf): buf: "
			+ Buffer( planeD.getLength() )
			+ ", exp: "
			+ Buffer( planeF.getBytesIfPlane( planeW, planeH ) ) );
	}

}

Image::Plane::Plane(
	const	Uint32		pPictureW,
	const	Uint32		pPictureH,
	const	PlaneFormat &	pPlaneF,
	const	Bool		pClrDat ) :

	planeW		( pPlaneF.getWidth( pPictureW ) ),
	planeH		( pPlaneF.getHeight( pPictureH ) ),
	planeF		( pPlaneF ),
	planeD		( pPlaneF.getBytesIfPicture( pPictureW, pPictureH ) ) {

	if ( pClrDat ) {
		getDataWriter().clearAll();
	}

}

Image::Plane::Plane(
	const	Uint32		pPictureW,
	const	Uint32		pPictureH,
	const	PlaneFormat &	pPlaneF,
	const	Mem::Block &	pPlaneD ) :

	planeW		( pPlaneF.getWidth( pPictureW ) ),
	planeH		( pPlaneF.getHeight( pPictureH ) ),
	planeF		( pPlaneF ),
	planeD		( pPlaneD ) {

	if ( ! planeF.checkBytesIfPicture(
			pPictureW, pPictureH, planeD.getLength() ) ) {
		// Yes! An exception in a constructor :-)
		throw InvalidArgument(
			"Plane(block): buf: "
			+ Buffer( planeD.getLength() )
			+ ", exp: "
			+ Buffer( planeF.getBytesIfPicture( pPictureW, pPictureH ) ) );
	}

}

Image::Plane::Plane(
	const	Plane &		pOther ) :

	SObject		(),
	DObject		(),
	VarUser		( pOther ),

	planeW( pOther.planeW ),
	planeH( pOther.planeH ),
	planeF( pOther.planeF ),
	planeD( pOther.planeD ) {

}

// ============================================================================

Image::Plane & Image::Plane::operator = (
	const	Plane &		pOther ) {

	if ( this != & pOther ) {
		VarUser::operator = ( pOther );
		planeW	= pOther.planeW;
		planeH	= pOther.planeH;
		planeF	= pOther.planeF;
		planeD	= pOther.planeD;
	}

	return *this;

}

// ============================================================================

Bool Image::Plane::operator == (
	const	Plane &		other ) const {

	return ( planeW == other.planeW
	      && planeH == other.planeH
	      && planeF == other.planeF
	      && getDataReader() == other.getDataReader() );

}

// ============================================================================

void Image::Plane::kill() {

	delAllDynVars();

	planeW = 0;
	planeH = 0;
	planeF.kill();
	planeD.kill();

}

// ============================================================================

void Image::Plane::setFormat(
	const	PlaneFormat	& pPlaneF ) {

	if ( ! pPlaneF.checkBytesIfPlane( planeW, planeH, planeD.getLength() ) ) {
		throw InvalidArgument(
			"setFormat(): buf: "
			+ Buffer( planeD.getLength() )
			+ ", new format: "
			+ Buffer( pPlaneF.getBytesIfPlane( planeW, planeH ) ) );
	}

	planeF = pPlaneF;

}

// ============================================================================

Mem::Reader Image::Plane::getDataReader() const {

	return Mem::Reader(
		planeD,
		planeF.getBytesPerPixel(),
		planeW,
		planeH );

}

Mem::Writer Image::Plane::getDataWriter() {

	return Mem::Writer(
		planeD,
		planeF.getBytesPerPixel(),
		planeW,
		planeH );

}

// ============================================================================

void Image::Plane::getClip(
		Plane &		pClip,
	const	Uint32		pPicStartX,
	const	Uint32		pPicStartY ) const {

	if ( getFormat() != pClip.getFormat() ) {
		throw InternalError( "getClip(): different formats!" );
	}

	pClip.getDataWriter().copyRectangle(
		0,
		0,
		pClip.getWidth(),
		pClip.getHeight(),
		getDataReader(),
		pPicStartX / getFormat().getMinXDownsampling(),
		pPicStartY / getFormat().getMinYDownsampling(),
		false
	);

}

void Image::Plane::setClip(
	const	Plane &		pClip,
	const	Uint32		pPicStartX,
	const	Uint32		pPicStartY ) {

	if ( getFormat() != pClip.getFormat() ) {
		throw InternalError( "getClip(): different formats!" );
	}

	getDataWriter().copyRectangle(
		pPicStartX / getFormat().getMinXDownsampling(),
		pPicStartY / getFormat().getMinYDownsampling(),
		pClip.getWidth(),
		pClip.getHeight(),
		pClip.getDataReader(),
		0,
		0,
		false
	);

}

// ============================================================================

Uint32 Image::Plane::getMin() const {

	Uint32	res = (Uint32)-1;
	Uint32	c;
	Uint32	Bps = planeF.getBytesPerPixel();

	if ( Bps == 1 ) {
		Mem::UcharReader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c < res ) {
				res = c;
			}
		}
	}
	else if ( Bps == 2 ) {
		Mem::Uint16Reader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c < res ) {
				res = c;
			}
		}
	}
	else if ( Bps == 3 ) {
		Mem::Uint24Reader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c < res ) {
				res = c;
			}
		}
	}
	else if ( Bps == 4 ) {
		Mem::Uint32Reader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c < res ) {
				res = c;
			}
		}
	}
	else {
		throw InternalError("min() with strange Bps");
	}

	return res;

}

Uint32 Image::Plane::getMax() const {

	Uint32	res = 0;
	Uint32	c;
	Uint32	Bps = planeF.getBytesPerPixel();

	if ( Bps == 1 ) {
		Mem::UcharReader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c > res ) {
				res = c;
			}
		}
	}
	else if ( Bps == 2 ) {
		Mem::Uint16Reader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c > res ) {
				res = c;
			}
		}
	}
	else if ( Bps == 3 ) {
		Mem::Uint24Reader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c > res ) {
				res = c;
			}
		}
	}
	else if ( Bps == 4 ) {
		Mem::Uint32Reader r = getDataReader();
		while ( r ) {
			c = *r;
			r++;
			if ( c > res ) {
				res = c;
			}
		}
	}
	else {
		throw InternalError("min() with strange Bps");
	}

	return res;

}

// ============================================================================

void Image::Plane::downsampleX2() {

//	msgDbg("... downsampling X2");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "downsampleX2() with bps != 8" );
	}

	Uint32		newPlaneW = planeW / 2;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, planeH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, planeH );

	const Uchar *	src;
	Uchar *		dst;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst = ( Uchar * )w.getRowAddress( j );
		for ( Uint32 i = newPlaneW ; i ; i-- ) {
			*dst++ = ( Uchar )
				( ( ( Uint16 )src[ 0 ] + ( Uint16 )src[ 1 ]
				  + 1 ) / 2 );
			src += 2;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;

}

void Image::Plane::downsampleX4() {

//	msgDbg("... downsampling X4");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "downsampleX4() with bps != 8" );
	}

	Uint32		newPlaneW = planeW / 4;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, planeH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, planeH );

	const Uchar *	src;
	Uchar *		dst;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst = ( Uchar * )w.getRowAddress( j );
		for ( Uint32 i = newPlaneW ; i ; i-- ) {
			*dst++ = ( Uchar )
				( ( ( Uint16 )src[ 0 ] + ( Uint16 )src[ 1 ]
				  + ( Uint16 )src[ 2 ] + ( Uint16 )src[ 3 ]
				  + 2 ) / 4 );
			src += 4;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;

}

void Image::Plane::downsampleY2() {

//	msgDbg("... downsampling Y2");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "downsampleY2() with bps != 8" );
	}

	Uint32		newPlaneH = planeH / 2;
	Uint32		newLength = planeF.getBytesIfPlane( planeW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, planeW, newPlaneH );

	const Uchar *	src0;
	const Uchar *	src1;
	Uchar *		dst;

	for ( Uint32 j = 0 ; j < newPlaneH ; j++ ) {
		src0 = ( const Uchar * )r.getRowAddress( 2 * j );
		src1 = ( const Uchar * )r.getRowAddress( 2 * j + 1 );
		dst = ( Uchar * )w.getRowAddress( j );
		for ( Uint32 i = planeW ; i ; i-- ) {
			*dst++ = ( Uchar )
				( ( ( Uint16 )*src0++ + ( Uint16 )*src1++
				  + 1 ) / 2 );
		}
	}

	planeD = newPlaneD;
	planeH = newPlaneH;

}

void Image::Plane::downsampleY4() {

//	msgDbg("... downsampling Y4");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "downsampleY4() with bps != 8" );
	}

	Uint32		newPlaneH = planeH / 4;
	Uint32		newLength = planeF.getBytesIfPlane( planeW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, planeW, newPlaneH );

	const Uchar *	src0;
	const Uchar *	src1;
	const Uchar *	src2;
	const Uchar *	src3;
	Uchar *		dst;

	for ( Uint32 j = 0 ; j < newPlaneH ; j++ ) {
		src0 = ( const Uchar * )r.getRowAddress( 4 * j );
		src1 = ( const Uchar * )r.getRowAddress( 4 * j + 1 );
		src2 = ( const Uchar * )r.getRowAddress( 4 * j + 2 );
		src3 = ( const Uchar * )r.getRowAddress( 4 * j + 3 );
		dst = ( Uchar * )w.getRowAddress( j );
		for ( Uint32 i = planeW ; i ; i-- ) {
			*dst++ = ( Uchar )
				( ( ( Uint16 )*src0++ + ( Uint16 )*src1++
				  + ( Uint16 )*src2++ + ( Uint16 )*src3++
				  + 2 ) / 4 );
		}
	}

	planeD = newPlaneD;
	planeH = newPlaneH;

}

void Image::Plane::downsampleXY2() {

//	msgDbg("... downsampling XY2");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "downsampleXY2() with bps != 8" );
	}

	Uint32		newPlaneW = planeW / 2;
	Uint32		newPlaneH = planeH / 2;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, newPlaneH );

	const Uchar *	src0;
	const Uchar *	src1;
	Uchar *		dst;

	for ( Uint32 j = 0 ; j < newPlaneH ; j++ ) {
		src0 = ( const Uchar * )r.getRowAddress( 2 * j );
		src1 = ( const Uchar * )r.getRowAddress( 2 * j + 1 );
		dst = ( Uchar * )w.getRowAddress( j );
		for ( Uint32 i = newPlaneW ; i ; i-- ) {
			*dst++ = ( Uchar )
				( ( ( Uint16 )src0[ 0 ] + ( Uint16 )src0[ 1 ]
				  + ( Uint16 )src1[ 0 ] + ( Uint16 )src1[ 1 ]
				  + 2 ) / 4 );
			src0 += 2;
			src1 += 2;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;
	planeH = newPlaneH;

}

void Image::Plane::downsampleXY4() {

//	msgDbg("... downsampling XY4");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "downsampleXY4() with bps != 8" );
	}

	Uint32		newPlaneW = planeW / 4;
	Uint32		newPlaneH = planeH / 4;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, newPlaneH );

	const Uchar *	src0;
	const Uchar *	src1;
	const Uchar *	src2;
	const Uchar *	src3;
	Uchar *		dst;

	for ( Uint32 j = 0 ; j < newPlaneH ; j++ ) {
		src0 = ( const Uchar * )r.getRowAddress( 4 * j );
		src1 = ( const Uchar * )r.getRowAddress( 4 * j + 1 );
		src2 = ( const Uchar * )r.getRowAddress( 4 * j + 2 );
		src3 = ( const Uchar * )r.getRowAddress( 4 * j + 3 );
		dst = ( Uchar * )w.getRowAddress( j );
		for ( Uint32 i = newPlaneW ; i ; i-- ) {
			*dst++ = ( Uchar )
				( ( ( Uint16 )src0[ 0 ] + ( Uint16 )src0[ 1 ]
				  + ( Uint16 )src0[ 2 ] + ( Uint16 )src0[ 3 ]
				  + ( Uint16 )src1[ 0 ] + ( Uint16 )src1[ 1 ]
				  + ( Uint16 )src1[ 2 ] + ( Uint16 )src1[ 3 ]
				  + ( Uint16 )src2[ 0 ] + ( Uint16 )src2[ 1 ]
				  + ( Uint16 )src2[ 2 ] + ( Uint16 )src2[ 3 ]
				  + ( Uint16 )src3[ 0 ] + ( Uint16 )src3[ 1 ]
				  + ( Uint16 )src3[ 2 ] + ( Uint16 )src3[ 3 ]
				  + 8 ) / 16 );
			src0 += 4;
			src1 += 4;
			src2 += 4;
			src3 += 4;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;
	planeH = newPlaneH;

}

// ============================================================================

void Image::Plane::upsampleX2() {

//	msgDbg("... upsampling X2");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "upsampleX2() with bps != 8" );
	}

	Uint32		newPlaneW = planeW * 2;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, planeH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, planeH );

	const Uchar *	src;
	Uint16 *	dst;
	Uint16		val;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst = ( Uint16 * )w.getRowAddress( j );
		for ( Uint32 i = planeW ; i ; i-- ) {
			val = ( Uint16 )*src++;
			val |= ( val << 8 );
			*dst++ = val;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;

}

void Image::Plane::upsampleX4() {

//	msgDbg("... upsampling X4");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "upsampleX4() with bps != 8" );
	}

	Uint32		newPlaneW = planeW * 4;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, planeH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, planeH );

	const Uchar *	src;
	Uint32 *	dst;
	Uint32		val;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst = ( Uint32 * )w.getRowAddress( j );
		for ( Uint32 i = planeW ; i ; i-- ) {
			val = ( Uint32 )*src++;
			val |= ( val << 8 );
			val |= ( val << 16 );
			*dst++ = val;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;

}

void Image::Plane::upsampleY2() {

//	msgDbg("... upsampling Y2");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument("upsampleY2() with bps != 8");
	}

	Uint32		newPlaneH = planeH * 2;
	Uint32		newLength = planeF.getBytesIfPlane( planeW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, planeW, newPlaneH );

	const Uchar *	src;
	Uchar *		dst0;
	Uchar *		dst1;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst0 = ( Uchar * )w.getRowAddress( 2 * j );
		dst1 = ( Uchar * )w.getRowAddress( 2 * j + 1 );
		for ( Uint32 i = planeW ; i ; i-- ) {
			*dst0++ = *dst1++ = *src++;
		}
	}

	planeD = newPlaneD;
	planeH = newPlaneH;

}

void Image::Plane::upsampleY4() {

//	msgDbg("... upsampling Y4");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument("upsampleY4() with bps != 8");
	}

	Uint32		newPlaneH = planeH * 4;
	Uint32		newLength = planeF.getBytesIfPlane( planeW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, planeW, newPlaneH );

	const Uchar *	src;
	Uchar *		dst0;
	Uchar *		dst1;
	Uchar *		dst2;
	Uchar *		dst3;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst0 = ( Uchar * )w.getRowAddress( 4 * j );
		dst1 = ( Uchar * )w.getRowAddress( 4 * j + 1 );
		dst2 = ( Uchar * )w.getRowAddress( 4 * j + 2 );
		dst3 = ( Uchar * )w.getRowAddress( 4 * j + 3 );
		for ( Uint32 i = planeW ; i ; i-- ) {
			*dst0++ = *dst1++ = *dst2++ = *dst3++ = *src++;
		}
	}

	planeD = newPlaneD;
	planeH = newPlaneH;

}

void Image::Plane::upsampleXY2() {

//	msgDbg("... upsampling XY2");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "upsampleXY2() with bps != 8" );
	}

	Uint32		newPlaneW = planeW * 2;
	Uint32		newPlaneH = planeH * 2;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, newPlaneH );

	const Uchar *	src;
	Uint16 *	dst0;
	Uint16 *	dst1;
	Uint16		val;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst0 = ( Uint16 * )w.getRowAddress( 2 * j );
		dst1 = ( Uint16 * )w.getRowAddress( 2 * j + 1 );
		for ( Uint32 i = planeW ; i ; i-- ) {
			val = ( Uint16 )*src++;
			val |= ( val << 8 );
			*dst0++ = *dst1++ = val;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;
	planeH = newPlaneH;

}

void Image::Plane::upsampleXY4() {

//	msgDbg("... upsampling XY4");

	if ( planeF.getBitsPerPixel() != 8 ) {
		throw InvalidArgument( "upsampleXY4() with bps != 8" );
	}

	Uint32		newPlaneW = planeW * 4;
	Uint32		newPlaneH = planeH * 4;
	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, newPlaneH );
	Mem::Block	newPlaneD( newLength );
	Mem::Reader	r( planeD, 1, planeW, planeH );
	Mem::Writer	w( newPlaneD, 1, newPlaneW, newPlaneH );

	const Uchar *	src;
	Uint32 *	dst0;
	Uint32 *	dst1;
	Uint32 *	dst2;
	Uint32 *	dst3;
	Uint32		val;

	for ( Uint32 j = 0 ; j < planeH ; j++ ) {
		src = ( const Uchar * )r.getRowAddress( j );
		dst0 = ( Uint32 * )w.getRowAddress( 4 * j );
		dst1 = ( Uint32 * )w.getRowAddress( 4 * j + 1 );
		dst2 = ( Uint32 * )w.getRowAddress( 4 * j + 2 );
		dst3 = ( Uint32 * )w.getRowAddress( 4 * j + 3 );
		for ( Uint32 i = planeW ; i ; i-- ) {
			val = ( Uint32 )*src++;
			val |= ( val << 8 );
			val |= ( val << 16 );
			*dst0++ = *dst1++ = *dst2++ = *dst3++ = val;
		}
	}

	planeD = newPlaneD;
	planeW = newPlaneW;
	planeH = newPlaneH;

}

// ============================================================================

void Image::Plane::flipTB() {

	Uint32		newLength = planeF.getBytesIfPlane( planeW, planeH );
	Mem::Block	newPlaneD( newLength );
	Uint32		Bps = planeF.getBytesPerPixel();

	Mem::UcharReader r( planeD );
	Mem::UcharWriter w( newPlaneD );

	r.setDimensions( planeW * Bps, planeH );
	w.setDimensions( planeW * Bps, planeH );

	for ( Uint32 y = 0 ; y < planeH ; y++ ) {
		w.copySegment( 0, y, planeW * Bps, r, 0, planeH - y - 1, false );
	}

	planeD = newPlaneD;

}

void Image::Plane::flipLR() {

	Uint32		newLength = planeF.getBytesIfPlane( planeW, planeH );
	Mem::Block	newPlaneD( newLength );
	Uint32		Bps = planeF.getBytesPerPixel();

	Mem::UcharReader r( planeD );
	Mem::UcharWriter w( newPlaneD );

	r.setDimensions( planeW * Bps, planeH );
	w.setDimensions( planeW * Bps, planeH );

	for ( Uint32 y = 0 ; y < planeH ; y++ ) {
		r.gotoElement( 0, y );
		w.gotoElement( planeW * Bps - 1, y );
		for ( Uint32 x = planeW * Bps ; x ; x-- ) {
			*w = *r;
			w--;
			r++;
		}
	}

	planeD = newPlaneD;

}

// ============================================================================

void Image::Plane::resize(
	const	Uint32		newPlaneW,
	const	Uint32		newPlaneH ) {

	Uint32		newLength = planeF.getBytesIfPlane( newPlaneW, newPlaneH );

	if ( ! newLength ) {
		planeD.kill();
		planeW = 0;
		planeH = 0;
		return;
	}

	Mem::Block	newPlaneD( newLength );
	Uint32		Bps = planeF.getBytesPerPixel();

	Uint32		minPlaneW = Math::getMin( ( Uint32 )planeW, newPlaneW );
	Uint32		minPlaneH = Math::getMin( ( Uint32 )planeH, newPlaneH );

	if ( minPlaneW && minPlaneH ) {

		Mem::UcharReader r( planeD );
		Mem::UcharWriter w( newPlaneD );

		r.setDimensions( planeW * Bps, planeH );
		w.setDimensions( newPlaneW * Bps, newPlaneH );

		for ( Uint32 y = 0 ; y < minPlaneH ; y++ ) {
			r.gotoElement( 0, y );
			w.gotoElement( 0, y );
			for ( Uint32 x = minPlaneW * Bps ; x ; x-- ) {
				*w = *r;
				w++;
				r++;
			}
		}

	}

	planeD = newPlaneD;
	planeW = newPlaneW;
	planeH = newPlaneH;

}

// ============================================================================

void Image::Plane::randomize() {

	Mem::UcharWriter	writer( planeD );

	Uint32			nbrCols = writer.getRowLength();
	Uint32			nbrRows = writer.getNbrRows();

	for ( Uint32 i = 0 ; i < nbrRows ; i++ ) {
		Uchar * ptr = ( Uchar * )writer.getElementAddress( 0, i );
		for ( Uint32 j = nbrCols ; j ; j-- ) {
			*ptr++ = Math::rand();
		}
	}

}

// ============================================================================

Image::Plane Image::Plane::getMean(
	const	Plane &		plane0,
	const	Plane &		plane1,
	const	Plane &		plane2 ) {

	// Sanity checks...

	Uint32	bpp = plane0.getFormat().getBitsPerPixel();

	if ( plane1.getFormat().getBitsPerPixel() != bpp
	  || plane2.getFormat().getBitsPerPixel() != bpp ) {
		throw InvalidArgument( "getMean(): different formats!" );
	}

	if ( ! plane1.sameSize( plane0 )
	  || ! plane2.sameSize( plane0 ) ) {
		throw InvalidArgument( "getMean(): different dimensions!" );
	}

	// Let's create a new plane to hold the result...

	Plane	planeR( plane0.getWidth(), plane0.getHeight(), bpp );
	Uint32	Bpp = plane0.getFormat().getBytesPerPixel();

	if ( Bpp == 1 ) {
		Mem::UcharReader r0 = plane0.getDataReader();
		Mem::UcharReader r1 = plane1.getDataReader();
		Mem::UcharReader r2 = plane2.getDataReader();
		Mem::UcharWriter wr = planeR.getDataWriter();
		while ( wr ) {
			*wr = ( ( Uint32 )*r0 + ( Uint32 )*r1 + ( Uint32 )*r2 ) / 3;
			r0++;
			r1++;
			r2++;
			wr++;
		}
	}
	else if ( Bpp == 2 ) {
		Mem::Uint16Reader r0 = plane0.getDataReader();
		Mem::Uint16Reader r1 = plane1.getDataReader();
		Mem::Uint16Reader r2 = plane2.getDataReader();
		Mem::Uint16Writer wr = planeR.getDataWriter();
		while ( wr ) {
			*wr = ( ( Uint32 )*r0 + ( Uint32 )*r1 + ( Uint32 )*r2 ) / 3;
			r0++;
			r1++;
			r2++;
			wr++;
		}
	}
	else if ( Bpp == 3 ) {
		Mem::Uint24Reader r0 = plane0.getDataReader();
		Mem::Uint24Reader r1 = plane1.getDataReader();
		Mem::Uint24Reader r2 = plane2.getDataReader();
		Mem::Uint24Writer wr = planeR.getDataWriter();
		while ( wr ) {
			*wr = ( ( ( Uint32 )*r0 + ( Uint32 )*r1 + ( Uint32 )*r2 ) / 3 );
			r0++;
			r1++;
			r2++;
			wr++;
		}
	}
	else if ( Bpp == 4 ) {
		Mem::Uint32Reader r0 = plane0.getDataReader();
		Mem::Uint32Reader r1 = plane1.getDataReader();
		Mem::Uint32Reader r2 = plane2.getDataReader();
		Mem::Uint32Writer wr = planeR.getDataWriter();
		while ( wr ) {
			*wr = ( ( Uint32 )*r0 + ( Uint32 )*r1 + ( Uint32 )*r2 ) / 3;
			r0++;
			r1++;
			r2++;
			wr++;
		}
	}
	else if ( Bpp == 8 ) {
		Mem::Uint32Reader r0 = plane0.getDataReader();
		Mem::Uint32Reader r1 = plane1.getDataReader();
		Mem::Uint32Reader r2 = plane2.getDataReader();
		Mem::Uint32Writer wr = planeR.getDataWriter();
		while ( wr ) {
			*wr = ( ( Uint32 )*r0 + ( Uint32 )*r1 + ( Uint32 )*r2 ) / 3;
			r0++;
			r1++;
			r2++;
			wr++;
		}
	}

	return planeR;

}

Image::Plane Image::Plane::getAbsDiff(
	const	Plane		& plane0,
	const	Plane		& plane1 ) {

	// Sanity checks...

	Uint32	bpp = plane0.getFormat().getBitsPerPixel();

	if ( plane1.getFormat().getBitsPerPixel() != bpp ) {
		throw InvalidArgument( "getAbsDiff(): different formats!" );
	}

	if ( ! plane1.sameSize( plane0 ) ) {
		throw InvalidArgument( "getAbsDiff(): different dimensions!" );
	}

	// Let's create a new plane to hold the result...

	Plane	planeR( plane0.getWidth(), plane0.getHeight(), bpp );
	Uint32	Bpp = plane0.getFormat().getBytesPerPixel();

	Mem::UnivU32Reader r0( plane0.getDataReader(), Bpp );
	Mem::UnivU32Reader r1( plane1.getDataReader(), Bpp );
	Mem::UnivU32Writer wr( planeR.getDataWriter(), Bpp );

	while ( wr ) {
		Uint32 i0 = * r0;
		Uint32 i1 = * r1;
		wr.set( i0 < i1 ? i1 - i0 : i0 - i1 );
		r0++;
		r1++;
		wr++;
	}

	return planeR;

}

// ============================================================================

void Image::Plane::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	Dyn::VarUser::doSerialize( pOStream );

	pOStream->putLEUint32( planeW );
	pOStream->putLEUint32( planeH );
	planeF.doSerialize( pOStream );
	BufferSerializer::doSerialize( planeD.toBuffer(), pOStream );

}

void Image::Plane::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	Dyn::VarUser::unSerialize( pIStream );

	planeW = pIStream->getLEUint32();
	planeH = pIStream->getLEUint32();
	planeF.unSerialize( pIStream );
	planeD = Mem::Block( BufferSerializer::unSerialize( pIStream ) );

}

// ============================================================================

