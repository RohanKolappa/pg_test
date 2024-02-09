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
//	VMP.AVC.CodecConfig.cpp
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
#include "BFC.IO.MemoryInputStream.h"
#include "BFC.IO.MemoryOutputStream.h"

#include "VMP.ConfigInfoRegisterer.h"

#include "VMP.AVC.CodecConfig.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.PictureParameterSet.h"
#include "VMP.AVC.SequenceParameterSet.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, CodecConfig )

// ============================================================================

const DL::TypeCPtr & AVC::CodecConfig::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "d44efcfa-cf05-4cc2-8d26-c62b5fd690d5" ),
		"VMP.AVC.CodecConfig",
		ConfigInfo::getClassType() );

	return i;

}

// ============================================================================

AVC::CodecConfig::CodecConfig() :

	ConfigInfo	( getClassType() ),

	confVers	( 1 ),
	prfIndic	( 0 ),
	prfCompt	( 0 ),
	lvlIndic	( 0 ),
	lngthSze	( 3 ) {

}

// ============================================================================

void AVC::CodecConfig::setProfileIndication(
	const	Uchar		pCode ) {

	if ( ! prfIndic ) {
		prfIndic = pCode;
	}
	else if ( prfIndic != pCode ) {
		throw InvalidArgument( "Mutating profile_idc!" );
	}

}

void AVC::CodecConfig::setProfileCompatibility(
	const	Uchar		pCode ) {

	prfCompt = pCode;

}

void AVC::CodecConfig::setLevelIndication(
	const	Uchar		pCode ) {

	if ( ! lvlIndic ) {
		lvlIndic = pCode;
	}
	else if ( lvlIndic != pCode ) {
		throw InvalidArgument( "Mutating level_idc!" );
	}

}

Bool AVC::CodecConfig::hasSPS(
		NALUCPtr	pSPS ) const {

	for ( Uint32 i = 0 ; i < spsArray.getSize() ; i++ ) {
		if ( spsArray[ i ]->getData() == pSPS->getData() ) {
			return true;
		}
	}

	return false;

}

void AVC::CodecConfig::addSPS(
		NALUCPtr	pSPS ) {

	if ( ! hasSPS( pSPS ) ) {
		if ( pSPS->getType() != NALU::TypeSPS ) {
			throw InvalidArgument( "Not a NALU!" );
		}
		spsArray += pSPS;
		// Check profile, compat, level.
		if ( ! prfIndic
		  && ! prfCompt
		  && ! lvlIndic ) {
			prfIndic = pSPS->getData()[ 1 ];
			prfCompt = pSPS->getData()[ 2 ];
			lvlIndic = pSPS->getData()[ 3 ];
		}
		else if ( prfIndic != pSPS->getData()[ 1 ]
		       || prfCompt != pSPS->getData()[ 2 ]
		       || lvlIndic != pSPS->getData()[ 3 ] ) {
			throw InvalidArgument( "Mutating prof/compat/level!" );
		}
	}

}

Bool AVC::CodecConfig::hasPPS(
		NALUCPtr	pPPS ) const {

	for ( Uint32 i = 0 ; i < ppsArray.getSize() ; i++ ) {
		if ( ppsArray[ i ]->getData() == pPPS->getData() ) {
			return true;
		}
	}

	return false;

}

void AVC::CodecConfig::addPPS(
		NALUCPtr	pPPS ) {

	if ( ! hasPPS( pPPS ) ) {
		if ( pPPS->getType() != NALU::TypePPS ) {
			throw InvalidArgument( "Not a NALU!" );
		}
		ppsArray += pPPS;
	}

}

// ============================================================================

Bool AVC::CodecConfig::isInitialized() const {

	return ( prfIndic != 0
	      && lvlIndic != 0
	      && ! spsArray.isEmpty()
	      && ! ppsArray.isEmpty() );

}

// ============================================================================

