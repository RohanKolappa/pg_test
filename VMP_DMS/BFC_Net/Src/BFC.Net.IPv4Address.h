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
//	BFC.Net.IPv4Address.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IPv4Address_H_
#define _BFC_IPv4Address_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCS.h"

// ============================================================================

#define LOCALHOST	"localhost"
#define LOOPBACK	"127.0.0.1"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief IP v4 address (4 bytes).
///
/// \ingroup BFC_Net

class BFC_Net_DLL IPv4Address {

public: 

	static IPv4Address loopback() { return IPv4Address( LOOPBACK ); }
	static IPv4Address localhost() { return IPv4Address( LOCALHOST ); }

	IPv4Address(
	);

	IPv4Address(
		const	IPv4Address &	addr
	);

	IPv4Address(
		const	Char *		addr,
		const	Bool		tryResolve = false
	);

	IPv4Address(
		const	Buffer &	addr,
		const	Bool		tryResolve = false
	);

	IPv4Address(
		const	Uint32		addr	///< in NBO!!!
	);

	IPv4Address& operator = (
		const	IPv4Address &	o
	);

	void initWithSelf(
	);

	Bool operator == (
		const	IPv4Address &	a2
	) const {
		return ( addr == a2.addr );
	}

	Bool operator != (
		const	IPv4Address &	a2
	) const {
		return ( addr != a2.addr );
	}

	Bool isMulticast(
	) const;

	Bool hasFQN(
	) const;

	Buffer getFQN(
	) const;

	Buffer getDot(
	) const;

	Buffer toBuffer(
	) const;

	Uint32 getUint(
	) const {	// hack!!! in NBO
		return addr;
	}

private:

	Uint32		addr;	// IP in network byte order
	Buffer		fqn;

	FastCS		resolverMutex;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================


#endif  // _BFC_IPv4Address_H_


// ============================================================================





