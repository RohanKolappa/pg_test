#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_console.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "vlog.h"
#include "hald.h"
#include "temperature_control.h"
#include "lut_utils.h"
#include "display_config.h"
#include "override_config.h"
#include "override_utils.h"
#include "osd_control.h"
#include "vlib.h"
#include "strmsrv_comm.h"
#include "hal_LED.h"
#include "tx_connection_config.h"

#ifdef __XPI__
#include "rx_splash_config.h"
#include "netvizxpi/sii7172.h"
#include "ethernet/OmniTek_Ethernet.h"
#include "xpi_i2c_control.h"
#include "xpi_gpio_control.h"
#include "hal_edid.h"
#include "edid_utils.h"
#include "sync_edid_eeprom_config.h"
#else
#include "vutils.h"
#include "fpga.h"
#include "ad9888vif.h"
#include "ad9852dds.h"
#include "led_control.h"
#include "xp_i2c_control.h"
#include "osd_control.h"
#include "chromakey_control.h"
#include "kbms_utils.h"
#include "kbmode_config.h"
#endif

#define PIDPREFIX   "/tmp/hald"
static char pidfile[256];

#define NEW_MATCHING_LUT    1
#define NO_LUT_CHANGE       0
#define NO_MATCHING_LUT     -1

static int debug = 0;

/****************************************************************
 * Monitor temperature. If any of the temperature readings is   *
 * above TEMP_VERYHIGH_SETPOINT, set fan to high, if fan is     *
 * high and all temperatures are below TEMP_HIGH_SETPOINT, set  *
 * fan to medium. If fan is low and any temperatures are above  *
 * TEMP_MEDIUM_SETPOINT, set fan to medium, if all temperatures *
 * are below TEMP_LOW_SETPOINT, set fan to low.                 *
 ****************************************************************/
static void monitor_temperature(HAL *hp) {
    int rc, speed;

    rc = hal_monitor_temperature(hp, 0);

    if (rc < 0) {
        VLOG_WARNING_CAT(VLOG_HALD, "Failed to read XP100 temperature");
    }
    else if (rc != TEMP_NOCHANGE) {
        speed = hal_get_fan_speed(hp);
        if (speed != FAN_SPEED_HIGH && rc == TEMP_RANGE_HIGH) {
            hal_set_fan_speed(hp, FAN_SPEED_HIGH);
        }
        else if (speed == FAN_SPEED_HIGH && rc != TEMP_RANGE_HIGH &&
                rc != TEMP_RANGE_MEDIUM_2) {
            hal_set_fan_speed(hp, FAN_SPEED_MEDIUM);
        }
        else if (speed == FAN_SPEED_LOW && rc != TEMP_RANGE_LOW_2 &&
                rc != TEMP_RANGE_LOW_1) {
            hal_set_fan_speed(hp, FAN_SPEED_MEDIUM);
        }
        else if (speed != FAN_SPEED_LOW && rc == TEMP_RANGE_LOW_1) {
            hal_set_fan_speed(hp, FAN_SPEED_LOW);
        }
    }
}

static void monitor_board_temperature(HAL *hp, int timeout) 
{
    int iter = 0;

    if (++iter > ((TEMPMON_INTERVAL * 1000000) / timeout)) {
        iter = 0;
        monitor_temperature(hp);
    }
}

/**************************************************************
 * Returns 1 if overlay source or scales changed, 0 otherwise *
 **************************************************************/
static int check_source_scale_and_cropping_change(struct display_config *oldcfg,
                                                  struct display_config *cfg) 
{
    if ((oldcfg->source != cfg->source) ||
        (oldcfg->hscale_scheme != cfg->hscale_scheme) ||
        (oldcfg->vscale_scheme != cfg->vscale_scheme) ||
        (cfg->hscale_scheme != SCALE_BY_RESOLUTION && 
         cfg->hscale != oldcfg->hscale) ||
        (cfg->hscale_scheme == SCALE_BY_RESOLUTION && 
            cfg->hres != oldcfg->hres) ||
        (cfg->vscale_scheme != SCALE_BY_RESOLUTION && 
            cfg->vscale != oldcfg->vscale) ||
        (cfg->vscale_scheme == SCALE_BY_RESOLUTION && 
            cfg->vres != oldcfg->vres) ||
        (oldcfg->keep_aspect != cfg->keep_aspect) ||
        (oldcfg->cropping != cfg->cropping ||
                oldcfg->hcropleft != cfg->hcropleft ||
                oldcfg->hcropright != cfg->hcropright ||
                oldcfg->vcroptop != cfg->vcroptop ||
                oldcfg->vcropbottom != cfg->vcropbottom)) {
        return 1;
    }
    return 0;
}

/**************************************************************
 * Update overlay settings in driver for display config 'cfg' *
 **************************************************************/
static void update_overlay_parameters(HAL *hp, struct display_config *cfg) 
{
    struct ov_param_io ov;

    memset(&ov, 0, sizeof(struct ov_param_io));
    if (cfg->source == DISPLAY_SOURCE_LOCAL)
        ov.flags = OV_FLAG_SRC_LOCAL;
    else if (cfg->source == DISPLAY_SOURCE_OVERLAY)
        ov.flags = OV_FLAG_SRC_OVERLAY;
    else
        ov.flags = OV_FLAG_SRC_REMOTE;

    if (cfg->hscale_scheme == SCALE_BY_DISPLAY)
        ov.flags |= OV_FLAG_HSCALEDISPLAY;
    else if (cfg->hscale_scheme == SCALE_BY_SOURCE)
        ov.flags |= OV_FLAG_HSCALESRC;
    else
        ov.flags |= OV_FLAG_HRES;

    if (cfg->vscale_scheme == SCALE_BY_DISPLAY)
        ov.flags |= OV_FLAG_VSCALEDISPLAY;
    else if (cfg->vscale_scheme == SCALE_BY_SOURCE)
        ov.flags |= OV_FLAG_VSCALESRC;
    else
        ov.flags |= OV_FLAG_VRES;

    if (cfg->hpos_scheme == POS_BY_LOCATION)
        ov.flags |= OV_FLAG_HPOSLOCATION;
    else
        ov.flags |= OV_FLAG_HPOSOFFSET;

    if (cfg->vpos_scheme == POS_BY_LOCATION)
        ov.flags |= OV_FLAG_VPOSLOCATION;
    else
        ov.flags |= OV_FLAG_VPOSOFFSET;

    if (cfg->keep_aspect)
        ov.flags |= OV_FLAG_KEEPASPECT;

    ov.hscale = (cfg->hscale * RX_MAX_SCALE_VAL) / 100;
    if (ov.hscale < RX_MIN_SCALE_VAL)
        ov.hscale = RX_MIN_SCALE_VAL;
    ov.vscale = (cfg->vscale * RX_MAX_SCALE_VAL) / 100;
    if (ov.vscale < RX_MIN_SCALE_VAL)
        ov.vscale = RX_MIN_SCALE_VAL;
    ov.hres = cfg->hres;
    ov.vres = cfg->vres;
    ov.hlocation = cfg->hlocation;
    ov.vlocation = cfg->vlocation;
    ov.hoffset = cfg->hoffset;
    ov.voffset = cfg->voffset;
    ov.oalpha = cfg->transparency;

    if (cfg->cropping)
        ov.flags |= OV_FLAG_CROP;
    else
        ov.flags &= ~OV_FLAG_CROP;
    ov.hcropleft = cfg->hcropleft;
    ov.hcropright = cfg->hcropright;
    ov.vcroptop = cfg->vcroptop;
    ov.vcropbottom = cfg->vcropbottom;
    ov.setmask = OV_SET_PARAM_OVERLAY | OV_SET_PARAM_CROP;

    hal_set_overlay_params(hp, &ov);
}

/*****************************************************************
 * Re-configures overlay settings in driver to what's in Display *
 * Parameters menu. It is needed when we switch from override to *
 * no override or when dropside or display LUT changes.          *
 *****************************************************************/
static void reconfigure_overlay_parameters(HAL *hp) 
{
    struct display_config cfg;
    get_display_config(hp, &cfg);
    update_overlay_parameters(hp, &cfg);
}

static void publish_board_status(int channel_number)
{
    char buff[255];

    sprintf(buff, "%s -publish -channelNumber %d",
            "/V2O/bin/get_display_status", channel_number);
    // Cannot allow too many of these to fire
    // Also this should fire only in Managed mode
    system_command(buff);
    VLOG_DEBUG_CAT(VLOG_HALD,
            "Sending status update for channel %s", buff);
}

#ifndef __XPI__
static int CodecChanged;
static int check_reload(HAL *hp);
/***********************************************************
 * This routine configures the AD9888 based on parameters  *
 * in LUT pointed to by 'lutp'                             *
 ***********************************************************/
static void v4_configure_ad9888(HAL *hp, struct LUT_struct *lutp) {
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return;
    if (lutp == NULL || strncmp(lutp->comment, "INVALID", 7) == 0) {
        hal_set_v4_ad9888_val_k(hp, VIF_PLL_MSB, DEFAULT_PLL_MSB);
        hal_set_v4_ad9888_val_k(hp, VIF_PLL_LSB, DEFAULT_PLL_LSB);
        hal_set_v4_ad9888_val_k(hp, VIF_VCO_CPMP, DEFAULT_VCO_CPMP);
        hal_set_v4_ad9888_val_k(hp, VIF_HSYNC_OUT, DEFAULT_HSYNC_OUT);
    }
    else {
        hal_set_v4_ad9888_val_k(hp, VIF_PLL_MSB, lutp->a_adcpll01);
        hal_set_v4_ad9888_val_k(hp, VIF_PLL_LSB, lutp->a_adcpll02);
        hal_set_v4_ad9888_val_k(hp, VIF_VCO_CPMP, lutp->a_adcvco03);
        hal_set_v4_ad9888_val_k(hp, VIF_PHASE_ADJ, lutp->a_phase << 3);

        hal_set_v4_ad9888_val_k(hp, VIF_MODE_MUX, lutp->a_adcmc1);
        hal_set_v4_ad9888_val_k(hp, VIF_TEST1, lutp->a_adcvco16);
        hal_set_v4_ad9888_val_k(hp, VIF_HSYNC_OUT, lutp->a_hsync);
        hal_set_v4_ad9888_val_k(hp, VIF_CLAMP_COAST, 0x6f);

        if (lutp->a_cspace == LUT_CSPACE_YPRPB)
            hal_set_v4_ad9888_val_k(hp, VIF_SYNC_ONGRN, 0x7f);
        else
            hal_set_v4_ad9888_val_k(hp, VIF_SYNC_ONGRN, 0x79);
        hal_set_v4_ad9888_val_k(hp, VIF_RED_GAIN, lutp->a_rgain);
        hal_set_v4_ad9888_val_k(hp, VIF_RED_OFFSET, (lutp->a_roffset << 1));
        hal_set_v4_ad9888_val_k(hp, VIF_GRN_GAIN, lutp->a_ggain);
        hal_set_v4_ad9888_val_k(hp, VIF_GRN_OFFSET, (lutp->a_goffset << 1));
        hal_set_v4_ad9888_val_k(hp, VIF_BLU_GAIN, lutp->a_bgain);
        hal_set_v4_ad9888_val_k(hp, VIF_BLU_OFFSET, (lutp->a_boffset << 1));
    }
}

/******************************************************************
 * This routine monitors the AD9888 coast timing and sync control *
 * registers and selects SOG or sync separator if no H or V sync. *
 * Returns interface type (INTERFACE_HV etc) in return value.     *
 ******************************************************************/
static int v4_monitor_ad9888(HAL *hp) {
    static unsigned char first_time = 1, old_sync_ctl = 0;
    unsigned char sync_detect, sync_ctl, old_coast_ctl, coast_ctl;
    int rc = INTERFACE_HV;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return 0;
    old_coast_ctl = coast_ctl = hal_get_ad9888_val_k(hp, VIF_CLAMP_COAST);
    coast_ctl |= VIFBIT_COAST_SELECT;
    if (coast_ctl != old_coast_ctl) {
        VLOG_DEBUG_CAT(VLOG_VIF,
                "Changing Channel %d VIF_CLAMP_COAST %x --> %x",
                hal_get_channel_number(hp), old_coast_ctl, coast_ctl);
        hal_set_ad9888_val_k(hp, VIF_CLAMP_COAST, coast_ctl);
    }

    /* override auto control of hsync and vsync output */
    sync_detect = hal_get_ad9888_val_k(hp, VIF_SYNC_DETECT);
    sync_ctl = hal_get_ad9888_val_k(hp, VIF_SYNC_CNTL);

    sync_ctl &= ~(VIFBIT_HSYNC_OVERRIDE |
                  VIFBIT_VSYNC_OVERRIDE |
                  VIFBIT_HSYNC_SELECT |
                  VIFBIT_VSYNC_SELECT);

    if (!(sync_detect & VIFBIT_VSYNC_DETECT)) {
        if (!(sync_detect & VIFBIT_HSYNC_DETECT)) {
            /* No hsync. Select SOG for hsync output */
            sync_ctl |= (VIFBIT_HSYNC_OVERRIDE | VIFBIT_HSYNC_SELECT);
            rc = INTERFACE_SOG;
        }
        else {
            rc = INTERFACE_COMPOSITE;
        }
        /* No vsync. Select sync separator for vsync output */
        sync_ctl |= (VIFBIT_VSYNC_OVERRIDE | VIFBIT_VSYNC_SELECT);
    }

    /* We use FPGA registers to detect what's happening on the sync */
    /* inputs. The AD9888 VIF_SYNC_DETECT register is not reliable. */
    {
        int adcvpolreg = hal_get_fpga_val_k(hp, FPGA_ADCVPOLREG);
        if (adcvpolreg & 0x1) {
            sync_ctl &= ~VIFBIT_VSYNC_INVERT;
        }
        else {
            sync_ctl |= VIFBIT_VSYNC_INVERT;
        }
    }

    /* Set Hsync output polarity to logic high sync */
    sync_ctl &= ~VIFBIT_HSYNC_OUTPUT_POLARITY;

    if (first_time || (old_sync_ctl != sync_ctl)) {
        VLOG_DEBUG_CAT(VLOG_VIF,
                "Changing Channel %d VIF_SYNC_CNTL %x --> %x",
                hal_get_channel_number(hp), old_sync_ctl, sync_ctl);
        hal_set_ad9888_val_k(hp, VIF_SYNC_CNTL, sync_ctl);
        first_time = 0;
        old_sync_ctl = sync_ctl;
    }
    return rc;
}

