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
//	BFC.Net.NetIface.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if ! defined( PLATFORM_WIN32 )
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#endif

#include "BFC.Base.System.h"

#include "BFC.Debug.DObject.h"

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.NetIface.h"
#include "BFC.Net.SDAutoCloser.h"
#include "BFC.Net.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, NetIface )

// ============================================================================

Net::NetIfaceArray Net::NetIface::getNetworkInterfaces() {

	NetIfaceArray	res;

	enumIPv4Interfaces( res );
	enumIPv6Interfaces( res );

	return res;

}

// ============================================================================

Net::NetIface::NetIface(
	const	Buffer &	pName ) :

	name	( pName ),
	indx	( 0 ),
	flgs	( 0 ),
	mtu	( 0 ),
	virt	( false ),
	mask	( 0 ) {

	setObjectName( "BFC.Net.NetIface( \"" + pName + "\" )" );

}

// ============================================================================

Bool Net::NetIface::hasFlag(
	const	Flag		f ) const {

#if defined( PLATFORM_WIN32 )
	// todo
	return false;
#else

	switch ( f ) {
	case Up			: return ( flgs & IFF_UP          );
	case Broadcast		: return ( flgs & IFF_BROADCAST   );
	case Debug		: return ( flgs & IFF_DEBUG       );
	case Loopback		: return ( flgs & IFF_LOOPBACK    );
	case Pointopoint	: return ( flgs & IFF_POINTOPOINT );
	case Running		: return ( flgs & IFF_RUNNING     );
	case Noarp		: return ( flgs & IFF_NOARP       );
	case Promisc		: return ( flgs & IFF_PROMISC     );
	case Notrailers		: return ( flgs & IFF_NOTRAILERS  );
	case Allmulti		: return ( flgs & IFF_ALLMULTI    );
	case Master 
#if defined( IFF_MASTER )
			: return ( flgs & IFF_MASTER      );
#else
			: return false; // not supported
#endif
	case Slave
#if defined( IFF_SLAVE )
			: return ( flgs & IFF_SLAVE       );
#else
			: return false; // not supported
#endif
	case Multicast		: return ( flgs & IFF_MULTICAST   );
	case Portsel
#if defined( IFF_PORTSEL )
			: return ( flgs & IFF_PORTSEL     );
#else
			: return false; // not supported
#endif
	case Automedia
#if defined( IFF_AUTOMEDIA )
			: return ( flgs & IFF_AUTOMEDIA   );
#else
			: return false; // not supported
#endif
	case Dynamic
#if defined( IFF_DYNAMIC )
				: return ( flgs & IFF_DYNAMIC    );
#else
				: return false; // not supported
#endif
	}

	throw InvalidArgument();

#endif
}

//Buffer Net::NetIface::getHardwareAddress() const {
//
//	for (IPAddr addr : addr) {
//		if (addr instanceof Inet4Address) {
//			return getMacAddr0(((Inet4Address)addr).getRawAddr(), name, indx);
//		}
//	}
//
//	return getMacAddr0(null, name, indx);
//
//}

// ============================================================================

void Net::NetIface::refresh() {

#if defined( PLATFORM_WIN32 )

	throw NotImplemented();

#else

	int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );

	if ( sock < 0 ) {
		throw InternalError( "Can't create socket!" );
	}

	SDAutoCloser sdAutoCloser( sock );

	ifreq	if2;

	name.dumpToBuffer( ( Uchar * )if2.ifr_name, sizeof( if2.ifr_name ) );

	// ... flags

	if ( ::ioctl( sock, SIOCGIFFLAGS, &if2 ) < 0 ) {
		throw InternalError( "Can't ioctl( SIOCGIFFLAGS )!" );
	}

	flgs = ( Uint32 )if2.ifr_flags;

	// ... index

#if defined( PLATFORM_DARWIN )
	indx = if_nametoindex( if2.ifr_name );
#else
	if ( ::ioctl( sock, SIOCGIFINDEX, &if2 ) >= 0 ) {
		indx = ( Uint32 )if2.ifr_ifindex;
	}
	else {
		indx = ( Uint32 )-1;
	}
