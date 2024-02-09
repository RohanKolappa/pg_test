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
//	VMP.RTP.NProgramDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.EnmuxerEngine.h"

#include "VMP.RTP.NEStreamDecoderEngine.h"
#include "VMP.RTP.NProgramDecoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NProgramDecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::NProgramDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e6a253fe-1ceb-4ccb-959f-471a541bc205" ),
		"VMP.RTP.NProgramDecoderEngine",
		L2::ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

RTP::NProgramDecoderEngine::NProgramDecoderEngine() :

	L2::ProxyPushEngine	( getClassType() ),

	muxrCore		( new L2::EnmuxerCore ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, MuxedSession::getClassType() );

	muxrCore->setBlockingPutSession( false );

}

// ============================================================================

void RTP::NProgramDecoderEngine::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	if ( isInSession() ) {
		throw L2::AlreadyInSession();
	}

	MuxedSessionPtr	iMuxSess = pSession;

	nbrComps = iMuxSess->getNbrComponents();

	dcdrList.resize( nbrComps );
	muxrList.resize( nbrComps );

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		dcdrList[ i ] = new NEStreamDecoderEngine;
		muxrList[ i ] = new L2::EnmuxerEngine( muxrCore );
		dcdrList[ i ]->attachPeerEngine( 0, muxrList[ i ] );
		muxrList[ i ]->attachPeerEngine( 0, getPeerEngine( 0 ) );
	}

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		dcdrList[ i ]->putSessionCallback( iMuxSess->getComponent( i ) );
	}

	setInSession( true );

}

void RTP::NProgramDecoderEngine::endSessionCallback() {

	setInSession( false );

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		dcdrList[ i ]->endSessionCallback();
	}

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		dcdrList[ i ]->detachPeerEngine( 0 );
		muxrList[ i ]->detachPeerEngine( 0 );
	}

	dcdrList.kill();
	muxrList.kill();

}

void RTP::NProgramDecoderEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame(): " + pFrame->toBuffer() );

	MuxedFramePtr	muxdFrme = pFrame;
	Uint32		muxdIndx = muxdFrme->getSubIndex();
	FramePtr	subFrame = muxdFrme->getSubFrame();

	dcdrList[ muxdIndx ]->putFrameCallback( subFrame );

}

void RTP::NProgramDecoderEngine::flushSessionCallback() {

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		dcdrList[ i ]->flushSessionCallback();
	}

}

// ============================================================================

