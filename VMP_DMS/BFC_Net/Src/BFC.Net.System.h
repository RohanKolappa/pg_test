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
//	BFC.Net.System.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_System_H_
#define _BFC_Net_System_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, System )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Net.NetIface.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Provides info about available network interfaces.
///
/// \ingroup BFC_Net

class BFC_Net_DLL System : virtual public DObject {

protected :

	/// \brief Creates a new System.

	System(
	);

public :

	static SystemPtr instance(
	);

	/// \brief Refreshes the internal (cached) lists of devices and
	///	addresses.

	void refresh(
	);

	/// \brief Returns the number of network interfaces found on this
	///	system.

	Uint32 getNbrNetIfaces(
	) const;

	/// \brief Returns the \a pIndex-th network interface (in internal
	///	cache order, not in system order).

	NetIfaceCPtr getNetIface(
		const	Uint32		pIndex
	) const;

	/// \brief Searches for the network interface with the specified name.
	///
	/// \param pName
	///	The name of the network interface.
	///
	/// \returns
	///	A NetIface with the specified name, or null if there is no
	///	network interface with the specified name.

	NetIfaceCPtr findNetIface(
		const	Buffer &	pName
	) const;

	/// \brief Searches for a network interface given its index.
	///
	/// \param pIndex
	///	The index of the interface.

	NetIfaceCPtr findNetIface(
		const	Uint32		pIndex
	) const;

	/// \brief Searches for a network interface that has the specified
	///	Internet Protocol (IP) address bound to it.
	///
	/// If the specified IP address is bound to multiple network interfaces,
	/// it is not defined which network interface is returned.
	///
	/// \param pAddr
	///	The IPAddr to search with.
	///
	/// \returns
	///	A NetIface or null if there is no network interface with the
	///	specified IP address.

	NetIfaceCPtr findNetIface(
			IPAddrCPtr	pAddr
	) const;

	/// \brief Returns the number of IP addresses assigned to the various
	///	network interfaces on this system.

	Uint32 getNbrIPAddrs(
	) const;

	/// \brief Returns the \a pIndex-th IP address.

	IPAddrCPtr getIPAddr(
		const	Uint32		pIndex
	) const ;

protected :

private :

	Map< Buffer, NetIfaceCPtr >	devices;
	Array< IPAddrCPtr >		address;

	/// \brief Forbidden copy constructor.

	System(
		const	System &
	);

	/// \brief Forbidden copy operator.

	System & operator = (
		const	System &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_System_H_

// ============================================================================

