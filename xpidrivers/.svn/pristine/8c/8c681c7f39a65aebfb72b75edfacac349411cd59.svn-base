#pragma once

/* IOCTL Definitions */

struct efdma_packet_iobuf
{
  char * buffer;
  size_t length;
};

#define TEST_EFDMA_MAX_PACKET_SIZE  4096


#define OMNITEK_IOC_MAGIC 'O'
#define OMNITEK_BAR_IOCEFDMAWRITEPACKET	  _IOWR(OMNITEK_IOC_MAGIC,1,struct efdma_packet_iobuf *)
#define OMNITEK_BAR_IOCEFDMAREADPACKET    _IOWR(OMNITEK_IOC_MAGIC,2,struct efdma_packet_iobuf *)

