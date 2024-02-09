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
//	BFC.Net.Log.Handler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Log_Handler_H_
#define _BFC_Net_Log_Handler_H_

// ============================================================================

#include "BFC.Net.Log.DLL.h"

// ============================================================================

#include "BFC.Net.Log.HandlerPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.MT.Thread.h"

#include "BFC.Net.TCPSocketPtr.h"

// ============================================================================

namespace BFC {
namespace Net {
namespace Log {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net_Log

class BFC_Net_Log_DLL Handler : public MT::Thread {

public :

	/// \brief Creates a new Handler.

	Handler(
			Net::TCPSocketPtr
					pClntSock,
		const	Bool		pConsFlag,
		const	Bool		pFileFlag
	);

	/// \brief Destroys this object.

	virtual ~Handler(
	);

protected :

	virtual void run(
	);

private :

	Net::TCPSocketPtr	clntSock;
	Buffer			peerAddr;
	Bool			consFlag;
	Bool			fileFlag;

	/// \brief Forbidden default constructor.

	Handler(
	);

	/// \brief Forbidden copy constructor.

	Handler(
		const	Handler &
	);

	/// \brief Forbidden copy operator.

	Handler & operator = (
		const	Handler &
	);

};

// ============================================================================

} // namespace Log
} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Log_Handler_H_

// ============================================================================

