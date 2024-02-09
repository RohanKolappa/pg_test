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
//	VMP.L2.Pumper.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.Pumper.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, Pumper )

// ============================================================================

L2::Pumper::Pumper() :

	nbrLoops	( 0 ),
	nbrFrmes	( 0 ) {

	setObjectName( "VMP.L2.Pumper" );

}

L2::Pumper::~Pumper() {

	safeStopAndJoin();

}

// ============================================================================

void L2::Pumper::run() {

//	msgDbg( "run(): --->" );

	niceExit = false;

	try {
		for ( Uint32 i = 0 ; ! nbrLoops || i < nbrLoops ; i++ ) {
			if ( shouldStop() ) {
				break;
			}
			if ( niceExit ) {
				break;
			}
			doSession();
		}
	}
	catch ( ThreadStopped & ) {
//		msgDbg( "run(): stopped!" );
	}
	catch ( Exception & e ) {
		msgExc( e, "run(): failed!" );
	}

//	msgDbg( "run(): <---" );

}

void L2::Pumper::cleanup(
	const	Time::Delta &	pGrceTime ) {

//	msgDbg( "cleanup(): --->" );

	niceExit = true;

	( Time::now() + pGrceTime ).waitUntilReached();

	if ( getStatus() == Running ) {
//		msgWrn( "cleanup(): still running... killing thread!" );
		try {
			stop();
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't kill inner thread!" );
		}
	}

//	msgDbg( "cleanup(): <---" );

}

// ============================================================================

void L2::Pumper::doSession() {

//	msgDbg( "doSession(): --->" );

	SessionPtr	session;

	session = producer->getSessionCallback();

	consumer->putSessionCallback( session );

//	msgDbg( "doSession(): in session!" );

	Bool	iEndNeeded = true;
	Bool	oEndNeeded = true;

	Uint64	offset = 0;

	for ( Uint64 i = 0 ; ! nbrFrmes || i < nbrFrmes ; i++ ) {

		if ( shouldStop() ) {
			break;
		}

		if ( niceExit ) {
			break;
		}

		FramePtr frame;

		try {
			frame = producer->getFrameCallback( offset );
		}
		catch ( L2::EndOfStream & ) {
//			msgWrn( "doSession(): L EOS" );
			break;
		}
		catch ( L2::BrokenSession & ) {
//			msgWrn( "doSession(): L BS" );
			iEndNeeded = false;
			break;
		}

		if ( frame->getOffset() != Frame::NoOffset
		  && frame->getOffset() != offset ) {
			msgWrn( "doSession(): frame offset discontinuity!" );
			offset = frame->getOffset();
		}

		offset += frame->getLength();

		try {
			consumer->putFrameCallback( frame );
		}
		catch ( L2::EndOfStream & ) {
//			msgWrn( "doSession(): R EOS" );
			break;
		}
		catch ( L2::BrokenSession & ) {
//			msgWrn( "doSession(): R BS" );
			oEndNeeded = false;
			break;
		}

	}
	
	if ( iEndNeeded ) {
		producer->endSessionCallback();
	}

	if ( oEndNeeded ) {
		consumer->endSessionCallback();
	}

//	msgDbg( "doSession(): <---" );

}

// ============================================================================

