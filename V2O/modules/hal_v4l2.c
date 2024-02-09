#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>
                                                                                
#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "vlib.h"

#ifdef __XPI__
#include <asm/types.h>      /* for videodev2.h */
#include <linux/videodev2.h>
#include "netvizxpi/netvizxpi.h"
#include "hal_v4l2.h"

int hal_v4l2_querycap(HAL *hp, struct v4l2_capability *cap)
{
    int rc = -1;

    if (hp  == NULL || cap == NULL)
        return rc;
    else {
        int fd;
        if ((fd = hal_get_video_fd(hp)) < 0) 
            return rc;
        rc = ioctl(fd, VIDIOC_QUERYCAP, cap);
    }
    return rc;
}

static int v4l2_cid_from_netvizxpi_cid(int id)
{
    if (id == NETVIZXPI_CID_BRIGHTNESS)
        return V4L2_CID_BRIGHTNESS;
    else if (id == NETVIZXPI_CID_CONTRAST)
        return V4L2_CID_CONTRAST;
    else if (id == NETVIZXPI_CID_SATURATION)
        return V4L2_CID_SATURATION;
    else if (id == NETVIZXPI_CID_HUE)
        return V4L2_CID_HUE;
    else if (id == NETVIZXPI_CID_AUDIO_MUTE)
        return V4L2_CID_AUDIO_MUTE;
    else if (id == NETVIZXPI_CID_RX_RGB_RANGE)
        return V4L2_CID_ADV_RX_RGB_RANGE;
    else if (id == NETVIZXPI_CID_SAMPLING_PHASE)
        return V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE;
    return -1;
}

static int v4l2_rgb_range_from_netvizxpi_rgb_range(int range)
{
    if (range == NETVIZXPI_RGB_RANGE_AUTO)
        return V4L2_DV_RGB_RANGE_AUTO;
    else if (range == NETVIZXPI_RGB_RANGE_LIMITED)
        return V4L2_DV_RGB_RANGE_LIMITED;
    else if (range == NETVIZXPI_RGB_RANGE_FULL)
        return V4L2_DV_RGB_RANGE_FULL;
    return -1;
}

static int netvizxpi_rgb_range_from_v4l2_rgb_range(int range)
{
    if (range == V4L2_DV_RGB_RANGE_AUTO)
        return NETVIZXPI_RGB_RANGE_AUTO;
    else if (range == V4L2_DV_RGB_RANGE_LIMITED)
        return NETVIZXPI_RGB_RANGE_LIMITED;
    else if (range == V4L2_DV_RGB_RANGE_FULL)
        return NETVIZXPI_RGB_RANGE_FULL;
    return -1;
}

int hal_v4l2_g_ctrl(HAL *hp, int id)
{
    int rc = -1;

    if (hp  == NULL)
        return rc;
    else {
        int fd;
        struct v4l2_control ctrl;
        if ((fd = hal_get_video_fd(hp)) < 0) 
            return rc;
        if (id > NETVIZXPI_CID_VIDIOC_MAX && id <= NETVIZXPI_CID_MAX) 
            ctrl.id = id;
        else if ((ctrl.id = v4l2_cid_from_netvizxpi_cid(id)) < 0) 
            return rc;
        rc = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        if (rc == 0) {
            if (id == NETVIZXPI_CID_RX_RGB_RANGE) {
                return netvizxpi_rgb_range_from_v4l2_rgb_range(ctrl.value);
            }
            return ctrl.value;
        }
    }
    return rc;
}

int hal_v4l2_s_ctrl(HAL *hp, int id, int value)
{
    int rc = -1;

    if (hp  == NULL)
        return rc;
    else {
        int fd;
        struct v4l2_control ctrl;
        if ((fd = hal_get_video_fd(hp)) < 0) 
            return rc;
        if (id > NETVIZXPI_CID_VIDIOC_MAX && id <= NETVIZXPI_CID_MAX) 
            ctrl.id = id;
        else if ((ctrl.id = v4l2_cid_from_netvizxpi_cid(id)) < 0) 
            return rc;
        if (id == NETVIZXPI_CID_RX_RGB_RANGE) {
            if ((ctrl.value = 
                        v4l2_rgb_range_from_netvizxpi_rgb_range(value)) < 0)
                return rc;
        }
        else
            ctrl.value = value;
        rc = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    }
    return rc;
}

int hal_v4l2_s_input(HAL *hp, int input)
{
    int rc = -1, value;

    if (hp  == NULL)
        return rc;
    else if (input != NETVIZXPI_MODE_GR && input != NETVIZXPI_MODE_COMP &&
                input != NETVIZXPI_MODE_HDMI)
        return rc;
    else {
        int fd;
        if ((fd = hal_get_video_fd(hp)) < 0) 
            return rc;
        value = input;
        rc = ioctl(fd, VIDIOC_S_INPUT, &value);
    }
    return rc;
}


int hal_v4l2_g_dv_timings(HAL *hp, struct v4l2_dv_timings *timings)
{
    int rc = -1;

    if (hp  == NULL || timings == NULL)
        return rc;
    else {
        int fd;
        if ((fd = hal_get_video_fd(hp)) < 0) 
            return rc;
        rc = ioctl(fd, VIDIOC_G_DV_TIMINGS, timings);
    }
    return rc;
}
#endif      /* __XPI__ */
