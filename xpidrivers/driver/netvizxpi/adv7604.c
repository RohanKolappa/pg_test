/*
 * adv7604 - Based on the v4l2 adv7612 driver from Tandberg/Cisco
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/videodev2.h>
#include <linux/workqueue.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/platform-netvizxpi.h>

#include "v4l2-chip-ident.h"
#include "v4l2-hdmi.h"
#include "v4l2-dv-bt-timings.h"
#include "netvizxpi.h"
#include "lut_table.h"
#include "adv7604.h"

//#define SUPPORT_INTERRUPT

MODULE_DESCRIPTION("Netviz XPI adv7604 video decoder driver");
MODULE_AUTHOR("Barco IP Video Systems");
MODULE_LICENSE("GPL");

static int debug = 0;

module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "debug level, default is 0 (no debug messages)");

/*
 **********************************************************************
 *   EDID for 12 bit video, DSD and HBR audio enabled
 * (see ADV7612-Ver.2.9c_AVEB.txt for ADV7612 eval board)
 **********************************************************************
 */
static const u8 edid_hdmi[256] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, /* 0x00 */
    0x08, 0x32, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x32, 0x17, 0x01, 0x03, 0x80, 0x0c, 0x09, 0x78, /* 0x10 */
    0x0a, 0x1e, 0xac, 0x98, 0x59, 0x56, 0x85, 0x28,
    0x29, 0x52, 0x57, 0x03, 0xcf, 0x80, 0x81, 0xcd, /* 0x20 */
    0x81, 0x40, 0x95, 0x00, 0xb3, 0x00, 0x01, 0x01,
    0x81, 0x80, 0xa9, 0x40, 0x71, 0x40, 0x1a, 0x36, /* 0x30 */
    0x80, 0xa0, 0x70, 0x38, 0x1f, 0x40, 0x30, 0x20,
    0x35, 0x00, 0xc4, 0x8e, 0x21, 0x00, 0x00, 0x1e, /* 0x40 */
    0x21, 0x39, 0x90, 0x30, 0x62, 0x1a, 0x27, 0x40,
    0x68, 0xb0, 0x36, 0x00, 0xc4, 0x8e, 0x21, 0x00, /* 0x50 */
    0x00, 0x1e, 0x52, 0x24, 0x00, 0xa0, 0x51, 0xd0,
    0x1f, 0x20, 0x48, 0x88, 0x13, 0x00, 0xc4, 0x8e, /* 0x60 */
    0x21, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xfc,
    0x00, 0x58, 0x50, 0x69, 0x44, 0x69, 0x67, 0x69, /* 0x70 */
    0x74, 0x61, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x7e, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x80 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x90 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xa0 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xb0 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xc0 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xd0 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xe0 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xf0 */
};


/*
 **********************************************************************
 *  Supported video standards for ADV7604
 **********************************************************************
 */
struct adv7604_video_standards {
    struct v4l2_dv_timings timings;
    u8 vid_std;
    u8 v_freq;
    u16 lcf;
    u16 lcvs;
};

/*  sort by number of lines */
static const struct adv7604_video_standards adv7604_prim_mode_comp[] = {
	{ V4L2_DV_BT_CEA_720X576P50, 0x0b, 0x00, 0, 0 },
	{ V4L2_DV_BT_CEA_1280X720P50, 0x19, 0x01, 0, 0 },
	{ V4L2_DV_BT_CEA_1280X720P60, 0x19, 0x00, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P24, 0x1e, 0x04, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P25, 0x1e, 0x03, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P30, 0x1e, 0x02, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P50, 0x1e, 0x01, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P60, 0x1e, 0x00, 0, 0 },
	/* TODO add 1920x1080P60_RB (CVT timing) */
	{ },
};

/* sorted by number of lines */
static const struct adv7604_video_standards adv7604_prim_mode_gr[] = {
	{ V4L2_DV_BT_DMT_640X480P60, 0x08, 0, 497, 3 },
	{ V4L2_DV_BT_DMT_640X480P72, 0x09, 0, 501, 3 },
	{ V4L2_DV_BT_DMT_640X480P75, 0x0a, 0, 502, 3 },
	{ V4L2_DV_BT_DMT_640X480P85, 0x0b, 0, 505, 3 },
	{ V4L2_DV_BT_DMT_800X600P56, 0x00, 0, 625, 2 },
	{ V4L2_DV_BT_DMT_800X600P60, 0x01, 0, 628, 4 },
	{ V4L2_DV_BT_DMT_800X600P72, 0x02, 0, 666, 6 },
	{ V4L2_DV_BT_DMT_800X600P75, 0x03, 0, 625, 3 },
	{ V4L2_DV_BT_DMT_800X600P85, 0x04, 0, 631, 3 },
	{ V4L2_DV_BT_DMT_1024X768P60, 0x0c, 0, 806, 6 },
	{ V4L2_DV_BT_DMT_1024X768P70, 0x0d, 0, 806, 6 },
	{ V4L2_DV_BT_DMT_1024X768P75, 0x0e, 0, 800, 3 },
	{ V4L2_DV_BT_DMT_1024X768P85, 0x0f, 0, 808, 3 },
	{ V4L2_DV_BT_DMT_1280X1024P60, 0x05, 0, 1066, 3 },
	{ V4L2_DV_BT_DMT_1280X1024P75, 0x06, 0, 1066, 3 },
	{ V4L2_DV_BT_DMT_1360X768P60, 0x12, 0, 795, 6 },
	{ V4L2_DV_BT_DMT_1366X768P60, 0x13, 0, 798, 3 },
	{ V4L2_DV_BT_DMT_1400X1050P60, 0x14, 0, 1089, 4 },
	{ V4L2_DV_BT_DMT_1400X1050P75, 0x15, 0, 1099, 4 },
	{ V4L2_DV_BT_DMT_1600X1200P60, 0x16, 0, 1250, 3 }, 
	/* TODO add 1600X1200P60_RB (not a DMT timing) */
	{ V4L2_DV_BT_DMT_1680X1050P60, 0x18, 0, 1089, 6 },
	{ V4L2_DV_BT_DMT_1920X1200P60_RB, 0x19, 0, 1235, 6 }, 
	{ },
};

const int adv7604_prim_mode_gr_size = ARRAY_SIZE(adv7604_prim_mode_gr);

/* sorted by number of lines */
static const struct adv7604_video_standards adv7604_prim_mode_hdmi_comp[] = {
	{ V4L2_DV_BT_CEA_720X480P59_94, 0x0a, 0x00, 0, 0 },
	{ V4L2_DV_BT_CEA_720X576P50, 0x0b, 0x00, 0, 0 },
	{ V4L2_DV_BT_CEA_1280X720P50, 0x13, 0x01, 0, 0 },
	{ V4L2_DV_BT_CEA_1280X720P60, 0x13, 0x00, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P24, 0x1e, 0x04, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P25, 0x1e, 0x03, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P30, 0x1e, 0x02, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P50, 0x1e, 0x01, 0, 0 },
	{ V4L2_DV_BT_CEA_1920X1080P60, 0x1e, 0x00, 0, 0 },
	{ },
};

/* sorted by number of lines */
static const struct adv7604_video_standards adv7604_prim_mode_hdmi_gr[] = {
	{ V4L2_DV_BT_DMT_640X480P60, 0x08, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_640X480P72, 0x09, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_640X480P75, 0x0a, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_640X480P85, 0x0b, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_800X600P56, 0x00, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_800X600P60, 0x01, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_800X600P72, 0x02, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_800X600P75, 0x03, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_800X600P85, 0x04, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_1024X768P60, 0x0c, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_1024X768P70, 0x0d, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_1024X768P75, 0x0e, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_1024X768P85, 0x0f, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_1280X1024P60, 0x05, 0x00, 0, 0 },
	{ V4L2_DV_BT_DMT_1280X1024P75, 0x06, 0x00, 0, 0 },
	{ },
};

/*
 **********************************************************************
 *  ADV7604 device structure
 **********************************************************************
 */

struct adv7604_state {
    struct v4l2_subdev sd;
    struct v4l2_ctrl_handler hdl;
    struct mutex lock;
    struct adv7604_platform_data *pdata;
    struct mutex pdata_lock;
    int mode;
    u8 edid[256];
    unsigned edid_blocks;
    struct workqueue_struct *work_queues;
    struct delayed_work delayed_work_enable_hotplug;
    struct v4l2_dv_timings timings;
    struct v4l2_fract aspect_ratio;
    u32 rgb_quantization_range;
    bool connector_hdmi;
    struct stdi_readback stdi;
    bool restart_stdi_once;
    u8 curr_vid_std;
    u8 curr_add_status;     /* bit 7: 1 if vert sync filter locked
                               bit 6: 1 if DE regen block locked on incoming DE
                               bit 5: 1 if TMDS PLL locked on incoming TMDS clk
                               bit 4: 1 if TMDS clock detected on port A 
                               bit 1: 1 if digital input detected
                               bit 0: 1 if analog input detected */ 
                                   
    /* i2c clients */
    struct i2c_client *i2c_client;
    struct i2c_client *i2c_avlink;
    struct i2c_client *i2c_cec;
    struct i2c_client *i2c_infoframe;
    struct i2c_client *i2c_esdp;
    struct i2c_client *i2c_dpp;
    struct i2c_client *i2c_afe;
    struct i2c_client *i2c_repeater;
    struct i2c_client *i2c_edid;
    struct i2c_client *i2c_hdmi;
    struct i2c_client *i2c_test;
    struct i2c_client *i2c_cp;
    struct i2c_client *i2c_vdp;

    /* controls */
    struct v4l2_ctrl *detect_tx_5v_ctrl;
    struct v4l2_ctrl *analog_sampling_phase_ctrl;
    struct v4l2_ctrl *free_run_color_manual_ctrl;
    struct v4l2_ctrl *free_run_color_ctrl;
    struct v4l2_ctrl *rgb_quantization_range_ctrl;
};

/* ----------------------------------------------------------------------- */

static inline struct adv7604_state *to_state(struct v4l2_subdev *sd)
{
    return container_of(sd, struct adv7604_state, sd);
}

static inline struct v4l2_subdev *to_sd(struct v4l2_ctrl *ctrl)
{
    return &container_of(ctrl->handler, struct adv7604_state, hdl)->sd;
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
        v4l_err(client, "ADV7604: %s: error %d reading %02x, %02x.\n", 
                __func__, err, client->addr, command);
    }
    return -EIO;
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

    /* write byte data to device */
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
    if (err < 0)
    	v4l_err(client, "ADV7612: %s: error %d writing %02x, %02x, %02x.\n", 
		    __func__, err, client->addr, command, value);
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
        v4l_err(client, "ADV7604: %s: error %d\n", __func__, err);
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

