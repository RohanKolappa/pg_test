// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Debug.Debugger.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.FastCSLocker.h"

#include "TBFC.Debug.Console.h"
#include "TBFC.Debug.Debugger.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL( Debugger )

// ============================================================================

Debugger::~Debugger() {



}

// ============================================================================

StderrDebugger::StderrDebugger() {

	console = Console::instance();

}

StderrDebugger::~StderrDebugger() {



}

// ============================================================================

void StderrDebugger::consumeMessage(
	const	ColoredBuffer	& msg) {

	FastCSLocker locker( critSect );

	if ( msg.getLength()
	  && msg.getBuffer()[ msg.getLength() - 1 ] == '\r' ) {
		( * console ) << msg;
	}
	else {
		( * console ) << msg << nl;
	}

}

// ============================================================================

DebuggerPtr StderrDebugger::instance() {

	static DebuggerPtr i = new StderrDebugger;

	return i;

}

// ============================================================================





