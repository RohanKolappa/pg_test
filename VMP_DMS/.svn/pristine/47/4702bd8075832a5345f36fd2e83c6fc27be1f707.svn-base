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
//	VMP.RTP.TProgramClientEngine.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include <stdlib.h>

#include "BFC.Dyn.VBool.h"

#include "BFC.Msg.Funcs.h"

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.EngineRegisterer.h"
#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.EnmuxerEngine.h"

#include "VMP.RTP.TClockSharer.h"
#include "VMP.RTP.TEStreamClientEngine.h"
#include "VMP.RTP.TProgramClientEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TProgramClientEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TProgramClientEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "d436d1ea-18c2-4b54-814b-544e74b39245" ),
		"VMP.RTP.TProgramClientEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Program Client",
			"Devices",
			"Simple RTP reader" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TProgramClientEngine::TProgramClientEngine() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );
	setNbrOutputs( 1 );
	setOutputType( 0, MuxedSession::getClassType() );

	multSSRC = new Dyn::VBool( "multSSRC", false );

	addDynVar( multSSRC );

}

RTP::TProgramClientEngine::~TProgramClientEngine() {

	delAllDynVars();

}

// ============================================================================

void RTP::TProgramClientEngine::setAllowMultipleSSRC(
	const	Bool		pMultSSRC ) {

	multSSRC->setValue( pMultSSRC );

}

// ============================================================================

void RTP::TProgramClientEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

//	msgDbg( "putSessionCallback(): --->" );

//	msgDbg( "putSessionCallback(): session:\n" + pInptSess->toBuffer() );

	if ( ! peerList.isEmpty() ) {
		throw L2::AlreadyInSession();
	}

	MuxedSessionPtr		iMuxSess = pInptSess;
	L2::EnmuxerCorePtr	muxrCore = new L2::EnmuxerCore;

	muxrCore->setBlockingPutSession( false );
	muxrCore->setBlockingPutFrame( false );
	muxrCore->setForceReorder( false );

	TClockSharerPtr		clckShrr = new TClockSharer;
	Bool			masterOk = false;

	for ( Uint32 i = 0 ; i < iMuxSess->getNbrComponents() ; i++ ) {
		// Will this component drive the output clock ?
		Bool			isMaster = ( ! masterOk && iMuxSess->getComponent( i )->getStream().isA< AudioStream >() );
		if ( isMaster ) {
			masterOk = true;
		}
//		msgDbg( "putSession(): stream " + Buffer( i ) + " clock master: " + Buffer( isMaster ) + "!" );
		// Create a stream client for this component...
		TEStreamClientEnginePtr	estrClnt = new TEStreamClientEngine;
		Msg::sync( multSSRC, estrClnt->getDynVar( "multSSRC" ) );
		estrClnt->setTClockSharer( clckShrr, isMaster );
		L2::EnmuxerEnginePtr	muxrEngn = new L2::EnmuxerEngine( muxrCore );
		muxrEngn->addObjectName( "[" + getObjectName() + ":" + Buffer( i ) + "]" );
		muxrEngn->attachPeerEngine( 0, getPeerEngine( 0 ) );
		estrClnt->attachPeerEngine( 0, muxrEngn );
		peerList += estrClnt;
	}

	for ( Uint32 i = 0 ; i < peerList.getSize() ; i++ ) {
		peerList[ i ]->putSessionCallback( iMuxSess->getComponent( i ) );
	}

//	msgDbg( "putSessionCallback(): <---" );

}

void RTP::TProgramClientEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	if ( peerList.isEmpty() ) {
		throw L2::NotInSession();
	}

	for ( Uint32 i = 0 ; i < peerList.getSize() ; i++ ) {
		peerList[ i ]->endSessionCallback();
	}

	peerList.kill();

//	msgDbg( "endSessionCallback(): <---" );

}

void RTP::TProgramClientEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

//	msgDbg( "putFrameCallback(): " + pInptFrme->toBuffer() );

	MuxedFramePtr	iMuxFrme = pInptFrme;
	Uint32		iSubIndx = iMuxFrme->getSubIndex();
	FramePtr	iSubFrme = iMuxFrme->getSubFrame();

//	msgDbg( "putFrameCallback(): [ " + Buffer( iSubIndx ) + " ]: " + iSubFrme->toBuffer() );

	peerList[ iSubIndx ]->putFrameCallback( iSubFrme );

//	msgDbg( "putFrameCallback(): <---" );

}

void RTP::TProgramClientEngine::flushSessionCallback() {

	for ( Uint32 i = 0 ; i < peerList.getSize() ; i++ ) {
		peerList[ i ]->flushSessionCallback();
	}

}

// ============================================================================

static L2::EngineRegisterer< RTP::TProgramClientEngine > clientRegisterer;

// ============================================================================

