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
//	VMP.SegUtils.SegmentWriter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.IO.File.h"

#include "VMP.BytesSession.h"

#include "VMP.SegUtils.PathComputer.h"
#include "VMP.SegUtils.SegmentFrame.h"
#include "VMP.SegUtils.SegmentSession.h"
#include "VMP.SegUtils.SegmentWriter.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( SegUtils, SegmentWriter )

// ============================================================================

const DL::TypeCPtr & SegUtils::SegmentWriter::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "a100fd77-d709-4735-be28-62ed24497782" ),
		"VMP.SegUtils.SegmentWriter",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"SegmentWriter",
			"Devices",
			"Writes segments to separate file(s)." /*,
			SegUtils::userInterface */
		)
	);

	return i;

}

// ============================================================================

SegUtils::SegmentWriter::SegmentWriter() :

	L2::ProxyPushEngine	( getClassType() ),

	nmbrWrtn		( 0 ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, SegmentSession::getClassType() );

}

SegUtils::SegmentWriter::~SegmentWriter() {

}

// ============================================================================

void SegUtils::SegmentWriter::init(
		PathComputerCPtr
				pPathComp,
	const	Frac64 &	pSegmDrtn,
	const	Uint64		pFrstIndx,
	const	Bool		pSncWrite ) {

	pathComp = pPathComp;
	segmDrtn = pSegmDrtn;
	currIndx = pFrstIndx;
	sncWrite = pSncWrite;
	nmbrWrtn = 0;

}

// ============================================================================

void SegUtils::SegmentWriter::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	if ( ! pathComp ) {
		throw InternalError( "Not initialized" );
	}

	if ( ! pSession->doesUseSegmentStart() ) {
		throw L2::NotSuitable( "Input not generating segmentation!" );
	}

	SegmentSessionPtr	segmSess = new SegmentSession(
							pSession,
							segmDrtn );

	getPeerEngine( 0 )->putSessionCallback( segmSess );

	setInSession( true );

}

void SegUtils::SegmentWriter::endSessionCallback() {

//	msgDbg( "endSession()" );

	setInSession( false );

	if ( outpFile ) {
// FIXME: not correct, because the duration will not account for the last frame!
		closeFile( lastTime );
	}

	getPeerEngine( 0 )->endSessionCallback();

}

void SegUtils::SegmentWriter::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame()" );

	BytesFramePtr	bFrm = pFrame;

//	msgDbg( "putFrame(): " + pFrame->toBuffer() );

	if ( bFrm->isSegmentStart() ) {

		// Close currently open file (if any) ?

		if ( outpFile ) {
			closeFile( bFrm->getDTS() );
		}

		// Open new file if needed...

		if ( ! outpFile ) {
			startFile( bFrm->getDTS() );
		}

	}

	if ( outpFile ) {

		// Write content...

//		msgDbg( "putFrame(): writing frame..." );

		lastTime = bFrm->getDTS();

		Buffer d = bFrm->getData();

		writeFile( d );
//		outpFile->putBytes( d );

		FastCSLocker locker( critSect );

		nmbrWrtn += d.getLength();

	}
	else {

		msgWrn( "putFrame(): dropping frame!" );

	}

}

// ============================================================================

Uint64 SegUtils::SegmentWriter::getDiskStats() {

	FastCSLocker locker( critSect );

	return nmbrWrtn;

}

// ============================================================================

void SegUtils::SegmentWriter::startFile(
	const	Time::Clock &	pStrtTime ) {

	filePath = pathComp->getFilePath( currIndx );
	fileName = filePath.getLastComponent();
	fullName = filePath.toBuffer();
	frstTime = pStrtTime;

//	msgDbg( "startFile(): name: " + fullName + ", time: " + frstTime.dateTimeToBuffer( false, true ) );

	outpFile = new IO::File( fullName, IO::File::New | ( sncWrite ? IO::File::Sync : 0 ) );

	outpFile->open();

	fileCche.kill();

}

void SegUtils::SegmentWriter::closeFile(
	const	Time::Clock &	pStopTime ) {

//	msgDbg( "closeFile(): name: " + fullName + ", stop: " + pStopTime.dateTimeToBuffer( true, true ) );

//	if ( pStopTime < frstTime ) {
//		throw AssertionFailed( "closeFile(): Negative duration! Frst: "
//			+ frstTime.dateTimeToBuffer( false, true ) + ", last: "
//			+ pStopTime.dateTimeToBuffer( false, true ) );
//	}

	if ( sncWrite && fileCche.hasContent() ) {
		outpFile->putBytes( fileCche );
		fileCche.kill();
	}

	outpFile->flush();

	outpFile.kill();

	fileCche.kill();

	SegmentFramePtr	segmFrme = new SegmentFrame(
					currIndx++,
					Frac64( pStopTime - frstTime, 1000000 ),
					fileName );

	getPeerEngine( 0 )->putFrameCallback( segmFrme );

}

void SegUtils::SegmentWriter::writeFile(
	const	Buffer &	pSegmData ) {

	if ( ! sncWrite ) {
		outpFile->putBytes( pSegmData );
		return;
	}

	if ( fileCche.getLength() + pSegmData.getLength() > CcheMxSz ) {
		outpFile->putBytes( fileCche );
		fileCche.kill();
	}

	if ( fileCche.isEmpty() ) {
		if ( pSegmData.getLength() >= CcheMxSz ) {
			outpFile->putBytes( pSegmData );
			return;
		}
		fileCche.supposedMaxSize( CcheMxSz );
	}

	fileCche += pSegmData;

	if ( fileCche.getLength() >= CcheMxSz ) {
		outpFile->putBytes( fileCche );
		fileCche.kill();
	}

}

// ============================================================================

