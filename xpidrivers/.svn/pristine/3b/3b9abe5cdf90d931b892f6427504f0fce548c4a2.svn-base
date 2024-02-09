/*
 * adv7612 - Based on the v4l2 adv7612 driver from Tandberg
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/in.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/videodev2.h>
#include <linux/workqueue.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/platform-netvizxpi.h>

#include "v4l2-chip-ident.h"
#include "v4l2-hdmi.h"
#include "v4l2-dv-bt-timings.h"

#include "netvizxpi.h"
#include "adv7604.h"
#include "adv7612.h"

MODULE_DESCRIPTION("Netviz XPI adv7612 video decoder driver");
MODULE_AUTHOR("Barco IP Video Systems");
MODULE_LICENSE("GPL");

static int debug = 0;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "debug level, default is 0 (no debug messages)");

/*
 **********************************************************************
 *  Supported standard timings for the ADV7604
 **********************************************************************
 */
const struct v4l2_dv_timings adv7612_timings[] = {
    INVALID_DV_BT_TIMING,
    V4L2_DV_BT_CEA_720X480P59_94,
    V4L2_DV_BT_CEA_720X576P50,
    V4L2_DV_BT_CEA_1280X720P24,
    V4L2_DV_BT_CEA_1280X720P25,
    V4L2_DV_BT_CEA_1280X720P30,
    V4L2_DV_BT_CEA_1280X720P50,
    V4L2_DV_BT_CEA_1280X720P60,
    V4L2_DV_BT_CEA_1920X1080P24,
    V4L2_DV_BT_CEA_1920X1080P25,
    V4L2_DV_BT_CEA_1920X1080P30,
    V4L2_DV_BT_CEA_1920X1080P50,
    V4L2_DV_BT_CEA_1920X1080P60,
    V4L2_DV_BT_DMT_800X600P56,
    V4L2_DV_BT_DMT_800X600P60,
    V4L2_DV_BT_DMT_1024X768P60,
    V4L2_DV_BT_DMT_1280X1024P60,
    V4L2_DV_BT_DMT_1400X1050P60,
    V4L2_DV_BT_DMT_1600X1200P60,
    V4L2_DV_BT_DMT_1680X1050P60
};

/*
 **********************************************************************
 *  Supported video standards for ADV7604
 *  ADV7612_Hardware_Users_Guide_Rev0.pdf p.22 and p.24
 **********************************************************************
 */
struct adv7612_video_standards {
    struct v4l2_dv_timings timings;
    u8 vid_std;
    u8 v_freq;
};

/* sorted by number of lines */
static const struct adv7612_video_standards adv7612_mode_hdmi_comp[] = {
	{ V4L2_DV_BT_CEA_1280X720P24, 0x13, 0x04 },
	{ V4L2_DV_BT_CEA_1280X720P25, 0x13, 0x03 },
	{ V4L2_DV_BT_CEA_1280X720P50, 0x13, 0x01 },
	{ V4L2_DV_BT_CEA_1280X720P60, 0x13, 0x00 },
	{ V4L2_DV_BT_CEA_1920X1080P24, 0x1e, 0x04 },
	{ V4L2_DV_BT_CEA_1920X1080P25, 0x1e, 0x03 },
	{ V4L2_DV_BT_CEA_1920X1080P30, 0x1e, 0x02 },
	{ V4L2_DV_BT_CEA_1920X1080P50, 0x1e, 0x01 },
	{ V4L2_DV_BT_CEA_1920X1080P60, 0x1e, 0x00 },
	{ },
};

/* sorted by number of lines */
static const struct adv7612_video_standards adv7612_mode_hdmi_gr[] = {
	{ V4L2_DV_BT_DMT_640X480P60, 0x08, 0x00 },
	{ V4L2_DV_BT_DMT_640X480P72, 0x09, 0x00 },
	{ V4L2_DV_BT_DMT_640X480P75, 0x0a, 0x00 },
	{ V4L2_DV_BT_DMT_640X480P85, 0x0b, 0x00 },
	{ V4L2_DV_BT_DMT_800X600P56, 0x00, 0x00 },
	{ V4L2_DV_BT_DMT_800X600P60, 0x01, 0x00 },
	{ V4L2_DV_BT_DMT_800X600P72, 0x02, 0x00 },
	{ V4L2_DV_BT_DMT_800X600P75, 0x03, 0x00 },
	{ V4L2_DV_BT_DMT_800X600P85, 0x04, 0x00 },
	{ V4L2_DV_BT_DMT_1024X768P60, 0x0c, 0x00 },
	{ V4L2_DV_BT_DMT_1024X768P70, 0x0d, 0x00 },
	{ V4L2_DV_BT_DMT_1024X768P75, 0x0e, 0x00 },
	{ V4L2_DV_BT_DMT_1024X768P85, 0x0f, 0x00 },
	{ V4L2_DV_BT_DMT_1280X1024P60, 0x05, 0x00 },
	{ V4L2_DV_BT_DMT_1280X1024P75, 0x06, 0x00 },
	{ },
};

/* sorted by number of lines */
static const struct adv7612_video_standards adv7612_mode_hdmi_861_frun[] = {
	{ V4L2_DV_BT_CEA_720X480P59_94, 0x0a, 0x00 },
	{ V4L2_DV_BT_CEA_1280X720P60, 0x13, 0x00 },
	{ V4L2_DV_BT_CEA_1920X1080P60, 0x1e, 0x00 },
	{ V4L2_DV_BT_CEA_720X576P50, 0x0b, 0x00 },
	{ V4L2_DV_BT_CEA_1280X720P50, 0xa3, 0x01 },
	{ V4L2_DV_BT_CEA_1920X1080P50, 0x1e, 0x01 },
	{ V4L2_DV_BT_CEA_1920X1080P24, 0x1e, 0x04 },
	{ V4L2_DV_BT_CEA_1920X1080P25, 0x1e, 0x03 },
	{ V4L2_DV_BT_CEA_2880X480P60, 0x0a, 0x00 },
	{ V4L2_DV_BT_CEA_2880X576P60, 0x0a, 0x00 },
};

