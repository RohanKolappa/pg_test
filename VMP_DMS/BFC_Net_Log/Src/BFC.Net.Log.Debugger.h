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
//	BFC.Net.Log.Debugger.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Log_Debugger_H_
#define _BFC_Net_Log_Debugger_H_

// ============================================================================

#include "BFC.Net.Log.DLL.h"

// ============================================================================

#include "BFC.Net.Log.DebuggerPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.Debug.Debugger.h"

#include "BFC.IO.ByteOutputStreamPtr.h"

#include "BFC.Net.SockAddrPtr.h"

// ============================================================================

namespace BFC {
namespace Net {
namespace Log {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net_Log

class BFC_Net_Log_DLL Debugger : public BFC::Debugger {

public :

	/// \brief Creates a new Debugger.

	Debugger(
	);

	void init(
			SockAddrCPtr	pSockAddr
	);

	virtual void consumeMessage(
		const	ColoredBuffer &	message
	);

protected :

private :

	FastCS			critSect;
	IO::ByteOutputStreamPtr	outpStrm;

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

} // namespace Log
} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Log_Debugger_H_

// ============================================================================

