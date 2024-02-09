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
//	VMP.RTP.TClockLine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

//#include "BFC.Debug.DObject.h"

#include "VMP.RTP.TClockLine.h"
#include "VMP.RTP.TClockPoint.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TClockLine )

// ============================================================================

RTP::TClockLine::TClockLine(
		TClockPointCPtr	pFrsPoint ) :

	frsPoint	( pFrsPoint ),
	lstPoint	( pFrsPoint ) {

	computeSpeed();

}

RTP::TClockLine::TClockLine(
		TClockPointCPtr	pFrsPoint,
		TClockPointCPtr	pLstPoint ) :

	frsPoint	( pFrsPoint ),
	lstPoint	( pLstPoint ) {

	computeSpeed();

}

// ============================================================================

Time::Clock RTP::TClockLine::getTargetForSource(
	const	Time::Clock &	pSrceTime ) const {

	return frsPoint->getTargetTime() + ( pSrceTime - frsPoint->getSourceTime() ) * clkSpeed;

}

Time::Clock RTP::TClockLine::getSourceForTarget(
	const	Time::Clock &	pTrgtTime ) const {

	return frsPoint->getSourceTime() + ( pTrgtTime - frsPoint->getTargetTime() ) / clkSpeed;

}

// ============================================================================

RTP::TClockPointCPtr RTP::TClockLine::getPointForSource(
	const	Time::Clock &	pSrceTime ) const {

	return new TClockPoint( pSrceTime, getTargetForSource( pSrceTime ) );

}

RTP::TClockPointCPtr RTP::TClockLine::getPointForTarget(
	const	Time::Clock &	pTrgtTime ) const {

	return new TClockPoint( getSourceForTarget( pTrgtTime ), pTrgtTime );

}

// ============================================================================

Time::Delta RTP::TClockLine::getSourceDelta() const {

	return ( lstPoint->getSourceTime() - frsPoint->getSourceTime() );

}

Time::Delta RTP::TClockLine::getTargetDelta() const {

	return ( lstPoint->getTargetTime() - frsPoint->getTargetTime() );

}

// ============================================================================

Time::Delta RTP::TClockLine::getSourceDelta(
		TClockPointCPtr	pCurPoint ) const {

	Time::Clock	currSrce = pCurPoint->getSourceTime();
	Time::Clock	theoSrce = getSourceForTarget( pCurPoint->getTargetTime() );

	return ( currSrce - theoSrce );

}

Time::Delta RTP::TClockLine::getTargetDelta(
		TClockPointCPtr	pCurPoint ) const {

	Time::Clock	currTrgt = pCurPoint->getTargetTime();
	Time::Clock	theoTrgt = getTargetForSource( pCurPoint->getSourceTime() );

	return ( currTrgt - theoTrgt );

}

// ============================================================================

void RTP::TClockLine::resetPoints(
	const	Time::Clock &	pSrceTime ) {

	frsPoint = lstPoint = getPointForSource( pSrceTime );

	computeSpeed();

}

void RTP::TClockLine::setLastPoint(
		TClockPointCPtr	pLstPoint ) {

	lstPoint = pLstPoint;

	computeSpeed();

}

void RTP::TClockLine::extendLastPoint(
	const	Time::Clock &	pSrceTime ) {

	lstPoint = getPointForSource( pSrceTime );

	computeSpeed();

}

// ============================================================================

Buffer RTP::TClockLine::toBuffer() const {

	return "( frs: " + frsPoint->toBuffer() + ", lst: "
		+ lstPoint->toBuffer() + ", spd: " + Buffer( clkSpeed ) + " )";

}

// ============================================================================

void RTP::TClockLine::computeSpeed() {

	Time::Delta	totlSrce = getSourceDelta();
	Time::Delta	totlTrgt = getTargetDelta();
	Double		sftSpeed = 1.0;

	if ( ! totlSrce ) {
		if ( totlTrgt ) {
			throw InternalError( "Speed == infinity!" );
		}
		clkSpeed = sftSpeed;
		return;
	}

	Double		hrdSpeed = ( Double )totlTrgt / ( Double )totlSrce;

//	if ( totlSrce >= Time::OneSecond ) {
		clkSpeed = hrdSpeed;
//		return;
//	}
//
//	Double		hardWght = totlSrce / Time::OneSecond;
//	Double		softWght = 1.0 - hardWght;
//
//	clkSpeed = ( sftSpeed * softWght + hrdSpeed * hardWght );

}

// ============================================================================

