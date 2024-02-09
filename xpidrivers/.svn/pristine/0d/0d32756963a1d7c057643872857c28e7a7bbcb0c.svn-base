#ifndef __V4L2_DV_BT_TIMINGS_H__

#define __V4L2_DV_BT_TIMINGS_H__

/* 
 * Copied from Linux kernel 3.6 include/linux/v4l2-dv-timings.h
 */
#define V4L2_DV_BT_STD_CEA861 (1 << 0)  /* CEA-861 Digital TV Profile */
#define V4L2_DV_BT_STD_DMT    (1 << 1)  /* VESA Discrete Monitor Timings */
#define V4L2_DV_BT_STD_CVT    (1 << 2)  /* VESA Coordinated Video Timings */
#define V4L2_DV_BT_STD_GTF    (1 << 3)  /* VESA Generalized Timings Formula */ 

/* 
 * Additional V4L2 controls for digital video
 * (https://patchwork.kernel.org/patch/1305101/)
 */

#define ADV7604_CID_CUSTOM_BASE			        (V4L2_CID_USER_BASE | 0xf000)
#define	V4L2_CID_ADV_RX_POWER_PRESENT		    (ADV7604_CID_CUSTOM_BASE + 1)
#define V4L2_CID_ADV_RX_RGB_RANGE		        (ADV7604_CID_CUSTOM_BASE + 2)
#define V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE   (ADV7604_CID_CUSTOM_BASE + 3)
#define V4L2_CID_ADV_RX_ANALOG_HORIZ_POS        (ADV7604_CID_CUSTOM_BASE + 4)
#define V4L2_CID_ADV_RX_ANALOG_VERT_POS         (ADV7604_CID_CUSTOM_BASE + 5)
#define V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL   (ADV7604_CID_CUSTOM_BASE + 6)
#define V4L2_CID_ADV_RX_FREE_RUN_COLOR          (ADV7604_CID_CUSTOM_BASE + 7)

enum v4l2_dv_tx_mode {
    V4L2_DV_TX_MODE_DVI_D	= 0,
    V4L2_DV_TX_MODE_HDMI	= 1,
};

enum v4l2_dv_rgb_range {
    V4L2_DV_RGB_RANGE_AUTO      = 0,
    V4L2_DV_RGB_RANGE_LIMITED   = 1,
    V4L2_DV_RGB_RANGE_FULL      = 2,
};

#define ADV_fsc               (28636360)

/* Invalid timing (No video) */

