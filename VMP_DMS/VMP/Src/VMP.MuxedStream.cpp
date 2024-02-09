// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedStream.h"
#include "VMP.StreamFactory.h"
#include "VMP.StreamSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( MuxedStream )

// ============================================================================

const DL::TypeCPtr & MuxedStream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "90400f1a-1b97-49db-bf5a-0c81277db65e" ),
		"VMP.MuxedStream",
		Stream::getClassType() );

	return i;

}

// ============================================================================

MuxedStream::MuxedStream(
	const	MuxedStream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	Stream		( pOther ),
	format		( pOther.format ) {

	streams.resize( pOther.streams.getSize() );

	for ( Uint32 i = 0 ; i < streams.getSize() ; i++ ) {
		streams[ i ] = pOther.streams[ i ]->clone();
	}

}

// ============================================================================

StreamPtr MuxedStream::clone() const {

	return new MuxedStream( *this );

}

// ============================================================================

Bool MuxedStream::isCompatibleWith(
		StreamCPtr	pStream ) const {

	if ( ! pStream->inherits( MuxedStream::getClassType() ) ) {
		return false;
	}

	MuxedStreamCPtr mStream = pStream;

	if ( getMuxedFormat() != mStream->getMuxedFormat()
	  || getNbrComponents() != mStream->getNbrComponents() ) {
		return false;
	}

	for ( Uint32 i = 0 ; i < getNbrComponents() ; i++ ) {
		if ( ! getComponent( i )->isCompatibleWith(
				mStream->getComponent( i ) ) ) {
			return false;
		}
	}

	return true;

}

// ============================================================================

StreamPtr MuxedStream::findComponent(
	const	DL::TypeCPtr &	pClassType ) const {

	StreamPtr s, t;

	for ( Uint32 i = 0 ; i < getNbrComponents() ; i++ ) {
		t = getComponent( i );
		if ( t->inherits( pClassType ) ) {
			s = t;
			break;
		}
	}

	return s;

}

// ============================================================================

Buffer MuxedStream::toBuffer() const {

	Buffer res = Stream::toBuffer() + "\n"
		+ "MuxedStream" + "\n"
		+ "    format: " + format.name();

	for ( Uint32 i = 0 ; i < streams.getSize() ; i++ ) {
		res += "\n";
		res += "Muxed [ " + Buffer( i ) + " ]\n";
		res += streams[ i ]->toBuffer();
	}

	return res;

}

// ============================================================================

void MuxedStream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	Stream::doSerialize( out );

	format.doSerialize( out );

	out->putLEUint32( streams.getSize() );

	for ( Uint32 i = 0 ; i < streams.getSize() ; i++ ) {
		StreamSerializer::doSerialize( out, streams[ i ] );
	}

}

void MuxedStream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Stream::unSerialize( in );

	format.unSerialize( in );

	Uint32 size = in->getLEUint32();

	streams.kill();
	streams.resize( size );

	for ( Uint32 i = 0 ; i < size ; i++ ) {
		streams[ i ] = StreamSerializer::unSerialize( in );
	}

}

// ============================================================================

static StreamFactoryTmpl< MuxedStream > muxedStreamFactory;

// ============================================================================

