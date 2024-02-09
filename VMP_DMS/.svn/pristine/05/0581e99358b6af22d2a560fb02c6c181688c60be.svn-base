// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.SessionProducer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.IO.File.h"

#include "VMP.FrameSerializer.h"
#include "VMP.SessionSerializer.h"
#include "VMP.VideoSession.h"
#include "VMP.VideoFrame.h"

#include "VMP.L2.SessionProducer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::L2::VideoSessionProducer
// ============================================================================

L2::VideoSessionProducer::VideoSessionProducer(
	const	Buffer&		pName ) :

	FinalPullEngine( getClassType() ) {

	setNbrOutputs( 1 );
	setOutputType( 0, VideoSession::getClassType() );

	setObjectName( "VMP.L2.VideoSessionProducer" );

	name = pName;

}

// ============================================================================

SessionPtr L2::VideoSessionProducer::getSessionCallback() {

	// Interpret the 'name' argument...

	BufferArray fields = BufferParser::tokenize( name, ":" );

	if ( fields.size() != 4 ) {
		throw NotSuitable( "Need exactly 4 fields in filename!" );
	}

	Buffer fnam = fields[ 0 ];
	picW = fields[ 1 ].toUint32();
	picH = fields[ 2 ].toUint32();
	Buffer tmpF = fields[ 3 ];

	Uint32 i;

	for ( i = Image::PictureFormat::MinFormat
	    ; i < Image::PictureFormat::MaxFormat
	    ; i++ ) {
		picF = Image::PictureFormat( (Image::PictureFormat::Format)( i ) );
		if ( tmpF == picF.shortName() ) {
			break;
		}
	}

	if ( i >= Image::PictureFormat::MaxFormat ) {
		throw NotSuitable( "Invalid pixel format!" );
	}

	picS = 0;

	for ( i = 0 ; i < picF.getPlanesNumber() ; i++ ) {
		picS += picF.getBytesPerPlane( i, picW, picH );
	}


	IO::FilePtr file = new IO::File( fnam, IO::File::Read );

//	file->open();

	stream = new IO::ByteInputStream( file );

	stream->open();

	VideoSessionPtr res = new VideoSession( Session::Seekable );
	res->setContent( new VideoStream );

	res->setLength( stream->length() / picS );
	res->getContent()->setWidth( picW );
	res->getContent()->setHeight( picH );
	res->getContent()->setFormat( picF );

	setInSession( true );

	return res;

}

void L2::VideoSessionProducer::endSessionCallback() {

	if ( stream ) {
		stream.kill();
	}

	setInSession( false );

}

FramePtr L2::VideoSessionProducer::getFrameCallback(
	const	Uint64		index ) {

	if ( ! stream ) {
		throw EndOfStream();
	}

	Uint64 offset = index * picS;

	if ( index != Frame::NoOffset
	  && offset != stream->tell() ) {
//		msgWrn( "getFrame(): seeking from "
//			+ Buffer( stream->tell() )
//			+ " to "
//			+ Buffer( offset )
//			+ " in video input file!" );
		stream->seek( offset );
	}

	Image::PlaneArray planes;

	try {
		for ( Uint32 i = 0 ; i < picF.getPlanesNumber() ; i++ ) {
			planes += Image::Plane(
				stream->getBytes(
					picF.getBytesPerPlane(
						i, picW, picH ) ),
				picW,
				picH,
				picF.getBitsPerSamplePerPlane( i ) );
		}
	}
	catch ( IO::EndOfStream& ) {
		throw EndOfStream();
	}
	catch ( IO::BrokenPipe& ) {
		stream.kill();
		setInSession( false );
		throw BrokenSession();
	}

	FramePtr res = new VideoFrame( Image::Picture( picW, picH, picF, planes ) );

	res->setOffset( index );

	return res;

}

// ============================================================================
// VMP::L2::SessionUnserializer
// ============================================================================

L2::SessionUnserializer::SessionUnserializer(
		IO::DevicePtr		__idevice,
	const	Bool			__reopen,
	const	DL::TypeCPtr &	__classId ) :

	FinalPullEngine( getClassType() ) {

	setObjectName( "VMP.L2.SessionUnserializer" );

	setNbrOutputs( 1 );
	setOutputType( 0, __classId );

	reopen = __reopen;
	stream = new IO::ByteInputStream( __idevice );
	classId = __classId;

}

// ============================================================================

SessionPtr L2::SessionUnserializer::getSessionCallback() {

	if ( ! stream->isConnected() ) {
		try {
			stream->open();
		}
		catch ( Exception & e ) {
			throw InputError( e, "Can't open input device!" );
		}
	}

	SessionPtr sess = SessionSerializer::unSerialize( stream );

	if ( ! sess->inherits( classId ) ) {
		throw InputError( "Invalid session from input device!" );
	}

	setInSession( true );

	return sess;

}

void L2::SessionUnserializer::endSessionCallback() {

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

FramePtr L2::SessionUnserializer::getFrameCallback(
	const	Uint64		/* index */ ) {

	if ( ! stream->isConnected() ) {
		throw EndOfStream();
	}

	FramePtr res;

	try {
		res = FrameSerializer::unSerialize( stream );
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't unserialize frame!" );
		throw EndOfStream();
	}

	return res;

}

// ============================================================================

