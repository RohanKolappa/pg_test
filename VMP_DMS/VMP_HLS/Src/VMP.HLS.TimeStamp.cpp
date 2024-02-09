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
//	VMP.HLS.TimeStamp.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <math.h>

#include "VMP.HLS.TimeStamp.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, TimeStamp )

// ============================================================================

void HLS::TimeStamp::ensureCloseTo(
		TimeStampCPtr	pOther ) {

	if ( ! pOther ) {
		return;
	}

	if ( ! roundMod ) {
		throw InternalError( "Invalid modulo!" );
	}

//	if ( timeStmp == Infinity ) {
//		throw InternalError( "Can't adjust Infinity!!" );
//	}

	Double	distThrs = ( Double )roundMod * 0.90 / ( Double )timeScle;
	Double	trgtStmp = pOther->toDouble();

	while ( toDouble() - trgtStmp >= distThrs ) {
		timeStmp -= roundMod;
	}

	while ( trgtStmp - toDouble() >= distThrs ) {
		timeStmp += roundMod;
	}

}

// ============================================================================

HLS::TimeStampCPtr HLS::TimeStamp::add(
		TimeStampCPtr	p1,
		TimeStampCPtr	p2 ) {

	if ( ! p1 ) {
		return p2;
	}

	if ( ! p2 ) {
		return p1;
	}

	Int64	num = p1->timeStmp * p2->timeScle + p2->timeStmp * p1->timeScle;
	Uint64	den = p1->timeScle * p2->timeScle;
	Uint64	gcd = getGCD( p1->timeScle, p2->timeScle );
	Uint64	mod = p1->roundMod * p2->timeScle;

	return new TimeStamp(
			num / gcd,
			den / gcd,
			mod / gcd );

}

HLS::TimeStampCPtr HLS::TimeStamp::sub(
		TimeStampCPtr	p1,
		TimeStampCPtr	p2 ) {

	if ( ! p1 ) {
		return new TimeStamp( - p2->timeStmp, p2->timeScle, p2->roundMod );
	}

	if ( ! p2 ) {
		return p1;
	}

	Int64	num = p1->timeStmp * p2->timeScle - p2->timeStmp * p1->timeScle;
	Uint64	den = p1->timeScle * p2->timeScle;
	Uint64	gcd = getGCD( p1->timeScle, p2->timeScle );
	Uint64	mod = p1->roundMod * p2->timeScle;

	return new TimeStamp(
			num / gcd,
			den / gcd,
			mod / gcd );

}

// ============================================================================

HLS::TimeStampCPtr HLS::TimeStamp::addNumber(
	const	Double		p2 ) const {

	return new TimeStamp(
			timeStmp + ( Int64 )( p2 * ( Double )timeScle ),
			timeScle,
			roundMod );

}

HLS::TimeStampCPtr HLS::TimeStamp::subNumber(
	const	Double		p2 ) const {

	return new TimeStamp(
			timeStmp - ( Int64 )( p2 * ( Double )timeScle ),
			timeScle,
			roundMod );

}

// ============================================================================

HLS::TimeStampCPtr HLS::TimeStamp::min(
		TimeStampCPtr	p1,
		TimeStampCPtr	p2 ) {

	if ( ! p1 ) {
		return p2;
	}

	if ( ! p2 ) {
		return p1;
	}

	return ( p1->toDouble() <= p2->toDouble() ? p1 : p2 );

}

HLS::TimeStampCPtr HLS::TimeStamp::max(
		TimeStampCPtr	p1,
		TimeStampCPtr	p2 ) {

	if ( ! p1 ) {
		return p2;
	}

	if ( ! p2 ) {
		return p1;
	}

	return ( p1->toDouble() >= p2->toDouble() ? p1 : p2 );

}

// ============================================================================

Int64 HLS::TimeStamp::toMSecs() const {

	return ( Int64 )round( ( Double )timeStmp / ( Double )timeScle * 1000.0 );

}

Double HLS::TimeStamp::toDouble() const {

	return ( Double )timeStmp / ( Double )timeScle;

}

Buffer HLS::TimeStamp::toString() const {

	return Buffer( toDouble() );

}

// ============================================================================

Uint64 HLS::TimeStamp::getGCD(
	const	Uint64		a,
	const	Uint64		b ) {

	Uint64	mdc = a;
	Uint64	aux = b;

	Uint64	rem;

	while ( aux ) {
		rem = mdc % aux;
		mdc = aux;
		aux = rem;
	}

	return mdc;

}

// ============================================================================

