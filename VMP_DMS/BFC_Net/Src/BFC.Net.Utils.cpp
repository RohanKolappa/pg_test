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
//	BFC.Net.Utils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.File.h"

#include "BFC.Net.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Bool Net::ipv6_available() {

	return IPv6_supported(); // & (!preferIPv4Stack);

}

//static int initialized = 0;
//
//void init(JNIEnv *env) {
//    if (!initialized) {
//        Java_java_net_InetAddress_init(env, 0);
//        Java_java_net_Inet4Address_init(env, 0);
//        Java_java_net_Inet6Address_init(env, 0);
//        initialized = 1;
//    }
//}
//
//jobject
//NET_SockaddrToInetAddress(JNIEnv *env, struct sockaddr *him, int *port) {
//    jobject iaObj;
//    init(env);
//#ifdef AF_INET6
//    if (him->sa_family == AF_INET6) {
//        jbyteArray ipaddress;
//#ifdef WIN32
//        struct SOCKADDR_IN6 *him6 = (struct SOCKADDR_IN6 *)him;
//#else
//        struct sockaddr_in6 *him6 = (struct sockaddr_in6 *)him;
//#endif
//        jbyte *caddr = (jbyte *)&(him6->sin6_addr);
//        if (NET_IsIPv4Mapped(caddr)) {
//            int address;
//            static jclass inet4Cls = 0;
//            if (inet4Cls == 0) {
//                jclass c = (*env)->FindClass(env, "java/net/Inet4Address");
//                CHECK_NULL_RETURN(c, NULL);
//                inet4Cls = (*env)->NewGlobalRef(env, c);
//                CHECK_NULL_RETURN(inet4Cls, NULL);
//                (*env)->DeleteLocalRef(env, c);
//            }
//            iaObj = (*env)->NewObject(env, inet4Cls, ia4_ctrID);
//            CHECK_NULL_RETURN(iaObj, NULL);
//            address = NET_IPv4MappedToIPv4(caddr);
//            (*env)->SetIntField(env, iaObj, ia_addressID, address);
//            (*env)->SetIntField(env, iaObj, ia_familyID, IPv4);
//        } else {
//            static jclass inet6Cls = 0;
//            if (inet6Cls == 0) {
//                jclass c = (*env)->FindClass(env, "java/net/Inet6Address");
//                CHECK_NULL_RETURN(c, NULL);
//                inet6Cls = (*env)->NewGlobalRef(env, c);
//                CHECK_NULL_RETURN(inet6Cls, NULL);
//                (*env)->DeleteLocalRef(env, c);
//            }
//            iaObj = (*env)->NewObject(env, inet6Cls, ia6_ctrID);
//            CHECK_NULL_RETURN(iaObj, NULL);
//            ipaddress = (*env)->NewByteArray(env, 16);
//            CHECK_NULL_RETURN(ipaddress, NULL);
//            (*env)->SetByteArrayRegion(env, ipaddress, 0, 16,
//                                       (jbyte *)&(him6->sin6_addr));
//
//            (*env)->SetObjectField(env, iaObj, ia6_ipaddressID, ipaddress);
//
//            (*env)->SetIntField(env, iaObj, ia_familyID, IPv6);
//            (*env)->SetIntField(env, iaObj, ia6_scopeidID, getScopeID(him));
//        }
//        *port = ntohs(him6->sin6_port);
//    } else
//#endif /* AF_INET6 */
//        {
//            struct sockaddr_in *him4 = (struct sockaddr_in *)him;
//            static jclass inet4Cls = 0;
//
//            if (inet4Cls == 0) {
//                jclass c = (*env)->FindClass(env, "java/net/Inet4Address");
//                CHECK_NULL_RETURN(c, NULL);
//                inet4Cls = (*env)->NewGlobalRef(env, c);
//                CHECK_NULL_RETURN(inet4Cls, NULL);
//                (*env)->DeleteLocalRef(env, c);
//            }
//            iaObj = (*env)->NewObject(env, inet4Cls, ia4_ctrID);
//            CHECK_NULL_RETURN(iaObj, NULL);
//            (*env)->SetIntField(env, iaObj, ia_familyID, IPv4);
//            (*env)->SetIntField(env, iaObj, ia_addressID,
//                                ntohl(him4->sin_addr.s_addr));
//            *port = ntohs(him4->sin_port);
//        }
//    return iaObj;
//}
//
//jint
//NET_SockaddrEqualsInetAddress(JNIEnv *env, struct sockaddr *him, jobject iaObj)
//{
//    jint family = (*env)->GetIntField(env, iaObj, ia_familyID) == IPv4?
//        AF_INET : AF_INET6;
//
//#ifdef AF_INET6
//    if (him->sa_family == AF_INET6) {
//#ifdef WIN32
//        struct SOCKADDR_IN6 *him6 = (struct SOCKADDR_IN6 *)him;
//#else
//        struct sockaddr_in6 *him6 = (struct sockaddr_in6 *)him;
//#endif
//        jbyte *caddrNew = (jbyte *)&(him6->sin6_addr);
//        if (NET_IsIPv4Mapped(caddrNew)) {
//            int addrNew;
//            int addrCur;
//            if (family == AF_INET6) {
//                return false;
//            }
//            addrNew = NET_IPv4MappedToIPv4(caddrNew);
//            addrCur = (*env)->GetIntField(env, iaObj, ia_addressID);
//            if (addrNew == addrCur) {
//                return JNI_TRUE;
//            } else {
//                return false;
//            }
//        } else {
//            jbyteArray ipaddress;
//            jbyte caddrCur[16];
//            int scope;
//
//            if (family == AF_INET) {
//                return false;
//            }
//            ipaddress = (*env)->GetObjectField(env, iaObj, ia6_ipaddressID);
//            scope = (*env)->GetIntField(env, iaObj, ia6_scopeidID);
//            (*env)->GetByteArrayRegion(env, ipaddress, 0, 16, caddrCur);
//            if (NET_IsEqual(caddrNew, caddrCur) && cmpScopeID(scope, him)) {
//                return JNI_TRUE;
//            } else {
//                return false;
//            }
//        }
//    } else
//#endif /* AF_INET6 */
//        {
//            struct sockaddr_in *him4 = (struct sockaddr_in *)him;
//            int addrNew, addrCur;
//            if (family != AF_INET) {
//                return false;
//            }
//            addrNew = ntohl(him4->sin_addr.s_addr);
//            addrCur = (*env)->GetIntField(env, iaObj, ia_addressID);
//            if (addrNew == addrCur) {
//                return JNI_TRUE;
//            } else {
//                return false;
//            }
//        }
//}
//
//unsigned short
//in_cksum(unsigned short *addr, int len) {
//    int nleft = len;
//    int sum = 0;
//    unsigned short *w = addr;
//    unsigned short answer = 0;
//    while(nleft > 1) {
//        sum += *w++;
//        nleft -= 2;
//    }
//
//    if (nleft == 1) {
//        *(unsigned char *) (&answer) = *(unsigned char *)w;
//        sum += answer;
//    }
//
//    sum = (sum >> 16) + (sum & 0xffff);
//    sum += (sum >> 16);
//    answer = ~sum;
//    return (answer);
//}

// ============================================================================

#if defined( PLATFORM_WIN32 )

Bool Net::IPv6_supported() {

	return false;

}

#else

//getaddrinfo_f getaddrinfo_ptr = NULL;
//freeaddrinfo_f freeaddrinfo_ptr = NULL;
//getnameinfo_f getnameinfo_ptr = NULL;
//
///*
// * EXCLBIND socket options only on Solaris 8 & 9.
// */
//#if defined(__solaris__) && !defined(TCP_EXCLBIND)
//#define TCP_EXCLBIND            0x21
//#endif
//#if defined(__solaris__) && !defined(UDP_EXCLBIND)
//#define UDP_EXCLBIND            0x0101
//#endif
//
//static int kernelV22 = 0;
//static int vinit = 0;
//
//int kernelIsV22 () {
//    if (!vinit) {
//        struct utsname sysinfo;
//        if (uname(&sysinfo) == 0) {
//            sysinfo.release[3] = '\0';
//            if (strcmp(sysinfo.release, "2.2") == 0) {
//                kernelV22 = JNI_TRUE;
//            }
//        }
//        vinit = 1;
//    }
//    return kernelV22;
//}
//
//static int kernelV24 = 0;
//static int vinit24 = 0;
//
//int kernelIsV24 () {
//    if (!vinit24) {
//        struct utsname sysinfo;
//        if (uname(&sysinfo) == 0) {
//            sysinfo.release[3] = '\0';
//            if (strcmp(sysinfo.release, "2.4") == 0) {
//                kernelV24 = JNI_TRUE;
//            }
//        }
//        vinit24 = 1;
//    }
//    return kernelV24;
//}
//
//int getScopeID (struct sockaddr *him) {
//    struct sockaddr_in6 *hext = (struct sockaddr_in6 *)him;
//    if (kernelIsV22()) {
//        return 0;
//    }
//    return hext->sin6_scope_id;
//}
//
//int cmpScopeID (unsigned int scope, struct sockaddr *him) {
//    struct sockaddr_in6 *hext = (struct sockaddr_in6 *)him;
//    if (kernelIsV22()) {
//        return 1;       /* scope is ignored for comparison in 2.2 kernel */
//    }
//    return hext->sin6_scope_id == scope;
//}
//
//void
//NET_ThrowByNameWithLastError(JNIEnv *env, const char *name,
//                   const char *defaultDetail) {
//    char errmsg[255];
//    sprintf(errmsg, "errno: %d, error: %s\n", errno, defaultDetail);
//    JNU_ThrowByNameWithLastError(env, name, errmsg);
//}
//
//void
//NET_ThrowCurrent(JNIEnv *env, char *msg) {
//    NET_ThrowNew(env, errno, msg);
//}
//
//void
//NET_ThrowNew(JNIEnv *env, int errorNumber, char *msg) {
//    char fullMsg[512];
//    if (!msg) {
//        msg = "no further information";
//    }
//    switch(errorNumber) {
//    case EBADF:
//        jio_snprintf(fullMsg, sizeof(fullMsg), "socket closed: %s", msg);
//        JNU_ThrowByName(env, JNU_JAVANETPKG "SocketException", fullMsg);
//        break;
//    case EINTR:
//        JNU_ThrowByName(env, JNU_JAVAIOPKG "InterruptedIOException", msg);
//        break;
//    default:
//        errno = errorNumber;
//        JNU_ThrowByNameWithLastError(env, JNU_JAVANETPKG "SocketException", msg);
//        break;
//    }
//}
//
//
//jfieldID
//NET_GetFileDescriptorID(JNIEnv *env)
//{
//    jclass cls = (*env)->FindClass(env, "java/io/FileDescriptor");
//    CHECK_NULL_RETURN(cls, NULL);
//    return (*env)->GetFieldID(env, cls, "fd", "I");
//}

