// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::DMS".
// 
// "VMP::DMS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::DMS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::DMS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.DMS.StreamCutter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "VMP.MuxedFrame.h"

#include "VMP.DMS.StreamCutter.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( DMS, StreamCutter )

// ============================================================================

const DL::TypeCPtr & DMS::StreamCutter::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "38717aec-22e8-4b19-a3cd-42008b91c651" ),
		"VMP.DMS.StreamCutter",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"StreamCutter",
			"Utils",
			"xxx" /*,
			DMS::userInterface */
		)
	);

	return i;

}

// ============================================================================

DMS::StreamCutter::StreamCutter() :

	L2::ProxyPushEngine	( getClassType() ),

	lastOpen		( false ) {

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, Session::getClassType() );

}

DMS::StreamCutter::~StreamCutter() {

}

// ============================================================================

void DMS::StreamCutter::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): --->" );

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

//	msgDbg( "putSession(): <---" );

}

void DMS::StreamCutter::endSessionCallback() {

//	msgDbg( "endSession(): --->" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

//	msgDbg( "endSession(): <---" );

}

void DMS::StreamCutter::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame(): --->" );

	Time::Clock	timeStmp = pFrame->getPTS();
	Time::Delta	timeDlta;
	Bool		flshSess = false;

//	msgDbg( "putFrame(): " + Buffer( ( Uint64 )timeStmp ) );

	{

	FastCSLocker	locker( critSect );

	if ( entrList.isEmpty() ) {
		msgWrn( "putFrame(): no cutlist (before purging) ?" );
		return;
	}

	while ( ! entrList.isEmpty()
	     && entrList[ 0 ].stopTime
	     && timeStmp >= entrList[ 0 ].stopTime ) {
		lastStop = entrList[ 0 ].stopTime;
                lastDlta = entrList[ 0 ].timeDlta;
		entrList.pop( 0 );
		flshSess = true;
	}

	if ( entrList.isEmpty() ) {
		msgWrn( "putFrame(): no cutlist (after purging) ?" );
		return;
	}

	if ( timeStmp < entrList[ 0 ].strtTime ) {
		msgWrn( "putFrame(): gate not open yet!" );
		return;
	}

	timeDlta = entrList[ 0 ].timeDlta;

	}

	if ( flshSess ) {
//		msgDbg( "putFrame(): flushing session..." );
		getPeerEngine( 0 )->flushSessionCallback();
	}

//	msgDbg( "putFrame(): " + Buffer( ( Uint64 )timeStmp )
//		+ ", delta: " + Buffer( ( Int64 )timeDlta ) );

	adjustTimeStamp( pFrame, timeDlta );

	getPeerEngine( 0 )->putFrameCallback( pFrame );

//	msgDbg( "putFrame(): <---" );

}

// ============================================================================

void DMS::StreamCutter::pause(
	const	Time::Clock &	pTimeStmp ) {

//	msgDbg( "pause(): " + Buffer( ( Uint64 )pTimeStmp ) );

	FastCSLocker	locker( critSect );

	if ( ! lastOpen ) {
		throw InternalError( "Not open!" );
	}

	entrList.getLast().stopTime = pTimeStmp;

	lastOpen = false;

//        msgDbg( "pause(): last entry: " + Buffer( entrList.getLast().strtTime ) + " - " + Buffer( entrList.getLast().stopTime ) + ", delta: " + Buffer( entrList.getLast().timeDlta ) );

}

void DMS::StreamCutter::resume(
	const	Time::Clock &	pTimeStmp ) {

//	msgDbg( "resume(): " + Buffer( ( Uint64 )pTimeStmp ) );

	FastCSLocker	locker( critSect );

	if ( lastOpen ) {
		throw InternalError( "Already open!" );
	}

	Time::Delta	timeDlta;

	if ( ! entrList.isEmpty() ) {
		timeDlta = pTimeStmp - entrList.getLast().stopTime
                         + entrList.getLast().timeDlta;
	}
	else if ( lastStop ) {
		timeDlta = pTimeStmp - lastStop
                         + lastDlta;
	}

	Entry		currEntr;

	currEntr.strtTime = pTimeStmp;
	currEntr.timeDlta = timeDlta;

	entrList += currEntr;
	lastOpen = true;

//      msgDbg( "resume(): last entry: " + Buffer( entrList.getLast().strtTime ) + " - " + Buffer( entrList.getLast().stopTime ) + ", delta: " + Buffer( entrList.getLast().timeDlta ) );

}

// ============================================================================

void DMS::StreamCutter::adjustTimeStamp(
		FramePtr	pCurrFrme,
	const	Time::Delta &	pTimeDlta ) const {

	for (;;) {
		if ( ( Uint64 )pCurrFrme->getPTS() != 0 ) {
			pCurrFrme->setPTS( pCurrFrme->getPTS() - pTimeDlta );
		}
		if ( pCurrFrme.isA< BytesFrame >() ) {
			BytesFramePtr byteFrme = pCurrFrme;
			if ( ( Uint64 )byteFrme->getDTS() != 0 ) {
				byteFrme->setDTS( byteFrme->getDTS() - pTimeDlta );
			}
		}
		if ( ! pCurrFrme.isA< MuxedFrame >() ) {
			break;
		}
		pCurrFrme = MuxedFramePtr( pCurrFrme )->getSubFrame();
	}

}

// ============================================================================

