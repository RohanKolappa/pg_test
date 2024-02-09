#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <errno.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "device_detection.h"
#include "osd_control.h"
#include "chromakey_control.h"

#define VLOG_CATEGORY_NAME          "chromakey"
#define RDPIXEL_LOCK_RETRY_COUNT    10

int hal_get_chromakey_enable_k(HAL *hp, int matte) {
    if (matte == MATTE_LOCAL) {
        if (hal_get_fpga_val32_k(hp, FPGA_ATESTREG) & LMATTE_ENABLE)
            return 1;
        else
            return 0;
    }
    else {
        if (hal_get_fpga_val32_k(hp, FPGA_ATESTREG) & RMATTE_ENABLE)
            return 1;
        else
            return 0;
    }
}

void hal_set_chromakey_enable_k(HAL *hp, int matte, int enable) {
    if (matte == MATTE_LOCAL) {
        if (enable)
            hal_or_in_fpga_val32_k(hp, FPGA_ATESTREG, LMATTE_ENABLE);
        else
            hal_and_in_fpga_val32_k(hp, FPGA_ATESTREG, ~LMATTE_ENABLE);
    }
    else {
        if (enable)
            hal_or_in_fpga_val32_k(hp, FPGA_ATESTREG, RMATTE_ENABLE);
        else
            hal_and_in_fpga_val32_k(hp, FPGA_ATESTREG, ~RMATTE_ENABLE);
    }
}

int hal_get_chromakey_alpha_k(HAL *hp, int matte) {

    if (matte == MATTE_LOCAL)
        return (100 - hal_get_fpga_val_k(hp, FPGA_DALPHAM2_L) * 100 / 1024);
    else
        return (100 - hal_get_fpga_val_k(hp, FPGA_DALPHAM1_R) * 100 / 1024);
}

void hal_set_chromakey_alpha_k(HAL *hp, int matte, int alpha) {
    int val;
    if (alpha < 0 || alpha > 100)
        return;
    val = 1024 - alpha * 1024 / 100; 
    if (matte == MATTE_LOCAL) {
        hal_set_fpga_val_k(hp, FPGA_DALPHAM2_L, val);
        hal_set_fpga_val_k(hp, FPGA_DALPHAM2_R, 1024 - val); 
    }
    else {
        hal_set_fpga_val_k(hp, FPGA_DALPHAM1_R, val);
        hal_set_fpga_val_k(hp, FPGA_DALPHAM1_L, 1024 - val); 
    }
}

void hal_get_chromakey_color_value_k(HAL *hp, 
                                     int matte,
                                     unsigned char *redvalue,
                                     unsigned char *redtol,
                                     unsigned char *greenvalue,
                                     unsigned char *greentol,
                                     unsigned char *bluevalue,
                                     unsigned char *bluetol) {
    unsigned long low, high;
    int lo, hi, val, tol;

    if (matte == MATTE_LOCAL) {
        low = hal_get_fpga_val32_k(hp, FPGA_DTHRESM2_L);
        high = hal_get_fpga_val32_k(hp, FPGA_DTHRESM2_H);
    }
    else {
        low = hal_get_fpga_val32_k(hp, FPGA_DTHRESM1_L);
        high = hal_get_fpga_val32_k(hp, FPGA_DTHRESM1_H);
    }

    lo = (unsigned char) ((low & 0x00ff0000) >> 16);
    hi = (unsigned char) ((high & 0x00ff0000) >> 16);
    val = (hi + lo) / 2;
    tol = (hi - lo) / 2;
    *redvalue = (val > 255) ? 255 : ((val < 0) ? 0 : val); 
    *redtol = (tol > 255) ? 255 : ((tol < 0) ? 0 : tol); 

    lo = (unsigned char) ((low & 0x0000ff00) >> 8);
    hi = (unsigned char) ((high & 0x0000ff00) >> 8);
    val = (hi + lo) / 2;
    tol = (hi - lo) / 2;
    *greenvalue = (val > 255) ? 255 : ((val < 0) ? 0 : val); 
    *greentol = (tol > 255) ? 255 : ((tol < 0) ? 0 : tol); 
    
    lo = (unsigned char) (low & 0x000000ff);
    hi = (unsigned char) (high & 0x000000ff);
    val = (hi + lo) / 2;
    tol = (hi - lo) / 2;
    *bluevalue = (val > 255) ? 255 : ((val < 0) ? 0 : val); 
    *bluetol = (tol > 255) ? 255 : ((tol < 0) ? 0 : tol); 
}

