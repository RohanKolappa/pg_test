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
//	BFC.Image.Picture.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Image.ConverterManager.h"
#include "BFC.Image.ConverterStrategy.h"
#include "BFC.Image.Picture.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Image, Picture )

// ============================================================================

Image::Picture::Picture() :

	picW	( 0 ),
	picH	( 0 ),
	picF	( PictureFormat::Unknown ) {

}

Image::Picture::Picture(
	const	Uint32		pPicW,
	const	Uint32		pPicH,
	const	PictureFormat &	pPicF,
	const	Bool		pClrDat ) :

	picW	( pPicW ),
	picH	( pPicH ),
	picF	( pPicF ) {

	if ( pPicW && pPicH && pPicF.isUnknown() ) {
		throw InvalidArgument( "in Picture(w,h,f)" );
	}

	picP.resize( picF.getNbrPlanes() );

	for ( Uint32 i = 0 ; i < picF.getNbrPlanes() ; i++ ) {
		picP[ i ] = Plane( picW, picH, picF.getPlane( i ), pClrDat );
	}

	checkValidness(); // dumb programmers, you know ;-)

}

Image::Picture::Picture(
	const	Uint32		pPicW,
	const	Uint32		pPicH,
	const	PictureFormat &	pPicF,
	const	PlaneArray &	pPicP ) :

	picW	( pPicW ),
	picH	( pPicH ),
	picF	( pPicF ),
	picP	( pPicP ) {

	checkValidness();

}

Image::Picture::Picture(
	const	Uint32		pPicW,
	const	Uint32		pPicH,
	const	PictureFormat &	pPicF,
	const	Buffer &	pPicP0,
	const	Buffer &	pPicP1,
	const	Buffer &	pPicP2 ) :

	picW( pPicW ),
	picH( pPicH ),
	picF( pPicF ) {

	picP.resize( 3 );

	picP[ 0 ] = Plane(
			picW,
			picH,
			picF.getPlane( 0 ),
			pPicP0 );
	picP[ 1 ] = Plane(
			picW,
			picH,
			picF.getPlane( 1 ),
			pPicP1 );
	picP[ 2 ] = Plane(
			picW,
			picH,
			picF.getPlane( 2 ),
			pPicP2 );

	checkValidness();

}

Image::Picture::Picture(
	const	Uint32		pPicW,
	const	Uint32		pPicH,
	const	PictureFormat &	pPicF,
	const	Plane &		pPicP0,
	const	Plane &		pPicP1,
	const	Plane &		pPicP2 ) :

	picW( pPicW ),
	picH( pPicH ),
	picF( pPicF ) {

	picP.resize( 3 );

	picP[ 0 ] = pPicP0;
	picP[ 1 ] = pPicP1;
	picP[ 2 ] = pPicP2;

	checkValidness();

}

Image::Picture::Picture(
	const	Uint32		pPicW,
	const	Uint32		pPicH,
	const	PictureFormat &	pPicF,
	const	Buffer &	pPacked ) :

	picW( pPicW ),
	picH( pPicH ),
	picF( pPicF ) {

	picP.resize( 1 );

	picP[ 0 ] = Plane( picW, picH, picF[ 0 ], pPacked );

	checkValidness();

}

Image::Picture::Picture(
	const	Uint32		pPicW,
	const	Uint32		pPicH,
	const	PictureFormat &	pPicF,
	const	Plane &		pPacked ) :

	picW( pPicW ),
	picH( pPicH ),
	picF( pPicF ) {

	picP.resize( 1 );

	picP[ 0 ] = pPacked;

	checkValidness();

}

Image::Picture::Picture(
	const	Picture &	pOther ) :

	SObject	(),
	DObject	(),
	VarUser	( pOther ),
	picW	( pOther.picW ),
	picH	( pOther.picH ),
	picF	( pOther.picF ),
	picP	( pOther.picP ) {



}

// ============================================================================

Image::Picture & Image::Picture::operator = (
	const	Picture &	pOther ) {

	if ( this != & pOther ) {
		VarUser::operator = ( pOther );
		picW = pOther.picW;
		picH = pOther.picH;
		picF = pOther.picF;
		picP = pOther.picP;
	}

	return *this;

}

// ============================================================================

Bool Image::Picture::operator == (
	const	Picture &	other ) const {

	return ( picW == other.picW
	      && picH == other.picH
	      && picF == other.picF
	      && picP == other.picP );

}

Bool Image::Picture::operator != (
	const	Picture &	other ) const {

	return ( picW != other.picW
	      || picH != other.picH
	      || picF != other.picF
	      || picP != other.picP );

}

// ============================================================================

void Image::Picture::kill() {

	delAllDynVars();

	picW = 0;
	picH = 0;
	picF.kill(); // = PictureFormat::Unknown;
	picP.kill();

}

// ============================================================================