static inline int io_write_and_or(struct v4l2_subdev *sd, 
                                  u8 reg, 
                                  u8 mask, 
                                  u8 val)
{
    return io_write(sd, reg, (io_read(sd, reg) & mask) | val);
}

static inline int avlink_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_avlink, reg);
}

static inline int avlink_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);
 
    return adv_smbus_write_byte_data(state->i2c_avlink, reg, val);
}

static inline int cec_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);
 
    return adv_smbus_read_byte_data(state->i2c_cec, reg);
}

static inline int cec_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);
 
    return adv_smbus_write_byte_data(state->i2c_cec, reg, val);
}

static inline int cec_write_and_or(struct v4l2_subdev *sd, 
                                   u8 reg, 
                                   u8 mask, 
                                   u8 val)
{
    return cec_write(sd, reg, (cec_read(sd, reg) & mask) | val);
}

static inline int infoframe_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_infoframe, reg);
}

static inline int infoframe_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_infoframe, reg, val);
}

static inline int esdp_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_esdp, reg);
}

static inline int esdp_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_esdp, reg, val);
}

static inline int dpp_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_dpp, reg);
}

static inline int dpp_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_dpp, reg, val);
}

static inline int afe_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_afe, reg);
}

static inline int afe_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_afe, reg, val);
}

static inline int rep_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_repeater, reg);
}

static inline int rep_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_repeater, reg, val);
}

static inline int rep_write_and_or(struct v4l2_subdev *sd, 
                                   u8 reg, 
                                   u8 mask, 
                                   u8 val)
{
    return rep_write(sd, reg, (rep_read(sd, reg) & mask) | val);
}

static inline int hdmi_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_hdmi, reg);
}

static inline int hdmi_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_hdmi, reg, val);
}

static inline int test_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_test, reg);
}

static inline int test_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_test, reg, val);
}

static inline int cp_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_cp, reg);
}

static inline int cp_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_cp, reg, val);
}

static inline int cp_write_and_or(struct v4l2_subdev *sd, 
                                  u8 reg, 
                                  u8 mask, 
                                  u8 val)
{
    return cp_write(sd, reg, (cp_read(sd, reg) & mask) | val);
}

static inline int vdp_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_vdp, reg);
}

static inline int vdp_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_vdp, reg, val);
}

static inline int edid_read(struct v4l2_subdev *sd, u8 reg)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_read_byte_data(state->i2c_edid, reg);
}

static inline int edid_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct adv7604_state *state = to_state(sd);

    return adv_smbus_write_byte_data(state->i2c_edid, reg, val);
}

static inline int edid_read_block(struct v4l2_subdev *sd, 
                                  unsigned len, 
                                  u8 *val)
{
    struct adv7604_state *state = to_state(sd);
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

static void adv7604_delayed_work_enable_hotplug(struct work_struct *work)
{
    struct delayed_work *dwork = to_delayed_work(work);
    struct adv7604_state *state = container_of(dwork, struct adv7604_state,
    					delayed_work_enable_hotplug);
    struct v4l2_subdev *sd = &state->sd;

    v4l2_subdev_notify(sd, ADV7604_HOTPLUG, (void *)1);
}

static inline int edid_write_block(struct v4l2_subdev *sd,
    				               unsigned len, 
                                   const u8 *val)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    struct adv7604_state *state = to_state(sd);
    int i, rc = 0;

    /* Disables I2C access to internal EDID ram from DDC port */
    rep_write_and_or(sd, 0x77, 0xf0, 0x0);
    for (i = 0; !rc && i < len; i += I2C_SMBUS_BLOCK_MAX) {
        rc = adv_smbus_write_i2c_block_data(state->i2c_edid, i,
    			I2C_SMBUS_BLOCK_MAX, val + i);
    }

    if (!rc) {
        /* adv7604 calculates the checksums and enables I2C access 
           to internal EDID ram from DDC port. */
        rep_write_and_or(sd, 0x77, 0xf0, 0x1);
        for (i = 0; i < 1000; i++) {
            if (rep_read(sd, 0x7d) & 1)
                break;
            mdelay(1);
        }
        if (i == 1000) {
            v4l_err(client, "error enabling edid\n");
            rc = -EIO;
        }
    }

    /* enable hotplug after 100 ms */
    if (state->work_queues)
        queue_delayed_work(state->work_queues,
		    &state->delayed_work_enable_hotplug, HZ / 10);

    v4l2_dbg(2, debug, sd, "%s: returning %d\n", __func__, rc);

    return rc;
}

/* ------------------------------------------------------------------------- */

static inline bool digital_input(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);
    return (state->mode == NETVIZXPI_MODE_HDMI);
}

static inline bool no_power(struct v4l2_subdev *sd)
{
    /* Entire chip or CP powered off */
    return io_read(sd, 0x0c) & 0x24;
}

static inline bool no_signal_tmds(struct v4l2_subdev *sd)
{
    /* No TMDS_CLK_A_RAW */
    return !(io_read(sd, 0x6a) & 0x10);
}

static inline bool no_lock_tmds(struct v4l2_subdev *sd)
{
    /* No V_LOCKED_RAW, DE_REGEN_LCK_RAW or VIDEO_PLL_LCK_RAW on port A */
    return (io_read(sd, 0x6a) & 0xe0) != 0xe0;
}

static inline bool no_lock_sspd(struct v4l2_subdev *sd)
{
    /* No SSPD lock on port A */
    return ((cp_read(sd, 0xb5) & 0xd0) != 0xd0);
}

static inline bool no_lock_stdi(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);

    if (state->pdata->flags & ADV7604_FLAG_SKIP_STDI)
        return FALSE;
    else {
        /* No STDI lock on port A */
        return !(cp_read(sd, 0xb1) & 0x80);
    }
}

static inline bool no_lock_cp(struct v4l2_subdev *sd)
{
    /* CP has detected a non standard number of lines on the incoming
       video compared to what it is configured to receive by s_dv_timings */
    return io_read(sd, 0x12) & 0x01;
}

static inline bool no_signal(struct v4l2_subdev *sd)
{
    bool rc = no_power(sd);

    rc |= no_lock_stdi(sd);
    rc |= no_lock_sspd(sd);

    if (digital_input(sd)) {
        rc |= no_lock_tmds(sd);
        rc |= no_signal_tmds(sd);
    }
    return rc;
}

static inline unsigned hblanking(const struct v4l2_bt_timings *t)
{
    return t->hfrontporch + t->hsync + t->hbackporch;
}

static inline unsigned htotal(const struct v4l2_bt_timings *t)
{
    return t->width + t->hfrontporch + t->hsync + t->hbackporch;
}

static inline unsigned vblanking(const struct v4l2_bt_timings *t)
{
    return t->vfrontporch + t->vsync + t->vbackporch;
}

static inline unsigned vtotal(const struct v4l2_bt_timings *t)
{
    return t->height + t->vfrontporch + t->vsync + t->vbackporch;
}

/**
 * v4l_match_dv_timings - check if two timings match
 * @t1 - compare this v4l2_dv_timings struct...
 * @t2 - with this struct.
 * @pclock_delta - the allowed pixelclock deviation.
 *
 * Compare t1 with t2 with a given margin of error for the pixelclock.
 */
bool v4l_match_dv_timings(const struct v4l2_dv_timings *t1,
                          const struct v4l2_dv_timings *t2,
                          unsigned pclock_delta)
{
       if (t1->type != t2->type || t1->type != V4L2_DV_BT_656_1120)
               return false;
       if (t1->bt.width == t2->bt.width &&
           t1->bt.height == t2->bt.height &&
           t1->bt.interlaced == t2->bt.interlaced &&
           t1->bt.polarities == t2->bt.polarities &&
           t1->bt.pixelclock >= t2->bt.pixelclock - pclock_delta &&
           t1->bt.pixelclock <= t2->bt.pixelclock + pclock_delta &&
           t1->bt.hfrontporch == t2->bt.hfrontporch &&
           t1->bt.vfrontporch == t2->bt.vfrontporch &&
           t1->bt.vbackporch == t2->bt.vbackporch &&
           (!t1->bt.interlaced ||
               (t1->bt.il_vfrontporch == t2->bt.il_vfrontporch &&
                t1->bt.il_vsync == t2->bt.il_vsync &&
                t1->bt.il_vbackporch == t2->bt.il_vbackporch)))
               return true;
       return false;
}
EXPORT_SYMBOL(v4l_match_dv_timings);

static int read_stdi(struct v4l2_subdev *sd, struct stdi_readback *stdi)
{
    if (no_lock_stdi(sd) || no_lock_sspd(sd)) {
        v4l2_dbg(1, debug, sd, "%s: STDI and/or SSPD not locked\n", __func__);
        return -1;
    }

    /* read STDI */
    stdi->bl = ((cp_read(sd, 0xb1) & 0x3f) << 8) | cp_read(sd, 0xb2);
    stdi->lcf = ((cp_read(sd, 0xb3) & 0x7) << 8) | cp_read(sd, 0xb4);
    stdi->lcvs = cp_read(sd, 0xb3) >> 3;
    stdi->interlaced = io_read(sd, 0x12) & 0x10;
    stdi->fcl = ((cp_read(sd, 0xb8) & 0x1f) << 8) | cp_read(sd, 0xb9);

    /* read SSPD */
    if ((cp_read(sd, 0xb5) & 0x03) == 0x01) {
        stdi->hs_pol = ((cp_read(sd, 0xb5) & 0x10) ?
                        ((cp_read(sd, 0xb5) & 0x08) ? '+' : '-') : 'x');
        stdi->vs_pol = ((cp_read(sd, 0xb5) & 0x40) ?
                        ((cp_read(sd, 0xb5) & 0x20) ? '+' : '-') : 'x');
    } else {
        stdi->hs_pol = 'x';
        stdi->vs_pol = 'x';
    }
    if (no_lock_stdi(sd) || no_lock_sspd(sd)) {
        v4l2_dbg(1, debug, sd,
                "%s: signal lost during readout of STDI/SSPD\n", __func__);
        return -1;
    }
    if (stdi->lcf < 239 || stdi->bl < 8 || stdi->bl == 0x3fff) {
        v4l2_dbg(1, debug, sd, "%s: invalid signal\n", __func__);
        memset(stdi, 0, sizeof(struct stdi_readback));
        return -1;
    }

    return 0;
}

