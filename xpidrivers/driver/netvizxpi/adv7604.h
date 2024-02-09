/*
 * adv7604 - Analog Devices ADV7604 video decoder driver
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

#ifndef _ADV7604_
#define _ADV7604_

#ifndef __NETVIZXPI_H_INCLUDED__
#include "netvizxpi.h"
#endif

/* Flags bits for the ADV7604 driver */
#define ADV7604_FLAG_VERBOSE        0x01
#define ADV7604_FLAG_SKIP_STDI      0x02

/* Analog input muxing modes (AFE register 0x02, [2:0]) */

enum adv7604_ain_sel {
    ADV7604_AIN1_2_3_NC_SYNC_1_2 = 0,
    ADV7604_AIN4_5_6_NC_SYNC_2_1 = 1,
    ADV7604_AIN7_8_9_NC_SYNC_3_1 = 2,
    ADV7604_AIN10_11_12_NC_SYNC_4_1 = 3,
    ADV7604_AIN9_4_5_6_SYNC_2_1 = 4,
};

/* Bus rotation and reordering (IO register 0x04, [7:5]) */

enum adv7604_op_ch_sel {
    ADV7604_OP_CH_SEL_GBR = 0,
    ADV7604_OP_CH_SEL_GRB = 1,
    ADV7604_OP_CH_SEL_BGR = 2,
    ADV7604_OP_CH_SEL_RGB = 3,
    ADV7604_OP_CH_SEL_BRG = 4,
    ADV7604_OP_CH_SEL_RBG = 5,
};

/* Input Color Space (IO register 0x02, [7:4]) */

enum adv7604_inp_color_space {
    ADV7604_INP_COLOR_SPACE_LIM_RGB = 0,
    ADV7604_INP_COLOR_SPACE_FULL_RGB = 1,
    ADV7604_INP_COLOR_SPACE_LIM_YCbCr_601 = 2,
    ADV7604_INP_COLOR_SPACE_LIM_YCbCr_709 = 3,
    ADV7604_INP_COLOR_SPACE_XVYCC_601 = 4,
    ADV7604_INP_COLOR_SPACE_XVYCC_709 = 5,
    ADV7604_INP_COLOR_SPACE_FULL_YCbCr_601 = 6,
    ADV7604_INP_COLOR_SPACE_FULL_YCbCr_709 = 7,
    ADV7604_INP_COLOR_SPACE_AUTO = 0xf,
};

/* Select output format (IO register 0x03, [7:0]) */

enum adv7604_op_format_sel {
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_8 = 0x00,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_10 = 0x01,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_12_MODE0 = 0x02,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_12_MODE1 = 0x06,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_12_MODE2 = 0x0a,
    ADV7604_OP_FORMAT_SEL_DDR_422_8 = 0x20,
    ADV7604_OP_FORMAT_SEL_DDR_422_10 = 0x21,
    ADV7604_OP_FORMAT_SEL_DDR_422_12_MODE0 = 0x22,
    ADV7604_OP_FORMAT_SEL_DDR_422_12_MODE1 = 0x23,
    ADV7604_OP_FORMAT_SEL_DDR_422_12_MODE2 = 0x24,
    ADV7604_OP_FORMAT_SEL_SDR_444_24 = 0x40,
    ADV7604_OP_FORMAT_SEL_SDR_444_30 = 0x41,
    ADV7604_OP_FORMAT_SEL_SDR_444_36_MODE0 = 0x42,
    ADV7604_OP_FORMAT_SEL_DDR_444_24 = 0x60,
    ADV7604_OP_FORMAT_SEL_DDR_444_30 = 0x61,
    ADV7604_OP_FORMAT_SEL_DDR_444_36 = 0x62,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_16 = 0x80,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_20 = 0x81,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_24_MODE0 = 0x82,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_24_MODE1 = 0x86,
    ADV7604_OP_FORMAT_SEL_SDR_ITU656_24_MODE2 = 0x8a,
};

#define ADV7604_REV_REG		    0x11	/* Chip revision register */
#define ADV7604_REV_VAL		    0x03	/* Revision value */

#define ADV7604_VID_STD_HDMI    0x02    /* HDMI video standard */
#define ADV7604_VID_STD_AUTO    0x07    /* Auto-graphics video standard */

/* Analog parameters */

struct adv7604_analog_parameters {
    int analog_hstart_set;
    s16 analog_hstart;
    int analog_vstart_set;
    s16 analog_vstart;
    int analog_sampling_phase_set;
    u8 analog_sampling_phase;
};

/* Platform dependent definition */

struct adv7604_platform_data {
    /* connector - HDMI or DVI? */
    unsigned connector_hdmi:1;

