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
//	VMP.RTP.NProgramReaderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.Scheduler.h"

#include "BFC.Net.SockAddr.h"

#include "VMP.MuxedSession.h"

#include "VMP.L2.AlreadyInSession.h"
#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.EnmuxerEngine.h"
#include "VMP.L2.Pumper.h"

#include "VMP.RTP.NEStreamReaderEngine.h"
#include "VMP.RTP.NProgramReaderEngine.h"
#include "VMP.RTP.NSession.h"
#include "VMP.RTP.SConnInfo.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.SProgram.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NProgramReaderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::NProgramReaderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "7f350a90-3448-4797-9294-d0acfbbdba59" ),
		"VMP.RTP.NProgramReaderEngine",
		L2::ProxyProdEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Program Client (Push)",
			"Devices",
			"RTP Program Client (Push)" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::NProgramReaderEngine::NProgramReaderEngine() :

	L2::ProxyProdEngine	( getClassType() ){

	setNbrOutputs( 1 );
	setOutputType( 0, MuxedSession::getClassType() );

}

RTP::NProgramReaderEngine::~NProgramReaderEngine() {

}

// ============================================================================

void RTP::NProgramReaderEngine::init(
		SProgramCPtr	pSProgram,
		Net::NetIfacePtr
				pNetIface ) {

	sProgram = pSProgram;
	netIface = pNetIface;

}

// ============================================================================

void RTP::NProgramReaderEngine::fireWork() {

	if ( ! sProgram ) {
		throw NotInitialized();
	}

	if ( ! pumpList.isEmpty() ) {
		throw L2::AlreadyInSession();
	}

	L2::EnmuxerCorePtr	muxrCore = new L2::EnmuxerCore;

	muxrCore->resetGraceTime();
	muxrCore->setBlockingPutSession( false );
	muxrCore->setBlockingPutFrame( true );
	muxrCore->setForceReorder( false );

	for ( Uint32 i = 0 ; i < sProgram->getNbrSEStreams() ; i++ ) {

		SEStreamCPtr		sEStream = sProgram->getSEStream( i );
		Uint16			strmPort = sEStream->getStreamPort();
		Buffer			strmAddr = sEStream->getConnectionInfo()->getAddress();
		Net::SockAddrCPtr	sockAddr = new Net::SockAddr( strmAddr, strmPort );

//		msgDbg( "fireWork(): converting estream:\n" + sEStream->toBuffer() );

		StreamPtr		outpStrm = sEStream->toStream();
		NSessionPtr		oNetSess = new NSession( Session::Live, outpStrm, sProgram, sEStream );
		NEStreamReaderEnginePtr	nUdpRder = new NEStreamReaderEngine;

		nUdpRder->init( sockAddr, oNetSess, netIface );

		L2::EnmuxerEnginePtr	muxrEngn = new L2::EnmuxerEngine( muxrCore );

		muxrEngn->attachPeerEngine( 0, getPeerEngine( 0 ) );

		L2::PumperPtr		strmPump = new L2::Pumper;

		strmPump->addObjectName( "[ NProgRderEngn, " + sEStream->getMetaType() + " ]" );

		strmPump->setSourceEngine( nUdpRder );
		strmPump->setTargetEngine( muxrEngn );

		pumpList += strmPump;

	}

	taskSchd = new MT::Scheduler;

	for ( Uint32 i = 0 ; i < pumpList.getSize() ; i++ ) {
		taskSchd->addThread( pumpList[ i ], true );
	}

}

void RTP::NProgramReaderEngine::haltWork() {

	if ( ! taskSchd ) {
		return;
	}

	for ( Uint32 i = 0 ; i < pumpList.getSize() ; i++ ) {
		pumpList[ i ]->cleanup();
	}

	pumpList.kill();

	taskSchd->kill();
	taskSchd->waitEmpty();

	taskSchd.kill();

}

void RTP::NProgramReaderEngine::waitDone() {

	MT::SchedulerPtr	copySchd = taskSchd;

	if ( copySchd ) {
		copySchd->waitEmpty();
	}

}

Bool RTP::NProgramReaderEngine::isDone() const {

	MT::SchedulerPtr	copySchd = taskSchd;

	return ( ! copySchd || copySchd->isEmpty() );

}

// ============================================================================

