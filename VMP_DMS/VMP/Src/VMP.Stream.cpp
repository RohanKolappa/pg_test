// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Stream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.Time.ClockSerializer.h"

#include "VMP.Stream.h"
#include "VMP.StreamSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( Stream )

// ============================================================================

const DL::TypeCPtr & Stream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1fe2d133-b957-4607-93ef-a21d2f5e4535" ),
		"VMP.Stream",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

const Uint32 Stream::NoStreamId = ( Uint32 )Uint32Max;

// ============================================================================

Stream::Stream() :

	DL::Object	( getClassType() ),
	streamId	( NoStreamId ) {

}

Stream::Stream(
	const	Stream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	() {

	title		= pOther.title;
	streamId	= pOther.streamId;
	extraData	= pOther.extraData;

}

// ============================================================================

Stream& Stream::operator = (
	const	Stream &		pOther) {

	if ( this != &pOther ) {
		title		= pOther.title;
		streamId	= pOther.streamId;
		extraData	= pOther.extraData;
	}

	return *this;

}

// ============================================================================

Buffer Stream::toBuffer() const {

	Buffer res =
		"Stream\n"
		"    title   : \"" + title + "\"" + "\n"
		"    streamId: " + ( hasStreamId() ? Buffer( streamId ) : Buffer( "n/a" ) );

	if ( getExtraData().hasContent() ) {
		res += "\n    extra   : " + Utils::toHex( getExtraData() );
	}

	return res;

}

// ============================================================================

void Stream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	out->putLEUint32( title.length() );
	out->putBytes( title );
	out->putLEUint32( ( Uint32 )streamId );
	out->putLEUint32( extraData.getLength() );
	if ( extraData.hasContent() ) {
		out->putBytes( extraData );
	}

}

void Stream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Uint32 l = in->getLEUint32();
	title = in->getBytes( l );
	streamId = in->getLEUint32();

	Uint32 tmp = in->getLEUint32();

	if ( tmp ) {
		extraData = in->getBytes( tmp );
	}
	else {
		extraData.kill();
	}

}

// ============================================================================

