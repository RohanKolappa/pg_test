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
//	VMP.DMS.RTPMultiInjector.cpp
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

#include "VMP.DMS.RTPMultiInjector.h"

#include "VMP.L2.PushEngine.h"

#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.NSession.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.SProgram.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( DMS, RTPMultiInjector )

// ============================================================================

DMS::RTPMultiInjector::RTPMultiInjector() {

	setNbrOutputs( 1 );

}

DMS::RTPMultiInjector::~RTPMultiInjector() {

}

// ============================================================================

void DMS::RTPMultiInjector::init(
		RTP::SProgramPtr
				pSProgram ) {

	vidIndex = ( Uint32 )-1;
	audIndex = ( Uint32 )-1;

	MuxedSessionPtr		oMuxSess = new MuxedSession;

	for ( Uint32 i = 0 ; i < pSProgram->getNbrSEStreams() ; ) {

		RTP::SEStreamCPtr	sEStream = pSProgram->getSEStream( i );
		Buffer			metaType = sEStream->getMetaType();

		if ( metaType == "video" ) {
			vidIndex = i++;
		}
		else if ( metaType == "audio" ) {
			audIndex = i++;
		}
		else {
			msgWrn( "init(): popping unsupported type \"" + metaType + "\"!" );
			pSProgram->popSEStream( i );
			continue;
//			throw InternalError();
		}

		StreamPtr		outpStrm = sEStream->toStream();
		RTP::NSessionPtr	oNetSess = new RTP::NSession( Session::Live, outpStrm, pSProgram, sEStream );

		oMuxSess->addComponent( oNetSess );

	}

	getPeerEngine( 0 )->putSessionCallback( oMuxSess );

}

void DMS::RTPMultiInjector::cleanup() {

	getPeerEngine( 0 )->endSessionCallback();

}

void DMS::RTPMultiInjector::putVideoPacket(
		RTP::NFramePtr	pNetFrame ) {

	getPeerEngine( 0 )->putFrameCallback(
		new MuxedFrame( vidIndex, pNetFrame ) );

}

void DMS::RTPMultiInjector::putAudioPacket(
		RTP::NFramePtr	pNetFrame ) {

	getPeerEngine( 0 )->putFrameCallback(
		new MuxedFrame( audIndex, pNetFrame ) );

}

// ============================================================================