#endif

	// ... MTU

	if ( ::ioctl( sock, SIOCGIFMTU, &if2 ) < 0 ) {
		throw InternalError( "Can't ioctl( SIOCGIFMTU )!" );
	}

	mtu = ( Uint32 )if2.ifr_mtu;

	// ... addr

	if ( ::ioctl( sock, SIOCGIFADDR, &if2 ) < 0 ) {
		throw InternalError( "Can't ioctl( SIOCGIFADDR )!" );
	}

	addr = IPAddr::createNew( &if2.ifr_addr );

	// ... broadcast

	if ( flgs & IFF_BROADCAST ) {

		if ( ::ioctl( sock, SIOCGIFBRDADDR, &if2 ) < 0 ) {
			throw InternalError( "Can't ioctl( SIOCGIFBRDADDR )!" );
		}

		brdc = IPAddr::createNew( &if2.ifr_broadaddr );
	}
	else {

		brdc.kill();

	}

	// ... netmask

	if ( ::ioctl( sock, SIOCGIFNETMASK, &if2 ) < 0 ) {
		throw InternalError( "Can't ioctl( SIOCGIFNETMASK )!" );
	}

	Uint32 tmp = ntohl((( sockaddr_in * )&(if2.ifr_addr))->sin_addr.s_addr);

	mask = 0;

	while ( tmp ) {
		tmp <<= 1;
		mask++;
	}

//	// ... hardware address
//
//	if ( ::ioctl( sd, SIOCGIFHWADDR, &ifr ) == 0 ) {
//
//		switch ( ifr.ifr_hwaddr.sa_family ) {
//		case ARPHRD_NETROM:
//		case ARPHRD_ETHER:
//		case ARPHRD_PPP:
//		case ARPHRD_EETHER:
//		case ARPHRD_IEEE802: {
//			macAddr.buildFromRawBuffer(
//				Buffer(
//					( const Uchar * )&ifr.ifr_addr.sa_data,
//					6,
//					true ) );
//			break;
//			}
//		}
//
//	}

#endif

}

//void addif(
//		NetIfaceArray &	pArray,
//	const	Buffer &	pName,
//	const	Uint32		pIndx,
//		IPAddrCPtr	pAddr,
//	const	Uint32		prefix ) {
//
//	Bool		isVirtual = false;
//	Buffer		name = pName;
//
//	IfaceAddrPtr	ifce = new IfaceAddr;
//
//	ifce->setRawAddr( pAddr );
//	ifce->setMaskLength( prefix );
//
//	if ( pAddr->isIPv4() ) {
//		ifce->setBroadcast( getBroadcast( name ) );
//		if ( ifce->getBroadcast() ) {
//			ifce->setMaskLength( getSubnet( name ) );
//		}
//	}
//
//	Buffer	vname;
//	Uint32	len = name.findFwd( ':' );
//
//	if ( len == 0 ) {
//		throw InternalError( "Interface name starts with ':'!" );
//	}
//
//	if ( len < name.getLength() ) {
//		int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
//		if ( sock < 0 ) {
//			throw InternalError( "Can't create IPv4socket!" );
//		}
//		ifreq if2;
//		vname = name( 0, len );
//		System::bzero( ( Uchar * )&if2, sizeof( if2 ) );
//		System::strcpy( if2.ifr_name, vname.getCStringPtr() );
//		if ( ::ioctl( sock, SIOCGIFFLAGS, ( char * )&if2 ) >= 0 ) {
//			// Got access to parent, so create it if necessary.
//			vname = name;
//			name = name( 0, len );
//		}
//		else {
//			// failed to access parent interface do not create parent.
//			// We are a virtual interface with no parent.
//			isVirtual = true;
//			vname.kill();
//		}
//		::close( sock );
//	}
//
//	NetIfacePtr currif;
//
//	for ( Uint32 i = 0 ; i < pArray.getSize() ; i++ ) {
//		if ( pArray[ i ]->name == name ) {
//			currif = pArray[ i ];
//			break;
//		}
//	}
//
//	if ( ! currif ) {
//		currif = new NetIface;
//		currif->name = name;
//		currif->desc = name;
//		currif->indx = pIndx;
//		currif->virt = isVirtual;
//		pArray += currif;
//	}
//
//	currif->bind += ifce;
//
//	NetIfacePtr parent = currif;
//
//	if ( vname.hasContent() ) {
//		currif.kill();
//		for ( Uint32 i = 0 ; i < parent->chld.getSize() ; i++ ) {
//			if ( parent->chld[ i ]->name == vname ) {
//				currif = parent->chld[ i ];
//			}
//		}
//		if ( ! currif ) {
//			currif = new NetIface;
//			currif->name = vname;
//			currif->desc = vname;
//			currif->indx = pIndx;
//			currif->virt = true;
//			currif->prnt = parent;
//			parent->chld += currif;
//		}
//		currif->addr += iface;
//	}
//
//}