/****************************************************
 * Wait till add side video DCM lock. Returns -1 if *
 * this takes too much time.                        *
 ****************************************************/
static int wait_till_add_pll_locked(HAL *hp) {
    unsigned short regval, asreg;
    int count = 0;

    regval = ASREG_VIDEO_CLOCK_DCM_LOCKED | ASREG_SONET_CLOCK_DCM_LOCKED;
    while (((asreg = hal_get_fpga_val_k(hp, FPGA_ASREG)) & regval) != regval) {
        hal_set_fpga_val_k(hp, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM);
        usleep(1000);
        hal_set_fpga_val_k(hp, FPGA_CDCMRREG, 0);
        usleep(1000);
        if (++count > 3) return -1;
    }
    return count;
}

/*****************************************************
 * Wait till drop side video DCM lock. Returns -1 if *
 * this takes too much time.                         *
 *****************************************************/
static int wait_till_drop_pll_locked(HAL *hp) {
    unsigned short regval;
    int count = 0;

    regval = (DSREG_VIDEO_CLOCK_DCM_LOCKED | DSREG_SONET_CLOCK_DCM_LOCKED);
    while ((hal_get_fpga_val_k(hp, FPGA_DSREG) & regval) != regval) {
        hal_set_fpga_val_k(hp, FPGA_CDCMRREG,
                CDCMRREG_DROP_RESET_VIDEO_DCM|CDCMRREG_DROP_RESET_SONET_DCM);
        usleep(1000);
        hal_set_fpga_val_k(hp, FPGA_CDCMRREG, 0);
        usleep(1000);
        if (++count > 3) return -1;
    }
    return count;
}

/******************************************
 * Returns 1 if input is DVI, 0 otherwise *
 ******************************************/
static int check_dvi_input(HAL *hp) {
    unsigned short treg = hal_get_fpga_val_k(hp, FPGA_AMDVITREG);
    if ((treg & AMTREG_HVSYNC) == AMTREG_HVSYNC)
        return 1;
    return 0;
}

/*************************************************************
 * This routine returns the measured H/V frequency etc and   *
 * type of interface with detected HV sync in function value.*
 *************************************************************/
static int get_video_registers(HAL *hp,
                               unsigned short *amlreg,
                               unsigned short *amhreg,
                               unsigned short *amvreg,
                               unsigned short *amtreg) {
    int rc;

    if (check_dvi_input(hp)) {
        *amtreg = hal_get_fpga_val_k(hp, FPGA_AMDVITREG);
        *amlreg = hal_get_fpga_val_k(hp, FPGA_AMDVILREG);
        *amhreg = hal_get_fpga_val_k(hp, FPGA_AMDVIHREG);
        *amvreg = hal_get_fpga_val_k(hp, FPGA_AMDVIVREG);
        rc = INTERFACE_DIGITAL;
        return rc;
    }

    /*****************************************************
     * Set coast & sync select on AD9888 if needed       *
     *****************************************************/
    if ((rc = v4_monitor_ad9888(hp)) == INTERFACE_HV) {
        /*****************************************************
         * H/V syncs are detected on the ADC. Set the FPGA   *
         * to use syncs from the external connector.         *
         *****************************************************/
        hal_set_ext_sync(hp, 1);
    }
    else {
        /*****************************************************
         * If no H/Vsync & green is detected on the ADC, the *
         * input is Composite or SOG. Set the FGPA to use    *
         * sync from ADC then read the FPGA video registers. *
         *****************************************************/
        hal_set_ext_sync(hp, 0);
    }
    *amtreg = hal_get_fpga_val_k(hp, FPGA_AMTREG);
    *amlreg = hal_get_fpga_val_k(hp, FPGA_AMLREG);
    *amhreg = hal_get_fpga_val_k(hp, FPGA_AMHREG);
    *amvreg = hal_get_fpga_val_k(hp, FPGA_AMVREG);

    return rc;
}

/**********************************************************************
 * This routine finds the new addside LUT for current input video.    *
 * Returns NO_LUT_CHANGE if no LUT change, NEW_MATCHING_LUT if found  *
 * matching LUT & LUT changed, NO_MATCHING_LUT if no matching LUT &   *
 * LUT changed. Returns matching LUT in struct pointed to by 'lutp'.  *
 **********************************************************************/
static int v4_configure_addside_video(HAL *hp,
                                      int last_lutid,
                                      struct LUT_struct *lutp) {
    int input, interface, dual;
    struct LUT_struct lut;
    unsigned short treg, lreg, vreg, hreg, hpol, vpol;
    static int first_time = 1;
    static int last_input;
    static unsigned short last_hpol, last_vpol;
    unsigned char cval;
    int hardware_type = hal_get_hardware_type(hp);

    dual = DUALLINK_IGNORE;
    hpol = hal_get_fpga_val_k(hp, FPGA_ADCHPOLREG);
    vpol = hal_get_fpga_val_k(hp, FPGA_ADCVPOLREG);

    if (hardware_type != XPI_DEVICE && check_dvi_input(hp)) {
        dual = (hal_get_pca9555_dvi_val_k(hp, 0x01) & 0x01) ?
            DUALLINK_YES : DUALLINK_NO;
        hpol = hal_get_fpga_val_k(hp, FPGA_DVIHPOLREG);
        vpol = hal_get_fpga_val_k(hp, FPGA_DVIVPOLREG);
        /************************************************
         * Set DVI switch to 48 bit for dual link input *
         ************************************************/
        if (dual == DUALLINK_YES) {
            cval = hal_get_pca9555_dvi_val_k(hp, 0x03);
            hal_set_pca9555_dvi_val_k(hp, 0x03, cval | 0x08);
            interface = INTERFACE_48BIT | INTERFACE_DUALLINK;
            usleep(100000);
        }
    }

    memset(&lut, 0, sizeof(struct LUT_struct));
    input = get_video_registers(hp, &lreg, &hreg, &vreg, &treg);

    interface = input &
        (INTERFACE_DIGITAL|INTERFACE_HV|INTERFACE_COMPOSITE|INTERFACE_SOG);

    /*********************************************
     * Find the LUT matching current video input *
     *********************************************/

    if (hal_configure_addside_lut(hp, lreg, hreg, vreg, treg, dual, &lut) < 0) {
        if (first_time || (last_input != input) ||  
                (last_lutid != RESERVED_INVALID_LUTID) || 
                (last_hpol != hpol) ||
                (last_vpol != vpol)) {
            hal_LED_off(hp, VIDEO_LED);	/* turn off video LED */
            VLOG_INFO_CAT(VLOG_HALD,
                "Addside Channel %d %s (%s/%s) No LUT Match.",
                hal_get_channel_number(hp),
                (input & INTERFACE_DIGITAL) ? "DVI" :
                ((input & INTERFACE_HV) ? "Analog HV" :
                ((input & INTERFACE_SOG) ? "Analog SOG" :
                ((input & INTERFACE_COMPOSITE) ? "Analog Composite" : "???"))),
                (hpol) ? "Neg" : "Pos",
                (vpol) ? "Neg" : "Pos");

            hal_setActiveIntf(hp, interface);
            last_input = input;
            last_hpol = hpol;
            last_vpol = vpol;
            first_time = 0;
            return NO_MATCHING_LUT;
        }
        else if (!first_time) {
            return NO_LUT_CHANGE;
        }
    }

    /*******************************************
     * Don't do anything if LUT hasn't changed *
     *******************************************/
    if (!first_time && (last_hpol == hpol) && (last_vpol == vpol) &&
            (last_input == input) && (lut.a_alutreg == last_lutid)) {
        return NO_LUT_CHANGE;
    }

    if (first_time)
        first_time = 0;

    last_input = input;
    last_hpol = hpol;
    last_vpol = vpol;

    hal_LED_on(hp, VIDEO_LED);	/* turn on video LED */

    VLOG_INFO_CAT(VLOG_HALD,
            "Addside Channel %d %s Input Matched LUT ID 0x%x (%s/%s).",
            hal_get_channel_number(hp),
            (input & INTERFACE_DIGITAL) ? "DVI" :
            ((input & INTERFACE_HV) ? "Analog HV" :
            ((input & INTERFACE_SOG) ? "Analog SOG" :
            ((input & INTERFACE_COMPOSITE) ? "Analog Composite" : "???"))),
            lut.a_alutreg,
            (hpol) ? "Neg" : "Pos",
            (vpol) ? "Neg" : "Pos");

    if (interface & INTERFACE_DIGITAL) {
        /**************************************************
         * Set DVI switch to 48 or 24 bit for single link *
         **************************************************/
        if (dual == DUALLINK_NO) {
            if (lut.a_pcr < PCR_48BIT_DVI) {
                cval = hal_get_pca9555_dvi_val_k(hp, 0x03);
                hal_set_pca9555_dvi_val_k(hp, 0x03, cval & 0xf7);
            }
            else {
                cval = hal_get_pca9555_dvi_val_k(hp, 0x03);
                hal_set_pca9555_dvi_val_k(hp, 0x03, cval | 0x08);
                interface |= INTERFACE_48BIT;
            }
            usleep(100000);
        }
        else if (dual == DUALLINK_YES) {
            interface |= INTERFACE_48BIT | INTERFACE_DUALLINK;
        }
        else {
            interface &= ~(INTERFACE_48BIT | INTERFACE_DUALLINK);
        }
    }
    else {
        v4_configure_ad9888(hp, &lut);
        if (lut.a_pcr >= PCR_48BIT_ANALOG)
            interface |= INTERFACE_48BIT;
    }

    memcpy(lutp, &lut, sizeof(struct LUT_struct));

    /********************************************************
     * Set FPGA to process 24 or 48 bit DVI or analog input *
     ********************************************************/
    hal_setActiveIntf(hp, interface);

    return NEW_MATCHING_LUT;
}

/******************************************************************
 * This routine executes the SiI1178 programming sequence that's  *
 * necessary after every resolution switch or video clock change. *
 * Note (1) FPGA video DCM shoud be disabled before calling this  *
 * routine as stated on SII1178 data sheet Sil-DS-0127-A p23. (2) *
 * video output is disabled by this routine. The caller needs to  *
 * enable it afterwards (w/some delay). This is to ensure the SiI,*
 * the DDS and the FPGA registers are all set to the final values *
 * when it's enabled so it's always stable (TP Bug 18761).        *
 ******************************************************************/
static void program_sii_registers(HAL *hp, double pixclk) 
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE) 
         return;

    if (pixclk < PCR_48BIT_DVI) {        /* 24 bit interface */
        unsigned char b, c;
        /* set sii1178 for DVI output */
        c = hal_get_pca9555_clk_val_k(hp, 0x07);
        hal_set_pca9555_clk_val_k(hp, 0x07, c & 0x5f);
        usleep(1000);

        b = hal_get_pca9555_clk_val_k(hp, 0x03);
        hal_set_pca9555_clk_val_k(hp, 0x03, b & 0x5f);
        usleep(1000);

        hal_set_pca9555_clk_val_k(hp, 0x03, b | 0xa0);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x08, 0x35);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x0f, 0x4c);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x0f, 0x44);
        usleep(1000);

        /* power down the master and slave */
        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);
        hal_set_sii1178_2_val_k(hp, 0x08, 0x34);
        usleep(1000);

        /* set slave in reset */
        b = hal_get_pca9555_clk_val_k(hp, 0x03);
        hal_set_pca9555_clk_val_k(hp, 0x03, b & 0xdf);
    }
    else {				                 /* 48 bit interface */
        unsigned char b, c;
        /* set sii1178 for DVI output */
        c = hal_get_pca9555_clk_val_k(hp, 0x07);
        hal_set_pca9555_clk_val_k(hp, 0x07, c & 0x5f);
        usleep(1000);

        b = hal_get_pca9555_clk_val_k(hp, 0x03);
        hal_set_pca9555_clk_val_k(hp, 0x03, b & 0x5f);
        usleep(1000);
        if (pixclk > PCR_DUALLINK_DVI)
            hal_set_pca9555_clk_val_k(hp, 0x03, b | 0xa0);
        else
            hal_set_pca9555_clk_val_k(hp, 0x03, b | 0xa0);
        //check above for redundancy -- Piyush
        //06May09
       usleep(1000);

       /* Dual link. Program the slave. */
       if (pixclk > PCR_DUALLINK_DVI) {
            hal_set_sii1178_2_val_k(hp, 0x0f, 0x44);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0f, 0x4c);
            usleep(1);
            if (pixclk >= (double) 200)
                hal_set_sii1178_2_val_k(hp, 0x0e, 0x34);
            else
                hal_set_sii1178_2_val_k(hp, 0x0e, 0x10);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0a, 0x80);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x09, 0x30);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0c, 0x89);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0d, 0x70);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x08, 0x34);
        }

        /* Program the master */
        hal_set_sii1178_1_val_k(hp, 0x0f, 0x44);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0f, 0x4c);
        usleep(1);
        if (pixclk >= (double) 200)
            hal_set_sii1178_1_val_k(hp, 0x0e, 0x34);
        else
            hal_set_sii1178_1_val_k(hp, 0x0e, 0x10);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0a, 0x80);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x09, 0x30);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0c, 0x89);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0d, 0x70);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);

        /* Check master/slave setting option */
        if ((hal_get_sii1178_1_val_k(hp, 0x0c) & 0x40) == 0) {
            VLOG_INFO_CAT(VLOG_HALD,
                    "Error: device at I2C addr 0x38 not a master.");
        }
        if (pixclk > PCR_DUALLINK_DVI) {
            if ((hal_get_sii1178_2_val_k(hp, 0x0c) & 0x40) == 1) {
                VLOG_INFO_CAT(VLOG_HALD,
                        "Error: device at I2C addr 0x39 not a slave.");
            }
        }
        usleep(1000);
    }

    /* Disable h/vsync output. Caller will enable it later. */
    hal_or_in_fpga_val_k(hp, FPGA_ATESTREG, SYNCOFF_ENABLE);
}

/********************************************************************
 * Configures the SII and the DDS on RX so the DDS output frequency *
 * differs by 'tweak' from the LUT entry 'lutp'. Returns a return   *
 * code of 1 if success, -1 if failure. Video output is disabled    *
 * when return code is 1.                                           *
 ********************************************************************/
