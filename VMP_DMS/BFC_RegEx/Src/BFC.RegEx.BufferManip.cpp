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
//	BFC.RegEx.BufferManip.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Math.Utils.h"

#include "BFC.RegEx.BufferManip.h"

// ============================================================================

using namespace BFC;
using namespace BFC::RegEx;

// ============================================================================

void BufferManip::doSort(
		Buffer &	str ) {

	if ( str.getLength() <= 1 ) {
		return;
	}

	Uchar *	posMin = str.getVarPtr();
	Uchar *	posMax = str.getVarPtr() + str.getLength();
	Uchar *	posCur = posMin;
	Uchar *	posNxt = posCur + 1;
	Uchar	tmp1;
	Uchar	tmp2;

	for (;;) {
		if ( *posCur <= *posNxt ) {
			posCur++;
			posNxt++;
		}
		else {
			// swap elements!
			tmp1 = *posCur;
			tmp2 = *posNxt;
			*posNxt = tmp1;
			*posCur = tmp2;
			if ( posCur == posMin ) {
				posCur++;
				posNxt++;
			}
			else {
				posCur--;
				posNxt--;
			}
		}
		if ( posNxt == posMax ) {
			return;
		}
	}

}

Buffer BufferManip::getSorted(
	const	Buffer &	str ) {

	Buffer s = str;

	doSort( s );

	return s;

}

// ============================================================================

Buffer BufferManip::getUnion(
	const	Buffer &	__s1,
	const	Buffer &	__s2 ) {

	Buffer s1 = getSorted( __s1 );
	Buffer s2 = getSorted( __s2 );

	if ( s1.isEmpty() ) {
		return s2;
	}

	if ( s2.isEmpty() ) {
		return s1;
	}

	Uchar *	posCur1 = s1.getVarPtr();
	Uchar *	posMax1 = posCur1 + s1.getLength();
	Uchar *	posCur2 = s2.getVarPtr();
	Uchar *	posMax2 = posCur2 + s2.getLength();

	Buffer	res;

	res.supposedMaxSize( s1.getLength() + s2.getLength() );

// [2007-07-13 KAE,GAG: "It f... doesn't work!"]

//	Uchar *	posRes = res.getVarPtr();

	for (;;) {
		if ( posCur1 == posMax1 ) {
			if ( posCur2 == posMax2 ) {
				break;
			}
//			*posRes++ = *posCur2++;
			res += *posCur2++;
		}
		else if ( posCur2 == posMax2 ) {
			if ( posCur1 == posMax1 ) {
				break;
			}
//			*posRes++ = *posCur1++;
			res += *posCur1++;
		}
		else if ( *posCur1 < *posCur2 ) {
//			*posRes++ = *posCur1++;
			res += *posCur1++;
		}
		else if ( *posCur1 > *posCur2 ) {
//			*posRes++ = *posCur2++;
			res += *posCur2++;
		}
		else {
//			*posRes++ = *posCur1++;
			res += *posCur1++;
			posCur2++;
		}
	}

	return res;

}

Buffer BufferManip::getIntersection(
	const	Buffer &	__s1,
	const	Buffer &	__s2 ) {

	Buffer	res;

	if ( __s1.isEmpty() || __s2.isEmpty() ) {
		return res;
	}

	Buffer	s1 = getSorted( __s1 );
	Buffer	s2 = getSorted( __s2 );

	Uchar *	posCur1 = s1.getVarPtr();
	Uchar *	posMax1 = posCur1 + s1.getLength();
	Uchar *	posCur2 = s2.getVarPtr();
	Uchar *	posMax2 = posCur2 + s2.getLength();

	res.supposedMaxSize( Math::getMin( s1.getLength(), s2.getLength() ) );

// [2007-07-13 KAE,GAG: "It f... doesn't work!"]

//	Uchar *	posRes = res.getVarPtr();

	for (;;) {
		if ( posCur1 == posMax1 || posCur2 == posMax2 ) {
			break;
		}
		else if ( *posCur1 < *posCur2 ) {
			posCur1++;
		}
		else if ( *posCur1 > *posCur2 ) {
			posCur2++;
		}
		else {
//			*posRes++ = *posCur1++;
			res += *posCur1++;
			posCur2++;
		}
	}

	return res;

}

// ============================================================================

