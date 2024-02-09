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
//	BFC.Net.NetIface.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_NetIface_H_
#define _BFC_Net_NetIface_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.NetIfacePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.Net.IPAddrPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents a Network Interface made up of a name, and a list of IP
///	addresses assigned to this interface.
///
/// This class is used to identify the local interface on which a multicast
/// group is joined.
///
/// Interfaces are normally known by names such as "eth0".
///
/// \ingroup BFC_Net

class BFC_Net_DLL NetIface : virtual public DObject {

public :

	/// \brief Returns all the interfaces on this machine, or null if no
	///	network interfaces could be found on this machine.

	static NetIfaceArray getNetworkInterfaces(
	);

	/// \brief Creates a new NetIface object with name set to \a pName, and
	///	all other fields set to default values.

	NetIface(
		const	Buffer &	pName
	);

//	NetIface(
//		const	Buffer &	pName,
//		const	Uint32		pIndex,
//		const	IPAddrCArray &	pAddrs
//	) :
//		name	( pName ),
//		indx	( pIndex ),
//		virt	( false ),
//		addr	( pAddrs ) {
//	}

	/// \brief Returns the name of this network interface.

	const Buffer & getName(
	) const {
		return name;
	}

	/// \brief Returns the index of this network interface.

	Uint32 getIndex(
	) const {
		return indx;
	}

	/// \brief Returns whether this interface is a virtual interface (also
	///	called subinterface).
	///
	/// Virtual interfaces are, on some systems, interfaces created as a
	/// child of a physical interface and given different settings (like
	/// address or MTU). Usually the name of the interface will the name of
	/// the parent followed by a colon (:) and a number identifying the
	/// child since there can be several virtual interfaces attached to a
	/// single physical interface.

	Bool isVirtual(
	) const {
		return virt;
	}

	/// \brief Returns the first IPAddr bound to this network interface.

	IPAddrCPtr getAddress(
	) const {
		return addr;
	}

	/// \brief Returns the broadcast address for this network interface.

	IPAddrCPtr getBroadcast(
	) const {
		return brdc;
	}

	/// \brief Returns the network prefix length for this address.
	///
	/// This is also known as the subnet mask in the context of IPv4
	/// addresses.
	///
	/// Typical IPv4 values would be 8 (255.0.0.0), 16 (255.255.0.0) or 24
	/// (255.255.255.0). Typical IPv6 values would be 128 (::1/128) or 10
	/// (fe80::203:baff:fe27:1243/10).
	///
	/// \returns
	///	The prefix length for the subnet of that address.

	Uint32 getNetMask(
	) const {
		return mask;
	}

	/// \brief Returns an Array with all the subinterfaces (also known as
	///	virtual interfaces) attached to this network interface.
	///
	/// For instance eth0:1 will be a subinterface to eth0.

	const NetIfaceCArray & getSubInterfaces(
	) const {
		return chld;
	}

	/// \brief Returns the parent NetIface of this interface if this is a
	///	subinterface, or null if it is a physical (non virtual)
	///	interface or has no parent.

	NetIfaceCPtr getParent(
	) const {
		return prnt;
	}

	enum Flag {
		Up,		///< Interface is running.
		Broadcast,	///< Valid broadcast address set.
		Debug,		///< Internal debugging flag.
		Loopback,	///< Interface is a loopback interface.
		Pointopoint,	///< Interface is a point-to-point link.
		Running,	///< Resources allocated.
		Noarp,		///< No arp protocol, L2 destination address not set.
		Promisc,	///< Interface is in promiscuous mode.
		Notrailers,	///< Avoid use of trailers.
		Allmulti,	///< Receive all multicast packets.
		Master,		///< Master of a load balancing bundle.
		Slave,		///< Slave of a load balancing bundle.
		Multicast,	///< Supports multicast
		Portsel,	///< Is able to select media type via ifmap.
		Automedia,	///< Auto media selection active.
		Dynamic		///< The  addresses  are lost when the interface goes down.
	};

	Bool hasFlag(
		const	Flag		f
	) const;

	/// \brief Returns whether a network interface is up and running.

	Bool isUp(
	) const {
		return ( hasFlag( Up ) && hasFlag( Running ) );
	}

