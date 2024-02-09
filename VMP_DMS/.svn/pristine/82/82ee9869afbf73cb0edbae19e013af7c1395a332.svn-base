// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.PES2VMPConverter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PushEngine.h"

#include "VMP.M2S.PES2VMPConverter.h"
#include "VMP.M2S.PESPacket.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PES2VMPConverter )

// ============================================================================

M2S::PES2VMPConverter::PES2VMPConverter(
		BytesSessionPtr	pOutpSess ) :

	outpSess	( pOutpSess ) {

	setObjectName( "VMP.M2S.PES2VMPConverter" );

}

// ============================================================================

void M2S::PES2VMPConverter::setPeerEngine(
		L2::PushEnginePtr
				pConsumer ) {

	consumer = pConsumer;

	consumer->putSessionCallback( outpSess );

}

void M2S::PES2VMPConverter::delPeerEngine() {

	consumer->endSessionCallback();

	consumer.kill();

}

// ============================================================================

void M2S::PES2VMPConverter::putObject(
		SPtr		pPESPacket ) {

	PESPacketPtr	pPacket = pPESPacket;

	BytesFramePtr res = new BytesFrame;

	res->setData( pPacket->data_byte );
	res->setPTS( pPacket->PTS.toClock() );
	res->setDTS( pPacket->DTS.toClock() );

	consumer->putFrameCallback( res );

}

// ============================================================================