void Image::Picture::setPlane(
	const	Uint32		pPicPlane,
	const	Plane &		pNewPlane ) {

	if ( pPicPlane >= picP.getSize() ) {
		throw InvalidArgument( "setPlane(): out of range!" );
	}

	if ( picF.isUnknown() ) {
		throw InvalidArgument( "setPlane(): unknown current format!" );
	}

	if ( pNewPlane.getFormat().isKnown()
	       && pNewPlane.getFormat() != picF.getPlane( pPicPlane ) ) {
		throw InvalidArgument( "setPlane(): formats differ!" );
	}

	// Copy the plane. That's all!

	picP[ pPicPlane ] = pNewPlane;

}

// ============================================================================

const Image::Plane & Image::Picture::getPlane(
	const	Uint32		pPicPlane ) const {

	if ( pPicPlane >= picP.getSize() ) {
		throw InvalidArgument( "getPlane(): out of range!" );
	}

	return picP[ pPicPlane ];

}

Image::Plane & Image::Picture::getPlane(
	const	Uint32		pPicPlane ) {

	if ( pPicPlane >= picP.getSize() ) {
		throw InvalidArgument( "getPlane(): out of range!" );
	}

	return picP[ pPicPlane ];

}

Image::Plane & Image::Picture::privGetPlane(
	const	Uint32		pPicPlane ) {

	if ( pPicPlane >= picP.getSize() ) {
		throw InvalidArgument( "getPlane(): out of range!" );
	}

	return picP[ pPicPlane ];

}

const Image::Plane & Image::Picture::getPlane0() const {

	if ( picP.isEmpty() ) {
		throw InvalidArgument( "getPlane(): out of range!" );
	}

	return picP[ 0 ];

}

const Image::Plane & Image::Picture::getPlane1() const {

	if ( picP.getSize() <= 1 ) {
		throw InvalidArgument( "getPlane(): out of range!" );
	}

	return picP[ 1 ];

}

const Image::Plane & Image::Picture::getPlane2() const {

	if ( picP.getSize() <= 2 ) {
		throw InvalidArgument( "getPlane(): out of range!" );
	}

	return picP[ 2 ];

}

// ============================================================================

void Image::Picture::convertTo(
	const	PictureFormat &	newF ) {

//	cerr << "Convert: " << picF.getShortName() << " --> " << newF.getShortName() << endl;

	testConvertible();

	if ( newF == picF ) {
		return;
	}

	if ( ! newF.canRepresent( picW, picH ) ) {
		throw InvalidArgument( "convertTo(): dst not compatible! (WxH:"
			+ Buffer( picW ) + "x" + Buffer( picH )
			+ ", old: " + picF.name()
			+ ", dst: " + newF.name() + ")" );
	}

//	cerr << "Getting strategy..." << endl;

	ConverterStrategyPtr strategy = ConverterManager::instance()
			->getStrategyFor( picF, newF );

//	cerr << "Applying strategy..." << endl;

	operator = ( strategy->applyTo( *this ) );

}

void Image::Picture::convertFrom(
	const	Picture &	pOriginal,
	const	PictureFormat &	pNewFormat ) {

	if ( pNewFormat == pOriginal.getFormat() ) {
		operator = ( pOriginal );
		return;
	}

	ConverterManager::instance()
			->getStrategyFor( pOriginal.getFormat(), pNewFormat )
			->applyBetween( pOriginal, *this );

}

// ============================================================================

void Image::Picture::forcePlanar() {

// FIXME: stupid algo.

	PictureFormat oFormat = picF.getPlanar();

	if ( oFormat != picF ) {
		convertTo( oFormat );
	}

}

// ============================================================================

void Image::Picture::flipLR() {

	testConvertible();

	for ( Uint32 i = 0 ; i < picP.getSize() ; i++ ) {
		picP[ i ].flipLR();
	}

}

void Image::Picture::flipTB() {

	testConvertible();

	for ( Uint32 i = 0 ; i < picP.getSize() ; i++ ) {
		picP[ i ].flipTB();
	}

}

// ============================================================================

void Image::Picture::resize(
	const	Uint32		newW,
	const	Uint32		newH ) {

	testConvertible();

	if ( ! picF.canRepresent( newW, newH ) ) {
		throw InvalidArgument();
	}

	for ( Uint32 i = 0 ; i < picP.getSize() ; i++ ) {
		picP[ i ].resize(
			picF.getWidthPerPlane( i, newW ),
			picF.getHeightPerPlane( i, newH ) );
	}

	picW = newW;
	picH = newH;

}

void Image::Picture::downscaleXY2() {

	testConvertible();

	for ( Uint32 i = 0 ; i < picP.getSize() ; i++ ) {
		picP[ i ].downsampleXY2();
	}

	picW >>= 1;
	picH >>= 1;

}

void Image::Picture::downscaleXY4() {

	testConvertible();

	for ( Uint32 i = 0 ; i < picP.getSize() ; i++ ) {
		picP[ i ].downsampleXY4();
	}

	picW >>= 2;
	picH >>= 2;

}

// ============================================================================

