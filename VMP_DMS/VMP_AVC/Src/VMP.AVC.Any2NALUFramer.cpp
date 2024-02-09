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
//	VMP.AVC.Any2NALUFramer.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.MemoryInputStream.h"

#include "VMP.AVC.Any2NALUFramer.h"
#include "VMP.AVC.Frame.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.Session.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, Any2NALUFramer )

// ============================================================================

const DL::TypeCPtr & AVC::Any2NALUFramer::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "54a1162d-fd71-4616-b1ee-28bdc0e819e5" ),
		"VMP.AVC.Any2NALUFramer",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"Any2NALUFramer",
			"Utils",
			"Converts Frame's into AVC::Frame's."
		)
	);

	return i;

}

// ============================================================================

AVC::Any2NALUFramer::Any2NALUFramer() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, Session::getClassType() );

}

AVC::Any2NALUFramer::~Any2NALUFramer() {

}

// ============================================================================

void AVC::Any2NALUFramer::putSessionCallback(
		VMP::SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	if ( ! pSession.isA< Session >() ) {
		throw NotImplemented( "Raw not supported!" );
//		oNalSess = new Session( *iBytSess, NALUPacket, ModeNALU );
	}

	SessionPtr	iNalSess = pSession;

	mode = iNalSess->getStreamMode();

	if ( mode == ModeNALU && iNalSess->getPacketMode() != NALUPacket ) {
		throw InternalError();
	}

	SessionPtr	oNalSess = iNalSess->clone();

	oNalSess->setPacketMode( NALUPacket );
	oNalSess->setStreamMode( ModeNALU );

	getPeerEngine( 0 )->putSessionCallback( oNalSess );

	setInSession( true );

}

void AVC::Any2NALUFramer::endSessionCallback() {

//	msgDbg( "endSession()" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void AVC::Any2NALUFramer::putFrameCallback(
		VMP::FramePtr	pFrame ) {

//	msgDbg( "putFrame(): " + pFrame->toBuffer() );

	Array< FramePtr >	oNalFrme;

	switch ( mode ) {
	case ModeNALU :
		oNalFrme += pFrame;
		break;
	case ModeAnnexB :
		parseAnnexB( pFrame, oNalFrme );
		break;
	case ModePart15 :
		parsePart15( pFrame, oNalFrme );
		break;
	}

	for ( Uint32 i = 0 ; i < oNalFrme.getSize() ; i++ ) {
		getPeerEngine( 0 )->putFrameCallback( oNalFrme[ i ] );
	}

}

void AVC::Any2NALUFramer::flushSessionCallback() {

//	msgDbg( "rstSession()" );

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

void AVC::Any2NALUFramer::parseAnnexB(
		VMP::FramePtr	/*pInptFrme*/,
		Array< FramePtr > &
				/*pOutpFrme*/ ) {

	throw NotImplemented();

}

void AVC::Any2NALUFramer::parsePart15(
		VMP::FramePtr	pInptFrme,
		Array< FramePtr > &
				pOutpFrme ) {

	pOutpFrme.kill();

	BytesFramePtr	iBytFrme = pInptFrme;
	Time::Clock	iBuffPTS = iBytFrme->getPTS();
	Time::Clock	iBuffDTS = iBytFrme->getDTS();
	Bool		iKeyFrme = iBytFrme->isKeyFrame();
	Bool		iSgmStrt = iBytFrme->isSegmentStart();
	Buffer		iBufData = iBytFrme->getData();

	IO::MemoryInputStreamPtr	iMemStrm = new IO::MemoryInputStream( iBufData );
	IO::ByteInputStreamPtr		iBytStrm = new IO::ByteInputStream( iMemStrm );

	iBytStrm->open();

	Buffer listText;

	while ( iBytStrm->tell() < iBytStrm->length() ) {
		Uint32		oNalSize = iBytStrm->getBEUint32();
		Buffer		oNalData = iBytStrm->getBytes( oNalSize );
		NALUPtr		oNaluObj = new NALU( oNalData );
		FramePtr	oNalFrme = new Frame( oNaluObj );
		oNalFrme->setPTS( iBuffPTS );
		oNalFrme->setDTS( iBuffDTS );
		oNalFrme->setKeyFrame( iKeyFrme );
		oNalFrme->setSegmentStart( iSgmStrt );
		pOutpFrme += oNalFrme;
		listText += " " + NALU::getTypeName( oNaluObj->getType() );
	}

//	msgDbg( "parsePart15(): out:" + listText );

}

// ============================================================================

