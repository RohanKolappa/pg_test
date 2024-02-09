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
//	BFC.Net.MACAddress.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MACAddress_H_
#define _BFC_MACAddress_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief MAC address (6 bytes).
///
/// \ingroup BFC_Net

class BFC_Net_DLL MACAddress {

public: 

	MACAddress(
	);

	MACAddress(
		const	MACAddress&	addr
	);

	MACAddress(
		const	Buffer&		addr
	);	// 1.2.3.4.5.6

	virtual ~MACAddress(
	);

	MACAddress& operator = (
		const	MACAddress&	other
	);

	void buildFromRawBuffer(
		const	Buffer&		rawBuffer
	);

	Bool operator == (
		const	MACAddress&	a2
	) const {
		return ( addr == a2.addr );
	}

	Bool operator != (
		const	MACAddress&	a2
	) const {
		return ( addr != a2.addr );
	}

	Buffer getDot(
	) const;

	const Buffer& getRaw(
	) const {
		return addr;
	}

	Bool isOK(
	) const {
		return isOk;
	}

	Bool isNull(
	) const;

	BASE_CLASS_GEN_EXCEPTION( "BFC.Net.MACAddress" );

	CLASS_EXCEPTION( ArgumentOverflow, "Argument overflow" );
	CLASS_EXCEPTION( BadArgument, "Bad argument" );
	CLASS_EXCEPTION( ElementOverflow, "Element overflow" );
	CLASS_EXCEPTION( EmptyArgument, "Empty argument" );
	CLASS_EXCEPTION( InvalidElement, "Invalid element" );
	CLASS_EXCEPTION( MisformedAddress, "Misformed address" );

private:

	Bool		isOk;
	Buffer		addr;	// always 6 bytes

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif  // _BFC_MACAddress_H_

// ============================================================================

