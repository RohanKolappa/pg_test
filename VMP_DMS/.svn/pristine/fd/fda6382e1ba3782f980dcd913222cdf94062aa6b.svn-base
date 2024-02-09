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
//	VMP.RTP.NEStreamReaderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.Packet.h"
#include "BFC.Net.SockAddr.h"
#include "BFC.Net.UDPSocket.h"

#include "VMP.L2.AlreadyInSession.h"
#include "VMP.L2.EngineRegisterer.h"
#include "VMP.L2.NotInSession.h"

#include "VMP.RTP.NEStreamReaderEngine.h"
#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.NSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NEStreamReaderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::NEStreamReaderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "47a47c5c-346b-40ac-ae09-ab5d73aca384" ),
		"VMP.RTP.NEStreamReaderEngine",
		L2::FinalPullEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP UDP Reader",
			"Devices",
			"RTP UDP Reader" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::NEStreamReaderEngine::NEStreamReaderEngine() :

	FinalPullEngine( getClassType() ) {

	setNbrOutputs( 1 );
	setOutputType( 0, NSession::getClassType() );

}

// ============================================================================

void RTP::NEStreamReaderEngine::init(
		Net::SockAddrCPtr
				pSockAddr,
		NSessionPtr	pONetSess,
		Net::NetIfacePtr
				pNetIface ) {

	sockAddr = pSockAddr;
	oNetSess = pONetSess;
	netIface = pNetIface;

}

// ============================================================================

SessionPtr RTP::NEStreamReaderEngine::getSessionCallback() {

//	msgDbg( "getSessionCB(): opening \"" + name + "\"..." );

	if ( ! sockAddr ) {
		throw NotInitialized();
	}

	if ( inptSock ) {
		throw L2::AlreadyInSession();
	}

	Buffer	inptAddr = sockAddr->getAddr()->getAddr();
	Uint16	inptPort = sockAddr->getPort();

	inptSock = new Net::UDPSocket;

	if ( inptAddr.hasContent() ) {
		inptSock->openMCast(
			new Net::SockAddr(
				Net::IPAddr::getByName( inptAddr ), inptPort ),
				netIface );
	}
	else {
		inptSock->openRead( inptPort );
	}

	setInSession( true );

//	msgDbg( "getSessionCallback(): session:\n" + oNetSess->toBuffer() );

	return oNetSess;

}

void RTP::NEStreamReaderEngine::endSessionCallback() {

	if ( ! inptSock ) {
		throw L2::NotInSession();
	}

	inptSock->close();

	inptSock.kill();

	setInSession( false );

}

FramePtr RTP::NEStreamReaderEngine::getFrameCallback(
	const	Uint64		/*pFrmeOffs*/ ) {

	Net::PacketPtr	udpPackt;

	try {
		udpPackt = inptSock->getPacket();
	}
	catch ( IO::EndOfStream & ) {
		throw L2::EndOfStream();
	}
	catch ( IO::BrokenPipe & ) {
		setInSession( false );
		throw L2::BrokenSession();
	}

	Time::Clock	wallClck = Time::now();

//	msgDbg( "getFrameCallback(): <---" );

	return new NFrame(
			udpPackt->getPeerAddr(),
			udpPackt->getSelfAddr(),
			udpPackt->getBuffer(),
			wallClck );

}

// ============================================================================

static L2::EngineRegisterer< RTP::NEStreamReaderEngine > engineRegisterer;

// ============================================================================

