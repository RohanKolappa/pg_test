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
//	VMP.RTP.TProgramSyncerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TProgramSyncerEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TProgramSyncerEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TProgramSyncerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "0f659b10-bf6c-4ac7-9fc9-6a98074a485b" ),
		"VMP.RTP.TProgramSyncerEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"TProgramSyncerEngine",
			"Utils",
			"TProgramSyncerEngine"
		)
	);

	return i;

}

// ============================================================================

RTP::TProgramSyncerEngine::TProgramSyncerEngine() :

	ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

RTP::TProgramSyncerEngine::~TProgramSyncerEngine() {

}

// ============================================================================

void RTP::TProgramSyncerEngine::putSessionCallback(
		SessionPtr	pSession ) {

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void RTP::TProgramSyncerEngine::endSessionCallback() {

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TProgramSyncerEngine::putFrameCallback(
		FramePtr	pFrame ) {

	try {
		getPeerEngine( 0 )->putFrameCallback( pFrame );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		throw;
	}

}

void RTP::TProgramSyncerEngine::flushSessionCallback() {

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

static L2::EngineRegisterer< RTP::TProgramSyncerEngine > registerer;

// ============================================================================