struct adv7612_state {
    struct v4l2_subdev sd;
    struct v4l2_ctrl_handler hdl;
    struct adv7612_platform_data *pdata;
    enum adv7612_mode mode;
    u8 edid[256];
    struct workqueue_struct *work_queues;
    struct delayed_work delayed_work_enable_hotplug;
    struct v4l2_dv_timings timings;

    /* i2c clients */
    struct i2c_client *i2c_cec;
    struct i2c_client *i2c_infoframe;
    struct i2c_client *i2c_rep;
    struct i2c_client *i2c_edid;
    struct i2c_client *i2c_hdmi;
    struct i2c_client *i2c_cp;
};

/* ----------------------------------------------------------------------- */

static inline struct adv7612_state *to_state(struct v4l2_subdev *sd)
{
    return container_of(sd, struct adv7612_state, sd);
}

static inline struct v4l2_subdev *to_sd(struct v4l2_ctrl *ctrl)
{
    return &container_of(ctrl->handler, struct adv7612_state, hdl)->sd;
}

/* ----------------------------------------------------------------------- */

static s32 adv_smbus_read_byte_data_check(struct i2c_client *client,
    	                                  u8 command, 
                                          bool check)
{
    union i2c_smbus_data data;
    int err;

    /* error if no client or no platform data */
    if (!client)
        return -EIO;

    /* read byte data from device */
    if (!(err = i2c_smbus_xfer(client->adapter, 
                               client->addr, 
                               client->flags,
    		                   I2C_SMBUS_READ, 
                               command, 
                               I2C_SMBUS_BYTE_DATA, 
                               &data))) {
    	return data.byte;
    }
    if (check) {
    	v4l_err(client, "ADV7612: %s: error %d reading %02x, %02x\n", 
                __func__, err, client->addr, command);
    }
    return -1;
}

static s32 adv_smbus_read_byte_data(struct i2c_client *client, u8 command)
{
    return adv_smbus_read_byte_data_check(client, command, true);
}

static s32 adv_smbus_write_byte_data(struct i2c_client *client,
    				                 u8 command, 
                                     u8 value)
{
    union i2c_smbus_data data;
    int err;
    int retries;

    /* error if no client or no platform data */
    if (!client)
        return -EIO;

    /* write byte data from device */
    data.byte = value;
    for (retries = 0; retries < 3; retries++) {
    	err = i2c_smbus_xfer(client->adapter, 
                             client->addr, 
                             client->flags,
    			             I2C_SMBUS_WRITE, 
                             command, 
                             I2C_SMBUS_BYTE_DATA, 
                             &data);
    	if (!err)
    		break;
    }
    if (err < 0) {
    	v4l_err(client, "ADV7612: %s: error %d writing %02x, %02x, %02x\n", 
                __func__, err, client->addr, command, value);
    }
    return err;
}

static s32 adv_smbus_write_i2c_block_data(struct i2c_client *client,
	                                      u8 command, 
                                          unsigned length, 
                                          const u8 *values)
{
    union i2c_smbus_data data;
    int err = 0;

    /* error if no client or no platform data */
    if (!client) 
        return -EIO;

    /* write block data to device */
    if (length > I2C_SMBUS_BLOCK_MAX)
    	length = I2C_SMBUS_BLOCK_MAX;
    data.block[0] = length;
    memcpy(data.block + 1, values, length);

    err = i2c_smbus_xfer(client->adapter, 
                         client->addr, 
                         client->flags,
    		             I2C_SMBUS_WRITE, 
                         command,
    		             I2C_SMBUS_I2C_BLOCK_DATA, 
                         &data);
    if (err < 0)
        v4l_err(client, "ADV7612: %s: error %d.\n", __func__, err);
    return err;
}

/* ----------------------------------------------------------------------- */

static inline int io_read(struct v4l2_subdev *sd, u8 reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    return adv_smbus_read_byte_data(client, reg);
}

static inline int io_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    return adv_smbus_write_byte_data(client, reg, val);
}

static inline int io_write_and_or(struct v4l2_subdev *sd, u8 reg, u8 mask, u8 val)
{
    return io_write(sd, reg, (io_read(sd, reg) & mask) | val);
}

static inline int cec_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_read_byte_data(state->i2c_cec, reg);
}

static inline int cec_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_write_byte_data(state->i2c_cec, reg, val);
}

static inline int cec_write_and_or(struct v4l2_subdev *sd, u8 reg, u8 mask, u8 val)
{
    return cec_write(sd, reg, (cec_read(sd, reg) & mask) | val);
}

static inline int infoframe_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_read_byte_data(state->i2c_infoframe, reg);
}

static inline int infoframe_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_write_byte_data(state->i2c_infoframe, reg, val);
}

static inline int rep_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_read_byte_data(state->i2c_rep, reg);
}

static inline int rep_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_write_byte_data(state->i2c_rep, reg, val);
}

static inline int rep_write_and_or(struct v4l2_subdev *sd, u8 reg, u8 mask, u8 val)
{
    return rep_write(sd, reg, (rep_read(sd, reg) & mask) | val);
}

static inline int hdmi_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_read_byte_data(state->i2c_hdmi, reg);
}

static inline int hdmi_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_write_byte_data(state->i2c_hdmi, reg, val);
}

static inline int hdmi_write_and_or(struct v4l2_subdev *sd, u8 reg, u8 mask, u8 val)
{
    return hdmi_write(sd, reg, (hdmi_read(sd, reg) & mask) | val);
}

static inline int cp_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_read_byte_data(state->i2c_cp, reg);
}

static inline int cp_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_write_byte_data(state->i2c_cp, reg, val);
}

static inline int cp_write_and_or(struct v4l2_subdev *sd, u8 reg, u8 mask, u8 val)
{
    return cp_write(sd, reg, (io_read(sd, reg) & mask) | val);
}

static inline int edid_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_read_byte_data(state->i2c_edid, reg);
}

static inline int edid_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7612_state *state = to_state(sd);
    return adv_smbus_write_byte_data(state->i2c_edid, reg, val);
}

