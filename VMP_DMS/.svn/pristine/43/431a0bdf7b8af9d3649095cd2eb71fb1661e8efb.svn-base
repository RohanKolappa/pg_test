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
//	VMP.RTP.SProgram.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.NotFound.h"

#include "VMP.RTP.SBandwidth.h"
#include "VMP.RTP.SConnInfo.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.SOrigin.h"
#include "VMP.RTP.SProgram.h"
#include "VMP.RTP.STimeInfo.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SProgram )

// ============================================================================

RTP::SProgram::SProgram() {

}

// ============================================================================

void RTP::SProgram::addSEStream(
		SEStreamPtr	pEStream ) {

	strmList += pEStream;

	if ( ! pEStream->hasConnectionInfo() ) {
		pEStream->setConnectionInfo( getConnectionInfo() );
	}

}

// ============================================================================

RTP::SEStreamCPtr RTP::SProgram::findEStreamByMetaType(
	const	Buffer &	pMetaType ) const {

	for ( Uint32 i = 0 ; i < getNbrSEStreams() ; i++ ) {
		if ( getSEStream( i )->getMetaType() == pMetaType ) {
			return getSEStream( i );
		}
	}

	throw NotFound();

}

// ============================================================================

Buffer RTP::SProgram::toBuffer() const {

	Buffer	res;

	res += "--------";
	res += "\nSProgram";
	res += "\n--------";

	if ( hasOrigin() ) {
		res += "\nOrigin          : " + getOrigin()->encode();
	}

	if ( hasTitle() ) {
		res += "\nTitle           : " + getTitle();
	}

	if ( hasProgramInfo() ) {
		res += "\nProgram info    : " + getProgramInfo();
	}

	if ( hasDescriptionURI() ) {
		res += "\nDescription     : " + getDescriptionURI();
	}

	if ( hasEmail() ) {
		res += "\nEmail           : " + getEmail();
	}

	if ( hasPhone() ) {
		res += "\nPhone           : " + getPhone();
	}

	if ( hasConnectionInfo() ) {
		res += "\nConnection info : " + getConnectionInfo()->encode();
	}

	if ( hasBandwidth() ) {
		res += "\nBandwidth       : " + getBandwidth()->encode();
	}

	if ( hasTimeInfo() ) {
		res += "\nTime info       : " + getTimeInfo()->encode();
	}

	for ( Uint32 i = 0 ; i < getNbrSEStreams() ; i++ ) {
		res += "\n" + getSEStream( i )->toBuffer();
	}

	return res;

}

// ============================================================================

