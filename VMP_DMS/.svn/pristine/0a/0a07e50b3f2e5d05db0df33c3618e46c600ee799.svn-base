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
//	VMP.L2.Pump.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.Pump.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, Pump )

// ============================================================================

L2::Pump::Pump() {

	setObjectName( "VMP.L2.Pump" );

}

// ============================================================================

void L2::Pump::run(
	const	Uint32		pNbrLoops,
		PullEnginePtr	pProducer,
		PushEnginePtr	pConsumer ) {

	for ( Uint32 i = 0 ; ! pNbrLoops || i < pNbrLoops ; i++ ) {
		run( pProducer, pConsumer );
	}

}

void L2::Pump::run(
		PullEnginePtr	pProducer,
		PushEnginePtr	pConsumer ) {

//	msgDbg( "run(): -->" );

	SessionPtr	session;

	session = pProducer->getSessionCallback();

	pConsumer->putSessionCallback( session );

//	msgDbg( "run(): in session!" );

	for ( Uint32 i = 0 ; i < eng.getSize() ; i++ ) {
		eng[ i ]->setDynVarValue( tag[ i ], val[ i ] );
	}

	Bool iEndNeeded = true;
	Bool oEndNeeded = true;

	Uint64	offset = 0;

	for (;;) {

		FramePtr frame;

		try {
			frame = pProducer->getFrameCallback( offset );
		}
		catch ( L2::EndOfStream & ) {
			break;
		}
		catch ( L2::BrokenSession & ) {
			iEndNeeded = false;
			break;
		}

		offset += frame->getLength();

		try {
			pConsumer->putFrameCallback( frame );
		}
		catch ( L2::EndOfStream & ) {
			break;
		}
		catch ( L2::BrokenSession & ) {
			oEndNeeded = false;
			break;
		}

	}
	
	if ( iEndNeeded ) {
		pProducer->endSessionCallback();
	}

	if ( oEndNeeded ) {
		pConsumer->endSessionCallback();
	}

}

// ============================================================================

