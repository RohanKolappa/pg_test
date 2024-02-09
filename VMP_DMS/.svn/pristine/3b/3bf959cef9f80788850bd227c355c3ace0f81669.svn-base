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
//	VMP.M4S.SLConfigDescriptor.cpp
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

#include "VMP.M4S.Defines.h"
#include "VMP.M4S.SLConfigDescriptor.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M4S, SLConfigDescriptor )

// ============================================================================

M4S::SLConfigDescriptor::SLConfigDescriptor() {

	setObjectName( "VMP.M4S.SLConfigDescriptor" );

}

// ============================================================================

void M4S::SLConfigDescriptor::decode(
	const	Buffer &	pData ) {

//	class SLConfigDescriptor extends BaseDescriptor : bit(8) tag=SLConfigDescrTag {
//		bit(8) predefined;
//		if (predefined==0) {
//			bit(1) useAccessUnitStartFlag;
//			bit(1) useAccessUnitEndFlag;
//			bit(1) useRandomAccessPointFlag;
//			bit(1) hasRandomAccessUnitsOnlyFlag;
//			bit(1) usePaddingFlag;
//			bit(1) useTimeStampsFlag;
//			bit(1) useIdleFlag;
//			bit(1) durationFlag;
//			bit(32) timeStampResolution;
//			bit(32) OCRResolution;
//			bit(8) timeStampLength; // must be  64
//			bit(8) OCRLength; // must be  64
//			bit(8) AU_Length; // must be  32
//			bit(8) instantBitrateLength;
//			bit(4) degradationPriorityLength;
//			bit(5) AU_seqNumLength; // must be  16
//			bit(5) packetSeqNumLength; // must be  16
//			bit(2) reserved=0b11;
//		}
//		if (durationFlag) {
//			bit(32) timeScale;
//			bit(16) accessUnitDuration;
//			bit(16) compositionUnitDuration;
//		}
//		if (!useTimeStampsFlag) {
//			bit(timeStampLength) startDecodingTimeStamp;
//			bit(timeStampLength) startCompositionTimeStamp;
//		}
//	}
//
//		06
//		01
//			02

//	msgDbg( "decode(): " + Utils::toHex( pData ) );

	data = pData;

	IO::MemoryInputStreamPtr	iMemStrm = new IO::MemoryInputStream( data );
	IO::ByteInputStreamPtr		iBytStrm = new IO::ByteInputStream( iMemStrm );

	iBytStrm->open();

	if ( iBytStrm->getUchar() != SLConfigDescrTag ) {
		throw InternalError( "Not an SLConfigDescriptor ?" );
	}

	Uint32	len = iBytStrm->getUchar();

	if ( len != iBytStrm->length() - iBytStrm->tell() ) {
		throw InternalError( "Invalid SLConfigDescriptor length!" );
	}

	Uchar	pre = iBytStrm->getUchar();

	if ( pre != 0x02 ) { // Reserved for use in MP4 files.
		throw NotImplemented();
	}

}

Buffer M4S::SLConfigDescriptor::encode() const {

	return data;

}

// ============================================================================

