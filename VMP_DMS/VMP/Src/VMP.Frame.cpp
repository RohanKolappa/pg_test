// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Frame.cpp
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

#include "VMP.Frame.h"
#include "VMP.FrameFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( Frame )

// ============================================================================

const DL::TypeCPtr & Frame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f3d42de5-c916-4c05-9464-1b6426ccc7c4" ),
		"VMP.Frame",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

const Uint64 Frame::NoOffset = ( Uint64 )Uint64Max;

// ============================================================================

Buffer Frame::toBuffer() const {

	return "Offs: "
		+ ( frmeOffs != NoOffset
			? Buffer( frmeOffs, Buffer::Base16, 8 )
			: Buffer( "----------" ) )
		+ ", PTS: " + framePts.dateTimeToBuffer( false, true )
		+ ", DTS: " + frameDts.dateTimeToBuffer( false, true )
		+ ", flgs: "
		+ ( keyFrame ? 'K' : '.' )
		+ ( segmStrt ? 'S' : '.' )
		+ ( dataDisc ? 'D' : '.' )
		+ ( timeDisc ? 'T' : '.' );

}

// ============================================================================

void Frame::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	pOStream->putLEUint64( frmeOffs );
	pOStream->putLEUint64( framePts );
	pOStream->putLEUint64( frameDts );
	pOStream->putUchar( keyFrame ? 0x01 : 0x00 );
	pOStream->putUchar( segmStrt ? 0x01 : 0x00 );
	pOStream->putUchar( dataDisc ? 0x01 : 0x00 );
	pOStream->putUchar( timeDisc ? 0x01 : 0x00 );

}

void Frame::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	frmeOffs = pIStream->getLEUint64();
	framePts = pIStream->getLEUint64();
	frameDts = pIStream->getLEUint64();
	keyFrame = ( pIStream->getUchar() != 0x00 );
	segmStrt = ( pIStream->getUchar() != 0x00 );
	dataDisc = ( pIStream->getUchar() != 0x00 );
	timeDisc = ( pIStream->getUchar() != 0x00 );

}

// ============================================================================

