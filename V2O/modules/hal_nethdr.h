#ifndef __HAL_NETHDR_H_
#define __HAL_NETHDR_H_

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#define NETHDR_VIDEO_OUT_CONTEXT    0
#define NETHDR_AUDIO_OUT_CONTEXT    1

#ifdef __cplusplus
extern "C" {
#endif
    extern int hal_set_nethdr(HAL *hp, 
                              unsigned char context, 
                              unsigned int serverport, 
                              unsigned char ttl, 
                              unsigned int clientipaddr, 
                              unsigned short clientmacaddr[], 
                              unsigned int clientport);
    extern int hal_clear_nethdr(HAL *hp, 
                                unsigned char context);
#ifdef __cplusplus
}
#endif
#endif  /* __HAL_NETHDR_H_ */

