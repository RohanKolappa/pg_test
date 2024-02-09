// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.SessionConsumer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.BytesSession.h"
#include "VMP.BytesFrame.h"
#include "VMP.FrameSerializer.h"
#include "VMP.SessionSerializer.h"
#include "VMP.VideoSession.h"
#include "VMP.VideoFrame.h"

#include "VMP.L2.SessionConsumer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

L2::BytesSessionConsumer::BytesSessionConsumer(
	const	Buffer&		__name ) :

	FinalPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setObjectName( "VMP.L2.BytesSessionConsumer" );

	name = __name;

}

// ============================================================================

void L2::BytesSessionConsumer::putSessionCallback(
		SessionPtr		/* session */ ) {

	file = new IO::File( name,
		IO::File::Write | IO::File::Truncate | IO::File::Create );

	file->open();

}

void L2::BytesSessionConsumer::endSessionCallback() {

	file->close();

	file.kill();

}

void L2::BytesSessionConsumer::putFrameCallback(
		FramePtr		frame ) {

	BytesFramePtr bf = frame;

	if ( frame->getOffset() != Frame::NoOffset
	  && frame->getOffset() != file->tell() ) {
//		msgWrn( "Seeking from "
//			+ Buffer( file->tell() )
//			+ " to "
//			+ Buffer( frame->getOffset() )
//			+ " in output file!" );
		file->seek( frame->getOffset() );
	}

	file->putBytes( bf->getData() );

}

// ============================================================================

L2::VideoSessionConsumer::VideoSessionConsumer(
	const	Buffer&		__name ) :

	FinalPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, VideoSession::getClassType() );

	setObjectName( "VMP.L2.VideoSessionConsumer" );

	name = __name;

}

L2::VideoSessionConsumer::~VideoSessionConsumer() {



}

// ============================================================================

void L2::VideoSessionConsumer::putSessionCallback(
		SessionPtr		session ) {

	VideoSessionPtr vsession = session;
	VideoStreamPtr vstream = vsession->getContent();

	picW = vstream->getWidth();
	picH = vstream->getHeight();
	picF = vstream->getFormat();
	picS = picF.getBytesPerPicture( picW, picH );

//	msgDbg( "Output parameters:" );
//	msgDbg( "    picture width : " + Buffer( picW ) + " pixels" );
//	msgDbg( "    picture height: " + Buffer( picH ) + " pixels" );
//	msgDbg( "    pixel format  : " + Buffer( picF.shortName() ) );
//	msgDbg( "--> picture size  : " + Buffer( picS ) + " bytes" );

	file = new IO::File( name,
		IO::File::Write | IO::File::Truncate | IO::File::Create );

	file->open();

}

void L2::VideoSessionConsumer::endSessionCallback() {

	if ( file ) {
		file->close();
		file.kill();
	}

}

void L2::VideoSessionConsumer::putFrameCallback(
		FramePtr		frame ) {

	if ( ! file ) {
		throw EndOfStream();
	}

	VideoFramePtr vf = frame;
	Image::Picture pic = vf->getPic();

	if ( frame->getOffset() != Frame::NoOffset ) {

		Uint64 offset = frame->getOffset() * picS;

		if ( offset != file->tell() ) {
//			msgWrn( "Seeking from "
//				+ Buffer( file->tell() )
//				+ " to "
//				+ Buffer( offset )
//				+ " in output video file!" );
			file->seek( offset );
		}

	}

	if ( pic.getFormat() != picF ) {
		msgWrn( "Picture in incorrect PictureFormat! This means\n"
			"your engine doesn't respect its engagements!\n"
			"The expected format, as announced in the\n"
			"VideoStream descriptor, was \""
			+ Buffer( picF.shortName() ) + "\", but I\n"
			"received a picture using \""
			+ Buffer( pic.getFormat().shortName() ) + "\"!" );
		pic.convertTo( picF );
	}

	for ( Uint32 i = 0 ; i < picF.getPlanesNumber() ; i++ ) {
		file->putBytes( pic.getPlane( i ).getData() );
	}

}

// ============================================================================

L2::SessionSerializer::SessionSerializer(
		IO::DevicePtr		__odevice,
	const	Bool			__reopen ) :

	FinalPushEngine( getClassType() ) {

	setObjectName( "VMP.L2.SessionSerializer" );

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	reopen = __reopen;
	stream = new IO::ByteOutputStream( __odevice );

}

L2::SessionSerializer::~SessionSerializer() {



}

// ============================================================================

void L2::SessionSerializer::putSessionCallback(
		SessionPtr		session ) {

	if ( ! stream->isConnected() ) {
		try {
			stream->open();
		}
		catch ( Exception & e ) {
			throw NotSuitable( e, "Can't open output device!" );
		}
	}

	try {
		VMP::SessionSerializer::doSerialize( stream, session );
	}
	catch ( Exception & e ) {
		throw NotSuitable( e, "Can't serialize session descriptor!" );
	}

	setInSession( true );

}

void L2::SessionSerializer::endSessionCallback() {

	if ( stream->isConnected() && reopen ) {
		try {
			stream->close();
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't close stream!" );
		}
	}

	setInSession( false );

}

void L2::SessionSerializer::putFrameCallback(
		FramePtr		frame ) {

	if ( ! stream->isConnected() ) {
		throw EndOfStream();
	}

	try {
		FrameSerializer::doSerialize( stream, frame );
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't serialize frame!" );
		throw EndOfStream();
	}

}

// ============================================================================

