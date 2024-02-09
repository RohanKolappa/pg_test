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
//	VMP.DataStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.DataStream.h"
#include "VMP.StreamFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( DataStream )

// ============================================================================

const DL::TypeCPtr & DataStream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "010427af-6a48-4828-ae44-f73b629e4de5" ),
		"VMP.DataStream",
		ElementaryStream::getClassType() );

	return i;

}

// ============================================================================

DataStream::DataStream() :

	DL::Object	( getClassType() ) {

}

DataStream::DataStream(
	const	DataStream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	ElementaryStream( pOther ) {

}

// ============================================================================

DataStream& DataStream::operator = (
	const	DataStream&	pOther ) {

	ElementaryStream::operator = ( pOther );

	return *this;

}

// ============================================================================

StreamPtr DataStream::clone() const {

	return new DataStream( *this );

}

// ============================================================================

Bool DataStream::isCompatibleWith(
		StreamCPtr	pStream ) const {

	return ( pStream->inherits( DataStream::getClassType() ) );

}

// ============================================================================

Buffer DataStream::toBuffer() const {

	return ElementaryStream::toBuffer() + "\n"
		"DataStream";

}

// ============================================================================

void DataStream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	ElementaryStream::doSerialize( out );

}

void DataStream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	ElementaryStream::unSerialize( in );

}

// ============================================================================

static StreamFactoryTmpl< DataStream > dataFactory;

// ============================================================================

