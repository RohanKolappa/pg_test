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
//	TBFC.Debug.Debugger.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _TBFC_Light_Debugger_H_
#define _TBFC_Light_Debugger_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {

TBFC_PTR_DECL( TBFC_Light_DLL, Debugger )

}

// ============================================================================

#include "TBFC.Base.FastCS.h"
#include "TBFC.Base.SObject.h"

#include "TBFC.Debug.ColoredBuffer.h"
#include "TBFC.Debug.Console.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Abstract base class of all debuggers.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL Debugger : virtual public SObject {

public :

	Debugger(
	) {
	}

	virtual ~Debugger(
	);

	virtual void consumeMessage(
		const	ColoredBuffer	& message
	) = 0;

protected :

private :

	// others...

	Debugger(
		const Debugger		&
	);

	Debugger& operator = (
		const Debugger		&
	);

};

// ============================================================================

/// \brief Debugger sending its input to the console.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL StderrDebugger : public Debugger {

protected :

	StderrDebugger(
	);

public :

	virtual ~StderrDebugger(
	);

	static DebuggerPtr instance(
	);

	virtual void consumeMessage(
		const	ColoredBuffer	& message
	);

protected :

private :

	// internal state

	FastCS		critSect;
	ConsolePtr	console;


	// others...

	StderrDebugger(
		const StderrDebugger	&
	);

	StderrDebugger & operator = (
		const StderrDebugger	&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_Debugger_H_

// ============================================================================


