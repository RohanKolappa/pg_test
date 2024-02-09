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
//	VMP.HLS.ManifestLoader.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "BFC.HTTP.Answer.h"
#include "BFC.HTTP.Client.h"

#include "BFC.Time.Clock.h"

#include "VMP.HLS.LeafManifest.h"
#include "VMP.HLS.LevelInfo.h"
#include "VMP.HLS.ManifestDecoder.h"
#include "VMP.HLS.ManifestLoader.h"
#include "VMP.HLS.ProgramInfo.h"
#include "VMP.HLS.RootManifest.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, ManifestLoader )

// ============================================================================

HLS::ManifestLoader::ManifestLoader() :

	frceReld	( false ) {

	setObjectName( "VMP.HLS.ManifestLoader" );

}

HLS::ManifestLoader::~ManifestLoader() {

	safeStopAndJoin();

}

// ============================================================================

Double HLS::ManifestLoader::getStreamDuration() const {

	return ( curLevel ? curLevel->getTotalDuration() : 0.0 );

}

// ============================================================================

void HLS::ManifestLoader::load(
	const	Buffer &	pMnfstUrl ) {

	msgDbg( "load(): url: " + pMnfstUrl );

	if ( isRunning() ) {
		throw InternalError( "Already initialized!" );
	}

	currLink = pMnfstUrl;

	start();

}

void HLS::ManifestLoader::kill() {

	msgDbg( "kill()" );

	if ( ! isRunning() ) {
		return;
	}

	safeStopAndJoin();

	curLevel.kill();
	progInfo.kill();

}

// ============================================================================

void HLS::ManifestLoader::run() {

	msgDbg( "run(): --->" );

	try {
		while ( ! shouldStop() ) {
			doLoop();
			if ( strmType == "VOD" ) {
				break;
			}
			if ( curLevel->getUrl() == currLink ) {
				System::usleep( ( Uint64 )( 1000000.0 * curLevel->getAverageDuration() ) );
			}
			else {
				currLink = curLevel->getUrl();
			}
		}
	}
	catch ( ThreadStopped & ) {
	}
	catch ( Exception & e ) {
		msgExc( e, "run(): FAILED!" );
	}

	msgDbg( "run(): <---" );

}

// ============================================================================

void HLS::ManifestLoader::doLoop() {

	Buffer		fullLink = currLink;

	if ( frceReld ) {
		fullLink += Buffer( fullLink.findFwd( '?' ) >= fullLink.getLength() ? "?" : "&" )
			+ "time=" + Buffer( ( Uint64 )Time::now() );
	}

	Net::URL	mnfstUrl = Net::URL( fullLink );
	HTTP::ClientPtr	httpClnt = new HTTP::Client;
	HTTP::AnswerPtr	httpAnsw = httpClnt->get( mnfstUrl );
	Buffer		mfstData = httpAnsw->getPayload();

	ManifestDecoderPtr
			mDecoder = new ManifestDecoder;
	ManifestPtr	manifest = mDecoder->decode( currLink, mfstData );

	if ( manifest.isA< RootManifest >() ) {
		mergeRootManifest( manifest );
	}
	else if ( manifest.isA< LeafManifest >() ) {
		mergeLeafManifest( manifest );
	}
	else {
		throw InternalError( "Unknown manifest type!" );
	}

}

// ============================================================================

void HLS::ManifestLoader::mergeRootManifest(
		ManifestPtr	pManifest ) {

	RootManifestPtr	rootMfst = pManifest;

	if ( progInfo ) {
		throw InternalError( "Loaded other root manifest!" );
	}

	progInfo = new ProgramInfo;

	progInfo->mergeManifest( rootMfst );

	curLevel = progInfo->getBestLevel();

}

void HLS::ManifestLoader::mergeLeafManifest(
		ManifestPtr	pManifest ) {

	LeafManifestPtr	leafMfst = pManifest;

	if ( ! curLevel ) {
		curLevel = new LevelInfo;
	}

	curLevel->mergeManifest( leafMfst );

	if ( leafMfst->hasEndlist() ) {
		strmType = "VOD";
	}
	else {
		strmType = "LIVE";
	}

}

// ============================================================================

