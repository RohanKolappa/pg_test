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
//	VMP.RTP.TMPEG4GenericEncoder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.IO.ByteOutputStream.h"
#include "BFC.IO.MemoryOutputStream.h"

#include "VMP.BytesSession.h"
#include "VMP.ConfigInfo.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TMPEG4GenericEncoder.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TMPEG4GenericEncoder )

// ============================================================================

const DL::TypeCPtr & RTP::TMPEG4GenericEncoder::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "184e379c-4267-4cc1-a3a5-74a28ca59fbc" ),
		"VMP.RTP.TMPEG4GenericEncoder",
		RTP::TEStreamEncoderEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP MPEG-4 Generic EStream Encoder",
			"Devices",
			"RTP MPEG-4 Generic EStream Encoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TMPEG4GenericEncoder::TMPEG4GenericEncoder() :

	TEStreamEncoderEngine	( getClassType() ) {

}

// ============================================================================

void RTP::TMPEG4GenericEncoder::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession()" );

	BytesSessionPtr		iBytSess = pSession;

	if ( iBytSess->hasConfigInfo() ) {
		ConfigInfoCPtr c = iBytSess->getConfigInfo();
		msgDbg( "putSession(): ASD: "
			+ Utils::toHex( c->encode(), false ) );
	}

	AudioStreamPtr		iAudStrm = iBytSess->getContent();

	smplRate = iAudStrm->getSampleRate();

	if ( iAudStrm->getCompression() != AudioCompression::AAC ) {
		throw InternalError();
	}

	frmeOffs = 0;
	unitOffs = 0;

	TSessionPtr		rtpTSess = new TSession( iBytSess->getFlags(), iAudStrm, 0, 0 );

	getPeerEngine( 0 )->putSessionCallback( rtpTSess );

	setInSession( true );

}

void RTP::TMPEG4GenericEncoder::endSessionCallback() {

//	msgDbg( "endSession()" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TMPEG4GenericEncoder::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame(): " + pFrame->toBuffer() );

	BytesFramePtr			oBytFrme = pFrame;
	Buffer				oBytData = oBytFrme->getData();

//	msgDbg( "putFrame(): " + Utils::toHex( oBytData ) );

	IO::MemoryOutputStreamPtr	oMStream = new IO::MemoryOutputStream;
	IO::ByteOutputStreamPtr		oBStream = new IO::ByteOutputStream( oMStream );

	oBStream->open();

	oBStream->putBEUint16( 16 );	// AU-headers-length
	oBStream->putBEUint16( oBytData.getLength() << 3 );
	oBStream->putBytes( oBytData );

	oBStream->flush();

	TFramePtr			outpFrme = makeTFrame();

	outpFrme->PT	= 96;
	outpFrme->M	= true;
	outpFrme->TS	= ( Uint32 )( ( ( Uint64 )oBytFrme->getPTS() * smplRate ) / 1000000 );
	outpFrme->data	= oMStream->getContent();

	getPeerEngine( 0 )->putFrameCallback( outpFrme );

}

void RTP::TMPEG4GenericEncoder::flushSessionCallback() {

//	msgDbg( "flushSessionCallback(): --->" );

	getPeerEngine( 0 )->flushSessionCallback();

//	msgDbg( "flushSessionCallback(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TMPEG4GenericEncoder > clientRegisterer;

// ============================================================================

