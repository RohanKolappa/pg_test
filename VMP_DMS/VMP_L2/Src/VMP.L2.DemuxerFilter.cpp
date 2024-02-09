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
//	VMP.L2.DemuxerFilter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.DemuxerFilter.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::L2::DemuxerFilter
// ============================================================================

BFC_PTR_IMPL_NS( L2, DemuxerFilter )

// ============================================================================
// VMP::L2::StreamSkipFilter
// ============================================================================

Uint32 L2::StreamSkipFilter::filter(
		MuxedSessionCPtr	/* pMSession */ ) {

	return NoSessionId;

}

// ============================================================================
// VMP::L2::StreamTitleFilter
// ============================================================================

Uint32 L2::StreamTitleFilter::filter(
		MuxedSessionCPtr	pMSession ) {

	MuxedStreamCPtr mStream = pMSession->getMuxedStream();

	for ( Uint32 i = 0 ; i < mStream->getNbrComponents() ; i++ ) {
		if ( mStream->getTitle() == streamTitle ) {
			return i;
		}
	}

	return NoSessionId;

}

// ============================================================================
// VMP::L2::StreamClassFilter
// ============================================================================

Uint32 L2::StreamClassFilter::filter(
		MuxedSessionCPtr	pMSession ) {

	MuxedStreamCPtr mStream = pMSession->getMuxedStream();

	Uint32 found = 0;

	for ( Uint32 i = 0 ; i < mStream->getNbrComponents() ; i++ ) {
		if ( mStream->getComponent( i )->inherits( streamClass )
		  && found++ == streamIndex ) {
			return i;
		}
	}

	return NoSessionId;

}

// ============================================================================
// VMP::L2::SessionClassFilter
// ============================================================================

Uint32 L2::SessionClassFilter::filter(
		MuxedSessionCPtr	pMSession ) {

	Uint32 found = 0;

	for ( Uint32 i = 0 ; i < pMSession->getNbrComponents() ; i++ ) {
		if ( pMSession->getComponent( i )->inherits( sessionClass )
		  && found++ == sessionIndex ) {
			return i;
		}
	}

	return NoSessionId;

}

// ============================================================================
// VMP::L2::StreamIdFilter
// ============================================================================

Uint32 L2::StreamIdFilter::filter(
		MuxedSessionCPtr	pMSession ) {

	MuxedStreamCPtr mStream = pMSession->getMuxedStream();

	for ( Uint32 i = 0 ; i < mStream->getNbrComponents() ; i++ ) {
		if ( mStream->getComponent( i )->getStreamId() == streamId ) {
			return i;
		}
	}

	return NoSessionId;

}

// ============================================================================
// VMP::L2::StreamIdFilter
// ============================================================================

Uint32 L2::SubIndexFilter::filter(
		MuxedSessionCPtr	/*pMSession*/ ) {

	return subIndex;

}

// ============================================================================