void hal_set_chromakey_color_value_k(HAL *hp, 
                                     int matte,
                                     unsigned char redvalue,
                                     unsigned char redtol,
                                     unsigned char greenvalue,
                                     unsigned char greentol,
                                     unsigned char bluevalue,
                                     unsigned char bluetol) {
    unsigned long low, high;
    unsigned char lo, hi;

    lo = (redvalue - redtol > 0) ? (redvalue - redtol) : 0;
    hi = (redvalue + redtol < 255) ? (redvalue + redtol) : 255;
    low = (unsigned long) lo << 16;
    high = (unsigned long) hi << 16;

    lo = (greenvalue - greentol > 0) ? (greenvalue - greentol) : 0;
    hi = (greenvalue + greentol < 255) ? (greenvalue + greentol) : 255;
    low |= (unsigned long) lo << 8;
    high |= (unsigned long) hi << 8;

    lo = (bluevalue - bluetol > 0) ? (bluevalue - bluetol) : 0;
    hi = (bluevalue + bluetol < 255) ? (bluevalue + bluetol) : 255;
    low |= (unsigned long) lo;
    high |= (unsigned long) hi;

    if (matte == MATTE_LOCAL) {
        hal_set_fpga_val32_k(hp, FPGA_DTHRESM2_L, low);
        hal_set_fpga_val32_k(hp, FPGA_DTHRESM2_H, high);
    }
    else {
        hal_set_fpga_val32_k(hp, FPGA_DTHRESM1_L, low);
        hal_set_fpga_val32_k(hp, FPGA_DTHRESM1_H, high);
    }
}

static int chromakey_rdpixel_lock(HAL *hp) {
    int val = 1;
    if (ioctl(hp->kernel_fd, FPGA_IOC_LOCK_RDPIXEL, &val) < 0)
        return -1;
    return 0;
}
                                                                                
static int chromakey_rdpixel_unlock(HAL *hp) {
    int val = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_LOCK_RDPIXEL, &val) < 0)
        return -1;
    return 0;
}

int hal_get_chromakey_pixel_color_k(HAL *hp, 
                                    int matte,
                                    int x, 
                                    int y,
                                    unsigned char *rval,
                                    unsigned char *gval,
                                    unsigned char *bval) {
    unsigned long lval;
    int i, active_interface, is_odd = 0;

    if ((active_interface = hal_getActiveIntf(hp)) < 0)
        return -1;

    /* Lock RDPIXEL register */
    for (i = 0; i < RDPIXEL_LOCK_RETRY_COUNT; i++) {
        if (chromakey_rdpixel_lock(hp) < 0)
            usleep(5000);
        else
            break;
    }
    if (i >= RDPIXEL_LOCK_RETRY_COUNT) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d. Failed to lock RDPIXEL register",
                hal_get_channel_number(hp));
        return -1;
    }

    /* Remote matte positions are relative to remote window */
    if (matte == MATTE_REMOTE) {
        x = (x * hal_get_fpga_val_k(hp, FPGA_DHSCALE)) / RX_MAX_SCALE_VAL +
            hal_get_fpga_val_k(hp, FPGA_DHSHIFT);
        y = (y * hal_get_fpga_val_k(hp, FPGA_DVSCALE)) / RX_MAX_SCALE_VAL +
            hal_get_fpga_val_k(hp, FPGA_DVSHIFT);
    }

    /* Half the horiz pixel position if display is in 48 bit mode */
    if (active_interface & INTERFACE_48BIT) {
        is_odd = (x % 2);
        x /= 2;
    }

    lval = (unsigned long) (x << 16) | (unsigned long) y;

    hal_set_fpga_val32_k(hp, FPGA_RDPIXELLOC, lval);

    /* Wait for at least 1 frame to go by */
    for (i = 0; i < 50; i++) {
        struct fc_wparam_io fcwpio;
        hal_get_fc_wparams(hp, &fcwpio);
        if (i > 1 && fcwpio.wframe_cnt > lval) {
            break; 
        }
        lval = fcwpio.wframe_cnt;
        usleep(20000);
    }
    if (i == 50) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d. Failed read valid frame",
                hal_get_channel_number(hp));
        /* Unlock RDPIXEL register */
        chromakey_rdpixel_unlock(hp);
        return -1;
    }

    /* Use even pixel color if pixel is even, else use odd */
    if (matte == MATTE_LOCAL) {
        if (is_odd)
            lval = hal_get_fpga_val32_k(hp, FPGA_LPIXINODD); 
        else
            lval = hal_get_fpga_val32_k(hp, FPGA_LPIXINEVEN); 
    }
    else  {
        if (is_odd)
            lval = hal_get_fpga_val32_k(hp, FPGA_RPIXINODD); 
        else
            lval = hal_get_fpga_val32_k(hp, FPGA_RPIXINEVEN); 
    }
    *rval = (unsigned char) ((lval & 0x00ff0000) >> 16);
    *gval = (unsigned char) ((lval & 0x0000ff00) >> 8);
    *bval = (unsigned char) (lval & 0x000000ff);

    /* Unlock RDPIXEL register */
    chromakey_rdpixel_unlock(hp);

    return 0;
}

