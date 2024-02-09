#ifndef __HAL_V4L2_H__
#define __HAL_V4L2_H__

#ifdef __XPI__

#ifndef __LINUX_VIDEODEV2_H
#include <linux/videodev2.h>
#endif

#ifndef __V4L2_DV_BT_TIMINGS_H__
#include "netvizxpi/v4l2-dv-bt-timings.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    int hal_v4l2_querycap(HAL *hp, struct v4l2_capability *cap);
    int hal_v4l2_g_ctrl(HAL *hp, int id);
    int hal_v4l2_s_ctrl(HAL *hp, int id, int value);
    int hal_v4l2_s_input(HAL *hp, int input);
    int hal_v4l2_g_dv_timings(HAL *hp, struct v4l2_dv_timings *timings);

#ifdef __cplusplus
}
#endif

#endif      /* __XPI__ */

#endif      /* __HAL_V4L2_H__ */