static void set_default_color(struct v4l2_subdev *sd, int enable_auto) 
{
    cp_write(sd, 0xbf, enable_auto ? 0x12 : 0x16);   
    if (!enable_auto) {
        cp_write(sd, 0xc0, 0x00);   /* ch_a (g) */
        cp_write(sd, 0xc1, 0x00);   /* ch_b (r) */
        cp_write(sd, 0xc2, 0x87);   /* ch_c (b) */
    }
}

static void enable_input(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);

    switch (state->mode) {
    case NETVIZXPI_MODE_COMP:
    case NETVIZXPI_MODE_GR:
    case NETVIZXPI_MODE_STDI:
        /* enable */
        io_write(sd, 0x15, 0xb0);   /* Disable Tristate of Pins (no audio) */
        break;
    case NETVIZXPI_MODE_HDMI:
        /* enable */
        hdmi_write(sd, 0x1a, 0x0a); /* Unmute audio */
        //hdmi_write(sd, 0x01, 0x00); /* Enable HDMI clock terminators */
        io_write(sd, 0x15, 0xa0);   /* Disable Tristate of Pins */
        break;
    default:
        v4l2_err(sd, "%s: unknown mode 0x%0x\n", __func__, state->mode);
        break;
    }
}

static void disable_input(struct v4l2_subdev *sd)
{
    /* disable */
    io_write(sd, 0x15, 0xbe);   /* Tristate all outputs from video core */
    hdmi_write(sd, 0x1a, 0x1a); /* Mute audio */
    hdmi_write(sd, 0x01, 0x78); /* Open termination */
}

static void setup_digital(struct v4l2_subdev *sd)
{
    /* based on the ":DVI script:" section in ADV7604IO_ADV7604CP_
     * ADV7604AFE_ADV7604HDMI_ADV7341B-VER.1.4c_RevA.txt and 
     * "ADV7604 Register Settings Recommendations(rev.2.5,June 2010)" */
    io_write(sd, 0x0c, 0x42);   /* power up part and power down VDP */
	io_write(sd, 0x00, 0x02);   /* VID_STD = 0x2 */
    io_write(sd, 0x01, 0x86);   /* Prim_Mode = 110b HDMI-GR */
    io_write(sd, 0x02, 0x12);   /* RGB in 0-255,  RGB out */
    io_write(sd, 0x03, 0x40);   /* 24 bit SDR 444 mode 0 */
    io_write(sd, 0x04, 0x62);   /* op_ch_sel = 0x03 */
    io_write(sd, 0x05, 0x28);   /* AV codes off */
    io_write(sd, 0x06, 0xa6);   /* VS and HS inverted */
    io_write(sd, 0x0b, 0x44);   /* Power down ESDP block */
    io_write(sd, 0x14, 0x7f);   /* Drive strength adjustment */
    io_write(sd, 0x15, 0xa0);   /* Disable tristate of pins */
    io_write(sd, 0x33, 0x60);   /* LLC DLL Mux Enable */

    /* component processor */
    cp_write(sd, 0x69, 0x30);   /* enable CP CSC for pass thru mode 
                                   HW Manual RevF Page 216 */
    cp_write(sd, 0x6c, 0x00);   /* ADV setting, use manual clamping 0 
                                   for all channels*/
	cp_write_and_or(sd, 0xc9, 0xfe, 0x00); /* DIS_AUTO_PARAM_BUFF = 0 */
    cp_write(sd, 0xcf, 0x01);   /* Power off macrovision */
	cp_write_and_or(sd, 0xbd, 0xff, 0x10); /* bypass DPP EN*/
    cp_write(sd, 0x3e, 0x00);   /* CP core pre-gain control */

    /* analog front end */
    afe_write(sd, 0x00, 0xff);   /* Power down ADCs & their assoc. clocks */
    afe_write(sd, 0x01, 0xfe);   /* Power down ref buffer/bandgap/clamp/sync */
    afe_write(sd, 0x13, 0x93);   /* Set LLC DLL phase */
    afe_write(sd, 0xb5, 0x01);   /* Set MCLK to 256Fs */
    afe_write(sd, 0xc8, 0x40);   /* ADI recommended setting */

    /* HDMI */
    hdmi_write(sd, 0x01, 0x00);  /* Enable clock terminators */
    hdmi_write(sd, 0x0d, 0x84);  /* ADI recommended write */
    hdmi_write(sd, 0x15, 0x03);  /* Set audio FIFO mute masks */
    hdmi_write(sd, 0x1a, 0x0a);  /* Set mute delay to 1_5sec */
    hdmi_write(sd, 0x3d, 0x10);  /* ADI recommended setting */
    hdmi_write(sd, 0x3e, 0x39);  /* ADI recommended setting */
    hdmi_write(sd, 0x48, 0x06);  /* Set audio FIFO reset */
    hdmi_write(sd, 0x57, 0xb6);  /* Enable dynamic PLL control */
    hdmi_write(sd, 0x58, 0x03);  /* Recommended PLL setting */
    hdmi_write(sd, 0x59, 0xa3);  /* ADI recommended setting */
    hdmi_write(sd, 0x8d, 0x18);  /* ADI recommended equalizer setting */
    hdmi_write(sd, 0x8e, 0x34);  /* ADI recommended equalizer setting */
    hdmi_write(sd, 0x93, 0x8b);  /* Equalizer ADI recommended setting */
    hdmi_write(sd, 0x94, 0x2d);  /* Equalizer ADI recommended setting */
    hdmi_write(sd, 0x96, 0x01);  /* Enable automatic EQ changing */
    hdmi_write(sd, 0x1a, 0x0a);  /* Unmute autio */
}

static int configure_custom_video_alignment(struct v4l2_subdev *sd, 
        struct adv7604_platform_data *pdata, 
        const struct v4l2_dv_timings *timings)
{
    s16 de_h_start, de_h_end, de_v_start, de_v_end;
    struct adv7604_state *state = to_state(sd);
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;

    if (mutex_lock_interruptible(&state->pdata_lock)) {
        return -EINTR;
    }
    de_h_start = pdata->analog_hstart;
    de_h_end = pdata->analog_hstart;
    de_v_start = pdata->analog_vstart;
    de_v_end = pdata->analog_vstart;

    if (verbose) {
        printk(KERN_INFO "%s: h_start %d, h_end %d, v_start %d, v_end %d\n",
                __func__, de_h_start, de_h_end, de_v_start, de_v_end);
    }

    /* horizontal alignment */
    cp_write_and_or(sd, 0x8b, 0xf3, ((de_h_start >> 8) & 0x03) << 2);
    cp_write(sd, 0x8d, de_h_start & 0xff);
    cp_write_and_or(sd, 0x8b, 0xfc, (de_h_end >> 8) & 0x03);
    cp_write(sd, 0x8c, de_h_end & 0xff);

    /* vertical alignment */
    cp_write_and_or(sd, 0x8e, 0x0f, (de_v_start & 0x0f) << 4);
    cp_write_and_or(sd, 0x8e, 0xf0, de_v_end & 0x0f);

    mutex_unlock(&state->pdata_lock);

    return 0;
}

static int configure_custom_video_timings(struct v4l2_subdev *sd,
		const struct v4l2_dv_timings *timings)
{
	struct adv7604_state *state = to_state(sd);
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    const struct v4l2_bt_timings *bt = &timings->bt;
	u32 width = htotal(bt);
	u32 height = vtotal(bt);
	u16 cp_start_sav = bt->hsync + bt->hbackporch - 4;
	u16 cp_start_eav = width - bt->hfrontporch;
	u16 cp_start_vbi = height - bt->vfrontporch;
	u16 cp_end_vbi = bt->vsync + bt->vbackporch;
	u16 ch1_fr_ll = (((u32)bt->pixelclock / 100) > 0) ?
		((width * (ADV7604_fsc / 100)) / ((u32)bt->pixelclock / 100)) : 0;
	const u8 pll[2] = {
		0xc0 | ((width >> 8) & 0x1f),
		width & 0xff
	};
	const u8 fr_ll[2] = {
		(ch1_fr_ll >> 8) & 0x07,
		ch1_fr_ll & 0xff
	};
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;
    int rc = 0;
    
    if (verbose) {
        printk(KERN_INFO "%s: mode %d, width %d, height %d, pixelclock %lld "
                "pll 0x%x/0x%x, ch1_fr_ll 0x%x fr_ll 0x%x/0x%x\n", 
                __func__, state->mode, width, height, bt->pixelclock, 
                pll[0], pll[1], ch1_fr_ll, fr_ll[0], fr_ll[1]);
    }

    if (width == 0 || height == 0) 
        return -1;

	switch (state->mode) {
	case NETVIZXPI_MODE_COMP:
	case NETVIZXPI_MODE_GR:
	case NETVIZXPI_MODE_STDI:
		/* enter auto graphics mode */
		io_write(sd, 0x00, 0x07); /* video std */
		io_write(sd, 0x01, 0x02); /* prim mode */

		/* enable embedded syncs for auto graphics mode */
		cp_write_and_or(sd, 0x81, 0xef, 0x10);

		/* should only be set in auto-graphics mode [REF_02, p. 91-92] */
		/* setup PLL_DIV_MAN_EN and PLL_DIV_RATIO */
		/* IO-map reg. 0x16 and 0x17 should be written in sequence */
		rc = adv_smbus_write_i2c_block_data(client, 0x16, 2, pll);
        if (rc) {
			v4l2_err(sd, "writing to IO reg 0x16 and 0x17 failed\n");
			break;
		}

        /* set free run mode number of lines per field */
	    rc = adv_smbus_write_i2c_block_data(state->i2c_cp, 0x8f, 2, fr_ll);
        if (rc) {
		    v4l2_err(sd, "writing to CP reg 0x8f and 0x90 failed\n");
        }

        /* set expected number of lines per frame */ 
	    cp_write(sd, 0xab, (height >> 4) & 0xff);
	    cp_write(sd, 0xac, (height & 0x0f) << 4);

        /* configure interlaced or progressive parameter */
        cp_write(sd, 0x91, 0x10);   

        /* adjusting horizontal and vertical alignment */
        rc = configure_custom_video_alignment(sd, state->pdata, timings);
        if (rc) {
			v4l2_err(sd, "configure custom video alignment failed.\n");
            break;
        }

		/* active video - horizontal timing */
		cp_write(sd, 0xa2, (cp_start_sav >> 4) & 0xff);
		cp_write(sd, 0xa3, ((cp_start_sav & 0x0f) << 4) |
					((cp_start_eav >> 8) & 0x0f));
		cp_write(sd, 0xa4, cp_start_eav & 0xff);

		/* active video - vertical timing */
		cp_write(sd, 0xa5, (cp_start_vbi >> 4) & 0xff);
		cp_write(sd, 0xa6, ((cp_start_vbi & 0xf) << 4) |
					((cp_end_vbi >> 8) & 0xf));
		cp_write(sd, 0xa7, cp_end_vbi & 0xff);
		break;
	case NETVIZXPI_MODE_HDMI:
		io_write(sd, 0x00, 0x02);   /* video std */
		io_write(sd, 0x01, 0x06);   /* prim mode */
	    cp_write(sd, 0x8f, (ch1_fr_ll >> 8) & 0x7);
	    cp_write(sd, 0x90, ch1_fr_ll & 0xff);
	    cp_write(sd, 0xab, (height >> 4) & 0xff);
	    cp_write(sd, 0xac, (height & 0x0f) << 4);
		break;
	default:
		v4l2_dbg(1, debug, sd, "%s: Unknown mode %d\n", __func__, state->mode);
		break;
	}
    return rc;
}