#define INVALID_DV_BT_TIMING { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
           {0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* CEA-861-E timings (i.e. standard HDTV timings) */

#define V4L2_DV_BT_CEA_640X480P59_94 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 640, 480, 0, 0, \
            25175000, 16, 96, 48, 10, 2, 33, 0, 0, 0, \
            {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_720X480P59_94 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 720, 480, 0, 0, \
            27000000, 16, 62, 60, 9, 6, 30, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_720X576P50 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 720, 576, 0, 0, \
            27000000, 12, 64, 68, 5, 5, 39, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1280X720P24 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 720, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            59400000, 1760, 40, 220, 5, 5, 20, 0, 0, 0, \
            {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1280X720P25 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 720, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 2420, 40, 220, 5, 5, 20, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1280X720P30 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 720, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 1760, 40, 220, 5, 5, 20, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1280X720P50 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 720, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 440, 40, 220, 5, 5, 20, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1280X720P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 720, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 110, 40, 220, 5, 5, 20, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080P24 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 638, 44, 148, 4, 5, 36, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080P25 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 528, 44, 148, 4, 5, 36, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080P30 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 88, 44, 148, 4, 5, 36, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080I50 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 528, 44, 148, 2, 5, 15, 2, 5, 16, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080P50 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            148500000, 528, 44, 148, 4, 5, 36, 0, 0, 0, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080I60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            74250000, 88, 44, 148, 2, 5, 15, 2, 5, 16, \
            {V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_1920X1080P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1080, 0, \
            V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
            148500000, 88, 44, 148, 4, 5, 36, 0, 0, 0, \
            {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_2880X480P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 2880, 480, 0, \
            V4L2_DV_VSYNC_POS_POL, \
            107750000, 80, 280, 360, 3, 10, 7, 0, 0, 0, \
            {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CEA861, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_CEA_2880X576P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 2880, 576, 0, \
        V4L2_DV_VSYNC_POS_POL, \
        131500000, 104, 288, 392, 3, 10, 10, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CEA861, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* VESA Discrete Monitor Timings as per version 1.0, revision 12 */

/* VGA resolutions */
#define V4L2_DV_BT_DMT_640X480P60 V4L2_DV_BT_CEA_640X480P59_94

#define V4L2_DV_BT_DMT_640X480P72 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 640, 480, 0, 0, \
        31500000, 24, 40, 128, 9, 3, 28, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0,  \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_640X480P75 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = {640, 480, 0, 0, \
        31500000, 16, 64, 120, 1, 3, 16, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0,  \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_640X480P85 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = {640, 480, 0, 0, \
        36000000, 56, 56, 80, 1, 3, 25, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0,  \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* SVGA resolutions */
#define V4L2_DV_BT_DMT_800X600P56 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 800, 600, 0, 0, \
        36000000, 24, 72, 128, 1, 2, 22, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_800X600P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 800, 600, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        40000000, 40, 128, 88, 1, 4, 23, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_800X600P72 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 800, 600, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        50000000, 56, 120, 64, 37, 6, 23, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_800X600P75 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 800, 600, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        49500000, 16, 80, 160, 1, 3, 21, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_800X600P85 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 800, 600, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        56250000, 32, 64, 152, 1, 3, 27, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* XGA resolutions */
#define V4L2_DV_BT_DMT_1024X768P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1024, 768, 0, 0, \
        65000000, 24, 136, 160, 3, 6, 29, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1024X768P70 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1024, 768, 0, 0, \
        75000000, 24, 136, 144, 3, 6, 29, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1024X768P75 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1024, 768, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        78750000, 16, 96, 176, 1, 3, 28, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1024X768P85 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1024, 768, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        94500000, 48, 96, 208, 1, 3, 36, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* WXGA resolutions */
#define V4L2_DV_BT_DMT_1280X768P60_RB { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 768, 0, V4L2_DV_HSYNC_POS_POL, \
        68250000, 48, 32, 80, 3, 7, 12, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1280X768P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 768, 0, V4L2_DV_VSYNC_POS_POL, \
        79500000, 64, 128, 192, 3, 7, 20, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0 \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
}

/* SXGA resolutions */
#define V4L2_DV_BT_DMT_1280X1024P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 1024, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        108000000, 48, 112, 248, 1, 3, 38, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1280X1024P75 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1280, 1024, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        135000000, 16, 144, 248, 1, 3, 38, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1360X768P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1360, 768, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        85500000, 64, 112, 256, 3, 6, 18, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1366X768P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1366, 768, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        85500000, 70, 143, 213, 3, 3, 24, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* SXGA+ resolutions */
#define V4L2_DV_BT_DMT_1400X1050P60_RB { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1400, 1050, 0, V4L2_DV_HSYNC_POS_POL, \
        101000000, 48, 32, 80, 3, 4, 23, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1400X1050P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1400, 1050, 0, V4L2_DV_VSYNC_POS_POL, \
        121750000, 88, 144, 232, 3, 4, 32, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1400X1050P75 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1400, 1050, 0, V4L2_DV_VSYNC_POS_POL, \
        156000000, 104, 144, 248, 3, 4, 42, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* WXGA+ resolutions */
#define V4L2_DV_BT_DMT_1440X900P60_RB { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1440, 900, 0, V4L2_DV_HSYNC_POS_POL, \
        88750000, 48, 32, 80, 3, 6, 17, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1440X900P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1440, 900, 0, V4L2_DV_VSYNC_POS_POL, \
        106500000, 80, 152, 232, 3, 6, 25, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* UXGA resolutions */
#define V4L2_DV_BT_DMT_1600X1200P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1600, 1200, 0, \
        V4L2_DV_HSYNC_POS_POL | V4L2_DV_VSYNC_POS_POL, \
        162000000, 64, 192, 304, 1, 3, 46, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* WUXGA resolutions */
#define V4L2_DV_BT_DMT_1920X1200P60_RB { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1920, 1200, 0, \
        V4L2_DV_HSYNC_POS_POL, \
        154000000, 48, 32, 80, 3, 6, 26, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

/* WSXGA+ resolutions */
#define V4L2_DV_BT_DMT_1680X1050P60_RB { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1680, 1050, 0, V4L2_DV_HSYNC_POS_POL, \
        119000000, 48, 32, 80, 3, 6, 21, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1680X1050P60 { \
    .type = V4L2_DV_BT_656_1120, \
    .bt = { 1680, 1050, 0, V4L2_DV_VSYNC_POS_POL, \
        146250000, 104, 176, 280, 3, 6, 30, 0, 0, 0, \
        {V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_CVT, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} \
    } \
}

#define V4L2_DV_BT_DMT_1920X1080P60 V4L2_DV_BT_CEA_1920X1080P60

#endif