//Net::NetIfacePtr createNetworkInterface(
//		netifPtr	ifs ) {
//
//	NetIfacePtr res = new NetIface;
//
//	res->name = ifs->name;
//	res->indx = ifs->index;
//	res->virt = ifs->virt;
//
//	for ( Uint32 i = 0 ; i < ifs->addr.getSize() ; i++ ) {
//
//		netaddrPtr	addrP = ifs->addr[ i ];
//
//		IPAddrPtr	addr;
//
//		if ( addrP->family == AF_INET ) {
//			addr = addrP->addr;
//			IfaceAddrPtr ifce = new IfaceAddr;
//			ifce->setRawAddr( addr );
//			if ( addrP->bcst ) {
//				ifce->setBroadcast( addrP->bcst );
//				ifce->setMaskLength( addrP->mask );
//			}
//			res->bind += ifce;
//		}
//
//		if ( addrP->family == AF_INET6 ) {
//			throw NotImplemented();
////			int scope=0;
////			addr = (*env)->NewObject(env, ni_ia6cls, ni_ia6ctrID);
////			if (addr) {
////				jbyteArray ipaddress = (*env)->NewByteArray(env, 16);
////				if (ipaddress == NULL) {
////					return NULL;
////				}
////				(*env)->SetByteArrayRegion(env, ipaddress, 0, 16,
////					(jbyte *)&(((struct sockaddr_in6*)addrP->addr)->sin6_addr));
////				if (!kernelIsV22()) {
////					scope = ((struct sockaddr_in6*)addrP->addr)->sin6_scope_id;
////				}
////				if (scope != 0) { /* zero is default value, no need to set */
////					(*env)->SetIntField(env, addr, ia6_scopeidID, scope);
////					(*env)->SetBooleanField(env, addr, ia6_scopeidsetID, JNI_TRUE);
////					(*env)->SetObjectField(env, addr, ia6_scopeifnameID, res);
////				}
////				(*env)->SetObjectField(env, addr, ni_ia6ipaddressID, ipaddress);
////			}
////			ifce = (*env)->NewObject(env, ni_ibcls, ni_ibctrID);
////			if (ifce) {
////			  (*env)->SetObjectField(env, ifce, ni_ibaddressID, addr);
////			  (*env)->SetShortField(env, ifce, ni_ib4maskID, addrP->mask);
////			  (*env)->SetObjectArrayElement(env, res->bind, bind_index++, ifce);
////			}
//		}
//
//		if ( addr == NULL ) {
//			throw InternalError( "Unsupported family!" );
//		}
//
//		res->addr += addr;
//
//	}
//
//	for ( Uint32 i = 0 ; i < ifs->chld.getSize() ; i++ ) {
//		netifPtr childP = ifs->chld[ i ];
//		tmp = createNetworkInterface( childP );
//		tmp->prnt = res;
//		res->chld += tmp;
//	}
//
//	return res;
//
//}

// ============================================================================

Buffer Net::NetIface::toBuffer() const {

	Buffer result
		= Buffer( "name: " )
		+ ( name.hasContent() ? name : Buffer( "null" ) )
		+ ", indx: " + Buffer( indx )
		+ ", addr: " + addr->toBuffer()
		+ ", brdc: " + ( brdc ? brdc->toBuffer() : Buffer( "n/a" ) ) + ";";

	return result;

}