Bool Net::IPv6_supported() {

#if ! defined( AF_INET6 )
	return false;
#endif

#if defined( AF_INET6 )

	int fd = ::socket( AF_INET6, SOCK_STREAM, 0 );

	if ( fd < 0 ) {
		return false;
	}

	::close( fd );

	// If fd 0 is a socket it means we've been launched from inetd or
	// xinetd. If it's a socket then check the family - if it's an
	// IPv4 socket then we need to disable IPv6.

	sockaddr sa;
	int sa_len = sizeof( sa );

	if ( ::getsockname( 0, ( sockaddr * )&sa, &sa_len ) == 0
	  && sa->sa_family != AF_INET6 ) {
		return false;
	}

	Buffer buf;

	try {
		buf = IO::File::dump( "/proc/net/if_inet6" );
	}
	catch ( Exception & ) {
		return false;
	}

	if ( buf.isEmpty() ) {
		return false;
	}

#endif /* AF_INET6 */

	return true;

}

//void
//NET_AllocSockaddr(struct sockaddr **him, int *len) {
//#ifdef AF_INET6
//    if (ipv6_available()) {
//        struct sockaddr_in6 *him6 = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6));
//        *him = (struct sockaddr*)him6;
//        *len = sizeof(struct sockaddr_in6);
//    } else
//#endif /* AF_INET6 */
//        {
//            struct sockaddr_in *him4 = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
//            *him = (struct sockaddr*)him4;
//            *len = sizeof(struct sockaddr_in);
//        }
//}
//
//#if defined(AF_INET6)
//
//
///* following code creates a list of addresses from the kernel
// * routing table that are routed via the loopback address.
// * We check all destination addresses against this table
// * and override the scope_id field to use the relevant value for "lo"
// * in order to work-around the Linux bug that prevents packets destined
// * for certain local addresses from being sent via a physical interface.
// */
//
//struct loopback_route {
//    struct in6_addr addr; /* destination address */
//    int plen; /* prefix length */
//};
//
//static struct loopback_route *loRoutes = 0;
//static int nRoutes = 0; /* number of routes */
//static int loRoutes_size = 16; /* initial size */
//static int lo_scope_id = 0;
//
//static void initLoopbackRoutes();
//
//void printAddr (struct in6_addr *addr) {
//    int i;
//    for (i=0; i<16; i++) {
//        printf ("%02x", addr->s6_addr[i]);
//    }
//    printf ("\n");
//}
//
//static jboolean needsLoopbackRoute (struct in6_addr* dest_addr) {
//    int byte_count;
//    int extra_bits, i;
//    struct loopback_route *ptr;
//
//    if (loRoutes == 0) {
//        initLoopbackRoutes();
//    }
//
//    for (ptr = loRoutes, i=0; i<nRoutes; i++, ptr++) {
//        struct in6_addr *target_addr=&ptr->addr;
//        int dest_plen = ptr->plen;
//        byte_count = dest_plen >> 3;
//        extra_bits = dest_plen & 0x3;
//
//        if (byte_count > 0) {
//            if (memcmp(target_addr, dest_addr, byte_count)) {
//                continue;  /* no match */
//            }
//        }
//
//        if (extra_bits > 0) {
//            unsigned char c1 = ((unsigned char *)target_addr)[byte_count];
//            unsigned char c2 = ((unsigned char *)&dest_addr)[byte_count];
//            unsigned char mask = 0xff << (8 - extra_bits);
//            if ((c1 & mask) != (c2 & mask)) {
//                continue;
//            }
//        }
//        return JNI_TRUE;
//    }
//    return false;
//}
//
//
//static void initLoopbackRoutes() {
//    FILE *f;
//    char srcp[8][5];
//    char hopp[8][5];
//    int dest_plen, src_plen, use, refcnt, metric;
//    unsigned long flags;
//    char dest_str[40];
//    struct in6_addr dest_addr;
//    char device[16];
//    jboolean match = false;
//
//    if (loRoutes != 0) {
//        free (loRoutes);
//    }
//    loRoutes = calloc (loRoutes_size, sizeof(struct loopback_route));
//    if (loRoutes == 0) {
//        return;
//    }
//    /*
//     * Scan /proc/net/ipv6_route looking for a matching
//     * route.
//     */
//    if ((f = fopen("/proc/net/ipv6_route", "r")) == NULL) {
//        return ;
//    }
//    while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x "
//                     "%4s%4s%4s%4s%4s%4s%4s%4s %02x "
//                     "%4s%4s%4s%4s%4s%4s%4s%4s "
//                     "%08x %08x %08x %08lx %8s",
//                     dest_str, &dest_str[5], &dest_str[10], &dest_str[15],
//                     &dest_str[20], &dest_str[25], &dest_str[30], &dest_str[35],
//                     &dest_plen,
//                     srcp[0], srcp[1], srcp[2], srcp[3],
//                     srcp[4], srcp[5], srcp[6], srcp[7],
//                     &src_plen,
//                     hopp[0], hopp[1], hopp[2], hopp[3],
//                     hopp[4], hopp[5], hopp[6], hopp[7],
//                     &metric, &use, &refcnt, &flags, device) == 31) {
//
//        /*
//         * Some routes should be ignored
//         */
//        if ( (dest_plen < 0 || dest_plen > 128)  ||
//             (src_plen != 0) ||
//             (flags & (RTF_POLICY | RTF_FLOW)) ||
//             ((flags & RTF_REJECT) && dest_plen == 0) ) {
//            continue;
//        }
//
//        /*
//         * Convert the destination address
//         */
//        dest_str[4] = ':';
//        dest_str[9] = ':';
//        dest_str[14] = ':';
//        dest_str[19] = ':';
//        dest_str[24] = ':';
//        dest_str[29] = ':';
//        dest_str[34] = ':';
//        dest_str[39] = '\0';
//
//        if (inet_pton(AF_INET6, dest_str, &dest_addr) < 0) {
//            /* not an Ipv6 address */
//            continue;
//        }
//        if (strcmp(device, "lo") != 0) {
//            /* Not a loopback route */
//            continue;
//        } else {
//            if (nRoutes == loRoutes_size) {
//                loRoutes = realloc (loRoutes, loRoutes_size *
//                                sizeof (struct loopback_route) * 2);
//                if (loRoutes == 0) {
//                    return ;
//                }
//                loRoutes_size *= 2;
//            }
//            memcpy (&loRoutes[nRoutes].addr,&dest_addr,sizeof(struct in6_addr));
//            loRoutes[nRoutes].plen = dest_plen;
//            nRoutes ++;
//        }
//    }
//
//    fclose (f);
//    {
//        /* now find the scope_id for "lo" */
//
//        char addr6[40], devname[20];
//        char addr6p[8][5];
//        int plen, scope, dad_status, if_idx;
//
//        if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
//            while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
//                      addr6p[0], addr6p[1], addr6p[2], addr6p[3],
//                      addr6p[4], addr6p[5], addr6p[6], addr6p[7],
//                  &if_idx, &plen, &scope, &dad_status, devname) == 13) {
//
//                if (strcmp(devname, "lo") == 0) {
//                    /*
//                     * Found - so just return the index
//                     */
//                    fclose(f);
//                    lo_scope_id = if_idx;
//                    return;
//                }
//            }
//            fclose(f);
//        }
//    }
//}
//
///*
// * Following is used for binding to local addresses. Equivalent
// * to code above, for bind().
// */
//
//struct localinterface {
//    int index;
//    char localaddr [16];
//};
//
//static struct localinterface *localifs = 0;
//static int localifsSize = 0;    /* size of array */
//static int nifs = 0;            /* number of entries used in array */
//
///* not thread safe: make sure called once from one thread */
//
//static void initLocalIfs () {
//    FILE *f;
//    unsigned char staddr [16];
//    char ifname [32];
//    struct localinterface *lif=0;
//    int index, x1, x2, x3;
//    unsigned int u0,u1,u2,u3,u4,u5,u6,u7,u8,u9,ua,ub,uc,ud,ue,uf;
//
//    if ((f = fopen("/proc/net/if_inet6", "r")) == NULL) {
//        return ;
//    }
//    while (fscanf (f, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x "
//                "%d %x %x %x %s",&u0,&u1,&u2,&u3,&u4,&u5,&u6,&u7,
//                &u8,&u9,&ua,&ub,&uc,&ud,&ue,&uf,
//                &index, &x1, &x2, &x3, ifname) == 21) {
//        staddr[0] = (unsigned char)u0;
//        staddr[1] = (unsigned char)u1;
//        staddr[2] = (unsigned char)u2;
//        staddr[3] = (unsigned char)u3;
//        staddr[4] = (unsigned char)u4;
//        staddr[5] = (unsigned char)u5;
//        staddr[6] = (unsigned char)u6;
//        staddr[7] = (unsigned char)u7;
//        staddr[8] = (unsigned char)u8;
//        staddr[9] = (unsigned char)u9;
//        staddr[10] = (unsigned char)ua;
//        staddr[11] = (unsigned char)ub;
//        staddr[12] = (unsigned char)uc;
//        staddr[13] = (unsigned char)ud;
//        staddr[14] = (unsigned char)ue;
//        staddr[15] = (unsigned char)uf;
//        nifs ++;
//        if (nifs > localifsSize) {
//            localifs = (struct localinterface *) realloc (
//                        localifs, sizeof (struct localinterface)* (localifsSize+5));
//            if (localifs == 0) {
//                nifs = 0;
//                fclose (f);
//                return;
//            }
//            lif = localifs + localifsSize;
//            localifsSize += 5;
//        } else {
//            lif ++;
//        }
//        memcpy (lif->localaddr, staddr, 16);
//        lif->index = index;
//    }
//    fclose (f);
//}
//
///* return the scope_id (interface index) of the
// * interface corresponding to the given address
// * returns 0 if no match found
// */
//
//static int getLocalScopeID (char *addr) {
//    struct localinterface *lif;
//    int i;
//    if (localifs == 0) {
//        initLocalIfs();
//    }
//    for (i=0, lif=localifs; i<nifs; i++, lif++) {
//        if (memcmp (addr, lif->localaddr, 16) == 0) {
//            return lif->index;
//        }
//    }
//    return 0;
//}
//
//void initLocalAddrTable () {
//    initLoopbackRoutes();
//    initLocalIfs();
//}
//
//#else
//
//void initLocalAddrTable () {}
//
//#endif
//
///* In the case of an IPv4 Inetaddress this method will return an
// * IPv4 mapped address where IPv6 is available and v4MappedAddress is TRUE.
// * Otherwise it will return a sockaddr_in structure for an IPv4 InetAddress.
//*/
//JNIEXPORT int JNICALL
//NET_InetAddressToSockaddr(JNIEnv *env, jobject iaObj, int port, struct sockaddr *him,
//                          int *len, jboolean v4MappedAddress) {
//    jint family;
//    family = (*env)->GetIntField(env, iaObj, ia_familyID);
//#ifdef AF_INET6
//    /* needs work. 1. family 2. clean up him6 etc deallocate memory */
//    if (ipv6_available() && !(family == IPv4 && v4MappedAddress == false)) {
//        struct sockaddr_in6 *him6 = (struct sockaddr_in6 *)him;
//        jbyteArray ipaddress;
//        jbyte caddr[16];
//        jint address;
//
//
//        if (family == IPv4) { /* will convert to IPv4-mapped address */
//            memset((char *) caddr, 0, 16);
//            address = (*env)->GetIntField(env, iaObj, ia_addressID);
//            if (address == INADDR_ANY) {
//                /* we would always prefer IPv6 wildcard address
//                   caddr[10] = 0xff;
//                   caddr[11] = 0xff; */
//            } else {
//                caddr[10] = 0xff;
//                caddr[11] = 0xff;
//                caddr[12] = ((address >> 24) & 0xff);
//                caddr[13] = ((address >> 16) & 0xff);
//                caddr[14] = ((address >> 8) & 0xff);
//                caddr[15] = (address & 0xff);
//            }
//        } else {
//            ipaddress = (*env)->GetObjectField(env, iaObj, ia6_ipaddressID);
//            (*env)->GetByteArrayRegion(env, ipaddress, 0, 16, caddr);
//        }
//        memset((char *)him6, 0, sizeof(struct sockaddr_in6));
//        him6->sin6_port = htons(port);
//        memcpy((void *)&(him6->sin6_addr), caddr, sizeof(struct in6_addr) );
//        him6->sin6_family = AF_INET6;
//        *len = sizeof(struct sockaddr_in6) ;
//
//        /*
//         * On Linux if we are connecting to a link-local address
//         * we need to specify the interface in the scope_id (2.4 kernel only)
//         *
//         * If the scope was cached the we use the cached value. If not cached but
//         * specified in the Inet6Address we use that, but we first check if the
//         * address needs to be routed via the loopback interface. In this case,
//         * we override the specified value with that of the loopback interface.
//         * If no cached value exists and no value was specified by user, then
//         * we try to determine a value ffrom the routing table. In all these
//         * cases the used value is cached for further use.
//         */
//        if (IN6_IS_ADDR_LINKLOCAL(&(him6->sin6_addr))) {
//            int cached_scope_id = 0, scope_id = 0;
//            int old_kernel = kernelIsV22();
//
//            if (ia6_cachedscopeidID && !old_kernel) {
//                cached_scope_id = (int)(*env)->GetIntField(env, iaObj, ia6_cachedscopeidID);
//                /* if cached value exists then use it. Otherwise, check
//                 * if scope is set in the address.
//                 */
//                if (!cached_scope_id) {
//                    if (ia6_scopeidID) {
//                        scope_id = (int)(*env)->GetIntField(env,iaObj,ia6_scopeidID);
//                    }
//                    if (scope_id != 0) {
//                        /* check user-specified value for loopback case
//                         * that needs to be overridden
//                         */
//                        if (kernelIsV24() && needsLoopbackRoute (&him6->sin6_addr)) {
//                            cached_scope_id = lo_scope_id;
//                            (*env)->SetIntField(env, iaObj, ia6_cachedscopeidID, cached_scope_id);
//                        }
//                    } else {
//                        /*
//                         * Otherwise consult the IPv6 routing tables to
//                         * try determine the appropriate interface.
//                         */
//                        if (kernelIsV24()) {
//                            cached_scope_id = getDefaultIPv6Interface( &(him6->sin6_addr) );
//                        } else {
//                            cached_scope_id = getLocalScopeID( (char *)&(him6->sin6_addr) );
//                            if (cached_scope_id == 0) {
//                                cached_scope_id = getDefaultIPv6Interface( &(him6->sin6_addr) );
//                            }
//                        }
//                        (*env)->SetIntField(env, iaObj, ia6_cachedscopeidID, cached_scope_id);
//                    }
//                }
//            }
//
//            /*
//             * If we have a scope_id use the extended form
//             * of sockaddr_in6.
//             */
//
//            if (!old_kernel) {
//                struct sockaddr_in6 *him6 =
//                        (struct sockaddr_in6 *)him;
//                him6->sin6_scope_id = cached_scope_id != 0 ?
//                                            cached_scope_id    : scope_id;
//                *len = sizeof(struct sockaddr_in6);
//            }
//        }
//    } else
//#endif /* AF_INET6 */
//        {
//            struct sockaddr_in *him4 = (struct sockaddr_in*)him;
//            jint address;
//            if (family == IPv6) {
//              JNU_ThrowByName(env, JNU_JAVANETPKG "SocketException", "Protocol family unavailable");
//              return -1;
//            }
//            memset((char *) him4, 0, sizeof(struct sockaddr_in));
//            address = (*env)->GetIntField(env, iaObj, ia_addressID);
//            him4->sin_port = htons((short) port);
//            him4->sin_addr.s_addr = (uint32_t) htonl(address);
//            him4->sin_family = AF_INET;
//            *len = sizeof(struct sockaddr_in);
//        }
//    return 0;
//}
//
//void
//NET_SetTrafficClass(struct sockaddr *him, int trafficClass) {
//#ifdef AF_INET6
//    if (him->sa_family == AF_INET6) {
//        struct sockaddr_in6 *him6 = (struct sockaddr_in6 *)him;
//        him6->sin6_flowinfo = htonl((trafficClass & 0xff) << 20);
//    }
//#endif /* AF_INET6 */
//}
//
//jint
//NET_GetPortFromSockaddr(struct sockaddr *him) {
//#ifdef AF_INET6
//    if (him->sa_family == AF_INET6) {
//        return ntohs(((struct sockaddr_in6*)him)->sin6_port);
//
//        } else
//#endif /* AF_INET6 */
//            {
//                return ntohs(((struct sockaddr_in*)him)->sin_port);
//            }
//}
//
//int
//NET_IsIPv4Mapped(jbyte* caddr) {
//    int i;
//    for (i = 0; i < 10; i++) {
//        if (caddr[i] != 0x00) {
//            return 0; /* false */
//        }
//    }
//
//    if (((caddr[10] & 0xff) == 0xff) && ((caddr[11] & 0xff) == 0xff)) {
//        return 1; /* true */
//    }
//    return 0; /* false */
//}
//
//int
//NET_IPv4MappedToIPv4(jbyte* caddr) {
//    return ((caddr[12] & 0xff) << 24) | ((caddr[13] & 0xff) << 16) | ((caddr[14] & 0xff) << 8)
//        | (caddr[15] & 0xff);
//}
//
//int
//NET_IsEqual(jbyte* caddr1, jbyte* caddr2) {
//    int i;
//    for (i = 0; i < 16; i++) {
//        if (caddr1[i] != caddr2[i]) {
//            return 0; /* false */
//        }
//    }
//    return 1;
//}
//
//jboolean NET_addrtransAvailable() {
//    return (jboolean)(getaddrinfo_ptr != NULL);
//}
//
///*
// * Map the Java level socket option to the platform specific
// * level and option name.
// */
//int
//NET_MapSocketOption(jint cmd, int *level, int *optname) {
//    static struct {
//        jint cmd;
//        int level;
//        int optname;
//    } const opts[] = {
//        { java_net_SocketOptions_TCP_NODELAY,           IPPROTO_TCP,    TCP_NODELAY },
//        { java_net_SocketOptions_SO_OOBINLINE,          SOL_SOCKET,     SO_OOBINLINE },
//        { java_net_SocketOptions_SO_LINGER,             SOL_SOCKET,     SO_LINGER },
//        { java_net_SocketOptions_SO_SNDBUF,             SOL_SOCKET,     SO_SNDBUF },
//        { java_net_SocketOptions_SO_RCVBUF,             SOL_SOCKET,     SO_RCVBUF },
//        { java_net_SocketOptions_SO_KEEPALIVE,          SOL_SOCKET,     SO_KEEPALIVE },
//        { java_net_SocketOptions_SO_REUSEADDR,          SOL_SOCKET,     SO_REUSEADDR },
//        { java_net_SocketOptions_SO_BROADCAST,          SOL_SOCKET,     SO_BROADCAST },
//        { java_net_SocketOptions_IP_TOS,                IPPROTO_IP,     IP_TOS },
//        { java_net_SocketOptions_IP_MULTICAST_IF,       IPPROTO_IP,     IP_MULTICAST_IF },
//        { java_net_SocketOptions_IP_MULTICAST_IF2,      IPPROTO_IP,     IP_MULTICAST_IF },
//        { java_net_SocketOptions_IP_MULTICAST_LOOP,     IPPROTO_IP,     IP_MULTICAST_LOOP },
//    };
//
//    int i;
//
//    /*
//     * Different multicast options if IPv6 is enabled
//     */
//#ifdef AF_INET6
//    if (ipv6_available()) {
//        switch (cmd) {
//            case java_net_SocketOptions_IP_MULTICAST_IF:
//            case java_net_SocketOptions_IP_MULTICAST_IF2:
//                *level = IPPROTO_IPV6;
//                *optname = IPV6_MULTICAST_IF;
//                return 0;
//
//            case java_net_SocketOptions_IP_MULTICAST_LOOP:
//                *level = IPPROTO_IPV6;
//                *optname = IPV6_MULTICAST_LOOP;
//                return 0;
//        }
//    }
//#endif
//
//    /*
//     * Map the Java level option to the native level
//     */
//    for (i=0; i<(int)(sizeof(opts) / sizeof(opts[0])); i++) {
//        if (cmd == opts[i].cmd) {
//            *level = opts[i].level;
//            *optname = opts[i].optname;
//            return 0;
//        }
//    }
//
//    /* not found */
//    return -1;
//}
//
///*
// * Determine the default interface for an IPv6 address.
// *
// * 1. Scans /proc/net/ipv6_route for a matching route
// *    (eg: fe80::/10 or a route for the specific address).
// *    This will tell us the interface to use (eg: "eth0").
// *
// * 2. Lookup /proc/net/if_inet6 to map the interface
// *    name to an interface index.
// *
// * Returns :-
// *      -1 if error
// *       0 if no matching interface
// *      >1 interface index to use for the link-local address.
// */
//#if defined(AF_INET6)
//int getDefaultIPv6Interface(struct in6_addr *target_addr) {
//    FILE *f;
//    char srcp[8][5];
//    char hopp[8][5];
//    int dest_plen, src_plen, use, refcnt, metric;
//    unsigned long flags;
//    char dest_str[40];
//    struct in6_addr dest_addr;
//    char device[16];
//    jboolean match = false;
//
//    /*
//     * Scan /proc/net/ipv6_route looking for a matching
//     * route.
//     */
//    if ((f = fopen("/proc/net/ipv6_route", "r")) == NULL) {
//        return -1;
//    }
//    while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x "
//                     "%4s%4s%4s%4s%4s%4s%4s%4s %02x "
//                     "%4s%4s%4s%4s%4s%4s%4s%4s "
//                     "%08x %08x %08x %08lx %8s",
//                     dest_str, &dest_str[5], &dest_str[10], &dest_str[15],
//                     &dest_str[20], &dest_str[25], &dest_str[30], &dest_str[35],
//                     &dest_plen,
//                     srcp[0], srcp[1], srcp[2], srcp[3],
//                     srcp[4], srcp[5], srcp[6], srcp[7],
//                     &src_plen,
//                     hopp[0], hopp[1], hopp[2], hopp[3],
//                     hopp[4], hopp[5], hopp[6], hopp[7],
//                     &metric, &use, &refcnt, &flags, device) == 31) {
//
//        /*
//         * Some routes should be ignored
//         */
//        if ( (dest_plen < 0 || dest_plen > 128)  ||
//             (src_plen != 0) ||
//             (flags & (RTF_POLICY | RTF_FLOW)) ||
//             ((flags & RTF_REJECT) && dest_plen == 0) ) {
//            continue;
//        }
//
//        /*
//         * Convert the destination address
//         */
//        dest_str[4] = ':';
//        dest_str[9] = ':';
//        dest_str[14] = ':';
//        dest_str[19] = ':';
//        dest_str[24] = ':';
//        dest_str[29] = ':';
//        dest_str[34] = ':';
//        dest_str[39] = '\0';
//
//        if (inet_pton(AF_INET6, dest_str, &dest_addr) < 0) {
//            /* not an Ipv6 address */
//            continue;
//        } else {
//            /*
//             * The prefix len (dest_plen) indicates the number of bits we
//             * need to match on.
//             *
//             * dest_plen / 8    => number of bytes to match
//             * dest_plen % 8    => number of additional bits to match
//             *
//             * eg: fe80::/10 => match 1 byte + 2 additional bits in the
//             *                  the next byte.
//             */
//            int byte_count = dest_plen >> 3;
//            int extra_bits = dest_plen & 0x3;
//
//            if (byte_count > 0) {
//                if (memcmp(target_addr, &dest_addr, byte_count)) {
//                    continue;  /* no match */
//                }
//            }
//
//            if (extra_bits > 0) {
//                unsigned char c1 = ((unsigned char *)target_addr)[byte_count];
//                unsigned char c2 = ((unsigned char *)&dest_addr)[byte_count];
//                unsigned char mask = 0xff << (8 - extra_bits);
//                if ((c1 & mask) != (c2 & mask)) {
//                    continue;
//                }
//            }
//
//            /*
//             * We have a match
//             */
//            match = JNI_TRUE;
//            break;
//        }
//    }
//    fclose(f);
//
//    /*
//     * If there's a match then we lookup the interface
//     * index.
//     */
//    if (match) {
//        char addr6[40], devname[20];
//        char addr6p[8][5];
//        int plen, scope, dad_status, if_idx;
//
//        if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
//            while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
//                      addr6p[0], addr6p[1], addr6p[2], addr6p[3],
//                      addr6p[4], addr6p[5], addr6p[6], addr6p[7],
//                  &if_idx, &plen, &scope, &dad_status, devname) == 13) {
//
//                if (strcmp(devname, device) == 0) {
//                    /*
//                     * Found - so just return the index
//                     */
//                    fclose(f);
//                    return if_idx;
//                }
//            }
//            fclose(f);
//        } else {
//            /*
//             * Couldn't open /proc/net/if_inet6
//             */
//            return -1;
//        }
//    }
//
//    /*
//     * If we get here it means we didn't there wasn't any
//     * route or we couldn't get the index of the interface.
//     */
//    return 0;
//}
//#endif
//
//
///*
// * Wrapper for getsockopt system routine - does any necessary
// * pre/post processing to deal with OS specific oddies :-
// *
// * IP_TOS is a no-op with IPv6 sockets as it's setup when
// * the connection is established.
// *
// * On Linux the SO_SNDBUF/SO_RCVBUF values must be post-processed
// * to compensate for an incorrect value returned by the kernel.
// */
//int
//NET_GetSockOpt(int fd, int level, int opt, void *result,
//               int *len)
//{
//    int rv;
//
//#ifdef AF_INET6
//    if ((level == IPPROTO_IP) && (opt == IP_TOS)) {
//        if (ipv6_available()) {
//
//            /*
//             * For IPv6 socket option implemented at Java-level
//             * so return -1.
//             */
//            int *tc = (int *)result;
//            *tc = -1;
//            return 0;
//        }
//    }
//#endif
//
//    rv = getsockopt(fd, level, opt, result, len);
//    if (rv < 0) {
//        return rv;
//    }
//
//    /*
//     * On Linux SO_SNDBUF/SO_RCVBUF aren't symmetric. This
//     * stems from additional socket structures in the send
//     * and receive buffers.
//     */
//    if ((level == SOL_SOCKET) && ((opt == SO_SNDBUF)
//                                  || (opt == SO_RCVBUF))) {
//        int n = *((int *)result);
//        n /= 2;
//        *((int *)result) = n;
//    }
//
//    return rv;
//}
//
//
///*
// * Wrapper for setsockopt system routine - performs any
// * necessary pre/post processing to deal with OS specific
// * issue :-
// *
// * On Solaris need to limit the suggested value for SO_SNDBUF
// * and SO_RCVBUF to the kernel configured limit
// *
// * For IP_TOS socket option need to mask off bits as this
// * aren't automatically masked by the kernel and results in
// * an error. In addition IP_TOS is a noop with IPv6 as it
// * should be setup as connection time.
// */
//int
//NET_SetSockOpt(int fd, int level, int  opt, const void *arg,
//               int len)
//{
//#ifndef IPTOS_TOS_MASK
//#define IPTOS_TOS_MASK 0x1e
//#endif
//#ifndef IPTOS_PREC_MASK
//#define IPTOS_PREC_MASK 0xe0
//#endif
//
//    /*
//     * IPPROTO/IP_TOS :-
//     * 1. IPv6 on Solaris: no-op and will be set in flowinfo
//     *    field when connecting TCP socket, or sending
//     *    UDP packet.
//     * 2. IPv6 on Linux: By default Linux ignores flowinfo
//     *    field so enable IPV6_FLOWINFO_SEND so that flowinfo
//     *    will be examined.
//     * 3. IPv4: set socket option based on ToS and Precedence
//     *    fields (otherwise get invalid argument)
//     */
//    if (level == IPPROTO_IP && opt == IP_TOS) {
//        int *iptos;
//
//#if defined(AF_INET6)
//        if (ipv6_available()) {
//            int optval = 1;
//            return setsockopt(fd, IPPROTO_IPV6, IPV6_FLOWINFO_SEND,
//                              (void *)&optval, sizeof(optval));
//        }
//#endif
//
//        iptos = (int *)arg;
//        *iptos &= (IPTOS_TOS_MASK | IPTOS_PREC_MASK);
//    }
//
//    /*
//     * SOL_SOCKET/{SO_SNDBUF,SO_RCVBUF} - On Solaris need to
//     * ensure that value is <= max_buf as otherwise we get
//     * an invalid argument.
//     */
//
//    /*
//     * On Linux the receive buffer is used for both socket
//     * structures and the the packet payload. The implication
//     * is that if SO_RCVBUF is too small then small packets
//     * must be discard.
//     */
//    if (level == SOL_SOCKET && opt == SO_RCVBUF) {
//        int *bufsize = (int *)arg;
//        if (*bufsize < 1024) {
//            *bufsize = 1024;
//        }
//    }
//
//    return setsockopt(fd, level, opt, arg, len);
//}
//
///*
// * Wrapper for bind system call - performs any necessary pre/post
// * processing to deal with OS specific issues :-
// *
// * Linux allows a socket to bind to 127.0.0.255 which must be
// * caught.
// *
// * On Solaris 8/9 with IPv6 enabled we must use an exclusive
// * bind to guaranteed a unique port number across the IPv4 and
// * IPv6 port spaces.
// *
// */
//int
//NET_Bind(int fd, struct sockaddr *him, int len)
//{
//    int rv;
//
//    /*
//     * ## get bugId for this issue - goes back to 1.2.2 port ##
//     * ## When IPv6 is enabled this will be an IPv4-mapped
//     * ## with family set to AF_INET6
//     */
//    if (him->sa_family == AF_INET) {
//        struct sockaddr_in *sa = (struct sockaddr_in *)him;
//        if ((ntohl(sa->sin_addr.s_addr) & 0x7f0000ff) == 0x7f0000ff) {
//            errno = EADDRNOTAVAIL;
//            return -1;
//        }
//    }
//
//    rv = bind(fd, him, len);
//
//    return rv;
//}
//
///**
// * Wrapper for select/poll with timeout on a single file descriptor.
// *
// * flags (defined in net_util_md.h can be any combination of
// * NET_WAIT_READ, NET_WAIT_WRITE & NET_WAIT_CONNECT.
// *
// * The function will return when either the socket is ready for one
// * of the specified operation or the timeout expired.
// *
// * It returns the time left from the timeout, or -1 if it expired.
// */
//
//jint
//NET_Wait(JNIEnv *env, jint fd, jint flags, jint timeout)
//{
//    jlong prevTime = JVM_CurrentTimeMillis(env, 0);
//    jint read_rv;
//
//    while (1) {
//        jlong newTime;
//#ifndef USE_SELECT
//        {
//          struct pollfd pfd;
//          pfd.fd = fd;
//          pfd.events = 0;
//          if (flags & NET_WAIT_READ)
//            pfd.events |= POLLIN;
//          if (flags & NET_WAIT_WRITE)
//            pfd.events |= POLLOUT;
//          if (flags & NET_WAIT_CONNECT)
//            pfd.events |= POLLOUT;
//
//          errno = 0;
//          read_rv = NET_Poll(&pfd, 1, timeout);
//        }
//#else
//        {
//          fd_set rd, wr, ex;
//          struct timeval t;
//
//          t.tv_sec = timeout / 1000;
//          t.tv_usec = (timeout % 1000) * 1000;
//
//          FD_ZERO(&rd);
//          FD_ZERO(&wr);
//          FD_ZERO(&ex);
//          if (flags & NET_WAIT_READ) {
//            FD_SET(fd, &rd);
//          }
//          if (flags & NET_WAIT_WRITE) {
//            FD_SET(fd, &wr);
//          }
//          if (flags & NET_WAIT_CONNECT) {
//            FD_SET(fd, &wr);
//            FD_SET(fd, &ex);
//          }
//
//          errno = 0;
//          read_rv = NET_Select(fd+1, &rd, &wr, &ex, &t);
//        }
//#endif
//
//        if (read_rv > 0) {
//          break;
//        }
//
//        newTime = JVM_CurrentTimeMillis(env, 0);
//        timeout -= (newTime - prevTime);
//        if (timeout <= 0) {
//          return read_rv > 0 ? 0 : -1;
//        }
//        newTime = prevTime;
//
//        if (read_rv > 0) {
//          break;
//        }
//
//
//      } /* while */
//
//    return timeout;
//}