static inline int edid_read_block(struct v4l2_subdev *sd, 
                                  unsigned len, 
                                  u8 *val)
{
    struct adv7612_state *state = to_state(sd);
    struct i2c_client *client = state->i2c_edid;
    u8 msgbuf0[1] = { 0 };
    u8 msgbuf1[256];
    struct i2c_msg msg[2] = { 
        { client->addr, 0, 1, msgbuf0 },
        { client->addr, 0 | I2C_M_RD, len, msgbuf1 }
    };

    if (i2c_transfer(client->adapter, msg, 2) < 0)
        return -EIO;
    else 
        memcpy(val, msgbuf1, len);
    v4l2_dbg(2, debug, sd, "%s: returning\n", __func__);

    return 0;
}

static inline int edid_write_block(struct v4l2_subdev *sd,
    				               unsigned len, 
                                   const u8 *val)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    struct adv7612_state *state = to_state(sd);
    int i, rc = 0;

    /* Disables I2C access to internal EDID ram from DDC port */
    rep_write_and_or(sd, 0x74, 0xf0, 0x0);
    for (i = 0; !rc && i < len; i += I2C_SMBUS_BLOCK_MAX) {
        rc = adv_smbus_write_i2c_block_data(state->i2c_edid, i,
    			I2C_SMBUS_BLOCK_MAX, val + i);
    }

    if (!rc) {
        /* adv7612 calculates the checksums and enables I2C access 
           to internal EDID ram from DDC port. */
        rep_write_and_or(sd, 0x74, 0xf0, 0x1);
        for (i = 0; i < 1000; i++) {
            if (rep_read(sd, 0x76) & 1)
                break;
            mdelay(1);
        }
        if (i == 1000) {
            v4l_err(client, "error enabling edid\n");
            rc = -EIO;
        }
    }

    /* enable hotplug after 100 ms */
    queue_delayed_work(state->work_queues,
		    &state->delayed_work_enable_hotplug, HZ / 10);

    v4l2_dbg(2, debug, sd, "%s: returning %d\n", __func__, rc);

    return rc;
}

static void adv7612_delayed_work_enable_hotplug(struct work_struct *work)
{
    struct delayed_work *dwork = to_delayed_work(work);
    struct adv7612_state *state = container_of(dwork, struct adv7612_state,
                                        delayed_work_enable_hotplug);
    struct v4l2_subdev *sd = &state->sd;

    v4l2_dbg(2, debug, sd, "%s: enable hotplug\n", __func__);

    hdmi_write_and_or(sd, 0x6c, 0xfe, 0x01);	/* enable HPA manual control */
    v4l2_subdev_notify(sd, ADV7612_HOTPLUG, (void *)1);
    io_write_and_or(sd, 0x20, 0xf3, 0xf8);	/* maually assert hot plug on A */
    hdmi_write_and_or(sd, 0x6c, 0xfe, 0x00);	/* enable HPA auto control */
}

/* ----------------------------------------------------------------------- */

static inline bool no_power(struct v4l2_subdev *sd)
{
    /* Entire chip or CP powered off */
    return io_read(sd, 0x0c) & 0x24;
}

static inline bool no_lock_tmds(struct v4l2_subdev *sd)
{
    /* TMDS PLL on port A locked to the incoming clock  */
    return !(io_read(sd, 0x6a) & 0x40);
}

static inline bool no_signal_tmds(struct v4l2_subdev *sd)
{
    /* No TMDS clock detected on port A */
    return !(io_read(sd, 0x6a) & 0x10);
}

static inline bool no_lock_deregen(struct v4l2_subdev *sd)
{
    /* DE regneration filter not locked */
    return !(hdmi_read(sd, 0x07) & 0x20);
}

static inline bool no_signal(struct v4l2_subdev *sd)
{
    bool ret;

    ret = no_power(sd);
    ret |= no_signal_tmds(sd);
    ret |= no_lock_tmds(sd);

    return ret;
}

static int find_and_set_predefined_video_timings(struct v4l2_subdev *sd, 
        u8 prim_mode, const struct adv7612_video_standards *predef_vid_timings,
        const struct v4l2_dv_timings *timings)
{
    int i;

    for (i = 0; predef_vid_timings[i].timings.bt.width; i++) {
        if (!v4l_match_dv_timings(timings, &predef_vid_timings[i].timings, 
                    250000)) 
            continue;
        io_write(sd, 0x00, predef_vid_timings[i].vid_std); /* video std */
        io_write(sd, 0x01, (predef_vid_timings[i].v_freq << 4) 
                + prim_mode);                   /* v_freq and prim mode */
            return 0;
    }
    return -1;
}

/* See ADV7612 Register Settings Recommendations Rev 1.2 Nov 2011 */
static int configure_predefined_video_timings(struct v4l2_subdev *sd, 
        struct v4l2_dv_timings *timings)
{
    struct adv7612_state *state = to_state(sd);
    int err;

    v4l2_dbg(1, debug, sd, "%s", __func__);

    switch (state->mode) {
    case ADV7612_MODE_HDMI_COMP:
        err = find_and_set_predefined_video_timings(sd, state->mode,
                adv7612_mode_hdmi_comp, timings);
        break;
    case ADV7612_MODE_HDMI_GR:
        err = find_and_set_predefined_video_timings(sd, state->mode,
                adv7612_mode_hdmi_gr, timings);
        break;
    default:
        v4l2_dbg(2, debug, sd, "%s: Unknown mode %d\n", __func__, state->mode);
        err = -1;
    }
    if (err < 0) {
        v4l2_dbg(1, debug, sd, "%s: failed to configure predefined timings", 
                __func__);
    }
    return err;
} 

