/*
 * adv7612.h 
 */

#ifndef _ADV7612_
#define _ADV7612_

/* Flags bits for the ADV7604 driver */
#define ADV7612_FLAG_VERBOSE        0x0001
#define ADV7612_FLAG_LLC_PHASE_MASK 0x01f0

/* Default LLC phase */
#define ADV7612_LLC_PHASE           3

/* Bus rotation and reordering (IO register 0x04, [7:5]) */
enum adv7612_op_ch_sel {
    ADV7612_OP_CH_SEL_GBR = 0,
    ADV7612_OP_CH_SEL_GRB = 1,
    ADV7612_OP_CH_SEL_BGR = 2,
    ADV7612_OP_CH_SEL_RGB = 3,
    ADV7612_OP_CH_SEL_BRG = 4,
    ADV7612_OP_CH_SEL_RBG = 5,
};

/* Primary mode (IO register 0x01, [3:0]) */
enum adv7612_mode {
    ADV7612_MODE_HDMI_COMP = 5,
    ADV7612_MODE_HDMI_GR = 6,
};

/* Input Color Space (IO register 0x02, [7:4]) */
enum adv7612_inp_color_space {
    ADV7612_INP_COLOR_SPACE_LIM_RGB = 0,
    ADV7612_INP_COLOR_SPACE_FULL_RGB = 1,
    ADV7612_INP_COLOR_SPACE_LIM_YCbCr_601 = 2,
    ADV7612_INP_COLOR_SPACE_LIM_YCbCr_709 = 3,
    ADV7612_INP_COLOR_SPACE_XVYCC_601 = 4,
    ADV7612_INP_COLOR_SPACE_XVYCC_709 = 5,
    ADV7612_INP_COLOR_SPACE_FULL_YCbCr_601 = 6,
    ADV7612_INP_COLOR_SPACE_FULL_YCbCr_709 = 7,
    ADV7612_INP_COLOR_SPACE_AUTO = 0xf,
};

/* Select output format (IO register 0x03, [7:0]) */
enum adv7612_op_format_sel {
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_8 = 0x00,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_10 = 0x01,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_12_MODE0 = 0x02,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_12_MODE1 = 0x06,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_12_MODE2 = 0x0a,
    ADV7612_OP_FORMAT_SEL_DDR_422_8 = 0x20,
    ADV7612_OP_FORMAT_SEL_DDR_422_10 = 0x21,
    ADV7612_OP_FORMAT_SEL_DDR_422_12_MODE0 = 0x22,
    ADV7612_OP_FORMAT_SEL_DDR_422_12_MODE1 = 0x23,
    ADV7612_OP_FORMAT_SEL_DDR_422_12_MODE2 = 0x24,
    ADV7612_OP_FORMAT_SEL_SDR_444_24 = 0x40,
    ADV7612_OP_FORMAT_SEL_SDR_444_30 = 0x41,
    ADV7612_OP_FORMAT_SEL_SDR_444_36_MODE0 = 0x42,
    ADV7612_OP_FORMAT_SEL_DDR_444_24 = 0x60,
    ADV7612_OP_FORMAT_SEL_DDR_444_30 = 0x61,
    ADV7612_OP_FORMAT_SEL_DDR_444_36 = 0x62,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_16 = 0x80,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_20 = 0x81,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_24_MODE0 = 0x82,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_24_MODE1 = 0x86,
    ADV7612_OP_FORMAT_SEL_SDR_ITU656_24_MODE2 = 0x8a,
    ADV7612_OP_FORMAT_SEL_SDR_422_20_MODE3 = 0x8d,
    ADV7612_OP_FORMAT_SEL_SDR_422_16_MODE4 = 0x90,
    ADV7612_OP_FORMAT_SEL_SDR_422_20_MODE4 = 0x91,
    ADV7612_OP_FORMAT_SEL_SDR_422_24_MODE4 = 0x92,
};

#define ADV7612_REV_HI_REG	0xEA	/* Chip revision high byte */
#define ADV7612_REV_LO_REG	0xEB	/* Chip revision low byte */
#define ADV7612_REV_HI		0x20	/* High byte value */
#define ADV7612_REV_LO		0x41	/* Low byte value */

#ifdef __KERNEL__

/* Platform dependent definition */

struct adv7612_platform_data {

    /* IO register 0x01: input Color Space. Default 1111 - in analog mode, 
     * input color space depends on Primary Mode and Video Standard. In HDMI 
     * mode, input color space depends on color space reported by HDMI block */
    enum adv7612_inp_color_space inp_color_space;

    /* IO register 0x02: default 0000 - no conversion, enable full output 
     * range of digital data, YPrPb color space out, data saturator enabled 
     * or disabled according to OP_656_RANGE setting */
    unsigned alt_gamma:1;
    unsigned op_656_range:1;
    unsigned rgb_out:1;
    unsigned alt_data_sat:1;

    /* IO register 0x03: select output format */
    enum adv7612_op_format_sel op_format_sel;

    /* IO register 0x04: bus rotation and reordering */
    enum adv7612_op_ch_sel op_ch_sel;

    /* IO register 0x05: default 1100 - blank data during horiz and vert
     * blanking periods, insert AV codes into data stream, outputs SAV/EAV 
     * codes on Y channel only, outputs Cr and Cb as per OP_FORMAT_SEL */
    unsigned data_blank_en:1;
    unsigned avcode_insert_en:1;
    unsigned repl_av_code:1;
    unsigned op_swap_cbcr:1;

    /* IO register 0x06: default 100 - vsync output on VS/FIELD pin,
     * negative polarity VS/FIELD, negative polarity HS */
    unsigned vs_out_sel:1;
    unsigned inv_vs_pol:1;
    unsigned inv_hs_pol:1;

    /* IO register 0x19[4:0]: LLC DLL phase */
    unsigned char llc_dll_phase;

    /* HDMI register 0x48: default 0 - reset the HDMI section if the 5 V 
     * input pin corresponding to the selected HDMI port (e.g. RXA_5V for 
     * port A) is inactive */
    unsigned dis_cable_det_rst:1;

    /* i2c addresses: 0 == use default */
    unsigned char i2c_cec;
    unsigned char i2c_infoframe;
    unsigned char i2c_repeater;
    unsigned char i2c_edid;
    unsigned char i2c_hdmi;
    unsigned char i2c_cp;

    /* Flag bits for the ADV7612 driver */
    unsigned flags;
};

extern int adv7612_query_dv_timings(struct v4l2_subdev *sd,
                                    struct v4l2_dv_timings *timings);

#endif
#endif