void Net::NetIface::dump() const {

	msgDbg( "Name: " + name );
	msgDbg( "... dev indx: " + Buffer( indx ) );
	msgDbg( "... dev addr: " + addr->toBuffer() );
	msgDbg( "... dev mask: " + Buffer( mask ) );
	msgDbg( "... brd addr: " + ( brdc ? brdc->toBuffer() : Buffer( "n/a" ) ) );
	msgDbg( "... dev mask: " + Buffer( mask ) );
//	msgDbg( "... mac addr: " + macAddr.getDot() );
	msgDbg( "... mtu     : " + Buffer(mtu) );
	msgDbg( "... flags:" );
	msgDbg( ".......... - Up          :" + Buffer(hasFlag(Up         )));
	msgDbg( ".......... - Broadcast   :" + Buffer(hasFlag(Broadcast  )));
	msgDbg( ".......... - Debug       :" + Buffer(hasFlag(Debug      )));
	msgDbg( ".......... - Loopback    :" + Buffer(hasFlag(Loopback   )));
	msgDbg( ".......... - Pointopoint :" + Buffer(hasFlag(Pointopoint)));
	msgDbg( ".......... - Running     :" + Buffer(hasFlag(Running    )));
	msgDbg( ".......... - Noarp       :" + Buffer(hasFlag(Noarp      )));
	msgDbg( ".......... - Promisc     :" + Buffer(hasFlag(Promisc    )));
	msgDbg( ".......... - Notrailers  :" + Buffer(hasFlag(Notrailers )));
	msgDbg( ".......... - Allmulti    :" + Buffer(hasFlag(Allmulti   )));
	msgDbg( ".......... - Master      :" + Buffer(hasFlag(Master     )));
	msgDbg( ".......... - Slave       :" + Buffer(hasFlag(Slave      )));
	msgDbg( ".......... - Multicast   :" + Buffer(hasFlag(Multicast  )));
	msgDbg( ".......... - Portsel     :" + Buffer(hasFlag(Portsel    )));
	msgDbg( ".......... - Automedia   :" + Buffer(hasFlag(Automedia  )));
	msgDbg( ".......... - Dynamic     :" + Buffer(hasFlag(Dynamic    )));

}

// ============================================================================

void Net::NetIface::enumIPv4Interfaces(
		NetIfaceArray &	pArray ) {

#if defined( PLATFORM_WIN32 )

	throw NotImplemented();

#else

	int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );

	if ( sock < 0 ) {
		if ( errno != EPROTONOSUPPORT ) {
			throw InternalError( "Can't create IPv4 socket!" );
		}
		return;
	}

	SDAutoCloser sdAutoCloser( sock );

	const Uint32	maxIfr = 16;
	const Uint32	lenIfr = maxIfr * sizeof( ifreq );
	ifreq		ifrLst[ maxIfr ];
	ifconf		ifc;

	ifc.ifc_len = lenIfr;
	ifc.ifc_req = ifrLst;

	if ( ::ioctl( sock, SIOCGIFCONF, &ifc ) < 0 ) {
		throw InternalError( Buffer( "ioctl( SIOCGIFCONF ): " ) + ::strerror( errno ) );
	}

	if ( ifc.ifc_len % sizeof( ifreq ) ) {
		throw InternalError( "Need to read the manpage :-(" );
	}

	const Uint32	nbrIfr = ifc.ifc_len / sizeof( ifreq );

	for ( Uint32 i = 0 ; i < nbrIfr ; i++ ) {

		ifreq * ifr = ifrLst + i;

//		if ( i && ifr->ifr_addr.sa_data == ( ifr - 1 )->ifr_addr.sa_data ) {
////			msgWrn( "refresh(): duplicate entries!" );
//			continue;
//		}

		NetIfacePtr ifce = new NetIface( Buffer( ifr->ifr_name, true ) );

		ifce->refresh();

//		staticMsgDbg( "IF: " + ifce->toBuffer() );

		pArray += ifce;

	}

#endif

}

