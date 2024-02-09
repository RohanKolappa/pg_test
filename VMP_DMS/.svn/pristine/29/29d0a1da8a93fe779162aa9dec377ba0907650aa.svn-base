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
//	BFC.Net.Utils.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Utils_H_
#define _BFC_Net_Utils_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

#if 0

#define MAX_PACKET_LEN 65536

#define IPv4 1
#define IPv6 2

#define NET_ERROR(env, ex, msg) \
{ if (!(*env)->ExceptionOccurred(env)) JNU_ThrowByName(env, ex, msg) }

#define CHECK_NULL(x) if ((x) == NULL) return;
#define CHECK_NULL_RETURN(x, y) if ((x) == NULL) return y;

#endif // 0

// void Java_java_net_InetAddress_init(JNIEnv *env, jclass cls);
// void Java_java_net_Inet4Address_init(JNIEnv *env, jclass cls);
// void Java_java_net_Inet6Address_init(JNIEnv *env, jclass cls);
// void Java_java_net_NetworkInterface_init(JNIEnv *env, jclass cls);
// 
// void NET_ThrowNew(JNIEnv *env, int errorNum, char *msg);
// int NET_GetError();
// 
// void NET_ThrowCurrent(JNIEnv *env, char *msg);
// 
// jfieldID NET_GetFileDescriptorID(JNIEnv *env);
 
/// \brief Returns true if IPv6 is available.
///
/// \ingroup BFC_Net

Bool ipv6_available(
);

/// \brief Returns true if IPv6 is supported.
///
/// \ingroup BFC_Net

Bool IPv6_supported(
);

// void
// NET_AllocSockaddr(struct sockaddr **him, int *len);
// 
// int
// NET_InetAddressToSockaddr(JNIEnv *env, jobject iaObj, int port, struct sockaddr *him, int *len, jboolean v4MappedAddress);
// 
// jobject
// NET_SockaddrToInetAddress(JNIEnv *env, struct sockaddr *him, int *port);

// void
// NET_SetTrafficClass(struct sockaddr *him, int trafficClass);
// 
// jint
// NET_GetPortFromSockaddr(struct sockaddr *him);
// 
// jint
// NET_SockaddrEqualsInetAddress(JNIEnv *env,struct sockaddr *him, jobject iaObj);
// 
// int
// NET_IsIPv4Mapped(jbyte* caddr);
// 
// int
// NET_IPv4MappedToIPv4(jbyte* caddr);
// 
// int
// NET_IsEqual(jbyte* caddr1, jbyte* caddr2);
// 
// /* Socket operations
//  *
//  * These work just like the JVM_* procedures, except that they may do some
//  * platform-specific pre/post processing of the arguments and/or results.
//  */
// 
// int
// NET_GetSockOpt(int fd, int level, int opt, void *result, int *len);
// 
// int
// NET_SetSockOpt(int fd, int level, int opt, const void *arg, int len);
// 
// int
// NET_Bind(int fd, struct sockaddr *him, int len);
// 
// int
// NET_MapSocketOption(jint cmd, int *level, int *optname);
// 
// int
// NET_MapSocketOptionV6(jint cmd, int *level, int *optname);
// 
// int getScopeID (struct sockaddr *);
// 
// int cmpScopeID (unsigned int, struct sockaddr *);
// 
// unsigned short in_cksum(unsigned short *addr, int len);

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Utils_H_

// ============================================================================

