// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.VideoStream.cpp
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

#include "VMP.StreamFactory.h"
#include "VMP.VideoStream.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( VMP, VideoStream )

// ============================================================================

const DL::TypeCPtr & VideoStream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "c8d67fb2-34e5-4aa8-929c-8009449b7a70" ),
		"VMP.VideoStream",
		ElementaryStream::getClassType() );

	return i;

}

// ============================================================================

VideoStream::VideoStream() :

	DL::Object	( getClassType() ),
	frmeRate	( 0, 1 ),
	width		( 0 ),
	height		( 0 ),
	flipped		( false ),
	compression	( VideoCompression::Unknown ),
	format		( Image::PictureFormat::Unknown ) {

}

VideoStream::VideoStream(
	const	VideoStream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	ElementaryStream( pOther ) {

	frmeRate	= pOther.frmeRate;
	width		= pOther.width;
	height		= pOther.height;
	flipped		= pOther.flipped;
	compression	= pOther.compression;
	format		= pOther.format;

}

// ============================================================================

VideoStream& VideoStream::operator = (
	const	VideoStream &	pOther ) {

	ElementaryStream::operator = ( pOther );

	frmeRate	= pOther.frmeRate;
	width		= pOther.width;
	height		= pOther.height;
	flipped		= pOther.flipped;
	compression	= pOther.compression;
	format		= pOther.format;

	return *this;

}

// ============================================================================

StreamPtr VideoStream::clone() const {

	return new VideoStream( *this );

}

// ============================================================================

Bool VideoStream::isCompatibleWith(
		StreamCPtr	pStream ) const {

	if ( ! pStream->inherits( VideoStream::getClassType() ) ) {
		return false;
	}

	VideoStreamCPtr vStream = pStream;

	return ( frmeRate == vStream->frmeRate
	      && width == vStream->getWidth()
	      && height == vStream->getHeight()
	      && flipped == vStream->isFlipped()
	      && compression == vStream->getCompression()
	      && format == vStream->getFormat() );

}

// ============================================================================

Buffer VideoStream::toBuffer() const {

	return ElementaryStream::toBuffer() + "\n"
		"VideoStream\n"
		"    rate   : " + frmeRate.toBuffer() + "\n"
		"    width  : " + Buffer(width) + "\n"
		"    height : " + Buffer(height) + "\n"
		"    flipped: " + Buffer( flipped ) + "\n"
		"    compr  : " + compression.getName() + "\n"
		"    format : " + format.name();

}

// ============================================================================

void VideoStream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	ElementaryStream::doSerialize( out );

	out->putLEUint32( ( Uint32 )frmeRate.getNum() );
	out->putLEUint32( ( Uint32 )frmeRate.getDen() );
	out->putLEUint32( width );
	out->putLEUint32( height );
	out->putUchar( flipped ? 0x01 : 0x00 );

	compression.doSerialize( out );

	format.doSerialize( out );

}

void VideoStream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	ElementaryStream::unSerialize( in );

	frmeRate.setNum( in->getLEUint32() );
	frmeRate.setDen( in->getLEUint32() );
	width = in->getLEUint32();
	height = in->getLEUint32();
	flipped = ( in->getUchar() == 0x01 );

	compression.unSerialize( in );

	format.unSerialize( in );

}

// ============================================================================

static StreamFactoryTmpl< VideoStream > videoFactory;

// ============================================================================