	/// \brief Returns whether a network interface is a loopback interface.

	Bool isLoopback(
	) const {
		return hasFlag( Loopback );
	}

	/// \brief Returns whether a network interface is a point to point
	///	interface.
	///
	/// A typical point to point interface would be a PPP connection
	/// through a modem.

	Bool isPointToPoint(
	) const {
		return hasFlag( Pointopoint );
	}

	/// \brief Returns whether a network interface supports multicasting
	///	or not.

	Bool supportsMulticast(
	) const {
		return hasFlag( Multicast );
	}

//	/// \brief Returns the hardware address (usually MAC) of the interface
//	///	if it has one.
//
//	Buffer getHardwareAddress(
//	) const;

//	const MACAddress & getMacAddr(
//	) const {
//		return macAddr;
//	}

	/// \brief Returns the Maximum Transmission Unit (MTU) of this
	///	interface.

	Uint32 getMTU(
	) const {
		return mtu;
	}

//	/*
//	 * Compares this object against the specified object.
//	 * The result is <code>true</code> if and only if the argument is
//	 * not <code>null</code> and it represents the same NetIface
//	 * as this object.
//	 * <p>
//	 * Two instances of <code>NetIface</code> represent the same
//	 * NetIface if both name and addrs are the same for both.
//	 *
//	 * @param   obj   the object to compare against.
//	 * @return  <code>true</code> if the objects are the same;
//	 *		  <code>false</code> otherwise.
//	 * @see	 java.net.InetAddress#getRawAddr()
//	 */
//	Bool equals(Object obj) {
//		if ((obj == null) || !(obj instanceof NetIface)) {
//			return false;
//		}
//		NetIface netIF = (NetIface)obj;
//		if (name != null ) {
//			if (netIF.getName() != null) {
//				if (!name.equals(netIF.getName())) {
//					return false;
//				}
//			} else {
//				return false;
//			}
//		} else {
//			if (netIF.getName() != null) {
//				return false;
//			}
//		}
//		Enumeration newAddrs = netIF.getInetAddresses();
//		int i = 0;
//		for (i = 0; newAddrs.hasMoreElements();newAddrs.nextElement(), i++);
//		if (addr == null) {
//			if (i != 0) {
//				return false;
//			}
//		} else {
//			/*
//			 * Compare number of addresses (in the checked subset)
//			 */
//			int count = 0;
//			Enumeration e = getInetAddresses();
//			for (; e.hasMoreElements(); count++) {
//				e.nextElement();
//			}
//			if (i != count) {
//				return false;
//			}
//		}
//		newAddrs = netIF.getInetAddresses();
//		for (; newAddrs.hasMoreElements();) {
//			Bool equal = false;
//			Enumeration thisAddrs = getInetAddresses();
//			IPAddr newAddr = (IPAddr)newAddrs.nextElement();
//			for (; thisAddrs.hasMoreElements();) {
//				IPAddr thisAddr = (IPAddr)thisAddrs.nextElement();
//				if (thisAddr.equals(newAddr)) {
//					equal = true;
//				}
//			}
//			if (!equal) {
//				return false;
//			}
//		}
//		return true;
//	}

	void refresh(
	);

	Buffer toBuffer(
	) const;

	void dump(
	) const;

protected :

	static void enumIPv4Interfaces(
			NetIfaceArray &	pArray
	);

	static void enumIPv6Interfaces(
			NetIfaceArray &	pArray
	);

private :

	Buffer		name;	///< Interface name.
	Uint32		indx;	///< Interface index (-1 if unknown).
	Uint32		flgs;	///< Interface flags.
	Uint32		mtu;	///< Maximum Transmit Unit.
	Bool		virt;	///< Is this a virtual interface ?
	IPAddrPtr	addr;	///< Interface address.
	IPAddrPtr	brdc;	///< Broadcast address.
	Uint32		mask;	///< Netmask.
	NetIfaceCArray	chld;
	NetIfacePtr	prnt;

	/// \brief Forbidden default constructor.

	NetIface(
	);

	/// \brief Forbidden copy constructor.

	NetIface(
		const	NetIface &
	);

	/// \brief Forbidden copy operator.

	NetIface & operator = (
		const	NetIface &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_NetIface_H_

// ============================================================================

