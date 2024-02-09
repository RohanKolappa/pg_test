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
//	BFC.SPtrHub.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <iostream>
#include <typeinfo>

#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtrHub.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

SPtrHub::SPtrHub(
		SObject	*o)
	: t(o)
	, ca(0)
	, cm(0) {

//	currCount++ ; if ( currCount > peakCount ) peakCount = currCount;

}

SPtrHub::~SPtrHub() {

//	currCount--;
//
//	if ( ! currCount ) {
//		cerr << "peak # SPtrHub == " << peakCount << endl;
//	}

}

// ============================================================================

void SPtrHub::incPtr(
	const	Bool		mst) {

	ca++;

	if ( mst ) {
		cm++;
	}

}

Bool SPtrHub::decPtr(
	const	Bool		mst) {

	SObject	*k = 0;

	{

		if ( mst ) {
			cm--;
			if ( ! cm && t ) {
				k = t;
				t = 0;
			}
		}

	}

	if ( k ) {
		delete k;
		// During the destruction process of 'k', our
		// 'decObj()' method will be called from '~SObject()'.
		// Additionally, new SPtr to 'k' could be temporarily
		// created and/or destroyed.
	}

	Bool	res;

	{

		ca--;

		res = ( ! t && ! ca );

	}

	return res;

}

// ============================================================================

void SPtrHub::incMst() {

	cm++;

}

void SPtrHub::decMst() {

	cm--;

}

// ============================================================================

Bool SPtrHub::decObj() {

	t = 0;

	return ( ! ca );

}

// ============================================================================

const char *SPtrHub::typeName() const {

	return (t ? typeid(*t).name() : 0);

}

// ============================================================================

//Uint32 SPtrHub::currCount = 0;
//Uint32 SPtrHub::peakCount = 0;

// ============================================================================

