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
//	VMP.HLS.FragmentInfo.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.FragmentInfo.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, FragmentInfo )

// ============================================================================

HLS::FragmentInfo::FragmentInfo(
	const	Buffer &	pFragmUrl,
	const	Double		pFragDrtn,
	const	Uint64		pFragSqNo,
	const	Uint64		pRngeStrt,
	const	Uint64		pRngeSize ) :

	fragmUrl	( pFragmUrl ),
	m3u8Drtn	( pFragDrtn ),
	fragDrtn	( pFragDrtn ),
	fragSqNo	( pFragSqNo ),
	rngeStrt	( pRngeStrt ),
	rngeSize	( pRngeSize ) {

}

// ============================================================================

Bool HLS::FragmentInfo::sameAs(
		FragmentInfoPtr	pFragInfo ) const {

	return ( fragmUrl == pFragInfo->fragmUrl
	      && m3u8Drtn == pFragInfo->m3u8Drtn
	      && fragSqNo == pFragInfo->fragSqNo );

}

// ============================================================================

Buffer HLS::FragmentInfo::toString() const {

	return "SeqNo: " + Buffer( fragSqNo ) + ", URL: \"" + fragmUrl
		+ "\", m3u8Drtn: " + Buffer( m3u8Drtn ) + ", fragDrtn: "
		+ Buffer( fragDrtn );

}

// ============================================================================

