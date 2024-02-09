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
//	VMP.SegUtils.VideoSegmenterEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.SegUtils.VideoSegmenterEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( SegUtils, VideoSegmenterEngine )

// ============================================================================

const DL::TypeCPtr & SegUtils::VideoSegmenterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b84d5999-0a4b-4f6a-8078-d985b358476e" ),
		"VMP.SegUtils.VideoSegmenterEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"VideoSegmenterEngine",
			"Utils",
			"VideoSegmenterEngine" /*,
			SegUtils::userInterface */
		)
	);

	return i;

}

// ============================================================================

SegUtils::VideoSegmenterEngine::VideoSegmenterEngine() :

	L2::ProxyPushEngine	( getClassType() ),

	forceCut		( false ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

}

SegUtils::VideoSegmenterEngine::~VideoSegmenterEngine() {

}

// ============================================================================

void SegUtils::VideoSegmenterEngine::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	if ( ! segmDrtn ) {
		throw NotInitialized();
	}

	VideoStreamPtr	iVidStrm = pSession->getStream();

//	if ( ! iVidStrm->getFrameRate() ) {
//		throw NotInitialized( "Input framerate == 0!" );
//	}

	if ( iVidStrm->getFrameRate() ) {
		gotFRate = true;
		frmeRate = iVidStrm->getFracRate();
	}
	else {
		gotFRate = false;
		gotFirst = false;
	}

	frmPrSeg = ( Uint64 )( segmDrtn * frmeRate ).toInt64();
	segmStrt = Frame::NoOffset;
	estGopSz = 0;
	lastKFrm = Frame::NoOffset;

	pSession->setUseSegmentStart( true );

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void SegUtils::VideoSegmenterEngine::endSessionCallback() {

//	msgDbg( "endSession()" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void SegUtils::VideoSegmenterEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame()" );

	if ( pFrame->hasDataDisc() ) {
		gotFirst = false;	// Force frate estimation reset (if any).
		lastKFrm = Frame::NoOffset;
	}

	// Estimate frame rate, if needed...

	if ( ! gotFRate ) {
		computeRates( pFrame );
	}

	BytesFramePtr	inptFrme = pFrame;

	Uint64	unitOffs = inptFrme->getUnitOffset();
	Bool	isKeyFrm = inptFrme->isKeyFrame();

//	msgDbg( "putFrame(): " + Buffer( unitOffs )
//		+ ", kf: " + Buffer( isKeyFrm ) );

	if ( isKeyFrm ) {
		if ( lastKFrm == Frame::NoOffset ) {
//			msgDbg( "putFrameCallback(): recording frame ["
//				+ Buffer( unitOffs ) + "] as first KF!" );
		}
		else if ( estGopSz ) {
			Uint64	newGopSz = unitOffs - lastKFrm;
			if ( newGopSz < estGopSz ) {
//				msgDbg( "putFrameCallback(): short GOP ("
//					+ Buffer( newGopSz ) + ")!" );
			}
			else if ( newGopSz > estGopSz ) {
				estGopSz = newGopSz;
				msgWrn( "putFrameCallback(): new gop size ("
					+ Buffer( estGopSz ) + ")!" );
			}
		}
		else {
			estGopSz = unitOffs - lastKFrm;
//			msgDbg( "putFrameCallback(): estimated gop size: "
//				+ Buffer( estGopSz ) );
			if ( estGopSz > frmPrSeg ) {
				msgWrn( "putFrameCallback(): gopSize: "
					+ Buffer( estGopSz ) + ", frames per segment: "
					+ Buffer( frmPrSeg ) );
			}
		}
		lastKFrm = unitOffs;
	}

	Bool	strtSegm = false;

	// Case 1: Start of session...

	if ( segmStrt == Frame::NoOffset ) {
		strtSegm = true;
	}

	// Case 2: Cut at keyframe...

	if ( isKeyFrm
	  && estGopSz
	  && unitOffs - segmStrt + estGopSz > frmPrSeg ) {
		strtSegm = true;
	}

	// Case 3: Force cut if at end of segment...

	if ( forceCut
	  && unitOffs - segmStrt >= frmPrSeg ) {
		strtSegm = true;
	}

	BytesFramePtr	outpFrme = inptFrme;

	if ( strtSegm ) {
//		msgDbg( "putFrameCallback(): starting chunk at ["
//			+ Buffer( unitOffs ) + "]!" );
		outpFrme->setSegmentStart( true );
		segmStrt = unitOffs;
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

void SegUtils::VideoSegmenterEngine::flushSessionCallback() {

//	msgDbg( "flushSession()" );

	segmStrt = Frame::NoOffset;
	lastKFrm = Frame::NoOffset;

}

// ============================================================================

void SegUtils::VideoSegmenterEngine::computeRates(
		FramePtr	pFrame ) {

	// First time we enter this method, record the frame.
	// Then replace this frame with the next keyframe.
	// Then wait for the next keyframe to do the math.
	// At any moment, don't use more than 100 frames to produce a result.

	BytesFramePtr	byteFrme = pFrame;
	Bool		currAKey = byteFrme->isKeyFrame();
	Uint64		currOffs = byteFrme->getUnitOffset();
	Time::Clock	currTime = byteFrme->getDTS();

	if ( ! gotFirst ) {
		gotFirst = true;
		frstAKey = currAKey;
		frstOffs = currOffs;
		frstTime = currTime;
		prevOffs = currOffs;
		return;
	}

	// Sanity check...

	if ( currOffs != prevOffs + 1 ) {
		throw InternalError( "UnitOffset discontinuity!" );
	}

	prevOffs = currOffs;	// Record it for next time...

	if ( frstAKey && currAKey ) {
		// Got 2 keyframes. Let's do some math...
	}
	else if ( currOffs - frstOffs > 100 ) { // Rule of thumb...
		// Don't delay it anymore...
	}
	else if ( currAKey ) {
		// Update first frame (a non-keyframe)...
		frstAKey = true;
		frstOffs = currOffs;
		frstTime = currTime;
		return;
	}
	else {
		return;
	}

	Time::Delta	chnkDrtn = currTime - frstTime;
	Uint64		chnkSize = currOffs - frstOffs;

	msgDbg( "computeRates(): chnkDrtn: " + chnkDrtn.toBuffer()
		+ ", chnkSize: " + Buffer( chnkSize ) );

	if ( ! chnkDrtn ) {
		throw InternalError( "DTS definitely stalled!" );
	}

	Double		tempRate = ( Double )chnkSize * 1000000.0 / ( Double )chnkDrtn;

	static const Double rateTble[] = {
		12.5, 15.0, 25.0, 30.0, 0.0
	};

	for ( Uint32 i = 0 ; rateTble[ i ] != 0.0 ; i++ ) {
		Double	candRate = rateTble[ i ];
		if ( tempRate >= candRate - 0.1
		  && tempRate <= candRate + 0.1 ) {
			tempRate = candRate;
			break;
		}
	}

	frmeRate = Frac64( tempRate );

	msgDbg( "computeRates(): frmeRate: " + frmeRate.toBuffer() );

	gotFRate = true;

	frmPrSeg = ( Uint64 )( segmDrtn * frmeRate ).toInt64();

}

// ============================================================================

static L2::EngineRegisterer< SegUtils::VideoSegmenterEngine > engineRegisterer;

// ============================================================================