void Net::NetIface::enumIPv6Interfaces(
		NetIfaceArray &	/*ifs*/ ) {

	if ( ! ipv6_available() ) {
		return;
	}

//	FILE *f;
//	char addr6[40], devname[20];
//	char addr6p[8][5];
//	int plen, scope, dad_status, if_idx;
//	uint8_t ipv6addr[16];
//
//	if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
//		while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
//					  addr6p[0], addr6p[1], addr6p[2], addr6p[3],
//					  addr6p[4], addr6p[5], addr6p[6], addr6p[7],
//				  &if_idx, &plen, &scope, &dad_status, devname) != EOF) {
//			struct netif *ifs_ptr = NULL;
//			struct netif *last_ptr = NULL;
//			struct sockaddr_in6 addr;
//
//			sprintf(addr6, "%s:%s:%s:%s:%s:%s:%s:%s",
//					addr6p[0], addr6p[1], addr6p[2], addr6p[3],
//					addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
//			inet_pton(AF_INET6, addr6, ipv6addr);
//
//			memset(&addr, 0, sizeof(struct sockaddr_in6));
//			memcpy((void*)addr.sin6_addr.s6_addr, (const void*)ipv6addr, 16);
//			addr.sin6_scope_id = if_idx;
//
//			ifs = addif(env, ifs, devname, if_idx, AF_INET6,
//						(struct sockaddr *)&addr,
//						sizeof(struct sockaddr_in6), plen);
//
//			/*
//			 * If an exception occurred then return the list as is.
//			 */
//			if ((*env)->ExceptionOccurred(env)) {
//				fclose(f);
//				return ifs;
//			}
//		}
//		fclose(f);
//	}

}

// ============================================================================

///**
// * Get the Hardware address (usually MAC address) for the named interface.
// * return puts the data in buf, and returns the length, in byte, of the
// * MAC address. Returns -1 if there is no hardware address on that interface.
// */
//int getMacAddress(JNIEnv *env, const struct in_addr* addr, const char* ifname,
//				  unsigned char *buf) {
//  int sock;
//  static struct ifreq ifr;
//  int i;
//
//  sock = JVM_Socket(AF_INET, SOCK_DGRAM, 0);
//
//  if (sock < 0) {
//	NET_ThrowByNameWithLastError(env, JNU_JAVANETPKG "SocketException",
//								 "Socket creation failed");
//	return -1;
//  }
//
//  strcpy(ifr.ifr_name, ifname);
//
//  if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
//	fprintf(stderr, "SIOCIFHWADDR: %s\n",
//			strerror(errno));
//	close(sock);
//	return -1;
//  }
//  memcpy(buf, &ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);
//  close(sock);
//  for (i = 0; i < IFHWADDRLEN; i++) {
//	if (buf[i] != 0)
//	  return IFHWADDRLEN;
//  }
//  /*
//   * All bytes to 0 means no hardware address.
//   */
//  return -1;
//}

///*
// * Class:	  java_net_NetworkInterface
// * Method:	  getMacAddr0
// * Signature: ([bLjava/lang/String;I)[b
// */
//JNIEXPORT jbyteArray JNICALL Java_java_net_NetworkInterface_getMacAddr0(JNIEnv *env, jclass class, jbyteArray addrArray, jstring name, jint index) {
//  jint addr;
//  jbyte caddr[4];
//  struct in_addr iaddr;
//  jbyteArray ret = NULL;
//  unsigned char mac[16];
//  int len;
//  jboolean isCopy;
//  const char* name_utf = (*env)->GetStringUTFChars(env, name, &isCopy);
//
//  if (!IS_NULL(addrArray)) {
//	(*env)->GetByteArrayRegion(env, addrArray, 0, 4, caddr);
//	addr = ((caddr[0]<<24) & 0xff000000);
//	addr |= ((caddr[1] <<16) & 0xff0000);
//	addr |= ((caddr[2] <<8) & 0xff00);
//	addr |= (caddr[3] & 0xff);
//	iaddr.s_addr = htonl(addr);
//	len = getMacAddress(env, &iaddr, name_utf, mac);
//  } else {
//	len = getMacAddress(env, NULL, name_utf, mac);
//  }
//  if (len > 0) {
//	ret = (*env)->NewByteArray(env, len);
//	if (IS_NULL(ret)) {
//	  /* we may have memory to free at the end of this */
//	  goto fexit;
//	}
//	(*env)->SetByteArrayRegion(env, ret, 0, len, (jbyte *) (mac));
//  }
// fexit:
//  /* release the UTF string and interface list */
//  (*env)->ReleaseStringUTFChars(env, name, name_utf);
//  return ret;
//}

// ============================================================================

