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
//	VMP.SegUtils.MuxedSynchronizer.cpp
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

#include "VMP.SegUtils.MuxedSynchronizer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( SegUtils, MuxedSynchronizer )

// ============================================================================

const DL::TypeCPtr & SegUtils::MuxedSynchronizer::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b6c4c791-d672-4025-9e31-ccafaafce318" ),
		"VMP.SegUtils.MuxedSynchronizer",
		L2::ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

SegUtils::MuxedSynchronizer::MuxedSynchronizer() :

	ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, MuxedSession::getClassType() );

}

// ============================================================================

void SegUtils::MuxedSynchronizer::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	MuxedSessionPtr iMuxSess = pSession;
	SessionPtr	iSubSess;
//	Uint32		nmbrRefs = 0;	// How many components use segment start ?
	Uint32		nbrComps = iMuxSess->getNbrComponents();

	for ( Uint32 i = 0 ; i < nbrComps ; i++ ) {
		SessionPtr	candSess = iMuxSess->getComponent( i );
		if ( candSess->doesUseSegmentStart()
		  && ( ! iSubSess
		    || ( candSess->getStream().isA< VideoStream >()
		      && ! iSubSess->getStream().isA< VideoStream >() ) ) ) {
//			nmbrRefs++;
			iSubSess = candSess;
			compIndx = i;
		}
	}

	if ( ! iSubSess ) {
		throw L2::NotSuitable( "No segmentation reference!" );
	}

	frstStrt = false;
	currFrme.kill();
	currFrme.resize( nbrComps );

//	msgDbg( "putSession(): segmentation ref component: " + Buffer( compIndx ) );

	MuxedSessionPtr	oMuxSess = iMuxSess;

	oMuxSess->setUseSegmentStart( true );

	getPeerEngine( 0 )->putSessionCallback( oMuxSess );

	setInSession( true );

}

void SegUtils::MuxedSynchronizer::endSessionCallback() {

	currFrme.kill();

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void SegUtils::MuxedSynchronizer::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame(): " + pFrame->toBuffer() );

	MuxedFramePtr	muxdFrme = pFrame;
	Uint32		muxdIndx = muxdFrme->getSubIndex();
	FramePtr	subFrame = muxdFrme->getSubFrame();

	if ( muxdIndx == compIndx ) {
		for ( Uint32 i = 0 ; i < currFrme.getSize() ; i++ ) {
			currFrme[ i ].kill();
		}
	}
	else if ( ! currFrme[ muxdIndx ] ) {
		currFrme[ muxdIndx ] = subFrame;
	}

	Bool		segmStrt;

	if ( ! subFrame->isSegmentStart() ) {
		segmStrt = false;
	}
	else if ( muxdIndx == compIndx ) {
		segmStrt = true;
	}
	else if ( subFrame->getDTS() - currFrme[ muxdIndx ]->getDTS() >= segmDrtn * 2 ) {
//		msgWrn( "putFrameCallback(): forcing segment start!" );
		segmStrt = true;
		for ( Uint32 i = 0 ; i < currFrme.getSize() ; i++ ) {
			if ( i != muxdIndx ) {
				currFrme[ i ].kill();
			}
		}
		currFrme[ muxdIndx ] = subFrame;
	}
	else {
		segmStrt = false;
	}


	if ( segmStrt ) {
		frstStrt = true;
	}
	else if ( ! frstStrt ) {
		msgWrn( "putFrame(): not synced yet... dropping frame..." );
		return;
	}

	muxdFrme->setSegmentStart( segmStrt );

	getPeerEngine( 0 )->putFrameCallback( muxdFrme );

}

// ============================================================================

