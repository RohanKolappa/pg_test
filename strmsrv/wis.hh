#ifndef __HH_WIS_H__
#define __HH_WIS_H__
#include <pthread.h>
/******************************************************/
/* Structures to implement RAW Header for WIS         */
/******************************************************/
typedef struct rawhdr {
  unsigned long packetFlag;
  unsigned long packetSize;
  unsigned int  packetTimestamp;
} rawhdr_t;
#define PACKET_BLOCK_SIZE   0x00000200
#define PACKET_BLOCK_LAST   0x000001FF
#define PACKET_BLOCK_MASK   0xFFFFFE00

#define FRAMECHID_MASK      0x000000FF
#define PACKET_RAW              1
#define PACKET_MPEGTS           2
/******************************************************/
/* Structures to implement Shared Memory Interface    */
/******************************************************/
typedef struct shmhdr {
        char h_cWriteFlag;
        char h_cReadFlag;
        char h_cReserved1;
        char h_cReserved2;
        unsigned int h_nReadBytes;
        unsigned int h_nWriteBytes;
        struct timeval h_tTs;
} shmhdr_t;
#define READ_IN_PROGRESS       1
#define READ_COMPLETE          2
#define WRITE_IN_PROGRESS       1
#define WRITE_COMPLETE          2
#define MAX_SEGMENTS            10
#define WISPKTSIZE      188
#define WISSTATINTERVAL 1000
#endif /*__HH_WIS_H__*/