static int configure_custom_video_timings(struct v4l2_subdev *sd,
		const struct v4l2_dv_timings *timings)
{
	struct adv7612_state *state = to_state(sd);
    int err;

    v4l2_dbg(2, debug, sd, "%s\n", __func__);

    state->mode = 0x05;         /* ADV7612_MODE_HDMI_COMP */

	/* set default prim_mode/vid_std for HDMI.     */ 
    /* ADV7612_Hardware_Users_Guide_Rev0.pdf, p.23 */
    err = find_and_set_predefined_video_timings(sd, 0x05, 
             adv7612_mode_hdmi_861_frun, timings);
    if (err < 0) {
        v4l2_dbg(1, debug, sd, "%s: failed to configure predefined timings", 
                __func__);
    }
    return err;
}

/* See ADV7612 eval board script ADV7612-VER.5.9c_AVEB.txt 6-1c */
static int adv7612_core_init(struct v4l2_subdev *sd)
{
    struct adv7612_state *state = to_state(sd);
    const struct adv7612_platform_data *pdata = state->pdata;

    /* based on the "6-1f Port A, 1080p Any Colorspace In (YCrCb 444 
     * 36bit from ADV761x)" section in in ADV7612-VER.2.9c_AVEB.txt */
    io_write(sd, 0x01, 0x06);   /* prim mode = 110b HDMI-GR */
    io_write(sd, 0x02, 0x12);   /* rgb in, RGB out, clear op_656 bit */
    io_write(sd, 0x03, 0x42);   /* 36 bit 4:4:4 SDR mode */
    io_write(sd, 0x05, 0x28);   /* AV codes off */
    io_write(sd, 0x06, 0xa6);   /* invert HS, VS pins */
    io_write(sd, 0x0b, 0x44);   /* power up part */
    io_write(sd, 0x0c, 0x42);   /* power up part */
    io_write(sd, 0x14, 0x7f);   /* max drive strength */
    io_write(sd, 0x15, 0x90);   /* trisate audio, disable tristate sync pins */
    io_write(sd, 0x19, 0x80|((pdata->flags&ADV7612_FLAG_LLC_PHASE_MASK)>>4));
                                /* LLC phase */
    io_write(sd, 0x33, 0x40);   /* LLC DLL enable */
    io_write(sd, 0xbf, 0x01);   /* CP_COMPLETE_BYPASS_IN_HDMI_MODE*/
    mdelay(100);
    cp_write(sd, 0xba, 0x00);   /* disable HDMI freerun mode*/
    rep_write(sd, 0x40, 0x81);  /* disable HDCP 1.1 features */
    hdmi_write(sd, 0x9b, 0x03); /* ADI recommended setting */
    hdmi_write(sd, 0x00, 0x80); /* Set HDMI input port A,dual link slave HDCP */
    hdmi_write(sd, 0x01, 0x01); /* enable Termination automatic control*/
    hdmi_write(sd, 0x02, 0x01); /* enable port A in background */
    hdmi_write(sd, 0x83, 0xfe); /* enable clock terminators for port A */
    hdmi_write(sd, 0x6f, 0x0c); /* ADI recommended setting */
    hdmi_write(sd, 0x85, 0x1f); /* ADI recommended setting  */
    hdmi_write(sd, 0x87, 0x70); /* ADI recommended setting  */
    hdmi_write(sd, 0x8d, 0x04); /* LFG port A */
    hdmi_write(sd, 0x8e, 0x1e); /* HFG port A */
    hdmi_write(sd, 0x96, 0x01); /* enable EQ_DYN_EN*/
    hdmi_write(sd, 0x57, 0xda); /* ADI recommended setting */
    hdmi_write(sd, 0x58, 0x01); /* ADI recommended setting */
    hdmi_write(sd, 0x03, 0x98); /* set DIS_I2C_ZERO_COMPR 0x03[7]=1 */
    hdmi_write(sd, 0x40, 0x40); /* override deep color to 8 bit */
    hdmi_write(sd, 0x4c, 0x44); /* set NEW_VS_PARAM 0x44[2] = 1*/
    hdmi_write(sd, 0x75, 0x10); /* DDC drive strength */
    hdmi_write(sd, 0x90, 0x04); /* LFG port B */
    hdmi_write(sd, 0x91, 0x1e); /* HFG port B */
    hdmi_write(sd, 0xc1, 0x01); /* Registers 0xc1 to 0xcc */
    hdmi_write(sd, 0xc2, 0x01); /* are set to values in   */
    hdmi_write(sd, 0xc3, 0x01); /* Section 1.2 ADV7612    */
    hdmi_write(sd, 0xc4, 0x01); /* Register Settings      */ 
    hdmi_write(sd, 0xc5, 0x01); /* Recommendations Rev1.2 */
    hdmi_write(sd, 0xc6, 0x01); /* dated November 2011    */
    hdmi_write(sd, 0xc7, 0x01);
    hdmi_write(sd, 0xc8, 0x01);
    hdmi_write(sd, 0xc9, 0x01);
    hdmi_write(sd, 0xca, 0x01);
    hdmi_write(sd, 0xcb, 0x01);
    hdmi_write(sd, 0xcc, 0x01);
    return 0;
}

/* See ADV7612 eval board script ADV7612-VER.2.9c_AVEB.txt */
static void enable_input(struct v4l2_subdev *sd, 
                         enum adv7612_mode mode)
{
    if (mode == ADV7612_MODE_HDMI_COMP || mode == ADV7612_MODE_HDMI_GR) {
        adv7612_core_init(sd);      /* re-initialize all registers */
        hdmi_write(sd, 0x1a, 0x8a); /* unmute audio */
        hdmi_write(sd, 0x83, 0xfc); /* enable clock terminators for ports A&B */
        io_write(sd, 0x15, 0x80);   /* disable tristate of pins */
    }
    else {
        v4l2_err(sd, "%s: reserved primary mode 0x%0x\n", __func__, mode);
    }
}

static void disable_input(struct v4l2_subdev *sd)
{
    /* disable */
    hdmi_write(sd, 0x1a, 0x9a); /* Mute audio */
    io_write(sd, 0x15, 0xbe);   /* Tristate audio and video outputs pins */
    hdmi_write(sd, 0x83, 0xff); /* Disable HDMI clock terminators */
}

