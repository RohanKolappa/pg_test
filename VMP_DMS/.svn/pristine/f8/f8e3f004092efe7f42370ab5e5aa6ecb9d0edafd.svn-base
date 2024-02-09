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
//	VMP.AudioStream.cpp
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

#include "BFC.Sound.Format.h"

#include "VMP.AudioStream.h"
#include "VMP.StreamFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( AudioStream )

// ============================================================================

const DL::TypeCPtr & AudioStream::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "08e152dd-46f3-438a-930d-6ed4681b9bdc" ),
		"VMP.AudioStream",
		ElementaryStream::getClassType() );

	return i;

}

// ============================================================================

AudioStream::AudioStream() :

	DL::Object	( getClassType() ),
	sampRate	( 0 ),
	sampSize	( 0 ),
	channels	( 0 ),
	compress	( AudioCompression::Unknown ) {

}

AudioStream::AudioStream(
		BFC::Sound::FormatCPtr
					pFormat ) :

	DL::Object	( getClassType() ),
	sampRate	( 0 ),
	sampSize	( 0 ),
	channels	( 0 ),
	compress	( AudioCompression::Unknown ) {

	setFormat( pFormat );

}

AudioStream::AudioStream(
	const	AudioStream &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	ElementaryStream( pOther ),
	sampRate	( pOther.sampRate ),
	sampSize	( pOther.sampSize ),
	channels	( pOther.channels ),
	blockFmt	( pOther.blockFmt ),
	compress	( pOther.compress ) {

}

// ============================================================================

AudioStream& AudioStream::operator = (
	const	AudioStream&	pOther ) {

	ElementaryStream::operator = ( pOther );

	sampRate = pOther.sampRate;
	sampSize = pOther.sampSize;
	channels = pOther.channels;
	blockFmt = pOther.blockFmt;
	compress = pOther.compress;

	return *this;

}

// ============================================================================

StreamPtr AudioStream::clone() const {

	return new AudioStream( *this );

}

// ============================================================================

Bool AudioStream::isCompatibleWith(
		StreamCPtr	pStream ) const {

	if ( ! pStream->inherits( AudioStream::getClassType() ) ) {
		return false;
	}

	AudioStreamCPtr aStream = pStream;

	return ( sampRate == aStream->getSampRate()
	      && sampSize == aStream->getSampSize()
	      && channels == aStream->getChannels()
	      && compress == aStream->getCompression() );

}

// ============================================================================

void AudioStream::setFormat(
		Sound::FormatCPtr	pFormat ) {

	blockFmt	= pFormat;
	sampRate	= pFormat->getSampleRate();
	sampSize	= pFormat->getSampleSize();
	channels	= pFormat->getNbrChannels();

}

// ============================================================================

Buffer AudioStream::toBuffer() const {

	return ElementaryStream::toBuffer() + "\n"
		"AudioStream\n"
		"    rate (Hz)  : " + Buffer(sampRate) + "\n"
		"    size (bits): " + Buffer(sampSize) + "\n"
		"    channels   : " + Buffer(channels) + "\n"
		"    compression: " + compress.name();

}

// ============================================================================

void AudioStream::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	ElementaryStream::doSerialize( out );

	out->putLEUint32( sampRate );
	out->putLEUint32( sampSize );
	out->putLEUint32( channels );
	compress.doSerialize( out );

}

void AudioStream::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	ElementaryStream::unSerialize( in );

	sampRate = in->getLEUint32();
	sampSize = in->getLEUint32();
	channels = in->getLEUint32();

	compress.unSerialize( in );

	updateFormat();

}

// ============================================================================

void VMP::AudioStream::updateFormat() {

	blockFmt = new Sound::Format( channels, sampRate, sampSize );

}

// ============================================================================

static StreamFactoryTmpl< AudioStream > audioFactory;

// ============================================================================

