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
//	VMP.RTP.TEStreamEncoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Math.Utils.h"

#include "VMP.RTP.TEStreamEncoderEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TEStreamEncoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TEStreamEncoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "92542168-b3a8-4cc9-9cab-bb9641fb5874" ),
		"VMP.RTP.TEStreamEncoderEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP EStream Encoder",
			"Devices",
			"RTP EStream Encoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TEStreamEncoderEngine::TEStreamEncoderEngine(
	const	DL::TypeCPtr &	pClssType ) :

	L2::ProxyPushEngine	( pClssType ),

	currSqNb	( Math::rand() ),
	currSSRC	( Math::rand() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );
	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

// ============================================================================

RTP::TFramePtr RTP::TEStreamEncoderEngine::makeTFrame() {

	TFramePtr	rtpTFrme = new TFrame;

	rtpTFrme->SN	= currSqNb++;
	rtpTFrme->SSRC	= currSSRC;

	return rtpTFrme;

}

// ============================================================================