/* See "ADV7612 Register Settings Recommendations (rev.1.2, Nov 2010)" p.4. */
static void select_input(struct v4l2_subdev *sd, 
                         enum adv7612_mode mode)
{
    if (mode == ADV7612_MODE_HDMI_COMP || mode == ADV7612_MODE_HDMI_GR) {
        io_write(sd, 0x01, mode);  /* prim mode */
        io_write(sd, 0x00, 0x02);  /* default video standard */
        hdmi_write(sd, 0x00, 0x00);/* HDMI input port A(BG_MEAS_PORT_SEL=001B)*/
    }
    else {
        v4l2_err(sd, "%s: reserved primary mode 0x%0x\n", __func__, mode);
    }
}

static void print_dv_timings(struct v4l2_subdev *sd,
                             struct v4l2_dv_timings *dv,
                             const char *txt, bool detailed)
{
    struct v4l2_bt_timings *bt;
    u32 htot, vtot;

    if (!sd || !dv)
        return;

    bt = &dv->bt;

    htot = bt->width + bt->hfrontporch + bt->hsync + bt->hbackporch;
    vtot = bt->height + bt->vfrontporch + bt->vsync + bt->vbackporch;

    v4l2_info(sd, "%s %dx%d%s%d (%dx%d)",
                  txt, bt->width, bt->height, bt->interlaced ? "i" : "p",
                  (htot * vtot) > 0 ? ((u32)bt->pixelclock / (htot * vtot)) : 0,
                  htot, vtot);

    if (detailed) {
        v4l2_info(sd, "    horizontal: fp = %d, %ssync = %d, bp = %d\n",
                      bt->hfrontporch,
                      (bt->polarities & V4L2_DV_HSYNC_POS_POL) ? "+" : "-",
                      bt->hsync, bt->hbackporch);
        v4l2_info(sd, "    vertical: fp = %d, %ssync = %d, bp = %d\n",
                      bt->vfrontporch,
                      (bt->polarities & V4L2_DV_VSYNC_POS_POL) ? "+" : "-",
                      bt->vsync, bt->vbackporch);
        v4l2_info(sd, "    pixelclock: %lld\n", bt->pixelclock);
    }
}

/* ----------------------------------------------------------------------- */

int adv7612_query_dv_timings(struct v4l2_subdev *sd,
                             struct v4l2_dv_timings *timings)
{
	struct adv7612_state *state = to_state(sd);
    struct v4l2_bt_timings *bt = &(timings->bt);
    unsigned int tmdsfreq, tmdsfreq_frac;

    if (!sd || !timings) {
        return -EINVAL;
    }

    memset(timings, 0, sizeof(struct v4l2_dv_timings));

    if (no_signal(sd)) {
        v4l2_dbg(2, debug, sd, "%s: no valid signal\n", __func__);
        return -ENOLINK;
    }

    timings->type = V4L2_DV_BT_656_1120;
    tmdsfreq_frac = hdmi_read(sd, 0x52);
    tmdsfreq = (hdmi_read(sd, 0x51) << 1) | ((tmdsfreq_frac & 0x80) >> 7);
    tmdsfreq_frac &= 0x7f;

    if (no_lock_deregen(sd)) {
        v4l2_dbg(2, debug, sd, "%s: DE regeneration not locked\n", 
                    __func__);
        return -ENOLINK;
    }

    bt->width = (hdmi_read(sd, 0x07) & 0x1f) * 256 + hdmi_read(sd, 0x08);
    bt->height = (hdmi_read(sd, 0x09) & 0x1f) * 256 + hdmi_read(sd, 0x0a);
    bt->pixelclock = (tmdsfreq * 1000000) + (tmdsfreq_frac * 1000000) / 128;
    bt->hfrontporch = (hdmi_read(sd, 0x20) & 0x1f) * 256 + 
        hdmi_read(sd, 0x21);
    bt->hsync = (hdmi_read(sd, 0x22) & 0x07) * 256 + 
        hdmi_read(sd, 0x23);
    bt->hbackporch = (hdmi_read(sd, 0x24) & 0x1f) * 256 + 
        hdmi_read(sd, 0x25);
    bt->vfrontporch = ((hdmi_read(sd, 0x2a) & 0x07) * 256 + 
            hdmi_read(sd, 0x2b)) / 2;
    bt->vsync = ((hdmi_read(sd, 0x2e) & 0x1f) * 256 + 
            hdmi_read(sd, 0x2f)) / 2;
    bt->vbackporch = ((hdmi_read(sd, 0x32) & 0x1f) * 256 + 
            hdmi_read(sd, 0x33)) / 2;
    bt->polarities = ((hdmi_read(sd, 0x05) & 0x10) ? 
                V4L2_DV_VSYNC_POS_POL : 0) | ((hdmi_read(sd, 0x05) & 0x20) ? 
                V4L2_DV_HSYNC_POS_POL : 0);
    if (bt->interlaced == V4L2_DV_INTERLACED) { 
        v4l2_dbg(2, debug, sd, "%s: INTERLACED not supported yet\n", __func__);
        return -EINVAL;
    }

    memcpy(&state->timings, &timings, sizeof(struct v4l2_dv_timings)); 

    if (state->pdata->flags & ADV7612_FLAG_VERBOSE)
        print_dv_timings(sd, timings, __func__, true);

    return 0;
}
EXPORT_SYMBOL(adv7612_query_dv_timings);

/* ----------------------------------------------------------------------- */

static void adv7612_inv_register(struct v4l2_subdev *sd)
{
    v4l2_info(sd, "0x000-0x0ff: IO Map\n");
    v4l2_info(sd, "0x100-0x1ff: CEC Map\n");
    v4l2_info(sd, "0x200-0x2ff: InfoFrame Map\n");
    v4l2_info(sd, "0x300-0x3ff: DPLL Map\n");
    v4l2_info(sd, "0x400-0x4ff: Repeater Map\n");
    v4l2_info(sd, "0x500-0x5ff: EDID Map\n");
    v4l2_info(sd, "0x600-0x6ff: HDMI Map\n");
    v4l2_info(sd, "0x700-0x7ff: CP Map\n");
}

