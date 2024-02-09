// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TSDecoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.BackendRegisterer.h"

#include "VMP.M2S.PESFrame.h"
#include "VMP.M2S.PESPacketFramer.h"
#include "VMP.M2S.PESSession.h"
#include "VMP.M2S.TSDecoderBackend.h"
#include "VMP.M2S.TSPacketDispatcher.h"
#include "VMP.M2S.TSPacketFramer.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSDecoderBackend )

// ============================================================================

const DL::TypeCPtr & M2S::TSDecoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "27988f10-97fb-4e3f-8ed0-818407ad53d0" ),
		"VMP.M2S.TSDecoderBackend",
		L3::StreamDecoderBackend::getClassType(),
		new UI::UObjectDescr(
			"MPEG2 TS Decoder",
			"Codecs,System",
			"MPEG2 TransportStream decoder"
		)
	);

	return i;

}

// ============================================================================

M2S::TSDecoderBackend::TSDecoderBackend() :

	L3::StreamDecoderBackend( getClassType() ),

	pid	( ( Uint16 )-1 ) {

}

// ============================================================================

const DL::TypeCPtr & M2S::TSDecoderBackend::getOutputType() const {

	return PESSession::getClassType();

}

// ============================================================================

Bool M2S::TSDecoderBackend::canDecode(
		BytesSessionPtr	/*pIBSession*/ ) {

	return ( getIODevice()->peekUchar() == 0x47 );

}

// ============================================================================

SessionPtr M2S::TSDecoderBackend::initSession(
		BytesSessionPtr	/*pIBSession*/ ) {

	TSPacketFramerPtr	tsFramer = new TSPacketFramer;
	TSPacketDispatcherPtr	tsDisper = new TSPacketDispatcher;

	tsFramer->open();
	tsFramer->setPeerConsumer( tsDisper );

	PESPacketFramerPtr	pesFrmer = new PESPacketFramer;

	tsDisper->setPeerConsumer( pid, pesFrmer );
	pesFrmer->setPeerConsumer( getThis() );

	frm = tsFramer;

	return new PESSession( Session::Pausable );

}

void M2S::TSDecoderBackend::exitSession() {

	frm.kill();
	pck.kill();

}

FramePtr M2S::TSDecoderBackend::getFrame() {

	try {
		while ( pck.isEmpty() ) {
			frm->putBytes( getIODevice()->getBytes() );
		}
	}
	catch ( IO::EndOfStream & ) {
		throw EndOfStream();
	}

	return new PESFrame( pck.popFirst() );

}

void M2S::TSDecoderBackend::seekFrame(
	const	Uint64		/* pIndex */ ) {

	throw NotSuitable();

}

// ============================================================================

void M2S::TSDecoderBackend::putObject(
		SPtr		pPESPackt ) {

	pck += pPESPackt;

}

// ============================================================================

static L3::BackendRegisterer< M2S::TSDecoderBackend > tsDecoderRegisterer;

// ============================================================================