#endif // defined( PLATFORM_LINUX )

// ============================================================================

#if defined( PLATFORM_WIN32 )

//#include <winsock2.h>
//#include <ws2tcpip.h>
//
//#include "net_util.h"
//#include "jni.h"
//#include "typedefs.h"
//
//#ifndef IPTOS_TOS_MASK
//#define IPTOS_TOS_MASK 0x1e
//#endif
//#ifndef IPTOS_PREC_MASK
//#define IPTOS_PREC_MASK 0xe0
//#endif
//
///* true if SO_RCVTIMEO is supported */
//jboolean isRcvTimeoutSupported = JNI_TRUE;
//
//LPFN_GETADDRINFO getaddrinfo_ptr = NULL;
//LPFN_FREEADDRINFO freaddrinfo_ptr = NULL;
//LPFN_GETNAMEINFO getnameinfo_ptr = NULL;
//
///*
// * Table of Windows Sockets errors, the specific exception we
// * throw for the error, and the error text.
// *
// * Note that this table excludes OS dependent errors.
// *
// * Latest list of Windows Sockets errors can be found at :-
// * http://msdn.microsoft.com/library/psdk/winsock/errors_3wc2.htm
// */
//static struct {
//    int errCode;
//    const char *exc;
//    const char *errString;
//} const winsock_errors[] = {
//    { WSAEACCES,                0,      "Permission denied" },
//    { WSAEADDRINUSE,            "BindException",        "Address already in use" },
//    { WSAEADDRNOTAVAIL,         "BindException",        "Cannot assign requested address" },
//    { WSAEAFNOSUPPORT,          0,      "Address family not supported by protocol family" },
//    { WSAEALREADY,              0,      "Operation already in progress" },
//    { WSAECONNABORTED,          0,      "Software caused connection abort" },
//    { WSAECONNREFUSED,          "ConnectException",     "Connection refused" },
//    { WSAECONNRESET,            0,      "Connection reset by peer" },
//    { WSAEDESTADDRREQ,          0,      "Destination address required" },
//    { WSAEFAULT,                0,      "Bad address" },
//    { WSAEHOSTDOWN,             0,      "Host is down" },
//    { WSAEHOSTUNREACH,          "NoRouteToHostException",       "No route to host" },
//    { WSAEINPROGRESS,           0,      "Operation now in progress" },
//    { WSAEINTR,                 0,      "Interrupted function call" },
//    { WSAEINVAL,                0,      "Invalid argument" },
//    { WSAEISCONN,               0,      "Socket is already connected" },
//    { WSAEMFILE,                0,      "Too many open files" },
//    { WSAEMSGSIZE,              0,      "The message is larger than the maximum supported by the underlying transport" },
//    { WSAENETDOWN,              0,      "Network is down" },
//    { WSAENETRESET,             0,      "Network dropped connection on reset" },
//    { WSAENETUNREACH,           0,      "Network is unreachable" },
//    { WSAENOBUFS,               0,      "No buffer space available (maximum connections reached?)" },
//    { WSAENOPROTOOPT,           0,      "Bad protocol option" },
//    { WSAENOTCONN,              0,      "Socket is not connected" },
//    { WSAENOTSOCK,              0,      "Socket operation on nonsocket" },
//    { WSAEOPNOTSUPP,            0,      "Operation not supported" },
//    { WSAEPFNOSUPPORT,          0,      "Protocol family not supported" },
//    { WSAEPROCLIM,              0,      "Too many processes" },
//    { WSAEPROTONOSUPPORT,       0,      "Protocol not supported" },
//    { WSAEPROTOTYPE,            0,      "Protocol wrong type for socket" },
//    { WSAESHUTDOWN,             0,      "Cannot send after socket shutdown" },
//    { WSAESOCKTNOSUPPORT,       0,      "Socket type not supported" },
//    { WSAETIMEDOUT,             "ConnectException",     "Connection timed out" },
//    { WSATYPE_NOT_FOUND,        0,      "Class type not found" },
//    { WSAEWOULDBLOCK,           0,      "Resource temporarily unavailable" },
//    { WSAHOST_NOT_FOUND,        0,      "Host not found" },
//    { WSA_NOT_ENOUGH_MEMORY,    0,      "Insufficient memory available" },
//    { WSANOTINITIALISED,        0,      "Successful WSAStartup not yet performed" },
//    { WSANO_DATA,               0,      "Valid name, no data record of requested type" },
//    { WSANO_RECOVERY,           0,      "This is a nonrecoverable error" },
//    { WSASYSNOTREADY,           0,      "Network subsystem is unavailable" },
//    { WSATRY_AGAIN,             0,      "Nonauthoritative host not found" },
//    { WSAVERNOTSUPPORTED,       0,      "Winsock.dll version out of range" },
//    { WSAEDISCON,               0,      "Graceful shutdown in progress" },
//    { WSA_OPERATION_ABORTED,    0,      "Overlapped operation aborted" },
//};
//
///*
// * Initialize Windows Sockets API support
// */
//BOOL WINAPI
//DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
//{
//    WSADATA wsadata;
//
//    switch (reason) {
//        case DLL_PROCESS_ATTACH:
//            if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
//                return FALSE;
//            }
//            break;
//
//        case DLL_PROCESS_DETACH:
//            WSACleanup();
//            break;
//
//        default:
//            break;
//    }
//    return TRUE;
//}
//
//void initLocalAddrTable () {}
//
///*
// * Since winsock doesn't have the equivalent of strerror(errno)
// * use table to lookup error text for the error.
// */
//JNIEXPORT void JNICALL
//NET_ThrowNew(JNIEnv *env, int errorNum, char *msg)
//{
//    int i;
//    int table_size = sizeof(winsock_errors) /
//                     sizeof(winsock_errors[0]);
//    char exc[256];
//    char fullMsg[256];
//    char *excP = NULL;
//
//    /*
//     * If exception already throw then don't overwrite it.
//     */
//    if ((*env)->ExceptionOccurred(env)) {
//        return;
//    }
//
//    /*
//     * Default message text if not provided
//     */
//    if (!msg) {
//        msg = "no further information";
//    }
//
//    /*
//     * Check table for known winsock errors
//     */
//    i=0;
//    while (i < table_size) {
//        if (errorNum == winsock_errors[i].errCode) {
//            break;
//        }
//        i++;
//    }
//
//    /*
//     * If found get pick the specific exception and error
//     * message corresponding to this error.
//     */
//    if (i < table_size) {
//        excP = (char *)winsock_errors[i].exc;
//        jio_snprintf(fullMsg, sizeof(fullMsg), "%s: %s",
//                     (char *)winsock_errors[i].errString, msg);
//    } else {
//        jio_snprintf(fullMsg, sizeof(fullMsg),
//                     "Unrecognized Windows Sockets error: %d: %s",
//                     errorNum, msg);
//
//    }
//
//    /*
//     * Throw SocketException if no specific exception for this
//     * error.
//     */
//    if (excP == NULL) {
//        excP = "SocketException";
//    }
//    sprintf(exc, "%s%s", JNU_JAVANETPKG, excP);
//    JNU_ThrowByName(env, exc, fullMsg);
//}
//
//void
//NET_ThrowCurrent(JNIEnv *env, char *msg)
//{
//    NET_ThrowNew(env, WSAGetLastError(), msg);
//}
//
//void
//NET_ThrowSocketException(JNIEnv *env, char* msg)
//{
//    static jclass cls = NULL;
//    if (cls == NULL) {
//        cls = (*env)->FindClass(env, "java/net/SocketException");
//        CHECK_NULL(cls);
//        cls = (*env)->NewGlobalRef(env, cls);
//        CHECK_NULL(cls);
//    }
//    (*env)->ThrowNew(env, cls, msg);
//}
//
//void
//NET_ThrowByNameWithLastError(JNIEnv *env, const char *name,
//                   const char *defaultDetail) {
//    char errmsg[255];
//    sprintf(errmsg, "errno: %d, error: %s\n", WSAGetLastError(), defaultDetail);
//    JNU_ThrowByNameWithLastError(env, name, errmsg);
//}
//
//jfieldID
//NET_GetFileDescriptorID(JNIEnv *env)
//{
//    jclass cls = (*env)->FindClass(env, "java/io/FileDescriptor");
//    CHECK_NULL_RETURN(cls, NULL);
//    return (*env)->GetFieldID(env, cls, "fd", "I");
//}
//
//jint  IPv6_supported()
//{
//    HMODULE lib;
//    int fd = socket(AF_INET6, SOCK_STREAM, 0) ;
//    if (fd < 0) {
//        return false;
//    }
//    closesocket (fd);
//
//    if ((lib = LoadLibrary ("ws2_32.dll")) == NULL) {
//        return false;
//    }
//    if ((getaddrinfo_ptr = (LPFN_GETADDRINFO)GetProcAddress (lib, "getaddrinfo")) == NULL) {
//        FreeLibrary (lib);
//        return false;
//    }
//    if ((freeaddrinfo_ptr = (LPFN_FREEADDRINFO)GetProcAddress (lib, "freeaddrinfo")) == NULL) {
//        FreeLibrary (lib);
//        return false;
//    }
//    if ((getnameinfo_ptr = (LPFN_GETNAMEINFO)GetProcAddress (lib, "getnameinfo")) == NULL) {
//        FreeLibrary (lib);
//        return false;
//    }
//    FreeLibrary(lib);
//
//    return JNI_TRUE;
//}
//
//jboolean NET_addrtransAvailable() {
//    return (jboolean)(getaddrinfo_ptr != NULL);
//}
//
//
///*
// * Return the default TOS value
// */
//int NET_GetDefaultTOS() {
//    static int default_tos = -1;
//    OSVERSIONINFO ver;
//    HKEY hKey;
//    LONG ret;
//
//    /*
//     * If default ToS already determined then return it
//     */
//    if (default_tos >= 0) {
//        return default_tos;
//    }
//
//    /*
//     * Assume default is "normal service"
//     */
//    default_tos = 0;
//
//    /*
//     * Which OS is this?
//     */
//    ver.dwOSVersionInfoSize = sizeof(ver);
//    GetVersionEx(&ver);
//
//    /*
//     * If 2000 or greater then no default ToS in registry
//     */
//    if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT) {
//        if (ver.dwMajorVersion >= 5) {
//            return default_tos;
//        }
//    }
//
//    /*
//     * Query the registry to see if a Default ToS has been set.
//     * Different registry entry for NT vs 95/98/ME.
//     */
//    if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT) {
//        ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
//                           "SYSTEM\\CurrentControlSet\\Services\\Tcp\\Parameters",
//                           0, KEY_READ, (PHKEY)&hKey);
//    } else {
//        ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
//                           "SYSTEM\\CurrentControlSet\\Services\\VxD\\MSTCP\\Parameters",
//                           0, KEY_READ, (PHKEY)&hKey);
//    }
//    if (ret == ERROR_SUCCESS) {
//        DWORD dwLen;
//        DWORD dwDefaultTOS;
//        ULONG ulType;
//        dwLen = sizeof(dwDefaultTOS);
//
//        ret = RegQueryValueEx(hKey, "DefaultTOS",  NULL, &ulType,
//                             (LPBYTE)&dwDefaultTOS, &dwLen);
//        RegCloseKey(hKey);
//        if (ret == ERROR_SUCCESS) {
//            default_tos = (int)dwDefaultTOS;
//        }
//    }
//    return default_tos;
//}
//
///* call NET_MapSocketOptionV6 for the IPv6 fd only
// * and NET_MapSocketOption for the IPv4 fd
// */
//JNIEXPORT int JNICALL
//NET_MapSocketOptionV6(jint cmd, int *level, int *optname) {
//
//    switch (cmd) {
//        case java_net_SocketOptions_IP_MULTICAST_IF:
//        case java_net_SocketOptions_IP_MULTICAST_IF2:
//            *level = IPPROTO_IPV6;
//            *optname = IPV6_MULTICAST_IF;
//            return 0;
//
//        case java_net_SocketOptions_IP_MULTICAST_LOOP:
//            *level = IPPROTO_IPV6;
//            *optname = IPV6_MULTICAST_LOOP;
//            return 0;
//    }
//    return NET_MapSocketOption (cmd, level, optname);
//}
//
///*
// * Map the Java level socket option to the platform specific
// * level and option name.
// */
//
//JNIEXPORT int JNICALL
//NET_MapSocketOption(jint cmd, int *level, int *optname) {
//
//    typedef struct {
//        jint cmd;
//        int level;
//        int optname;
//    } sockopts;
//
//    static sockopts opts[] = {
//        { java_net_SocketOptions_TCP_NODELAY,   IPPROTO_TCP,    TCP_NODELAY },
//        { java_net_SocketOptions_SO_OOBINLINE,  SOL_SOCKET,     SO_OOBINLINE },
//        { java_net_SocketOptions_SO_LINGER,     SOL_SOCKET,     SO_LINGER },
//        { java_net_SocketOptions_SO_SNDBUF,     SOL_SOCKET,     SO_SNDBUF },
//        { java_net_SocketOptions_SO_RCVBUF,     SOL_SOCKET,     SO_RCVBUF },
//        { java_net_SocketOptions_SO_KEEPALIVE,  SOL_SOCKET,     SO_KEEPALIVE },
//        { java_net_SocketOptions_SO_REUSEADDR,  SOL_SOCKET,     SO_REUSEADDR },
//        { java_net_SocketOptions_SO_BROADCAST,  SOL_SOCKET,     SO_BROADCAST },
//        { java_net_SocketOptions_IP_MULTICAST_IF,   IPPROTO_IP, IP_MULTICAST_IF },
//        { java_net_SocketOptions_IP_MULTICAST_LOOP, IPPROTO_IP, IP_MULTICAST_LOOP },
//        { java_net_SocketOptions_IP_TOS,            IPPROTO_IP, IP_TOS },
//
//    };
//
//
//    int i;
//
//    /*
//     * Map the Java level option to the native level
//     */
//    for (i=0; i<(int)(sizeof(opts) / sizeof(opts[0])); i++) {
//        if (cmd == opts[i].cmd) {
//            *level = opts[i].level;
//            *optname = opts[i].optname;
//            return 0;
//        }
//    }
//
//    /* not found */
//    return -1;
//}
//
//
///*
// * Wrapper for setsockopt dealing with Windows specific issues :-
// *
// * IP_TOS and IP_MULTICAST_LOOP can't be set on some Windows
// * editions.
// *
// * The value for the type-of-service (TOS) needs to be masked
// * to get consistent behaviour with other operating systems.
// */
//JNIEXPORT int JNICALL
//NET_SetSockOpt(int s, int level, int optname, const void *optval,
//               int optlen)
//{
//    int rv;
//
//    if (level == IPPROTO_IP && optname == IP_TOS) {
//        int *tos = (int *)optval;
//        *tos &= (IPTOS_TOS_MASK | IPTOS_PREC_MASK);
//    }
//
//    rv = setsockopt(s, level, optname, optval, optlen);
//
//    if (rv == SOCKET_ERROR) {
//        /*
//         * IP_TOS & IP_MULTICAST_LOOP can't be set on some versions
//         * of Windows.
//         */
//        if ((WSAGetLastError() == WSAENOPROTOOPT) &&
//            (level == IPPROTO_IP) &&
//            (optname == IP_TOS || optname == IP_MULTICAST_LOOP)) {
//            rv = 0;
//        }
//
//        /*
//         * IP_TOS can't be set on unbound UDP sockets.
//         */
//        if ((WSAGetLastError() == WSAEINVAL) &&
//            (level == IPPROTO_IP) &&
//            (optname == IP_TOS)) {
//            rv = 0;
//        }
//    }
//
//    return rv;
//}
//
///*
// * Wrapper for setsockopt dealing with Windows specific issues :-
// *
// * IP_TOS is not supported on some versions of Windows so
// * instead return the default value for the OS.
// */
//JNIEXPORT int JNICALL
//NET_GetSockOpt(int s, int level, int optname, void *optval,
//               int *optlen)
//{
//    int rv;
//
//    rv = getsockopt(s, level, optname, optval, optlen);
//
//
//    /*
//     * IPPROTO_IP/IP_TOS is not supported on some Windows
//     * editions so return the default type-of-service
//     * value.
//     */
//    if (rv == SOCKET_ERROR) {
//
//        if (WSAGetLastError() == WSAENOPROTOOPT &&
//            level == IPPROTO_IP && optname == IP_TOS) {
//
//            int *tos;
//            tos = (int *)optval;
//            *tos = NET_GetDefaultTOS();
//
//            rv = 0;
//        }
//    }
//
//    return rv;
//}
//
///*
// * Wrapper for bind winsock call - transparent converts an
// * error related to binding to a port that has exclusive access
// * into an error indicating the port is in use (facilitates
// * better error reporting).
// */
//JNIEXPORT int JNICALL
//NET_Bind(int s, struct sockaddr *him, int len)
//{
//    int rv = bind(s, him, len);
//
//    if (rv == SOCKET_ERROR) {
//        /*
//         * If bind fails with WSAEACCES it means that a privileged
//         * process has done an exclusive bind (NT SP4/2000/XP only).
//         */
//        if (WSAGetLastError() == WSAEACCES) {
//            WSASetLastError(WSAEADDRINUSE);
//        }
//    }
//
//    return rv;
//}
//
//JNIEXPORT int JNICALL
//NET_SocketClose(int fd) {
//    struct linger l;
//    int ret;
//    int len = sizeof (l);
//    if (getsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&l, &len) == 0) {
//        if (l.l_onoff == 0) {
//            WSASendDisconnect(fd, NULL);
//        }
//    }
//    ret = closesocket (fd);
//    return ret;
//}
//
//JNIEXPORT int JNICALL
//NET_Timeout(int fd, long timeout) {
//    int ret;
//    fd_set tbl;
//    struct timeval t;
//    t.tv_sec = timeout / 1000;
//    t.tv_usec = (timeout % 1000) * 1000;
//    FD_ZERO(&tbl);
//    FD_SET(fd, &tbl);
//    ret = select (fd + 1, &tbl, 0, 0, &t);
//    return ret;
//}
//
//
///*
// * differs from NET_Timeout() as follows:
// *
// * If timeout = -1, it blocks forever.
// *
// * returns 1 or 2 depending if only one or both sockets
// * fire at same time.
// *
// * *fdret is (one of) the active fds. If both sockets
// * fire at same time, *fdret = fd always.
// */
//JNIEXPORT int JNICALL
//NET_Timeout2(int fd, int fd1, long timeout, int *fdret) {
//    int ret;
//    fd_set tbl;
//    struct timeval t, *tP = &t;
//    if (timeout == -1) {
//        tP = 0;
//    } else {
//        t.tv_sec = timeout / 1000;
//        t.tv_usec = (timeout % 1000) * 1000;
//    }
//    FD_ZERO(&tbl);
//    FD_SET(fd, &tbl);
//    FD_SET(fd1, &tbl);
//    ret = select (0, &tbl, 0, 0, tP);
//    switch (ret) {
//    case 0:
//        return 0; /* timeout */
//    case 1:
//        if (FD_ISSET (fd, &tbl)) {
//            *fdret= fd;
//        } else {
//            *fdret= fd1;
//        }
//        return 1;
//    case 2:
//        *fdret= fd;
//        return 2;
//    }
//    return -1;
//}
//
//
//void dumpAddr (char *str, void *addr) {
//    struct SOCKADDR_IN6 *a = (struct SOCKADDR_IN6 *)addr;
//    int family = a->sin6_family;
//    printf ("%s\n", str);
//    if (family == AF_INET) {
//        struct sockaddr_in *him = (struct sockaddr_in *)addr;
//        printf ("AF_INET: port %d: %x\n", ntohs(him->sin_port),
//                                          ntohl(him->sin_addr.s_addr));
//    } else {
//        int i;
//        struct in6_addr *in = &a->sin6_addr;
//        printf ("AF_INET6 ");
//        printf ("port %d ", ntohs (a->sin6_port));
//        printf ("flow %d ", a->sin6_flowinfo);
//        printf ("addr ");
//        for (i=0; i<7; i++) {
//            printf ("%04x:", ntohs(in->s6_words[i]));
//        }
//        printf ("%04x", ntohs(in->s6_words[7]));
//        printf (" scope %d\n", a->sin6_scope_id);
//    }
//}
//
///* Macro, which cleans-up the iv6bind structure,
// * closes the two sockets (if open),
// * and returns SOCKET_ERROR. Used in NET_BindV6 only.
// */

