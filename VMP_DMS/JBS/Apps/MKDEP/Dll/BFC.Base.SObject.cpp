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
//	BFC.SObject.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtrHub.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

SObject::SObject() {

	sPtrHub = new SPtrHub( this );

//	currCount++ ; if ( currCount > peakCount ) peakCount = currCount;

}

SObject::~SObject() {

	if ( sPtrHub->decObj() ) {
		delete sPtrHub;
	}

//	currCount--;
//
//	if ( ! currCount ) {
//		cerr << "peak # SObject == " << peakCount << endl;
//	}

}

// ============================================================================

Uint32 SObject::getRefCount() const {

	return sPtrHub->getRefCount();

}

Uint32 SObject::getMstCount() const {

	return sPtrHub->getMstCount();

}

// ============================================================================

SPtrHub *SObject::getHub() {

	return sPtrHub;

}

// ============================================================================

//Uint32 SObject::currCount = 0;
//Uint32 SObject::peakCount = 0;

// ============================================================================