static int setup_analog(struct v4l2_subdev *sd, int vid_std)
{
    struct adv7604_state *state = to_state(sd);
    struct adv7604_platform_data *pdata = state->pdata;

    /* based on the ":Graphics" section in ADV7604IO_ADV7604CP_
     * ADV7604AFE_ADV7604HDMI_ADV7341B-VER.1.4c_RevA.txt and 
     * "ADV7604 Register Settings Recommendations(rev.2.5,June 2010)" */
    io_write(sd, 0x0c, 0x42);   /* power up part and power down VDP */
    io_write(sd, 0x00, vid_std);/* video standard */
    io_write(sd, 0x01, 0x82);   /* enable simultaneous mode, prim_mode=2 */
    io_write(sd, 0x02, 0xf2);   /* auto CSC, RGB out */
    io_write(sd, 0x03, 0x40);   /* 24 bit SDR 444 mode 0 */
    io_write(sd, 0x04, 0x42);   /* op_ch_sel = 0x02 */
    io_write(sd, 0x05, 0x2c);   /* avcode_insert_en = 1 */
    io_write(sd, 0x06, 0xa6);   /* VS and HS inverted */
    io_write(sd, 0x0b, 0x44);   /* power down ESDP block */
    io_write(sd, 0x14, 0x7f);   /* drive strength adjustment */
    io_write(sd, 0x15, 0x80);   /* disable tristate of pins */
    io_write(sd, 0x33, 0x60);   /* LLC DLL Mux Enable */

    /* autographics */
    if (vid_std == ADV7604_VID_STD_AUTO) {
        if (configure_custom_video_timings(sd, &state->timings)) {
			v4l2_err(sd, "configure custom video timings failed.\n");
            return -1;
        }
    }

    /* component processor */
    cp_write(sd, 0x3e, 0x04);   /* enable gain control for CP */
    cp_write(sd, 0x40, 0x5c);   /* adjust CP gain for PGA gain of 1.4 */
    cp_write(sd, 0x69, 0x30);   /* enable CP CSC */
    cp_write(sd, 0x85, 0x0b);   /* disable autodetect for sync src CH1 */
    cp_write(sd, 0xc3, 0x39);   /* ADI recommended write */
    cp_write(sd, 0xcf, 0x01);   /* power off macrovision */

    /* analog front end */
    afe_write(sd, 0x00, 0x08);  /* power up ADC */
    afe_write(sd, 0x01, 0x06);  /* power up Analog Front End */
    if (mutex_lock_interruptible(&state->pdata_lock))
        return -EINTR;
    afe_write(sd, 0xc8, pdata->analog_sampling_phase);  /* phase control */
    mutex_unlock(&state->pdata_lock);
    afe_write(sd, 0x13, 0x93);  /* Set LLC DLL phase */
    afe_write(sd, 0x02, 0x02);  /* ain_sel to 010 (AIN 7,8,9) */
    afe_write(sd, 0x05, 0x07);  /* aa filter enable */
    afe_write(sd, 0x06, 0x20);  /* aa_filt_high_bw->11 */
    afe_write(sd, 0x07, 0xa0);  /* aa_filt_high_bw->11, aa_filt_prog_bw->01 */
    afe_write(sd, 0x12, 0x63);  /* Turn noise shaping off */
    afe_write(sd, 0x0c, 0x1f);  /* Set PGA gain to 1.4 */
    afe_write(sd, 0x13, 0x80);  /* Set LLC DLL phase */
    afe_write(sd, 0xc6, 0x5f);  /* Disable audio core */

    return 0;
}

static int find_and_set_predefined_video_timings(struct v4l2_subdev *sd,
		u8 prim_mode,
		const struct adv7604_video_standards *predef_vid_timings,
		const struct v4l2_dv_timings *timings)
{
	int i;

	for (i = 0; predef_vid_timings[i].timings.bt.width; i++) {
		if (!v4l_match_dv_timings(timings, &predef_vid_timings[i].timings,
					digital_input(sd) ? 250000 : 1000000))
			continue;
		io_write(sd, 0x00, predef_vid_timings[i].vid_std); /* video std */
		io_write(sd, 0x01, (predef_vid_timings[i].v_freq << 4) +
				prim_mode); /* v_freq and prim mode */
		return 0;
	}

	return -1;
}

static void select_input(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);

    switch (state->mode) {
    case NETVIZXPI_MODE_COMP:
    case NETVIZXPI_MODE_GR:
	case NETVIZXPI_MODE_STDI:
        setup_analog(sd, ADV7604_VID_STD_AUTO);   /* vid_std=00000111 */
		break;
    case NETVIZXPI_MODE_HDMI:
        setup_digital(sd);
        break;
    default:
        v4l2_err(sd, "%s: unknown mode 0x%0x\n", __func__, state->mode);
        break;
    }
}

static int configure_predefined_video_timings(struct v4l2_subdev *sd,
		struct v4l2_dv_timings *timings)
{
	struct adv7604_state *state = to_state(sd);
	int err;

	v4l2_dbg(2, debug, sd, "%s", __func__);

	/* reset to default values */
	io_write(sd, 0x16, 0x43);
	io_write(sd, 0x17, 0x5a);
	/* disable embedded syncs for auto graphics mode */
	cp_write_and_or(sd, 0x81, 0xef, 0x00);
	cp_write(sd, 0x8f, 0x00);
	cp_write(sd, 0x90, 0x00);
	cp_write(sd, 0xa2, 0x00);
	cp_write(sd, 0xa3, 0x00);
	cp_write(sd, 0xa4, 0x00);
	cp_write(sd, 0xa5, 0x00);
	cp_write(sd, 0xa6, 0x00);
	cp_write(sd, 0xa7, 0x00);
	cp_write(sd, 0xab, 0x00);
	cp_write(sd, 0xac, 0x00);

	switch (state->mode) {
	case NETVIZXPI_MODE_COMP:
	case NETVIZXPI_MODE_GR:
	case NETVIZXPI_MODE_STDI:
		err = find_and_set_predefined_video_timings(sd,
				0x01, adv7604_prim_mode_comp, timings);
		if (err)
			err = find_and_set_predefined_video_timings(sd,
					0x02, adv7604_prim_mode_gr, timings);
		break;
	case NETVIZXPI_MODE_HDMI:
		err = find_and_set_predefined_video_timings(sd,
				0x05, adv7604_prim_mode_hdmi_comp, timings);
		if (err)
			err = find_and_set_predefined_video_timings(sd,
					0x06, adv7604_prim_mode_hdmi_gr, timings);
		break;
	default:
		v4l2_dbg(1, debug, sd, "%s: Unknown mode %d\n", __func__, state->mode);
		err = -1;
		break;
	}


	return err;
}

static void set_rgb_quantization_range(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);

    switch (state->rgb_quantization_range) {
    case V4L2_DV_RGB_RANGE_AUTO:
        /* automatic */
        if (digital_input(sd) && !(hdmi_read(sd, 0x05) & 0x80)) { 
            /* receiving DVI-D signal */

            /* ADV7604 selects RGB limited range regardless of
               input format (CE/IT) in automatic mode */
            if (state->timings.bt.reserved[0] & V4L2_DV_BT_STD_CEA861) {
                /* RGB limited range (16-235) */
                io_write_and_or(sd, 0x02, 0x0f, 0x00);

            } 
            else {
                /* RGB full range (0-255) */
                io_write_and_or(sd, 0x02, 0x0f, 0x10);
            }
        }
        else {
            /* receiving HDMI or analog signal, set automode */
            io_write_and_or(sd, 0x02, 0x0f, 0xf0);
        }
        break;
    case V4L2_DV_RGB_RANGE_LIMITED:
        /* RGB limited range (16-235) */
        io_write_and_or(sd, 0x02, 0x0f, 0x00);
        break;
    case V4L2_DV_RGB_RANGE_FULL:
        /* RGB full range (0-255) */
        io_write_and_or(sd, 0x02, 0x0f, 0x10);
        break;
    }
}