static int v4_tweak_frequency(HAL *hp, struct LUT_struct *lutp, int tweak) {

    double vf_ddsfreq, dval;
    unsigned int vf_hi, vf_lo, i;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;

    if (lutp == NULL) {
        VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d: failed to set DDS output frequency. No LUT.",
                hal_get_channel_number(hp));
        return -1;
    }

    usleep(1000);

    /* reset dds */
    hal_or_in_fpga_val_k(hp, FPGA_PDDSREG, PDDSREG_RESET);

    /* delay 100 usec */
    for (i = 0; i < 10000; i++);

    /* complete reset */
    hal_and_in_fpga_val_k(hp, FPGA_PDDSREG, ~PDDSREG_RESET);
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_03, 0x00);

    /* provision UPD as an output clock */
    hal_or_in_fpga_val_k(hp, FPGA_PDDSREG, PDDSREG_P_UDCK);

    /* initialize dds */
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_01, 0x00);
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_02, 0x49);
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_04, 0x60);
    hal_set_ad9852_val_k(hp, DDS_OSK_MULT_01, 0x0F);
    hal_set_ad9852_val_k(hp, DDS_OSK_MULT_02, 0xFF);
    hal_set_ad9852_val_k(hp, DDS_CNTL_DAC_01, 0x2F);
    hal_set_ad9852_val_k(hp, DDS_CNTL_DAC_02, 0xFF);

    /* set DDS clock frequency values */
    hal_set_pca9555_clk_val_k(hp, 0x06, 0xe0);

    /* Compute frequency offset equivalent for 'dval'   */
    /* Note the dds values in LUT are computed for half */
    /* the pixel clock so we don't divide by 2 as was   */
    /* done in the routine tweak_dds() above.           */
    (void) hal_compute_dds_frequency(lutp, tweak, &vf_ddsfreq);
    dval = vf_ddsfreq * V4_DDSFFACTOR / TWORAISED48;

    VLOG_DEBUG_CAT(VLOG_HALD,
            "Tweaked PCR = 0x%f, Original PCR = 0x%f", dval, lutp->a_pcr);

    /* program the DDS to half the frequency (and set   */
    /* the SII to dual link) for pixel clock > 165 MHz. */
    if (lutp->a_pcr >= (double) 165)
        vf_ddsfreq /= 2;

    program_sii_registers(hp, lutp->a_pcr);

    /* set cdcf5801a clock multiplier */
    hal_set_pca9555_clk_val_k(hp, 0x06, 0xe0);
    if (dval < 39) {
        hal_set_pca9555_clk_val_k(hp, 0x02, 0x11);
    }
    else if (dval < 78) {
        hal_set_pca9555_clk_val_k(hp, 0x02, 0x0b);
    }
    else {
        hal_set_pca9555_clk_val_k(hp, 0x02, 0x18);
    }

    /* set DDS clock frequency values */
    vf_hi = vf_ddsfreq/TWORAISED24;
    vf_lo = vf_ddsfreq - ((float)vf_hi)*TWORAISED24;

    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_01,(vf_hi >> 16) & 0x7f);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_02,(vf_hi >>  8) & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_03,(vf_hi      ) & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_04,(vf_lo >> 16) & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_05,(vf_lo >>  8)  & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_06, vf_lo        & 0xff);

    hal_or_in_fpga_val_k(hp, FPGA_PDDSREG, PDDSREG_P_UDCK | PDDSREG_UDCK);
    for (i = 0; i < 10000; i++);	/* delay 100 us */
    hal_and_in_fpga_val_k(hp, FPGA_PDDSREG, ~PDDSREG_UDCK);

    usleep(1000);

    return 1;
}

/*****************************************************************
 * Returns 1 if input stereo changed otherwise returns 0         *
 *****************************************************************/
static int check_stereo_change(HAL *hp) {
    unsigned long intrbits;

    /****************************************
     * Get and acknowledge stereo interrupt *
     ****************************************/
    intrbits = hal_get_fpga_val32_k(hp, FPGA_ISREG);

    if (intrbits & ADD_STEREO_CHANGE_BIT) {
        hal_set_fpga_val32_k(hp, FPGA_ISREG, ADD_STEREO_CHANGE_BIT);
        return 1;
    }
    return 0;
}

/********************************************************************
 * This routine configures the dropside video with the display LUT  *
 * 'lutp'. Returns 1 if success, -1 if error.  Video output is      *
 * disabled by this routine when return code is 1.                  *
 ********************************************************************/
static int v4_configure_display_video(HAL *hp,
                                      struct LUT_struct *lutp,
                                      int source) {
    struct LUT_struct lut;
    int pixadjust, interface = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;

    if (hp == NULL || lutp == NULL)
        return -1;

    if ((pixadjust = hal_get_pixel_adjust_k(hp)) < 0)
        pixadjust = 0;

    memcpy(&lut, lutp, sizeof(struct LUT_struct));

    if (source == DISPLAY_SOURCE_REMOTE) {
        int genlockmode, genlockdelta, avflag;

        get_configured_rx_genlock_params(hp, &genlockmode, &genlockdelta);

        avflag = hal_getAudioVideoFlag(hp);
        if (genlockmode == GENLOCK_MODE_MASTER) {
            lut.a_vbp += genlockdelta;
            hal_setAudioVideoFlag(hp,
                    (avflag & ~AVFLAG_GENLOCKSLAVE) | AVFLAG_GENLOCKMASTER);
        }
        else if (genlockmode == GENLOCK_MODE_SLAVE) {
            hal_setAudioVideoFlag(hp,
                    (avflag & ~AVFLAG_GENLOCKMASTER) | AVFLAG_GENLOCKSLAVE);
        }
        else {
            hal_setAudioVideoFlag(hp,
                    avflag & ~(AVFLAG_GENLOCKMASTER|AVFLAG_GENLOCKSLAVE));
        }
        /*******************************************************
         * Disable LRSYNC for 2 secs so downstream slaves can  *
         * detect a stereo change and resync video when LRSYNC *
         * is re-enabled once dropside video is reset.         *
         *******************************************************/
        hal_or_in_fpga_val_k(hp, FPGA_DCREG, DCREG_LRSYNC_DISABLE);
        sleep(2);
    }

    hal_set_display_lut(hp, &lut);

    /*********************************************************************
     * Disable video & video DCMs before setting DDS & SII. They'll be   *
     * re-enabled when we run v4_reset_dropside_and_wait_till_pll_locked *
     *********************************************************************/
    hal_set_fpga_val_k(hp, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM |
                                          CDCMRREG_DROP_RESET_VIDEO_DCM);
    hal_and_in_fpga_val_k(hp, FPGA_DCREG, ~(DCREG_DROP_ENABLE |
                DCREG_VID_ENABLE | DCREG_STEREO |
                DCREG_HSYNC_POL | DCREG_VSYNC_POL));

    if (v4_tweak_frequency(hp, &lut, (double) pixadjust) < 0) {
        VLOG_WARNING_CAT(VLOG_HALD,
            "Channel %d. Failed to set DDS output frequency.",
            hal_get_channel_number(hp));
        return -1;
    }

    /***********************************************
     * Set interface flags so dropside FPGA can be *
     * configured correctly in the driver.         *
     ***********************************************/
    if ((interface = hal_getActiveIntf(hp)) < 0) {
        VLOG_WARNING_CAT(VLOG_HALD,
            "Channel %d. Failed to get active interface from driver.",
            hal_get_channel_number(hp));
        return -1;
    }
    interface &= ~(INTERFACE_DUALLINK|INTERFACE_48BIT);
    if (source == DISPLAY_SOURCE_REMOTE || (interface & INTERFACE_DIGITAL)) {
        if (lut.a_pcr >= PCR_48BIT_DVI)
            interface |= INTERFACE_48BIT;
        if (lut.a_pcr >= PCR_DUALLINK_DVI)
            interface |= INTERFACE_DUALLINK;
    }
    else {
        if (lut.a_pcr >= PCR_48BIT_ANALOG)
            interface |= INTERFACE_48BIT;
    }

    hal_setActiveIntf(hp, interface);

    return 1;
}

/***************************************************
 * Sets or clears AVFLAG_LOCALRESET flag for XP200 *
 ***************************************************/
static void v4_set_video_reset_flags(HAL *hp, int flags) {
    int avflag;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return;

    if ((avflag = hal_getAudioVideoFlag(hp))< 0)
        return;

    avflag &= ~AVFLAG_LOCALRESET;

    if (flags & AVFLAG_LOCALRESET)
        hal_setAudioVideoFlag(hp, avflag | AVFLAG_LOCALRESET);
    else
        hal_setAudioVideoFlag(hp, avflag);
}

/**********************************************
 * Resets addside video waits for PLL to lock *
 **********************************************/
static int v4_reset_addside_and_wait_till_pll_locked(HAL *hp) {

    int lockbits = ASREG_SONET_CLOCK_DCM_LOCKED|ASREG_VIDEO_CLOCK_DCM_LOCKED;

    if (hal_resetAddsideVideo(hp) < 0)
        return -1;

    if ((hal_get_fpga_val_k(hp, FPGA_ASREG) & lockbits) != lockbits) {
        if (wait_till_add_pll_locked(hp) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Addside Channel %d Video DCM unlocked",
                    hal_get_channel_number(hp));
            return -1;
        }
    }
    return 0;
}

/**************************************************
 * Resets addside video DCM waits for PLL to lock *
 **************************************************/
static int v4_reset_addside_dcm_and_wait_till_pll_locked(HAL *hp) {

    int lockbits = ASREG_SONET_CLOCK_DCM_LOCKED|ASREG_VIDEO_CLOCK_DCM_LOCKED;

    hal_set_fpga_val_k(hp, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM);
    usleep(1000);
    hal_set_fpga_val_k(hp, FPGA_CDCMRREG, 0);

    if ((hal_get_fpga_val_k(hp, FPGA_ASREG) & lockbits) != lockbits) {
        if (wait_till_add_pll_locked(hp) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Addside Channel %d Video DCM unlocked",
                    hal_get_channel_number(hp));
            return -1;
        }
    }
    return 0;
}

/***********************************************
 * Resets dropside video waits for PLL to lock *
 ***********************************************/
static int v4_reset_dropside_and_wait_till_pll_locked(HAL *hp) {

    int lockbits = DSREG_SONET_CLOCK_DCM_LOCKED|DSREG_VIDEO_CLOCK_DCM_LOCKED;

    if (hal_resetDropsideVideo(hp) < 0)
        return -1;

    if ((hal_get_fpga_val_k(hp, FPGA_DSREG) & lockbits) != lockbits) {
        if (wait_till_drop_pll_locked(hp) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Dropside Channel %d Video DCM unlocked",
                    hal_get_channel_number(hp));
            return -1;
        }
    }
    return 0;
}

/***************************************************************
 * Returns 1 if there're addside video interrupts for TX or RX *
 ***************************************************************/
static int check_video_interrupts(HAL *hp) {
    unsigned long intrbits;
    int rc, board_type = hal_get_board_type(hp);

    /***********************************************
     * Get and acknowledge video change interrupts *
     ***********************************************/
    intrbits = hal_get_fpga_val32_k(hp, FPGA_ISREG);

    intrbits &= (AMHREG_CHANGE_BIT|AMVREG_CHANGE_BIT|AMLREG_CHANGE_BIT|
        AMTREG_CHANGE_BIT|AMDVIHREG_CHANGE_BIT|AMDVIVREG_CHANGE_BIT|
        AMDVILREG_CHANGE_BIT|AMDVITREG_CHANGE_BIT|RXAMVREG_CHANGE_BIT|
        RXAMHREG_CHANGE_BIT|RXAMLREG_CHANGE_BIT|RXAMTREG_CHANGE_BIT|
        RXAMDVIHREG_CHANGE_BIT|RXAMDVIVREG_CHANGE_BIT|
        RXAMDVILREG_CHANGE_BIT| RXAMDVITREG_CHANGE_BIT);

    hal_set_fpga_val32_k(hp, FPGA_ISREG, intrbits);

    if (board_type == BOARDTYPE_TX) {
        rc = intrbits & (AMHREG_CHANGE_BIT|AMVREG_CHANGE_BIT|
                        AMLREG_CHANGE_BIT| AMTREG_CHANGE_BIT|
                        AMDVIHREG_CHANGE_BIT|AMDVIVREG_CHANGE_BIT|
                        AMDVILREG_CHANGE_BIT|AMDVITREG_CHANGE_BIT);
    }
    else {
        rc = intrbits & (RXAMVREG_CHANGE_BIT|RXAMHREG_CHANGE_BIT|
                RXAMLREG_CHANGE_BIT|RXAMTREG_CHANGE_BIT|
                RXAMDVIHREG_CHANGE_BIT|RXAMDVIVREG_CHANGE_BIT|
                RXAMDVILREG_CHANGE_BIT|RXAMDVITREG_CHANGE_BIT);
    }

    return rc;
}

/**************************************************************
 * Returns 1 if overlay source or scales changed, 0 otherwise *
 **************************************************************/
static int check_position_change(struct display_config *oldcfg,
                                 struct display_config *cfg) {
    if (oldcfg->hpos_scheme != cfg->hpos_scheme)
        return 1;
    if (oldcfg->vpos_scheme != cfg->vpos_scheme)
        return 1;
    if (cfg->hpos_scheme == POS_BY_LOCATION &&
            cfg->hlocation != oldcfg->hlocation)
        return 1;
    if (cfg->vpos_scheme == POS_BY_LOCATION &&
            cfg->vlocation != oldcfg->vlocation)
        return 1;
    if (cfg->hpos_scheme == POS_BY_OFFSET && cfg->hoffset != oldcfg->hoffset)
        return 1;
    if (cfg->vpos_scheme == POS_BY_OFFSET && cfg->voffset != oldcfg->voffset)
        return 1;
    return 0;
}

/***************************************************************
 * Routines to control access to current addside LUT in driver *
 ***************************************************************/
static int lock_addsidelut(HAL *hp) {
    int val = 1;
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_LOCK_ADDSIDELUT, &val) < 0)
        return -1;
    return 0;
}

static int unlock_addsidelut(HAL *hp) {
    int val = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_LOCK_ADDSIDELUT, &val) < 0) 
        return -1;
    return 0;
}

/****************************************************************
 * Routine to handle add side video change for XP200 TX and RX. *
 * Returns 0 if dropside video not reconfigured as a result, 1  *
 * if addside video reconfigured, 0 if not reconfigured (no     *
 * change in video), -1 if error. Video output is disabled when *
 * returne code is 1.                                           *
 ****************************************************************/
