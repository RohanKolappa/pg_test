/*
 * CDefaultIPAddress.h
 *
 *  Created on: Sep 18, 2013
 *      Author: pearl
 */

#ifndef CDEFAULTIPADDRESS_H_
#define CDEFAULTIPADDRESS_H_

class CNETutils {
public:
	CNETutils();
	virtual ~CNETutils();

	bool GetLocalIPAddress(char *a_sBuffer, int a_nLen);
#ifndef WINDOWS
    int ReadNlSock(int sockFd, char *bufPtr, int seqNum, int pId);
    void ParseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,
            struct in_addr *target_addr, struct in_addr *gateway_addr,
            int *bitlen);
    int GetRouteForIp(struct in_addr *target_addr,
            struct in_addr *gateway_addr);
	bool GetDestinationMacAddress(int nSockfd, struct sockaddr_in *dest_addr,
            int isMulticast, unsigned short mac[]);
    unsigned int GetInAddr(char *ipaddress);
    bool GetMacForIp(char *dstip, unsigned short mac[]);
#endif
};

#endif /* CDEFAULTIPADDRESS_H_ */
