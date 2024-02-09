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
//	VMP.AudioFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Sound.Block.h"
#include "BFC.Sound.Format.h"

#include "VMP.AudioFrame.h"
#include "VMP.FrameFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( AudioFrame )

// ============================================================================

const DL::TypeCPtr & AudioFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "13949a51-dfad-4d33-b34a-c238f019f28b" ),
		"VMP.AudioFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

SPtr AudioFrame::clone() const {

	return new AudioFrame( *this );

}

// ============================================================================

void AudioFrame::setData(
	const	Buffer &	pPayload,
		Sound::FormatCPtr
				pFormat ) {

	block = new Sound::Block( pFormat, pPayload );

}

Buffer AudioFrame::getData() const {

	return ( block ? block->getData() : Buffer() );

}

Uint32 AudioFrame::getLength() const {

	return ( block ? block->getLength() : 0 );

}

// ============================================================================

Buffer AudioFrame::toBuffer() const {

	return Frame::toBuffer() + ", audio: " + Utils::toHex( getData()( 0, 128 ) );

}

// ============================================================================

void AudioFrame::doSerialize(
		IO::ByteOutputStreamPtr		out ) const {

	Frame::doSerialize( out );

	throw NotImplemented( "VMP::AudioFrame::doSerialize()" );

//	Buffer	buf = getData();
//	Uint32	len = getLength();
//
//	out->putLEUint32( buf.length() );
//	out->putBytes( buf );
//	out->putLEUint32( len );

}

void AudioFrame::unSerialize(
		IO::ByteInputStreamPtr		in ) {

	Frame::unSerialize( in );

	throw NotImplemented( "VMP::AudioFrame::unSerialize()" );

//	Uint32 l = in->getLEUint32();
//	data = in->getBytes( l );
//	length = in->getLEUint32();

}

// ============================================================================

static FrameFactoryTmpl< AudioFrame > audioFactory;

// ============================================================================