static int handle_addside_video_change(HAL *hp) {
    static int first_time = 1;
    struct LUT_struct lut;
    int lutid, lut_changed, video_interrupt;
    int rc = 0;

    /*********************************************************
     * Except for first time, do nothing if input is digital *
     * and there's no video interrupts and no LUT change     *
     *********************************************************/
    video_interrupt = check_video_interrupts(hp);
    if (first_time)
        first_time = 0;
    else if (!video_interrupt && !hal_get_addside_lut(hp, &lut) &&
                (hal_getActiveIntf(hp) & INTERFACE_DIGITAL)) {
        return 0;
    }

    /***************************************
     * Get current addside LUT from driver *
     ***************************************/
    if (hal_get_addside_lut(hp, &lut) < 0)
        lutid = RESERVED_INVALID_LUTID;
    else
        lutid = lut.a_alutreg;

    /***************************************
     * Lock addside LUT so StreamingServer *
     * can't read till we're done          *
     ***************************************/
    if (lock_addsidelut(hp) < 0) {
        VLOG_WARNING_CAT(VLOG_HALD, "Channel %d. Failed to lock addside LUT.",
            hal_get_channel_number(hp));
    }

    lut_changed = v4_configure_addside_video(hp, lutid, &lut);

    /*******************************************************
     * Re-configure TX or RX video for the new addside LUT *
     *******************************************************/
    if (hal_get_board_type(hp) == BOARDTYPE_TX) {

        if (lut_changed == NEW_MATCHING_LUT) {
            /*******************************************************
             * Reset addside DCM. It'll be taken out of reset when *
             * we run v4_reset_addside_and_wait_till_pll_locked.   *
             *******************************************************/
            hal_and_in_fpga_val_k(hp, FPGA_CDCMRREG,
                    CDCMRREG_ADD_RESET_VIDEO_DCM);

            /****************************************
             * Set DDS so TX DVI loopback will work *
             ****************************************/
            program_sii_registers(hp, lut.a_pcr);

            /*****************************
             * Re-configure addside FPGA *
             *****************************/
            (void) v4_reset_addside_and_wait_till_pll_locked(hp);
            rc = 1;

        }
        else if (lut_changed == NO_MATCHING_LUT) {
            /*********************************
             * Force EOF for StreamingServer *
             * to detect video change        *
             *********************************/
            hal_resetAddsideVideo(hp);
        }
        else if (video_interrupt) {
            VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d. video interrupt (0x%x) but no LUT change. "
                "Resetting addside DCM",
                hal_get_channel_number(hp), video_interrupt);
            v4_reset_addside_dcm_and_wait_till_pll_locked(hp);
        }
    }
    else if (lut_changed != NO_LUT_CHANGE) {
        struct ov_param_io ov;

        /*************************************
         * Get current overlay configuration *
         *************************************/
        if (hal_get_overlay_params(hp, &ov) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                "Channel %d Failed to get overlay params from driver",
                hal_get_channel_number(hp));
        }
        else if (ov.flags & (OV_FLAG_SRC_LOCAL|OV_FLAG_SRC_OVERLAY)) {
            /************************************************
             * No addside LUT. Configure the monitor with   *
             * remote LUT and just display the remote video *
             * (TODO: If local mode, display the "no input  *
             * device" splash instead of the remote video.) *
             ************************************************/
            if (lut_changed == NO_MATCHING_LUT) {
                if (ov.flags & OV_FLAG_SRC_LOCAL) {
                    /* TODO: request "no input device" splash */
                }
                if (hal_get_dropside_lut(hp, &lut) < 0) {
                    VLOG_WARNING_CAT(VLOG_HALD,
                        "Channel %d Failed to get dropside LUT from driver",
                        hal_get_channel_number(hp));
                    return -1;
                }
                VLOG_WARNING_CAT(VLOG_HALD,
                    "Channel %d Using dropside LUT ID 0x%x for display.",
                    hal_get_channel_number(hp), lut.a_alutreg);
            }

            /**********************************
             * Reconfigure overlay parameters *
             **********************************/
            reconfigure_overlay_parameters(hp);

            /*****************************
             * Reconfigure dropside FPGA *
             *****************************/
            hal_soft_reset_fpga(hp);
            rc = v4_configure_display_video(hp, &lut,
                    (ov.flags & OV_FLAG_SRC_LOCAL) ? DISPLAY_SOURCE_LOCAL :
                    DISPLAY_SOURCE_OVERLAY);
            if (rc < 0) {
                VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d Failed to configure display video",
                    hal_get_channel_number(hp));
            }
            else {
                v4_set_video_reset_flags(hp, AVFLAG_LOCALRESET);
                (void) v4_reset_dropside_and_wait_till_pll_locked(hp);
            }
        }
    }

    /**********************
     * Unlock addside LUT *
     **********************/
    if (unlock_addsidelut(hp) < 0) {
        VLOG_WARNING_CAT(VLOG_HALD, "Channel %d. Failed to unlock addside LUT.",
            hal_get_channel_number(hp));
    }
    return rc;
}

/***************************************************************
 * Configure remote mode dropside video to LUT 'lutp'. Returns *
 * 1 if video is reconfigured, -1 if error. Video output is    *
 * disabled when return code is 1.                             *
 ***************************************************************/
static int v4_configure_remote_mode_video(HAL *hp,
                                          struct LUT_struct *lutp,
                                          struct override_config *ocfg,
                                          int hscale,
                                          int vscale) {
    if (ocfg->ov_option == OV_OPTION_NONE) {
        /* Not overriding. Set overlay to what user has configured */
        reconfigure_overlay_parameters(hp);
    }
    else {
        struct ov_param_io ovpio;

        /* Change overlay settings in driver for override */
        memset(&ovpio, 0, sizeof(struct ov_param_io));

        ovpio.flags = OV_FLAG_SRC_REMOTE;

        if (ocfg->hscale_scheme == SCALE_BY_DISPLAY)
            ovpio.flags |= OV_FLAG_HSCALEDISPLAY;
        if (ocfg->hscale_scheme == SCALE_BY_SOURCE)
            ovpio.flags |= OV_FLAG_HSCALESRC;
        else
            ovpio.flags |= OV_FLAG_VRES;
        if (ocfg->vscale_scheme == SCALE_BY_DISPLAY)
            ovpio.flags |= OV_FLAG_VSCALEDISPLAY;
        if (ocfg->vscale_scheme == SCALE_BY_SOURCE)
            ovpio.flags |= OV_FLAG_VSCALESRC;
        else
            ovpio.flags |= OV_FLAG_VRES;

        if (ocfg->hpos_scheme == OV_POSITION_LOCATION)
            ovpio.flags |= OV_FLAG_HPOSLOCATION;
        else
            ovpio.flags |= OV_FLAG_HPOSOFFSET;
        if (ocfg->vpos_scheme == OV_POSITION_LOCATION)
            ovpio.flags |= OV_FLAG_VPOSLOCATION;
        else
            ovpio.flags |= OV_FLAG_VPOSOFFSET;

        if (ocfg->ov_keepaspect)
            ovpio.flags |= OV_FLAG_KEEPASPECT;

        ovpio.hscale = (ocfg->hscale * RX_MAX_SCALE_VAL) / 100;
        ovpio.vscale = (ocfg->vscale * RX_MAX_SCALE_VAL) / 100;
        if (ovpio.hscale < RX_MIN_SCALE_VAL)
            ovpio.hscale = RX_MIN_SCALE_VAL;
        if (ovpio.vscale < RX_MIN_SCALE_VAL)
            ovpio.vscale = RX_MIN_SCALE_VAL;
        ovpio.hres = ocfg->hres;
        ovpio.vres = ocfg->vres;
        ovpio.hlocation = ocfg->hlocation;
        ovpio.vlocation = ocfg->vlocation;
        ovpio.hoffset = ocfg->hoffset;
        ovpio.voffset = ocfg->voffset;

        ovpio.setmask = OV_SET_PARAM_OVERLAY;

        if (hal_set_overlay_params(hp, &ovpio) < 0)
            return -1;

        lutp->d_ov_control = DOVCONTROL_ENABLE | ocfg->ov_color;
    }

    /*******************************
     * Re-configure dropside video *
     *******************************/
    hal_soft_reset_fpga(hp);

    if (v4_configure_display_video(hp, lutp, DISPLAY_SOURCE_REMOTE) < 0) {
        VLOG_INFO_CAT(VLOG_HALD,
            "Channel %d. Failed to configure display video for remote video.",
            hal_get_channel_number(hp));
        return -1;
    }

    v4_set_video_reset_flags(hp, AVFLAG_LOCALRESET);
    v4_reset_dropside_and_wait_till_pll_locked(hp);

    return 1;
}

/*****************************************************************
 * Routine to handle drop side video change on RX. Returns 0 if  *
 * no video change,  1 if dropside reconfigured, -1 if error.    *
 * Video output is disabled when return code is 1.               *
 *****************************************************************/
static int handle_dropside_video_change(HAL *hp, int source) {
    static int first_time = 1, old_motion444;
    static int old_genlockmode = -1, old_genlockdelta = -1;
    static struct LUT_struct oldlut, oldolut;
    struct LUT_struct lut, olut;
    struct override_config ocfg;
    int motion444, stereo_changed, rc = 0;

    /************************************************
     * Get current dropside LUT and override config *
     ************************************************/
    if (hal_get_dropside_lut(hp, &lut) < 0) {
        VLOG_DEBUG_CAT(VLOG_HALD,
                "Channel %d Failed to get dropside LUT from driver",
            hal_get_channel_number(hp));
        memset(&oldlut, 0, sizeof(struct LUT_struct));
        return -1;
    }

    xp_get_override_config(hp, &ocfg);

    motion444 = (hal_getAudioVideoFlag(hp) & AVFLAG_MOTION444) ? 1 : 0;

    /*************************************
     * Configure remote video to new LUT *
     *************************************/
    if (source == DISPLAY_SOURCE_REMOTE) {
        int hs, vs, cno = hal_get_channel_number(hp);
        int genlockmode, genlockdelta;

        get_configured_rx_genlock_params(hp, &genlockmode, &genlockdelta);
        stereo_changed =
            (genlockmode == GENLOCK_MODE_SLAVE) ? check_stereo_change(hp) : 0;

        memset(&olut, 0, sizeof(struct LUT_struct));
        if (ocfg.ov_option == OV_OPTION_NONE) {
            memcpy(&olut, &lut, sizeof(struct LUT_struct));
        }
        else if (get_override_lut_and_scale(hp, &lut, &olut, &ocfg, 
                    &hs, &vs) < 0) {
            if (hal_check_display_lut(hp) < 0) {
                ocfg.ov_option = OV_OPTION_NONE;
                memcpy(&olut, &lut, sizeof(struct LUT_struct));
            }
            else {
                VLOG_INFO_CAT(VLOG_HALD,
                        "Channel %d. Failed to get override LUT.",
                        hal_get_channel_number(hp));
                return -1;
            }
        }
        if (!first_time && !stereo_changed &&
                (old_genlockmode == genlockmode) &&
                (old_genlockdelta == genlockdelta) &&
                (old_motion444 == motion444) &&
                !compare_luts(&oldolut, &olut) &&
                !compare_luts(&oldlut, &lut) &&
                oldlut.a_alutreg == lut.a_alutreg && !CodecChanged) {
            return 0;
        }

        old_genlockmode = genlockmode;
        old_genlockdelta = genlockdelta;
        memcpy(&oldolut, &olut, sizeof(struct LUT_struct));

        if (ocfg.ov_option != OV_OPTION_NONE)
            VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d. Overriding %s with %s. Scale %d/%d. Sync %s/%s.",
                cno, lut.comment, olut.comment, hs, vs,
                (olut.d_dcreg & DCREG_HSYNC_POL) ? "Neg" : "Pos",
                (olut.d_dcreg & DCREG_VSYNC_POL) ? "Neg" : "Pos");

        rc = v4_configure_remote_mode_video(hp, &olut, &ocfg, hs, vs);
    }
    else if (!first_time && (old_motion444 == motion444) &&
            !compare_luts(&oldlut, &lut) &&
            oldlut.a_alutreg == lut.a_alutreg && !CodecChanged) {
        rc = 0;
    }
    else if (source == DISPLAY_SOURCE_LOCAL) {
        v4_set_video_reset_flags(hp, 0);
        v4_reset_dropside_and_wait_till_pll_locked(hp);
        rc = 0;
    }
    else {
        struct LUT_struct alut;

        /* Set overlay for new remote LUT */
        reconfigure_overlay_parameters(hp);
        hal_soft_reset_fpga(hp);

        if (hal_get_addside_lut(hp, &alut) < 0) {
            VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d No addside LUT. Using dropside LUT for display.",
                hal_get_channel_number(hp));
            rc = v4_configure_display_video(hp, &lut, source);
            if (rc < 0) {
                VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d Failed to configure display video",
                    hal_get_channel_number(hp));
                return -1;
            }
            v4_set_video_reset_flags(hp, AVFLAG_LOCALRESET);
        }
        else {
            v4_set_video_reset_flags(hp, 0);
        }
        v4_reset_dropside_and_wait_till_pll_locked(hp);
    }

    first_time = 0;
    old_motion444 = motion444;
    memcpy(&oldlut, &lut, sizeof(struct LUT_struct));

    return rc;
}

/*************************************************************
 * Routine to handle drop side overlay, override or lrsync   *
 * change. Returns 1 if display is reconfigured, -1 if error.*
 * Video output disabled if return code is 1.                *
 *************************************************************/
