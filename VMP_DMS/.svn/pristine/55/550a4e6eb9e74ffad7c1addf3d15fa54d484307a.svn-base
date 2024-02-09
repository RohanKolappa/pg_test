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
//	BFC.Net.Log.Server.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Log_Server_H_
#define _BFC_Net_Log_Server_H_

// ============================================================================

#include "BFC.Net.Log.DLL.h"

// ============================================================================

#include "BFC.Net.Log.ServerPtr.h"

// ============================================================================

#include "BFC.MT.Thread.h"

// ============================================================================

namespace BFC {
namespace Net {
namespace Log {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net_Log

class BFC_Net_Log_DLL Server : public MT::Thread {

public :

	/// \brief Creates a new Server.

	Server(
		const	Bool		pCons,
		const	Bool		pFile,
		const	Uint16		pPort = 0
	);

	/// \brief Destroys this object.

	virtual ~Server(
	);

protected :

	virtual void run(
	);

private :

	Bool		cons;
	Bool		file;
	Uint16		port;

	/// \brief Forbidden copy constructor.

	Server(
		const	Server &
	);

	/// \brief Forbidden copy operator.

	Server & operator = (
		const	Server &
	);

};

// ============================================================================

} // namespace Log
} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Log_Server_H_

// ============================================================================

