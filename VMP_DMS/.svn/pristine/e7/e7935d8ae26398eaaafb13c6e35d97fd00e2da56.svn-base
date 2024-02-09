// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.FancyOutput.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.BufferIO.h"
#include "BFC.Debug.Debugger.h"

#include "BFC.Debug.ColoredBuffer.h"
#include "BFC.Debug.ConsoleColor.h"

#include "JBS.Context.h"
#include "JBS.FancyOutput.h"

// ============================================================================

using namespace std;
using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( FancyOutput )

// ============================================================================

FancyOutput::FancyOutput() {



}

FancyOutput::~FancyOutput() {



}

// ============================================================================

void FancyOutput::msg(
	const	Buffer&			msg1,
	const	Buffer&			msg2,
	const	ConsoleColor	col1Fg,
	const	ConsoleColor	col1Bg,
	const	ConsoleColor	col2Fg,
	const	ConsoleColor	col2Bg,
	const	Message::Type		prio ) const {

	if ( prio < context->verbosity ) {
		return;
	}

	if ( context->noColor ) {
		cout	<< "===> "
			<< msg1
			<< " "
			<< msg2
			<< endl;
	}
	else {
		StderrDebugger::instance()->consumeMessage(
			ColoredBuffer( "===> " )
			+ ColoredBuffer( msg1, col1Fg, col1Bg )
			+ ColoredBuffer( " " )
			+ ColoredBuffer( msg2, col2Fg, col2Bg )
		);
	}

}

// ============================================================================

