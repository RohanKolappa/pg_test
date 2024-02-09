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
//	VMP.RTP.TEStreamServerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.SockAddr.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.NEStreamEncoderEngine.h"
#include "VMP.RTP.NEStreamWriterEngine.h"
#include "VMP.RTP.TEStreamServerEngine.h"
#include "VMP.RTP.TEStreamEncoderEngine.h"
#include "VMP.RTP.TRepository.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TEStreamServerEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TEStreamServerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "5da557dc-e1ac-44ac-888b-420c4e022264" ),
		"VMP.RTP.TEStreamServerEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP EStream Server",
			"Devices",
			"Simple RTP writer" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TEStreamServerEngine::TEStreamServerEngine() :

	L2::FinalPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

}

RTP::TEStreamServerEngine::~TEStreamServerEngine() {

}

// ============================================================================

void RTP::TEStreamServerEngine::init(
		Net::SockAddrCPtr
				pSockAddr ) {

	sockAddr = pSockAddr;

}

// ============================================================================

void RTP::TEStreamServerEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

	/* TEStreamEncoderEnginePtr */	tEncoder = TRepository::instance()->makeTEStreamEncoderEngine( pInptSess->getStream() );
	NEStreamEncoderEnginePtr	nEncoder = new NEStreamEncoderEngine;
	NEStreamWriterEnginePtr	rtpNWrtr = new NEStreamWriterEngine;

	rtpNWrtr->init( sockAddr );

	tEncoder->attachPeerEngine( 0, nEncoder );
	nEncoder->attachPeerEngine( 0, rtpNWrtr );

	tEncoder->putSessionCallback( pInptSess );

}

void RTP::TEStreamServerEngine::endSessionCallback() {

	tEncoder->endSessionCallback();

	tEncoder.kill();

}

void RTP::TEStreamServerEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

	tEncoder->putFrameCallback( pInptFrme );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TEStreamServerEngine > serverRegisterer;

// ============================================================================