static int handle_dropside_display_or_lrsync_change(HAL *hp,
                                                    int reset,
                                                    int lrsync,
                                                    int source) {
    struct LUT_struct lut;
    int rc = 0;

    hal_soft_reset_fpga(hp);

    if (!reset) {
        v4_set_video_reset_flags(hp, 0);
        v4_reset_dropside_and_wait_till_pll_locked(hp);
    }
    else if (source == DISPLAY_SOURCE_REMOTE) {
        struct override_config ocfg;
        struct LUT_struct olut;
        int hs, vs;

        if (hal_get_dropside_lut(hp, &lut) < 0) {
            VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d Failed to get dropside LUT from driver",
                hal_get_channel_number(hp));
            return -1;
        }

        xp_get_override_config(hp, &ocfg);
        memset(&olut, 0, sizeof(struct LUT_struct));

        if (ocfg.ov_option == OV_OPTION_NONE) {
            memcpy(&olut, &lut, sizeof(struct LUT_struct));
        }
        else if (get_override_lut_and_scale(hp, &lut, &olut, &ocfg, 
                    &hs, &vs) < 0) {
            if (hal_check_display_lut(hp) < 0) {
                ocfg.ov_option = OV_OPTION_NONE;
                memcpy(&olut, &lut, sizeof(struct LUT_struct));
            }
            else {
                VLOG_INFO_CAT(VLOG_HALD,
                        "Channel %d. Failed to get override LUT.",
                        hal_get_channel_number(hp));
                return -1;
            }
        }
        VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d. Overriding %s with %s. Scale %d/%d, LRsync %s",
                hal_get_channel_number(hp), lut.comment, olut.comment,
                hs, vs, (lrsync) ? "Rev" : "Norm");

        if (lrsync)
            hal_or_in_fpga_val_k(hp, FPGA_DCREG, DCREG_LRSYNC_POL);
        else
            hal_and_in_fpga_val_k(hp, FPGA_DCREG, ~DCREG_LRSYNC_POL);

        rc = v4_configure_remote_mode_video(hp, &olut, &ocfg, hs, vs);
        if (rc < 0) {
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d Failed to configure remote mode video",
                    hal_get_channel_number(hp));
        }
    }
    else {
        if (hal_get_addside_lut(hp, &lut) < 0) {
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d Failed to get addside LUT from driver",
                    hal_get_channel_number(hp));
            if (hal_get_dropside_lut(hp, &lut) < 0) {
                VLOG_INFO_CAT(VLOG_HALD,
                        "Channel %d Failed to get dropside LUT from driver",
                        hal_get_channel_number(hp));
                return -1;
            }
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d Using dropside LUT ID 0x%x for display",
                    hal_get_channel_number(hp), lut.a_alutreg);
        }
        rc = v4_configure_display_video(hp, &lut, source);
        if (rc < 0) {
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d Failed to configure display video",
                    hal_get_channel_number(hp));
            return -1;
        }
        v4_set_video_reset_flags(hp, AVFLAG_LOCALRESET);
        v4_reset_dropside_and_wait_till_pll_locked(hp);
    }
    return rc;
}

/**********************************************
 * Get connection status from StreamingServer *
 **********************************************/
static int get_connection_status(HAL *hp) {
    struct strmsrv_tx_connection_params tx_params;
    struct strmsrv_rx_connection_params rx_params;
    int rc = -1, cno = hal_get_channel_number(hp);
    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        if (strmsrv_get_tx_connection_params(cno, &tx_params) != -1) {
            rc = tx_params.connected;
        }
    }
    else {
        if (strmsrv_get_rx_connection_params(cno, &rx_params) != -1) {
            rc = !rx_params.client_unavailable;
        }
    }
    return rc;
}

/********************************************************
 * Returns icon attributes for keyboard mode 'mode' in  *
 * struct pointed to by 'icon'                          *
 ********************************************************/
static void get_kb_icon_attribute(HAL *hp, KB_ICON *icon, int mode) {
    struct kbmode_config kbmode_cfg;

    if (hp == NULL || icon == NULL)
        return;

    if (mode == KBM_MODE_LOCAL)
        get_rx_kblocal_settings(hp, &kbmode_cfg);
    else if (mode == KBM_MODE_REMOTE)
        get_rx_kbremote_settings(hp, &kbmode_cfg);
    else
        get_kbcons_settings(hp, &kbmode_cfg);
    icon->color = kbmode_cfg.icon_color;
    icon->position = kbmode_cfg.icon_position;
    icon->duration = kbmode_cfg.icon_duration;
}

/*************************************************************
 * Re-displays keyboard icon 'icon' for keyboard mode 'mode' *
 *************************************************************/
static void update_keyboard_icon(HAL *hp, KB_ICON *icon, int mode) {
    int color, x, y;
    struct ov_param_io oi;
    struct LUT_struct lut;

    if (hp == NULL || icon == NULL)
        return;

    /* Don't show icon when duration has expired */
    if (icon->duration <= 0) {
        hal_set_icon_off(hp); 
        return; 
    }

    /* Re-initialize keyboard icon */
    hal_init_keyboard_icon(hp, 0, icon->color);
    color = icon->color;
    if (mode == KBM_MODE_REMOTE) {
        if (hal_get_overlay_info(hp, &oi) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Channel %d. Failed to get overlay info",
                    hal_get_channel_number(hp));
            return;
        }
        if (hal_get_dropside_lut(hp, &lut) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Channel %d. Failed to get remote LUT",
                    hal_get_channel_number(hp));
            return;
        }
        if (icon->position ==
                RXKEYBOARD_REMOTE_MODE_SETTINGS_ICON_POSITION_TOPRIGHT ||
                icon->position ==
                RXKEYBOARD_REMOTE_MODE_SETTINGS_ICON_POSITION_BOTTOMRIGHT)
            x = oi.hoffset + (lut.a_hres * oi.hscale) / RX_MAX_SCALE_VAL -
                DEFAULT_KEYBOARD_ICON_ENDX - DEFAULT_KEYBOARD_ICON_SIZE;
        else
            x = oi.hoffset + DEFAULT_KEYBOARD_ICON_STARTX;
        if (icon->position ==
                RXKEYBOARD_REMOTE_MODE_SETTINGS_ICON_POSITION_BOTTOMLEFT ||
                icon->position ==
                RXKEYBOARD_REMOTE_MODE_SETTINGS_ICON_POSITION_BOTTOMRIGHT)
            y = oi.voffset + (lut.a_vres * oi.vscale) / RX_MAX_SCALE_VAL -
                DEFAULT_KEYBOARD_ICON_ENDY - DEFAULT_KEYBOARD_ICON_SIZE;
        else
            y = oi.voffset + DEFAULT_KEYBOARD_ICON_STARTY;
    }
    else if (mode == KBM_MODE_CONSOLE) {
        if (hal_get_display_lut(hp, &lut) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Channel %d. Failed to get display LUT",
                    hal_get_channel_number(hp));
            return;
        }
        if (icon->position ==
                KEYBOARD_CONSOLE_MODE_SETTINGS_ICON_POSITION_TOPRIGHT ||
                icon->position ==
                KEYBOARD_CONSOLE_MODE_SETTINGS_ICON_POSITION_BOTTOMRIGHT)
            x = lut.a_hres - DEFAULT_KEYBOARD_ICON_ENDX -
                DEFAULT_KEYBOARD_ICON_SIZE;
        else
            x = DEFAULT_KEYBOARD_ICON_STARTX;
        if (icon->position ==
                KEYBOARD_CONSOLE_MODE_SETTINGS_ICON_POSITION_BOTTOMLEFT ||
                icon->position ==
                KEYBOARD_CONSOLE_MODE_SETTINGS_ICON_POSITION_BOTTOMRIGHT)
            y = lut.a_vres - DEFAULT_KEYBOARD_ICON_ENDY -
                DEFAULT_KEYBOARD_ICON_SIZE;
        else
            y = DEFAULT_KEYBOARD_ICON_STARTY;
    }
    else {
        if (hal_get_display_lut(hp, &lut) < 0) {
            VLOG_WARNING_CAT(VLOG_HALD,
                    "Channel %d. Failed to get display LUT",
                    hal_get_channel_number(hp));
            return;
        }
        if (icon->position ==
                RXKEYBOARD_LOCAL_MODE_SETTINGS_ICON_POSITION_TOPRIGHT ||
                icon->position ==
                RXKEYBOARD_LOCAL_MODE_SETTINGS_ICON_POSITION_BOTTOMRIGHT)
            x = lut.a_hres - DEFAULT_KEYBOARD_ICON_ENDX -
                DEFAULT_KEYBOARD_ICON_SIZE;
        else
            x = DEFAULT_KEYBOARD_ICON_STARTX;
        if (icon->position ==
                RXKEYBOARD_LOCAL_MODE_SETTINGS_ICON_POSITION_BOTTOMLEFT ||
                icon->position ==
                RXKEYBOARD_LOCAL_MODE_SETTINGS_ICON_POSITION_BOTTOMRIGHT)
            y = lut.a_vres - DEFAULT_KEYBOARD_ICON_ENDY -
                DEFAULT_KEYBOARD_ICON_SIZE;
        else
            y = DEFAULT_KEYBOARD_ICON_STARTY;
    }
    hal_set_icon_on(hp, color, x, y, DEFAULT_KEYBOARD_ICON_SIZE,
            DEFAULT_KEYBOARD_ICON_SIZE);
}

/********************************************************
 * Set sync polarities on RX  what's being configured   *
 ********************************************************/
static void apply_configured_rx_sync_polarities(HAL *hp) {
    int dcreg, lrsync, hsync, vsync;

    get_configured_rx_sync_polarities(hp, &lrsync, &hsync, &vsync);

    dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
    dcreg &= ~(DCREG_LRSYNC_POL|DCREG_HSYNC_POL|DCREG_VSYNC_POL);
    if (lrsync)
        dcreg |= DCREG_LRSYNC_POL;
    if (hsync)
        dcreg |= DCREG_HSYNC_POL;
    if (vsync)
        dcreg |= DCREG_VSYNC_POL;
    hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
}

/**********************************************************************
 * Set local & remote chromakey parameters to what's being configured *
 * so the color registers have the value for the pixels configured.   *
 **********************************************************************/
static void apply_configured_chromakey_parameters(HAL *hp) {
    struct chromakey_config ckcfg;

    get_chromakey_config(hp, &ckcfg);
    set_chromakey_config(hp, &ckcfg);
}

/********************************************************
 * Set AV buffer size limits to what's being configured *
 ********************************************************/
static void apply_configured_queue_limits(HAL *hp) {
    int readvideolimit, readaudiolimit, writevideolimit, writeaudiolimit;
    struct fc_queue_limits_io fcqlimitio;

    if (hal_get_fc_queue_limits(hp, &fcqlimitio) == -1) {
        VLOG_INFO_CAT(VLOG_HALD,
                "Channel %d. Failed to get AV queue limits",
                hal_get_channel_number(hp));
        return;
    }

    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        get_configured_tx_queue_limits(hp,
                                       &readvideolimit,
                                       &readaudiolimit,
                                       &writeaudiolimit);
        fcqlimitio.video_read_queue_limit = readvideolimit;
        fcqlimitio.audio_read_queue_limit = readaudiolimit;
        fcqlimitio.audio_write_queue_limit = writeaudiolimit;
        if (hal_set_fc_queue_limits(hp, &fcqlimitio) == -1) {
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d. Failed to set AV read queue limits",
                    hal_get_channel_number(hp));
        }
    }
    else {
        get_configured_rx_queue_limits(hp,
                                       &readaudiolimit,
                                       &writevideolimit,
                                       &writeaudiolimit);
        fcqlimitio.audio_read_queue_limit = readaudiolimit;
        fcqlimitio.video_write_queue_limit = writevideolimit;
        fcqlimitio.audio_write_queue_limit = writeaudiolimit;
        if (hal_set_fc_queue_limits(hp, &fcqlimitio) == -1) {
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d. Failed to set AV write queue limits",
                    hal_get_channel_number(hp));
        }
    }
    return;
}

static int check_reload(HAL *hp)
{
    int retval = -1;
    int codecversion;
    CodecChanged = 0;
    if ((codecversion = hal_get_configured_rx_codec_version(hp)) > 0) {
      if (codecversion != hal_get_codec_version(hp)) {
        VLOG_INFO("%s: Change in codec version detected : %d ! = %d", 
                __func__, codecversion, hal_get_codec_version(hp));
        return codecversion;
      }
    }

    return retval;
}

static int reload_fpga(HAL *hp, int codecversion)
{
    char buf[128];
    int retval = 0;


    sprintf(buf, "/V2O/bin/loadfpga -V %d -b %d -t %d", codecversion,
            hal_get_board_number(hp),
            hal_get_board_type(hp) == BOARDTYPE_RX ? HARDWARE_V2D_RX :
    HARDWARE_V2D_TX);
    VLOG_INFO("%s: Executing command \"%s\"", __func__, buf);
    system_command(buf);

    return retval;
}

static void monitor_connection_LED(HAL *hp, int *connled)
{
    if (get_connection_status(hp) == 1) {
        if (*connled == 0) {
            hal_LED_on(hp, CONNECTION_LED);
            *connled = 1;
        }
    }
    else {
        if (*connled) {
            hal_LED_off(hp, CONNECTION_LED);
            *connled = 0;
        }
    }
}

static void run_xp_tx_loop(HAL *hp, int timeout, int fanconfig)
{
    int iframe_interval, dvimode_auto, optimize_latency;
    int avflag, connled, video_output_delay=0;
    int is_xp100 = (get_hardware_profile() == HARDWARE_PROFILE_XP100) ? 1 : 0;

    /************************************
     * Initialize connection LED status *
     ************************************/ 
    if ((avflag = hal_getAudioVideoFlag(hp)) < 0) {
        VLOG_WARNING_CAT(VLOG_HALD, 
                "Channel %d Can't get AV flag", hal_get_channel_number(hp));
        return;
    }
    connled = (avflag & AVFLAG_CONNECTIONLED) ? 1 : 0;

    /*********************************************
     * Initialize A/V queue and video parameters *
     *********************************************/ 
    apply_configured_queue_limits(hp);
    iframe_interval = get_configured_iframe_interval(hp);
    hal_set_iframe_interval(hp, iframe_interval);
    dvimode_auto = get_configured_dvimode_auto(hp);
    hal_set_dvimode_auto(hp, dvimode_auto);
    optimize_latency = get_configured_optimize_latency(hp);
    hal_set_optimize_latency(hp, optimize_latency);
    apply_configured_rx_sync_polarities(hp);

    while (1) {
        int curr_iframe_interval, curr_dvimode_auto;
        int curr_optimize_latency;
        /*************************************************
         * Process addside video change (we delay couple *
         * secs before turning on output video on input  *
         * change to make sure the output has stablized).*
         *************************************************/ 
        if (handle_addside_video_change(hp) == 1) { 
            video_output_delay = 0;
        }
        if (video_output_delay >= 0) {
            if (++video_output_delay > 2) {
                hal_set_video_output(hp, get_configured_tx_video_output(hp)); 
                video_output_delay = -1; 
            }
        }
        /******************************************
         * Set video parameters in FPGA if needed *
         ******************************************/ 
        curr_iframe_interval = get_configured_iframe_interval(hp);
        curr_dvimode_auto = get_configured_dvimode_auto(hp);
        curr_optimize_latency = get_configured_optimize_latency(hp);
        if (iframe_interval != curr_iframe_interval) {
            iframe_interval = curr_iframe_interval;
            hal_set_iframe_interval(hp, iframe_interval);
        }
        if (dvimode_auto != curr_dvimode_auto) {
            dvimode_auto = curr_dvimode_auto;
            hal_set_dvimode_auto(hp, dvimode_auto);
            hal_set_video_output(hp, 0);
            video_output_delay = 1;
        }
        if (optimize_latency != curr_optimize_latency) {
            optimize_latency = curr_optimize_latency;
            hal_set_optimize_latency(hp, optimize_latency);
        }

        /******************************************
         * Turn on or off connction LED if needed *
         ******************************************/ 
        monitor_connection_LED(hp, &connled);

        /********************************
         * Process keyboard mode change *
         ********************************/ 
        hal_update_virtual_console(hp);

        /***************************
         * Check board temperature *
         ***************************/ 
        if (is_xp100 && fanconfig == FAN_SPEED_AUTO)
            monitor_board_temperature(hp, timeout);

        usleep (timeout);
    }
}

