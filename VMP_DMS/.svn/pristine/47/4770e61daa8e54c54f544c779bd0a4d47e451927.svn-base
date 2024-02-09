// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Session.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.IndexSerializer.h"
#include "VMP.Session.h"
#include "VMP.SessionFactory.h"
#include "VMP.SessionSerializer.h"
#include "VMP.StreamSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( Session )

// ============================================================================
// VMP::Session
// ============================================================================

const DL::TypeCPtr & Session::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "bf09a26c-765a-43c3-a3ff-f3f0391743ea" ),
		"VMP.Session",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

const Uint32	Session::NoIndex = ( Uint32 )Uint32Max;

// ============================================================================

Session::Session(
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	length		( ( Uint64 )0 ),
	flags		( pFlags ),
	index		( NoIndex ),
	segmStrt	( false ) {

}

Session::Session(
		StreamPtr	pStream,
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	length		( ( Uint64 )0 ),
	flags		( pFlags ),
	index		( NoIndex ),
	stream		( pStream ),
	segmStrt	( false ) {

}

Session::Session(
	const	Session &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	length		( pOther.length ),
	flags		( pOther.flags  ),
	index		( NoIndex ),
	segmStrt	( pOther.segmStrt ) {

	if ( pOther.stream ) {
		stream = pOther.stream->clone();
	}

}

// ============================================================================

Buffer Session::getFlagsName() const {

	return getFlagsName( flags );

}

Buffer Session::getFlagsName(
	const	Flags		flags ) {

	static const char *flagsTable[] = {
		"Live",
		"Pausable",
		"Seekable"
	};

	return Buffer( flagsTable[ (Uint32) flags ] );

}

// ============================================================================

Buffer Session::toBuffer() const {

	Buffer res;

	res = "Session\n"
		"    length: " + ( hasLength() ? Buffer( length ) : Buffer( "n/a" ) ) + "\n"
		"    flags : " + getFlagsName() + "\n"
		"    index : " + ( hasIndex() ? Buffer( index ) : Buffer( "n/a" ) ) + "\n"
		"    segmStrt: " + Buffer( segmStrt );

	return res;

}

// ============================================================================

void Session::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	out->putLEUint64( ( Uint64 )length );
	out->putLEUint32( ( Uint32 )flags );
	out->putLEUint32(           index );
	out->putUchar( segmStrt ? 0x01 : 0x00 );

	if ( stream ) {
		out->putUchar( 0x01 );
		StreamSerializer::doSerialize( out, stream );
	}
	else {
		out->putUchar( 0x00 );
	}

}

void Session::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	length = ( Uint64 )in->getLEUint64();
	flags  = ( Flags  )in->getLEUint32();
	index =           in->getLEUint32();
	segmStrt	= ( in->getUchar() == 0x01 );

	Uchar c = in->getUchar();

	if ( c == 0x01 ) {
		stream = StreamSerializer::unSerialize( in );
	}
	else if ( c == 0x00 ) {
		stream.kill();
	}
	else {
		throw InternalError( "Can't unserialize Session: corrupted stream!" );
	}

}

// ============================================================================

