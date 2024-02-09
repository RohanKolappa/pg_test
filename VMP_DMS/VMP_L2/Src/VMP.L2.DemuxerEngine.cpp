// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.DemuxerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.AudioStream.h"
#include "VMP.DataStream.h"
#include "VMP.VideoStream.h"

#include "VMP.L2.DemuxerEngine.h"
#include "VMP.L2.EngineRegisterer.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::L2::DemuxerEngine
// ============================================================================

BFC_PTR_IMPL_NS( L2, DemuxerEngine )

// ============================================================================

const DL::TypeCPtr & L2::DemuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "fb1b9faa-ef9c-4817-9842-745826b7590b" ),
		"VMP.L2.DemuxerEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::DemuxerEngine::DemuxerEngine(
	const	DL::TypeCPtr &	strmClass,
	const	Uint32			strmIndex ) :

	L2::ProxyPullEngine( getClassType() ) {

	privInit(
		new DemuxerCore,
		new StreamClassFilter( strmClass, strmIndex )
	);

}

L2::DemuxerEngine::DemuxerEngine(
		L2::DemuxerEnginePtr	dmxEngine,
	const	DL::TypeCPtr &	strmClass,
	const	Uint32			strmIndex ) :

	L2::ProxyPullEngine( getClassType() ) {

	privInit(
		dmxEngine->demuxCore,
		new StreamClassFilter( strmClass, strmIndex )
	);

}

L2::DemuxerEngine::DemuxerEngine(
		L2::DemuxerCorePtr	demuxCore,
	const	DL::TypeCPtr &	strmClass,
	const	Uint32			strmIndex ) :

	L2::ProxyPullEngine( getClassType() ) {

	privInit(
		demuxCore,
		new StreamClassFilter( strmClass, strmIndex )
	);

}

L2::DemuxerEngine::DemuxerEngine(
		L2::DemuxerCorePtr	demuxCore,
		L2::DemuxerFilterPtr	strFilter ) :

	L2::ProxyPullEngine( getClassType() ) {

	privInit(
		demuxCore,
		strFilter
	);

}

L2::DemuxerEngine::~DemuxerEngine() {

//	privExit();

}

// ============================================================================

void L2::DemuxerEngine::privInit(
		L2::DemuxerCorePtr	pDemuxCore,
		L2::DemuxerFilterPtr	pStrFilter ) {

	setNbrInputs( 1 );
	setInputType( 0, MuxedSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, Session::getClassType() );

	demuxCore = pDemuxCore;

	slotIndex = demuxCore->getSlot( pStrFilter );

//	msgDbg( "slotIndex: " + Buffer( slotIndex ) );

}

void L2::DemuxerEngine::privExit() {

	try {
		if ( isInSession() ) {
			endSessionCallback();
		}
	}
	catch ( Exception & ) {
	}

	demuxCore->delSlot( slotIndex );

}

// ============================================================================

void L2::DemuxerEngine::attachPeerEngine(
	const	Uint32			slot,
		L2::PullEnginePtr	dmxEngine ) {

	if ( slot ) {
		throw InternalError( "Invalid slot!" );
	}

	demuxCore->attachPeerEngine( dmxEngine );

}

void L2::DemuxerEngine::detachPeerEngine(
	const	Uint32			slot ) {

	if ( slot ) {
		throw InternalError( "Invalid slot!" );
	}

	demuxCore->detachPeerEngine();

}

// ============================================================================

SessionPtr L2::DemuxerEngine::getSessionCallback() {

	nextIndex = 0;

	SessionPtr res;

	while ( ! ( res = demuxCore->getOutputSession( slotIndex ) ) ) {
		demuxCore->endOutputSession( slotIndex );
	}

	setInSession( true );

	return res;

}

void L2::DemuxerEngine::endSessionCallback() {

	setInSession( false );

	demuxCore->endOutputSession( slotIndex );

}

