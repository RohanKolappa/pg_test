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
//	VMP.L2.BytesSessionProducer.cpp
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

#include "VMP.L2.BytesSessionProducer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, BytesSessionProducer )

// ============================================================================

L2::BytesSessionProducer::BytesSessionProducer(
	const	Buffer&		pName ) :

	FinalPullEngine( getClassType() ) {

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	setObjectName( "VMP.L2.BytesSessionProducer" );

	name = pName;

}

// ============================================================================

SessionPtr L2::BytesSessionProducer::getSessionCallback() {

//	msgDbg( "getSessionCB(): opening \"" + name + "\"..." );

	file = new IO::File( name, IO::File::Read );

	try {
		file->open();
	}
	catch ( Exception & e ) {
		throw NotSuitable( e );
	}

	BytesSessionPtr res = new BytesSession( Session::Seekable );

	res->setLength( file->length() );

	setInSession( true );

	return res;

}

void L2::BytesSessionProducer::endSessionCallback() {

	file->close();

	file.kill();

	setInSession( false );

}

FramePtr L2::BytesSessionProducer::getFrameCallback(
	const	Uint64		index ) {

//	msgDbg( "getFrameCallback(): req: "
//		+ Buffer( index )
//		+ " / cur: "
//		+ Buffer( file->tell() )
//		+ " / len: "
//		+ Buffer( file->length() ) );

	if ( index != Frame::NoOffset
	  && index != file->tell() ) {
//		msgWrn( "getFrameCallback(): seeking from "
//			+ Buffer( file->tell() )
//			+ " to "
//			+ Buffer( index )
//			+ " in raw input file!" );
		file->seek( index );
	}

	Buffer	buf;
	Uint64	pos = file->tell();

	try {
		buf = file->getBytes();
	}
	catch ( IO::EndOfStream& ) {
		throw EndOfStream();
	}
	catch ( IO::BrokenPipe& ) {
		file.kill();
		setInSession( false );
		throw BrokenSession();
	}

	FramePtr res = new BytesFrame( buf );

	res->setOffset( pos );

	return res;

}

// ============================================================================

