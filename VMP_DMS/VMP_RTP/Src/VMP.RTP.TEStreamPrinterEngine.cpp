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
//	VMP.RTP.TEStreamPrinterEngine.cpp
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

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TEStreamPrinterEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace std;
using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TEStreamPrinterEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TEStreamPrinterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "60ae355d-344f-438b-aa2c-fe17c6c42f5e" ),
		"VMP.RTP.TEStreamPrinterEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP TFrame Printer",
			"Devices",
			"RTP TFrame Printer" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TEStreamPrinterEngine::TEStreamPrinterEngine() :

	FinalPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

}

// ============================================================================

void RTP::TEStreamPrinterEngine::putSessionCallback(
		SessionPtr	pOutpSess ) {

	TSessionCPtr	rtpTSess = pOutpSess;

	printTSession( rtpTSess );

	setInSession( true );

}

void RTP::TEStreamPrinterEngine::endSessionCallback() {

	setInSession( false );

}

void RTP::TEStreamPrinterEngine::putFrameCallback(
		FramePtr	pOutpFrme ) {

	TFramePtr	rtpTFrme = pOutpFrme;

	printTFrame( rtpTFrme );

}

// ============================================================================

void RTP::TEStreamPrinterEngine::printTSession(
		TSessionCPtr	/*pRtpTSess*/ ) {

}

void RTP::TEStreamPrinterEngine::printTFrame(
		TFrameCPtr	pRtpTFrme ) {

	cerr << pRtpTFrme->toBuffer() << endl;

}

// ============================================================================

static L2::EngineRegisterer< RTP::TEStreamPrinterEngine > engineRegisterer;

// ============================================================================

