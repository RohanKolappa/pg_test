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
//	VMP.M4S.ESDescriptor.cpp
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

#include "VMP.M4S.DecoderConfigDescriptor.h"
#include "VMP.M4S.Defines.h"
#include "VMP.M4S.ESDescriptor.h"
#include "VMP.M4S.SLConfigDescriptor.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M4S, ESDescriptor )

// ============================================================================

M4S::ESDescriptor::ESDescriptor() {

	setObjectName( "VMP.M4S.ESDescriptor" );

}

// ============================================================================

void M4S::ESDescriptor::decode(
	const	Buffer &	pData ) {

//	class ES_Descriptor extends BaseDescriptor : bit(8) tag=ES_DescrTag {
//		bit(16) ES_ID;
//		bit(1) streamDependenceFlag;
//		bit(1) URL_Flag;
//		bit(1) OCRstreamFlag;
//		bit(5) streamPriority;
//		if (streamDependenceFlag)
//			bit(16) dependsOn_ES_ID;
//		if (URL_Flag) {
//			bit(8) URLlength;
//			bit(8) URLstring[URLlength];
//		}
//		if (OCRstreamFlag)
//			bit(16) OCR_ES_Id;
//		DecoderConfigDescriptor		decConfigDescr;
//		SLConfigDescriptor		slConfigDescr;
//		IPI_DescrPointer		ipiPtr[0 .. 1];
//		IP_IdentificationDataSet	ipIDS[0 .. 255];
//		IPMP_DescriptorPointer		ipmpDescrPtr[0 .. 255];
//		LanguageDescriptor		langDescr[0 .. 255];
//		QoS_Descriptor			qosDescr[0 .. 1];
//		RegistrationDescriptor		regDescr[0 .. 1];
//		ExtensionDescriptor		extDescr[0 .. 255];
//	}

//	msgDbg( "decode(): " + Utils::toHex( pData ) );

	data = pData;

	IO::MemoryInputStreamPtr	iMemStrm = new IO::MemoryInputStream( data );
	IO::ByteInputStreamPtr		iBytStrm = new IO::ByteInputStream( iMemStrm );

	iBytStrm->open();

	if ( iBytStrm->getUchar() != ES_DescrTag ) {
		throw InternalError( "Not an ES_Descriptor ?" );
	}

//	Uint32	len = iBytStrm->getUchar();

	Uint32	len = 0;
	Uchar	tmp;

	do {
		tmp = iBytStrm->getUchar();
		len = ( len << 7 ) | ( tmp & 0x7F );
	} while ( tmp & 0x80 );

	if ( len != iBytStrm->length() - iBytStrm->tell() ) {
		throw InternalError( "Invalid ES_Descriptor length! Left: "
			+ Buffer( iBytStrm->length() - iBytStrm->tell() )
			+ ", len: " + Buffer( len ) );
	}

	ES_id	= iBytStrm->getBEUint16();

	if ( iBytStrm->getUchar() ) {
		throw NotImplemented( "Unsupported ES_Descriptor flags!" );
	}

	// DecoderConfigDescriptor

	if ( iBytStrm->peekUchar() != DecoderConfigDescrTag ) {
		throw InternalError( "Missing DecoderConfigDescriptor!" );
	}

	cnfgDesc = new DecoderConfigDescriptor;

	cnfgDesc->decode( iBytStrm->getBytes( 2 + iBytStrm->peekUchar( 1 ) ) );

	// SLConfigDescriptor

	if ( iBytStrm->peekUchar() != SLConfigDescrTag ) {
		throw InternalError( "Missing SLConfigDescriptor!" );
	}

	slCfgDsc = new SLConfigDescriptor;

	slCfgDsc->decode( iBytStrm->getBytes( 2 + iBytStrm->peekUchar( 1 ) ) );

	if ( iBytStrm->tell() != iBytStrm->length() ) {
		msgWrn( "decode(): junk at end!" );
	}

}

Buffer M4S::ESDescriptor::encode() const {

	return data;

}

// ============================================================================

Buffer M4S::ESDescriptor::toBuffer() const {

	Buffer res = "ES_id: " + Buffer( ES_id );

	if ( cnfgDesc ) {
		res += "\n" + cnfgDesc->toBuffer();
	}

	return res;

}

// ============================================================================