#define CLOSE_SOCKETS_AND_RETURN {      \
    if (fd != -1) {                     \
        closesocket (fd);               \
        fd = -1;                        \
    }                                   \
    if (ofd != -1) {                    \
        closesocket (ofd);              \
        ofd = -1;                       \
    }                                   \
    if (close_fd != -1) {               \
        closesocket (close_fd);         \
        close_fd = -1;                  \
    }                                   \
    if (close_ofd != -1) {              \
        closesocket (close_ofd);        \
        close_ofd = -1;                 \
    }                                   \
    b->ipv4_fd = b->ipv6_fd = -1;       \
    return SOCKET_ERROR;                \
}

///*
// * if ipv6 is available, call NET_BindV6 to bind to the required address/port.
// * Because the same port number may need to be reserved in both v4 and v6 space,
// * this may require socket(s) to be re-opened. Therefore, all of this information
// * is passed in and returned through the ipv6bind structure.
// *
// * If the request is to bind to a specific address, then this (by definition) means
// * only bind in either v4 or v6, and this is just the same as normal. ie. a single
// * call to bind() will suffice. The other socket is closed in this case.
// *
// * The more complicated case is when the requested address is ::0 or 0.0.0.0.
// *
// * Two further cases:
// * 2. If the reqeusted port is 0 (ie. any port) then we try to bind in v4 space
// *    first with a wild-card port argument. We then try to bind in v6 space
// *    using the returned port number. If this fails, we repeat the process
// *    until a free port common to both spaces becomes available.
// *
// * 3. If the requested port is a specific port, then we just try to get that
// *    port in both spaces, and if it is not free in both, then the bind fails.
// *
// * On failure, sockets are closed and an error returned with CLOSE_SOCKETS_AND_RETURN
// */
//
//JNIEXPORT int JNICALL
//NET_BindV6(struct ipv6bind* b) {
//    int fd=-1, ofd=-1, rv, len;
//    /* need to defer close until new sockets created */
//    int close_fd=-1, close_ofd=-1;
//    SOCKETADDRESS oaddr; /* other address to bind */
//    int family = b->addr->him.sa_family;
//    int ofamily;
//    u_short port; /* requested port parameter */
//    u_short bound_port;
//
//    if (family == AF_INET && (b->addr->him4.sin_addr.s_addr != INADDR_ANY)) {
//        /* bind to v4 only */
//        int ret;
//        ret = NET_Bind (b->ipv4_fd, (struct sockaddr *)b->addr,
//                                sizeof (struct sockaddr_in));
//        if (ret == SOCKET_ERROR) {
//            CLOSE_SOCKETS_AND_RETURN;
//        }
//        closesocket (b->ipv6_fd);
//        b->ipv6_fd = -1;
//        return 0;
//    }
//    if (family == AF_INET6 && (!IN6_IS_ADDR_ANY(&b->addr->him6.sin6_addr))) {
//        /* bind to v6 only */
//        int ret;
//        ret = NET_Bind (b->ipv6_fd, (struct sockaddr *)b->addr,
//                                sizeof (struct SOCKADDR_IN6));
//        if (ret == SOCKET_ERROR) {
//            CLOSE_SOCKETS_AND_RETURN;
//        }
//        closesocket (b->ipv4_fd);
//        b->ipv4_fd = -1;
//        return 0;
//    }
//
//    /* We need to bind on both stacks, with the same port number */
//
//    memset (&oaddr, 0, sizeof(oaddr));
//    if (family == AF_INET) {
//        ofamily = AF_INET6;
//        fd = b->ipv4_fd;
//        ofd = b->ipv6_fd;
//        port = (u_short)GET_PORT (b->addr);
//        IN6ADDR_SETANY (&oaddr.him6);
//        oaddr.him6.sin6_port = port;
//    } else {
//        ofamily = AF_INET;
//        ofd = b->ipv4_fd;
//        fd = b->ipv6_fd;
//        port = (u_short)GET_PORT (b->addr);
//        oaddr.him4.sin_family = AF_INET;
//        oaddr.him4.sin_port = port;
//        oaddr.him4.sin_addr.s_addr = INADDR_ANY;
//    }
//
//    rv = NET_Bind (fd, (struct sockaddr *)b->addr, SOCKETADDRESS_LEN(b->addr));
//    if (rv == SOCKET_ERROR) {
//        CLOSE_SOCKETS_AND_RETURN;
//    }
//
//    /* get the port and set it in the other address */
//    len = SOCKETADDRESS_LEN(b->addr);
//    if (getsockname(fd, (struct sockaddr *)b->addr, &len) == -1) {
//        CLOSE_SOCKETS_AND_RETURN;
//    }
//    bound_port = GET_PORT (b->addr);
//    SET_PORT (&oaddr, bound_port);
//    if ((rv=NET_Bind (ofd, (struct sockaddr *) &oaddr,
//                                SOCKETADDRESS_LEN (&oaddr))) == SOCKET_ERROR) {
//        int retries;
//        int sotype, arglen=sizeof(sotype);
//
//        /* no retries unless, the request was for any free port */
//
//        if (port != 0) {
//            CLOSE_SOCKETS_AND_RETURN;
//        }
//
//        getsockopt(fd, SOL_SOCKET, SO_TYPE, (void *)&sotype, &arglen);
//
//#define SOCK_RETRIES 50
//        /* 50 is an arbitrary limit, just to ensure that this
//         * cannot be an endless loop. Would expect socket creation to
//         * succeed sooner.
//         */
//        for (retries = 0; retries < SOCK_RETRIES; retries ++) {
//            int len;
//            close_fd = fd; fd = -1;
//            close_ofd = ofd; ofd = -1;
//            b->ipv4_fd = SOCKET_ERROR;
//            b->ipv6_fd = SOCKET_ERROR;
//
//            /* create two new sockets */
//            fd = socket (family, sotype, 0);
//            if (fd == SOCKET_ERROR) {
//                CLOSE_SOCKETS_AND_RETURN;
//            }
//            ofd = socket (ofamily, sotype, 0);
//            if (ofd == SOCKET_ERROR) {
//                CLOSE_SOCKETS_AND_RETURN;
//            }
//
//            /* bind random port on first socket */
//            SET_PORT (&oaddr, 0);
//            rv = NET_Bind (ofd, (struct sockaddr *)&oaddr, SOCKETADDRESS_LEN(&oaddr));
//            if (rv == SOCKET_ERROR) {
//                CLOSE_SOCKETS_AND_RETURN;
//            }
//            /* close the original pair of sockets before continuing */
//            closesocket (close_fd);
//            closesocket (close_ofd);
//            close_fd = close_ofd = -1;
//
//            /* bind new port on second socket */
//            len = SOCKETADDRESS_LEN(&oaddr);
//            if (getsockname(ofd, (struct sockaddr *)&oaddr, &len) == -1) {
//                CLOSE_SOCKETS_AND_RETURN;
//            }
//            bound_port = GET_PORT (&oaddr);
//            SET_PORT (b->addr, bound_port);
//            rv = NET_Bind (fd, (struct sockaddr *)b->addr, SOCKETADDRESS_LEN(b->addr));
//
//            if (rv != SOCKET_ERROR) {
//                if (family == AF_INET) {
//                    b->ipv4_fd = fd;
//                    b->ipv6_fd = ofd;
//                } else {
//                    b->ipv4_fd = ofd;
//                    b->ipv6_fd = fd;
//                }
//                return 0;
//            }
//        }
//        CLOSE_SOCKETS_AND_RETURN;
//    }
//    return 0;
//}
//
///*
// * Determine the default interface for an IPv6 address.
// *
// * Returns :-
// *      0 if error
// *      > 0 interface index to use
// */
//jint getDefaultIPv6Interface(JNIEnv *env, struct SOCKADDR_IN6 *target_addr)
//{
//    int ret;
//    DWORD b;
//    struct sockaddr_in6 route;
//    SOCKET fd = socket(AF_INET6, SOCK_STREAM, 0);
//    if (fd < 0) {
//        return 0;
//    }
//
//    ret = WSAIoctl(fd, SIO_ROUTING_INTERFACE_QUERY,
//                    (void *)target_addr, sizeof(struct sockaddr_in6),
//                    (void *)&route, sizeof(struct sockaddr_in6),
//                    &b, 0, 0);
//    if (ret < 0) {
//        // error
//        closesocket(fd);
//        return 0;
//    } else {
//        closesocket(fd);
//        return route.sin6_scope_id;
//    }
//}
//
///* If address types is IPv6, then IPv6 must be available. Otherwise
// * no address can be generated. In the case of an IPv4 Inetaddress this
// * method will return an IPv4 mapped address where IPv6 is available and
// * v4MappedAddress is TRUE. Otherwise it will return a sockaddr_in
// * structure for an IPv4 InetAddress.
//*/
//JNIEXPORT int JNICALL
//NET_InetAddressToSockaddr(JNIEnv *env, jobject iaObj, int port, struct sockaddr *him,
//                          int *len, jboolean v4MappedAddress) {
//    jint family, iafam;
//    iafam = (*env)->GetIntField(env, iaObj, ia_familyID);
//    family = (iafam == IPv4)? AF_INET : AF_INET6;
//    if (ipv6_available() && !(family == AF_INET && v4MappedAddress == false)) {
//        struct SOCKADDR_IN6 *him6 = (struct SOCKADDR_IN6 *)him;
//        jbyteArray ipaddress;
//        jbyte caddr[16];
//        jint address, scopeid = 0;
//        jint cached_scope_id = 0;
//
//        if (family == AF_INET) { /* will convert to IPv4-mapped address */
//            memset((char *) caddr, 0, 16);
//            address = (*env)->GetIntField(env, iaObj, ia_addressID);
//            if (address == INADDR_ANY) {
//                /* we would always prefer IPv6 wildcard address
//                caddr[10] = 0xff;
//                caddr[11] = 0xff; */
//            } else {
//                caddr[10] = 0xff;
//                caddr[11] = 0xff;
//                caddr[12] = ((address >> 24) & 0xff);
//                caddr[13] = ((address >> 16) & 0xff);
//                caddr[14] = ((address >> 8) & 0xff);
//                caddr[15] = (address & 0xff);
//            }
//        } else {
//            ipaddress = (*env)->GetObjectField(env, iaObj, ia6_ipaddressID);
//            scopeid = (jint)(*env)->GetIntField(env, iaObj, ia6_scopeidID);
//            cached_scope_id = (jint)(*env)->GetIntField(env, iaObj, ia6_cachedscopeidID);
//            (*env)->GetByteArrayRegion(env, ipaddress, 0, 16, caddr);
//        }
//
//        memset((char *)him6, 0, sizeof(struct SOCKADDR_IN6));
//        him6->sin6_port = (u_short) htons((u_short)port);
//        memcpy((void *)&(him6->sin6_addr), caddr, sizeof(struct in6_addr) );
//        him6->sin6_family = AF_INET6;
//        if ((family == AF_INET6) && IN6_IS_ADDR_LINKLOCAL( &(him6->sin6_addr) )
//            && (!scopeid && !cached_scope_id)) {
//            cached_scope_id = getDefaultIPv6Interface(env, him6);
//            (*env)->SetIntField(env, iaObj, ia6_cachedscopeidID, cached_scope_id);
//        }
//        him6->sin6_scope_id = scopeid != 0 ? scopeid : cached_scope_id;
//        *len = sizeof(struct SOCKADDR_IN6) ;
//    } else {
//        struct sockaddr_in *him4 = (struct sockaddr_in*)him;
//        jint address;
//        if (family != AF_INET) {
//          JNU_ThrowByName(env, JNU_JAVANETPKG "SocketException", "Protocol family unavailable");
//          return -1;
//        }
//        memset((char *) him4, 0, sizeof(struct sockaddr_in));
//        address = (int)(*env)->GetIntField(env, iaObj, ia_addressID);
//        him4->sin_port = htons((short) port);
//        him4->sin_addr.s_addr = (u_long) htonl(address);
//        him4->sin_family = AF_INET;
//        *len = sizeof(struct sockaddr_in);
//    }
//    return 0;
//}
//
//jint
//NET_GetPortFromSockaddr(struct sockaddr *him) {
//    if (him->sa_family == AF_INET6) {
//        return ntohs(((struct sockaddr_in6*)him)->sin6_port);
//    } else {
//        return ntohs(((struct sockaddr_in*)him)->sin_port);
//    }
//}
//
//int
//NET_IsIPv4Mapped(jbyte* caddr) {
//    int i;
//    for (i = 0; i < 10; i++) {
//        if (caddr[i] != 0x00) {
//            return 0; /* false */
//        }
//    }
//
//    if (((caddr[10] & 0xff) == 0xff) && ((caddr[11] & 0xff) == 0xff)) {
//        return 1; /* true */
//    }
//    return 0; /* false */
//}
//
//int
//NET_IPv4MappedToIPv4(jbyte* caddr) {
//    return ((caddr[12] & 0xff) << 24) | ((caddr[13] & 0xff) << 16) | ((caddr[14] & 0xff) << 8)
//        | (caddr[15] & 0xff);
//}
//
//int
//NET_IsEqual(jbyte* caddr1, jbyte* caddr2) {
//    int i;
//    for (i = 0; i < 16; i++) {
//        if (caddr1[i] != caddr2[i]) {
//            return 0; /* false */
//        }
//    }
//    return 1;
//}
//
//int getScopeID (struct sockaddr *him) {
//    struct SOCKADDR_IN6 *him6 = (struct SOCKADDR_IN6 *)him;
//    return him6->sin6_scope_id;
//}
//
//int cmpScopeID (unsigned int scope, struct sockaddr *him) {
//    struct SOCKADDR_IN6 *him6 = (struct SOCKADDR_IN6 *)him;
//    return him6->sin6_scope_id == scope;
//}
//
///**
// * Wrapper for select/poll with timeout on a single file descriptor.
// *
// * flags (defined in net_util_md.h can be any combination of
// * NET_WAIT_READ, NET_WAIT_WRITE & NET_WAIT_CONNECT.
// *
// * The function will return when either the socket is ready for one
// * of the specified operation or the timeout expired.
// *
// * It returns the time left from the timeout, or -1 if it expired.
// */
//
//jint
//NET_Wait(JNIEnv *env, jint fd, jint flags, jint timeout)
//{
//    jlong prevTime = JVM_CurrentTimeMillis(env, 0);
//    jint read_rv;
//
//    while (1) {
//        jlong newTime;
//        fd_set rd, wr, ex;
//        struct timeval t;
//
//        t.tv_sec = timeout / 1000;
//        t.tv_usec = (timeout % 1000) * 1000;
//
//        FD_ZERO(&rd);
//        FD_ZERO(&wr);
//        FD_ZERO(&ex);
//        if (flags & NET_WAIT_READ) {
//          FD_SET(fd, &rd);
//        }
//        if (flags & NET_WAIT_WRITE) {
//          FD_SET(fd, &wr);
//        }
//        if (flags & NET_WAIT_CONNECT) {
//          FD_SET(fd, &wr);
//          FD_SET(fd, &ex);
//        }
//
//        errno = 0;
//        read_rv = select(fd+1, &rd, &wr, &ex, &t);
//
//        newTime = JVM_CurrentTimeMillis(env, 0);
//        timeout -= (newTime - prevTime);
//        if (timeout <= 0) {
//          return read_rv > 0 ? 0 : -1;
//        }
//        newTime = prevTime;
//
//        if (read_rv > 0) {
//          break;
//        }
//
//
//      } /* while */
//
//    return timeout;
//}
//
//int NET_Socket (int domain, int type, int protocol) {
//    int sock;
//    sock = socket (domain, type, protocol);
//    if (sock != INVALID_SOCKET) {
//        SetHandleInformation((HANDLE)(uintptr_t)sock, HANDLE_FLAG_INHERIT, FALSE);
//    }
//    return sock;
//}

#endif // defined( PLATFORM_WIN32 )

// ============================================================================