static void run_xp_rx_loop(HAL *hp, int timeout, int fanconfig)
{
    struct display_config oldcfg, cfg;
    struct override_config overridecfg, oldoverridecfg;
    int kbmode, curr_kbmode, curr_kb_active;
    int first_time = 1, overlay_changed, override_changed, position_changed;
    KB_ICON local_icon, remote_icon, console_icon, icon, curr_icon;
    int hsync, vsync, lrsync, lrsync_changed;
    int publish_status=1, video_output_delay = 0, chromakey_delay=-1;
    char displayconfig[255];
    int cno = hal_get_channel_number(hp);
    int is_xp100 = (get_hardware_profile() == HARDWARE_PROFILE_XP100) ? 1 : 0;

    /********************************
     * Initialize overlay in driver *
     ********************************/ 
    get_display_config(hp, &oldcfg);
    update_overlay_parameters(hp, &oldcfg);

    /*************************************
     * Initialize override configuration *
     *************************************/ 
    xp_get_override_config(hp, &oldoverridecfg);

    /********************************************
     * Initialize A/V queue and sync parameters *
     ********************************************/ 
    apply_configured_queue_limits(hp);
    apply_configured_rx_sync_polarities(hp);

    /***************************************
     * Initialize keyboard icon attributes *
     ***************************************/ 
    curr_kb_active = hp->console_info.kb_active;
    get_kb_icon_attribute(hp, &local_icon, KBM_MODE_LOCAL);
    get_kb_icon_attribute(hp, &remote_icon, KBM_MODE_REMOTE);
    get_kb_icon_attribute(hp, &console_icon, KBM_MODE_CONSOLE);
    curr_kbmode = hal_get_current_kbmode(hp);
    if (curr_kbmode == KBM_MODE_LOCAL) {
        memcpy(&curr_icon, &local_icon, sizeof(KB_ICON));
        update_keyboard_icon(hp, &curr_icon, curr_kbmode); 
    }
    else if (curr_kbmode == KBM_MODE_REMOTE) {
        memcpy(&curr_icon, &remote_icon, sizeof(KB_ICON));
        update_keyboard_icon(hp, &curr_icon, curr_kbmode); 
    }
    else {
        memcpy(&curr_icon, &console_icon, sizeof(KB_ICON));
        update_keyboard_icon(hp, &curr_icon, curr_kbmode); 
    }

    apply_configured_queue_limits(hp);

    while (1) {
        /****************************************
         * Process display configuration change *
         ****************************************/ 
        sprintf (displayconfig, "/tmp/displayconfig_%d", cno - 1); 
        get_display_config(hp, &cfg);
        overlay_changed = 
            check_source_scale_and_cropping_change(&oldcfg, &cfg); 
        // If the config has been changed
        if (!unlink (displayconfig)) {
            publish_status=1;
        }
        if (overlay_changed) {
            update_overlay_parameters(hp, &cfg);
            publish_status=1;
        }
        if (oldcfg.transparency != cfg.transparency) {
            oldcfg.transparency = cfg.transparency;
            publish_status=1;
        }
        position_changed = check_position_change(&oldcfg, &cfg); 
        if (position_changed) {
            update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            publish_status=1;
        }
        override_changed = 0;
        xp_get_override_config(hp, &overridecfg);
        if (memcmp(&oldoverridecfg, &overridecfg, 
			sizeof(struct override_config)) != 0) {
            memcpy(&oldoverridecfg, &overridecfg, 
			sizeof(struct override_config));
            if (cfg.source == DISPLAY_SOURCE_REMOTE)
                override_changed = 1;
        }
        get_configured_rx_sync_polarities(hp, &lrsync, &hsync, &vsync);
        if ((!lrsync && 
            (hal_get_fpga_val_k(hp, FPGA_DCREG) & DCREG_LRSYNC_POL)) ||
            (lrsync && 
            !(hal_get_fpga_val_k(hp, FPGA_DCREG) & DCREG_LRSYNC_POL)))
            lrsync_changed = 1;
         else
            lrsync_changed = 0;

        if (first_time || overlay_changed || override_changed ||
                lrsync_changed) {
            int reset = 0;
            if ((oldcfg.source == DISPLAY_SOURCE_REMOTE) || 
                    (cfg.source == DISPLAY_SOURCE_REMOTE)) 
                reset = 1;
            if (handle_dropside_display_or_lrsync_change(hp, reset, 
                        lrsync, cfg.source) == 1) {
                video_output_delay = 0;
            }
            if (cfg.source != DISPLAY_SOURCE_LOCAL) {
                strmsrv_rx_fullscreen_refresh(cno);
            }
            update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            memcpy(&oldcfg, &cfg, sizeof(struct display_config));
        }
        else if (position_changed) {
            memcpy(&oldcfg, &cfg, sizeof(struct display_config));
        }

        /********************************
         * Process addside video change *
         ********************************/ 
        if (handle_addside_video_change(hp) == 1) {
            strmsrv_rx_fullscreen_refresh(cno);
            if (cfg.source != DISPLAY_SOURCE_REMOTE) {
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
            publish_status=1;
            video_output_delay = 0;
        }

        /*********************************
         * Process dropside video change * 
         *********************************/ 
        if (handle_dropside_video_change(hp, cfg.source) == 1) {
            publish_status=1;
            video_output_delay = 0;
        }

        if (video_output_delay >= 0) {
            if (++video_output_delay > 2) {
                hal_set_video_output(hp, 
                        get_configured_rx_video_output(hp));
                strmsrv_rx_fullscreen_refresh(cno);
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
                video_output_delay = -1; 
            }
        }

        /***************************************************
         * Init chromakey configuration (this must be done *
         * after RX video has been configured otherwise we *
         * we won't be able to read correct pixel colors). *
         ***************************************************/
        if (first_time || chromakey_delay >= 0) {
            if (first_time) {
                first_time = chromakey_delay = 0;
            }
            if (++chromakey_delay >= 15) {
                VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d Applying chromakey configuration", cno);
                apply_configured_chromakey_parameters(hp);
                chromakey_delay = -1;
            }
        }

        /********************************************
         * Process keyboard mode and/or icon change *
         ********************************************/ 
        hal_update_virtual_console(hp);
        if (curr_kb_active != hp->console_info.kb_active) {
            curr_kb_active = hp->console_info.kb_active;
            update_keyboard_icon(hp, &curr_icon, curr_kbmode);
        }
        get_kb_icon_attribute(hp, &icon, KBM_MODE_LOCAL);
        if (memcmp(&icon, &local_icon, sizeof(KB_ICON))) {
            memcpy(&local_icon, &icon, sizeof(KB_ICON));
            if (curr_kbmode == KBM_MODE_LOCAL) {
                curr_icon.color = icon.color;
                curr_icon.position = icon.duration;
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
        }
        get_kb_icon_attribute(hp, &icon, KBM_MODE_REMOTE);
        if (memcmp(&icon, &remote_icon, sizeof(KB_ICON))) {
            memcpy(&remote_icon, &icon, sizeof(KB_ICON));
            if (curr_kbmode == KBM_MODE_REMOTE) {
                curr_icon.color = icon.color;
                curr_icon.position = icon.duration;
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
        }
        get_kb_icon_attribute(hp, &icon, KBM_MODE_CONSOLE);
        if (memcmp(&icon, &console_icon, sizeof(KB_ICON))) {
            memcpy(&console_icon, &icon, sizeof(KB_ICON));
            if (curr_kbmode == KBM_MODE_CONSOLE) {
                curr_icon.color = icon.color;
                curr_icon.position = icon.duration;
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
        }
        if ((kbmode = hal_get_current_kbmode(hp)) != curr_kbmode) {
            if (kbmode == KBM_MODE_LOCAL) {
                memcpy(&curr_icon, &local_icon, sizeof(KB_ICON));
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
            else if (kbmode == KBM_MODE_REMOTE) {
                memcpy(&curr_icon, &remote_icon, sizeof(KB_ICON));
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
            else {
                memcpy(&curr_icon, &console_icon, sizeof(KB_ICON));
                update_keyboard_icon(hp, &curr_icon, curr_kbmode);
            }
            curr_kbmode = kbmode;
        }
        else if (curr_icon.duration > 0) {
            if (--curr_icon.duration <= 0) 
                hal_set_icon_off(hp);
        }

        /************************
         * Publish board status *
         ************************/ 
        if (publish_status) {
            publish_board_status(cno);
            publish_status=0;
        }

        /***************************
         * Check board temperature *
         ***************************/ 
        if (is_xp100 && fanconfig == FAN_SPEED_AUTO)
            monitor_board_temperature(hp, timeout);

        usleep (timeout);
    }
}

#else

/* revert to default button timer GPIO */
#define REVERT_TO_DEFAULT_GPIO          59

/* revert to default button timer max (in secs) */
#define REVERT_TO_DEFAULT_TIMER_MAX     7

/* revert to default command */
#define REVERT_TO_DEFAULT_COMMAND       \
    "/etc/scripts/revert_to_defaults.sh reset-network delete-media"

/* max retries to re-read monitor EDID on failure */
#define MAX_MON_RETRIES                 5

/* video in LED status bits */
#define VIDIN_SIGNAL_DETECT             0x01
#define VIDIN_LUT_DETECT                0x02
#define VIDIN_STREAMING                 0x04

/* video out LED status bits */
#define VIDOUT_MON_DETECT               0x01
#define VIDOUT_MON_GOT_EDID             0x02
#define VIDOUT_EDID_LUT_MATCH           0x04
#define VIDOUT_DISPLAYING               0x08
#define VIDOUT_MON_ERROR                0x80

/* encoder/decoder LED status bits */
#define CODEC_IS_ENCODER                0x01
#define CODEC_IS_DECODER                0x02
#define CODEC_STREAMING                 0x04

struct status_st {
    int codec_status;
    int video_status;
    int monitor_status;
    int link_index;
    int link0_speed;
    int link1_speed;
};

static inline int revert_to_default_pressed(void)
{
    int val;
    xpi_get_gpio_pin_value(REVERT_TO_DEFAULT_GPIO, &val);
    return (val) ? 0 : 1;
}

static inline int check_monitor_detect(HAL *hp)
{
    unsigned char val;

    if (!xpi_get_main_i2c_val(hp, SII7172_I2C_PORT, SII7172_I2C_ADDR, 
                SII7172_TX_STAT, &val)) {
        return (val & SII7172_TX_STAT_RSEN) ? 1 : 0;
    }
    return -1;
}

static inline int get_compmode(HAL *hp)
{
    return hal_getActiveIntf(hp) & 
        (INTERFACE_COMPMODE_444 | INTERFACE_COMPMODE_MOTION444);
}

static inline int check_lut_change(struct LUT_struct *olutp, 
                                   struct LUT_struct *lutp)
{
    if (lutp == NULL || lutp->a_alutreg == RESERVED_INVALID_LUTID) {
        if (olutp == NULL || olutp->a_alutreg == RESERVED_INVALID_LUTID) 
            return 0;
        else
            return 1;
    }
    else if (olutp == NULL || olutp->a_alutreg == RESERVED_INVALID_LUTID) {
        return 1;
    }
    return (compare_luts(lutp, olutp) || (lutp->a_alutreg != olutp->a_alutreg));
}

static inline int check_override_config_change(struct override_config *ocfg,
                                               struct override_config *cfg) 
{
    return memcmp(ocfg, cfg, sizeof(struct override_config)) ? 1 : 0;
}

static inline int monitor_plugin(int old_detect, int new_detect)
{
    return (old_detect == 0 && new_detect == 1) ? 1 : 0;
}

static inline int monitor_unplug(int old_detect, int new_detect)
{
    return (old_detect == 1 && new_detect == 0) ? 1 : 0;
}

static int check_connection_status(HAL *hp)
{
    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        struct strmsrv_tx_connection_params tx_connection_params;
        if (strmsrv_get_tx_connection_params(hal_get_channel_number(hp), 
                &tx_connection_params) == 0) 
            return tx_connection_params.connected;
        else 
            return 0;
    }
    else {
        struct strmsrv_rx_connection_params rx_connection_params;
        if (strmsrv_get_rx_connection_params(hal_get_channel_number(hp), 
                &rx_connection_params) == 0)
            return (rx_connection_params.client_unavailable == 1) ? 0 : 1;
        else
            return 0;
    }

    return 0;
}

static int synchronize_lut_and_edid_eeprom(HAL *hp, unsigned char *monbuf)
{
    unsigned char buf[EDID_DATA_LEN];

    if (hp == NULL)
        return -1;

    VLOG_INFO("Synchronizing LUT and EDID EEPROM with monitor");

    if (load_edid_eeprom_from_monitor(hp) < 0) {
        VLOG_INFO("Failed to update EDID EEPROM from monitor");
        return -1;
    }
    if (hal_read_mon_edid(hp, buf, sizeof(buf)) < 0) {
        VLOG_INFO("Failed to read monitor EDID. Not synchronizing EDID EEPROM");
        return -1;
    }

    return update_lut_from_buffer(hp, buf);
}

static int get_video_status(HAL *hp, struct LUT_struct *alutp, int streamflag)
{
    int status = 0;

    if (alutp != NULL && alutp->a_alutreg != RESERVED_INVALID_LUTID) 
        status = VIDIN_LUT_DETECT | VIDIN_SIGNAL_DETECT; 
    else if (hal_getActiveIntf(hp) & (INTERFACE_DIGITAL | INTERFACE_HV)) 
        status = VIDIN_SIGNAL_DETECT;

    if (streamflag && (status & VIDIN_LUT_DETECT))
        status |= VIDIN_STREAMING;

    return status;
}

static int get_monitor_status_and_update_edid(HAL *hp,
                                              int connected,
                                              int monitor_detect,
                                              int old_monitor_detect,
                                              struct LUT_struct *lutp)
{
    int status = 0;
    unsigned char monbuf[EDID_DATA_LEN];

    if (monitor_detect) {
        status = VIDOUT_MON_DETECT;
        if (hal_read_mon_edid(hp, monbuf, sizeof(monbuf)) < 0) {
            status |= VIDOUT_MON_ERROR;
            VLOG_INFO("Failed to read monitor EDID");
        }
        else {
            status |= VIDOUT_MON_GOT_EDID;
            if (monitor_plugin(old_monitor_detect, monitor_detect)) {
                VLOG_INFO("Detected monitor plug-in");
                if (get_sync_edid_eeprom_config(hp)) {
                    if (!synchronize_lut_and_edid_eeprom(hp, monbuf))
                        status |= VIDOUT_EDID_LUT_MATCH;
                    else
                        status |= VIDOUT_MON_ERROR;
                }
            }
        }
        if ((status & VIDOUT_MON_GOT_EDID) && lutp != NULL) {
            if (check_lut_edid_match(lutp, monbuf, sizeof(monbuf),
                        (debug > 2) ? 1 : 0))
                status |= VIDOUT_EDID_LUT_MATCH;
        }
    }
    else if (monitor_unplug(old_monitor_detect, monitor_detect)) {
        VLOG_INFO("Detected monitor unplug");
    }

    if (connected && (status & VIDOUT_EDID_LUT_MATCH))
        status |= VIDOUT_DISPLAYING;

    return status;
}

static void get_netif_status(HAL *hp,
                             int *link_index,
                             int *link0_speed,
                             int *link1_speed)
{
    int rc, phy0_status, phy1_status;

    phy0_status = get_sysfs_attribute(OMNITEK_PHY0_STATUS, "%d\n");

    phy1_status = get_sysfs_attribute(OMNITEK_PHY1_STATUS, "%d\n");

    if (phy0_status == 0 || phy1_status == 0) {
        *link0_speed = *link1_speed = LINK_SPEED_IDLE;
        *link_index = -1;
        return;
    }

    rc = get_sysfs_attribute(OMNITEK_LINK_INDEX, "%d\n");
    *link_index = (rc < 0) ? 0 : rc;

    rc = get_sysfs_attribute(OMNITEK_LINK0_SPEED, "%d\n");
    *link0_speed = (rc < 0) ? 0 : rc;

    rc = get_sysfs_attribute(OMNITEK_LINK1_SPEED, "%d\n");
    *link1_speed = (rc < 0) ? 0 : rc;

    if (debug > 2)
        VLOG_INFO("%s: returning index %d, link0_speed %d, link1_speed %d", 
                __func__, *link_index, *link0_speed, *link1_speed);
}

static inline int check_status_change(struct status_st *ostatus,
                                      struct status_st *status) 
{
    return memcmp(ostatus, status, sizeof(struct status_st)) ? 1 : 0;
}

static int testLED_inprogress(HAL *hp)
{
    struct stat buf;
    if (stat("/tmp/.testLED", &buf) == 0) 
        return 1;
    return 0;
}

static int main_RJ45_LED_settings(int link_speed, int on) 
{
    int rc = 0;

    if (on) {
        if (link_speed == LINK_SPEED_1G)
            rc = (1 << MAINRJ45_GREEN_BIT);
        else if (link_speed == LINK_SPEED_100M)
            rc = (1 << MAINRJ45_ORANGE_BIT);
        else if (link_speed == LINK_SPEED_10M)
            rc = (1 << MAINRJ45_YELLOW_BIT);
    }

    if (debug > 2)
        VLOG_INFO("%s: link_speed = %d, on = %d. returning 0x%x", __func__,
                link_speed, on, rc);
    return rc;
}

static int exp_RJ45_LED_settings(int link_speed, int on) 
{
    int rc = 0;

    if (on) {
        if (link_speed == LINK_SPEED_1G)
            rc = (1 << EXPRJ45_GREEN);
        else if (link_speed == LINK_SPEED_100M)
            rc = (1 << EXPRJ45_ORANGE);
        else if (link_speed == LINK_SPEED_10M)
            rc = (1 << EXPRJ45_YELLOW);
    }

    if (debug > 2)
        VLOG_INFO("%s: link_speed = %d, on = %d. returning 0x%x", __func__,
                link_speed, on, rc);
    return rc;
}

static void get_LED_settings(struct status_st *status,
                             int *led_on, 
                             int *led_blink)
{
    static int rj45_on = 1;

    *led_on = (1 << EXPRJ45_ORANGE) | (1 << EXPRJ45_GREEN) | 
        (1 << EXPRJ45_YELLOW);

    *led_blink = 0;

    if (status->link_index < 0) { 
        return;
    }
    else if (status->link_index == 0) {
        *led_on = main_RJ45_LED_settings(status->link0_speed, rj45_on);
        *led_on |= exp_RJ45_LED_settings(status->link1_speed, 1);
    }
    else if (status->link_index == 1) {
        *led_on = main_RJ45_LED_settings(status->link0_speed, 1);
        *led_on |= exp_RJ45_LED_settings(status->link1_speed, rj45_on);
    }
    rj45_on = !rj45_on;

    if (status->video_status & VIDIN_STREAMING)
        *led_blink |= (1 << VIDEOIN_LED_GREEN);
    else if (status->video_status & VIDIN_LUT_DETECT)
        *led_on |= (1 << VIDEOIN_LED_GREEN);
    else if (status->video_status & VIDIN_SIGNAL_DETECT)
        *led_on |= (1 << VIDEOIN_LED_RED);

    if (status->monitor_status & VIDOUT_DISPLAYING) {
        if (status->codec_status & CODEC_IS_DECODER)
            *led_blink |= (1 << VIDEOOUT_LED_GREEN);
        else
            *led_on |= (1 << VIDEOOUT_LED_GREEN);
    }
    else if (status->monitor_status & VIDOUT_EDID_LUT_MATCH) 
        *led_on |= (1 << VIDEOOUT_LED_GREEN);
    else if (status->monitor_status & VIDOUT_MON_GOT_EDID)
        *led_blink |= (1 << VIDEOOUT_LED_RED);
    else if (status->monitor_status & VIDOUT_MON_DETECT)
        *led_on |= (1 << VIDEOOUT_LED_RED);

    if (status->codec_status & CODEC_STREAMING) {
        if (status->codec_status & CODEC_IS_ENCODER)
            *led_blink |= (1 << ENCODER_LED_GREEN);
        else if (status->codec_status & CODEC_IS_DECODER)
            *led_blink |= (1 << DECODER_LED_GREEN);
    }
    else if (status->codec_status & CODEC_IS_ENCODER) 
        *led_on |= (1 << ENCODER_LED_GREEN);
    else if (status->codec_status & CODEC_IS_DECODER) 
        *led_on |= (1 << DECODER_LED_GREEN);
}

static void update_LEDs(HAL *hp, struct status_st *status)
{
    static int old_led_on = 0, old_led_blink = 0;
    int led_on, led_blink;

    if (!testLED_inprogress(hp)) {
        get_LED_settings(status, &led_on, &led_blink);
        if (debug > 0)
            VLOG_INFO("%s: led_on 0x%x, led_blink 0x%x", __func__, 
                led_on, led_blink); 
        if (led_on != old_led_on || led_blink != old_led_blink) {
            hal_set_all_LEDs(hp, led_on, led_blink);  
            old_led_on = led_on;
            old_led_blink = led_blink;
        }
    }
}

static void set_revert_to_default_LEDs(HAL *hp)
{
    hal_LED_blink(hp, ENCODER_LED_GREEN, 0);
    hal_LED_blink(hp, DECODER_LED_GREEN, 0);
}

static void handle_revert_to_default(HAL *hp)
{
    static int revert_to_default_timer = 0;

    if (revert_to_default_pressed()) 
        revert_to_default_timer++;
    else if (revert_to_default_timer > 0) 
        revert_to_default_timer = 0;

    if (revert_to_default_timer >= REVERT_TO_DEFAULT_TIMER_MAX) {
        set_revert_to_default_LEDs(hp);
        system_command(REVERT_TO_DEFAULT_COMMAND);
    }
}

static int check_license_violation(HAL *hp)
{
    return (hal_get_fpga_flags(hp) & FLAG_XPI_LICENSE_VIOLATION) ? 1 : 0;
}

#define NO_LICENSE_STR          "     No License     "
#define NO_STEREO_LICENSE_STR   " No Stereo License  " 
#define NO_DUALLINK_LICENSE_STR "No Dual Link License"

static void init_osd_parameters(HAL *hp, char *str)
{
    int license;

    license = hal_get_configured_license(hp);
    if (license == PRO3D_LICENSE) 
        strcpy(str, "");
    else if (license == PRO_LICENSE)
        strcpy(str, NO_STEREO_LICENSE_STR);
    else if (license == LITE_LICENSE) 
        strcpy(str, NO_DUALLINK_LICENSE_STR);
    else 
        strcpy(str, NO_LICENSE_STR);
}

static int get_hvres(HAL *hp, int *hres, int *vres)
{
    struct LUT_struct lut;

    *hres = *vres = 0;
    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        if (hal_get_addside_lut(hp, &lut) == 0) {
            *hres = lut.a_orig_hres;
            *vres = lut.a_orig_vres;
            return 0;
        }
    }
    else {
        if (hal_get_display_lut(hp, &lut) == 0) {
            *hres = lut.a_orig_hres;
            *vres = lut.a_orig_vres;
            return 0;
        }
    }
    return -1;
}

static void get_osd_position(HAL *hp, int init, int *xpos, int *ypos)
{
    static int dx, dy, osdminx, osdminy, osdmaxx, osdmaxy;
    
    if (init) {
        int hres, vres;
        if (get_hvres(hp, &hres, &vres) == 0) {
            *xpos = hres / 2 - DEFAULT_STRING_ICON_WIDTH / 2;
            osdminx = *xpos - hres / 4;
            osdmaxx = *xpos + hres / 4;
            *ypos = vres / 2 - DEFAULT_STRING_ICON_HEIGHT / 2;
            osdminy = *ypos - vres / 4;
            osdmaxy = *ypos + vres / 4;
            dx = dy = DEFAULT_STRING_ICON_DELTA;
        }
        else {
            *xpos = osdminx = osdmaxx = *ypos = osdminy = osdmaxy = dx = dy = 0;
        }
    }
    else {
        if ((*xpos + dx) > osdmaxx || (*xpos + dx) < osdminx) 
            dx = -dx;
        if ((*ypos + dy) > osdmaxy || (*ypos + dy) < osdminy)
            dy = -dy;
        *xpos += dx;
        *ypos += dy;
    }
}

static void handle_license_violation(HAL *hp, char *str)
{
    static int old_license_violation = 0, x = 0, y = 0;
    int license_violation;
    int color = color_code_from_name("Yellow");
    int is_tx = (hal_get_board_type(hp) == BOARDTYPE_TX) ? 1 : 0;

    /* check if driver detected license violation */
    license_violation = check_license_violation(hp);
    if (license_violation != old_license_violation) {
        if (license_violation) {
            if (is_tx) {
                if (hal_set_adc_free_run(hp, RESERVED_FRUN_1280_LUTID) < 0) {
                    VLOG_INFO_CAT(VLOG_HALD, 
                        "%s: failed to set ADC into free run.", __func__);
                }
            }
            hal_init_string_icon(hp, 1, color, str);
            get_osd_position(hp, 1, &x, &y);
            hal_move_icon_xpos(hp, x);
            hal_move_icon_ypos(hp, y);
        }
        else {
            if (is_tx) {
                if (hal_set_adc_free_run(hp, 0) < 0) {
                    VLOG_INFO_CAT(VLOG_HALD, 
                        "%s: failed to set ADC to not free run.", __func__);
                }
            }
            hal_init_string_icon(hp, 0, color, str);
        }
        old_license_violation = license_violation;
    }
    else if (license_violation) {
        get_osd_position(hp, 0, &x, &y);
        hal_move_icon_xpos(hp, x);
        hal_move_icon_ypos(hp, y);
    }
}

static void run_xpi_tx_loop(HAL *hp, int timeout, int fanconfig)
{
    struct LUT_struct old_addsidelut, addsidelut;
    int old_connected = 0, connected, old_monitor_detect = 0, monitor_detect;
    struct status_st old_status, status;
    int sync_cfg, addside_changed, mon_retries = 0;
    char osdstr[32];

    /* initialize OSD parameters */
    init_osd_parameters(hp, osdstr);
    
    /* Initialize codec status */
    memset(&old_status, 0, sizeof(struct status_st));
    old_status.codec_status = CODEC_IS_ENCODER;

    /* Initialize setting for whether to sync EDID EEPROM with monitor */ 
    sync_cfg = get_sync_edid_eeprom_config(hp);
    if (sync_cfg) {
        (void) synchronize_lut_and_edid_eeprom(hp, NULL);
    }

    /* Initialize current addside LUT */
    memset(&old_addsidelut, 0, sizeof(struct LUT_struct));
    old_addsidelut.a_alutreg = RESERVED_INVALID_LUTID;

    /* Initialize LEDs */
    update_LEDs(hp, &old_status);

    /* 
     * The following loop monitors temperature, the revert-to-default button,
     * whether driver detected license violation, synchronizes the EEPROM EDID 
     * with the monitor, and changes LEDs.
     */
    while (1) {

        /* check temperature */
        if (hal_check_max6650(hp) && fanconfig == FAN_SPEED_AUTO) 
            monitor_board_temperature(hp, timeout);

        /* handle revert-to-default push-button */
        handle_revert_to_default(hp);

        /* handle license violation */
        handle_license_violation(hp, osdstr);

        /* check if sync_edid config changed */
        if (get_sync_edid_eeprom_config(hp)) {
            if (!sync_cfg) {
                (void) synchronize_lut_and_edid_eeprom(hp, NULL);
                sync_cfg = 1;
            }
        }
        else {
            sync_cfg = 0;
        }

        /* check if should update LEDs */
        if (hal_get_addside_lut(hp, &addsidelut) < 0) {
            memset(&addsidelut, 0, sizeof(struct LUT_struct));
            addsidelut.a_alutreg = RESERVED_INVALID_LUTID;
        }
        if ((addside_changed = check_lut_change(&old_addsidelut, &addsidelut)))
            memcpy(&old_addsidelut, &addsidelut, sizeof(struct LUT_struct));

        connected = check_connection_status(hp);
        monitor_detect = check_monitor_detect(hp);
        status.codec_status = CODEC_IS_ENCODER | 
                            ((connected) ? CODEC_STREAMING : 0);
        status.video_status = get_video_status(hp, &addsidelut, connected);

        if (monitor_detect >= 0 && (addside_changed || 
                    (connected != old_connected) ||
                    (monitor_detect != old_monitor_detect))) {
            status.monitor_status = get_monitor_status_and_update_edid(hp, 
                    connected, monitor_detect, old_monitor_detect, &addsidelut);
            if (!(status.monitor_status & VIDOUT_MON_ERROR) ||
                    ++mon_retries >= MAX_MON_RETRIES) {
                old_monitor_detect = monitor_detect;
                mon_retries = 0;
            }
            old_connected = connected;
        }
        else {
            status.monitor_status = old_status.monitor_status;
        }

        /* check if network interface status changed */
        get_netif_status(hp, &(status.link_index), &(status.link0_speed), 
                &(status.link1_speed));

        if (debug > 1)
            VLOG_INFO("%s: video_status 0x%x monitor_status 0x%x " 
                "codec_status 0x%x link_status %d/%d/%d", __func__, 
                status.video_status, status.monitor_status, status.codec_status,
                status.link_index, status.link0_speed, status.link1_speed); 

        /* update LEDs */
        update_LEDs(hp, &status);
        if (check_status_change(&old_status, &status)) 
            memcpy(&old_status, &status, sizeof(struct status_st));

        usleep(timeout);
    }
}

static void run_xpi_rx_loop(HAL *hp, int timeout, int fanconfig)
{
    struct LUT_struct old_addsidelut, addsidelut;
    struct LUT_struct old_dropsidelut, dropsidelut, overridelut;
    struct LUT_struct *old_displaylutp = NULL, *displaylutp = NULL;
    struct display_config oldcfg, cfg;
    struct override_config oldoverridecfg, overridecfg;
    int overlay_changed = 0, override_changed = 0, addside_changed;
    int dropside_changed, display_changed, old_connected = 0, connected;
    int old_monitor_detect = 0, monitor_detect, mon_retries = 0;
    int sync_cfg, old_compmode, compmode;
    struct status_st old_status, status;
    int cno = hal_get_channel_number(hp);
    char osdstr[32];

    /* initialize OSD parameters */
    init_osd_parameters(hp, osdstr);
    
    /* Initialize codec status */
    memset(&old_status, 0, sizeof(struct status_st));
    old_status.codec_status = CODEC_IS_DECODER;

    /* Initialize current addside LUT */
    memset(&old_addsidelut, 0, sizeof(struct LUT_struct));
    old_addsidelut.a_alutreg = RESERVED_INVALID_LUTID;

    /* Initialize current dropside LUT */
    memset(&old_dropsidelut, 0, sizeof(struct LUT_struct));
    old_dropsidelut.a_alutreg = RESERVED_INVALID_LUTID;

    /* Initialize current compmode */
    old_compmode = get_compmode(hp);

    /* Initialize setting for whether to sync EDID EEPROM with monitor */ 
    sync_cfg = get_sync_edid_eeprom_config(hp);
    if (sync_cfg) {
        (void) synchronize_lut_and_edid_eeprom(hp, NULL);
    }

    /* Initialize overlay in driver */
    memset(&oldcfg, 0, sizeof(struct display_config));

    /* Initialize override configuration */
    memset(&oldoverridecfg, 0, sizeof(struct override_config));

    /* Initialize LEDs */
    update_LEDs(hp, &old_status);

    /* 
     * The following loop checks for addside and overlay and override 
     * configuration changes and re-configures the display LUT and the
     * dropside FPGAs. It also monitors temperature, the revert-to-default 
     * button, synchronizes the EDID EEPROM with monitor and updates LEDs.
     */
    while (1) {

        /* check addside, overlay and override config changes */
        if (hal_get_addside_lut(hp, &addsidelut) < 0) {
            memset(&addsidelut, 0, sizeof(struct LUT_struct));
            addsidelut.a_alutreg = RESERVED_INVALID_LUTID;
        }
        addside_changed = check_lut_change(&old_addsidelut, &addsidelut);

        if (hal_get_dropside_lut(hp, &dropsidelut) < 0) {
            memset(&dropsidelut, 0, sizeof(struct LUT_struct));
            dropsidelut.a_alutreg = RESERVED_INVALID_LUTID;
        }
        dropside_changed = check_lut_change(&old_dropsidelut, &dropsidelut);

        get_display_config(hp, &cfg);
        overlay_changed = check_source_scale_and_cropping_change(&oldcfg, &cfg);

        xp_get_override_config(hp, &overridecfg);
        override_changed = check_override_config_change(&oldoverridecfg, 
                &overridecfg);

        /* 
         * Note since the driver needs current display LUT to compute actual 
         * overlay parameters the routine update_overlay_parameters() must 
         * be called AFTER the display LUT has been updated in the driver.
         */
        if (cfg.source != DISPLAY_SOURCE_REMOTE && addside_changed) {
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Addside changed. Setting display LUT to %d (%s).", 
                    addsidelut.a_alutreg, addsidelut.comment);
            hal_set_display_video(hp, &addsidelut);
            displaylutp = &addsidelut;
            update_overlay_parameters(hp, &cfg);
        }

        if (overlay_changed) {
            if (cfg.source != oldcfg.source && 
                    cfg.source != DISPLAY_SOURCE_REMOTE) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Overlay changed. Setting display LUT to %d (%s)",
                        addsidelut.a_alutreg, addsidelut.comment);
                hal_set_display_video(hp, &addsidelut);
                displaylutp = &addsidelut;
            }
            update_overlay_parameters(hp, &cfg);
        }

        monitor_detect = check_monitor_detect(hp);
        if (cfg.source == DISPLAY_SOURCE_REMOTE && (dropside_changed ||
                    overlay_changed || override_changed ||
                    monitor_plugin(old_monitor_detect, monitor_detect))) {
            if (get_override_display_lut(hp, &dropsidelut, &overridelut, 
                        &overridecfg) < 0) {
                VLOG_INFO_CAT(VLOG_HALD, 
                    "Failed to get override LUT. Not overriding LUT %d (%s)",
                    dropsidelut.a_alutreg, dropsidelut.comment);
                hal_set_display_video(hp, &dropsidelut);
                displaylutp = &dropsidelut;
            }
            else if (overridecfg.ov_option == OV_OPTION_NONE) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Setting display LUT to ID %d (%s)", 
                        dropsidelut.a_alutreg, dropsidelut.comment); 
                hal_set_display_video(hp, &dropsidelut);
                displaylutp = &dropsidelut;
            }
            else {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Overriding LUT ID %d(%s) with LUT ID %d(%s)", 
                        dropsidelut.a_alutreg, dropsidelut.comment, 
                        overridelut.a_alutreg, overridelut.comment);
                set_overlay_params_for_override(hp, &overridecfg);
                hal_set_display_video(hp, &overridelut);
                displaylutp = &overridelut;
            }
        }

        // TSM0090-291: if we disconnect and reconnect within 2 or 3 secs,
        // there're cases where HALD misses the dropside LUT change even 
        // though the display LUT has been changed to splash when disconnect
        // happened. This is a workaround for the problem. 
        {
            struct LUT_struct tmplut;
            if (hal_get_display_lut(hp, &tmplut) == 0 || 
                    !compare_luts(displaylutp, &tmplut)) {
                hal_set_display_video(hp, displaylutp);
            }
        }

        if (addside_changed) 
            memcpy(&old_addsidelut, &addsidelut, sizeof(struct LUT_struct));

        if (dropside_changed) 
            memcpy(&old_dropsidelut, &dropsidelut, sizeof(struct LUT_struct));

        if (overlay_changed)
            memcpy(&oldcfg, &cfg, sizeof(struct display_config));

        if (override_changed) 
            memcpy(&oldoverridecfg, &overridecfg, 
                    sizeof(struct override_config));

        /* check if should request for full screen fresh */
        compmode = get_compmode(hp);
        if (old_compmode != compmode || dropside_changed || overlay_changed || 
                override_changed) {
            old_compmode = compmode;
            if (cfg.source != DISPLAY_SOURCE_LOCAL) {
                strmsrv_rx_fullscreen_refresh(cno);
            }
        }

        /* check if should request mouse cursor refresh */
        if (cfg.source != DISPLAY_SOURCE_LOCAL && (dropside_changed ||
                    overlay_changed || override_changed)) {
            struct strmsrv_rx_clnt_params clnt_params;
            memset(&clnt_params, 0, sizeof(clnt_params));
            clnt_params.rfb_refresh = 1;
            strmsrv_set_rx_clnt_params(cno, &clnt_params);
        }

        /* check temperature */
        if (hal_check_max6650(hp) && fanconfig == FAN_SPEED_AUTO) 
            monitor_board_temperature(hp, timeout);

        /* handle revert-to-default push-button */
        handle_revert_to_default(hp);

        /* handle license violation */
        handle_license_violation(hp, osdstr);

        /* check if sync_edid config changed */
        if (get_sync_edid_eeprom_config(hp)) {
            if (!sync_cfg) {
                (void) synchronize_lut_and_edid_eeprom(hp, NULL);
                sync_cfg = 1;
            }
        }
        else {
            sync_cfg = 0;
        }

        /* check if should update LEDs */
        if ((display_changed = check_lut_change(old_displaylutp, displaylutp)))
            old_displaylutp = displaylutp;

        connected = check_connection_status(hp);
        monitor_detect = check_monitor_detect(hp);
        status.codec_status = CODEC_IS_DECODER | 
                                ((connected) ? CODEC_STREAMING : 0);
        status.video_status = get_video_status(hp, &addsidelut, 
                (cfg.source == DISPLAY_SOURCE_LOCAL));
        
        if (monitor_detect >= 0 && (addside_changed || display_changed || 
                    (connected != old_connected) ||
                    (monitor_detect != old_monitor_detect))) {

            status.monitor_status = get_monitor_status_and_update_edid(hp, 
                    connected, monitor_detect, old_monitor_detect, displaylutp);
            if (!(status.monitor_status & VIDOUT_MON_ERROR) ||
                    ++mon_retries >= MAX_MON_RETRIES) {
                old_monitor_detect = monitor_detect;
                mon_retries = 0;
            }
            old_connected = connected;
        }
        else {
            status.monitor_status = old_status.monitor_status;
        }

        /* check network interface status */
        get_netif_status(hp, &(status.link_index), &(status.link0_speed),
                &(status.link1_speed));

        if (debug > 1)
            VLOG_INFO("%s: video_status 0x%x monitor_status 0x%x " 
                "codec_status 0x%x link_status %d/%d/%d", __func__, 
                status.video_status, status.monitor_status, status.codec_status,
                status.link_index, status.link0_speed, status.link1_speed); 

        /* update LEDs */
        update_LEDs(hp, &status);
        if (check_status_change(&old_status, &status)) 
            memcpy(&old_status, &status, sizeof(struct status_st));

        usleep(timeout);
    }
}
#endif

