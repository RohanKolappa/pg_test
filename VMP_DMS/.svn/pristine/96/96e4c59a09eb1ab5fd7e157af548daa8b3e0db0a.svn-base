// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Time.Chrono.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Time.Chrono.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Time::Chrono::Chrono(
	const	Bool		pAutoStart ) :

	startFlag( false ),
	avrgCount( 0 ) {

	if ( pAutoStart ) {
		start();
	}

}

// ============================================================================

void Time::Chrono::start() {

	startTime = Time::now();
	startFlag = true;

}

void Time::Chrono::stop() {

	stopTime = Time::now();
	startFlag = false;

	avrgTotal += ( stopTime - startTime );
	avrgCount++;

}

// ============================================================================

Time::Delta Time::Chrono::getElapsed() const {

	return ( startFlag
		? Time::now() - startTime
		: stopTime - startTime );

}

Time::Delta Time::Chrono::getAverage() const {

	return ( avrgCount > 0
		? avrgTotal / ( Int64 )avrgCount
		: Delta( 0 ) );

}

void Time::Chrono::resetAverage() {

	avrgTotal = 0;
	avrgCount = 0;

}

// ============================================================================

