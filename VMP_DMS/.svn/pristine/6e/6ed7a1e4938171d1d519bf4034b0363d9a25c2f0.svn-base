// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.IO.Debugger.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_Debugger_H_
#define _BFC_IO_Debugger_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.IO.DebuggerPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.Debug.Debugger.h"

#include "BFC.IO.DevicePtr.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_IO

class BFC_IO_DLL Debugger : public BFC::Debugger {

public :

	/// \brief Creates a new Debugger.

	Debugger(
			DevicePtr	pOutpDvce
	) :
		outpDvce( pOutpDvce )
	{
	}

	virtual void consumeMessage(
		const	ColoredBuffer &	pDebugMsg
	);

protected :

private :

	DevicePtr	outpDvce;
	FastCS		critSect;

	/// \brief Forbidden default constructor.

	Debugger(
	);

	/// \brief Forbidden copy constructor.

	Debugger(
		const	Debugger &
	);

	/// \brief Forbidden copy operator.

	Debugger & operator = (
		const	Debugger &
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_Debugger_H_

// ============================================================================

