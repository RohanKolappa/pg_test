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
//	VMP.AVC.FramerPushEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.AVC.CodecConfig.h"
#include "VMP.AVC.FramerPushEngine.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.Frame.h"
#include "VMP.AVC.Session.h"
#include "VMP.AVC.SequenceParameterSet.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, FramerPushEngine )

// ============================================================================

const DL::TypeCPtr & AVC::FramerPushEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "3ca0e47c-5c89-49e7-a72e-d169d2280fc8" ),
		"VMP.AVC.FramerPushEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"AVC",
			"Utils",
			"Computes the input rate (video frame rate or audio sample rate)." /*,
			AVC::userInterface */
		)
	);

	return i;

}

// ============================================================================

AVC::FramerPushEngine::FramerPushEngine() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	syncOnKF = true;
//	useCfgRC = true;

}

AVC::FramerPushEngine::~FramerPushEngine() {

}

// ============================================================================

void AVC::FramerPushEngine::putSessionCallback(
		VMP::SessionPtr	pSession ) {

	msgDbg( "putSession(): " + pSession->toBuffer() );

	iBytSess = pSession;
	iVidStrm = pSession->getStream();

	sentSess = false;
	seenKFrm = false;

	if ( iVidStrm->getExtraData().hasContent() ) {
		CodecConfigPtr cfg = new CodecConfig;
		cfg->decode( iVidStrm->getExtraData() );
		if ( cfg->getNbrSPSs() != 1
		  || cfg->getNbrPPSs() != 1 ) {
			throw InternalError( "Missing SPS/PPS!" );
		}
		spsCache = cfg->getSPS( 0 );
		ppsCache = cfg->getPPS( 0 );
//		msgDbg( "putSession(): got SPS/PPS from cfg_rec!" );
	}
	else {
		spsCache.kill();
		ppsCache.kill();
	}

	oBytSess = iBytSess->clone();
	oVidStrm = oBytSess->getStream();

	setInSession( true );

}

void AVC::FramerPushEngine::endSessionCallback() {

	msgDbg( "endSession()" );

	setInSession( false );

	if ( sentSess ) {
		getPeerEngine( 0 )->endSessionCallback();
	}

}

void AVC::FramerPushEngine::putFrameCallback(
		VMP::FramePtr	pFrame ) {

	msgDbg( "putFrame()" );

	FrameCPtr	iFrme = pFrame;
	NALUCPtr	iNalu = iFrme->getNALU();

	Uchar		iType = iNalu->getType();

	if ( iType == NALU::TypeSPS ) {
		if ( ! spsCache ) {
			spsCache = iNalu;
		}
		else if ( spsCache->getData() != iNalu->getData() ) {
			msgWrn( "Mutating SPS!" );
//			throw InternalError( "Mutating SPS!" );
		}
		return;
	}

	if ( iType == NALU::TypePPS ) {
		if ( ! ppsCache ) {
			ppsCache = iNalu;
		}
		else if ( ppsCache->getData() != iNalu->getData() ) {
			msgWrn( "Mutating PPS!" );
//			throw InternalError( "Mutating PPS!" );
		}
		return;
	}

	if ( iType == NALU::TypeIDR ) {
		seenKFrm = true;
	}
	else if ( iType == NALU::TypeSlice ) {
		if ( ! seenKFrm ) {
			if ( ! syncOnKF ) {
				msgWrn( "putFrame(): will maybe send slice without ref!" );
			}
			else {
				msgDbg( "putFrame(): dropping slice..." );
				return;
			}
		}
	}
	else {
		msgWrn( "putFrame(): unsupported NALU type ("
			+ NALU::getTypeName( iType ) + ")!" );
		return;
	}

	if ( ! sentSess ) {
		if ( oVidStrm->getExtraData().isEmpty() ) {
			if ( ! spsCache || ! ppsCache ) {
				return;
			}
			CodecConfigPtr cfg = new CodecConfig;
			cfg->addSPS( spsCache );
			cfg->addPPS( ppsCache );
			oVidStrm->setExtraData( cfg->encode() );
		}
		getPeerEngine( 0 )->putSessionCallback( oBytSess );
		sentSess = true;
	}

	static Buffer prefix = Buffer( ( const Uchar * )"\x00\x00\x00\x01", 4, false );

	BytesFramePtr	oFrme = new BytesFrame( prefix + iNalu->getData() );

	oFrme->setKeyFrame( iType == NALU::TypeIDR );
//	oFrme->setUnitOffset( offset );
	oFrme->setPTS( iFrme->getPTS() );
	oFrme->setDTS( iFrme->getDTS() );

	getPeerEngine( 0 )->putFrameCallback( oFrme );

}

// ============================================================================

static L2::EngineRegisterer< AVC::FramerPushEngine > framerPushRegisterer;

// ============================================================================

