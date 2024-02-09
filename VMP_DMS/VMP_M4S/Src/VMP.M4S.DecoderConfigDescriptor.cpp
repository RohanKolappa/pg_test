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
//	VMP.M4S.DecoderConfigDescriptor.cpp
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
#include "BFC.IO.MemoryInputStream.h"

#include "VMP.AAC.AudioSpecificConfig.h"

#include "VMP.M4S.Defines.h"
#include "VMP.M4S.DecoderConfigDescriptor.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M4S, DecoderConfigDescriptor )

// ============================================================================

M4S::DecoderConfigDescriptor::DecoderConfigDescriptor() {

	setObjectName( "VMP.M4S.DecoderConfigDescriptor" );

}

// ============================================================================

void M4S::DecoderConfigDescriptor::decode(
	const	Buffer &	pData ) {

//	class DecoderConfigDescriptor extends BaseDescriptor : bit(8) tag=DecoderConfigDescrTag {
//		bit(8) objectTypeIndication;
//		bit(6) streamType;
//		bit(1) upStream;
//		const bit(1) reserved=1;
//		bit(24) bufferSizeDB;
//		bit(32) maxBitrate;
//		bit(32) avgBitrate;
//		DecoderSpecificInfo decSpecificInfo[0 .. 1];
//		profileLevelIndicationIndexDescriptor profileLevelIndicationIndexDescr [0..255];
//	}

//	msgDbg( "decode(): " + Utils::toHex( pData ) );

	data = pData;

	IO::MemoryInputStreamPtr	iMemStrm = new IO::MemoryInputStream( data );
	IO::ByteInputStreamPtr		iBytStrm = new IO::ByteInputStream( iMemStrm );

	iBytStrm->open();

	if ( iBytStrm->getUchar() != DecoderConfigDescrTag ) {
		throw InternalError( "Not an DecoderConfigDescriptor ?" );
	}

	Uint32	len = iBytStrm->getUchar();

	if ( len != iBytStrm->length() - iBytStrm->tell() ) {
		throw InternalError( "Invalid DecoderConfigDescriptor length!" );
	}

	objTypId	= iBytStrm->getUchar();
	strmType	= iBytStrm->getUchar();

	if ( ( strmType & 0x01 ) != 0x01 ) {
		throw InternalError( "Invalid reserved bit!" );
	}

	if ( ( strmType & 0x02 ) != 0x00 ) {
		throw InternalError( "Invalid upstream bit!" );
	}

	strmType >>= 2;

	buffSize	= iBytStrm->getBEUint24();
	maxBRate	= iBytStrm->getBEUint32();
	avgBRate	= iBytStrm->getBEUint32();

	// DecoderSpecificInfo & profileLevelIndicationIndexDescriptor...

	while ( iBytStrm->tell() < iBytStrm->length() ) {
		Uchar	tag = iBytStrm->peekUchar( 0 );
		Uchar	len = iBytStrm->peekUchar( 1 );
		Buffer	dat = iBytStrm->getBytes( 2 + len );
		if ( tag == DecSpecificInfoTag
		  && objTypId == M4AObjectType
		  && strmType == AudioStream ) {
			if ( audConfg ) {
				throw InternalError( "Multiple audio config's!" );
			}
			audConfg = new AAC::AudioSpecificConfig;
			audConfg->decode( dat( 2 ) );
		}
		else {
			throw NotImplemented();
		}
	}

}

Buffer M4S::DecoderConfigDescriptor::encode() const {

	return data;

}

// ============================================================================

Buffer M4S::DecoderConfigDescriptor::toBuffer() const {

	Buffer res = Buffer( "DecoderConfigDescriptor\n" )
		+ "... objTypId: " + Buffer( objTypId, Buffer::Base16, 2 ) + "\n"
		+ "... strmType: " + Buffer( strmType, Buffer::Base16, 2 ) + "\n"
		+ "... buffSize: " + Buffer( buffSize ) + "\n"
		+ "... maxBRate: " + Buffer( maxBRate ) + "\n"
		+ "... avgBRate: " + Buffer( avgBRate );

	if ( audConfg ) {
		res += "\n" + audConfg->toBuffer();
	}

	return res;

}

// ============================================================================