FramePtr L2::DemuxerEngine::getFrameCallback(
	const	Uint64		pIndex ) {

//	msgDbg( "getFrameCallback(): --->" );

//	msgDbg( "getFrameCallback(): index == "
//		+ ( pIndex == Frame::NoOffset
//			? Buffer( "ANY" )
//			: Buffer( pIndex ) ) );

	if ( pIndex != Frame::NoOffset
	  && pIndex != nextIndex ) {
//		msgWrn( "getFrameCallback(): seeking --> ( "
//			+ Buffer( slotIndex ) + ", " + Buffer( pIndex )
//			+ " )!" );
		demuxCore->seekOutputFrame( slotIndex, pIndex );
		nextIndex = pIndex;
	}

	FramePtr res;

	try {
		res = demuxCore->getOutputFrame( slotIndex );
	}
	catch ( DemuxerCore::BrokenSession & ) {
		setInSession( false );
		throw BrokenSession();
	}
	catch ( DemuxerCore::EndOfStream & ) {
		throw EndOfStream();
	}

//	msgDbg( "getFrameCallback(): requested: "
//		+ ( pIndex == Frame::NoOffset
//			? Buffer( "ANY" )
//			: Buffer( pIndex ) )
//		+ ", received: "
//		+ ( res->getOffset() == Frame::NoOffset
//			? Buffer( "n/a" )
//			: Buffer( res->getOffset() ) ) );

	if ( res->getOffset() == Frame::NoOffset ) {
		res->setOffset( nextIndex );
	}
	else if ( res->getOffset() != nextIndex ) {
//		msgWrn( "getFrameCallback(): unexpected discontinuity!" );
		nextIndex = res->getOffset();
	}

	nextIndex += res->getLength();

//	msgDbg( "getFrameCallback(): <---" );

	return res;

}

FramePtr L2::DemuxerEngine::getFrameCallback() {

	return getFrameCallback( Frame::NoOffset );

}

// ============================================================================
// VMP::L2::VideoDemuxerEngine
// ============================================================================

const DL::TypeCPtr & L2::VideoDemuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b35bfb7a-2ef5-400a-837b-1607dffe80f1" ),
		"VMP.L2.VideoDemuxerEngine",
		DemuxerEngine::getClassType(),
		new UI::UObjectDescr(
			"Simple Video Demuxer",
			"Utils,Muxed",
			"Simple Video Demuxer (1 out)"
		)
	);

	return i;

}

// ============================================================================

L2::VideoDemuxerEngine::VideoDemuxerEngine(
	const	Uint32		strmIndex ) :

	DemuxerEngine(
		VideoStream::getClassType(),
		strmIndex ) {

}

// ============================================================================

static L2::EngineRegisterer< L2::VideoDemuxerEngine > videoDemuxerEngineRegisterer;

// ============================================================================
// VMP::L2::AudioDemuxerEngine
// ============================================================================

const DL::TypeCPtr & L2::AudioDemuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "6b4adfa0-c86b-4c66-8ebd-d9d05b5f95aa" ),
		"VMP.L2.AudioDemuxerEngine",
		DemuxerEngine::getClassType(),
		new UI::UObjectDescr(
			"Simple Audio Demuxer",
			"Utils,Muxed",
			"Simple Audio Demuxer (1 out)"
		)
	);

	return i;

}

// ============================================================================

L2::AudioDemuxerEngine::AudioDemuxerEngine(
	const	Uint32		strmIndex ) :

	DemuxerEngine(
		AudioStream::getClassType(),
		strmIndex ) {

}

// ============================================================================

static L2::EngineRegisterer< L2::AudioDemuxerEngine > audioDemuxerEngineRegisterer;

// ============================================================================
// VMP::L2::DataDemuxerEngine
// ============================================================================

const DL::TypeCPtr & L2::DataDemuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1e768dd1-1cfe-4864-b064-e43bb0fdf299" ),
		"VMP.L2.DataDemuxerEngine",
		DemuxerEngine::getClassType(),
		new UI::UObjectDescr(
			"Simple Data Demuxer",
			"Utils,Muxed",
			"Simple Data Demuxer (1 out)"
		)
	);

	return i;

}

// ============================================================================

L2::DataDemuxerEngine::DataDemuxerEngine(
	const	Uint32		strmIndex ) :

	DemuxerEngine(
		DataStream::getClassType(),
		strmIndex ) {

}

// ============================================================================

static L2::EngineRegisterer< L2::DataDemuxerEngine > dataDemuxerEngineRegisterer;

// ============================================================================

