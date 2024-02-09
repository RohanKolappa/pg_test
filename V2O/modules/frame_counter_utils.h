#ifndef __FRAME_COUNTER_UTILS_H__
#define __FRAME_COUNTER_UTILS_H__

#include <stdio.h>

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#define FRAMECNT_LINE_SIZE      40
#define FRAMECNT_FIFO_SIZE      (0x600 * sizeof(unsigned long))
#define FRAMECNT_TEXT_SIZE      (0x600 * FRAMECNT_LINE_SIZE)

#ifdef __cplusplus
extern "C" {
#endif
                                 
    int hal_read_framecnt_fifo(HAL *hp,
                               unsigned char *buf,
                               int sampling_interval);
    int print_framecnt_to_buffer(unsigned char *buf,
                                 char *txt,
                                 char *pager,
                                 int level,
                                 int tm,
                                 int type);
#ifdef __cplusplus
}
#endif

#endif