void adv7604_print_timings(struct v4l2_subdev *sd,
                           struct v4l2_dv_timings *timings,
                           const char *txt, bool detailed)
{
    struct v4l2_bt_timings *bt = &(timings->bt);
    u32 htot, vtot;

    if (timings->type != V4L2_DV_BT_656_1120) 
        return;

    htot = htotal(bt);
    vtot = vtotal(bt);

    v4l2_info(sd, "%s %dx%d%s%d (%dx%d)",
                  txt, bt->width, bt->height, bt->interlaced ? "i" : "p",
                  (htot * vtot) > 0 ? ((u32)bt->pixelclock /
                  (htot * vtot)) : 0,
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
EXPORT_SYMBOL(adv7604_print_timings);

#ifdef SUPPORT_INTERRUPT
static int adv7604_isr(struct v4l2_subdev *sd, u32 status, bool *handled) 
{
    if (adv7604_status_change(sd) && handled)
        *handled = true;
    return 0;
}
#endif

/* ----------------------------------------------------------------------- */

bool adv7604_status_change(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;
    u8 status;
    static int first_time = 1, timer_count_since_last_change = 0;
    static u8 last_status = 0;

    /* format change */
    status = io_read(sd, 0x6a) & 0xf0;
    if (first_time) {
        first_time = 0;
        last_status = status;
    }
    if (status == last_status) {
        /* status uchanged for at least 3 consecutive times */
        if (timer_count_since_last_change == 0) {
            return false;
        }
        /* status changed for less than 3 consecutive times */
        else if (timer_count_since_last_change++ < 2) { 
            last_status = status;
            return false;
        }
    }
    else {
        /* status changed since last time but back to original */
        if (status == (state->curr_add_status & 0xf0)) {
            timer_count_since_last_change = 0;
        }
        /* status changed since last time */
        else {
            last_status = status;
            timer_count_since_last_change = 1;
        }
        return false;
    }
    state->curr_add_status = status | (state->curr_add_status & 0x0f); 
    timer_count_since_last_change = 0;

    if (verbose)
        printk("%s: video input status 0x%x\n", __func__, status);

    return true;
}
EXPORT_SYMBOL(adv7604_status_change);

/* 
 * returns input mode if there's input signal and it locked, or negative 
 * error code if no signal or signal not locked 
 */ 
int adv7604_query_dv_timings(struct v4l2_subdev *sd,
                             struct v4l2_dv_timings *timings)
{
    struct adv7604_state *state = to_state(sd);
    struct v4l2_bt_timings *bt = &(timings->bt);
    struct stdi_readback stdi;
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;
    int rc = 0;

    if (!timings) 
        return -EINVAL;

    memset(timings, 0, sizeof(struct v4l2_dv_timings));

    if (no_power(sd) || no_signal_tmds(sd)) 
        return -ENOLINK;

    memset(&stdi, 0, sizeof(struct stdi_readback));
    
    if (verbose) 
        printk(KERN_INFO \
                "%s: no_lock_tmds %s, no_lock_sspd %s, no_lock_stdi %s\n", 
                __func__, no_lock_tmds(sd) ? "TRUE" : "FALSE", 
                no_lock_sspd(sd) ? "TRUE" : "FALSE", 
                no_lock_stdi(sd) ? "TRUE" : "FALSE"); 

    if (digital_input(sd)) {

        if (no_lock_tmds(sd) || no_lock_sspd(sd)) {
            goto out;
        }

        /* read TMDS timing */
        timings->type = V4L2_DV_BT_656_1120;
        bt->width = (hdmi_read(sd, 0x07) & 0x0f) * 256 + hdmi_read(sd, 0x08);
        bt->height = (hdmi_read(sd, 0x09) & 0x0f) * 256 + hdmi_read(sd, 0x0a);
        bt->pixelclock = (hdmi_read(sd, 0x06) * 1000000) +
                ((hdmi_read(sd, 0x3b) & 0x30) >> 4) * 250000;
        bt->hfrontporch = (hdmi_read(sd, 0x20) & 0x03) * 256 + 
                hdmi_read(sd, 0x21);
        bt->hsync = (hdmi_read(sd, 0x22) & 0x03) * 256 + 
                hdmi_read(sd, 0x23);
        bt->hbackporch = (hdmi_read(sd, 0x24) & 0x03) * 256 + 
                hdmi_read(sd, 0x25);
        bt->vfrontporch = ((hdmi_read(sd, 0x2a) & 0x1f) * 256 +
                hdmi_read(sd, 0x2b)) / 2;
        bt->vsync = ((hdmi_read(sd, 0x2e) & 0x1f) * 256 +
                hdmi_read(sd, 0x2f)) / 2;
        bt->vbackporch = ((hdmi_read(sd, 0x32) & 0x1f) * 256 +
                hdmi_read(sd, 0x33)) / 2;
        bt->polarities = ((hdmi_read(sd, 0x05) & 0x10) ? 
                V4L2_DV_VSYNC_POS_POL : 0) | 
                ((hdmi_read(sd, 0x05) & 0x20) ? V4L2_DV_HSYNC_POS_POL : 0);
        if (bt->interlaced == V4L2_DV_INTERLACED) {
            bt->height += (hdmi_read(sd, 0x0b) & 0x0f) * 256 +
                                hdmi_read(sd, 0x0c);
            bt->il_vfrontporch = ((hdmi_read(sd, 0x2c) & 0x1f) * 256 +
                                hdmi_read(sd, 0x2d)) / 2;
            bt->il_vsync = ((hdmi_read(sd, 0x30) & 0x1f) * 256 +
                                hdmi_read(sd, 0x31)) / 2;
            bt->vbackporch = ((hdmi_read(sd, 0x34) & 0x1f) * 256 +
                                hdmi_read(sd, 0x35)) / 2;
        }
        state->curr_vid_std = ADV7604_VID_STD_HDMI;
        rc = NETVIZXPI_MODE_HDMI;
    }
    else {
        if (no_lock_sspd(sd) || no_lock_stdi(sd)) {
            goto out;
        }
        /* read STDI */
        if (read_stdi(sd, &stdi)) { 
            if (verbose)
                printk("%s: STDI/SSPD not locked\n", __func__);
            return -ENOLINK;
        }
        bt->interlaced = stdi.interlaced ? V4L2_DV_INTERLACED : 
            V4L2_DV_PROGRESSIVE;

        if (verbose) 
            printk(KERN_INFO \
                    "%s: read_stdi returns bl=%d, lcf=%d, lcvs=%d, fcl = %d, "
                    "hs_pol = %c, vs_pol=%c\n", __func__, stdi.bl, stdi.lcf, 
                    stdi.lcvs, stdi.fcl, stdi.hs_pol, stdi.vs_pol); 
    
        state->curr_vid_std = ADV7604_VID_STD_AUTO;

        memcpy(&state->stdi, &stdi, sizeof(struct stdi_readback));
        rc = NETVIZXPI_MODE_STDI;
    }

out:
    if (no_signal(sd)) {
        memset(timings, 0, sizeof(struct v4l2_dv_timings));
        return -ENOLINK;
    }
    if ((digital_input(sd) && bt->pixelclock > 170000000) ||
                (digital_input(sd) && bt->pixelclock > 225000000)) {
        if (verbose)
            printk("%s: pixelclock = 0x%llu, signel out of range\n", 
                    __func__, bt->pixelclock);
        return -ERANGE;
    }

    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    memcpy(&state->timings, timings, sizeof(struct v4l2_dv_timings));
    mutex_unlock(&state->lock);

    if (verbose) 
        adv7604_print_timings(sd, timings, __func__, true);

    return rc;
}
EXPORT_SYMBOL(adv7604_query_dv_timings);

void adv7604_setup_videoinput(struct v4l2_subdev *sd,
                              struct v4l2_dv_timings *timings)
{
    struct adv7604_state *state = to_state(sd);

    v4l2_dbg(2, debug, sd, "%s: entered. mode %d. vid_std %d\n", 
            __func__, state->mode, state->curr_vid_std);

    if (timings != NULL)
        memcpy(&state->timings, timings, sizeof(struct v4l2_dv_timings));

    switch (state->mode) {
	case NETVIZXPI_MODE_COMP:
	case NETVIZXPI_MODE_GR:
	case NETVIZXPI_MODE_STDI:
        setup_analog(sd, state->curr_vid_std);
        break;
    case NETVIZXPI_MODE_HDMI:
        setup_digital(sd);
        break;
    }
}
EXPORT_SYMBOL(adv7604_setup_videoinput);

int adv7604_g_stdi_readback(struct v4l2_subdev *sd,
                            struct stdi_readback *stdi)
{
    struct adv7604_state *state = to_state(sd);

    memcpy(stdi, &state->stdi, sizeof(struct stdi_readback));

    return 0;
}
EXPORT_SYMBOL(adv7604_g_stdi_readback);

int adv7604_set_analog_parameters(struct v4l2_subdev *sd,
                                  struct adv7604_analog_parameters *param)
{
    struct adv7604_state *state = to_state(sd);

    if (mutex_lock_interruptible(&state->pdata_lock))
        return -EINTR;
    if (param->analog_hstart_set) 
        state->pdata->analog_hstart = param->analog_hstart;
    if (param->analog_vstart_set) 
        state->pdata->analog_vstart = param->analog_vstart;
    if (param->analog_sampling_phase_set)
        state->pdata->analog_sampling_phase = param->analog_sampling_phase;
    mutex_unlock(&state->pdata_lock);

    if (state->mode == NETVIZXPI_MODE_COMP || 
            state->mode == NETVIZXPI_MODE_GR ||
            state->mode == NETVIZXPI_MODE_STDI) {
        setup_analog(sd, state->curr_vid_std);
    }
    return 0;
}
EXPORT_SYMBOL(adv7604_set_analog_parameters);

int adv7604_set_analog_timings(struct v4l2_subdev *sd,
                               struct v4l2_dv_timings *timings)
{
    struct adv7604_state *state = to_state(sd);

    if (state->mode == NETVIZXPI_MODE_COMP || 
            state->mode == NETVIZXPI_MODE_GR ||
            state->mode == NETVIZXPI_MODE_STDI) {

        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        memcpy(&state->timings, timings, sizeof(struct v4l2_dv_timings));
        mutex_unlock(&state->lock);
    }
    return 0;
} 
EXPORT_SYMBOL(adv7604_set_analog_timings);

int adv7604_set_free_run(struct v4l2_subdev *sd, int lutid)
{
    if (lutid > 0) {
        /* See Table 3 ADV7604 HW manual Rev F August 2010 */ 
        if (lutid == RESERVED_FRUN_800_LUTID) {
            io_write_and_or(sd, 0x00, 0xc0, 0x01); 
            io_write_and_or(sd, 0x01, 0x80, 0x06);  
        }
        else if (lutid == RESERVED_FRUN_1024_LUTID) {
            io_write_and_or(sd, 0x00, 0xc0, 0x0c); 
            io_write_and_or(sd, 0x01, 0x80, 0x06);  
        }
        else {
            io_write_and_or(sd, 0x00, 0xc0, 0x05); 
            io_write_and_or(sd, 0x01, 0x80, 0x06);  
        }
        cp_write_and_or(sd, 0xba, 0xfc, 0x03);  /* enable free run, mode 1 */
        cp_write_and_or(sd, 0xc9, 0xfe, 0x01);  /* disable buffering of */
                                                /* measured params in HDMI */
                                                /* mode */
        cp_write_and_or(sd, 0xbf, 0xfc, 0x03);  /* force CP to free run and */
                                                /* output default colors */
    }
    else {
        cp_write_and_or(sd, 0xba, 0xfc, 0x00);  /* set HDMI_FRUN_EN HDMI_FRUN_*/
                                                /* and DIS_AUTO_PARAM_BUFF */
        cp_write_and_or(sd, 0xc9, 0xfe, 0x00);  /* to default */
        cp_write_and_or(sd, 0xbf, 0xfc, 0x00);  /* set CP to not free run */
    }
    return 0;
}
EXPORT_SYMBOL(adv7604_set_free_run);

/* ----------------------------------------------------------------------- */

static int adv7604_s_ctrl(struct v4l2_ctrl *ctrl)
{
    struct v4l2_subdev *sd = to_sd(ctrl);
    struct adv7604_state *state = to_state(sd);
    int rc = 0;

    switch (ctrl->id) {
    case V4L2_CID_BRIGHTNESS:
        cp_write_and_or(sd, 0x3e, 0x7f, 0x80);
        cp_write(sd, 0x3c, ctrl->val);
        cp_write_and_or(sd, 0x3e, 0x7f, 0x00);
        break;
    case V4L2_CID_CONTRAST:
        cp_write_and_or(sd, 0x3e, 0x7f, 0x80);
        cp_write(sd, 0x3a, ctrl->val);
        cp_write_and_or(sd, 0x3e, 0x7f, 0x00);
        break;
    case V4L2_CID_SATURATION:
        cp_write_and_or(sd, 0x3e, 0x7f, 0x80);
        cp_write(sd, 0x3b, ctrl->val);
        cp_write_and_or(sd, 0x3e, 0x7f, 0x00);
        break;
    case V4L2_CID_HUE:
        cp_write_and_or(sd, 0x3e, 0x7f, 0x80);
        cp_write(sd, 0x3d, ctrl->val);
        cp_write_and_or(sd, 0x3e, 0x7f, 0x00);
        break;
    case V4L2_CID_AUDIO_MUTE:
        hdmi_write(sd, 0x1a, (ctrl->val) ? 0x98 : 0x88);
        break;
    case V4L2_CID_ADV_RX_RGB_RANGE:
        state->rgb_quantization_range = ctrl->val;
        set_rgb_quantization_range(sd);
        break;
    case V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE:
        /* Set the analog sampling phase. This is needed to find the
           best sampling phase for analog video: an application or
           driver has to try a number of phases and analyze the picture
           quality before settling on the best performing phase. */
        if (mutex_lock_interruptible(&state->pdata_lock))
            rc = -EINTR;
        else {
            state->pdata->analog_sampling_phase = ctrl->val;
            afe_write(sd, 0xc8, ctrl->val);
            mutex_unlock(&state->pdata_lock);
        }
        break;
    case V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL:
        /* Use the default blue color for free running mode,
           or supply your own. */
        cp_write_and_or(sd, 0xbf, ~0x04, (ctrl->val << 2));
        break;
    case V4L2_CID_ADV_RX_FREE_RUN_COLOR:
        cp_write(sd, 0xc0, (ctrl->val & 0xff0000) >> 16);
        cp_write(sd, 0xc1, (ctrl->val & 0x00ff00) >> 8);
        cp_write(sd, 0xc2, (u8)(ctrl->val & 0x0000ff));
        break;
    default:
        rc = -EINVAL;
    }

    return rc;
}

static int adv7604_g_chip_ident(struct v4l2_subdev *sd,
                                struct v4l2_dbg_chip_ident *chip)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_ADV7604, 0);
}

