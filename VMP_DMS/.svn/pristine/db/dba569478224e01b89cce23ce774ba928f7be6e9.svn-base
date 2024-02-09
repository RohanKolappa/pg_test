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
//	VMP.AVC.SEI.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.AVC.Bitstream.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.SEI.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, SEI )

// ============================================================================

AVC::SEI::SEI() {

	setObjectName( "VMP.AVC.SEI" );

}

// ============================================================================

void AVC::SEI::decode(
		NALUCPtr	pNALU ) {

	Buffer		d = pNALU->getRBSP();

	msgDbg( "decode(): " + Utils::toHex( d ) );

	BitstreamPtr	s = new Bitstream( d );
	Uint32		v;

	Uint32		dataType = 0;	// payloadType

	do {
		v = s->read_u_v( 8 );
		dataType += v;
	} while ( v == 0xFF );

	Uint32		dataSize = 0; // payloadSize

	do {
		v = s->read_u_v( 8 );
		dataSize += v;
	} while ( v == 0xFF );

	msgDbg( "decode(): type: " + Buffer( dataType ) + ", size: " + Buffer( dataSize ) );

	switch ( dataType ) {
//	case 0 :
//		decodeBufferingPeriod( s, dataSize );
//		break;
//	case 4 :
//		decodeUserDataRegisteredItuT35( s, dataSize );
//		break;
//	case 5 :
//		decodeUserDataUnregistered( s, dataSize );
//		break;
	default :
		msgWrn( "decode(): unsupported type [ " + Buffer( dataType ) + " ]!" );
		s->skip( dataSize * 8 );
	}

	if ( s->tell() != s->length() ) {
		Uint32 d = s->length() - s->tell();
		if ( d != 8 || s->read_u_v( 8 ) != 0x80 ) {
		msgWrn( "decode(): junk (?) at end of SEI ("
			+ Buffer( d ) + " bits left)!" );
		}
	}

}

// ============================================================================

Buffer AVC::SEI::toBuffer() const {

	Buffer res;

//	res += "SEI:";

	return res;

}

// ============================================================================

//void AVC::SEI::decodeBufferingPeriod(
//		BitstreamPtr	pIBitStrm,
//	const	Uint32		pDataSize ) {
//
//}

// ============================================================================

