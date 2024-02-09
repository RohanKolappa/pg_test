// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::MT".
//
// "BFC::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.SysCondition.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================

#include <windows.h>

#include "BFC.Time.Delta.h"
#include "BFC.Time.Clock.h"

#include "BFC.MT.SysCondition.h"
#include "BFC.MT.SysMutex.h"
#include "BFC.MT.SysMutexData.h"

// ============================================================================

using namespace BFC;

// ============================================================================

struct MT::SysCondition::Data {

	CONDITION_VARIABLE	condVrbl;

};

// ============================================================================

MT::SysCondition::SysCondition(
		SysMutex &	pShrdMutx ) :
		
	shrdMutx	( pShrdMutx ) {

	condData = new Data;

	InitializeConditionVariable( &condData->condVrbl );

}

MT::SysCondition::~SysCondition() {

	delete condData;

}

// ============================================================================

void MT::SysCondition::signal() {

	WakeConditionVariable( &condData->condVrbl );

}

void MT::SysCondition::broadcast() {

	WakeAllConditionVariable( &condData->condVrbl );

}

// ============================================================================

void MT::SysCondition::wait() {

	SleepConditionVariableCS( &condData->condVrbl, &shrdMutx.data->critSect, INFINITE );

}

Bool MT::SysCondition::timedWait(
	const	Time::Delta &	d ) {

	return ( SleepConditionVariableCS( &condData->condVrbl, &shrdMutx.data->critSect, ( DWORD )( ( Int64 )d / 1000 ) ) == TRUE );

}

Bool MT::SysCondition::timedWait(
	const	Time::Clock &	limit ) {

	return timedWait( limit - Time::now() );

}

// ============================================================================

void MT::SysCondition::lock() {

	shrdMutx.lock();

}

void MT::SysCondition::unlock() {

	shrdMutx.unlock();

}

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

