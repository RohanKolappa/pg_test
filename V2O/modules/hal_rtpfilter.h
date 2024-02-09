#ifndef __HAL_RTPFILTER_H_
#define __HAL_RTPFILTER_H_

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#define RTPFILTER_FLAG_DEBUG	    0x0001
#define RTPFILTER_FLAG_AES	        0x0002
#define RTPFILTER_FLAG_CPU	        0x0004
#define RTPFILTER_FLAG_DISCARD	    0x0008

#ifdef __cplusplus
extern "C" {
#endif
    extern int hal_set_rtpfilter(HAL *hp,
                                 int index,
                                 unsigned long ipaddr,
                                 unsigned short port,
                                 unsigned short flags);
    extern int hal_clear_rtpfilter(HAL *hp, int index);
#ifdef __cplusplus
}
#endif
#endif  /* __HAL_RTPFILTER_H_ */

