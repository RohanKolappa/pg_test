/*
 * netutils.cpp
 *
 *  Created on: Sep 18, 2013
 *      Author: pearl
 */

#include "strmsrv_compatibility.h"
#include "common.hh"
#include "netutils.hh"

#ifndef WINDOWS
#include <netdb.h>
#include <ifaddrs.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#endif

using namespace std;

CNETutils::CNETutils() {
}

CNETutils::~CNETutils() {
}

// Retrieves IP address of "eth0" interface.
bool CNETutils::GetLocalIPAddress(char *a_sBuffer, int a_nLen) {
    bool retval = false;

    strcpy(a_sBuffer, "");

    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1)
        return false;

    int family, s;
    char host[NI_MAXHOST];
    struct ifaddrs *ifa;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        family = ifa->ifa_addr->sa_family;
        if (family != AF_INET || strcmp(ifa->ifa_name, "eth0") != 0)
            continue;
        s = getnameinfo(ifa->ifa_addr,
                sizeof(struct sockaddr_in),
                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0)
            continue;
        strncpy(a_sBuffer, host, a_nLen);
        a_sBuffer[a_nLen - 1] = 0;
        retval = true;
    }

    freeifaddrs(ifaddr);

    return retval;
}


#ifndef WINDOWS

#define BUFSIZE 8192

struct route_info {
    struct in_addr dstAddr;
    struct in_addr srcAddr;
    struct in_addr gateWay;
    char ifName[IF_NAMESIZE];
};

int CNETutils::ReadNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;
    
    do {
        /* Recieve response from the kernel */
        if ((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0) {
            perror("SOCK READ: ");
            return -1;
        }
        
        nlHdr = (struct nlmsghdr *) bufPtr;
        

        /* Check if the header is valid */
        if ((NLMSG_OK(nlHdr, (unsigned int) readLen) == 0)
            || (nlHdr->nlmsg_type == NLMSG_ERROR)) {
            perror("Error in recieved packet");
            return -1;
        }
        
        /* Check if the its the last message */
        if (nlHdr->nlmsg_type == NLMSG_DONE) {
            break;
        } else {
            /* Else move the pointer to buffer appropriately */
            bufPtr += readLen;
            msgLen += readLen;
        }
        
        /* Check if its a multi part message */
        if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
            /* return if its not */
            break;
        }
    } while (((int)nlHdr->nlmsg_seq != seqNum) || ((int)nlHdr->nlmsg_pid != pId));
    return msgLen;
}


/* For parsing the route info returned */
void CNETutils::ParseRoutes(struct nlmsghdr *nlHdr, 
        struct route_info *rtInfo, struct in_addr *target_addr, 
        struct in_addr *gateway_addr, int *bitlen)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;


    rtMsg = (struct rtmsg *) NLMSG_DATA(nlHdr);

    /* If the route is not for AF_INET or does not belong to main routing table
       then return. */
    if ((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN)) {
        // printf("Not main route\n");
        return;
    }

    /* get the rtattr field */
    rtAttr = (struct rtattr *) RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)) {
        switch (rtAttr->rta_type) {
        case RTA_OIF:
            if_indextoname(*(int *) RTA_DATA(rtAttr), rtInfo->ifName);
            break;
        case RTA_GATEWAY:
            rtInfo->gateWay.s_addr = *(u_int *) RTA_DATA(rtAttr);
            break;
        case RTA_PREFSRC:
            rtInfo->srcAddr.s_addr = *(u_int *) RTA_DATA(rtAttr);
            break;
        case RTA_DST:
            rtInfo->dstAddr.s_addr = *(u_int *) RTA_DATA(rtAttr);
            break;
        default:
            break;
        }
    }

    int bits = rtMsg->rtm_dst_len;
    bits &= 0x1f;
    u_int mask = htonl((0xffffffff) << (0x20 - bits));
    if (bits == 0)
        mask = 0;
    if ((target_addr->s_addr & mask) == 
        (rtInfo->dstAddr.s_addr & mask)) {
        if (bits >= *bitlen) {
            *bitlen = bits;
             gateway_addr->s_addr = rtInfo->gateWay.s_addr;
        }
    }

    return;
}

int CNETutils::GetRouteForIp(struct in_addr *target_addr, 
        struct in_addr *gateway_addr)
{
    struct nlmsghdr *nlMsg;
    struct route_info *rtInfo;
    char msgBuf[BUFSIZE];
    int bitlen = -1;

    int sock, len, msgSeq = 0;

    gateway_addr->s_addr = 0;

