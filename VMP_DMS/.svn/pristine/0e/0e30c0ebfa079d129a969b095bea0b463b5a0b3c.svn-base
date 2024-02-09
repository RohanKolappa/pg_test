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
//	VMP.RTP.TCurveSegment.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.RTP.TClockLine.h"
#include "VMP.RTP.TClockPoint.h"
#include "VMP.RTP.TCurveSegment.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TCurveSegment )

// ============================================================================

RTP::TCurveSegment::TCurveSegment(
		TClockPointCPtr	pClkPoint ) {

	clkPoint += pClkPoint;
	clckLine = new TClockLine( pClkPoint );

}

RTP::TCurveSegment::TCurveSegment(
		TClockPointCPtr	pFrsPoint,
		TClockPointCPtr	pLstPoint ) {

	clkPoint += pFrsPoint;
	clkPoint += pLstPoint;

	clckLine = new TClockLine( pFrsPoint, pLstPoint );

}

// ============================================================================

void RTP::TCurveSegment::dropFirstPoint() {

	if ( clkPoint.getSize() <= 1 ) {
		throw InternalError();
	}

	clkPoint.pop( 0 );

	if ( clkPoint.getSize() >= 2 ) {
		clkPoint[ 0 ] = clckLine->getPointForTarget( clkPoint[ 0 ]->getTargetTime() );
	}

}

// ============================================================================

void RTP::TCurveSegment::mergeWithNext(
		TCurveSegmentCPtr
				pNextSegm ) {

	if ( getLastPoint() != pNextSegm->getFirstPoint() ) {
		throw InternalError();
	}

	if ( isSingleton() ) {
		clkPoint = pNextSegm->clkPoint;
		clckLine = pNextSegm->clckLine;
	}
	else {
		clkPoint.popLast();
		clkPoint += pNextSegm->clkPoint;
		clckLine = new TClockLine( clkPoint.getFirst(), clkPoint.getLast() );
	}

}

void RTP::TCurveSegment::mergeWithPoint(
		TClockPointCPtr	pLstPoint ) {

	if ( isSingleton()
	  || clkPoint.getLast()->getSourceTime() == pLstPoint->getSourceTime() ) {
		clkPoint.setLast( pLstPoint );
	}
	else {
		clkPoint += pLstPoint;
	}

	clckLine = new TClockLine( clkPoint.getFirst(), clkPoint.getLast() );

}

// ============================================================================

Buffer RTP::TCurveSegment::toBuffer() const {

	return "clckLine: " + clckLine->toBuffer()
		+ ", frsPoint: " + clkPoint.getFirst()->toBuffer()
		+ ", lstPoint: " + clkPoint.getLast()->toBuffer();

}

// ============================================================================