void AVC::CodecConfig::decode(
	const	Buffer &	pData ) {

	IO::MemoryInputStreamPtr	mStr = new IO::MemoryInputStream( pData );
	IO::ByteInputStreamPtr		bStr = new IO::ByteInputStream( mStr );

	bStr->open();

	Uchar c = bStr->getUchar();

	if ( c != 0x01 ) {
		throw InternalError( "Invalid version: " + Buffer( ( Uint32 )c ) );
	}

	confVers = c;

	prfIndic = bStr->getUchar();
	prfCompt = bStr->getUchar();
	lvlIndic = bStr->getUchar();

	c = bStr->getUchar();

	if ( ( c & 0xFC ) != 0xFC ) {
		throw InternalError( "Invalid length size: " + Buffer( ( Uint32 )c ) );
	}

	lngthSze = ( c & 0x03 );

	c = bStr->getUchar();

	if ( ( c & 0xE0 ) != 0xE0 ) {
		throw InternalError( "Invalid nbr of SPS's: " + Buffer( ( Uint32 )c ) );
	}

	Uint32	n = ( Uint32 )( c & 0x1F );

	spsArray.kill();

	for ( Uint32 i = 0 ; i < n ; i++ ) {
		Uint32	l = ( Uint32 )bStr->getBEUint16();
		spsArray += new NALU( bStr->getBytes( l ) );
	}

	c = bStr->getUchar();

	n = ( Uint32 )c;

	ppsArray.kill();

	for ( Uint32 i = 0 ; i < n ; i++ ) {
		Uint32	l = ( Uint32 )bStr->getBEUint16();
		ppsArray += new NALU( bStr->getBytes( l ) );
	}

}

Buffer AVC::CodecConfig::encode() const {

	IO::MemoryOutputStreamPtr	mStr = new IO::MemoryOutputStream;
	IO::ByteOutputStreamPtr		bStr = new IO::ByteOutputStream( mStr );

	bStr->open();

	bStr->putUchar( confVers );
	bStr->putUchar( prfIndic );
	bStr->putUchar( prfCompt );
	bStr->putUchar( lvlIndic );
	bStr->putUchar( 0xFC | lngthSze );

	bStr->putUchar( 0xE0 | spsArray.getSize() );
	for ( Uint32 i = 0 ; i < spsArray.getSize() ; i++ ) {
		bStr->putBEUint16( spsArray[ i ]->getData().getLength() );
		bStr->putBytes( spsArray[ i ]->getData() );
	}

	bStr->putUchar( ppsArray.getSize() );
	for ( Uint32 i = 0 ; i < ppsArray.getSize() ; i++ ) {
		bStr->putBEUint16( ppsArray[ i ]->getData().getLength() );
		bStr->putBytes( ppsArray[ i ]->getData() );
	}

	bStr->flush();

	return mStr->getContent();

}

// ============================================================================

Buffer AVC::CodecConfig::toBuffer() const {

	Buffer res = "Prof: " + Buffer( ( Uint32 )prfIndic, Buffer::Base16, 2 )
		+ ", Comp: " + Buffer( ( Uint32 )prfCompt, Buffer::Base16, 2 )
		+ ", Levl: " + Buffer( ( Uint32 )lvlIndic, Buffer::Base16, 2 );

	for ( Uint32 i = 0 ; i < spsArray.getSize() ; i++ ) {
		SequenceParameterSetPtr sps = new SequenceParameterSet;
		sps->decode( spsArray[ i ] );
		res += "\nSPS[ " + Buffer( i ) + " ]\n" + sps->toBuffer();
	}

	for ( Uint32 i = 0 ; i < ppsArray.getSize() ; i++ ) {
		PictureParameterSetPtr pps = new PictureParameterSet;
		pps->decode( ppsArray[ i ] );
		res += "\nPPS[ " + Buffer( i ) + " ]\n" + pps->toBuffer();
	}

	return res;

}

// ============================================================================

static ConfigInfoRegisterer< AVC::CodecConfig > registerer;

// ============================================================================

