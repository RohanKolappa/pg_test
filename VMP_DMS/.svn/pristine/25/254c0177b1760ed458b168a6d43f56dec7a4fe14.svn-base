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
//	VMP.RTP.TProgramCSVWriterEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.File.h"

#include "VMP.MuxedFrame.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TProgramCSVWriterEngine.h"
#include "VMP.RTP.TTSConverterEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TProgramCSVWriterEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TProgramCSVWriterEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f2ce6296-0760-46b5-abbc-46bb9d36edc9" ),
		"VMP.RTP.TProgramCSVWriterEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP TProgram CSV Writer",
			"Devices",
			"RTP TProgram CSV Writer" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TProgramCSVWriterEngine::TProgramCSVWriterEngine() :

	L2::FinalPushEngine( getClassType() ) {

}

RTP::TProgramCSVWriterEngine::TProgramCSVWriterEngine(
	const	Buffer &	pFileName ) :

	L2::FinalPushEngine	( getClassType() ),

	fileName		( pFileName ) {

}

// ============================================================================

void RTP::TProgramCSVWriterEngine::init(
	const	Buffer &	pFileName ) {

	fileName = pFileName;

}

// ============================================================================

void RTP::TProgramCSVWriterEngine::putSessionCallback(
		SessionPtr	/*pOutpSess*/ ) {

	outpFile = new IO::File( fileName, IO::File::New );

	outpFile->open();

	currLine = 0;

	setInSession( true );

}

void RTP::TProgramCSVWriterEngine::endSessionCallback() {

	setInSession( false );

	outpFile->flush();
	outpFile->close();

	outpFile.kill();

}

void RTP::TProgramCSVWriterEngine::putFrameCallback(
		FramePtr	pOutpFrme ) {

	MuxedFramePtr	iMuxFrme = pOutpFrme;

	Uint32		strmIndx = iMuxFrme->getSubIndex();
	TFramePtr	rtpTFrme = iMuxFrme->getSubFrame();

	if ( strmIndx >= entrList.getSize() ) {
		entrList.resize( strmIndx + 1 );
	}

	Entry & e = entrList[ strmIndx ];

	Time::Clock	currRead = rtpTFrme->readTime;
	Uint16		currSqNo = rtpTFrme->SN;
	Uint64		currStmp = rtpTFrme->TS;
	Uint32		currSSRC = rtpTFrme->SSRC;

	Buffer	flgs;

	if ( ! e.initDone ) {
		e.initDone = true;
	}
	else {
		// Sequence number...
		if ( currSqNo != e.prevSqNo ) {
			flgs += "N";
		}
		// Timestamp...
		currStmp |= ( e.prevStmp & RTP::TTSConverterEngine::Mask_63_32 );
		if ( currStmp + RTP::TTSConverterEngine::Mod_2_31 < e.prevStmp ) {
			currStmp += RTP::TTSConverterEngine::Mod_2_32;
		}
		else if ( currStmp > e.prevStmp + RTP::TTSConverterEngine::Mod_2_31 ) {
			currStmp -= RTP::TTSConverterEngine::Mod_2_32;
		}
		if ( currStmp < e.prevStmp ) {
			flgs += "T";
		}
		// SSRC...
		if ( currSSRC != e.prevSSRC ) {
			flgs += "S";
		}
		// Read time...
		if ( currRead - e.prevRead >= Time::Delta( 0, 500000 ) ) {
			flgs += "D";
		}
		if ( currStmp != e.prevStmp && currRead - e.prevRead <= Time::Delta( 0, 10000 ) ) {
			flgs += "B";
		}
	}

	Buffer	line;

	line	= Buffer( currLine )
		+ "," + Buffer( strmIndx )
		+ "," + currRead.dateTimeToBuffer( false, true )
		+ "," + Buffer( ( Uint64 )currRead )
		+ "," + Buffer( currSqNo )
		+ "," + Buffer( currStmp )
		+ "," + Buffer( currSSRC )
		+ "," + flgs
		+ "\n";

	outpFile->putBytes( line );

	currLine++;
	currSqNo++;

	e.prevRead = currRead;
	e.prevSqNo = currSqNo;
	e.prevStmp = currStmp;
	e.prevSSRC = currSSRC;

}

// ============================================================================

static L2::EngineRegisterer< RTP::TProgramCSVWriterEngine > engineRegisterer;

// ============================================================================

