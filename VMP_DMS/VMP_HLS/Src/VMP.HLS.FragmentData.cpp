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
//	VMP.HLS.FragmentData.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.FragmentData.h"
#include "VMP.HLS.FragmentInfo.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, FragmentData )

// ============================================================================

HLS::FragmentData::FragmentData(
		FragmentInfoPtr	pFragInfo,
//		FLVVectorPtr	pAudTgLst,
//		FLVVectorPtr	pVidTgLst,
	const	Bool		pFragDisc ) {

	fragInfo = pFragInfo;
//	audTgLst = pAudTgLst;
//	vidTgLst = pVidTgLst;
	fragDisc = pFragDisc;

	ensureCloseTo( TimeStampCPtr() );

//	dataTags = new FLVVector();
//
//	dataTags.pushVector( vidTgLst );
//	dataTags.pushVector( audTgLst );

}

// ============================================================================

void HLS::FragmentData::ensureCloseTo(
		TimeStampCPtr	/*pTimeStmp*/ ) {

//	audTgLst.ensureCloseTo( pTimeStmp );
//	vidTgLst.ensureCloseTo( pTimeStmp );
//
//	if ( audTgLst.getLength() > 0 ) {
//		strtTime = audTgLst.getMinPTS();
//		stopTime = audTgLst.getMaxPTS();
//	}
//	else if ( vidTgLst.getLength() > 0 ) {
//		strtTime = vidTgLst.getMinPTS();
//		stopTime = vidTgLst.getMaxPTS();
//	}

//	Logger.debug( "FragmentData", "ensureCloseTo",
//		"min: " + strtTime->toString()
//		+ ", max: " + stopTime->toString() );

}

Double HLS::FragmentData::getRelativeStartTime() const {

	return fragInfo->getRelativeStartTime();

}

// ============================================================================