static int adv7604_g_edid(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);
    int err;

    err = edid_read_block(sd, ARRAY_SIZE(state->edid), state->edid);
    if (err < 0)
        v4l2_err(sd, "%s: error reading edid\n", __func__);

    return err;
}

static int adv7604_s_edid(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);
    int err;

    err = edid_write_block(sd, ARRAY_SIZE(state->edid), state->edid);
    if (err < 0)
        v4l2_err(sd, "%s: error %d writing edid.\n",  __func__, err);

    return err;
}

static int adv7604_s_routing(struct v4l2_subdev *sd,
                             u32 input,
                             u32 output,
                             u32 config)
{
    struct adv7604_state *state = to_state(sd);
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;

    if (verbose)
        printk("%s: input %d, output %d, config %d\n", __func__, 
                input, output, config);

    state->mode = input;
    disable_input(sd);
    select_input(sd);
    enable_input(sd);

    return 0;
}

static int adv7604_g_input_status(struct v4l2_subdev *sd, u32 *status)
{
    struct adv7604_state *state = to_state(sd);
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;

    *status = 0;
    *status |= no_power(sd) ? V4L2_IN_ST_NO_POWER : 0;
    *status |= no_signal(sd) ? V4L2_IN_ST_NO_SIGNAL : 0;
    if (no_lock_cp(sd))
        *status |= digital_input(sd) ? V4L2_IN_ST_NO_SYNC : 
            V4L2_IN_ST_NO_H_LOCK;

    if (verbose)
        printk("%s: status = 0x%x\n", __func__, *status);

    return 0;
}

static int adv7604_enum_mbus_fmt(struct v4l2_subdev *sd,
                                 unsigned int index,
                                 enum v4l2_mbus_pixelcode *code)
{
    if (index)
        return -EINVAL;
    /* Good enough for now */
    *code = V4L2_MBUS_FMT_FIXED;
    return 0;
}

static int adv7604_g_mbus_fmt(struct v4l2_subdev *sd,
                              struct v4l2_mbus_framefmt *fmt)
{
    struct adv7604_state *state = to_state(sd);

    fmt->width = state->timings.bt.width;
    fmt->height = state->timings.bt.height;
    fmt->code = V4L2_MBUS_FMT_FIXED;
    fmt->field = V4L2_FIELD_NONE;
    if (state->timings.bt.reserved[0] & V4L2_DV_BT_STD_CEA861) {
        fmt->colorspace = (state->timings.bt.height <= 576) ?
                V4L2_COLORSPACE_SMPTE170M : V4L2_COLORSPACE_REC709;
    }
    return 0;
}

static int adv7604_s_dv_timings(struct v4l2_subdev *sd,
                                struct v4l2_dv_timings *timings)
{
    struct adv7604_state *state = to_state(sd);
    struct v4l2_bt_timings *bt;
    int verbose = state->pdata->flags & ADV7604_FLAG_VERBOSE;
    int err = 0;

    if (!timings)
        return -EINVAL;

    bt = &(timings->bt);

    if ((!digital_input(sd) && bt->pixelclock > 170000000) ||
                (digital_input(sd) && bt->pixelclock > 225000000)) {
        if (verbose) 
            printk("%s: pixelclock out of range %d\n", 
                    __func__, (u32)bt->pixelclock);
        err = -ERANGE;
        goto out;
    }
    memcpy(&state->timings, timings, sizeof(struct v4l2_dv_timings));

    cp_write(sd, 0x91, bt->interlaced ? 0x50 : 0x10);

    /* Use prim_mode and vid_std when available */
    err = configure_predefined_video_timings(sd, timings);
    if (err) { 
        /* custom settings when the video format 
         * does not have prim_mode/vid_std */
        err = configure_custom_video_timings(sd, timings);
    }

    if (!err) 
        set_rgb_quantization_range(sd);

    if (verbose)
        adv7604_print_timings(sd, timings, "adv7604_s_dv_timings:", true);

out:
    return err;
}

static int adv7604_g_dv_timings(struct v4l2_subdev *sd,
                                struct v4l2_dv_timings *timings)
{
    struct adv7604_state *state = to_state(sd);

    memcpy(timings, &state->timings, sizeof(struct v4l2_dv_timings));

    return 0;
}

static int adv7604_log_status(struct v4l2_subdev *sd)
{
    struct adv7604_state *state = to_state(sd);
    struct v4l2_dv_timings timings;
    struct stdi_readback stdi;
    u8 reg_io_0x02 = io_read(sd, 0x02);

    char *csc_coeff_sel_rb[16] = {
        "bypassed", "YPbPr601 -> RGB", "reserved", "YPbPr709 -> RGB",
        "reserved", "RGB -> YPbPr601", "reserved", "RGB -> YPbPr709",
        "reserved", "YPbPr709 -> YPbPr601", "YPbPr601 -> YPbPr709",
        "reserved", "reserved", "reserved", "reserved", "manual"
    };
    char *input_color_space_txt[16] = {
        "RGB limited range (16-235)", "RGB full range (0-255)",
        "YCbCr Bt.601 (16-235)", "YCbCr Bt.709 (16-235)",
        "XvYCC Bt.601", "XvYCC Bt.709",
        "YCbCr Bt.601 (0-255)", "YCbCr Bt.709 (0-255)",
        "invalid", "invalid", "invalid", "invalid", "invalid",
        "invalid", "invalid", "automatic"
    };
    char *rgb_quantization_range_txt[] = {
        "Automatic",
        "RGB limited range (16-235)",
        "RGB full range (0-255)",
    };

    v4l2_info(sd, "-----Chip status-----\n");
    v4l2_info(sd, "Chip power: %s\n", no_power(sd) ? "off" : "on");
    v4l2_info(sd, "Connector type: %s\n", state->connector_hdmi ? 
            "HDMI" : (digital_input(sd) ? "DVI-D" : "DVI-A"));
    v4l2_info(sd, "EDID: %s\n", ((rep_read(sd, 0x7d) & 0x01) &&
                (rep_read(sd, 0x77) & 0x01)) ? "enabled" : "disabled ");
    v4l2_info(sd, "CEC: %s\n", !!(cec_read(sd, 0x2a) & 0x01) ?
                "enabled" : "disabled");

    v4l2_info(sd, "-----Signal status-----\n");
    v4l2_info(sd, "Cable detected (+5V power): %s\n",
                (io_read(sd, 0x6f) & 0x10) ? "true" : "false");
    v4l2_info(sd, "TMDS signal detected: %s\n",
                no_signal_tmds(sd) ? "false" : "true");
    v4l2_info(sd, "TMDS signal locked: %s\n",
                no_lock_tmds(sd) ? "false" : "true");
    v4l2_info(sd, "SSPD locked: %s\n", no_lock_sspd(sd) ? "false" : "true");
    v4l2_info(sd, "STDI locked: %s\n", no_lock_stdi(sd) ? "false" : "true");
    v4l2_info(sd, "CP locked: %s\n", no_lock_cp(sd) ? "false" : "true");
    v4l2_info(sd, "CP free run: %s\n",
                (!!(cp_read(sd, 0xff) & 0x10) ? "on" : "off"));
    v4l2_info(sd, "Prim-mode = 0x%x, video std = 0x%x\n",
                io_read(sd, 0x01) & 0x0f, io_read(sd, 0x00) & 0x3f);

    v4l2_info(sd, "-----Video Timings-----\n");
    if (read_stdi(sd, &stdi))
        v4l2_info(sd, "STDI: not locked\n");
    else
        v4l2_info(sd, "STDI: lcf (frame height - 1) = %d, bl = %d, fcl = %d, "
                "lcvs (vsync) = %d, %s, %chsync, %cvsync\n", 
                stdi.lcf, stdi.bl, stdi.fcl, stdi.lcvs, 
                stdi.interlaced ? "interlaced" : "progressive", 
                stdi.hs_pol, stdi.vs_pol);
    if (adv7604_g_dv_timings(sd, &timings))
        v4l2_info(sd, "No video detected\n");
    else
        adv7604_print_timings(sd, &timings, "Detected format:", true);

    adv7604_print_timings(sd, &state->timings, "Configured format:", true);

    v4l2_info(sd, "-----Color space-----\n");
    v4l2_info(sd, "RGB quantization range ctrl: %s\n",
                rgb_quantization_range_txt[state->rgb_quantization_range]);
    v4l2_info(sd, "Input color space: %s\n",
                input_color_space_txt[reg_io_0x02 >> 4]);
    v4l2_info(sd, "Output color space: %s %s, saturator %s\n",
                (reg_io_0x02 & 0x02) ? "RGB" : "YCbCr",
                (reg_io_0x02 & 0x04) ? "(16-235)" : "(0-255)",
                ((reg_io_0x02 & 0x04) ^ (reg_io_0x02 & 0x01)) ?
                                "enabled" : "disabled");
    v4l2_info(sd, "Color space conversion: %s\n",
                csc_coeff_sel_rb[cp_read(sd, 0xfc) >> 4]);
    return 0;
}

