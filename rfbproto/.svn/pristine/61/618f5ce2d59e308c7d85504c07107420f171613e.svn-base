#ifndef _RFBIO__H__
#define _RFBIO__H__

#include "rfbclient.h"

extern void RFBResetIO(RFBClient_t * client);
extern void RFBResetReadArea(RFBClient_t * client);
extern void RFBSetReadArea(RFBClient_t * client,
        unsigned char *readArea, int readAreaLength);
extern int RFBReadFromServer(RFBClient_t * client, unsigned int bytes_to_read);
extern int RFBWriteToServer(RFBClient_t * client, unsigned char *buffer, 
                            unsigned int bytes_to_write);
extern int FlushIO(RFBClient_t * client);
extern void PrintInHex(char *buf, int len);

#endif
