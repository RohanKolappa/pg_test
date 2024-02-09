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
//	VMP.HLS.PlayListWriter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <math.h>

#include "BFC.Base.FastCSLocker.h"

#include "BFC.VFS.FSNode.h"

#include "VMP.HLS.PlayListFastFile.h"
#include "VMP.HLS.PlayListSlowFile.h"
#include "VMP.HLS.PlayListWriter.h"
#include "VMP.HLS.ServerConfig.h"

#include "VMP.SegUtils.SegmentFrame.h"
#include "VMP.SegUtils.SegmentSession.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, PlayListWriter )

// ============================================================================

const DL::TypeCPtr & HLS::PlayListWriter::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "3c5814df-87f9-40da-9cab-b579a3899046" ),
		"VMP.HLS.PlayListWriter",
		L2::FinalPushEngine::getClassType()
	);

	return i;

}

// ============================================================================

HLS::PlayListWriter::PlayListWriter() :

	L2::FinalPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, SegUtils::SegmentSession::getClassType() );

}

HLS::PlayListWriter::~PlayListWriter() {

}

// ============================================================================

void HLS::PlayListWriter::init(
	const	FSPath &	pOutptDir,
	const	Buffer &	pM3u8Name,
	const	Uint32		pPvrLngth,
	const	Frac64 &	pSegmDrtn,
	const	Buffer &	pHttpPrfx,
	const	Bool		pFakeClse,
	const	Bool		pSyncWrte ) {

// FIXME: If no random access is possible and fakeClose is used, then force
// FIXME: slow mode too.

	if ( ! pPvrLngth ) {
		playList = new PlayListFastFile(
				pOutptDir,
				pM3u8Name,
				pSegmDrtn,
				pHttpPrfx,
				pFakeClse,
				pSyncWrte );
	}
	else {
		playList = new PlayListSlowFile(
				pOutptDir,
				pM3u8Name,
//				( Uint32 )::ceil( ( Double )pPvrLngth / pSegmDrtn.toDouble() ),
				pPvrLngth,
				pSegmDrtn,
				pHttpPrfx,
				pFakeClse,
				pSyncWrte );
	}

	playList->create();

}

// ============================================================================

void HLS::PlayListWriter::getDiskStats(
		Uint64 &	pNmbrWrtn,
		Uint64 &	pNmbrDltd ) {

	if ( playList ) {
		playList->getDiskStats( pNmbrWrtn, pNmbrDltd );
	}
	else {
		pNmbrWrtn = 0;
		pNmbrDltd = 0;
	}

}

// ============================================================================

void HLS::PlayListWriter::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSessionCB(): " + pSession->toBuffer() );

	segmSess = pSession;

}

void HLS::PlayListWriter::endSessionCallback() {

	playList->close();
	playList.kill();

}

void HLS::PlayListWriter::putFrameCallback(
		FramePtr	pFrame ) {

	playList->update( pFrame );

}

// ============================================================================