static void adv7604_inv_register(struct v4l2_subdev *sd)
{
    v4l2_info(sd, "0x000-0x0ff: IO Map\n");
    v4l2_info(sd, "0x100-0x1ff: AVLink Map\n");
    v4l2_info(sd, "0x200-0x2ff: CEC Map\n");
    v4l2_info(sd, "0x300-0x3ff: InfoFrame Map\n");
    v4l2_info(sd, "0x400-0x4ff: ESDP Map\n");
    v4l2_info(sd, "0x500-0x5ff: DPP Map\n");
    v4l2_info(sd, "0x600-0x6ff: AFE Map\n");
    v4l2_info(sd, "0x700-0x7ff: Repeater Map\n");
    v4l2_info(sd, "0x800-0x8ff: EDID Map\n");
    v4l2_info(sd, "0x900-0x9ff: HDMI Map\n");
    v4l2_info(sd, "0xa00-0xaff: Test Map\n");
    v4l2_info(sd, "0xb00-0xbff: CP Map\n");
    v4l2_info(sd, "0xc00-0xcff: VDP Map\n");
}

static int adv7604_g_register(struct v4l2_subdev *sd,
                              struct v4l2_dbg_register *reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    if (!v4l2_chip_match_i2c_client(client, &reg->match))
        return -EINVAL;
    if (!capable(CAP_SYS_ADMIN))
        return -EPERM;

    reg->size = 1;
    switch (reg->reg >> 8) {
    case 0:
    	reg->val = io_read(sd, reg->reg & 0xff);
    	break;
    case 1:
    	reg->val = avlink_read(sd, reg->reg & 0xff);
    	break;
    case 2:
    	reg->val = cec_read(sd, reg->reg & 0xff);
    	break;
    case 3:
    	reg->val = infoframe_read(sd, reg->reg & 0xff);
    	break;
    case 4:
    	reg->val = esdp_read(sd, reg->reg & 0xff);
    	break;
    case 5:
    	reg->val = dpp_read(sd, reg->reg & 0xff);
    	break;
    case 6:
    	reg->val = afe_read(sd, reg->reg & 0xff);
    	break;
    case 7:
    	reg->val = rep_read(sd, reg->reg & 0xff);
    	break;
    case 8:
    	reg->val = edid_read(sd, reg->reg & 0xff);
    	break;
    case 9:
    	reg->val = hdmi_read(sd, reg->reg & 0xff);
    	break;
    case 0xa:
    	reg->val = test_read(sd, reg->reg & 0xff);
    	break;
    case 0xb:
    	reg->val = cp_read(sd, reg->reg & 0xff);
    	break;
    case 0xc:
    	reg->val = vdp_read(sd, reg->reg & 0xff);
    	break;
    default:
    	v4l2_info(sd, "Register %03llx not supported\n", reg->reg);
    	adv7604_inv_register(sd);
    	break;
    }
    return 0;
}

static int adv7604_s_register(struct v4l2_subdev *sd,
                              struct v4l2_dbg_register *reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    if (!v4l2_chip_match_i2c_client(client, &reg->match))
    	return -EINVAL;
    if (!capable(CAP_SYS_ADMIN))
    	return -EPERM;

    reg->size = 1;

    switch (reg->reg >> 8) {
    case 0:
    	io_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 1:
    	avlink_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 2:
    	cec_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 3:
    	infoframe_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 4:
    	esdp_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 5:
    	dpp_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 6:
    	afe_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 7:
    	rep_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 8:
    	edid_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 9:
    	hdmi_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 0xa:
    	test_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 0xb:
    	cp_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    case 0xc:
    	vdp_write(sd, reg->reg & 0xff, reg->val & 0xff);
    	break;
    default:
    	v4l2_info(sd, "Register %03llx not supported\n", reg->reg);
    	adv7604_inv_register(sd);
    	break;
    }
    return 0;
}

static long adv7604_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
    int err;
    struct adv7604_state *state = to_state(sd);

    switch(cmd) {
    case V4L2_G_EDID: {
        err = adv7604_g_edid(sd);
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
        err = adv7604_s_edid(sd);
        if (err < 0)
            v4l2_err(sd, "error %d writing edid\n", err);
        break;
    }
    case V4L2_G_REGISTER:
        err = adv7604_g_register(sd, (struct v4l2_dbg_register *) arg);
        break;
    case V4L2_S_REGISTER:
        err = adv7604_s_register(sd, (struct v4l2_dbg_register *) arg);
        break;
    default:
        v4l2_err(sd, "%s: unknown ioctl 0x%08x\n", __func__, cmd);
        err = -EINVAL;
    }
    return err;
}

/* ----------------------------------------------------------------------- */

static const struct v4l2_ctrl_ops adv7604_ctrl_ops = {
    .s_ctrl = adv7604_s_ctrl,
};

static const struct v4l2_subdev_core_ops adv7604_core_ops = {
    .log_status = adv7604_log_status,
    .g_ext_ctrls = v4l2_subdev_g_ext_ctrls,
    .try_ext_ctrls = v4l2_subdev_try_ext_ctrls,
    .s_ext_ctrls = v4l2_subdev_s_ext_ctrls,
    .g_ctrl = v4l2_subdev_g_ctrl,
    .s_ctrl = v4l2_subdev_s_ctrl,
    .queryctrl = v4l2_subdev_queryctrl,
    .querymenu = v4l2_subdev_querymenu,
    .g_chip_ident = adv7604_g_chip_ident,
    .ioctl = adv7604_ioctl,
#ifdef CONFIG_VIDEO_ADV_DEBUG
    .g_register = adv7604_g_register,
    .s_register = adv7604_s_register,
#endif
};

static const struct v4l2_subdev_video_ops adv7604_video_ops = {
    .s_routing = adv7604_s_routing,
    .g_input_status = adv7604_g_input_status,
    .s_dv_timings = adv7604_s_dv_timings,
    .g_dv_timings = adv7604_g_dv_timings,
    .enum_mbus_fmt = adv7604_enum_mbus_fmt,
    .g_mbus_fmt = adv7604_g_mbus_fmt,
    .try_mbus_fmt = adv7604_g_mbus_fmt,
    .s_mbus_fmt = adv7604_g_mbus_fmt,
};

static const struct v4l2_subdev_ops adv7604_ops = {
    .core = &adv7604_core_ops,
    .video = &adv7604_video_ops,
};

/* -------------------------- custom ctrls ---------------------------------- */

static const struct v4l2_ctrl_config adv7604_ctrl_detect_tx_5v = {
    .ops = &adv7604_ctrl_ops,
    .id = V4L2_CID_ADV_RX_POWER_PRESENT,
    .name = "Detect 5V Power",
    .type = V4L2_CTRL_TYPE_BOOLEAN,
    .flags = V4L2_CTRL_FLAG_READ_ONLY,
    .min = false,
    .max = true,
    .step = 1,
    .def = false,
};

static const struct v4l2_ctrl_config adv7604_ctrl_rgb_quantization_range = {
    .ops = &adv7604_ctrl_ops,
    .id = V4L2_CID_ADV_RX_RGB_RANGE,
    .name = "RGB Range",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = V4L2_DV_RGB_RANGE_FULL,
    .step = 1,
    .def = V4L2_DV_RGB_RANGE_AUTO,
};


static const struct v4l2_ctrl_config adv7604_ctrl_analog_sampling_phase = {
    .ops = &adv7604_ctrl_ops,
    .id = V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE,
    .name = "Analog Sampling Phase",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 0x1f,
    .step = 1,
    .def = 0,
};

static const struct v4l2_ctrl_config adv7604_ctrl_free_run_color_manual = {
    .ops = &adv7604_ctrl_ops,
    .id = V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL,
    .name = "Free Running Color, Manual",
    .type = V4L2_CTRL_TYPE_BOOLEAN,
    .min = false,
    .max = true,
    .step = 1,
    .def = false,
};

static const struct v4l2_ctrl_config adv7604_ctrl_free_run_color = {
    .ops = &adv7604_ctrl_ops,
    .id = V4L2_CID_ADV_RX_FREE_RUN_COLOR,
    .name = "Free Running Color",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0x0,
    .max = 0xffffff,
    .step = 0x1,
    .def = 0x0,
};

/* ----------------------------------------------------------------------- */

static int adv7604_core_init(struct v4l2_subdev *sd)
{
    /* disable input */
    disable_input(sd);

    /* power up part and power down VDP */
    io_write(sd, 0x0c, 0x42);   

    /* initialize video format etc */
    setup_digital(sd);

    /* enable free-run default color insertion */
    set_default_color(sd, 1);

    return v4l2_ctrl_handler_setup(sd->ctrl_handler);
}