    /* IO register 0x01: input Color Space. Default 1111 - input color 
     * space depends on color space reported by HDMI block */
    enum adv7604_inp_color_space inp_color_space;

    /* IO register 0x02: default 0000 - no conversion, enable full output
     * range of digital data, YPrPb color space out, data saturator enabled
     * or disabled according to OP_656_RANGE setting */
    unsigned alt_gamma:1;
    unsigned op_656_range:1;
    unsigned rgb_out:1;
    unsigned alt_data_sat:1;

    /* IO register 0x03: select output format */
    enum adv7604_op_format_sel op_format_sel;

    /* IO register 0x04: bus rotation and reordering */
    enum adv7604_op_ch_sel op_ch_sel;

    /* IO register 0x05: default 1100 - blank data during horiz and vert
     * blanking periods, insert AV codes into data stream, outputs SAV/EAV
     * codes on Y channel only, outputs Cr and Cb as per OP_FORMAT_SEL */
    unsigned blank_data_en:1;
    unsigned avcode_insert_en:1;
    unsigned repl_av_code:1;
    unsigned op_swap_cbcr:1;

    /* IO register 0x06: default 100 - vsync output on VS/FIELD pin,
     * negative polarity Vsync, negative polarity Hsync */
    unsigned vs_out_sel: 1;
    unsigned inv_vs_pol: 1;
    unsigned inv_hs_pol: 1;

    /* IO register 0x07 */
    unsigned sync_ch_auto_mode: 1;
    unsigned sync_ch1_priority: 1;
    unsigned sync_ch1_hs_sel: 2;
    unsigned sync_ch1_vs_sel: 2;

    /* HDMI register 0x48: default 0 - when low PWRDNB powers down the chip, 
     * and activates all internal EDIDs */
    unsigned disable_pwrdnb:1;

    /* HDMI register 0x48: default 0: reset the HDMI section if the 5 V 
     * input pin corresponding to the selected HDMI port (e.g. RXA_5V for 
     * port A) is inactive */
    unsigned disable_cable_det_rst:1;

    /* AFE register 0x02: default 000 - analog input muxing mode 
     * Ain1 | Ain2 | Ain3 | NC | Sync1 | Sync 2 */
    enum adv7604_ain_sel ain_sel;

    /* CP register 0x8b and 0x8d: DE_H_START */
    s16 analog_hstart;

    /* CP register 0x8e: DE_V_START */
    s16 analog_vstart;

    /* AFE register 0xc8 DLL_PHASE: default 0 - analog sampling phase */
    u8 analog_sampling_phase;

    /* CP registers 0xb8-0xbd: default 0 - analog horizontal position */
    s16 analog_horiz_start, analog_horiz_end;

    /* CP registers 0xbe: default 0 - analog vertical position */
    s8 analog_vert_start, analog_vert_end;

    /* IO register 0x30: default 0 - output bus goes from LSB to MSB */
    unsigned pixbus_msb_to_lsb_reorder:1;

    /* CP register 0xba: default 00 - disable free run in HDMI mode */
    unsigned hdmi_frun_en : 1;
    unsigned hdmi_frun_mode : 1;

    /* i2c addresses: 0 == use default */
    u8 i2c_avlink;
    u8 i2c_cec;
    u8 i2c_infoframe;
    u8 i2c_esdp;
    u8 i2c_dpp;
    u8 i2c_afe;
    u8 i2c_repeater;
    u8 i2c_edid;
    u8 i2c_hdmi;
    u8 i2c_test;
    u8 i2c_cp;
    u8 i2c_vdp;

    /* Flag bits for ADV7604 driver */
    unsigned flags;
};

extern bool adv7604_status_change(struct v4l2_subdev *sd);
extern bool v4l_match_dv_timings(const struct v4l2_dv_timings *t1,
                                 const struct v4l2_dv_timings *t2,
                                 unsigned pclock_delta);
extern void adv7604_print_timings(struct v4l2_subdev *sd,
                                  struct v4l2_dv_timings *timings,
                                  const char *txt, 
                                  bool detailed);
extern int adv7604_query_dv_timings(struct v4l2_subdev *sd,
                                    struct v4l2_dv_timings *timings);
extern int adv7604_g_stdi_readback(struct v4l2_subdev *sd,
                                   struct stdi_readback *stdi);
extern void adv7604_setup_videoinput(struct v4l2_subdev *sd,
                                     struct v4l2_dv_timings *timings);
extern int adv7604_set_analog_parameters(struct v4l2_subdev *sd,
                                         struct adv7604_analog_parameters *param);
extern int adv7604_set_analog_timings(struct v4l2_subdev *sd,
                                      struct v4l2_dv_timings *timings);
extern int adv7604_set_free_run(struct v4l2_subdev *sd, int enable);
#endif
