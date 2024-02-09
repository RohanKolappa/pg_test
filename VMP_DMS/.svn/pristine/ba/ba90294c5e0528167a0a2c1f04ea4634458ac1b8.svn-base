// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::RTP".
// 
// "VMP::RTP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::RTP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::RTP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.TTSConverterEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"
#include "VMP.RTP.TTSConverterEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TTSConverterEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TTSConverterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1a928fe9-5c09-4ea7-b177-c66f7a4df8cf" ),
		"VMP.RTP.TTSConverterEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"TTSConverterEngine",
			"Utils",
			"TTSConverterEngine"
		)
	);

	return i;

}

// ============================================================================

RTP::TTSConverterEngine::TTSConverterEngine() :

	ProxyPushEngine	( getClassType() ),

	timeScle	( 0 ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

// ============================================================================

void RTP::TTSConverterEngine::putSessionCallback(
		SessionPtr	pSession ) {

	if ( ! timeScle ) {
		throw NotInitialized();
	}

	initDone = false;

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void RTP::TTSConverterEngine::endSessionCallback() {

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TTSConverterEngine::putFrameCallback(
		FramePtr	pFrame ) {

	TFramePtr	rtpTFrme = pFrame;
	Time::Clock	timeStmp = convert( rtpTFrme->TS, rtpTFrme->hasTimeDisc() );

	rtpTFrme->setPTS( timeStmp );
	rtpTFrme->setDTS( timeStmp );

//	msgDbg( "putFrameCallback(): " + rtpTFrme->toBuffer() );

	try {
		getPeerEngine( 0 )->putFrameCallback( pFrame );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		throw;
	}

}

// ============================================================================

Time::Clock RTP::TTSConverterEngine::convert(
	const	Uint32		pTimeStmp,
	const	Bool		pTimeDisc ) {

	// Test sequences:
	// - 0, 0xFFFFFFFF, 1, ...

	if ( pTimeDisc ) {
		initDone = false;
	}

	Uint64		currStmp = pTimeStmp;
	Time::Delta	currDlta;

	if ( ! initDone ) {
		initDone = true;
		frstStmp = currStmp;
//		timeBase = Time::now();
		timeBase = Time::Clock( 1980, 1, 1, 0, 0, 0 );
	}
	else {
		currStmp |= ( lastStmp & Mask_63_32 );
//		Uint64 prevStmp = currStmp;
		if ( currStmp + Mod_2_31 < lastStmp ) {
			currStmp += Mod_2_32;
		}
		else if ( currStmp > lastStmp + Mod_2_31 ) {
			currStmp -= Mod_2_32;
		}
		currDlta = ( Int64 )( currStmp - frstStmp ) * 1000000 / ( Int64 )timeScle;
//		msgDbg( "frst: " + Buffer( frstStmp, Buffer::Base16, 16 )
//			+ ", last: " + Buffer( lastStmp, Buffer::Base16, 16 )
//			+ ", prev: " + Buffer( prevStmp, Buffer::Base16, 16 )
//			+ ", curr: " + Buffer( currStmp, Buffer::Base16, 16 )
//			+ ", dlta: " + currDlta.toBuffer() + " (" + Buffer( ( Int64 )currDlta ) + ")" );
	}

	lastStmp = currStmp;

	return ( timeBase + currDlta );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TTSConverterEngine > registerer;

// ============================================================================