static int adv7612_g_register(struct v4l2_subdev *sd,
                              struct v4l2_dbg_register *reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    if (!v4l2_chip_match_i2c_client(client, &reg->match))
    	return -EINVAL;
    if (!capable(CAP_SYS_ADMIN))
    	return -EPERM;
    switch (reg->reg >> 8) {
    case 0:
    	reg->val = io_read(sd, reg->reg & 0xff);
    	break;
    case 1:
    	reg->val = cec_read(sd, reg->reg & 0xff);
    	break;
    case 2:
    	reg->val = infoframe_read(sd, reg->reg & 0xff);
    	break;
    case 4:
    	reg->val = rep_read(sd, reg->reg & 0xff);
    	break;
    case 5:
    	reg->val = edid_read(sd, reg->reg & 0xff);
    	break;
    case 6:
    	reg->val = hdmi_read(sd, reg->reg & 0xff);
    	break;
    case 7:
    	reg->val = cp_read(sd, reg->reg & 0xff);
    	break;
    default:
    	v4l2_info(sd, "Register %03llx not supported\n", reg->reg);
    	adv7612_inv_register(sd);
    	break;
    }
    return 0;
}

static int adv7612_s_register(struct v4l2_subdev *sd,
                              struct v4l2_dbg_register *reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    if (!v4l2_chip_match_i2c_client(client, &reg->match))
    	return -EINVAL;
    if (!capable(CAP_SYS_ADMIN))
    	return -EPERM;
    switch (reg->reg >> 8) {
    case 0:
    	io_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 1:
    	cec_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 2:
    	infoframe_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 4:
    	rep_write(sd, reg->reg & 0xff, reg->val & 0xff);
        break;
    case 5:
    	edid_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 6:
    	hdmi_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 7:
    	cp_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    default:
    	v4l2_info(sd, "Register %03llx not supported\n", reg->reg);
    	adv7612_inv_register(sd);
    	break;
    }
    return 0;
}

static int adv7612_s_ctrl(struct v4l2_ctrl *ctrl)
{
    struct v4l2_subdev *sd = to_sd(ctrl);
    int rc = 0;

    switch (ctrl->id) {
    case V4L2_CID_BRIGHTNESS:
    	cp_write(sd, 0x3c, ctrl->val);
        break;
    case V4L2_CID_CONTRAST:
    	cp_write(sd, 0x3a, ctrl->val);
        break;
    case V4L2_CID_SATURATION:
    	cp_write(sd, 0x3b, ctrl->val);
        break;
    case V4L2_CID_HUE:
    	cp_write(sd, 0x3d, ctrl->val);
        break;
    case V4L2_CID_AUDIO_MUTE:
        hdmi_write(sd, 0x1a, (ctrl->val) ? 0x98 : 0x88);
        break;
    default:
        rc = -EINVAL;
        break;
    }
    return rc;
}

static int adv7612_g_chip_ident(struct v4l2_subdev *sd,
    				            struct v4l2_dbg_chip_ident *chip)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_ADV7612, 0);
}

static int adv7612_g_edid(struct v4l2_subdev *sd)
{
    struct adv7612_state *state = to_state(sd);
    int err;

    err = edid_read_block(sd, ARRAY_SIZE(state->edid), state->edid);
    if (err < 0)
        v4l2_err(sd, "%s: error reading edid\n", __func__);

    return err;
}

static int adv7612_s_edid(struct v4l2_subdev *sd)
{
    struct adv7612_state *state = to_state(sd);
    int err;

    err = edid_write_block(sd, ARRAY_SIZE(state->edid), state->edid);
    if (err < 0)
        v4l2_err(sd, "%s: error %d writing edid.\n",  __func__, err);

    return err;
}

static int adv7612_log_status(struct v4l2_subdev *sd)
{
    u32 lock, type, interlace, width, height;

    v4l2_info(sd, "5V:      %d\n",         /* CABLE_DETECT_A_RAW */
                  !!(io_read(sd, 0x6f) & 0x01));      
    v4l2_info(sd, "TMDS:    %d\n",         /* TMDS_CLK_A_RAW */
                  !!(io_read(sd, 0x6a) & 0x10));     
    v4l2_info(sd, "EDID:    %d\n",         /* EDID_A_ENABLE_CPU */
                  !!(rep_read(sd, 0x76) & 0x1));      
    v4l2_info(sd, "EDen:    %d\n",         /* EDID_A_ENABLE*/
                  !!(rep_read(sd, 0x74) & 0x1));      
    v4l2_info(sd, "CEC:     %s\n",         /* CEC_POWER_UP */
    	          !!(cec_read(sd, 0x2a) & 0x01) ? "enabled" : "disabled");

    lock =                              /* TDMS_PLL_LOCKED */
        (hdmi_read(sd, 0x04) & 2) ? 1 : 0;
    type =                              /* HDMI_MODE */
        (hdmi_read(sd, 0x05) & 0x80) ? 1 : 0;
    interlace =                         /* HDMI_INTERLACED */
        (hdmi_read(sd, 0x0b) & 0x20) ? 1 : 0;
    width =                            /* LINE_WIDTH */
        (hdmi_read(sd, 0x7) & 0x1f) * 256 + hdmi_read(sd, 0x8);
    height =                           /* FIELD0_HEIGHT */
    	(hdmi_read(sd, 0x9) & 0x1f) * 256 + hdmi_read(sd, 0xa);

    v4l2_info(sd, "Lock: %d\n", lock);
    v4l2_info(sd, "Type: %s\n", (type) ?  "HDMI" : "DVI-D");
    v4l2_info(sd, "Interlaced: %s\n", (interlace) ? 
        "Interlaced" : "Progressive");
    v4l2_info(sd, "wxh:  %dx%d\n", height, width);
    v4l2_info(sd, "\n");

    return 0;
}

static int adv7612_s_routing(struct v4l2_subdev *sd,
     			             u32 input, 
                             u32 output, 
                             u32 config)
{
    struct adv7612_state *state = to_state(sd);

    v4l2_dbg(2, debug, sd, "%s: input %d", __func__, input);
    state->mode = input;
    disable_input(sd);
    select_input(sd, state->mode);
    enable_input(sd, state->mode);

    v4l2_dbg(2, debug, sd, "%s: returning 0", __func__);

    return 0;
}

