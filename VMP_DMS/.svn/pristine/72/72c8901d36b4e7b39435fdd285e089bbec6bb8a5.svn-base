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
//	BFC.Debug.Debugger.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Light_Debugger_H_
#define _BFC_Light_Debugger_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Debug.DebuggerPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"

#include "BFC.Debug.ColoredBuffer.h"
#include "BFC.Debug.Console.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Abstract base class of all debuggers.
///
/// \ingroup BFC_Light

class BFC_Light_DLL Debugger : virtual public SObject {

public :

	virtual void consumeMessage(
		const	ColoredBuffer &	message
	) = 0;

};

// ============================================================================

/// \brief Debugger sending its input to the console.
///
/// \ingroup BFC_Light

class BFC_Light_DLL StderrDebugger : public Debugger {

protected :

	StderrDebugger(
	);

public :

	static DebuggerPtr instance(
	);

	virtual void consumeMessage(
		const	ColoredBuffer &	message
	);

protected :

private :

	FastCS		critSect;
	ConsolePtr	console;

	// others...

	StderrDebugger(
		const	StderrDebugger &
	);

	StderrDebugger & operator = (
		const	StderrDebugger &
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_Debugger_H_

// ============================================================================

