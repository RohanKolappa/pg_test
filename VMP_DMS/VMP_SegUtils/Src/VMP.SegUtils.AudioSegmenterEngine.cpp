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
//	VMP.SegUtils.AudioSegmenterEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.SegUtils.AudioSegmenterEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( SegUtils, AudioSegmenterEngine )

// ============================================================================

const DL::TypeCPtr & SegUtils::AudioSegmenterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "2419538b-0a84-4177-be07-314b4755b146" ),
		"VMP.SegUtils.AudioSegmenterEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"AudioSegmenterEngine",
			"Utils",
			"xxx" /*,
			SegUtils::userInterface */
		)
	);

	return i;

}

// ============================================================================

SegUtils::AudioSegmenterEngine::AudioSegmenterEngine() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

}

SegUtils::AudioSegmenterEngine::~AudioSegmenterEngine() {

}

// ============================================================================

void SegUtils::AudioSegmenterEngine::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	if ( ! segmDrtn ) {
		throw NotInitialized();
	}

	segmTime = Time::Clock();
	segmOffs = Frame::NoOffset;

	pSession->setUseSegmentStart( true );

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void SegUtils::AudioSegmenterEngine::endSessionCallback() {

//	msgDbg( "endSession()" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void SegUtils::AudioSegmenterEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame()" );

	BytesFramePtr	inptFrme = pFrame;

	Uint64		currOffs = inptFrme->getUnitOffset();
	Time::Clock	currTime = inptFrme->getDTS();

	BytesFramePtr	outpFrme = inptFrme;

	if ( segmOffs == Frame::NoOffset
	  || currTime - segmTime >= segmDrtn ) {
//		msgDbg( "putFrameCallback(): starting chunk at ["
//			+ Buffer( currOffs ) + "]!" );
		outpFrme->setSegmentStart( true );
		segmTime = currTime;
		segmOffs = currOffs;
	}
	else if ( inptFrme->isSegmentStart() ) {
		outpFrme->setSegmentStart( false );
	}

	try {
		getPeerEngine( 0 )->putFrameCallback( outpFrme );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		throw;
	}

}

void SegUtils::AudioSegmenterEngine::flushSessionCallback() {

//	msgDbg( "flushSession()" );

	segmOffs = Frame::NoOffset;

}

// ============================================================================

