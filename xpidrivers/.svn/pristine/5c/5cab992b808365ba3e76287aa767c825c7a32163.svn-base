/*
 * v4l2-hdmi.h - HDMI extensions to the V4L2 API
 *
 * Copyright 2011 Tandberg Telecom AS.  All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef _V4L2_HDMI_H_
#define _V4L2_HDMI_H_

#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h>
#endif
#include <linux/videodev2.h>

/* data[0] == 0 if format is invalid, == 1 if format is valid. */
#define V4L2_EVENT_FMT_CHANGED (V4L2_EVENT_PRIVATE_START + 0)

/* data[0] == 0 if EDID was not found, == 1 if EDID was found.
 * data[1] == EDID segment that was found. */
#define V4L2_EVENT_EDID        (V4L2_EVENT_PRIVATE_START + 1)

/* data[0] == cec tx status field */
#define V4L2_EVENT_CEC_TX      (V4L2_EVENT_PRIVATE_START + 2)

/* data[0] == cec rx status field */
#define V4L2_EVENT_CEC_RX      (V4L2_EVENT_PRIVATE_START + 3)

struct v4l2_edid {
    __u8 *edid;
};

struct v4l2_cec_cap {
    __u32 logicaldevices;
    __u32 reserved[7];
};

struct v4l2_cec_cmd {
    __u32 cmd;
    __u32 reserved[7];
    union {
        struct {
            __u32 id;
            __u32 enable;
            __u32 addr;
        } conf;
        struct {
            __u32 len;
            __u8  msg[16];
            __u32 status;
        } data;
        __u32 raw[8];
    };
};

/* HDMI-GR DV preset values */
#define V4L2_SVGA_800X600P56    100
#define V4L2_SVGA_800X600P60    101
#define V4L2_SVGA_800X600P72    102
#define V4L2_SVGA_800X600P75    103
#define V4L2_SVGA_800X600P85    104
#define V4L2_SXGA_1280X1024P60  105
#define V4L2_SXGA_1280X1024P75  106
#define V4L2_VGA_640X480P60     107
#define V4L2_VGA_640X480P72     108
#define V4L2_VGA_640X480P75     109
#define V4L2_VGA_640X480P85     110
#define V4L2_XGA_1024X768P60    111
#define V4L2_XGA_1024X768P70    112
#define V4L2_XGA_1024X768P75    113
#define V4L2_XGA_1024X768P85    114

/* cec commands */
#define V4L2_CEC_CMD_CONF  (0)
#define V4L2_CEC_CMD_TX    (1)
#define V4L2_CEC_CMD_RX    (2)

/* cec status field */
#define V4L2_CEC_TX_STATUS_OK            (1 << 0)
#define V4L2_CEC_TX_STATUS_ARB_LOST      (1 << 1)
#define V4L2_CEC_TX_STATUS_RETRY_TIMEOUT (1 << 2)

#define V4L2_CEC_RX_STATUS_READY         (1 << 0)

#define V4L2_CEC_RX_STATUS_READY0        (1 << 0)
#define V4L2_CEC_RX_STATUS_READY1        (1 << 1)
#define V4L2_CEC_RX_STATUS_READY2        (1 << 2)

/* ioctls */

/* set a new EDID segment for a receiver */
#define V4L2_S_EDID             _IOW('a', 1, struct v4l2_edid)

/* get the CEC capabilities */
#define V4L2_CEC_CAP            _IOR('a', 2, struct v4l2_cec_cap)

/* issue a CEC command */
#define V4L2_CEC_CMD            _IOWR('a', 3, struct v4l2_cec_cmd)

/* request a new EDID segment */
#define V4L2_REQ_EDID_SEGMENT   _IOW('a', 4, int)

/* get a EDID segment */
#define V4L2_G_EDID             _IOWR('a', 5, struct v4l2_edid)

/* notify test */
#define V4L2_NOTIFY             _IOWR('a', 6, int)

/* other */
#define V4L2_G_REGISTER         _IOWR('a', 7, struct v4l2_dbg_register)
#define V4L2_S_REGISTER         _IOWR('a', 8, struct v4l2_dbg_register)
#define V4L2_QUERY_DV_PRESET    _IOWR('a', 9, int)
#define V4L2_FIRE_INTERRUPT     _IOWR('a', 10, int)

/* notifications */
#define ADV7612_HOTPLUG         1
#define ADV7612_CABLE_DETECT    2
#define ADV7612_FMT_CHANGE      3
#define ADV7612_AUDIO_CHANGE    4
#define ADV7612_NOTIFY_MIN      ADV7612_HOTPLUG
#define ADV7612_NOTIFY_MAX      ADV7612_AUDIO_CHANGE
#define ADV7604_HOTPLUG         5
#define ADV7604_CABLE_DETECT    6
#define ADV7604_FMT_CHANGE      7
#define ADV7604_FMT_CHANGE_DIG  8
#define ADV7604_AUDIO_CHANGE    9
#define ADV7604_NOTIFY_MIN      ADV7604_HOTPLUG
#define ADV7604_NOTIFY_MAX      ADV7604_AUDIO_CHANGE
#define SII7172_RESET           10 
#define SII7172_MONITOR_DETECT	11
#define SII7172_NOTIFY_MIN      SII7172_RESET
#define SII7172_NOTIFY_MAX      SII7172_RCVR_CONNECT
#define NOTIFY_MIN              ADV7612_NOTIFY_MIN
#define NOTIFY_MAX              SII7172_NOTIFY_MAX

#endif
