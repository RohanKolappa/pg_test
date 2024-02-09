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
//	VMP.L2.RemuxerEngine.cpp
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

#include "VMP.L2.DemuxerFilter.h"
#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.EnmuxerEngine.h"
#include "VMP.L2.RemuxerEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, RemuxerEngine )

// ============================================================================

const DL::TypeCPtr & L2::RemuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "fd62e35c-6d29-497a-bb99-14fa87a477be" ),
		"VMP.L2.RemuxerEngine",
		L2::ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

L2::RemuxerEngine::RemuxerEngine() :

	ProxyPushEngine	( getClassType() ),

	muxrCore	( new EnmuxerCore ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, MuxedSession::getClassType() );

}

// ============================================================================

void L2::RemuxerEngine::addProcessor(
		DemuxerFilterPtr
				pStrmFltr,
		L2::ProxyPushEnginePtr
				pPeerEngn ) {

	if ( pPeerEngn->getNbrOutputs() != 1 ) {
		throw InvalidArgument( "Processor not 1-to-1!" );
	}

	fltrList += pStrmFltr;
	engnList += pPeerEngn;

}

// ============================================================================

void L2::RemuxerEngine::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	MuxedSessionPtr iMuxSess = pSession;

	createGraph( iMuxSess );

	for ( Uint32 i = 0 ; i < procList.getSize() ; i++ ) {
		procList[ i ][ 0 ]->putSessionCallback( iMuxSess->getComponent( i ) );
	}

	setInSession( true );

}

void L2::RemuxerEngine::endSessionCallback() {

	setInSession( false );

	for ( Uint32 i = 0 ; i < procList.getSize() ; i++ ) {
		procList[ i ][ 0 ]->endSessionCallback();
	}

	deleteGraph();

}

void L2::RemuxerEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame(): " + pFrame->toBuffer() );

	MuxedFramePtr	muxdFrme = pFrame;
	Uint32		muxdIndx = muxdFrme->getSubIndex();
	FramePtr	subFrame = muxdFrme->getSubFrame();

	procList[ muxdIndx ][ 0 ]->putFrameCallback( subFrame );

}

void L2::RemuxerEngine::flushSessionCallback() {

	for ( Uint32 i = 0 ; i < procList.getSize() ; i++ ) {
		procList[ i ][ 0 ]->flushSessionCallback();
	}

}

// ============================================================================

void L2::RemuxerEngine::createGraph(
		SessionPtr	pSession ) {

	if ( ! procList.isEmpty() ) {
		throw InternalError( "Still got graph ?!?" );
	}

	// Attach processors to individual streams...

	MuxedSessionPtr	iMuxSess = pSession;
	Uint32		nbrStrms = iMuxSess->getNbrComponents();

	procList.resize( nbrStrms );

	for ( Uint32 i = 0 ; i < fltrList.getSize() ; i++ ) {
		Uint32	subsIndx = fltrList[ i ]->filter( iMuxSess );
		if ( subsIndx != DemuxerFilter::NoSessionId ) {
			procList[ subsIndx ] += engnList[ i ];
		}
	}

	// Terminate all branches by a muxer...

	for ( Uint32 i = 0 ; i < nbrStrms ; i++ ) {
		procList[ i ] += new L2::EnmuxerEngine( muxrCore );
		procList[ i ].getLast()->addObjectName( "[Remuxer:" + Buffer( i ) + "]" );
// Not good. You don't know if our peer is a proxy at all...
//		procList[ i ] += getPeerEngine( 0 );
	}

	// Attach the nodes...

	for ( Uint32 i = 0 ; i < nbrStrms ; i++ ) {
		for ( Uint32 j = 0 ; j + 1 < procList[ i ].getSize() ; j++ ) {
			procList[ i ][ j ]->attachPeerEngine( 0, procList[ i ][ j + 1 ] );
		}
		procList[ i ].getLast()->attachPeerEngine( 0, getPeerEngine( 0 ) );
	}

	for ( Uint32 i = 0 ; i < nbrStrms ; i++ ) {
		Buffer b;
		for ( Uint32 j = 0 ; j < procList[ i ].getSize() ; j++ ) {
			b += " " + procList[ i ][ j ]->getObjectName();
		}
//		msgDbg( "createGraph(): [ " + Buffer( i ) + " ]: " + b );
	}

}

void L2::RemuxerEngine::deleteGraph() {

	Uint32		nbrStrms = procList.getSize();

	for ( Uint32 i = 0 ; i < nbrStrms ; i++ )
	for ( Uint32 j = 0 ; j + 1 < procList[ i ].getSize() ; j++ ) {
		procList[ i ][ j ]->detachPeerEngine( 0 );
	}

	procList.kill();

}

// ============================================================================

