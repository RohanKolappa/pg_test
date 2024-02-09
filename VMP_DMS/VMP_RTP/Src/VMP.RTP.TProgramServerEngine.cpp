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
//	VMP.RTP.TProgramServerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.SockAddr.h"

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TEStreamServerEngine.h"
#include "VMP.RTP.TProgramServerEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TProgramServerEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TProgramServerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "7ce88892-a712-4306-8598-7af5b1d859fe" ),
		"VMP.RTP.TProgramServerEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Program Server",
			"Devices",
			"Simple RTP writer"
		)
	);

	return i;

}

// ============================================================================

RTP::TProgramServerEngine::TProgramServerEngine() :

	L2::FinalPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

}

RTP::TProgramServerEngine::~TProgramServerEngine() {

}

// ============================================================================

void RTP::TProgramServerEngine::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession()" );
//	msgDbg( "putSession(): " + pSession->toBuffer() );

	strmSrvr.kill();

	MuxedSessionPtr	iMuxSess = pSession;
	Uint32		nbrComps = iMuxSess->getNbrComponents();

	strmSrvr.resize( nbrComps );

	Bool		gotVideo = false;
	Bool		gotAudio = false;

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		SessionPtr	iSubSess = iMuxSess->getComponent( i );
		StreamPtr	iSubStrm = iSubSess->getStream();
		Uint16		outpPort = 0;
		Bool *		currFlag = 0;
		if ( iSubStrm->inherits( VideoStream::getClassType() ) ) {
			if ( ! gotVideo ) {
				outpPort = oVidPort;
				currFlag = &gotVideo;
			}
		}
		else if ( iSubStrm->inherits( AudioStream::getClassType() ) ) {
			if ( ! gotAudio ) {
				outpPort = oAudPort;
				currFlag = &gotAudio;
			}
		}
		if ( ! outpPort ) {
			continue;
		}
		TEStreamServerEnginePtr
				candSrvr;
		try {
			candSrvr = makeTEStreamServerEngine( outpAddr, outpPort );
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't create estream server!" );
			continue;
		}
		try {
			candSrvr->putSessionCallback( iSubSess );
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't init estream session!" );
			continue;
		}
		*currFlag = true;
		strmSrvr[ i ] = candSrvr;
	}

}

void RTP::TProgramServerEngine::endSessionCallback() {

//	msgDbg( "endSession()" );

	for ( Uint32 i = 0 ; i < strmSrvr.getSize() ; i++ ) {
		if ( strmSrvr[ i ] ) {
			strmSrvr[ i ]->endSessionCallback();
		}
	}

	strmSrvr.kill();

}

void RTP::TProgramServerEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame()" );

	MuxedFramePtr	iMuxFrme = pFrame;
	Uint32		subIndex = iMuxFrme->getSubIndex();

	if ( strmSrvr[ subIndex ] ) {
		strmSrvr[ subIndex ]->putFrameCallback( iMuxFrme->getSubFrame() );
	}

}

// ============================================================================

RTP::TEStreamServerEnginePtr RTP::TProgramServerEngine::makeTEStreamServerEngine(
		Net::IPAddrCPtr	pOutpAddr,
	const	Uint16		pOutpPort ) const {

	TEStreamServerEnginePtr	strmSrvr = new TEStreamServerEngine;

	strmSrvr->init( new Net::SockAddr( pOutpAddr, pOutpPort ) );

	return strmSrvr;

}

// ============================================================================

static L2::EngineRegisterer< RTP::TProgramServerEngine > serverRegisterer;

// ============================================================================

