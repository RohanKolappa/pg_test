// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Stats.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.Stats.h"
#include "VMP.StreamSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::StatsFrame
// ============================================================================

BFC_PTR_IMPL( StatsFrame )

// ============================================================================

SPtr StatsFrame::clone() const {

	return new StatsFrame( *this );

}

// ============================================================================

void StatsFrame::doSerialize(
		IO::ByteOutputStreamPtr		out ) const {

	Frame::doSerialize( out );

	out->putDouble( v );

//	out->putLEUint32( data.length() );
//	out->putBytes( data );
//	out->putLEUint32( start );

}

void StatsFrame::unSerialize(
		IO::ByteInputStreamPtr		in ) {

	Frame::unSerialize( in );

	v = in->getDouble();

//	Uint32 l = in->getLEUint32();
//	data = in->getBytes( l );
//	start = in->getLEUint32();

}

// ============================================================================

const DL::TypeCPtr & StatsFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "7dc06011-aab3-417b-89c0-a0a6ea147b9d" ),
		"VMP.StatsFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================
// VMP::StatsStream
// ============================================================================

BFC_PTR_IMPL( StatsStream )

// ============================================================================

StatsStream::StatsStream() :

	DL::Object	( getClassType() ) {

}

StatsStream::StatsStream(
	const	StatsStream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	ElementaryStream( pOther ) {

}

// ============================================================================

StatsStream& StatsStream::operator = (
	const	StatsStream&	pOther) {

	ElementaryStream::operator = ( pOther );

	return *this;

}

// ============================================================================

StreamPtr StatsStream::clone() const {

	return new StatsStream( *this );

}

// ============================================================================

Bool StatsStream::isCompatibleWith(
		StreamCPtr	/* pStream */ ) const {

	return true;

}

// ============================================================================

//Buffer StatsStream::toBuffer() const {
//
//	return ElementaryStream::toBuffer();
//
//}

// ============================================================================

void StatsStream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	ElementaryStream::doSerialize( out );

}

void StatsStream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	ElementaryStream::unSerialize( in );

}

// ============================================================================

const DL::TypeCPtr & StatsStream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "5697be07-130a-41db-a9b3-6ece6633e822" ),
		"VMP.StatsStream",
		ElementaryStream::getClassType() );

	return i;

}

// ============================================================================
// VMP::StatsSession
// ============================================================================

BFC_PTR_IMPL( StatsSession )

// ============================================================================

StatsSession::StatsSession(
	const	Flags		f ) :

	DL::Object	( getClassType() ),
	Session		( f ) {

}

StatsSession::StatsSession(
	const	StatsSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable(),
	Session		( pOther ) {

	if ( pOther.content ) {
		content = pOther.content->clone();
	}

}

// ============================================================================

SessionPtr StatsSession::clone() const {

	return new StatsSession( *this );

}

// ============================================================================

Bool StatsSession::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	return true;

}

// ============================================================================

Buffer StatsSession::toBuffer() const {

	return Session::toBuffer() + "\n"
		+ "StatsSession" + "\n"
//		+ "    bitRate: " + Buffer(bitRate) + "\n"
		+ content->toBuffer() /* + "\n"
		+ (index ? index->toBuffer() : Buffer("No index.")) */ ;

}

// ============================================================================

void StatsSession::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	Session::doSerialize( out );

//	out->putLEUint32( bitRate );

	if ( content ) {
		out->putUchar( 0x01 );
		StreamSerializer::doSerialize( out, content );
	}
	else {
		out->putUchar( 0x00 );
	}

//	if ( index ) {
//		out->putUchar( 0x01 );
//		IndexSerializer::doSerialize( out, index );
//	}
//	else {
//		out->putUchar( 0x00 );
//	}

}

void StatsSession::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Session::unSerialize( in );

//	bitRate = in->getLEUint32();

	Uchar c;

	c = in->getUchar();

	if ( c == 0x01 ) {
		content = StreamSerializer::unSerialize( in );
	}
	else {
		content.kill();
	}

//	c = in->getUchar();
//
//	if ( c == 0x01 ) {
//		index = IndexSerializer::unSerialize( in );
//	}
//	else {
//		index.kill();
//	}

}

// ============================================================================

const DL::TypeCPtr & StatsSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "012571e9-6946-4a7b-a92b-7a7b3ec4e6a7" ),
		"VMP.StatsSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

