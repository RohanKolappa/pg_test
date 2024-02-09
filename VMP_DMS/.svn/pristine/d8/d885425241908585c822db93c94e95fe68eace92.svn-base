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
//	BFC.Net.Packet.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Packet_H_
#define _BFC_Net_Packet_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.PacketPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.AddressPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net

class BFC_Net_DLL Packet : virtual public SObject {

public :

	Packet(
		const	Buffer &	pData,
			AddressCPtr	pPeer = AddressCPtr(),
			AddressCPtr	pSelf = AddressCPtr()
	) :
		data	( pData ),
		peer	( pPeer ),
		self	( pSelf )
	{
	}

	const Buffer & getBuffer(
	) const {
		return data;
	}

	AddressCPtr getPeerAddr(
	) const {
		return peer;
	}

	AddressCPtr getSelfAddr(
	) const {
		return self;
	}

protected :

private :

	Buffer		data;
	AddressCPtr	peer;
	AddressCPtr	self;

	/// \brief Forbidden default constructor.

	Packet(
	);

	/// \brief Forbidden copy constructor.

	Packet(
		const	Packet &
	);

	/// \brief Forbidden copy operator.

	Packet & operator = (
		const	Packet &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Packet_H_

// ============================================================================

