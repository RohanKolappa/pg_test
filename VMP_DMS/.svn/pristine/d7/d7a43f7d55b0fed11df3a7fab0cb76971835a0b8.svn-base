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
//	VMP.RTP.TEStreamClientEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.VBool.h"

#include "VMP.MuxedFrame.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.NEStreamDecoderEngine.h"
#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.NSession.h"
#include "VMP.RTP.TDTSComputerEngine.h"
#include "VMP.RTP.TDriftRemoverEngine.h"
#include "VMP.RTP.TEStreamCheckerEngine.h"
#include "VMP.RTP.TEStreamDecoderEngine.h"
#include "VMP.RTP.TEStreamClientEngine.h"
#include "VMP.RTP.THandler.h"
#include "VMP.RTP.TJitterRemoverEngine.h"
#include "VMP.RTP.TRepository.h"
#include "VMP.RTP.TTSConverterEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TEStreamClientEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TEStreamClientEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "32d3dbb2-5d02-454c-af63-cd9d60a7f375" ),
		"VMP.RTP.TEStreamClientEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP EStream Client",
			"Devices",
			"RTP EStream Client" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TEStreamClientEngine::TEStreamClientEngine() :

	L2::ProxyPushEngine	( getClassType() ),

	clckPrio		( 0 ) {

	setNbrInputs( 1 );
	setInputType( 0, NSession::getClassType() );
	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	multSSRC = new Dyn::VBool( "multSSRC", false );

	addDynVar( multSSRC );

}

RTP::TEStreamClientEngine::~TEStreamClientEngine() {

	delAllDynVars();

}

// ============================================================================

void RTP::TEStreamClientEngine::setAllowMultipleSSRC(
	const	Bool		pMultSSRC ) {

	multSSRC->setValue( pMultSSRC );

}

// ============================================================================

void RTP::TEStreamClientEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

//	msgDbg( "putSessionCallback(): --->" );

	NSessionPtr			iStrSess = pInptSess;
	SEStreamCPtr			sEStream = iStrSess->getSEStream();
	TRepositoryPtr			hndlrRep = TRepository::instance();
	THandlerPtr			tHandler = hndlrRep->makeTHandler( sEStream );
	Uint32				timeScle = tHandler->getTimeScale();

	Buffer	xtraName = "[ " + Buffer( clckPrio ) + " ]";

	// +-----------------------+
	// | NEStreamDecoderEngine |
	// +-----------------------+

	/*NEStreamDecoderEnginePtr*/	nDecoder = new NEStreamDecoderEngine;

	nDecoder->addObjectName( xtraName );

	// +-----------------------+
	// | TEStreamCheckerEngine |
	// +-----------------------+

	TEStreamCheckerEnginePtr	tChecker = new TEStreamCheckerEngine;

	tChecker->addObjectName( xtraName );
	tChecker->setAllowMultipleSSRC( multSSRC->getValue() );

	nDecoder->attachPeerEngine( 0, tChecker );

	// +--------------------+
	// | TTSConverterEngine |
	// +--------------------+

	TTSConverterEnginePtr		tStmpCnv = new TTSConverterEngine;

	tStmpCnv->addObjectName( xtraName );
	tStmpCnv->init( timeScle );

	tChecker->attachPeerEngine( 0, tStmpCnv );

	// +--------------------+
	// | TDTSComputerEngine |
	// +--------------------+

	TDTSComputerEnginePtr		dtsCmptr = new TDTSComputerEngine;

	dtsCmptr->addObjectName( xtraName );

	tStmpCnv->attachPeerEngine( 0, dtsCmptr );

//	// +----------------------+
//	// | TJitterRemoverEngine |
//	// +----------------------+
//
//	TJitterRemoverEnginePtr		jittRmvr = new TJitterRemoverEngine;
//
//	jittRmvr->addObjectName( xtraName );
//
//	dtsCmptr->attachPeerEngine( 0, jittRmvr );

	// +---------------------+
	// | TDriftRemoverEngine |
	// +---------------------+

	TDriftRemoverEnginePtr		drftRmvr = new TDriftRemoverEngine;

	drftRmvr->addObjectName( xtraName );
	drftRmvr->init( clckPrio, clckShrr );

//	jittRmvr->attachPeerEngine( 0, drftRmvr );
	dtsCmptr->attachPeerEngine( 0, drftRmvr );

	// +-----------------------+
	// | TEStreamDecoderEngine |
	// +-----------------------+

	TEStreamDecoderEnginePtr	tDecoder = tHandler->makeTEStreamDecoderEngine();

	drftRmvr->attachPeerEngine( 0, tDecoder );

	tDecoder->attachPeerEngine( 0, getPeerEngine( 0 ) );

	// GO!

	nDecoder->putSessionCallback( pInptSess );

//	msgDbg( "putSessionCallback(): <---" );

}

void RTP::TEStreamClientEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	nDecoder->endSessionCallback();

	nDecoder.kill();

//	msgDbg( "endSessionCallback(): <---" );

}

void RTP::TEStreamClientEngine::putFrameCallback(
	const	FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

	nDecoder->putFrameCallback( pInptFrme );

//	msgDbg( "putFrameCallback(): <---" );

}

void RTP::TEStreamClientEngine::flushSessionCallback() {

//	msgDbg( "flushSessionCallback(): --->" );

	nDecoder->flushSessionCallback();

//	msgDbg( "flushSessionCallback(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TEStreamClientEngine > clientRegisterer;

// ============================================================================

