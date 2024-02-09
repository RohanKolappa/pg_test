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
//	VMP.RTP.TProgramPrinterEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.BufferIO.h"

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TProgramPrinterEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace std;
using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TProgramPrinterEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TProgramPrinterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f4d99bd8-f191-4264-9808-13eebed7f220" ),
		"VMP.RTP.TProgramPrinterEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP TProgram Printer",
			"Devices",
			"RTP TProgram Printer" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TProgramPrinterEngine::TProgramPrinterEngine(
	const	Bool		pDumpSess,
	const	Bool		pDumpPckt,
	const	Bool		pPrtHrtBt ) :

	FinalPushEngine( getClassType() ),

	dumpSess	( pDumpSess ),
	dumpPckt	( pDumpPckt ),
	prtHrtBt	( pPrtHrtBt ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

}

// ============================================================================

void RTP::TProgramPrinterEngine::putSessionCallback(
		SessionPtr	pOutpSess ) {

	MuxedSessionCPtr	iMuxSess = pOutpSess;

	for ( Uint32 i = 0 ; i < iMuxSess->getNbrComponents() ; i++ ) {
		TSessionCPtr	rtpTSess = iMuxSess->getComponent( i );
		printTSession( i, rtpTSess );
	}

	setInSession( true );

}

void RTP::TProgramPrinterEngine::endSessionCallback() {

	setInSession( false );

}

void RTP::TProgramPrinterEngine::putFrameCallback(
		FramePtr	pOutpFrme ) {

	MuxedFramePtr	iMuxFrme = pOutpFrme;

	Uint32		strIndex = iMuxFrme->getSubIndex();
	TFramePtr	rtpTFrme = iMuxFrme->getSubFrame();

	printTFrame( strIndex, rtpTFrme );

}

// ============================================================================

void RTP::TProgramPrinterEngine::printTSession(
	const	Uint32		pStrIndex,
		TSessionCPtr	pRtpTSess ) {

	if ( dumpSess ) {
		cerr << "[" << pStrIndex << "] " << pRtpTSess->toBuffer() << endl;
	}

}

void RTP::TProgramPrinterEngine::printTFrame(
	const	Uint32		pSubIndex,
		TFrameCPtr	pRtpTFrme ) {

	if ( dumpSess ) {
		cerr << "[" << pSubIndex << "] " << pRtpTFrme->toBuffer() << endl;
	}

	if ( prtHrtBt ) {
		Time::Clock	curHrtBt = Time::now();
		if ( curHrtBt - lstHrtBt >= Time::OneSecond ) {
			cerr << curHrtBt.dateTimeToBuffer( false, true ) << endl;
			lstHrtBt = curHrtBt;
		}
	}

}

// ============================================================================

static L2::EngineRegisterer< RTP::TProgramPrinterEngine > engineRegisterer;

// ============================================================================