/*************
 * HALD help *
 *************/
static void print_hald_help(void) {
    printf("\t-b board number\n");
    printf("\t-F run program in foreground\n");
    printf("\t-d level for debug messages (1 for LED, 2 for EDID match)\n");
    printf("\t-h print this message\n");
}

/***********************
 * HALD signal handler *
 ***********************/
static void sighandler(int sig) {
    switch (sig) {
        case SIGPIPE:
            VLOG_INFO_CAT(VLOG_HALD, "Process %d ignoring SIGPIPE", getpid());
        break;
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
            VLOG_INFO_CAT(VLOG_HALD, "Stopping Process %d", getpid());
            unlink(pidfile);
            exit(1);
            break;
        case SIGHUP:
            VLOG_INFO_CAT(VLOG_HALD, "Process %d ignoring HUP", getpid());
            break;
        default:
            break;
    }
    return;
}

/*************************
 * Main routine for HALD *
 *************************/
int hald_main(int argc, char **argv) {
    int opt, bno = 0;
    HAL *hp;
    int nTimeout = 1000000;
    int foreGround=0;
    FILE *fp=NULL;
    int codecversion = 0;
    int fanConfig;
    int hardware_type;

    signal (SIGPIPE, sighandler);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGINT, sighandler);
    signal (SIGTERM, sighandler);
    signal (SIGQUIT, sighandler);
    signal (SIGHUP, sighandler);

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this program\n");
        return 0;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "hb:Fd:")) > 0) {
        switch (opt) {
        case 'b':
            bno = atoi(optarg);
            break;
        case 'F':
            foreGround = 1;
            break;
        case 'd':
            debug = atoi(optarg);
            break;
        case 'h':
        case '?':
            print_hald_help();
            return 0;
        default:
            break;
        }
    }

    if (bno >= MAX_FPGA_BOARDS || bno < 0) {
        print_hald_help();
        return 0;
    }

    if (!foreGround) daemon(1,0);
    sprintf (pidfile, "%s.%d", PIDPREFIX, bno);
    fp = fopen(pidfile, "w");
    fprintf (fp, "%u\n", getpid());
    fclose(fp);

    hp = hal_acquire (bno);
    if (hp == NULL) {
        VLOG_CRITICAL_CAT(VLOG_HALD, "Invalid Channel %d", bno+1);
        goto OUT;
    }

    /************************************
     * Don't load if board not a V2D XP *
     ************************************/
    hardware_type = hal_get_hardware_type(hp);
    if (!hal_get_hardware_detected(hp) ||
	    (hardware_type != PLX_DEVICE && hardware_type != XPI_DEVICE)) {
        VLOG_WARNING_CAT(VLOG_HALD, "Channel %d not a V2D board", bno+1);
        goto OUT;
    }

    /*********************************************
     * Initialize for console and fan operations * 
     *********************************************/
    if (hardware_type== PLX_DEVICE) {
        if (hal_get_console_fd(hp) < 0) {
            VLOG_CRITICAL_CAT(VLOG_HALD, 
                "Channel %d Unable to open Keyboard TTY", bno+1);
            goto OUT;
        }
    }
    hal_init_fan_speed(hp, get_fan_speed_config(hp));    
    fanConfig = get_fan_speed_config(hp);

    /*********************************************
     * Constantly poll the add and drop side for *  
     * LUT and/or video configuration changes    *
     *********************************************/
    VLOG_INFO_CAT(VLOG_HALD, "Starting HALD Daemon...");

#ifndef __XPI__
    if (hal_get_board_type(hp) == BOARDTYPE_TX) 
        run_xp_tx_loop(hp, nTimeout, fanConfig);
    else
        run_xp_rx_loop(hp, nTimeout, fanConfig);
#else
    if (hal_get_board_type(hp) == BOARDTYPE_TX) 
        run_xpi_tx_loop(hp, nTimeout, fanConfig);
    else
        run_xpi_rx_loop(hp, nTimeout, fanConfig);
#endif

OUT:
    VLOG_INFO_CAT(VLOG_HALD, "HALD Daemon Exiting...");

    hal_release(hp);

    unlink(pidfile);

    return 0;
}

