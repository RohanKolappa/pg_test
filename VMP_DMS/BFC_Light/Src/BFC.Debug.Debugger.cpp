// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Debug".
// 
// "BFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Debug.Debugger.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Debug.Console.h"
#include "BFC.Debug.Debugger.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL( Debugger )

// ============================================================================

StderrDebugger::StderrDebugger() {

	console = Console::instance();

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

static DebuggerPtr dummy = StderrDebugger::instance();

// ============================================================================

