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
//	VMP.RTP.TEStreamCheckerEngine.cpp
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
#include "VMP.RTP.TEStreamCheckerEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TEStreamCheckerEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TEStreamCheckerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e3ab656a-1c04-45cd-ae8c-0e813f03f2cd" ),
		"VMP.RTP.TEStreamCheckerEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Checker (Push)",
			"Devices",
			"RTP Checker (Push)" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TEStreamCheckerEngine::TEStreamCheckerEngine() :

	L2::ProxyPushEngine	( getClassType() ),

	multSSRC	( false ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );
	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

// ============================================================================

void RTP::TEStreamCheckerEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

//	msgDbg( "putSessionCallback(): --->" );

	gotFirst = false;

	getPeerEngine( 0 )->putSessionCallback( pInptSess );

//	msgDbg( "putSessionCallback(): <---" );

}

void RTP::TEStreamCheckerEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	getPeerEngine( 0 )->endSessionCallback();

//	msgDbg( "endSessionCallback(): <---" );

}

void RTP::TEStreamCheckerEngine::putFrameCallback(
	const	FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

	TFramePtr	rtpTFrme	= pInptFrme;

	Time::Clock	packTime	= rtpTFrme->readTime;
	Uint32		packSSRC	= rtpTFrme->SSRC;
	Uint32		packType	= rtpTFrme->PT;
	Uint16		packSqNb	= rtpTFrme->SN;
	Uint32		packTmSt	= rtpTFrme->TS;

//	msgDbg( "putFrameCallback(): packet SSRC: " + Buffer( packSSRC, Buffer::Base16, 8 ) );

	if ( ! gotFirst ) {

		currSSRC = packSSRC;
//		msgDbg( "putFrameCallback(): acquired SSRC: " + Buffer( currSSRC, Buffer::Base16, 8 ) );
		currType = packType;
//		msgDbg( "putFrameCallback(): acquired PT: " + Buffer( currType ) );
		nextSqNb = ( packSqNb + 1 ) % 65536;
//		msgDbg( "putFrameCallback(): acquired seq_nbr: " + Buffer( packSqNb ) );
		gotFirst = true;

	}
	else {

		// Check Rx time...

		if ( packTime < lastRead ) {
			msgWrn( "putFrameCallback(): Rx time rolling back ( "
				+ packTime.dateTimeToBuffer( false ,true ) + " < "
				+ lastRead.dateTimeToBuffer( false ,true ) + " )!" );
			return;
		}

		// Check SSRC...

		if ( currSSRC != packSSRC ) {
			msgWrn( "putFrameCallback(): SSRC switch detected ( "
				+ Buffer( currSSRC, Buffer::Base16, 8 ) + " --> "
				+ Buffer( packSSRC, Buffer::Base16, 8 ) + " )!" );
			if ( multSSRC ) {
				currSSRC = packSSRC;
				rtpTFrme->setDataDisc();
				rtpTFrme->setTimeDisc();
			}
			else {
//				throw InternalError( "Multiple SSRCs on this addr/port!" );
				msgWrn( "putFrameCallback(): Multiple SSRCs on this addr/port!" );
				return;
			}
		}

		// Check PT...

		if ( currType != packType ) {
//			throw InternalError( "Multiple PTs on this addr/port!" );
			msgWrn( "putFrameCallback(): Multiple PTs on this addr/port!" );
			return;
		}

		// Check SN...

		if ( nextSqNb != packSqNb ) {
			msgWrn( "putFrameCallback(): Lost packet! Expected " + Buffer( nextSqNb )
				+ ", got " + Buffer( packSqNb ) + "!" );
			rtpTFrme->setDataDisc();
			// Special case: SN disc and SN and TS reset to 0 --> TS disc too!
			if ( packSqNb < 5 && packTmSt <= packSqNb * 90000U ) {
				msgWrn( "putFrameCallback(): Source reset detected!" );
				rtpTFrme->setTimeDisc();
			}
			// Only allow small positive deltas...
			if ( ( Uint16 )( packSqNb - nextSqNb ) >= 4 ) {
				msgWrn( "putFrameCallback(): Forcing time discontinuity!" );
				rtpTFrme->setTimeDisc();
			}
		}

		// Check TS...

		if ( timeStmp >= 700000000 && timeStmp <= 800000000 && packTmSt <= 90000 ) {
			msgWrn( "putFrameCallback(): Detected TS discontinuity ( "
				+ Buffer( timeStmp ) + "  --> "
				+ Buffer( packTmSt ) + " )!" );
			rtpTFrme->setTimeDisc();
		}

	}

	nextSqNb = ( packSqNb + 1 ) % 65536;
	lastRead = packTime;
	timeStmp = packTmSt;

	getPeerEngine( 0 )->putFrameCallback( rtpTFrme );

//	msgDbg( "putFrameCallback(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TEStreamCheckerEngine > clientRegisterer;

// ============================================================================

