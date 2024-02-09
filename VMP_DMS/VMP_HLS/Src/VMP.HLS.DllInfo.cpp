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
//	VMP.HLS.DllInfo.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.DllInfo.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, DllInfo )

// ============================================================================

HLS::DllInfo::DllInfo() :

	compVers	( JBS_TARGET_VERSION_MAJ, JBS_TARGET_VERSION_MED, JBS_TARGET_VERSION_MIN ),
	compTime	( JBS_TARGET_BUILD_TIME, 0 ) {

}

// ============================================================================

HLS::DllInfoCPtr HLS::DllInfo::instance() {

	static DllInfoCPtr i = new DllInfo;

	return i;

}

// ============================================================================

Buffer HLS::DllInfo::toBuffer() const {

	return compVers.toBuffer() + " (" + compTime.dateTimeToBuffer() + ")";

}

// ============================================================================

