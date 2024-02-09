#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <syslog.h>

#include "hal.h"
#include "hal_nethdr.h"
#include "vlog.h"
#include "system_utils.h"

#define IPNUDPHEADER_SIZE   64
#define IPNUDPPACKETIZER_BASE   2048

#define DEBUG

static int convert_mac_addr(char *mac_str, unsigned short mac_addr[])
{
    unsigned int mac[6];

    mac_addr[0] = mac_addr[1] = mac_addr[2] = 0;

    if (sscanf(mac_str, "%x:%x:%x:%x:%x:%x", &mac[5], &mac[4], &mac[3],
        	&mac[2], &mac[1], &mac[0]) != 6) {
        return -1; 
    }
    mac_addr[2] = mac[5] << 8 | mac[4];  
    mac_addr[1] = mac[3] << 8 | mac[2];  
    mac_addr[0] = mac[1] << 8 | mac[0];  

    return 0;
}

static int get_mac_info(char *ifname, unsigned short mac[])
{
    char filename[64];
    char str[128];
    int fd, rc, i;
    struct stat astat;

    mac[0] = mac[1] = mac[2] = 0;
    sprintf(filename, "/sys/class/net/%s/address", ifname);
    if (lstat(filename, &astat) != 0) {
        return -1;
    }
    if (S_ISREG(astat.st_mode) != 1) 
        return -1;

    if ((fd = open(filename, O_RDONLY)) < 0) 
        return -1;

    rc = read(fd, str, 128);
    for (i = 0; i < 128; i++) {
        if (str[i] != ':' && !isxdigit(str[i])) {
            str[i] = 0;
            break;
        }
    }
    close(fd);

    if (rc < 0) 
        return -1;

    return convert_mac_addr(str, mac);
}

static unsigned int get_ip_info(char *ifname)
{
    int rc;
    struct in_addr ip_addr;
    struct in_addr netmask_addr;

    if ((rc = get_if_info(ifname, &ip_addr, &netmask_addr)) < 0)
        return 0;

    return (unsigned int) ip_addr.s_addr;
}

#ifdef DEBUG
static void dump_header(unsigned short header[], int headersize)
{
    int i;
    for (i = 0; i < headersize; i += 8) {
        if (i <= headersize - 8) 
	        VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x %04x %04x %04x %04x %04x %04x",
                header[i], header[i+1], header[i+2], header[i+3], 
                header[i+4], header[i+5], header[i+6], header[i+7]);
        else if (i == headersize - 7) 
	        VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x %04x %04x %04x %04x %04x",
                header[i], header[i+1], header[i+2], header[i+3], 
                header[i+4], header[i+5], header[i+6]);
        else if (i == headersize - 6) 
	       VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x %04x %04x %04x %04x",
                header[i], header[i+1], header[i+2], header[i+3], 
                header[i+4], header[i+5]);
        else if (i == headersize - 5) 
	        VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x %04x %04x %04x",
                header[i], header[i+1], header[i+2], header[i+3], 
                header[i+4]);
        else if (i == headersize - 4) 
	        VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x %04x %04x",
                header[i], header[i+1], header[i+2], header[i+3]);
        else if (i == headersize - 3) 
	        VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x %04x",
                header[i], header[i+1], header[i+2]);
        else if (i == headersize - 2) 
	        VLOG_INFO_CAT(VLOG_NETHDR,
                "%04x %04x", header[i], header[i+1]);
        else if (i == headersize - 1) 
  	        VLOG_INFO_CAT(VLOG_NETHDR, 
                "%04x", header[i]);
    }
}
#endif

/* 
 * Sets the network header and starts streaming to the client. 
 */
