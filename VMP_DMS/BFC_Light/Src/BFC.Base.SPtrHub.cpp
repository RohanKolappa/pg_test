// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.SPtrHub.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_LINUX )
#include <cxxabi.h>
#include <stdlib.h>
#endif
#include <typeinfo>

#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtrHub.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

#if defined( PLATFORM_LINUX )

SPtrHub::~SPtrHub() {

	if ( dmngle ) {
		::free( dmngle );
	}

}

#endif

// ============================================================================

void SPtrHub::incPtr(
	const	Bool		mst ) {

	Bool protect = thrSfe;

	if ( protect ) {
		fastCS.enter();
	}

	nbrAll++;

	if ( mst ) {
		nbrMst++;
	}

	if ( protect ) {
		fastCS.leave();
	}

}

Bool SPtrHub::decPtr(
	const	Bool		mst ) {

	SObject	* toKill = 0;

	Bool protect = thrSfe;

	if ( protect ) {
		fastCS.enter();
	}

	if ( mst ) {
		nbrMst--;
		if ( ! nbrMst && ptrObj ) {
			toKill = ptrObj;
			ptrObj = 0;
		}
	}

	if ( protect ) {
		fastCS.leave();
	}

	if ( toKill ) {
		delete toKill;
		// During the destruction process of 'toKill', our
		// 'decObj()' method will be called from '~SObject()'.
		// Additionally, this process could indirectly try to
		// create new SPtr to 'toKill'. Those will get attached to
		// this hub, and only see a NULL pointer! Clever, no ?!
	}

	if ( protect ) {
		fastCS.enter();
	}

	nbrAll--;

	Bool res = ( ! ptrObj && ! nbrAll );

	if ( protect ) {
		fastCS.leave();
	}

	return res;

}

// ============================================================================

void SPtrHub::incMst() {

	Bool protect = thrSfe;

	if ( protect ) {
		fastCS.enter();
	}

	nbrMst++;

	if ( protect ) {
		fastCS.leave();
	}

}

void SPtrHub::decMst() {

	Bool protect = thrSfe;

	if ( protect ) {
		fastCS.enter();
	}

	nbrMst--;

	if ( protect ) {
		fastCS.leave();
	}

}

// ============================================================================

Bool SPtrHub::decObj() {

	Bool protect = thrSfe;

	if ( protect ) {
		fastCS.enter();
	}

	ptrObj = 0;

	Bool res = ( ! nbrAll );

	if ( protect ) {
		fastCS.leave();
	}

	return res;

}

// ============================================================================

const char * SPtrHub::typeName() const {

#if defined( PLATFORM_LINUX )

	if ( ! dmngle && ptrObj ) {
		int status;
		dmngle = abi::__cxa_demangle( typeid( *ptrObj ).name(), 0, 0, &status );
	}

	return dmngle;

#else

	return ( ptrObj ? typeid( * ptrObj ).name() : 0 );

#endif

}

// ============================================================================

Bool SPtrHub::setThreadSafe(
	const	Bool		sfe ) {

	Bool protect = thrSfe;

	if ( protect ) {
		fastCS.enter();
	}

	if ( nbrAll <= 1 ) {
		thrSfe = sfe;
	}

	if ( protect ) {
		fastCS.leave();
	}

	return thrSfe;

}

// ============================================================================