static int adv7612_g_input_status(struct v4l2_subdev *sd, u32 *status)
{
    v4l2_dbg(1, debug, sd, "%s entered\n", __func__);

    *status = 0;
    *status |= no_power(sd) ? V4L2_IN_ST_NO_POWER : 0;
    *status |= no_signal_tmds(sd) ? V4L2_IN_ST_NO_SIGNAL : 0;

    v4l2_dbg(1, debug, sd, "%s returning\n", __func__);

    return 0;
}

static int adv7612_s_dv_timings(struct v4l2_subdev *sd,
    		                struct v4l2_dv_timings *timings)
{
    struct adv7612_state *state = to_state(sd);
    struct v4l2_bt_timings *bt;
    int err = 0;

    if (!timings)
        return -EINVAL;

    bt = &timings->bt;

    if (bt->pixelclock > 225000000) {
        v4l2_dbg(1, debug, sd, "%s: pixelclock out of range %d\n",
            __func__, (u32) bt->pixelclock); 
        return -ERANGE;
    }

    memcpy(&state->timings, timings, sizeof(struct v4l2_dv_timings));

    cp_write(sd, 0x91, bt->interlaced ? 0x40 : 0x00);

    /* Use prim_mode and vid_std when available */
    err = configure_predefined_video_timings(sd, timings);
    if (err < 0) {
        /* custom settings when the video format
           does not have prim_mode/vid_std */
        configure_custom_video_timings(sd, timings);
    }

    if (state->pdata->flags & ADV7612_FLAG_VERBOSE)
        print_dv_timings(sd, timings, __func__, true);

    return err;
}

static int adv7612_g_dv_timings(struct v4l2_subdev *sd,
    		                struct v4l2_dv_timings *timings)
{
    struct adv7612_state *state = to_state(sd);
    memcpy(timings, &state->timings, sizeof(struct v4l2_dv_timings)); 
    return 0;
}

static long adv7612_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
    int err;
    struct adv7612_state *state = to_state(sd);

    switch (cmd) {
    case V4L2_G_EDID: {
        err = adv7612_g_edid(sd);
        if (err < 0)
            v4l2_err(sd, "error %d reading edid\n", err);
        else
            memcpy(((struct v4l2_edid *)arg)->edid, state->edid, 
                    sizeof(state->edid));
        break;
    }
    case V4L2_S_EDID: {
        memcpy(state->edid, ((struct v4l2_edid *)arg)->edid, 
                sizeof(state->edid));
        err = adv7612_s_edid(sd);
        if (err < 0)
            v4l2_err(sd, "error %d writing edid\n", err);
        break;
    }
    case V4L2_G_REGISTER: {
        return adv7612_g_register(sd, (struct v4l2_dbg_register *) arg);
    }
    case V4L2_S_REGISTER: {
        return adv7612_s_register(sd, (struct v4l2_dbg_register *) arg);
    }
    default:
    	v4l2_err(sd, "%s: unknown ioctl 0x%08x\n", __func__, cmd);
    	err = -EINVAL;
    }
    return err;
}

/* ----------------------------------------------------------------------- */

static const struct v4l2_ctrl_ops adv7612_ctrl_ops = {
    .s_ctrl = adv7612_s_ctrl,
};

static const struct v4l2_subdev_core_ops adv7612_core_ops = {
    .g_chip_ident = adv7612_g_chip_ident,
    .log_status = adv7612_log_status,
    .ioctl = adv7612_ioctl,
#ifdef CONFIG_VIDEO_ADV_DEBUG
    .g_register = adv7612_g_register,
    .s_register = adv7612_s_register,
#endif
};

static const struct v4l2_subdev_video_ops adv7612_video_ops = {
    .s_routing = adv7612_s_routing,
    .g_input_status = adv7612_g_input_status,
    .s_dv_timings = adv7612_g_dv_timings,
    .g_dv_timings = adv7612_s_dv_timings,
};

static const struct v4l2_subdev_ops adv7612_ops = {
    .core = &adv7612_core_ops,
    .video = &adv7612_video_ops, 
};

/* ----------------------------------------------------------------------- */

static int adv7612_init_controls(struct adv7612_state *state) 
{
    struct v4l2_ctrl_handler *hdl = &state->hdl;
    int err = 0;

    v4l2_ctrl_handler_init(hdl, 5);

    /* add in ascending ID order */
    v4l2_ctrl_new_std(hdl, &adv7612_ctrl_ops,
    		V4L2_CID_BRIGHTNESS, -128, 127, 1, 0);
    v4l2_ctrl_new_std(hdl, &adv7612_ctrl_ops,
    		V4L2_CID_CONTRAST, 0, 255, 1, 128);
    v4l2_ctrl_new_std(hdl, &adv7612_ctrl_ops,
    		V4L2_CID_SATURATION, 0, 255, 1, 128);
    v4l2_ctrl_new_std(hdl, &adv7612_ctrl_ops,
    		V4L2_CID_HUE, 0, 128, 1, 0);
    v4l2_ctrl_new_std(hdl, &adv7612_ctrl_ops,
    		V4L2_CID_AUDIO_MUTE, 0, 1, 1, 0);
    state->sd.ctrl_handler = hdl;

    if (hdl->error) {
    	err = hdl->error;
        v4l2_ctrl_handler_free(hdl);
    }
    else {
        err = v4l2_ctrl_handler_setup(&state->hdl);
        if (err) 
            v4l2_ctrl_handler_free(&state->hdl);
    }

    return err;
}

static void adv7612_exit_controls(struct adv7612_state *state)
{
    v4l2_ctrl_handler_free(&state->hdl);
}

