// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "VMP::HLS".
//
// "VMP::HLS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "VMP::HLS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::HLS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	VMP.HLS.FrameRateComputer.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.HLS.FrameRateComputer.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, FrameRateComputer )

// ============================================================================

const DL::TypeCPtr & HLS::FrameRateComputer::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "8f5c2bac-7617-412c-80f8-5492a4193d6a" ),
		"VMP.HLS.FrameRateComputer",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"FrameRateComputer",
			"Utils",
			"Computes framerate based on unit offsets and timestamps."
		)
	);

	return i;

}

// ============================================================================

HLS::FrameRateComputer::FrameRateComputer() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

}

HLS::FrameRateComputer::~FrameRateComputer() {

}

// ============================================================================

void HLS::FrameRateComputer::putSessionCallback(
		SessionPtr	pSession ) {

	VideoStreamPtr	iVidStrm = pSession->getStream();

	if ( iVidStrm->getFrameRate() ) {
		gotFRate = true;
		getPeerEngine( 0 )->putSessionCallback( pSession );
	}
	else {
		gotFRate = false;
		inptSess = pSession;
		inptStrm = iVidStrm;
		inptFrme.kill();
		gotKFrme = false;
	}

	setInSession( true );

}

void HLS::FrameRateComputer::endSessionCallback() {

	setInSession( false );

	if ( gotFRate ) {
		getPeerEngine( 0 )->endSessionCallback();
	}
	else {
		inptSess.kill();
		inptFrme.kill();
	}

}

void HLS::FrameRateComputer::putFrameCallback(
		FramePtr	pFrame ) {

	if ( gotFRate ) {
		getPeerEngine( 0 )->putFrameCallback( pFrame );
		return;
	}

	Uint64	currIndx = inptFrme.getSize();

	inptFrme += pFrame;

	Bool	isKFrame = pFrame->isKeyFrame();
	Bool	flshCche;
	Uint64	strtFrme;
	Uint64	stopFrme;

	if ( gotKFrme && isKFrame ) {
		// Got 2 keyframes. Let's do some math...
//		msgDbg( "putFrame(): got 2 keyframes!" );
		flshCche = true;
		strtFrme = kFrmIndx;
		stopFrme = currIndx;
	}
	else if ( inptFrme.getSize() > 100 ) { // Rule of thumb...
		// Don't delay it anymore...
//		msgDbg( "putFrame(): cache size limit reached!" );
		flshCche = true;
		strtFrme = 0;
		stopFrme = currIndx;
	}
	else if ( isKFrame ) {
		flshCche = false;
		gotKFrme = true;
		kFrmIndx = currIndx;
	}
	else {
		flshCche = false;
	}

	if ( ! flshCche ) {
		return;
	}

	Time::Delta	chnkDrtn = inptFrme[ stopFrme ]->getPTS()
			         - inptFrme[ strtFrme ]->getPTS();
	Uint64		chnkSize = stopFrme
			         - strtFrme;

//	msgDbg( "putFrame(): chnkDrtn: " + Buffer( ( Int64 )chnkDrtn )
//		+ ", chnkSize: " + Buffer( chnkSize ) );

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

	Frac64		frmeRate = Frac64( tempRate );

	inptStrm->setFrameRate( frmeRate );

//	msgDbg( "putFrame(): session: " + inptSess->toBuffer() );

	getPeerEngine( 0 )->putSessionCallback( inptSess );

	inptSess.kill();
	inptStrm.kill();

	for ( Uint32 i = 0 ; i < inptFrme.getSize() ; i++ ) {
		getPeerEngine( 0 )->putFrameCallback( inptFrme[ i ] );
	}

	inptFrme.kill();

	gotFRate = true;

}

void HLS::FrameRateComputer::flushSessionCallback() {

	if ( gotFRate ) {
		getPeerEngine( 0 )->flushSessionCallback();
	}

}

// ============================================================================