static int get_reference_color_from_pixels(HAL *hp,
                                           int matte,
                                           struct chromakey_config *cfg,
                                           unsigned char *rval,
                                           unsigned char *rtol,
                                           unsigned char *gval, 
                                           unsigned char *gtol,
                                           unsigned char *bval,
                                           unsigned char *btol) {
    unsigned char r1, g1, b1, r2, g2, b2, r3, g3, b3, rt, gt, bt;
    int x1, y1, x2, y2, x3, y3;

    x1 = cfg->pix1x;
    y1 = cfg->pix1y;
    x2 = cfg->pix2x;
    y2 = cfg->pix2y;
    x3 = cfg->pix3x;
    y3 = cfg->pix3y;
    rt = cfg->redtolerance;
    gt = cfg->greentolerance;
    bt = cfg->bluetolerance;

    if (hal_get_chromakey_pixel_color_k(hp, matte, x1, y1, &r1, &g1, &b1) < 0)
        return -1;
    if (hal_get_chromakey_pixel_color_k(hp, matte, x2, y2, &r2, &g2, &b2) < 0)
        return -1;
    if (hal_get_chromakey_pixel_color_k(hp, matte, x3, y3, &r3, &g3, &b3) < 0)
        return -1;

    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, 
            "Channel %d. Pixel #1(%d,%d) = 0x%x/0x%x/0x%x", 
            hal_get_channel_number(hp), x1, y1, r1, g1, b1);
    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME,
            "           Pixel #2(%d,%d) = 0x%x/0x%x/0x%x", x2, y2, r2, g2, b2);
    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME,
            "           Pixel #3(%d,%d) = 0x%x/0x%x/0x%x", x3, y3, r3, g3, b3);
    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME,
            "           Tolerance = 0x%x/0x%x/0x%x", rt, gt, bt);

    if (OVLP(VALMAX(r1,rt), VALMIN(r1,rt), VALMAX(r2,rt), VALMIN(r2,rt)) &&
         OVLP(VALMAX(g1,gt), VALMIN(g1,gt), VALMAX(g2,gt), VALMIN(g2,gt)) &&
         OVLP(VALMAX(b1,bt), VALMIN(b1,bt), VALMAX(b2,bt), VALMIN(b2,bt))) {
        /* pixel 1 and 2 matched, use pixel 1 */
        VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, "Pixel 1 and Pixel 2 matched. ");
        *rval = r1; *gval = g1; *bval = b1;
        *rtol = rt; *gtol = gt; *btol = bt;
    }
    else if (OVLP(VALMAX(r1,rt), VALMIN(r1,rt), VALMAX(r3,rt), VALMIN(r3,rt)) &&
         OVLP(VALMAX(g1,gt), VALMIN(g1,gt), VALMAX(g3,gt), VALMIN(g3,gt)) &&
         OVLP(VALMAX(b1,bt), VALMIN(b1,bt), VALMAX(b3,bt), VALMIN(b3,bt))) {
        /* pixel 1 and 3 matched, use pixel 1 */
        VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, "Pixel 1 and Pixel 3 matched. ");
        *rval = r1; *gval = g1; *bval = b1;
        *rtol = rt; *gtol = gt; *btol= bt;
    }
    else if (OVLP(VALMAX(r2,rt), VALMIN(r2,rt), VALMAX(r3,rt), VALMIN(r3,rt)) &&
         OVLP(VALMAX(g2,gt), VALMIN(g2,gt), VALMAX(g3,gt), VALMIN(g3,gt)) &&
         OVLP(VALMAX(b2,bt), VALMIN(b2,bt), VALMAX(b3,bt), VALMIN(b3,bt))) {
        /* pixel 2 and 3 matched, use pixel 2 */
        VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, "Pixel 2 and Pixel 3 matched. ");
        *rval = r2; *gval = g2; *bval = b2;
        *rtol = rt; *gtol = gt; *btol = bt;
    }
    else {
        /* no two pixels matched, use pixel 1 */
        VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, "No Pixels matched. ");
        *rval = r1; *gval = g1; *bval = b1;
        *rtol = rt; *gtol = gt; *btol = bt;
    }

    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, 
            "Channel %d. Returning 0x%x/0x%x, 0x%x/0x%x, 0x%x/0x%x", 
            hal_get_channel_number(hp), *rval, *rtol, *gval, *gtol, 
            *bval, *btol);
    return 0;
}