    /* Create Socket */
    if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
        perror("Socket Creation: ");
        return -1;
    }

    memset(msgBuf, 0, BUFSIZE);

    /* point the header and the msg structure pointers into the buffer */
    nlMsg = (struct nlmsghdr *) msgBuf;

    /* Fill in the nlmsg header*/
    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));  // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE;   // Get the routes from kernel 
                                        // routing table .

    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;// The message is a 
                                                    // request for dump.
    nlMsg->nlmsg_seq = msgSeq++;    // Sequence of the message packet.
    nlMsg->nlmsg_pid = getpid();    // PID of process sending the request.

    /* Send the request */
    if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
        printf("Write To Socket Failed...\n");
        close(sock);
        return -1;
    }

    /* Read the response */
    if ((len = ReadNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
        printf("Read From Socket Failed...\n");
        close(sock);
        return -1;
    }

    /* Parse and print the response */
    rtInfo = (struct route_info *) malloc(sizeof(struct route_info));

    //fprintf(stdout, "Destination\tGateway\tInterface\tSource\n");
    for (; (NLMSG_OK(nlMsg, (unsigned int) len)); nlMsg = NLMSG_NEXT(nlMsg, len)) {
        memset(rtInfo, 0, sizeof(struct route_info));
        ParseRoutes(nlMsg, rtInfo, target_addr, gateway_addr, &bitlen);
    }

    free(rtInfo);
    close(sock);

    return 0;
}

// Retrieves MAC address for destination with IP address 'dest_addr'
bool CNETutils::GetDestinationMacAddress(int nSockfd, 
        struct sockaddr_in * dest_addr, int isMulticast, unsigned short mac[]) {
    struct arpreq areq;
    struct sockaddr_in *sin;
    struct in_addr gateway_addr;
    bool retval = false;

    memset(&areq, 0, sizeof(struct arpreq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = dest_addr->sin_addr.s_addr;
    strcpy(areq.arp_dev, "eth0");

    if (isMulticast) {
        /* For multicast  the dest MAC is of the form 01:00:5e:7X:XX:XX
           with the lowest 23 bits mapped with the lowest 23 bits of 
           multicast address */
        mac[0] = ((0xFF & (dest_addr->sin_addr.s_addr >> 16)) << 8) | 
            (0xFF & (dest_addr->sin_addr.s_addr >> 24));
        mac[1] = (0x5e << 8) | 
            (0x7F & (dest_addr->sin_addr.s_addr >> 8));
        mac[2] = 0x01 << 8 | 00;
        return true;
    }

    /* Check if the destination should be routed via a gateway and if found
     use that ip address instead */
    gateway_addr.s_addr = 0;
    GetRouteForIp(&dest_addr->sin_addr, &gateway_addr);

    if (gateway_addr.s_addr != 0)
        sin->sin_addr.s_addr = gateway_addr.s_addr;

    if (ioctl(nSockfd, SIOCGARP, (caddr_t) &areq) == 0) {
        mac[0] = areq.arp_ha.sa_data[4] << 8 | areq.arp_ha.sa_data[5];
        mac[1] = areq.arp_ha.sa_data[2] << 8 | areq.arp_ha.sa_data[3];
        mac[2] = areq.arp_ha.sa_data[0] << 8 | areq.arp_ha.sa_data[1];
        retval = true;
    }
    return retval;
}

// Returns integer representation for IP address in numbers-and-dots format
unsigned int CNETutils::GetInAddr(char *ipaddress) {
    return inet_addr(ipaddress);
} 

// Returns true if found MAC address for IP address else false
bool CNETutils::GetMacForIp(char *dstip, unsigned short mac[]) {
    int sock;
    struct addr;
    struct arpreq areq;
    struct sockaddr_in *sin;
    struct in_addr dst_addr, gateway_addr;
    bool retval = false;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return -1;
 
    if (inet_aton(dstip, &dst_addr) == 0)
        return -1;

    memset(&areq, 0, sizeof(struct arpreq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = dst_addr.s_addr;
    strcpy(areq.arp_dev, "eth0");

    /* Check if the destination should be routed via a gateway and if found
     use that ip address instead */
    gateway_addr.s_addr = 0;
    GetRouteForIp(&dst_addr, &gateway_addr);

    if (gateway_addr.s_addr != 0)
        sin->sin_addr.s_addr = gateway_addr.s_addr;

    if (ioctl(sock, SIOCGARP, (caddr_t) &areq) == 0) {
        mac[0] = areq.arp_ha.sa_data[4] << 8 | areq.arp_ha.sa_data[5];
        mac[1] = areq.arp_ha.sa_data[2] << 8 | areq.arp_ha.sa_data[3];
        mac[2] = areq.arp_ha.sa_data[0] << 8 | areq.arp_ha.sa_data[1];
        retval = true;
    }
    return retval;
}

#endif