static void adv7604_unregister_clients(struct adv7604_state *state)
{
    if (state->i2c_avlink)
    	i2c_unregister_device(state->i2c_avlink);
    if (state->i2c_cec)
    	i2c_unregister_device(state->i2c_cec);
    if (state->i2c_infoframe)
    	i2c_unregister_device(state->i2c_infoframe);
    if (state->i2c_esdp)
    	i2c_unregister_device(state->i2c_esdp);
    if (state->i2c_dpp)
    	i2c_unregister_device(state->i2c_dpp);
    if (state->i2c_afe)
    	i2c_unregister_device(state->i2c_afe);
    if (state->i2c_repeater)
    	i2c_unregister_device(state->i2c_repeater);
    if (state->i2c_edid)
    	i2c_unregister_device(state->i2c_edid);
    if (state->i2c_hdmi)
    	i2c_unregister_device(state->i2c_hdmi);
    if (state->i2c_test)
    	i2c_unregister_device(state->i2c_test);
    if (state->i2c_cp)
    	i2c_unregister_device(state->i2c_cp);
    if (state->i2c_vdp)
    	i2c_unregister_device(state->i2c_vdp);
}

static struct i2c_client *adv7604_dummy_client(struct v4l2_subdev *sd,
                                               u8 addr, 
                                               u8 io_reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);

    if (addr)
    	io_write(sd, io_reg, addr << 1);
    return i2c_new_dummy(client->adapter, io_read(sd, io_reg) >> 1);
}

static int adv7604_init_controls(struct adv7604_state *state)
{
    struct v4l2_ctrl_handler *hdl = &state->hdl;
    int err = 0;

    v4l2_dbg(2, debug, &state->sd, "%s: entered\n", __func__);

    v4l2_ctrl_handler_init(hdl, 10);

    v4l2_ctrl_new_std(hdl, &adv7604_ctrl_ops,
    		V4L2_CID_BRIGHTNESS, 0, 255, 1, 128);
    v4l2_ctrl_new_std(hdl, &adv7604_ctrl_ops,
    		V4L2_CID_CONTRAST, 0, 255, 1, 128);
    v4l2_ctrl_new_std(hdl, &adv7604_ctrl_ops,
    		V4L2_CID_SATURATION, 0, 255, 1, 128);
    v4l2_ctrl_new_std(hdl, &adv7604_ctrl_ops,
    		V4L2_CID_HUE, -128, 127, 1, 0);
    v4l2_ctrl_new_std(hdl, &adv7604_ctrl_ops,
    		V4L2_CID_AUDIO_MUTE, 0, 1, 1, 0);

    /* custom controls */
    state->detect_tx_5v_ctrl = 
        v4l2_ctrl_new_custom(hdl, &adv7604_ctrl_detect_tx_5v, NULL);
    if (state->detect_tx_5v_ctrl)
        state->detect_tx_5v_ctrl->is_private = true;
    state->rgb_quantization_range_ctrl =
    	v4l2_ctrl_new_custom(hdl, &adv7604_ctrl_rgb_quantization_range, NULL);
    if (state->rgb_quantization_range_ctrl)
        state->rgb_quantization_range_ctrl->is_private = true;
    state->analog_sampling_phase_ctrl =
    	v4l2_ctrl_new_custom(hdl, &adv7604_ctrl_analog_sampling_phase, NULL);
    if (state->analog_sampling_phase_ctrl)
        state->analog_sampling_phase_ctrl->is_private = true;
    state->free_run_color_manual_ctrl =
    	v4l2_ctrl_new_custom(hdl, &adv7604_ctrl_free_run_color_manual, NULL);
    if (state->free_run_color_manual_ctrl)
        state->free_run_color_manual_ctrl->is_private = true;
    state->free_run_color_ctrl =
    	v4l2_ctrl_new_custom(hdl, &adv7604_ctrl_free_run_color, NULL);
    if (state->free_run_color_ctrl)
        state->free_run_color_ctrl->is_private = true;

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
    v4l2_dbg(2, debug, &state->sd, "%s: returning %d\n", __func__, err);

    return err;
}

static void adv7604_exit_controls(struct adv7604_state *state)
{
    v4l2_ctrl_handler_free(&state->hdl);
}

static int adv7604_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    struct adv7604_state *state;
    struct adv7604_platform_data *pdata;
    struct v4l2_subdev *sd;
    unsigned char rd_info;
    int err;

    v4l_info(client, "%s: probing device at address 0x%x (%s)\n",
	        ADV7604_DRIVER_NAME, client->addr, client->adapter->name);

    /* Check if the adapter supports the needed features */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
    	err = -EIO;
        goto err;
    }

    /* platform data */
    pdata = client->dev.platform_data;
    if (!pdata) {
    	v4l_err(client, "No platform data!\n");
    	err = -ENODEV;
        goto err;
    }
    rd_info = adv_smbus_read_byte_data_check(client, ADV7604_REV_REG, false);
    if (rd_info != ADV7604_REV_VAL) {
        v4l_err(client, "not an adv7604 on address 0x%x (RD_INFO %02x)\n",
            client->addr << 1, rd_info);
        err = -ENODEV;
        goto err;
    }
    v4l_info(client, "ADV7604 revision (0x%x) detected.\n", rd_info);

    state = kzalloc(sizeof(struct adv7604_state), GFP_KERNEL);
    if (state == NULL) {
    	v4l_err(client, "Could not allocate adv7604_state memory!\n");
    	err = -ENOMEM;
        goto err;
    }

    sd = &state->sd;
    v4l2_i2c_subdev_init(sd, client, &adv7604_ops);
    sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
    mutex_init(&state->lock);
    state->connector_hdmi = pdata->connector_hdmi;
    state->pdata = pdata;
    mutex_init(&state->pdata_lock);

    state->i2c_avlink = adv7604_dummy_client(sd, pdata->i2c_avlink, 0xf3);
    state->i2c_cec = adv7604_dummy_client(sd, pdata->i2c_cec, 0xf4);
    state->i2c_infoframe = adv7604_dummy_client(sd, pdata->i2c_infoframe, 0xf5);
    state->i2c_esdp = adv7604_dummy_client(sd, pdata->i2c_esdp, 0xf6);
    state->i2c_dpp = adv7604_dummy_client(sd, pdata->i2c_dpp, 0xf7);
    state->i2c_afe = adv7604_dummy_client(sd, pdata->i2c_afe, 0xf8);
    state->i2c_repeater = adv7604_dummy_client(sd, pdata->i2c_repeater, 0xf9);
    state->i2c_edid = adv7604_dummy_client(sd, pdata->i2c_edid, 0xfa);
    state->i2c_hdmi = adv7604_dummy_client(sd, pdata->i2c_hdmi, 0xfb);
    state->i2c_test = adv7604_dummy_client(sd, pdata->i2c_test, 0xfc);
    state->i2c_cp = adv7604_dummy_client(sd, pdata->i2c_cp, 0xfd);
    state->i2c_vdp = adv7604_dummy_client(sd, pdata->i2c_vdp, 0xfe);
    if (!state->i2c_avlink || !state->i2c_cec || !state->i2c_infoframe ||
        !state->i2c_esdp || !state->i2c_dpp || !state->i2c_afe ||
        !state->i2c_repeater || !state->i2c_edid || !state->i2c_hdmi ||
        !state->i2c_test || !state->i2c_cp || !state->i2c_vdp) {
    	err = -ENOMEM;
    	v4l2_err(sd, "failed to create all i2c clients\n");
    	goto err_free_clients;
    }

    /* set restart_stdi_once */
    state->restart_stdi_once = true;

    /* initialize input video mode */
    state->mode = NETVIZXPI_MODE_HDMI;

    /* set default edid */
    memcpy(state->edid, edid_hdmi, ARRAY_SIZE(state->edid));
    if (edid_write_block(sd, ARRAY_SIZE(state->edid), state->edid) < 0) {
        v4l2_err(sd, "%s: error writing edid\n", __func__);
    }

    /* work queues */
    state->work_queues = create_singlethread_workqueue(client->name);
    if (!state->work_queues) {
    	v4l2_err(sd, "Could not create work queue\n");
    	err = -ENOMEM;
    	goto err_free_clients;
    }

    INIT_DELAYED_WORK(&state->delayed_work_enable_hotplug,
    		adv7604_delayed_work_enable_hotplug);

    /* control handlers */
    err = adv7604_init_controls(state);
    if (err) {
        v4l2_err(sd, "Could not initialize controls\n");
        goto err_work_queues;
    }
    err = adv7604_core_init(sd);
    if (err) {
        v4l2_err(sd, "Could not initialize ADV7604!\n");
    	goto err_free_controls;
    }

    return 0;

err_free_controls:
    adv7604_exit_controls(state);

err_work_queues:
    cancel_delayed_work(&state->delayed_work_enable_hotplug);
    destroy_workqueue(state->work_queues);

err_free_clients:
    adv7604_unregister_clients(state);
    v4l2_device_unregister_subdev(sd);
    kfree(state);

err:
    v4l_info(client, "failed to initialize AD7604. err %d.\n", err);

    return err;
}

/* ----------------------------------------------------------------------- */

static int adv7604_remove(struct i2c_client *client)
{
    struct v4l2_subdev *sd = i2c_get_clientdata(client);
    struct adv7604_state *state = to_state(sd);

    v4l_info(client, "%s: removing %s on address 0x%x (%s)\n", 
	    ADV7604_DRIVER_NAME, client->name, client->addr << 1, 
        client->adapter->name);

    adv7604_exit_controls(state);
    cancel_delayed_work(&state->delayed_work_enable_hotplug);
    destroy_workqueue(state->work_queues);
    adv7604_unregister_clients(state);
    v4l2_device_unregister_subdev(sd);
    kfree(state);

    v4l_info(client, "%s removed\n", client->name);

    return 0;
}

/* ----------------------------------------------------------------------- */

static struct i2c_device_id adv7604_id[] = {
    { ADV7604_DRIVER_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, adv7604_id);

static struct i2c_driver adv7604_driver = {
    .driver = {
    	.owner = THIS_MODULE,
    	.name = ADV7604_DRIVER_NAME,
    },
    .probe = adv7604_probe,
    .remove = adv7604_remove,
    .id_table = adv7604_id,
};

static int __init adv7604_init(void)
{
    return i2c_add_driver(&adv7604_driver);
}

static void __exit adv7604_exit(void)
{
    i2c_del_driver(&adv7604_driver);
}

module_init(adv7604_init);
module_exit(adv7604_exit);

