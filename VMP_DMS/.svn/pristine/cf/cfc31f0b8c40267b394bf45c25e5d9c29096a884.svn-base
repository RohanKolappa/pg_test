// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Base.SPtrHub.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <typeinfo>

#include "TBFC.Base.SObject.h"
#include "TBFC.Base.SPtrHub.h"
#include "TBFC.Base.FastCSLocker.h"

// ============================================================================

using namespace std;
using namespace TBFC;

// ============================================================================

SPtrHub::SPtrHub(
		SObject		* __ptrObj )
	: ptrObj( __ptrObj )
	, nbrAll( 0 )
	, nbrMst( 0 ) {



}

SPtrHub::~SPtrHub() {



}

// ============================================================================

void SPtrHub::incPtr(
	const	Bool		mst ) {

	FastCSLocker locker( fastCS );

	nbrAll++;

	if ( mst ) {
		nbrMst++;
	}

}

Bool SPtrHub::decPtr(
	const	Bool		mst ) {

	SObject	* toKill = 0;

	{

		FastCSLocker locker( fastCS );

		if ( mst ) {
			nbrMst--;
			if ( ! nbrMst && ptrObj ) {
				toKill = ptrObj;
				ptrObj = 0;
			}
		}

	}

	if ( toKill ) {

		delete toKill;

		// During the destruction process of 'toKill', our
		// 'decObj()' method will be called from '~SObject()'.
		// Additionally, this process could indirectly try to
		// create new SPtr to 'toKill'. Those will get attached to
		// this hub, and only see a NULL pointer! Clever, no ?!

	}

	Bool	res;

	{

		FastCSLocker locker( fastCS );

		nbrAll--;

		res = ( ! ptrObj && ! nbrAll );

	}

	return res;

}

// ============================================================================

void SPtrHub::incMst() {

	FastCSLocker locker( fastCS );

	nbrMst++;

}

void SPtrHub::decMst() {

	FastCSLocker locker( fastCS );

	nbrMst--;

}

// ============================================================================

Bool SPtrHub::decObj() {

	FastCSLocker locker( fastCS );

	ptrObj = 0;

	return ( ! nbrAll );

}

// ============================================================================

const char *SPtrHub::typeName() const {

	return ( ptrObj ? typeid( * ptrObj ).name() : 0 );

}

// ============================================================================

