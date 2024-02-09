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
//	VMP.L2.PushDemuxerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.DemuxerFilter.h"
#include "VMP.L2.EngineRegisterer.h"
#include "VMP.L2.PushDemuxerEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PushDemuxerEngine )

// ============================================================================

const DL::TypeCPtr & L2::PushDemuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "4b5b3635-98ba-42b2-87c3-fad965bbd3e8" ),
		"VMP.L2.PushDemuxerEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"PushDemuxerEngine",
			"Utils",
			"PushDemuxerEngine"
		)
	);

	return i;

}

// ============================================================================

L2::PushDemuxerEngine::PushDemuxerEngine() :

	ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

}

L2::PushDemuxerEngine::~PushDemuxerEngine() {

}

// ============================================================================

void L2::PushDemuxerEngine::addOutput(
		DemuxerFilterPtr
				pStrmFltr ) {

	Uint32	i = outpFltr.getSize();

	outpFltr += pStrmFltr;

	setNbrOutputs( i + 1 );
	setOutputType( i, Session::getClassType() );

}

// ============================================================================

void L2::PushDemuxerEngine::putSessionCallback(
		SessionPtr	pSession ) {

	MuxedSessionPtr	iMuxSess = pSession;

	outpMppr.kill();
	outpMppr.resize( iMuxSess->getNbrComponents() );

	for ( Uint32 i = 0 ; i < outpFltr.getSize() ; i++ ) {
		Uint32	s = outpFltr[ i ]->filter( iMuxSess );
		if ( s == DemuxerFilter::NoSessionId ) {
			throw NotSuitable( "Can't find input session for output [ "
				+ Buffer( i ) + " ]!" );
		}
		outpMppr[ s ] = i;
		getPeerEngine( i )->putSessionCallback( iMuxSess->getComponent( s ) );
	}

	setInSession( true );

}

void L2::PushDemuxerEngine::endSessionCallback() {

	setInSession( false );

	for ( Uint32 i = 0 ; i < getNbrOutputs() ; i++ ) {
		if ( getPeerEngine( i )->isInSession() ) {
			getPeerEngine( i )->endSessionCallback();
		}
	}

}

void L2::PushDemuxerEngine::putFrameCallback(
		FramePtr	pFrame ) {

	MuxedFramePtr	iMuxFrme = pFrame;
	Uint32		iSubIndx = iMuxFrme->getSubIndex();
	FramePtr	iSubFrme = iMuxFrme->getSubFrame();

	try {
		getPeerEngine( outpMppr[ iSubIndx ] )->putFrameCallback( iSubFrme );
	}
	catch ( BrokenSession & ) {
		setInSession( false );
		throw;
	}

}

void L2::PushDemuxerEngine::flushSessionCallback() {

	for ( Uint32 i = 0 ; i < getNbrOutputs() ; i++ ) {
		getPeerEngine( i )->flushSessionCallback();
	}

}

// ============================================================================

static L2::EngineRegisterer< L2::PushDemuxerEngine > registerer;

// ============================================================================