Image::Picture Image::Picture::getClip(
	const	Uint32		pStartX,
	const	Uint32		pStartY,
	const	Uint32		pClipW,
	const	Uint32		pClipH ) const {

	if ( pStartX >= getWidth()
	  || pStartY >= getHeight() ) {
		throw InvalidArgument( "getClip(): start out of range!" );
	}

	Uint32	rectX = pStartX;
	Uint32	rectY = pStartY;

	Uint32	rectW = ( rectX + pClipW >= getWidth()
			? getWidth() - rectX
			: pClipW );
	Uint32	rectH = ( rectY + pClipH >= getHeight()
			? getHeight() - rectY
			: pClipH );

	Picture res( rectW, rectH, getFormat() );

	for ( Uint32 i = 0 ; i < getNbrPlanes() ; i++ ) {
		getPlane( i ).getClip(
			res.privGetPlane( i ),
			rectX,
			rectY );
	}

	return res;

}

void Image::Picture::setClip(
	const	Picture &	pClip,
	const	Uint32		pStartX,
	const	Uint32		pStartY ) {

	if ( pStartX + pClip.getWidth() > getWidth()
	  || pStartY + pClip.getHeight() > getHeight() ) {
		throw InvalidArgument( "setClip(): stop out of range!" );
	}

	Uint32	rectX = pStartX;
	Uint32	rectY = pStartY;

	for ( Uint32 i = 0 ; i < getNbrPlanes() ; i++ ) {
		privGetPlane( i ).setClip(
			pClip.getPlane( i ),
			rectX,
			rectY );
	}

}

// ============================================================================

void Image::Picture::randomize() {

	for ( Uint32 i = 0 ; i < picP.getSize() ; i++ ) {
		picP[ i ].randomize();
	}

}

// ============================================================================

void Image::Picture::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	Dyn::VarUser::doSerialize( pOStream );

	pOStream->putLEUint32( picW );
	pOStream->putLEUint32( picH );
	picF.doSerialize( pOStream );

	Uint32 nbr = picF.getNbrPlanes();

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		picP[ i ].doSerialize( pOStream );
	}

	pOStream->flush();

}

void Image::Picture::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	Dyn::VarUser::unSerialize( pIStream );

	picW = pIStream->getLEUint32();
	picH = pIStream->getLEUint32();
	picF.unSerialize( pIStream );

	Uint32 nbr = picF.getNbrPlanes();

	picP.resize( nbr );

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		picP[ i ].unSerialize( pIStream );
	}

	checkValidness();

}

// ============================================================================

void Image::Picture::checkValidness() {

	if ( picF.isUnknown() ) {
		throw InvalidArgument(
			"Unknown format!" );
	}

	if ( picF.getNbrPlanes() != picP.getSize() ) {
		throw InvalidArgument(
			"Number of planes mismatch (given: "
			+ Buffer( picP.getSize() )
			+ ", expected: "
			+ Buffer( picF.getNbrPlanes() )
			+ " for "
			+ picF.getShortName()
			+ ")!" );
	}

	if ( ! picF.canRepresent( picW, picH ) ) {
		throw InvalidArgument(
			"Format incompatible with dims ("
			+ Buffer( picW )
			+ "x"
			+ Buffer( picH )
			+ " with \""
			+ picF.getShortName()
			+ "\")!" );
	}

	for ( Uint32 i = 0 ; i < picF.getNbrPlanes() ; i++ ) {

		// Dimensions must match!

		if ( picP[ i ].getWidth() != picF.getWidthPerPlane( i, picW ) ) {
			throw InvalidArgument(
				"Invalid width for plane[ "
				+ Buffer( i )
				+ " ]! Exp: "
				+ Buffer( picF.getWidthPerPlane( i, picW ) )
				+ ", got: "
				+ Buffer( picP[ i ].getWidth() ) );
		}

		if ( picP[ i ].getHeight() != picF.getHeightPerPlane( i, picH ) ) {
			throw InvalidArgument(
				"Invalid height for plane[ "
				+ Buffer( i )
				+ " ]! Exp: "
				+ Buffer( picF.getHeightPerPlane( i, picH ) )
				+ ", got: "
				+ Buffer( picP[ i ].getHeight() ) );
		}

		if ( picF.getPlane( i ).isUnknown() ) {
			throw InvalidArgument(
				"Unknown plane[ "
				+ Buffer( i )
				+ " ] format!" );
		}

		if ( picP[ i ].getFormat().isUnknown() ) {
			picP[ i ].setFormat( picF.getPlane( i ) );
		}
		else if ( picF.getPlane( i ) != picP[ i ].getFormat() ) {
			// WARNING!
			picP[ i ].setFormat( picF.getPlane( i ) );
		}

	}

}

// ============================================================================

void Image::Picture::testConvertible() const {

	if ( picF.isUserDefined() ) {
		throw InvalidArgument( "User types not convertible!" );
	}

	if ( picF.getPredefined() == PictureFormat::Unknown ) {
		throw InvalidArgument( "Unknown type not convertible!" );
	}

}

// ============================================================================

