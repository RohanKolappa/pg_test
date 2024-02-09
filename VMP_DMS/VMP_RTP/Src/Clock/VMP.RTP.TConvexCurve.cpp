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
//	VMP.RTP.TConvexCurve.cpp
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
#include "VMP.RTP.TConvexCurve.h"
#include "VMP.RTP.TCurveSegment.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TConvexCurve )

// ============================================================================

RTP::TConvexCurve::TConvexCurve() {

	setObjectName( "VMP.RTP.TConvexCurve" );

}

// ============================================================================

RTP::TClockLineCPtr RTP::TConvexCurve::addPoint(
		TClockPointCPtr	pNewPoint ) {

	TClockPointCPtr	newPoint = pNewPoint;

	if ( segmList.isEmpty() ) {
//		msgDbg( "addPoint(): adding first: " + newPoint->toBuffer() );
		TCurveSegmentPtr frstSegm = new TCurveSegment( newPoint );
		segmList += frstSegm;
		bestLine = frstSegm->getLine();
		return bestLine;
	}

//	msgDbg( "addPoint(): " + newPoint->toBuffer() );

	// We've got at least 1 point. Do some sanity checks...

	Time::Clock	currSrce = newPoint->getSourceTime();
	Time::Clock	currTrgt = newPoint->getTargetTime();

	TClockPointCPtr	lstPoint = segmList.getLast()->getLastPoint();

	Time::Clock	lastSrce = lstPoint->getSourceTime();
	Time::Clock	lastTrgt = lstPoint->getTargetTime();

	if ( currSrce < lastSrce ) {
		throw InternalError( "Source time rolling back ( "
			+ currSrce.dateTimeToBuffer( false, true ) + " < "
			+ lastSrce.dateTimeToBuffer( false, true ) + " )!" );
	}

	if ( currTrgt < lastTrgt ) {
		throw InternalError( "Target time rolling back ( "
			+ currTrgt.dateTimeToBuffer( false, true ) + " < "
			+ lastTrgt.dateTimeToBuffer( false, true ) + " )!" );
	}

	if ( currTrgt == lastTrgt ) {
		// Redundant piece of information...
//		msgDbg( "addPoint(): dropping redundant..." );
		return bestLine;
	}

	if ( currSrce == lastSrce ) {
		if ( segmList.getSize() == 1
		  && segmList.getFirst()->isSingleton() ) {
			return bestLine;
		}
		segmList.getLast()->mergeWithPoint( newPoint );
	}
	else {
		segmList += new TCurveSegment( lstPoint, newPoint );
	}

	mergeSegments();
	clipCurve();

	bestLine = segmList.getFirst()->getLine();

	return bestLine;

}

// ============================================================================

Buffer RTP::TConvexCurve::toBuffer() const {

	Buffer res;

	for ( Uint32 i = 0 ; i < segmList.getSize() ; i++ ) {
		if ( i ) {
			res += "\n";
		}
		res += "Segment [ " + Buffer( i + 1 ) + " / "
			+ Buffer( segmList.getSize() ) + " ]: "
			+ segmList[ i ]->toBuffer();
	}

	return res;

}

// ============================================================================

void RTP::TConvexCurve::mergeSegments() {

	while ( segmList.getSize() >= 2 ) {

		TCurveSegmentPtr	frstSegm = segmList.getLast( 1 );
		TCurveSegmentPtr	lastSegm = segmList.getLast( 0 );

		if ( ! frstSegm->isSingleton()
		  && isConvex( frstSegm->getFirstPoint(), frstSegm->getLastPoint(), lastSegm->getLastPoint() ) ) {
			break;
		}

		frstSegm->mergeWithNext( lastSegm );

		segmList.popLast();

	}

}

void RTP::TConvexCurve::clipCurve() {

	Time::Clock	lastSrce = segmList.getLast()->getLastPoint()->getSourceTime();

	while ( segmList.getFirst()->getFirstPoint()->getSourceTime() + Time::OneSecond * 15 <= lastSrce ) {

		segmList.getFirst()->dropFirstPoint();

		if ( segmList.getFirst()->isSingleton() ) {
			if ( segmList.getSize() == 1 ) {
				break;
			}
			segmList.popFirst();
		}

	}

}

// ============================================================================

Bool RTP::TConvexCurve::isConvex(
		TClockPointCPtr	pFrsPoint,
		TClockPointCPtr	pMidPoint,
		TClockPointCPtr	pLstPoint ) const {

	//                     * - pLstPoint
	//                 *
	//             *
	//         *
	//     * - pMidPoint
	//    *
	//   *
	//  *
	// * - pFrsPoint

	TClockLineCPtr	interLne = new TClockLine( pFrsPoint, pLstPoint );

	Time::Clock	iPntTime = pMidPoint->getTargetTime();
	Time::Clock	iLneTime = interLne->getTargetForSource( pMidPoint->getSourceTime() );

//	msgDbg( "frs: " + pFrsPoint->toBuffer() );
//	msgDbg( "mid: " + pMidPoint->toBuffer() );
//	msgDbg( "lst: " + pLstPoint->toBuffer() );
//
//	msgDbg( iPntTime.dateTimeToBuffer( false ,true ) + " <=> "
//		+ iLneTime.dateTimeToBuffer( false ,true ) );

	return ( iPntTime > iLneTime );

}

// ============================================================================