static void adv7612_unregister_clients(struct adv7612_state *state)
{
    if (state->i2c_cec)
    	i2c_unregister_device(state->i2c_cec);
    if (state->i2c_infoframe)
    	i2c_unregister_device(state->i2c_infoframe);
    if (state->i2c_rep)
    	i2c_unregister_device(state->i2c_rep);
    if (state->i2c_hdmi)
    	i2c_unregister_device(state->i2c_hdmi);
    if (state->i2c_cp)
    	i2c_unregister_device(state->i2c_cp);
    if (state->i2c_edid)
    	i2c_unregister_device(state->i2c_edid);
}

static struct i2c_client *adv7612_dummy_client(struct v4l2_subdev *sd,
    						u8 addr, u8 io_reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    if (addr) {
    	io_write(sd, io_reg, addr << 1);
    }
    return i2c_new_dummy(client->adapter, io_read(sd, io_reg) >> 1);
}

static int adv7612_probe(struct i2c_client *client,
    		         const struct i2c_device_id *id)
{
    struct adv7612_state *state;
    struct adv7612_platform_data *pdata;
    struct v4l2_subdev *sd;
    int err = 0;
    unsigned char rd_info_hi, rd_info_lo;

    v4l_info(client, "%s: probing device at address 0x%x (%s)\n",
    	ADV7612_DRIVER_NAME, client->addr, client->adapter->name);

    /* Check if the adapter supports the needed features */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
        err = -EIO;
        goto err;
    }

    /* platform data */
    pdata = client->dev.platform_data;
    if (pdata == NULL) {
        v4l_err(client, "no platform data!\n");
        err = -EIO;
        goto err;
    }

    rd_info_hi = adv_smbus_read_byte_data_check(client, ADV7612_REV_HI_REG, 
            false);
    rd_info_lo = adv_smbus_read_byte_data_check(client, ADV7612_REV_LO_REG, 
            false);
    if (rd_info_hi != ADV7612_REV_HI || rd_info_lo != ADV7612_REV_LO) {
    	v4l_err(client, "not an adv7612 on address 0x%x (RD_INFO %02x%02x)\n", 
            client->addr, rd_info_hi, rd_info_lo);
    	err = -ENODEV;
        goto err;
    }

    v4l_info(client, "ADV7612 revsion (0x%x%x) detected.\n", 
	    rd_info_hi, rd_info_lo); 

    state = kzalloc(sizeof(struct adv7612_state), GFP_KERNEL);
    if (state == NULL) {
    	v4l_err(client, "could not allocate adv7612_state memory!\n");
        err = -ENOMEM;
        goto err;
    }

    sd = &state->sd;
    v4l2_i2c_subdev_init(sd, client, &adv7612_ops);
    sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

    state->pdata = pdata;

    state->i2c_cec = adv7612_dummy_client(sd, pdata->i2c_cec, 0xf4);
    state->i2c_infoframe = adv7612_dummy_client(sd, pdata->i2c_infoframe, 0xf5);
    state->i2c_rep = adv7612_dummy_client(sd, pdata->i2c_repeater, 0xf9);
    state->i2c_hdmi = adv7612_dummy_client(sd, pdata->i2c_hdmi, 0xfb);
    state->i2c_cp = adv7612_dummy_client(sd, pdata->i2c_cp, 0xfd);
    state->i2c_edid = adv7612_dummy_client(sd, pdata->i2c_edid, 0xfa);

    if (!state->i2c_cec || !state->i2c_infoframe || !state->i2c_rep || 
            !state->i2c_hdmi || !state->i2c_cp || !state->i2c_edid) {
    	err = -ENOMEM;
    	v4l2_err(sd, "failed to create all i2c clients\n");
    	goto err_free_clients;
    }

    state->work_queues = create_singlethread_workqueue(client->name);
    if (!state->work_queues) {
        v4l2_err(sd, "could not create work queue\n");
        err = -ENOMEM;
        goto err_free_clients;
    }       

    INIT_DELAYED_WORK(&state->delayed_work_enable_hotplug,
                      adv7612_delayed_work_enable_hotplug); 
    err = adv7612_init_controls(state);
    if (err) { 
    	v4l_err(client, "error initializing V4L2 controls\n");
        goto err_work_queues;
    }
    err = adv7612_core_init(sd);
    if (err) {
    	v4l_err(client, "error initializing ADV7612!\n");
    	goto err_free_controls;
    }
    return 0;

err_free_controls:
    adv7612_exit_controls(state);

err_work_queues:
    cancel_delayed_work(&state->delayed_work_enable_hotplug);
    destroy_workqueue(state->work_queues);

err_free_clients:
    adv7612_unregister_clients(state);
    v4l2_device_unregister_subdev(sd);
    kfree(state);

err:
    v4l_info(client, "failed to initialize AD7612. err %d\n", err);

    return err;
}

/* ----------------------------------------------------------------------- */

static int adv7612_remove(struct i2c_client *client)
{
    struct v4l2_subdev *sd = i2c_get_clientdata(client);
    struct adv7612_state *state = to_state(sd);

    v4l_info(client, "%s: removing %s on address 0x%x (%s)\n", 
            ADV7612_DRIVER_NAME, client->name, client->addr << 1,
            client->adapter->name);
    adv7612_exit_controls(state);
    cancel_delayed_work(&state->delayed_work_enable_hotplug);
    destroy_workqueue(state->work_queues);
    adv7612_unregister_clients(state);
    v4l2_device_unregister_subdev(sd);
    kfree(state);

    v4l_info(client, "%s removed.\n", __func__);

    return 0;
}

/* ----------------------------------------------------------------------- */

static struct i2c_device_id adv7612_id[] = {
    { ADV7612_DRIVER_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, adv7612_id);

static struct i2c_driver adv7612_driver = {
    .driver = {
    	.owner = THIS_MODULE,
    	.name = ADV7612_DRIVER_NAME,
    },
    .probe = adv7612_probe,
    .remove = adv7612_remove,
    .id_table = adv7612_id,
};

static int __init adv7612_init(void)
{
    return i2c_add_driver(&adv7612_driver);
}

static void __exit adv7612_exit(void)
{
    i2c_del_driver(&adv7612_driver);
}

module_init(adv7612_init);
module_exit(adv7612_exit);