int hal_set_nethdr(HAL *hp,
                   unsigned char context,
                   unsigned int serverport,
                   unsigned char ttl,
                   unsigned int clientipaddr, 
                   unsigned short clientmacaddr[],
                   unsigned int clientport) 
{
#ifdef __XPI__
    int i, rc = -1; 
    unsigned short *header;
    char ifname[32];
    unsigned int ipaddr;
    unsigned short macaddr[6];

    if (context > 15)
        return 0;

    strcpy(ifname, DEFAULT_IF_NAME);

    if (get_mac_info(ifname, macaddr) < 0) {
        VLOG_INFO_CAT(VLOG_NETHDR,
            "Failed to get host MAC string for %s", ifname);
        return -1;
    }
    if ((ipaddr = get_ip_info(ifname)) == 0) {
        VLOG_INFO_CAT(VLOG_NETHDR,
            "Failed to get host IP string for %s", ifname);
        return -1;
    }

    header = (unsigned short *) calloc(IPNUDPHEADER_SIZE, sizeof(unsigned short));
    if (header == NULL) {
        VLOG_INFO_CAT(VLOG_NETHDR,
            "Failed to alloc memory for header errno %d/%m", errno);
        goto out;
    }

    header[0] = clientmacaddr[2];	    /* dst MAC */	
    header[1] = clientmacaddr[1];
    header[2] = clientmacaddr[0];
    header[3] = macaddr[2];		        /* src MAC */
    header[4] = macaddr[1];
    header[5] = macaddr[0];
    header[6] = 0x8100;			        /* TPID */
    header[7] = 0x0000;			        /* VLAN tag */
    header[8] = 0x0800;			        /* EtherType */
    header[9] = 0x4500;			        /* Ver & header len */
    header[10] = 0x0000;		        /* Total len (done by HW) */
    header[11] = 0x0000;		        /* Ident */
    header[12] = 0x0000;		        /* Fragmentation */
    header[13] = (ttl << 8) | 0x11;	    /* Multicast TTL */
    header[14] = 0x0000;		        /* Header cksum (done by HW) */
    ipaddr = htonl(ipaddr);		        /* Src IP v4 addr */
    header[15] = (ipaddr >> 16);	      
    header[16] = (ipaddr & 0xffff);
    clientipaddr = htonl(clientipaddr);	/* Dst IP v4 addr */
    header[17] = (clientipaddr >> 16);	
    header[18] = (clientipaddr & 0xffff);
    header[19] = 0x6000;		        /* Ver, traffic class, */
    header[20] = 0x0000;		        /* and flow label */
    header[21] = 0x0000;		        /* Payload len (done by HW) */ 
    header[22] = 0x1140;               	/* Next buffer, hop limit */
    for (i = 23; i < 31; i++)		    /* Src IP v6 addr */
        header[i] = 0x0000;                      
    for (i = 31; i < 39; i++)		    /* Dst IP v6 addr */
        header[i] = 0x0000;                      
    header[39] = serverport;   	        /* Src port */
    header[40] = clientport;        	/* Dst port */
    header[41] = 0x0000;           	    /* UDP len (done by HW) */
    header[42] = 0x0000;           	    /* UDP cksum (done by HW) */
    for (i = 0; i < IPNUDPHEADER_SIZE; i++) {
        hal_set_omnitek_val_k(hp, 
                IPNUDPPACKETIZER_BASE + context * IPNUDPHEADER_SIZE + i, 
                (unsigned long) header[i]); 
    } 
    VLOG_INFO_CAT(VLOG_NETHDR,
        "Successfully wrote %d bytes header to FPGA Bar %d addr 0x%x", 
        IPNUDPHEADER_SIZE, FPGA_BARNO, 
        IPNUDPPACKETIZER_BASE + context * IPNUDPHEADER_SIZE);
#ifdef DEBUG
    dump_header(header, IPNUDPHEADER_SIZE);
#endif
    if (context == NETHDR_VIDEO_OUT_CONTEXT)
        rc = hal_enable_addside_video(hp);
    else if (context == NETHDR_AUDIO_OUT_CONTEXT)
        rc = hal_enable_addside_audio(hp);
    else
        rc = 0;

out:
    if (header != NULL)
        free(header);

    return rc;
#else
    return -1;
#endif
}

/* 
 * Clears the network header and stops streaming. 
 */
int hal_clear_nethdr(HAL *hp, unsigned char context)
{
#ifdef __XPI__
    int i, rc = -1;

    if (context == NETHDR_VIDEO_OUT_CONTEXT)
        rc = hal_disable_addside_video(hp);
    else if (context == NETHDR_AUDIO_OUT_CONTEXT)
        rc = hal_disable_addside_audio(hp);

    if (rc == 0) {
        for (i = 0; i < IPNUDPHEADER_SIZE; i++) {
            hal_set_omnitek_val_k(hp, 
                IPNUDPPACKETIZER_BASE + context * IPNUDPHEADER_SIZE + i, 0);
        }
    } 
    return rc;
#else
    return -1;
#endif
}

