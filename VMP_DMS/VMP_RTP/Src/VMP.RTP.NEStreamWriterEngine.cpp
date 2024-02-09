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
//	VMP.RTP.NEStreamWriterEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.SockAddr.h"
#include "BFC.Net.UDPSocket.h"

#include "VMP.L2.AlreadyInSession.h"
#include "VMP.L2.EngineRegisterer.h"
#include "VMP.L2.NotInSession.h"

#include "VMP.RTP.NEStreamWriterEngine.h"
#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.NSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NEStreamWriterEngine )

// ============================================================================

const DL::TypeCPtr & RTP::NEStreamWriterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "330ee708-37b8-4602-b287-50db09c338aa" ),
		"VMP.RTP.NEStreamWriterEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP UDP Writer",
			"Devices",
			"RTP UDP Writer" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::NEStreamWriterEngine::NEStreamWriterEngine() :

	FinalPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, NSession::getClassType() );

}

// ============================================================================

void RTP::NEStreamWriterEngine::init(
		Net::SockAddrCPtr
				pSockAddr ) {

	sockAddr = pSockAddr;

}

// ============================================================================

void RTP::NEStreamWriterEngine::putSessionCallback(
		SessionPtr	/*pOutpSess*/ ) {

//	msgDbg( "getSessionCB(): opening \"" + name + "\"..." );

	if ( ! sockAddr ) {
		throw NotInitialized();
	}

	if ( outpSock ) {
		throw L2::AlreadyInSession();
	}

	Buffer	outpAddr = sockAddr->getAddr()->getAddr();
	Uint16	outpPort = sockAddr->getPort();

	outpSock = new Net::UDPSocket;

	if ( outpAddr.hasContent() ) {
		outpSock->openMCast( Net::IPAddr::getByName( outpAddr ), outpPort );
	}
	else {
		outpSock->openWrite( sockAddr );
	}

	setInSession( true );

//	msgDbg( "getSessionCallback(): session:\n" + oNetSess->toBuffer() );

}

void RTP::NEStreamWriterEngine::endSessionCallback() {

	if ( ! outpSock ) {
		throw L2::NotInSession();
	}

	outpSock->close();

	outpSock.kill();

	setInSession( false );

}

void RTP::NEStreamWriterEngine::putFrameCallback(
		FramePtr	pOutpFrme ) {

	NFramePtr	rtpNFrme = pOutpFrme;

	try {
		outpSock->putBytes( rtpNFrme->getData() );
	}
	catch ( IO::EndOfStream & ) {
		throw L2::EndOfStream();
	}
	catch ( IO::BrokenPipe & ) {
		setInSession( false );
		throw L2::BrokenSession();
	}

}

// ============================================================================

static L2::EngineRegisterer< RTP::NEStreamWriterEngine > engineRegisterer;

// ============================================================================

