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
//	VMP.HLS.ServerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedSession.h"

#include "VMP.AVC.Any2NALUFramer.h"
#include "VMP.AVC.NALU2AUFramer.h"

//#include "VMP.HLS.FrameRateComputer.h"
#include "VMP.HLS.ServerEngine.h"
#include "VMP.HLS.PlayListWriter.h"
#include "VMP.HLS.ServerConfig.h"

#include "VMP.L2.DemuxerFilter.h"
#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.EngineRegisterer.h"
#include "VMP.L2.ProxyPushFifo.h"
#include "VMP.L2.RemuxerEngine.h"

#include "VMP.M2S.TSEncoderEngine.h"

#include "VMP.SegUtils.AudioSegmenterEngine.h"
#include "VMP.SegUtils.MuxedSynchronizer.h"
#include "VMP.SegUtils.SegmentWriter.h"
#include "VMP.SegUtils.SimplePathComputer.h"
#include "VMP.SegUtils.VideoSegmenterEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, ServerEngine )

// ============================================================================

const DL::TypeCPtr & HLS::ServerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "2c8ea07f-164f-4fac-b5a0-a381ab048ed7" ),
		"VMP.HLS.ServerEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"HLS Server",
			"Devices",
			"HLS Server"
		)
	);

	return i;

}

// ============================================================================

HLS::ServerEngine::ServerEngine() :

	L2::FinalPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

}

HLS::ServerEngine::~ServerEngine() {

}

// ============================================================================

void HLS::ServerEngine::init(
		ServerConfigCPtr	pCnfgData ) {

	cnfgData	= pCnfgData;

	SegUtils::VideoSegmenterEnginePtr
				videoSgm = new SegUtils::VideoSegmenterEngine;

	SegUtils::AudioSegmenterEnginePtr
				audioSgm = new SegUtils::AudioSegmenterEngine;

	videoSgm->init(
		cnfgData->segmDrtn );

	audioSgm->init(
		cnfgData->segmDrtn );

	L2::DemuxerFilterPtr	vidSFltr = new L2::StreamClassFilter( VideoStream::getClassType(), 0 );
	L2::DemuxerFilterPtr	audSFltr = new L2::StreamClassFilter( AudioStream::getClassType(), 0 );

	rmxrEngn	= new L2::RemuxerEngine;

	rmxrEngn->getEnmuxerCore()->setBlockingPutSession( false );
	rmxrEngn->getEnmuxerCore()->setBlockingPutFrame( false );
	rmxrEngn->addProcessor( vidSFltr, new AVC::Any2NALUFramer );
	rmxrEngn->addProcessor( vidSFltr, new AVC::NALU2AUFramer );
//	rmxrEngn->addProcessor( vidSFltr, new FrameRateComputer );
	rmxrEngn->addProcessor( vidSFltr, videoSgm );
	rmxrEngn->addProcessor( audSFltr, audioSgm );
//	rmxrEngn->addProcessor( audSFltr, new L2::ProxyPushFifo );

	syncEngn	= new SegUtils::MuxedSynchronizer;

	syncEngn->init(
		cnfgData->segmDrtn );

	m2tsEngn	= new M2S::TSEncoderEngine;

	wrtrEngn	= new SegUtils::SegmentWriter;

	wrtrEngn->init(
		new SegUtils::SimplePathComputer(
			cnfgData->outptDir,
			cnfgData->segmPrfx,
			".ts" ),
		cnfgData->segmDrtn,
		1,
		cnfgData->sncWrite );

	mngrEngn	= new PlayListWriter;

	mngrEngn->init(
		cnfgData->outptDir,
		cnfgData->m3u8Name,
		cnfgData->pvrLngth,
		cnfgData->segmDrtn,
		cnfgData->httpPrfx,
		cnfgData->fakeClse,
		cnfgData->sncWrite );

	rmxrEngn->attachPeerEngine( 0, syncEngn );
	syncEngn->attachPeerEngine( 0, m2tsEngn );
	m2tsEngn->attachPeerEngine( 0, wrtrEngn );
	wrtrEngn->attachPeerEngine( 0, mngrEngn );

}

// ============================================================================

void HLS::ServerEngine::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSessionCallback(): --->" );
//	msgDbg( "putSessionCallback(): " + pSession->toBuffer() );

	rmxrEngn->putSessionCallback( pSession );

//	msgDbg( "putSessionCallback(): <---" );

}

void HLS::ServerEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	rmxrEngn->endSessionCallback();

//	msgDbg( "endSessionCallback(): <---" );

}

void HLS::ServerEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrameCallback(): --->" );
//	msgDbg( "putFrameCallback(): " + pFrame->toBuffer() );
//	msgDbg( "putFrameCallback(): PTS/DTS: "
//		+ pFrame->getPTS().dateTimeToBuffer( false, true )
//		+ " / "
//		+ pFrame->getDTS().dateTimeToBuffer( false, true ) );

	rmxrEngn->putFrameCallback( pFrame );

//	msgDbg( "putFrameCallback(): <---" );

}

void HLS::ServerEngine::flushSessionCallback() {

	rmxrEngn->flushSessionCallback();

}

// ============================================================================

void HLS::ServerEngine::getDiskStats(
		Uint64 &	pNmbrWrtn,
		Uint64 &	pNmbrDltd ) {

	if ( ! cnfgData ) {
		throw NotInitialized();
	}

	Uint64	segmWrtn = wrtrEngn->getDiskStats();

	Uint64	plstWrtn;
	Uint64	plstDltd;

	mngrEngn->getDiskStats( plstWrtn, plstDltd );

	pNmbrWrtn = segmWrtn + plstWrtn;
	pNmbrDltd = plstDltd;

}

// ============================================================================

