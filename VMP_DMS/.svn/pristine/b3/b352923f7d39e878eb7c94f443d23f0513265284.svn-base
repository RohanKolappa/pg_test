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
//	VMP.ElementaryStream.cpp
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

#include "VMP.ElementaryStream.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( ElementaryStream )

// ============================================================================

const DL::TypeCPtr & ElementaryStream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "5eea0119-2565-4bc7-a34a-8d79daf71568" ),
		"VMP.ElementaryStream",
		Stream::getClassType() );

	return i;

}

// ============================================================================

ElementaryStream::ElementaryStream() :

	DL::Object	( getClassType() ) {

}

ElementaryStream::ElementaryStream(
	const	ElementaryStream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	Stream		( pOther ),
	fourCC		( pOther.fourCC ) {

}

// ============================================================================

ElementaryStream& ElementaryStream::operator = (
	const	ElementaryStream&	pOther) {

	Stream::operator = ( pOther );

	fourCC = pOther.fourCC;

	return *this;

}

// ============================================================================

Buffer ElementaryStream::toBuffer() const {

	return Stream::toBuffer() + "\n"
		+ "ElementaryStream" + "\n"
		+ "    fourCC: "
		+ ( ( Uint32 )fourCC == 0 ? Buffer( "<NULL>" ) : fourCC.toBuffer( true ) );

}

// ============================================================================

void ElementaryStream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	Stream::doSerialize( out );

	out->putFourCC( fourCC );

}

void ElementaryStream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Stream::unSerialize( in );

	fourCC = in->getFourCC();

}

// ============================================================================