int set_color_registers_from_values(HAL *hp, 
                                    int matte,
                                    struct chromakey_config *cfg) {
    unsigned char redtol, greentol, bluetol; 
    unsigned char redvalue, greenvalue, bluevalue;

    redvalue = cfg->redvalue;
    redtol = cfg->redtolerance;
    greenvalue = cfg->greenvalue;
    greentol = cfg->greentolerance;
    bluevalue = cfg->bluevalue;
    bluetol = cfg->bluetolerance;

    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME,
           "Channel %d. Setting %s color 0x%x/0x%x, 0x%x/0x%x, 0x%x/0x%x",
           hal_get_channel_number(hp), (matte == MATTE_LOCAL) ? "local" : 
           "remote", redvalue, redtol, greenvalue, greentol,
           bluevalue, bluetol);

    hal_set_chromakey_color_value_k(hp, matte, redvalue, redtol, greenvalue, 
            greentol, bluevalue, bluetol); 
    return 0;
}

int set_color_registers_from_pixels(HAL *hp, 
                                    int matte,
                                    struct chromakey_config *cfg) {
    unsigned char redtol, greentol, bluetol; 
    unsigned char redvalue, greenvalue, bluevalue;

    if (get_reference_color_from_pixels(hp, matte, cfg, &redvalue, &redtol, 
            &greenvalue, &greentol, &bluevalue, &bluetol) < 0) {
        VLOG_WARNING_CAT(VLOG_CATEGORY_NAME, 
                "Channel %d. Failed to read %s chromakey pixel colors",
                hal_get_channel_number(hp), 
                (matte == MATTE_LOCAL) ? "local" : "remote");
        return -1;
    }

    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME,
           "Channel %d. Setting %s color 0x%x/0x%x, 0x%x/0x%x, 0x%x/0x%x",
           hal_get_channel_number(hp), (matte == MATTE_LOCAL) ? "local" : 
           "remote", redvalue, redtol, greenvalue, greentol, bluevalue, 
           bluetol);

    hal_set_chromakey_color_value_k(hp, matte, redvalue, redtol, greenvalue, 
            greentol, bluevalue, bluetol); 
    return 0;
}
