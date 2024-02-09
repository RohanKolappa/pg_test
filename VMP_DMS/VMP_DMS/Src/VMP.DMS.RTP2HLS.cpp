// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "VMP::DMS".
//
// "VMP::DMS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "VMP::DMS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::DMS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	VMP.DMS.RTP2HLS.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Frac64.h"

#include "BFC.Debug.DObject.h"

#include "BFC.IO.File.h"

#include "BFC.Net.SockAddr.h"

#include "BFC.Net.Log.Debugger.h"

#include "BFC.VFS.FSPath.h"

#include "VMP.DMS.RTP2HLS.h"
#include "VMP.DMS.RTPMultiInjector.h"
#include "VMP.DMS.StreamCutter.h"

#include "VMP.HLS.ServerConfig.h"
#include "VMP.HLS.ServerEngine.h"

#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.SDecoder.h"
#include "VMP.RTP.SProgram.h"
#include "VMP.RTP.TProgramClientEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

class DMS::RTP2HLS::Config {

public :

	Buffer			iSDPFile;	///< Reference SDP file.
	Bool			multSSRC;	///< Multiple SSRC's ?
	HLS::ServerConfigPtr	srvrCnfg;	///< HLS Server Config.

};

class DMS::RTP2HLS::Context {

public :

	Context(
	) :
		dltaInit( false )
	{
	}

	RTPMultiInjectorPtr	rtpInjct;	///< Injector for raw RTP audio/video packet.
	RTP::TProgramClientEnginePtr
				rtpPClnt;
	StreamCutterPtr		strmCttr;
	HLS::ServerEnginePtr	srvrEngn;	///< Main HLS server component.

	unsigned long long	timeDlta;	///< Value to subtract from all timestamps (in msecs).
	Bool			dltaInit;	///< Is "timeDlta" initialized ?

};

// ============================================================================

void DMS::RTP2HLS::setLogLevel(
	const	unsigned int	pLogLevel ) {

	DObject::setGlobalLogLevel( ( DObject::LogLevel )pLogLevel );

}

// ============================================================================

DMS::RTP2HLS::RTP2HLS() {

	cfg = 0;
	ctx = 0;

//	try {
//		Net::Log::DebuggerPtr	dbg = new Net::Log::Debugger;
//		dbg->init( new Net::SockAddr( "10.80.1.1", 28270 ) );
//		DObject::setDebugger( dbg );
//	}
//	catch ( Exception & ) {
//	}

	msgDbg( "ctor()" );

}

DMS::RTP2HLS::~RTP2HLS() {

	msgDbg( "dtor()" );

	if ( cfg ) {
		delete cfg;
	}

	if ( ctx ) {
		delete ctx;
	}

}

// ============================================================================

void DMS::RTP2HLS::init(
	const	char *		pISDPFile,
	const	bool		pMultSSRC,
	const	char *		pOutptDir,
	const	char *		pSegmPrfx,
	const	double		pSegmDrtn,
	const	char *		pM3u8Name,
	const	char *		pHttpPrfx,
	const	unsigned int	pPvrLngth,
	const	bool		pSncWrite,
	const	bool		pFakeClse ) {

	msgDbg( "init()" );

	if ( cfg ) {
		throw AlreadyInitialized();
	}

	cfg = new Config;

	cfg->iSDPFile = Buffer( pISDPFile, true );	// Input could be volatile --> force copy.
	cfg->multSSRC = pMultSSRC;

	cfg->srvrCnfg = new HLS::ServerConfig;

	cfg->srvrCnfg->outptDir = Buffer( pOutptDir, true );	// Input could be volatile --> force copy.
	cfg->srvrCnfg->segmPrfx = Buffer( pSegmPrfx, true );	// Input could be volatile --> force copy.
	cfg->srvrCnfg->segmDrtn = pSegmDrtn;
	cfg->srvrCnfg->m3u8Name = Buffer( pM3u8Name, true );	// Input could be volatile --> force copy.
	cfg->srvrCnfg->httpPrfx = Buffer( pHttpPrfx, true );	// Input could be volatile --> force copy.
	cfg->srvrCnfg->pvrLngth = pPvrLngth;
	cfg->srvrCnfg->sncWrite = pSncWrite;
	cfg->srvrCnfg->fakeClse = pFakeClse;

}

// ============================================================================

void DMS::RTP2HLS::play() {

	msgDbg( "play()" );

	try {

		if ( ! cfg ) {
			throw NotInitialized();
		}

		if ( ctx ) {
			throw InternalError();
		}

		ctx = new Context;

		// Prepare the RTP input...

		// ... RTP injector

		ctx->rtpInjct	= new RTPMultiInjector;

		// ... RTP decoder

		ctx->rtpPClnt	= new RTP::TProgramClientEngine;

		ctx->rtpPClnt->setAllowMultipleSSRC( cfg->multSSRC );

		ctx->rtpInjct->attachPeerEngine( 0, ctx->rtpPClnt );

		// ... MuxedStream cutter

		ctx->strmCttr	= new StreamCutter;

		ctx->rtpPClnt->attachPeerEngine( 0, ctx->strmCttr );

		// ... HLS server

		ctx->srvrEngn	= new HLS::ServerEngine;

		ctx->srvrEngn->init( cfg->srvrCnfg );

		ctx->strmCttr->attachPeerEngine( 0, ctx->srvrEngn );

		// Start session...

		RTP::SDecoderPtr	sDecoder = new RTP::SDecoder;
		RTP::SProgramPtr	sProgram = sDecoder->decodeSDP( IO::File::dump( cfg->iSDPFile ) );

		ctx->rtpInjct->init( sProgram );

	}
	catch ( Exception & e ) {
		msgExc( e, "Can't play!" );
		throw;
	}

}

