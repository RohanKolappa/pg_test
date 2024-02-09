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
//	TBFC.Time.Chrono.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "TBFC.Time.Chrono.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Time::Chrono::Chrono()
	: startFlag( false ) {



}

Time::Chrono::~Chrono() {



}

// ============================================================================

void Time::Chrono::start() {

	startTime = Time::now();
	startFlag = true;

}

void Time::Chrono::stop() {

	stopTime = Time::now();
	startFlag = false;

}

Bool Time::Chrono::isStarted() const {

	return ( startFlag );

}

// ============================================================================

Time::Clock Time::Chrono::getStartTime() const {

	return startTime;

}

Time::Clock Time::Chrono::getStopTime() const {

	return stopTime;

}

Time::Delta Time::Chrono::getElapsed() const {

	return ( startFlag
		? Time::now() - startTime
		: stopTime - startTime );

}

// ============================================================================

