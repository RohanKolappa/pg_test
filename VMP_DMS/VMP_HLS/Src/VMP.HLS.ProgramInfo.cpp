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
//	VMP.HLS.ProgramInfo.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.LevelInfo.h"
#include "VMP.HLS.ProgramInfo.h"
#include "VMP.HLS.RootManifest.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, ProgramInfo )

// ============================================================================

HLS::ProgramInfo::ProgramInfo() {

}

// ============================================================================

void HLS::ProgramInfo::mergeManifest(
		RootManifestPtr	pRootMfst ) {

	mnfstUrl = pRootMfst->getUrl();
	levelLst = pRootMfst->getLevels();

}

HLS::LevelInfoPtr HLS::ProgramInfo::getBestLevel() const {

	LevelInfoPtr r ;

	for ( Uint32 i = 0 ; i < levelLst.getSize() ; i++ ) {
		if ( ! r || levelLst[ i ]->getBandwidth() > r->getBandwidth() ) {
			r = levelLst[ i ];
		}
	}

	return r;

}

// ============================================================================