void DMS::RTP2HLS::pause(
	const	unsigned long long
				pTimeStmp ) {

	msgDbg( "pause(): timestamp: " + Buffer( pTimeStmp ) );

	try {

		if ( ! ctx ) {
			throw InternalError();
		}

		ctx->strmCttr->pause( Time::Clock( ( Uint64 )( adjustClock( pTimeStmp ) * 1000 ) ) );

	}
	catch ( Exception & e ) {
		msgExc( e, "Can't pause!" );
		throw;
	}

}

void DMS::RTP2HLS::resume(
	const	unsigned long long
				pTimeStmp ) {

	msgDbg( "resume(): timestamp: " + Buffer( pTimeStmp ) );

	try {

		if ( ! ctx ) {
			throw InternalError();
		}

		ctx->strmCttr->resume( Time::Clock( ( Uint64 )( adjustClock( pTimeStmp ) * 1000 ) ) );

	}
	catch ( Exception & e ) {
		msgExc( e, "Can't resume!" );
		throw;
	}

}

void DMS::RTP2HLS::stop() {

	msgDbg( "stop()" );

	try {

		if ( ! ctx ) {
			throw InternalError();
		}

		if ( ctx->rtpInjct ) {
			ctx->rtpInjct->cleanup();
		}

		delete ctx;

		ctx = 0;

	}
	catch ( Exception & e ) {
		msgExc( e, "Can't stop!" );
		throw;
	}

}

// ============================================================================

void DMS::RTP2HLS::putVideoFrame(
	const	char *		pPacktPtr,
	const	unsigned long	pPacktLen,
	const	unsigned long long
				pWallClck ) {

//	msgDbg( "putVideoFrame(): len == " + Buffer( ( Uint32 )pPacktLen ) );

	try {

		if ( ! ctx ) {
			throw InternalError();
		}

		RTP::NFramePtr nPacket = new RTP::NFrame(
			Net::SockAddrPtr(),
			Net::SockAddrPtr(),
			Buffer( ( const Uchar * )pPacktPtr, ( const Uint32 )pPacktLen, true ),
			Time::Clock( ( Uint64 )adjustClock( pWallClck ) * 1000 ) );

//		msgDbg( "putVideoFrame(): " + nPacket->toBuffer() );

		ctx->rtpInjct->putVideoPacket( nPacket );

	}
	catch ( Exception & e ) {
		msgExc( e, "Can't putVideoFrame!" );
		throw;
	}

}

void DMS::RTP2HLS::putAudioFrame(
	const	char *		pPacktPtr,
	const	unsigned long	pPacktLen,
	const	unsigned long long
				pWallClck ) {

//	msgDbg( "putAudioFrame(): len == " + Buffer( ( Uint32 )pPacktLen ) );

	try {

		if ( ! ctx ) {
			throw InternalError();
		}

		RTP::NFramePtr nPacket = new RTP::NFrame(
			Net::SockAddrPtr(),
			Net::SockAddrPtr(),
			Buffer( ( const Uchar * )pPacktPtr, ( const Uint32 )pPacktLen, true ),
			Time::Clock( ( Uint64 )adjustClock( pWallClck ) * 1000 ) );

//		msgDbg( "putAudioFrame(): " + nPacket->toBuffer() );

		ctx->rtpInjct->putAudioPacket( nPacket );

	}
	catch ( Exception & e ) {
		msgExc( e, "Can't putAudioFrame!" );
		throw;
	}

}

// ============================================================================

unsigned long DMS::RTP2HLS::getVideoFifoSize() const {

	return 0;

}

unsigned long DMS::RTP2HLS::getAudioFifoSize() const {

	return 0;

}

void DMS::RTP2HLS::getDiskStats(
		unsigned long long &
				pNmbrWrtn,
		unsigned long long &
				pNmbrDltd ) {

	Uint64	w;
	Uint64	d;

	if ( ctx ) {
		ctx->srvrEngn->getDiskStats( w, d );
	}
	else {
		w = 0;
		d = 0;
	}

	pNmbrWrtn = w;
	pNmbrDltd = d;

}

// ============================================================================

unsigned long long DMS::RTP2HLS::adjustClock(
	const	unsigned long long
				pWallClck ) {

	if ( ! ctx->dltaInit ) {
		ctx->timeDlta = pWallClck;
		ctx->dltaInit = true;
	}

	return ( pWallClck < ctx->timeDlta ? 0 : pWallClck - ctx->timeDlta );

}

// ============================================================================

