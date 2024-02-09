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
#include "hal_rtpfilter.h"
#include "vlog.h"
#include "system_utils.h"
#ifdef __XPI__
#include "OmniTekIoctl_linux.h"
#endif

#define RTPFILTER_BASE	0x3840
#define RTPFILTER_SIZE	8

#define DEBUG
#ifdef DEBUG
static void dump_hex(unsigned long filter[], int filtersize)
{
    int i;
    for (i = 0; i < filtersize; i += 8) {
        if (i <= filtersize - 8) 
            VLOG_INFO_CAT(VLOG_RTPFILTER,
	            "%08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx",
                filter[i], filter[i+1], filter[i+2], filter[i+3], 
                filter[i+4], filter[i+5], filter[i+6], filter[i+7]);
        else if (i == filtersize - 7) 
            VLOG_INFO_CAT(VLOG_RTPFILTER,
	            "%08lx %08lx %08lx %08lx %08lx %08lx %08lx",
                filter[i], filter[i+1], filter[i+2], filter[i+3], 
                filter[i+4], filter[i+5], filter[i+6]);
        else if (i == filtersize - 6) 
            VLOG_INFO_CAT(VLOG_RTPFILTER,
	            "%08lx %08lx %08lx %08lx %08lx %08lx",
                filter[i], filter[i+1], filter[i+2], filter[i+3], 
                filter[i+4], filter[i+5]);
        else if (i == filtersize - 5) 
            VLOG_INFO_CAT(VLOG_RTPFILTER,
	            "%08lx %08lx %08lx %08lx %08lx",
                filter[i], filter[i+1], filter[i+2], filter[i+3], 
                filter[i+4]);
        else if (i == filtersize - 4) 
            VLOG_INFO_CAT(VLOG_RTPFILTER, "%08lx %08lx %08lx %08lx",
                filter[i], filter[i+1], filter[i+2], filter[i+3]);
        else if (i == filtersize - 3) 
            VLOG_INFO_CAT(VLOG_RTPFILTER, "%08lx %08lx %08lx",
                filter[i], filter[i+1], filter[i+2]);
        else if (i == filtersize - 2) 
            VLOG_INFO_CAT(VLOG_RTPFILTER, "%08lx %08lx", 
                    filter[i], filter[i+1]);
        else if (i == filtersize - 1) 
            VLOG_INFO_CAT(VLOG_RTPFILTER, "%08lx", filter[i]);
    }
}
#endif

/* 
 *  Sets the RTP filter and enables receiving streamed data
 */
int hal_set_rtpfilter(HAL *hp, 
                      int index, 
                      unsigned long addr,
                      unsigned short port, 
                      unsigned short flags)
{
#ifdef __XPI__
    int i, rc = 0;
    unsigned long filter[RTPFILTER_SIZE];
    OmniTekBARIoctlBuffer buffer;
    struct in_addr in_ip_addr;
    struct in_addr in_netmask_addr;

    if (index < 0)
        return -1;
    memset(filter, 0, sizeof(unsigned long) * RTPFILTER_SIZE);
    if (port != 0) { 
        filter[0] = 0xe0000000;		    /* enable, ipv4, ignore ssrc */	
        filter[1] = addr;
        filter[5] = port;               /* port to receive packets */
        filter[6] = 0;                  /* ssrc */
        filter[7] = 
            (index == RTPFILTER_INDEX_AUDIO) ?  RTPFILTER_CONTEXT_AUDIO : 
            ((index == RTPFILTER_INDEX_VIDEO) ? RTPFILTER_CONTEXT_VIDEO : index);
        if (flags & RTPFILTER_FLAG_AES) 
            filter[7] |= 0x80000000;	/* aes */
        if (flags & RTPFILTER_FLAG_CPU)
            filter[7] |= 0x40000000;	/* cpu */
        if (flags & RTPFILTER_FLAG_DISCARD)
            filter[7] |= 0x20000000;	/* discard */
    }

#ifdef DEBUG
    VLOG_INFO_CAT(VLOG_RTPFILTER,
        "%d words RTP filter will be written to FPGA Bar %d addr 0x%x:", 
        RTPFILTER_SIZE, FPGA_BARNO, 
        RTPFILTER_BASE + index * RTPFILTER_SIZE); 
    dump_hex(filter, RTPFILTER_SIZE);
#endif

    for (i = 0; i < RTPFILTER_SIZE; i++) {
        hal_set_omnitek_val_k(hp, RTPFILTER_BASE + index * RTPFILTER_SIZE + i,
                filter[i]);
    }
    
    if (index == RTPFILTER_INDEX_AUDIO) {
        hal_disable_dropside_audio(hp);     /* disable to flush the buffer */
        usleep(1000);                       
        rc = hal_enable_dropside_audio(hp); /* re-enable drop audio */
    }
    else 
        rc = 0; 

    return rc;
#else 
    return -1; 
#endif
}

/* 
 *  Clears the RTP filter and disables receiving streamed data
 */
int hal_clear_rtpfilter(HAL *hp, int index) 
{
#ifdef __XPI__
    OmniTekBARIoctlBuffer buffer;
    int i, rc = -1;

    if (index == RTPFILTER_INDEX_VIDEO)
        rc = 0;     /* dropside video is always enabled */
    else if (index == RTPFILTER_INDEX_AUDIO)
        rc = hal_disable_dropside_audio(hp);

    if (rc == 0) {
#ifdef DEBUG
        VLOG_INFO_CAT(VLOG_RTPFILTER,
            "%d words RTP filter will be written to FPGA Bar %d addr 0x%x:", 
            RTPFILTER_SIZE, FPGA_BARNO, 
            RTPFILTER_BASE + index * RTPFILTER_SIZE); 
        VLOG_INFO_CAT(VLOG_RTPFILTER,
	            "%08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx",
                0ul, 0ul, 0ul, 0ul, 0ul, 0ul, 0ul, 0ul);
#endif
        for (i = 0; i < RTPFILTER_SIZE; i++) {
            hal_set_omnitek_val_k(hp, 
                    RTPFILTER_BASE + index * RTPFILTER_SIZE + i, 0);
        }
    } 

    return rc;
#else
    return -1;
#endif
}

