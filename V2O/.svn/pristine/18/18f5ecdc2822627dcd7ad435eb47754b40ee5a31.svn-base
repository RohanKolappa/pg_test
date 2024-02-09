#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "fpga.h"
#include "fpga_ioctl.h"
#include "legacyv2d.h"
#include "v4_fpga.h"
#include "lut_table.h"
#include "ad9852dds.h"
#include "ad9887vif.h"
#include "ad9888vif.h"
#include "id18125fifo.h"
#include "ddcedid.h"
#include "led_control.h"
#include "log_control.h"
#include "coeff_matrix.h"
#include "flow_control.h"
#include "slice_size.h"

/*****************************
 * Local function prototypes *
 *****************************/
static void configure_addside(struct v2d_board *board, struct LUT_struct *lutp);
static int configure_dropside(struct v2d_board *board, struct LUT_struct *lutp);
static void configure_addside_fpga(struct v2d_board *board,
                                   struct LUT_struct *lutp);
static void compute_actual_overlay_params(struct v2d_board *board,
                                          struct ov_param_io *a_cfg,
                                          struct ov_param_io *cfg);
static void update_overlay_info(struct v2d_board *board);
static void v2_configure_addside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp);
static void v4_configure_addside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp);
static int v2_configure_dropside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp);
static int v4_configure_dropside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp);
static void v2_configure_ad9888(struct v2d_board *board,
                                struct LUT_struct *lutp);
static void v2_configure_ad9852(struct v2d_board *board,
                                struct LUT_struct *lutp);
static void v2_configure_id18125 (struct v2d_board *board,
                                  struct LUT_struct *lutp);
static void v2_monitor_ad9888(struct v2d_board *board);

static int fpga_procfile_reg_read(char* page,
                                  char** start,
                                  off_t off,
                                  int count,
                                  int* eof,
                                  void* data) ;

static void set_pixel_adjust(struct v2d_board *board,
                             int pixels,
                             int tweak_hi,
                             int tweak_lo);

static unsigned char  get_phase_adjust(struct v2d_board *board);
static void set_phase_adjust(struct v2d_board *board, unsigned char val);

static void adjust_hd(struct v2d_board *board, int shift);
static void adjust_vd(struct v2d_board *board, int shift);
static int get_vd(struct v2d_board *board);
static int get_hd(struct v2d_board *board);

static int initialize_fpga_registers(struct v2d_board *board);
static int get_codec_version(struct v2d_board *board);

static int get_codec_version(struct v2d_board *board) {
    int sval;
    int ival;

    if ((sval = get_fpga_val(board, FPGA_IDREG)) <= 0)
         return -1;

    sval &= CODECVERSION_MASK;
    ival = sval >> CODECVERSION_SHIFT;

    return ival;
}
/***********************************************
 * Interrupt handler for Add Side video change *
 ***********************************************/
static void addside_video_change(unsigned long data) {

    struct v2d_board *board = (struct v2d_board*) data;
    struct v2d_ctl *vctl;
    struct LUT_struct *lutp;
    unsigned short amlreg, amvreg, amhreg, amtreg;

    if ((vctl = board->v_ctl) == NULL)
        return;

    /**********************
     * Disable interrupts *
     **********************/
    fpga_interrupt_disable(board,INT_3_ADD_VHLSYNC_ACT_CHANGE);
    fpga_interrupt_disable(board,INT_4_ADD_VHSYNC_FREQ_CHANGE);

    /***************************
     * Set sync-on-green muxes *
     ***************************/
    v2_monitor_ad9888(board);

    /*********************************************
     * Find the LUT matching current video input *
     *********************************************/
    amlreg = get_fpga_val(board, FPGA_AMLREG);
    amvreg = get_fpga_val(board, FPGA_AMVREG);
    amhreg = get_fpga_val(board, FPGA_AMHREG);
    amtreg = get_fpga_val(board, FPGA_AMTREG);

    lutp = find_lut_entry(board,amlreg, amvreg, amhreg, amtreg,
            DUALLINK_IGNORE, TRUE);

    /*************************************
     * If no change in video, do nothing *
     *************************************/
    if (lutp == vctl->current_addside_lut) {

        if (lutp != NULL) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                     "Addside Current LUT unchanged %s (0x%x)\n",
                     lutp->comment, lutp->a_alutreg);
        }
        else {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                     "Addside Current LUT unchanged (NULL) \n");

            /* PSCW - When there's no sync input VHSYNC_FREQ_CHANGE interrupt
               may come continuously, "configure_addside" will keep on setting
               VID_ENABLE bit in FPGA_ACREG so we only configure the 9888/7 */
            v2_configure_ad9888(board, NULL);
        }
    }

    /*****************************************************
     * Else if no video found, schedule a search quickly *
     *****************************************************/
    else if (lutp == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                 "Addside  No matching LUT found\n");
        vctl->current_addside_lut = NULL;
        configure_addside(board, lutp);
        vctl->add_drop_timer_count = 0;
        mod_timer(&vctl->add_drop_timer, jiffies + HZ/10);

        /* PSCW - Disable the FPGA video otherwise when reading
           FPGA will wait forever for video and not read audio */
        and_in_fpga_val(board, FPGA_ACREG, ~(ACREG_VID_ENABLE | ACREG_LAN_ENABLE));
    }

    /************************************************************
     * New video found. Configure add side, and schedule a lazy *
     * search.                                                  *
     ************************************************************/
    else {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Addside Matched LUT %s (0x%x). \n",
                 lutp->comment, lutp->a_alutreg);

        vctl->current_addside_lut = lutp;
        vctl->hd_shift = lutp->a_hshift;
        vctl->vd_shift = lutp->a_vshift;
        configure_addside(board, lutp);
        LEDon(board,VIDEO_LED);
        mod_timer(&vctl->add_drop_timer, jiffies + HZ);
    }

    vctl->addside_lock_count = 0;

    /*********************
     * Enable interrupts *
     *********************/
    fpga_interrupt_enable(board,INT_3_ADD_VHLSYNC_ACT_CHANGE);
    fpga_interrupt_enable(board,INT_4_ADD_VHSYNC_FREQ_CHANGE);

}

static void add_side_process(unsigned long data) {

    struct v2d_board *board = (struct v2d_board*) data;
    struct LUT_struct *lutp;
    unsigned short amlreg, amvreg, amhreg, amtreg;
    struct v2d_ctl *vctl;

    if ((vctl = board->v_ctl) == NULL)
        return;

    amlreg = get_fpga_val(board, FPGA_AMLREG);
    amvreg = get_fpga_val(board, FPGA_AMVREG);
    amhreg = get_fpga_val(board, FPGA_AMHREG);
    amtreg = get_fpga_val(board, FPGA_AMTREG);

    /*********************************************
     * Find the LUT matching current video input *
     *********************************************/
    lutp = find_lut_entry(board,amlreg, amvreg, amhreg, amtreg,
            DUALLINK_IGNORE, FALSE);

    /****************************************************
     * If no video found, schedule another quick search *
     ****************************************************/
    if (lutp == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                 "Addside (Timer) No matching LUT found\n");
        vctl->no_addside_match_count++;
        if (vctl->add_drop_timer_count++ > 30) {
            mod_timer(&vctl->add_drop_timer, jiffies + HZ);
            if (vctl->addside_lock_count <= 30 && vctl->current_addside_lut != NULL) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                         "Addside (Timer) No matching LUT: resetting LUT\n");
                vctl->current_addside_lut = NULL;
                /* Really wipe out the fpga conf ?? */
                configure_addside(board, lutp);

                /* PSCW - Disable FPGA video. Otherwise a DMA request
                   from the application will cause FPGA to try to read
                   video and wait forever since no data will come  */
                and_in_fpga_val(board, FPGA_ACREG, ~(ACREG_VID_ENABLE | ACREG_LAN_ENABLE));
            }
        }
        else {
            mod_timer(&vctl->add_drop_timer, jiffies + HZ/10);
        }
    }

    /************************************************************
     * If video unchanged, do nothing. Schedule a lazy search.  *
     ************************************************************/
    else if (lutp == vctl->current_addside_lut) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                 "Addside (Timer) Current LUT unchanged %s (0x%x)\n",
                 lutp->comment, lutp->a_alutreg);
        mod_timer(&vctl->add_drop_timer, jiffies + HZ);

        vctl->addside_lock_count++;
    }

    /***********************************************************
     * New video found, configure add side and schedule a lazy *
     * search.                                                 *
     ***********************************************************/
    else {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Addside (Timer) Matched LUT %s (0x%x)\n",
                 lutp->comment, lutp->a_alutreg);
        vctl->current_addside_lut = lutp;
        vctl->hd_shift = lutp->a_hshift;
        vctl->vd_shift = lutp->a_vshift;
        configure_addside(board, lutp);
        mod_timer(&vctl->add_drop_timer, jiffies + HZ);
        vctl->addside_lock_count = 0;
    }


    /****************************************************************
     * Set Video LED on I50 TX. If video detected and matched LUT,  *
     * turn on LED, else if not matched, disable FPGA video reading *
     * and blink or turn off the LED.                               *
     ****************************************************************/
    if (vctl->current_addside_lut == NULL) {
        and_in_fpga_val(board, FPGA_ACREG, ~(ACREG_VID_ENABLE |
                                             ACREG_LAN_ENABLE));
        if ( (amtreg & (AMTREG_HSYNC|AMTREG_VSYNC)) !=
                        (AMTREG_HSYNC|AMTREG_VSYNC) ) {
            LEDoff(board,VIDEO_LED);
        }
        else {
            LEDblink(board,VIDEO_LED, LED_BLINK_FAST);
        }
    }
    else {
        LEDon(board,VIDEO_LED);
    }

    /* Set sync-on-green muxes */
    v2_monitor_ad9888(board);
}

static void drop_side_process(unsigned long data) {

    struct v2d_board *board = (struct v2d_board*) data;
    struct LUT_struct *lutp;
    unsigned long regval;
    struct v2d_ctl *vctl;

    if ((vctl = board->v_ctl) == NULL)
        return;

    lutp = board->v_ctl->current_display_lut;

    if (lutp != NULL) {
        if (lutp->d_dcreg & DCREG_VID_BYPASS)
            regval = (DSREG_VIDEO_CLOCK_DCM_LOCKED |
                    DSREG_SONET_CLOCK_DCM_LOCKED);
        else
            regval = (DSREG_SRAM_CLOCK_DCM_LOCKED |
                    DSREG_VIDEO_CLOCK_DCM_LOCKED |
                    DSREG_SONET_CLOCK_DCM_LOCKED);
        if ((get_fpga_val(board, FPGA_DSREG) & regval) != regval) {
            if (board->v_ctl->add_drop_timer_count++ > 2) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                    "Drop side DCM unlocked. Resetting dropside video ...\n");
                configure_dropside(board, lutp);
                board->v_ctl->add_drop_timer_count = 0;
            }
        }
    }
    mod_timer(&board->v_ctl->add_drop_timer, jiffies + HZ);
}

int initialize_fpga_registers (struct v2d_board *board) {
    unsigned short hardware_type;
    unsigned short mkcreg;
    struct v2d_ctl *vctl;

    if ((vctl = board->v_ctl) == NULL)
        return -1;

    hardware_type = (get_fpga_val(board, FPGA_IDREG) & 0xff00) >> 8;

    if (board->v_pci_dev_type == V2DI50_DEVICE) {

        and_in_fpga_val(board, FPGA_AAUDCREG, ~AUDCREG_DISABLE);
        mkcreg = get_fpga_val(board, FPGA_AAUDCREG);

        /******************************
         * Disable audio input to DAC *
         ******************************/
        or_in_fpga_val(board, FPGA_AAUDCREG, AUDCREG_DISABLE);
        mdelay(1);

        /************************************************************
         * If no LRCLK or SCLK from DAC, tell FPGA to generate them *
         ************************************************************/
        if ((mkcreg & AUDSTSREG_SCLK_PRESENT) != AUDSTSREG_SCLK_PRESENT)
            or_in_fpga_val(board, FPGA_AAUDCREG, AUDCREG_SET_SCLK);
        if ((mkcreg & AUDSTSREG_LRCLK_PRESENT) != AUDSTSREG_LRCLK_PRESENT)
            or_in_fpga_val(board, FPGA_AAUDCREG, AUDCREG_SET_LRCLK);

        /*****************************
         * Enable audio input to DAC *
         *****************************/
        and_in_fpga_val(board, FPGA_AAUDCREG, ~AUDCREG_DISABLE);
        if (hardware_type == 0xD2) {
            and_in_fpga_val(board, FPGA_DAUDCREG, ~AUDCREG_DISABLE);
            mkcreg = get_fpga_val(board, FPGA_DAUDCREG);

            /******************************
            * Disable audio input to DAC *
            ******************************/
            or_in_fpga_val(board, FPGA_DAUDCREG, AUDCREG_DISABLE);
            mdelay(1);

            /************************************************************
            * If no LRCLK or SCLK from DAC, tell FPGA to generate them *
            ************************************************************/
            if ((mkcreg & AUDSTSREG_SCLK_PRESENT) != AUDSTSREG_SCLK_PRESENT)
                or_in_fpga_val(board, FPGA_DAUDCREG, AUDCREG_SET_SCLK);
            if ((mkcreg & AUDSTSREG_LRCLK_PRESENT) != AUDSTSREG_LRCLK_PRESENT)
                or_in_fpga_val(board, FPGA_DAUDCREG, AUDCREG_SET_LRCLK);

            /*****************************
            * Enable audio input to DAC *
            *****************************/
            and_in_fpga_val(board, FPGA_DAUDCREG, ~AUDCREG_DISABLE);
        }


        /**********************************************
        * Initialize default ADC charge pump current *
        **********************************************/
        if (board->v_hardware_version == HARDWARE_VERSION_A)
            vctl->default_vco_cpmp = DEFAULT_VCO_CPMP;
        else
            vctl->default_vco_cpmp = DEFAULT_9887_VCO_CPMP;
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        or_in_fpga_val(board, FPGA_DAUDCREG, (DAUDCREG_RESET |
                    DAUDCREG_AMIXER | DAUDCREG_AFIFOCHK |
                    DAUDCREG_DMIXER | DAUDCREG_DFIFOCHK));
        and_in_fpga_val (board,  FPGA_DAUDCREG, ~AUDCREG_LOOPBACK);
        mdelay(1);
        and_in_fpga_val(board, FPGA_DAUDCREG, ~DAUDCREG_RESET );
        or_in_fpga_val(board, FPGA_DCREG, DCREG_LRSYNC_DISABLE);
        and_in_fpga_val(board, FPGA_DCREG, ~DCREG_GENLOCK_MODE);
    }
    return 0;
}


/****************************************
 * FPGA control initialization function *
 ****************************************/
int fpga_control_init(struct v2d_board *board)
{
    unsigned short mkcreg;
    struct v2d_ctl *vctl;
    struct proc_dir_entry *pdr;

    if ((vctl = kzalloc (sizeof (struct v2d_ctl), GFP_KERNEL)) == NULL)
        return ERROR;
    vctl->active_interface = INTERFACE_HV;
    vctl->video_freeze_check = 1;
    vctl->external_sync = 1;
    vctl->force_interface = AUTO_INTERFACE;
    vctl->progressive_compression = 1;
    vctl->add_drop_timer_count = 0;
    vctl->hd_shift = 0;
    vctl->vd_shift = 0;
    vctl->pix_adjust = vctl->pix_tweak_hi = vctl->pix_tweak_lo = 0;
    vctl->phase_shift = 0x80;
    vctl->force_no_video = 0;
    vctl->progressive_compression = 1;
    vctl->no_addside_match_count = 0;
    vctl->addside_lock_count = 0;
    vctl->num_slices_per_frame = 640;
    vctl->noise_thres_mode = 1;
    vctl->av_flag = 0;

    /*********************************
     * Initialize overlay parameters *
     *********************************/
    memset(&vctl->ocfg, 0, sizeof(struct ov_param_io));
    memset(&vctl->oinfo, 0, sizeof(struct ov_param_io));
    if (board->v_pci_dev_type == PLX_DEVICE) {
        vctl->ocfg.flags = vctl->oinfo.flags =
            OV_FLAG_SRC_REMOTE | OV_FLAG_VSCALESRC | OV_FLAG_HSCALESRC;
        vctl->ocfg.hscale = vctl->ocfg.vscale =
            vctl->oinfo.hscale = vctl->oinfo.vscale = RX_MAX_SCALE_VAL;
        vctl->ocfg.oalpha = vctl->oinfo.oalpha = 0;
    }
    board->v_ctl = vctl;

    /*******************************
     * Initialize global variables *
     *******************************/
    vctl->current_display_lut = NULL;
    vctl->current_dropside_lut = NULL;
    vctl->current_addside_lut = NULL;

    /*****************************************
     * Create interrupt statistics proc file *
     *****************************************/
    pdr = create_proc_read_entry("registers", 0, board->v_pdr,
                                 fpga_procfile_reg_read, board);
    if (pdr == NULL)
        return ERROR;


    /*****************************
     * Disable Drop Side of FPGA *
     *****************************/
    set_fpga_val(board, FPGA_DCREG,0x0);

    /****************************
     * Disable Add Side of FPGA *
     ****************************/
    and_in_fpga_val(board, FPGA_ACREG, ~ACREG_FIFO_XCLK);

    /*************
     * Reset DCM *
     *************/
    set_fpga_val(board, FPGA_CDCMRREG ,
                 CDCMRREG_ADD_RESET_VIDEO_DCM |
                 CDCMRREG_ADD_RESET_SONET_DCM |
                 CDCMRREG_DROP_RESET_VIDEO_DCM |
                 CDCMRREG_DROP_RESET_SRAM2_DCM |
                 CDCMRREG_DROP_RESET_SONET_DCM);

    mdelay(10);

    /*************************
     * Enable Add side first *
     *************************/
    or_in_fpga_val(board, FPGA_ACREG, ACREG_ADD_ENABLE);

    mdelay(1); /* Delay */

    or_in_fpga_val(board, FPGA_ACREG, (ACREG_ADD_ENABLE | ACREG_VID_ENABLE |
                                       ACREG_AUD_ENABLE | ACREG_LAN_ENABLE));

    /***************************
     * Enable Drop side second *
     ***************************/
    or_in_fpga_val(board, FPGA_DCREG, DCREG_DROP_ENABLE);


    mdelay(1); /* Delay */

    or_in_fpga_val(board, FPGA_DCREG, (DCREG_DROP_ENABLE | DCREG_VID_ENABLE |
                                       DCREG_AUD_ENABLE | DCREG_LAN_ENABLE));

    /****************************************************
     * Create timers for I50 add & drop side processing *
     ****************************************************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        init_timer(&vctl->add_drop_timer);
        vctl->add_drop_timer.data = (long)board;
        if (board->v_type == HARDWARE_V2D_TX)
            vctl->add_drop_timer.function = add_side_process;
        else
            vctl->add_drop_timer.function = drop_side_process;
        vctl->add_drop_timer.expires = jiffies + HZ;
        add_timer(&vctl->add_drop_timer);
    }

    initialize_fpga_registers(board);

    /*****************************
     * Enable mouse and keyboard *
     *****************************/
    mkcreg = 0x7900;
    mkcreg |= (MKCREG_MOUSE_ENABLE | MKCREG_KEYBOARD_ENABLE);
    set_fpga_val(board, FPGA_MKCREG, mkcreg);

    /******************************
     * Register interrupt handlers *
     ******************************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        fpga_register_irq(board, INT_3_ADD_VHLSYNC_ACT_CHANGE,
                      addside_video_change);

        fpga_register_irq(board, INT_4_ADD_VHSYNC_FREQ_CHANGE,
                      addside_video_change);

        fpga_interrupt_fire(board, INT_3_ADD_VHLSYNC_ACT_CHANGE);
    }


    return 0;
}


/**************************************
 * FPGA control finalization function *
 **************************************/
void fpga_control_exit(struct v2d_board *board)
{
    struct v2d_ctl *vctl;
    vctl = board->v_ctl;
    if (vctl != NULL) {
        if (vctl->current_dropside_lut != NULL)
            kfree(vctl->current_dropside_lut);
        if (vctl->current_display_lut != NULL)
            kfree(vctl->current_display_lut);
        kfree(vctl);
        board->v_ctl=NULL;
    }

    /*****************
     * Delete timers *
     *****************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        del_timer_sync(&vctl->add_drop_timer);
    }

    /*******************
     * Unregister IRQs *
     *******************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        fpga_unregister_irq(board,INT_3_ADD_VHLSYNC_ACT_CHANGE);
        fpga_unregister_irq(board,INT_4_ADD_VHSYNC_FREQ_CHANGE);
    }

    /**********************************
     * Unregister the proc file entry *
     **********************************/
    remove_proc_entry("registers", board->v_pdr);

}

/*******************************
 * FPGA control ioctl function *
 *******************************/
int fpga_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data) {

    struct reg_io regio;
    struct reg_io *regiop = (struct reg_io *) data;
    int empty, full;
    int shift;
    int ival, oval;
    unsigned char val;
    struct LUT_struct *lutp;
    unsigned short sval;

    struct v2d_ctl *vctl;

    vctl = board->v_ctl;

    if ((!(board->v_status & FPGA_BOARD_INITIALIZED) || vctl == NULL) &&
        (cmd != FPGA_IOC_MOD_REG)) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL,
            "%s  Channel %d not initialized!! \n", __func__, board->v_id+1);
        return -EINVAL;
    }

    switch (cmd) {
    case FPGA_IOC_MOD_REG:
        if ((regiop == NULL) ||
            (copy_from_user(&regio, regiop, sizeof(struct reg_io)))) {
            return -EFAULT;
        }

        switch (regio.type) {
        case REGIO_TYPE_FPGA_GET:
            regio.value = get_fpga_val(board, regio.reg);
            break;

        case REGIO_TYPE_FPGA_SET:
            set_fpga_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_FPGA_ANDIN:
            and_in_fpga_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_FPGA_ORIN:
            or_in_fpga_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_BAR2_GET:
            if (!is_plx_device(board))
                return -EFAULT;
            regio.value = get_bar2_val(board, regio.reg);
            break;

        case REGIO_TYPE_BAR2_SET:
            if (!is_plx_device(board))
                return -EFAULT;
            set_bar2_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_BAR3_GET:
            if (!is_plx_device(board))
                return -EFAULT;
            regio.value = get_bar3_val(board, regio.reg);
            break;

        case REGIO_TYPE_BAR3_SET:
            if (!is_plx_device(board))
                return -EFAULT;
            set_bar3_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_AD9888_GET:
            if (board->v_hardware_version == HARDWARE_VERSION_DA)
                regio.value = get_ad9887_val(regio.reg);
            else
                regio.value = get_ad9888_val(regio.reg);
            break;

        case REGIO_TYPE_AD9888_SET:
            if (board->v_hardware_version == HARDWARE_VERSION_DA)
                set_ad9887_val(regio.reg, regio.value);
            else
                set_ad9888_val(regio.reg, regio.value);
            break;

        case REGIO_TYPE_AD9852_GET:
            regio.value = get_ad9852_val(board,regio.reg);
            break;

        case REGIO_TYPE_AD9852_SET:
            set_ad9852_val(board,regio.reg, regio.value);
            break;

        case REGIO_TYPE_ID18125_GET:
            id18125_read(&empty, &full);
            regio.reg = empty;
            regio.value = full;
            break;

        case REGIO_TYPE_ID18125_SET:
            empty = regio.reg;
            full = regio.value;
            id18125_write(empty, full);
            break;
        }

        if (copy_to_user((struct reg_io *)data,
                         &regio, sizeof(struct reg_io)) != 0)
            return -EFAULT;

        return 0;

    case FPGA_IOC_GET_HDELAY:
        if ((int *)data == NULL)
            return -EFAULT;
        shift = get_hd(board);
        if (copy_to_user((int *)data, &shift, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_HDELAY:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&shift, (int *)data, sizeof(int)) != 0)
            return -EFAULT;
        adjust_hd(board,shift);
        return 0;

    case FPGA_IOC_GET_VDELAY:
        if ((int *)data == NULL)
            return -EFAULT;
        shift = get_vd(board);
        if (copy_to_user((int *)data, &shift, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_VDELAY:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&shift, (int *)data, sizeof(int)) != 0)
            return -EFAULT;
        adjust_vd(board,shift);
        return 0;

    case FPGA_IOC_GET_PSHIFT:
        if ((int *)data == NULL || is_plx_device(board))
            return -EFAULT;
        val = get_phase_adjust(board);
        if (copy_to_user((unsigned char *)data, &val, sizeof(unsigned char)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_PSHIFT:
        if ((int *)data == NULL || is_plx_device(board))
            return -EFAULT;
        if (copy_from_user(&val, (unsigned char *)data, sizeof(unsigned char)) != 0)
            return -EFAULT;
        set_phase_adjust(board,val);
        return 0;

    case FPGA_IOC_GET_DISPLAY_HPOL:
        if ((int *)data == NULL || !is_plx_device(board) ||
                            vctl->current_display_lut == NULL)
            return -EFAULT;
        ival = (vctl->current_display_lut->d_dcreg & DCREG_HSYNC_POL) ? 1 : 0;
        if (copy_to_user((unsigned char *)data, &ival, sizeof(unsigned char)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_DISPLAY_HPOL:
        if ((int *)data == NULL || !is_plx_device(board) ||
                            vctl->current_display_lut == NULL)
            return -EFAULT;
        if (copy_from_user(&val, (unsigned char *)data, sizeof(unsigned char)) != 0)
            return -EFAULT;
        else if (val)
            vctl->current_display_lut->d_dcreg |= DCREG_HSYNC_POL;
        else
            vctl->current_display_lut->d_dcreg &= ~DCREG_HSYNC_POL;
        return 0;

    case FPGA_IOC_GET_DISPLAY_VPOL:
        if ((int *)data == NULL || !is_plx_device(board) ||
                            vctl->current_display_lut == NULL)
            return -EFAULT;
        ival = (vctl->current_display_lut->d_dcreg & DCREG_VSYNC_POL) ? 1 : 0;
        if (copy_to_user((unsigned char *)data, &ival, sizeof(unsigned char)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_DISPLAY_VPOL:
        if ((int *)data == NULL || !is_plx_device(board) ||
                            vctl->current_display_lut == NULL)
            return -EFAULT;
        if (copy_from_user(&val, (unsigned char *)data, sizeof(unsigned char)) != 0)
            return -EFAULT;
        else if (val)
            vctl->current_display_lut->d_dcreg |= DCREG_VSYNC_POL;
        else
            vctl->current_display_lut->d_dcreg &= ~DCREG_VSYNC_POL;
        return 0;

    case FPGA_IOC_GET_PIXADJUST:
        {
            struct pixadjust_io pio;
            struct pixadjust_io *piop = (struct pixadjust_io *) data;

            if (piop == NULL)
                return -EFAULT;

            pio.pix_adjust = board->v_ctl->pix_adjust;
            pio.pix_tweak_hi = board->v_ctl->pix_tweak_hi;
            pio.pix_tweak_lo = board->v_ctl->pix_tweak_lo;

            if (copy_to_user((int *)data, &pio, sizeof(int)) != 0)
                return -EFAULT;
        }
        return 0;

    case FPGA_IOC_SET_PIXADJUST:
        {
            struct pixadjust_io pio;
            struct pixadjust_io *piop = (struct pixadjust_io *) data;

            if (piop == NULL ||
                    copy_from_user(&pio, piop, sizeof(struct pixadjust_io)))
                return -EFAULT;
            set_pixel_adjust(board, pio.pix_adjust, pio.pix_tweak_hi,
                    pio.pix_tweak_lo);
        }
        return 0;

    case FPGA_IOC_RESET_ADDSIDE:
    {
        configure_addside(board, vctl->current_addside_lut);
        return 0;
    }

    case FPGA_IOC_RESET_DROPSIDE:
    {
        return configure_dropside(board, vctl->current_dropside_lut);
    }

    case FPGA_IOC_GET_ADDSIDELUT:
    {
        if ((struct LUT_struct *)data == NULL) {
            return -EINVAL;
        }
        lutp = vctl->current_addside_lut;
        if (lutp != NULL) {
            if (test_bit(FPGA_BOARD_ADDSIDELUT_BUSY_BIT, &board->v_status)) {
                return -EBUSY;
            }
            else if (copy_to_user((struct LUT_struct *)data,
                             lutp, sizeof(struct LUT_struct)) != 0)
                return -EFAULT;
        }
        else {
            return -EINVAL;
        }
        return 0;
    }

    case FPGA_IOC_SET_ADDSIDELUT:
    {
        struct video_param_io vio, *viop;

        if ((int *)data == NULL)
            return -EFAULT;

        viop = (struct video_param_io *) data;
        if (viop == NULL)
            return -EINVAL;

        if (copy_from_user(&vio, viop, sizeof(struct video_param_io)))
            return -EFAULT;

        vctl->current_addside_lut = lutp = find_lut_entry(board,
                                                          vio.hsyncc,
                                                          vio.amvreg,
                                                          vio.amhreg,
                                                          vio.amtreg,
                                                          vio.duallink,
                                                          0);
        update_overlay_info(board);

        if (vctl->current_addside_lut != NULL) {
            if (copy_to_user((struct LUT_struct *) &(viop->alut),
                        lutp, sizeof(struct LUT_struct)) != 0)
                return -EFAULT;
            /* Initialize delays to values for addside LUT */
            vctl->hd_shift = lutp->a_hshift;
            vctl->vd_shift = lutp->a_vshift;
        }
        else
            return -EFAULT;

        return 0;
    }

    case FPGA_IOC_GET_DROPSIDELUT:
    {
        if ((struct LUT_struct *)data == NULL)
            return -EINVAL;

        if (((lutp = vctl->current_dropside_lut) != NULL) &&
                !test_bit(FPGA_BOARD_DROPSIDELUT_BUSY_BIT, &board->v_status)) {
            if (copy_to_user((struct LUT_struct *)data,
                             lutp, sizeof(struct LUT_struct)) != 0)
                return -EFAULT;
        }
        else
            return -EINVAL;
        return 0;
    }

    case FPGA_IOC_SET_DROPSIDELUT:
    {
        if ((int *)data == NULL) {
            return -EFAULT;
        }
        if (vctl->current_dropside_lut == NULL)
            vctl->current_dropside_lut =
                kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
        if (copy_from_user(vctl->current_dropside_lut,
                (struct LUT_struct *) data,
                sizeof(struct LUT_struct)) != 0) {
            return -EFAULT;
        }
        return 0;
    }

    case FPGA_IOC_GET_DISPLAYLUT:
    {
        if ((struct LUT_struct *)data == NULL) {
            return -EINVAL;
        }
        lutp = vctl->current_display_lut;
        if (lutp != NULL) {
            if (copy_to_user((struct LUT_struct *)data,
                        lutp, sizeof(struct LUT_struct)) != 0) {
                return -EFAULT;
            }
        }
        else {
            return -EINVAL;
        }
        return 0;
    }

    case FPGA_IOC_SET_DISPLAYLUT:
    {
        if ((int *)data == NULL) {
            return -EFAULT;
        }
        if (vctl->current_display_lut == NULL)
            vctl->current_display_lut =
                kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
        if (copy_from_user(vctl->current_display_lut,
                (struct LUT_struct *) data,
                sizeof(struct LUT_struct)) != 0) {
            return -EFAULT;
        }

        /* Initialize delays to values for display LUT */
        vctl->hd_shift = vctl->current_display_lut->a_hshift;
        vctl->vd_shift = vctl->current_display_lut->a_vshift;

        update_overlay_info(board);
        return 0;
    }

    case FPGA_IOC_GET_AVFLAG:
    {
        if ((int *)data == NULL)
            return -EFAULT;
        ival = vctl->av_flag;
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    }

    case FPGA_IOC_SET_AVFLAG:
    {
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data,
                    sizeof(unsigned int)) != 0) {
            return -EFAULT;
        }
        if (is_plx_device(board)) {
            // Set Connection LEDs
            if ((ival & AVFLAG_CONNECTIONLED) !=
                    (vctl->av_flag & AVFLAG_CONNECTIONLED)) {
                if (ival & AVFLAG_CONNECTIONLED)
                    or_in_fpga_val(board, FPGA_XP200LEDREG, XP200LEDREG_CONN);
                else
                    and_in_fpga_val(board, FPGA_XP200LEDREG, ~XP200LEDREG_CONN);
            }
            // Set addside latency control and slice registers
            if (board->v_type == HARDWARE_V2D_TX &&
                vctl->current_addside_lut != NULL &&
                (ival & AVFLAG_LATENCY) != (vctl->av_flag & AVFLAG_LATENCY)) {
                unsigned long vact, thres0;
                int slicereg = get_fpga_val(board, FPGA_ANSLICEREG);

                vact = vctl->current_addside_lut->a_vres / 16 * 16;
                thres0 = (vctl->current_addside_lut->a_hres * vact) / 8 / 2 - 1;

                set_fpga_val(board, FPGA_ALATTHRES0, thres0);
                set_fpga_val(board, FPGA_ALATCONTROL, 0);

                if (ival & AVFLAG_LATENCY) {
                    set_fpga_val(board, FPGA_ALATSLICE3, 0);
                    set_fpga_val(board, FPGA_ALATSLICE4, 0);
                    set_fpga_val(board, FPGA_ALATSLICE5, 0);
                    set_fpga_val(board, FPGA_ALATSLICE0, slicereg / 4 - 1);
                    set_fpga_val(board, FPGA_ALATSLICE1, slicereg / 2 - 1);
                    set_fpga_val(board, FPGA_ALATSLICE2, slicereg * 3 / 4 - 1);
                    set_fpga_val(board, FPGA_ALATCONTROL, 0x00ff);
                }
                else {
                    set_fpga_val(board, FPGA_ALATSLICE0, 0);
                    set_fpga_val(board, FPGA_ALATSLICE1, 0);
                    set_fpga_val(board, FPGA_ALATSLICE2, 0);
                }
            }
            vctl->av_flag = ival;
        }
        return 0;
    }

    case FPGA_IOC_RESET_FPGA:
        {
            /************************************************************
             * We get the hardware type directly from the FPGA since    *
             * the "hardware_type" variable for the board  may not have *
             * been set yet when we are here                            *
             ************************************************************/
            unsigned short prod;

            prod = (get_fpga_val(board, FPGA_IDREG) & 0xff00) >> 8;

            if (prod == PROD_DFF_TX)
                set_fpga_val(board, FPGA_ARESETREG, 1);
            else
                set_fpga_val(board, FPGA_DRESETREG, 1);
            udelay(1000);

            if (prod == PROD_DFF_TX)
                set_fpga_val(board, FPGA_ARESETREG, 0);
            else
                set_fpga_val(board, FPGA_DRESETREG, 0);
        }

        break;

    case FPGA_IOC_GET_VID_AVL:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = video_available(board);
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_VID_AVL:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        oval = vctl->force_no_video;
        vctl->force_no_video = ival;
        if (oval != ival)
            flow_control_addside_video_reset(board);

        /* return previous value */
        if (copy_to_user((unsigned char *)data, &oval, sizeof(int)) != 0)
            return -EFAULT;
        return 0;


    case FPGA_IOC_GET_AMQUANT:
        if ((int *)data == NULL)
            return -EFAULT;
        sval = get_fpga_val(board, FPGA_AQUANT);


        if (copy_from_user(&ival, (unsigned char *)data,
                           sizeof(unsigned int)) != 0)
            return -EFAULT;
        if (ival & 0x8000) {
            sval &= MQUANT_LOW_MASK;
            ival = (sval >> MQUANT_LOW_SHIFT);
        }
        else {
            sval &= MQUANT_MASK;
            ival = (sval >> MQUANT_SHIFT);
        }

        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_AMQUANT:
        if ((int *)data == NULL)
            return -EFAULT;
        if (vctl->noise_thres_mode == 0) {
            break;
        }
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        sval = get_fpga_val(board, FPGA_AQUANT);
        if (ival & 0x8000) {
            sval &= ~MQUANT_LOW_MASK;
            sval |= ((ival & 0x0f) << MQUANT_LOW_SHIFT);
            set_fpga_val(board, FPGA_AQUANT, sval);
        }
        else {
            sval &= ~MQUANT_MASK;
            sval |= ((ival & 0x0f) << MQUANT_SHIFT);
            set_fpga_val(board, FPGA_AQUANT, sval);
        }

        return 0;

    case FPGA_IOC_GET_ADCQUANT:
        if ((int *)data == NULL)
            return -EFAULT;
        sval = get_fpga_val(board, FPGA_AQUANT);

        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        if (ival & 0x8000) {
            sval &= DCQUANT_LOW_MASK;
            ival = (sval >> DCQUANT_LOW_SHIFT);
        }
        else {
            sval &= DCQUANT_MASK;
            ival = (sval >> DCQUANT_SHIFT);
        }

        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_ADCQUANT:
        if ((int *)data == NULL)
            return -EFAULT;
        if (vctl->noise_thres_mode == 0) {
            break;
        }

        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;

        sval = get_fpga_val(board, FPGA_AQUANT);

        if (ival & 0x8000) {
            sval &= ~DCQUANT_LOW_MASK;
            sval |= ((ival & 0x0f) << DCQUANT_LOW_SHIFT);
            set_fpga_val(board, FPGA_AQUANT, sval);
        }
        else {
            sval &= ~DCQUANT_MASK;
            sval |= ((ival & 0x0f) << DCQUANT_SHIFT);
            set_fpga_val(board, FPGA_AQUANT, sval);

        }

        return 0;

    case FPGA_IOC_SET_NOISETHRESMODE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival,
                       (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        oval = vctl->noise_thres_mode;
        vctl->noise_thres_mode = ival;
        if (copy_to_user((int *)data, &oval, sizeof(int)) != 0)
            return -EFAULT; /* return previous value */
        break;

    case FPGA_IOC_GET_ANOBPSLICE:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = get_fpga_val(board, FPGA_ANOBREG);
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_ANOBPSLICE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        if (ival == 0) {
            vctl->force_anobreg = 0;
            return 0;
        }
        vctl->force_anobreg = 1;
        set_fpga_val(board, FPGA_ANOBREG, ival);
        {
            int tmpval = (get_fpga_val(board, FPGA_AVHAREG) *
                          get_fpga_val(board, FPGA_AVVAREG))/(64 * ival);
            set_fpga_val(board, FPGA_ANSLICEREG, tmpval);
            vctl->num_slices_per_frame = tmpval;
        }

        flow_control_addside_video_reset(board);
        return 0;

    case FPGA_IOC_GET_ACOMPMODE:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = COMPMODE_NONE;
        sval = get_fpga_val(board, FPGA_ACREG);
        if (sval & ACREG_444_SEL)
            ival = COMPMODE_444;
        if (is_plx_device(board) && (sval & ACREG_MOTION_444_SEL))
            ival = COMPMODE_MOTION_444;
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_ACOMPMODE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;

        sval = get_fpga_val(board, FPGA_ACREG);

        if (is_plx_device(board)) {
            if (ival == COMPMODE_MOTION_444)
                oval = (ACREG_444_SEL | ACREG_MOTION_444_SEL);
            else if (ival == COMPMODE_444)
                oval = ACREG_444_SEL;
            else
                oval = 0;
        }
        else {
            if (ival == COMPMODE_444)
                oval = ACREG_444_SEL;
            else
                oval = 0;
        }
        if ((sval & (ACREG_444_SEL|ACREG_MOTION_444_SEL)) != oval) {

            if (oval == (ACREG_444_SEL|ACREG_MOTION_444_SEL)) {
                or_in_fpga_val(board, FPGA_ACREG,
                        (ACREG_444_SEL | ACREG_MOTION_444_SEL));
            }
            else if (oval == ACREG_444_SEL) {
                or_in_fpga_val(board, FPGA_ACREG, ACREG_444_SEL);
                if (is_plx_device(board))
                    and_in_fpga_val(board, FPGA_ACREG, ~ACREG_MOTION_444_SEL);
            }
            else {
                and_in_fpga_val(board, FPGA_ACREG, ~ACREG_444_SEL);
                if (is_plx_device(board))
                    and_in_fpga_val(board, FPGA_ACREG, ~ACREG_MOTION_444_SEL);
            }
            if (vctl->current_addside_lut)
                configure_addside(board, vctl->current_addside_lut);
        }
        return 0;

    case FPGA_IOC_GET_DCOMPMODE:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = (get_fpga_val(board, FPGA_DCREG) & DCREG_444_SEL) != 0;
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_DCOMPMODE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;

        sval = (get_fpga_val(board, FPGA_DCREG) & DCREG_444_SEL) ? 1 : 0;

        if (sval == ival)
            return 0;

        if (!is_plx_device(board)) {
            if (ival == 0)
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_444_SEL & 0xffff);
            else
                or_in_fpga_val(board, FPGA_DCREG, DCREG_444_SEL);
        }

        return 0;

    case FPGA_IOC_SET_ASLICEDROP:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;

        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG, "Forcing I-Frame\n");

        if (board->v_pci_dev_type == PLX_DEVICE) {
            int retries;
            /***************************************************
             * On XP, wait till iframe valid bit in the iframe *
             * valid register is reset, then select frame      *
             * based count & iframe valid bit, progarm iframe  *
             * count to 'ival+1' so we get 'ival' I frames.    *
             ***************************************************/
            for (retries = 0; retries < 3; retries++) {
                if (get_fpga_val(board, FPGA_IFRM_CTLSTAT) &
                        IFRM_CTLSTAT_VALID_BIT)
                    udelay(10);
                else
                    break;
            }
            if (retries >= 3) {
                return -EFAULT;
            }
            or_in_fpga_val(board, FPGA_IFRM_RESET, IFRM_RESET_FRAME);
            udelay(10);
            and_in_fpga_val(board, FPGA_IFRM_RESET, ~IFRM_RESET_FRAME);
            set_fpga_val(board, FPGA_IFRM_FRMCNT, ival + 1);
            or_in_fpga_val(board, FPGA_IFRM_SELECT, IFRM_SELECT_FRAME);
            set_fpga_val(board, FPGA_IFRM_CTLSTAT, IFRM_CTLSTAT_VALID_BIT);
        }
        else {
            /**********************************
             * Always send one I frame on I50 *
             **********************************/
            sval = get_fpga_val(board, FPGA_ACREG);
            sval &= ~ACREG_IFRAME;
            set_fpga_val(board, FPGA_ACREG, sval);
            mdelay(1);
            sval |= ACREG_IFRAME;
            set_fpga_val(board, FPGA_ACREG, sval);
        }
        return 0;
    case FPGA_IOC_GET_CODEC_VERSION:
    {
        int codec_version;
        if (((int *)data == NULL))
            return -EFAULT;

        if ((codec_version = get_codec_version(board)) < 0)
             return -EFAULT;

        if (!(board->v_status & FPGA_BOARD_INITIALIZED))
            codec_version =  -EFAULT; //  CODEC_VERSION_UNKNOWN;

        switch (codec_version) {
        case 0:
          ival = CODEC_VERSION_STANDARD;
          break;
        case 1:
          ival = CODEC_VERSION_SCALABLE;
          break;
        case 2:
          ival = CODEC_VERSION_UNKNOWN;
          break;
        default:
          ival = CODEC_VERSION_UNKNOWN;
          break;
        }

        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;

        return 0;
    }
    case FPGA_IOC_GET_ACNOISET:
        if ((int *)data == NULL)
            return -EFAULT;
        sval = get_fpga_val(board, FPGA_ACNOISET);
        sval &= ACNOISE_MASK;
        ival = (sval >> ACNOISE_SHIFT);
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_ACNOISET:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        sval = get_fpga_val(board, FPGA_ACNOISET);
        sval &= ~ACNOISE_MASK;
        sval |= (ival << ACNOISE_SHIFT);
        set_fpga_val(board, FPGA_ACNOISET, sval);
        return 0;

    case FPGA_IOC_GET_DCNOISET:
        if ((int *)data == NULL)
            return -EFAULT;
        /* Use FPGA_ACNOISET if old I50 FPGA */
        if (is_plx_device(board) &&
                    (get_fpga_val(board, FPGA_IDREG) & 0x00ff) == 1) {
            sval = get_fpga_val(board, FPGA_ACNOISET);
            sval &= OLD_DCNOISE_MASK;
            ival = (sval >> OLD_DCNOISE_SHIFT);
        }
        else {
            sval = get_fpga_val(board, FPGA_DCNOISET);
            sval &= DCNOISE_MASK;
            ival = (sval >> DCNOISE_SHIFT);
        }
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_DCNOISET:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        /* Use FPGA_ACNOISET if old I50 FPGA */
        if (is_plx_device(board) &&
                    get_fpga_val(board, FPGA_IDREG) == 0xA201) {
            sval = get_fpga_val(board, FPGA_ACNOISET);
            sval &= ~OLD_DCNOISE_MASK;
            sval |= (ival << OLD_DCNOISE_SHIFT);
            set_fpga_val(board, FPGA_ACNOISET, sval);
        }
        else {
            sval = get_fpga_val(board, FPGA_DCNOISET);
            sval &= ~DCNOISE_MASK;
            sval |= (ival << DCNOISE_SHIFT);
            set_fpga_val(board, FPGA_DCNOISET, sval);
        }
        return 0;

    case FPGA_IOC_GET_DNOBPSLICE:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = get_fpga_val(board, FPGA_DNOBREG);
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_DNOBPSLICE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        if (ival == 0)
            return 0;
        /* For XP200 all registers are set in configure_dropside_fpga   */
        /* to minimize bad video when switching from one LUT to another */
        if (board->v_pci_dev_type == V2DI50_DEVICE) {
            int tmpval = (get_fpga_val(board, FPGA_DVHAREG) *
                    get_fpga_val(board, FPGA_DVVAREG))/(64 * ival);
            set_fpga_val(board, FPGA_DNOBREG, ival);
            set_fpga_val(board, FPGA_DNSLICEREG, tmpval);
        }
        return 0;

    case FPGA_IOC_SET_VIDEOCHECK:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        vctl->video_freeze_check = (ival) ? 1 : 0;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Channel %d. Setting video freeze check to %s\n",
                 board->v_id+1, (ival) ? "enabled" : "disabled");
        return 0;

    case FPGA_IOC_SET_EXTSYNC:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        vctl->external_sync = (ival) ? 1 : 0;
        if (board->v_pci_dev_type == PLX_DEVICE) {
            if (ival)   /* Set Bit 9 to 0 to use external sync */
                and_in_fpga_val(board, FPGA_ATESTREG, ~EXTSYNC_ENABLE);
            else        /* Set Bit 9 to 1 to use sync from ADC */
                or_in_fpga_val(board, FPGA_ATESTREG, EXTSYNC_ENABLE);
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                 "Channel %d. Setting EXTSYNC to %s\n",
                 board->v_id+1, (ival) ? "enabled" : "disabled");
        }
        return 0;

    case FPGA_IOC_SET_DEFAULT_CPMP:
        if ((int *)data == NULL || is_plx_device(board))
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        if (board->v_hardware_version == HARDWARE_VERSION_A) {
            if (ival >= 0 && ival <= 7)
                vctl->default_vco_cpmp = (DEFAULT_VCO_CPMP & 0xc7) | (ival << 3);
            else
                vctl->default_vco_cpmp = DEFAULT_VCO_CPMP;
            if (vctl->current_addside_lut == NULL)
                set_ad9887_val(VIF_VCO_CPMP, vctl->default_vco_cpmp);
        }
        else {
            if (ival >= 0 && ival <= 7)
                vctl->default_vco_cpmp = (DEFAULT_9887_VCO_CPMP & 0xe3) | (ival << 2);
            else
                vctl->default_vco_cpmp = DEFAULT_9887_VCO_CPMP;
            if (vctl->current_addside_lut == NULL)
                set_ad9887_val(VIF_9887_VCO_CPMP, vctl->default_vco_cpmp);
        }
        return 0;

    case FPGA_IOC_GET_DEFAULT_CPMP:
        if ((int *)data == NULL || is_plx_device(board))
            return -EFAULT;
        if (board->v_hardware_version == HARDWARE_VERSION_A) {
            if (vctl->default_vco_cpmp == DEFAULT_VCO_CPMP)
                ival = 0xff;
            else
                ival = (vctl->default_vco_cpmp & 0x38) >> 3;
        }
        else {
            if (vctl->default_vco_cpmp == DEFAULT_9887_VCO_CPMP)
                ival = 0xff;
            else
                ival = (vctl->default_vco_cpmp & 0x1c) >> 2;
        }
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_GET_INTERFACE:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = vctl->active_interface;
        if (copy_to_user((unsigned char *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_INTERFACE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data,
                    sizeof(unsigned int)) != 0) {
            return -EFAULT;
        }
        if (is_plx_device(board)) {
            vctl->active_interface = ival;
        }
        else if (board->v_hardware_version == HARDWARE_VERSION_DA) {
            oval = get_ad9887_val(VIF_9887_ACTIVE_INTF);
            if (ival == INTERFACE_DIGITAL)
                set_ad9887_val(VIF_9887_ACTIVE_INTF,
                               oval | VIFBIT_9887_DIGITAL_INTF_SELECT);
            else
                set_ad9887_val(VIF_9887_ACTIVE_INTF,
                               oval & ~VIFBIT_9887_DIGITAL_INTF_SELECT);
        }
        else
            return -EFAULT;
        return 0;

    case FPGA_IOC_FORCE_INTERFACE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ival, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        if (ival == INTERFACE_DIGITAL ||
            ival == INTERFACE_HV ||
            ival == INTERFACE_SOG)
            vctl->force_interface = ival;
        else
            vctl->force_interface = AUTO_INTERFACE;
        return 0;

    case FPGA_IOC_GET_DDC_EDID:
        {
            unsigned char buf[EDID_DATA_LEN];

            if ((int *)data == NULL || is_plx_device(board))
                return -EFAULT;
            if (ddcedid_read(DDC_EDID_DATA_START, buf, EDID_DATA_LEN) < 0)
                return -EFAULT;

            if (copy_to_user((unsigned char *)data, buf, EDID_DATA_LEN) != 0)
                return -EFAULT;
        }
        return 0;

    case FPGA_IOC_GET_OV_INFO:
        {
            struct ov_param_io oi;
            if ((int *)data == NULL || !is_plx_device(board))
                return -EFAULT;
            memset(&oi, 0, sizeof(struct ov_param_io));
            /* Return actual overlay flags and scales */
            oi.flags = vctl->oinfo.flags;
            oi.hscale = vctl->oinfo.hscale;
            oi.vscale = vctl->oinfo.vscale;
            /* Return actual offsets and transparency */
            oi.hoffset = (int) get_fpga_val(board, FPGA_DHSHIFT);
            oi.voffset = (int) get_fpga_val(board, FPGA_DVSHIFT);
            oi.oalpha = 100 -
                        (100 * get_fpga_val(board, FPGA_DOVLPALPHA1)) / 1024;
            oi.hcropleft = vctl->oinfo.hcropleft;
            oi.hcropright = vctl->oinfo.hcropright;
            oi.vcroptop = vctl->oinfo.vcroptop;
            oi.vcropbottom = vctl->oinfo.vcropbottom;
            if (copy_to_user((struct ov_param_io *)data,
                        &oi, sizeof(struct ov_param_io)) != 0)
                return -EFAULT;
        }
        break;
    case FPGA_IOC_GET_OV_PARAM:
        {
            struct ov_param_io *ov;
            if ((int *)data == NULL || !is_plx_device(board))
                return -EFAULT;
            ov = &vctl->ocfg;
            if (copy_to_user((struct ov_param_io *)data,
                            ov, sizeof(struct ov_param_io)) != 0)
            return -EFAULT;
        }
        break;
    case FPGA_IOC_SET_OV_PARAM:
        {
            struct ov_param_io *ov, a_ov;
            if ((int *)data == NULL || !is_plx_device(board))
                return -EFAULT;
            ov = &vctl->ocfg;
            if (copy_from_user(ov, (struct ov_param_io *)data,
                         sizeof(struct ov_param_io)) != 0) {
                return -EFAULT;
            }
            if (ov->setmask & OV_SET_PARAM_CROP) {
                if (ov->flags & OV_FLAG_CROP)
                    vctl->oinfo.flags |= OV_FLAG_CROP;
                else
                    vctl->oinfo.flags &= ~OV_FLAG_CROP;
                vctl->oinfo.hcropleft = ov->hcropleft;
                vctl->oinfo.hcropright = ov->hcropright;
                vctl->oinfo.vcroptop = ov->vcroptop;
                vctl->oinfo.vcropbottom = ov->vcropbottom;
            }
            if (ov->setmask & OV_SET_PARAM_OVERLAY) {
                update_overlay_info(board);
            }
            else {
                if (ov->setmask & OV_SET_PARAM_POSITION) {
                    compute_actual_overlay_params(board, &a_ov, ov);
                    set_fpga_val(board, FPGA_DHSHIFT, a_ov.hoffset);
                    set_fpga_val(board, FPGA_DVSHIFT, a_ov.voffset);
                }
                if (ov->setmask & OV_SET_PARAM_ALPHA) {
                    int oalpha = (1024 * ov->oalpha) / 100;
                    set_fpga_val(board, FPGA_DOVLPALPHA1, 1024 - oalpha);
                    set_fpga_val(board, FPGA_DOVLPALPHA2, oalpha);
                }
            }
        }
        break;
    case FPGA_IOC_FRAMECNT_ENABLE:
        if ((int *)data == NULL || !is_plx_device(board) ||
                                        board->v_type != HARDWARE_V2D_TX)
            return -EFAULT;
        if (copy_from_user(&ival, (int *)data, sizeof(int)) != 0)
            return -EFAULT;
        if (ival)
            /* Set bit 11 to 1 to enable frame/slice cnt w/o data */
            or_in_fpga_val(board, FPGA_ATESTREG, FRAMECNT_ENABLE);
        else
            /* Set bit 11 to 0 to disable frame/slice cnt w/o data */
            and_in_fpga_val(board, FPGA_ATESTREG, ~FRAMECNT_ENABLE);
         break;
    default:
        return -EINVAL;

    }
    return 0;
}

/***************************************************************
 * Configure Integrated Device Technology 18125 FIFO           *
 ***************************************************************/
static void v2_configure_id18125 (struct v2d_board *board,
                                  struct LUT_struct *lutp) {

    unsigned short extra = get_fpga_val(board, FPGA_PIXEXTRA);

    if (lutp == NULL)
        return;

    /***********************************************************
     * PSCW 05/15/06 - FPGA fix for Bugzilla# 377 makes it     *
     * necessary to set the FIFO full level to be "extra"      *
     * pixels more than the total pixels in the frame (i.e.    *
     * value for a_ffhwm in LUT). Note for FPGAs without the   *
     * 377 fix, the register value for FPGA_PIXEXTRA is 0.)    *
     ***********************************************************/
    id18125_write(lutp->a_fflwm, lutp->a_ffhwm + extra);
}

static void configure_addside(struct v2d_board *board, struct LUT_struct *lutp) {
    if (!is_plx_device(board)) {
        v2_configure_ad9888(board, lutp);
        v2_configure_id18125(board, lutp);
    }

    configure_addside_fpga(board, lutp);

    /* Force EOF to indicate video change */
    flow_control_addside_video_reset(board);
}

static int configure_dropside(struct v2d_board *board, struct LUT_struct *lutp) {
    struct v2d_ctl *vctl;
    int rc = 0;

    if ((vctl = board->v_ctl) == NULL)
        return -1;

    flow_control_dropside_video_reset(board);

    switch (board->v_pci_dev_type) {
        case PLX_DEVICE:
            rc = v4_configure_dropside_fpga(board, lutp);
            break;
        case V2DI50_DEVICE:
        default:
            v2_configure_ad9852(board, lutp);
            if ((rc = v2_configure_dropside_fpga(board, lutp)) == 0)
                board->v_ctl->add_drop_timer_count = 0;
            break;
    }
    return rc;
}


/***************************************************************
 * Configure ad9888 video AD converter based on add side video *
 * parameters.                                                 *
 ***************************************************************/
static void v2_configure_ad9888(struct v2d_board *board,
                                struct LUT_struct *lutp) {
    struct v2d_ctl *vctl;

    vctl = board->v_ctl;

    if (board->v_hardware_version == HARDWARE_VERSION_A) {
        /* V2O or I50 A. The ADC is AD9888 */
        set_ad9888_val(VIF_PHASE_ADJ, vctl->phase_shift);
        if (lutp == NULL || strncmp(lutp->comment, "INVALID", 7) == 0) {
            set_ad9888_val(VIF_PLL_MSB, DEFAULT_PLL_MSB);
            set_ad9888_val(VIF_PLL_LSB, DEFAULT_PLL_LSB);
            set_ad9888_val(VIF_VCO_CPMP, vctl->default_vco_cpmp);
        }
        else {
            set_ad9888_val(VIF_PLL_MSB, lutp->a_adcpll01);
            set_ad9888_val(VIF_PLL_LSB, lutp->a_adcpll02);
            set_ad9888_val(VIF_VCO_CPMP, lutp->a_adcvco03);
            set_ad9888_val(VIF_MODE_MUX, lutp->a_adcmc1);
            set_ad9888_val(VIF_TEST1, lutp->a_adcvco16);
            if (lutp->a_cspace)
                set_ad9888_val(VIF_SYNC_ONGRN, 0x7f);
            else
                set_ad9888_val(VIF_SYNC_ONGRN, 0x79);
            set_ad9888_val(VIF_RED_GAIN, lutp->a_rgain);
            set_ad9888_val(VIF_RED_OFFSET, (lutp->a_roffset << 1));
            set_ad9888_val(VIF_GRN_GAIN, lutp->a_ggain);
            set_ad9888_val(VIF_GRN_OFFSET, (lutp->a_goffset << 1));
            set_ad9888_val(VIF_BLU_GAIN, lutp->a_bgain);
            set_ad9888_val(VIF_BLU_OFFSET, (lutp->a_boffset << 1));
        }
    }
    else {                              /* I50 DA. The ADC is AD9887A */
        set_ad9887_val(VIF_9887_PHASE_ADJ, vctl->phase_shift);
        set_ad9888_val(VIF_9887_MODE_MUX, DEFAULT_9887_MODE_MUX);
        /* output format U/V, output mode 444 */
        set_ad9887_val(VIF_9887_CLAMP_PLACE, 0x04);
        set_ad9887_val(VIF_9887_CLAMP_DUR, 0x14);
        set_ad9887_val(VIF_9887_MODE_CNTL1, 0x60);

        if (lutp == NULL || strncmp(lutp->comment, "INVALID", 7) == 0) {
            set_ad9887_val(VIF_9887_PLL_MSB, DEFAULT_PLL_MSB);
            set_ad9887_val(VIF_9887_PLL_LSB, DEFAULT_PLL_LSB);
            set_ad9887_val(VIF_9887_VCO_CPMP, vctl->default_vco_cpmp);
        }
        else {
            set_ad9887_val(VIF_9887_PLL_MSB, lutp->a_adcpll01);
            set_ad9887_val(VIF_9887_PLL_LSB, lutp->a_adcpll02);
            set_ad9887_val(VIF_9887_VCO_CPMP, lutp->a_adcvco73);
            if (lutp->a_cspace)
                set_ad9887_val(VIF_9887_CLAMP_COAST, 0xd5);
            else
                set_ad9887_val(VIF_9887_CLAMP_COAST, 0xd0);
            set_ad9887_val(VIF_9887_RED_GAIN, lutp->a_rgain);
            set_ad9887_val(VIF_9887_RED_OFFSET, lutp->a_roffset << 1);
            set_ad9887_val(VIF_9887_GRN_GAIN, lutp->a_ggain);
            set_ad9887_val(VIF_9887_GRN_OFFSET, lutp->a_goffset << 1);
            set_ad9887_val(VIF_9887_BLU_GAIN, lutp->a_bgain);
            set_ad9887_val(VIF_9887_BLU_OFFSET, lutp->a_boffset << 1);
        }
    }
}

/* Monitor ad9888 for sync on green and composite sync */
static void v2_monitor_ad9888(struct v2d_board *board) {

    unsigned char sync_detect, sync_ctl, old_sync_ctl;
    unsigned char coast_ctl, old_coast_ctl;
    struct v2d_ctl *vctl;
    vctl = board->v_ctl;

    if (board->v_hardware_version == HARDWARE_VERSION_A) {

        vctl->active_interface = INTERFACE_HV;   /* default */

        coast_ctl = old_coast_ctl = get_ad9888_val(VIF_CLAMP_COAST);
        coast_ctl |= (VIFBIT_COAST_SELECT | VIFBIT_NO_PWRDN);
        if (coast_ctl != old_coast_ctl) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                     "Changing VIF_CLAMP_COAST   %x --> %x\n",
                     old_coast_ctl, coast_ctl);
            set_ad9888_val(VIF_CLAMP_COAST, coast_ctl);
        }
        old_sync_ctl = sync_ctl = get_ad9888_val(VIF_SYNC_CNTL);

        /* override auto control of hsync and vsync output */
        sync_ctl |= (VIFBIT_HSYNC_OVERRIDE | VIFBIT_VSYNC_OVERRIDE);
        sync_ctl &= ~(VIFBIT_HSYNC_SELECT | VIFBIT_VSYNC_SELECT);

        if (vctl->force_interface == AUTO_INTERFACE) {
            /* Detect what is happening on the sync inputs */
            sync_detect = get_ad9888_val(VIF_SYNC_DETECT);

            if ((sync_detect & VIFBIT_HSYNC_DETECT) == 0) { /* no hsync */
                /* Select Sync-On-Green for hsync output */
                sync_ctl |= VIFBIT_HSYNC_SELECT;
                vctl->active_interface = INTERFACE_SOG;
            }

            if ((sync_detect & VIFBIT_VSYNC_DETECT) == 0) { /* no vsync */
                /* Select sync seperator for vsync output */
                sync_ctl |= VIFBIT_VSYNC_SELECT;
                vctl->active_interface = INTERFACE_SOG;
            }
            if (old_sync_ctl != sync_ctl) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                         "Changing VIF_SYNC_CNTL   %x --> %x\n",
                         old_sync_ctl, sync_ctl);
                set_ad9888_val(VIF_SYNC_CNTL, sync_ctl);
            }

            if (vctl->external_sync == 1)
                /* Set FPGA to always use external syncs */
                or_in_fpga_val(board, FPGA_TSTREG, EXTSYNC_ENABLE);
            else
                /* Set FPGA to always use internal syncs */
                and_in_fpga_val(board, FPGA_TSTREG, ~EXTSYNC_ENABLE);
        }
        else {
            if (vctl->force_interface == INTERFACE_SOG)
                sync_ctl |= (VIFBIT_HSYNC_SELECT | VIFBIT_VSYNC_SELECT);
            if (old_sync_ctl != sync_ctl) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                         "Changing VIF_SYNC_CNTL   %x --> %x\n",
                         old_sync_ctl, sync_ctl);
                set_ad9888_val(VIF_SYNC_CNTL, sync_ctl);
            }
            vctl->active_interface = INTERFACE_HV;
        }
    }

    else {

        vctl->active_interface = INTERFACE_HV;   /* default */
        old_sync_ctl = sync_ctl = get_ad9887_val(VIF_9887_ACTIVE_INTF);

        /* Set normal (i.e. not powered down) state (Bug# 2288)*/
        sync_ctl |= VIFBIT_9887_NO_PWRDN;

        if (vctl->force_interface == AUTO_INTERFACE) {
            /* Detect what is happening on the sync inputs */
            sync_detect = get_ad9887_val(VIF_9887_SYNC_DETECT);

            /* Select active interface */
            sync_ctl |= VIFBIT_9887_ACTIVE_INTF_OVERRIDE;
            if ((sync_detect & VIFBIT_9887_HSYNC_DETECT) &&
                (sync_detect & VIFBIT_9887_VSYNC_DETECT)) {
                sync_ctl &= ~VIFBIT_9887_DIGITAL_INTF_SELECT;
            }
            else if (sync_detect & VIFBIT_9887_DIGITAL_INTF_DETECT) {
                sync_ctl |= VIFBIT_9887_DIGITAL_INTF_SELECT;
                vctl->active_interface = INTERFACE_DIGITAL;
            }
            else {
                sync_ctl &= ~VIFBIT_9887_DIGITAL_INTF_SELECT;
                vctl->active_interface = INTERFACE_SOG;
            }

            if ((get_fpga_val(board, FPGA_IDREG) & 0x00ff) >= 9) {
                if (vctl->active_interface == INTERFACE_DIGITAL ||
                    vctl->active_interface == INTERFACE_SOG)
                    /* Set FPGA to use internal syncs */
                    and_in_fpga_val(board, FPGA_TSTREG, ~EXTSYNC_ENABLE);
                else if (vctl->external_sync)
                    /* Set FPGA to use external syncs */
                    or_in_fpga_val(board, FPGA_TSTREG, EXTSYNC_ENABLE);
                else
                    /* Set FPGA to use internal syncs */
                    and_in_fpga_val(board, FPGA_TSTREG, ~EXTSYNC_ENABLE);
            }

            /* Override auto control of hsync and vsync output */
            sync_ctl |= (VIFBIT_9887_HSYNC_OVERRIDE |
                         VIFBIT_9887_VSYNC_OVERRIDE);
            sync_ctl &= ~(VIFBIT_9887_HSYNC_SELECT |
                         VIFBIT_9887_VSYNC_SELECT);

            if ((sync_detect & VIFBIT_9887_HSYNC_DETECT) == 0) { /* no hsync */
                /* Select Sync-On-Green for hsync output */
                sync_ctl |= VIFBIT_9887_HSYNC_SELECT;
            }

            if ((sync_detect & VIFBIT_9887_VSYNC_DETECT) == 0) { /* no vsync */
                /* Select sync seperator for vsync output */
                sync_ctl |= VIFBIT_9887_VSYNC_SELECT;
            }
            if (old_sync_ctl != sync_ctl) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                         "Changing VIF_9887_SYNC_CNTL   %x --> %x\n",
                         old_sync_ctl, sync_ctl);
                set_ad9887_val(VIF_9887_ACTIVE_INTF, sync_ctl);
            }
        }
        else {
            /* Select active interface and not powered down state (Bug#2288) */
            sync_ctl |= (VIFBIT_9887_HSYNC_OVERRIDE |
                         VIFBIT_9887_VSYNC_OVERRIDE |
                         VIFBIT_9887_ACTIVE_INTF_OVERRIDE);
            sync_ctl &= ~(VIFBIT_9887_HSYNC_SELECT | VIFBIT_9887_VSYNC_SELECT);
            if (vctl->force_interface == INTERFACE_HV) {
                sync_ctl &= ~VIFBIT_9887_DIGITAL_INTF_SELECT;
                vctl->active_interface = INTERFACE_HV;
            }
            else if (vctl->force_interface == INTERFACE_DIGITAL) {
                sync_ctl |= VIFBIT_9887_DIGITAL_INTF_SELECT;
                vctl->active_interface = INTERFACE_DIGITAL;
            }
            else {
                sync_ctl &= ~VIFBIT_9887_DIGITAL_INTF_SELECT;
                sync_ctl |= (VIFBIT_9887_HSYNC_SELECT |
                             VIFBIT_9887_VSYNC_SELECT);
                vctl->active_interface = INTERFACE_SOG;
            }
            if (old_sync_ctl != sync_ctl) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                         "Changing VIF_SYNC_CNTL   %x --> %x\n",
                         old_sync_ctl, sync_ctl);
                set_ad9887_val(VIF_9887_ACTIVE_INTF, sync_ctl);
            }
        }
    }
}

/****************************************************************
 * Configure ad9852 video AD converter based on drop side video *
 * parameters.                                                  *
 ****************************************************************/
static void v2_configure_ad9852(struct v2d_board *board,
                                struct LUT_struct *lutp)
{
    if (lutp == NULL)
        return;

    /******************************
     * Disable the Dropside Video *
     ******************************/
    and_in_fpga_val(board, FPGA_DCREG, ~(DCREG_DROP_ENABLE|DCREG_VID_ENABLE));

    /*****************
     * Reset the DDS *
     *****************/
    reset_ad9852(board);

    /****************************************
     * Configure DDS clock frequency values *
     ****************************************/
    tweak_frequency(board,
            (lutp->d_dds5 << 16) | (lutp->d_dds4 << 8) | lutp->d_dds3,
            (lutp->d_dds2 << 16) | (lutp->d_dds1 << 8) | lutp->d_dds0);

    /******************************
     * Enable the Dropside Video *
     ******************************/
    or_in_fpga_val(board, FPGA_DCREG, DCREG_DROP_ENABLE|DCREG_VID_ENABLE);
    mdelay(5);
};


/******************************************************************
 * Wait till drop side DCMs lock. Returns error if this takes too *
 * much time.                                                     *
 ******************************************************************/
static int wait_till_drop_pll_locked(struct v2d_board *board) {
    int count = 0;
    unsigned short regval;

    regval = (DSREG_SRAM_CLOCK_DCM_LOCKED |
              DSREG_VIDEO_CLOCK_DCM_LOCKED |
              DSREG_SONET_CLOCK_DCM_LOCKED);

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
        "Channel %d. Waiting for drop side DCM to be Locked ...\n",
        board->v_id+1);
    udelay(200);
    while (!((get_fpga_val(board, FPGA_DSREG) & regval) == regval)) {
        udelay(200);
        if (++count > 3)
            return -1;
    }
    return 0;
}

/*****************************************************************
 * Reset Drop Side 2nd SRAM DCM and wait till DCM locks. Returns *
 * error if this takes too much time.                            *
 ******************************************************************/
static int reset_and_wait_till_drop_sram2_pll_locked(struct v2d_board *board) {
    int count = 0;
    unsigned short regval;

    set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_DROP_RESET_SRAM2_DCM);

    regval = DSREG_SRAM2_CLOCK_DCM_LOCKED;

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
             "Channel %d. Waiting for drop side 2nd DCM to be Locked ...\n",
             board->v_id+1);
    udelay(200);
    while (!((get_fpga_val(board, FPGA_DSREG) & regval) == regval)) {
        udelay(200);
        if (++count > 3)
            return -1;
    }
    return 0;
}

/*****************************************************************
 * Wait till add side DCMs lock. Returns error if this takes too *
 * much time.                                                    *
 *****************************************************************/
static int wait_till_add_pll_locked(struct v2d_board *board) {
    int count = 0;
    unsigned short regval;
    unsigned short val;
    regval = (ASREG_VIDEO_CLOCK_DCM_LOCKED |
              ASREG_SONET_CLOCK_DCM_LOCKED);

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
             "Channel %d. Waiting for add side DCM to be Locked...\n",
             board->v_id+1);
    mdelay(1);
    while (!(((val = get_fpga_val(board, FPGA_ASREG)) & regval) == regval)) {
        mdelay(1);
        if (++count > 3)
            return -1;
    }
    return 0;
}

/************************************************************
 * Configure add side FPGA register based on add side video *
 * parameters (from current LUT table)                      *
 ************************************************************/
static void configure_addside_fpga(struct v2d_board *board,
                                   struct LUT_struct *lutp) {

    switch (board->v_pci_dev_type) {
        case V2DI50_DEVICE:
            v2_configure_addside_fpga(board, lutp);
            break;
        case PLX_DEVICE:
            v4_configure_addside_fpga(board, lutp);
            break;
        default :
            break;
    }
    return ;

}

static void v2_configure_addside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp) {
    int nval;
    struct v2d_ctl *vctl;
    unsigned short fpga_version = get_fpga_val(board, FPGA_IDREG) & 0x00ff;
    int enable_444;

    if (lutp == NULL)
        return;

    /**************************************************
     * Use FPGA_ACNOISET for coarse noise if old FPGA *
     * Otherwise use FPGA_DCNOISET for coarse noise,  *
     * FPGA_ACNOISET for fine noise.                  *
     **************************************************/
    if (fpga_version == 1) {
        nval = ((lutp->a_noise & LUT_ACNOISE_MASK) >> LUT_ACNOISE_SHIFT) <<
                ACNOISE_SHIFT;
        nval |= (((lutp->a_noise & LUT_DCNOISE_MASK) >> LUT_DCNOISE_SHIFT) &
                OLD_DCNOISE_MASK) << OLD_DCNOISE_SHIFT;
        set_fpga_val(board, FPGA_ACNOISET, (unsigned short) nval);
    }
    else {
        nval = ((lutp->a_noise & LUT_ACNOISE_MASK) >> LUT_ACNOISE_SHIFT) <<
                ACNOISE_SHIFT;
        set_fpga_val(board, FPGA_ACNOISET, nval);
        nval = (((lutp->a_noise & LUT_DCNOISE_MASK) >> LUT_DCNOISE_SHIFT) &
                DCNOISE_MASK) << DCNOISE_SHIFT;
        set_fpga_val(board, FPGA_DCNOISET, nval);
    }

    vctl = board->v_ctl;

    enable_444 = (get_fpga_val(board, FPGA_ACREG) & ACREG_444_SEL) != 0;

    /****************************
     * Set Addside LUT register *
     ****************************/
    set_fpga_val(board, FPGA_ALUTREG, lutp->a_alutreg);

    /**********************************************************
     * Or in a one if the table indicates frequency > 103 MHz *
     **********************************************************/
    set_fpga_val(board, FPGA_ACREG, ACREG_FIFO_XCLK);
    if ((get_fpga_val(board, FPGA_IDREG) & 0x00ff) >= 9)
        and_in_fpga_val(board, FPGA_ACREG, lutp->a_acreg);

    /*************************************
     * Setup Addside Audio FPGA register *
     *************************************/
    set_fpga_val(board, FPGA_AAFDREG, lutp->a_aafdreg); /* 46kHz audio */
    udelay(1);

    /**************
     * Reset DCMs *
     **************/
    set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM);
    udelay(1);

    /*************************************
     * Set Addside Horiz and Vert Delays *
     *************************************/
    {
        int hds,vds;
        /* Set initial delay before starting the vertical delay counter */
        if (fpga_version >= 9) {
            set_fpga_val(board, FPGA_AVVD_DELAY,
                         (lutp->a_hfp + lutp->a_hbp + lutp->a_hres)/ 2);
        }

        /* Do not apply negative delays to the FPGA */
        if (vctl->hd_shift >= lutp->a_avhdreg)
            hds = lutp->a_avhdreg - 1;
        else
            hds = vctl->hd_shift;
        if (vctl->vd_shift >= lutp->a_avvdreg)
            vds = lutp->a_avvdreg - 1;
        else
            vds = vctl->vd_shift;

        /***************************************************************
         * PSCW 02/16/07 - Add hsync width to delay for DVI for LUT    *
         * entries with positive horiz polarity (since DVI output from *
         * the ADC has opposite polarity to the LUT and anlog output   *
         * from the ADC always has postive polarity so DVI and analog  *
         * outputs have opposite polarity in this case).               *
         ***************************************************************/
        if (board->v_hardware_version == HARDWARE_VERSION_DA) {
            if ((lutp->d_dcreg & DCREG_HSYNC_POL) == 0) {
                if (vctl->active_interface == INTERFACE_DIGITAL) {
                    hds = ((hds + lutp->a_hsync) >= lutp->a_avhdreg) ?
                        (lutp->a_avhdreg - 1) : (hds + lutp->a_hsync);
                }
            }
        }

        set_fpga_val(board, FPGA_AVHDREG, lutp->a_avhdreg - hds);
        udelay(1);
        if ((get_fpga_val(board, FPGA_IDREG) & 0x00ff) <= 0x07)
            set_fpga_val(board, FPGA_AVVDREG, lutp->a_avvdreg - vds);
        else
            set_fpga_val(board, FPGA_AVVDREG, lutp->a_avvdreg - lutp->a_vsync - vds);
        udelay(1);

        /**************************************************************
         * Set start-of-frame delay register to (adjusted) vertical   *
         * back porch - vertical position - 1). Note 2.1-b06 sets it  *
         * to (adjusted) vertical back porch - 1 which causes no      *
         * video to be read if vertical position is a large enough    *
         * positive value. Also note the build_lut_entry() routine in *
         * configureLUT.c checks that the adjusted back porch >= 1.   *
         **************************************************************/
        if (fpga_version >= 9) {
            if (lutp->a_avvdreg - lutp->a_vsync - 1 - vds <= 0)
                set_fpga_val(board, FPGA_SOF_DELAY, 1);
            else
                set_fpga_val(board, FPGA_SOF_DELAY,
                             lutp->a_avvdreg - lutp->a_vsync - 1 - vds);
        }
        udelay(1);
    }

    /*****************************************
     * Set Addside Horiz and Vert Resolution *
     *****************************************/
    set_fpga_val(board, FPGA_AVHAREG, lutp->a_avhareg);
    udelay(1);
    set_fpga_val(board, FPGA_AVVAREG, lutp->a_avvareg);
    udelay(1);


    {
        /**********************************************
         * Figure out slice size and blocks per slice *
         **********************************************/
        int tmpval, encodedwidth, encodedheight;
        if (compute_i50_slice_size(lutp->a_avhareg, lutp->a_avvareg, &tmpval,
                    &encodedwidth, &encodedheight) < 0)
            tmpval = DEFAULT_NOBPSLICE;

        if (vctl->force_anobreg == 0) {
            set_fpga_val(board, FPGA_ANOBREG, tmpval);
            udelay(1);
        }
        tmpval = (lutp->a_avhareg * lutp->a_avvareg)/(64 * tmpval);
        if (vctl->force_anobreg == 0) {
            set_fpga_val(board, FPGA_ANSLICEREG, tmpval);
            udelay(1);
        }
        vctl->num_slices_per_frame = tmpval;
    }

    /***************************
     * Set DVI hot plug detect *
     ***************************/
    or_in_fpga_val(board, FPGA_VIDEOINPUT, VIDEOINPUT_DVIBIT);

    /**********************************
     * Set Stereo Mode on the Addside *
     **********************************/
    if (lutp->a_stereo) {
        or_in_fpga_val(board, FPGA_ACREG, ACREG_STEREO);
        udelay(1);
    }

    /**************************************
     * Set Addside LUT register (again?)  *
     **************************************/
    set_fpga_val(board, FPGA_ALUTREG, lutp->a_alutreg);
    udelay(1);

    /***************************
     * Enable Add Side of FPGA *
     ***************************/
    or_in_fpga_val(board, FPGA_ACREG, (ACREG_ADD_ENABLE | ACREG_VID_ENABLE |
                                       ACREG_AUD_ENABLE | ACREG_LAN_ENABLE));

    udelay(1);

    /****************************
     * Disable Add Side of FPGA *
     ****************************/
    and_in_fpga_val(board, FPGA_ACREG, ~ACREG_VID_ENABLE);
    udelay(1);

    /**************
     * Reset DCMs *
     **************/
    set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM);
    mdelay(1);


    /************************
     * Wait for PLL to lock *
     ************************/
    if (wait_till_add_pll_locked(board) != 0)
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_WARNING,
                 "Addside DCM remained unlocked.\n");

    if (enable_444 != 0) {
        or_in_fpga_val(board, FPGA_ACREG, ACREG_444_SEL);
        mdelay(1);
    }

    if (lutp->a_cspace == 0)
        and_in_fpga_val(board, FPGA_VIDEOINPUT, ~VIDEOINPUT_YUVBIT);
    else
        or_in_fpga_val(board, FPGA_VIDEOINPUT, VIDEOINPUT_YUVBIT);


    /***************************
     * Enable Add Side of FPGA *
     ***************************/
    or_in_fpga_val(board, FPGA_ACREG, ACREG_VID_ENABLE);

}

static void v4_configure_addside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp) {

    struct v2d_ctl *vctl;
    int enable_444, enable_motion444, hds, vds;
    unsigned short nval, avhareg, avvareg, avhdreg, avvdreg;
    unsigned long thres0, vact;
    int codecversion;

    vctl = board->v_ctl;

    if (lutp == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
            "Channel %d No addside LUT. Resetting addside video DCM.\n",
            board->v_id + 1);
        set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM);
        return;
    }

    vact = (unsigned long) lutp->a_vres / 16 * 16;
    thres0 = ((unsigned long) lutp->a_hres * vact) / 8 / 2 - 1;

    /* PSCW 05/29/08 - As a temporary fix for vert shift for  */
    /* 1920x1080 & 2560x1600, always set AVVDREG to 5 for DVI */
    if (!(get_fpga_val(board, FPGA_AMTREG) & AMTREG_HVSYNC)) {
        set_fpga_val(board, FPGA_SOF_DELAY, 5);
    }
    else {
        /* Set SOF delay register to (adjusted) VBP - 1. Note */
        /* "build_lut_entry()" checks if the adjusted VBP > 0.*/
        set_fpga_val(board, FPGA_SOF_DELAY,
        lutp->a_vbp - 1 - (lutp->a_vres - lutp->a_orig_vres));
    }

    /****************************************
     * Set DCNOISET and ACNOISET registers  *
     ****************************************/
    nval = ((lutp->a_noise & LUT_ACNOISE_MASK) >> LUT_ACNOISE_SHIFT) <<
                                                        ACNOISE_SHIFT;
    set_fpga_val(board, FPGA_ACNOISET, nval);
    nval = ((lutp->a_noise & LUT_DCNOISE_MASK) >> LUT_DCNOISE_SHIFT) <<
                                           DCNOISE_SHIFT;
    set_fpga_val(board, FPGA_DCNOISET, nval);

    enable_444 =
        ((get_fpga_val(board, FPGA_ACREG) & ACREG_444_SEL) != 0);

    enable_motion444 =
        ((get_fpga_val(board, FPGA_ACREG) & ACREG_MOTION_444_SEL) != 0);

    /****************************
     * Set Addside LUT register *
     ****************************/
    set_fpga_val(board, FPGA_ALUTREG, lutp->a_alutreg);

    /******************************************************************
     * Use pixel clock for FIFO xfer clock if pixel clock  > 47.5 MHz *
     ******************************************************************/
    set_fpga_val(board, FPGA_ACREG, lutp->a_acreg);

    /**************
     * Reset DCMs *
     **************/
    set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM);

    /****************************
     * Disable Add Side of FPGA *
     ****************************/
    and_in_fpga_val(board, FPGA_ACREG, ~(ACREG_ADD_ENABLE |
                                         ACREG_VID_ENABLE |
                                         ACREG_LAN_ENABLE |
                                         ACREG_IFRAME));

    /*************************
     * Take DCM out of reset *
     *************************/
    udelay(100);
    set_fpga_val(board, FPGA_CDCMRREG, 0);

    /************************
     * Wait for PLL to lock *
     ************************/
    if (wait_till_add_pll_locked(board) != 0) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_WARNING,
         "Channel %d Addside DCM remained unlocked.\n", board->v_id + 1);
    }
    else {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
         "Channel %d Addside DCM locked.\n", board->v_id+1);
    }

    if (vctl->active_interface & INTERFACE_DIGITAL) {
        mdelay(100);

        /********************
         * Enable DVI input *
         ********************/
        or_in_fpga_val(board, FPGA_ATESTREG, DVIINPUT_ENABLE);

        /******************************************************
         * Invert drop sync polarity if needed - Must do this *
         * before reading FPGA_AAVHDREG and FPGA_AAVVDREG     *
         ******************************************************/
        if (get_fpga_val(board, FPGA_DVIHPOLREG)) {/* neg pol - invert */
            or_in_fpga_val(board, FPGA_ATESTREG, DVIHPOL_INVERT);
            or_in_fpga_val(board, FPGA_DCREG, DCREG_HSYNC_POL);
        }
        else {                                    /* pos pol - don't invert */
            and_in_fpga_val(board, FPGA_ATESTREG, ~DVIHPOL_INVERT);
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_HSYNC_POL);
        }
        if (get_fpga_val(board, FPGA_DVIVPOLREG)){/* neg pol - invert */
            or_in_fpga_val(board, FPGA_ATESTREG, DVIVPOL_INVERT);
            or_in_fpga_val(board, FPGA_DCREG, DCREG_VSYNC_POL);
        }
        else {                                    /* pos pol - don't invert */
            and_in_fpga_val(board, FPGA_ATESTREG, ~DVIVPOL_INVERT);
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VSYNC_POL);
        }
        mdelay(50);

        /****************************************************
         * Set loopback to select data enable from DVI chip *
         ****************************************************/
        and_in_fpga_val(board, FPGA_ATESTREG, ~LPBK_DATA_ENABLE);

        /*****************************************************
         * Get H/V delay values from FPGA (this is done here *
         * so there's some delay after setting the sync pol. *
         * bits) but always use the LUT for H/V active.      *
         *****************************************************/
        if (vctl->av_flag & AVFLAG_DVIAUTO) {
            avhdreg = get_fpga_val(board, FPGA_AAVHDREG);
            avvdreg = get_fpga_val(board, FPGA_AAVVDREG);
            /*****************************************
             * Double the horiz delay in 48 bit mode *
             *****************************************/
            if (vctl->active_interface & INTERFACE_48BIT)
                avhdreg <<= 1;
        }
        else {
            avhdreg = lutp->a_avhdreg;
            avvdreg = lutp->a_avvdreg;
        }
        avhareg = lutp->a_hres;
        avvareg = lutp->a_vres;
    }
    else {
        /*********************
         * Disable DVI input *
         *********************/
        and_in_fpga_val(board, FPGA_ATESTREG, ~DVIINPUT_ENABLE);

        /**********************************************************
         * Set loopback to select data enable based on LUT timing *
         **********************************************************/
        or_in_fpga_val(board, FPGA_ATESTREG, LPBK_DATA_ENABLE);

        /***************************************
         * Invert drop sync polarity if needed *
         ***************************************/
        if (get_fpga_val(board, FPGA_ADCHPOLREG))  /* neg pol - invert */
            or_in_fpga_val(board, FPGA_DCREG, DCREG_HSYNC_POL);
        else                                       /* pos pol - don't invert */
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_HSYNC_POL);
        if (get_fpga_val(board, FPGA_ADCVPOLREG))  /* neg pol - invert */
            or_in_fpga_val(board, FPGA_DCREG, DCREG_VSYNC_POL);
        else                                       /* pos pol - don't invert */
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VSYNC_POL);

        /*********************************************
         * Use H/V active and delay values from LUT  *
         *********************************************/
        avhareg = lutp->a_avhareg;
        avvareg = lutp->a_avvareg;
        avhdreg = lutp->a_avhdreg;
        avvdreg = lutp->a_avvdreg;
    }

    /**********************************************
     * Set FPGA data enable to 0, i.e. always use *
     * external sync (Bugzille# 2973)             *
     **********************************************/
    and_in_fpga_val(board, FPGA_ATESTREG, ~FPGA_DATA_ENABLE);

    /****************************************
     * Set FPGA video mode to 48 or 24 bit  *
     ****************************************/
    if (vctl->active_interface & INTERFACE_48BIT) {
        and_in_fpga_val(board, FPGA_ATESTREG, ~FPGA_VIDEO_24BIT);
        if (vctl->active_interface & INTERFACE_DUALLINK) {
            or_in_fpga_val(board, FPGA_ATESTREG, VIDEO_DUALLINK);
        }
        else {
            and_in_fpga_val(board, FPGA_ATESTREG, ~VIDEO_DUALLINK);
        }
    }
    else {
        or_in_fpga_val(board, FPGA_ATESTREG, FPGA_VIDEO_24BIT);
        and_in_fpga_val(board, FPGA_ATESTREG, ~VIDEO_DUALLINK);
    }

    /******************************************************************
     * Set Threshold for AM(DVI)LREG change to trigger FPGA interrupt *
     ******************************************************************/
    set_fpga_val(board, FPGA_AMLTHRES, (lutp->a_vmargin>1)?(lutp->a_vmargin+1):2);

    /*****************************************
     * Compute adjusted Horiz and Ver Delays *
     *****************************************/
    if ((vctl->active_interface & INTERFACE_DIGITAL) &&
                (vctl->av_flag & AVFLAG_DVIAUTO)) {
        hds = vds = 0;
    }
    else {
        if (vctl->hd_shift >= avhdreg)
            hds = avhdreg - 1;
        else
            hds = vctl->hd_shift;
        if (vctl->vd_shift >= avvdreg)
            vds = avvdreg - 1;
        else
            vds = vctl->vd_shift;
    }

    /************************************************
     * Set Horiz and Vert Delays for Loopback Video *
     ************************************************/
    set_fpga_val(board, FPGA_AALHDELAY, avhdreg - hds);
    udelay(1);
    set_fpga_val(board, FPGA_AALVDELAY, avvdreg - vds);
    udelay(1);

    /***********************************************************
     * Compensate for FPGA timing differences for normal video *
     ***********************************************************/
    if (vctl->active_interface & INTERFACE_DIGITAL)
        avhdreg -= (vctl->active_interface & INTERFACE_48BIT) ? 4 : 2;
    else
        avhdreg -= (vctl->active_interface & INTERFACE_48BIT) ? 8 : 4;

    /**********************************************
     * Set Horiz and Vert Delays for Normal Video *
     **********************************************/
    set_fpga_val(board, FPGA_AVHDREG, avhdreg - hds);
    udelay(1);
    set_fpga_val(board, FPGA_AVVDREG, avvdreg - vds);
    udelay(1);

    /*****************************************
     * Set Addside Horiz and Vert Resolution *
     *****************************************/
    set_fpga_val(board, FPGA_AVHAREG, avhareg);
    udelay(1);
    set_fpga_val(board, FPGA_AVVAREG, avvareg);
    udelay(1);

    {
        /***********************************************
         * Figure out slice size and blocks per slice. *
         ***********************************************/
        int encodedwidth, encodedheight;
        int slicewidth, sliceheight, slicesize;
        int num_slice_per_frame;
        codecversion =  get_codec_version( board); // (get_fpga_val(board, FPGA_IDREG) & 0x00ff0000) >> 16;
        slicesize  = DEFAULT_NOBPSLICE;

        if (codecversion == 0) {    /* standard codec version */
          if (compute_slice_size(avhareg, avvareg, &slicesize, &encodedwidth,
                    &encodedheight) <  0);
        /*  LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                   " %s cannot compute slice size!! \n", __func__);*/
        }
        else {
            compute_svc_slice_size(avhareg, avvareg, &slicesize, &slicewidth,
                    &sliceheight, &encodedwidth, &encodedheight);

            /* set computed slicewidth & sliceheight for Version 2 codec */
            set_fpga_val(board, FPGA_SLICE_WIDTH, slicewidth);
            set_fpga_val(board, FPGA_SLICE_HEIGHT, sliceheight);

            /* set encoded width & encoded height for version 2 codec */
            set_fpga_val(board, FPGA_DOSDMMCTL3, encodedwidth);
            set_fpga_val(board, FPGA_DOSDMMCTL4, encodedheight);

        }

        if (vctl->force_anobreg == 0) {
            set_fpga_val(board, FPGA_ANOBREG, slicesize);
            udelay(1);
        }
        else {
            slicesize  = get_fpga_val(board, FPGA_ANOBREG);
        }
        num_slice_per_frame = (avhareg * avvareg)/(64 * slicesize);
        if (vctl->force_anobreg == 0) {
            set_fpga_val(board, FPGA_ANSLICEREG, num_slice_per_frame);
            udelay(1);
        }
        vctl->num_slices_per_frame = num_slice_per_frame;
    }

    /***************************
     * Set DVI hot plug detect *
     ***************************/
    or_in_fpga_val(board, FPGA_VIDEOINPUT, VIDEOINPUT_DVIBIT);

    /**********************************************************
     * Set Stereo Mode on Addside and Dropside (for loopback) *
     **********************************************************/
    if (lutp->a_stereo == 1) {        /* stereo == Yes */
        or_in_fpga_val(board, FPGA_ACREG, ACREG_STEREO);
        or_in_fpga_val(board, FPGA_DCREG, DCREG_STEREO);
    }
    else if (lutp->a_stereo == 0xffff) {/* stereo == Both */
        unsigned short amtreg = get_fpga_val(board, FPGA_AMTREG);
        if (!(amtreg & AMTREG_HVSYNC))
            amtreg = get_fpga_val(board, FPGA_AMDVITREG) & AMTREG_STEREO;
        if (amtreg & AMTREG_STEREO) {
            or_in_fpga_val(board, FPGA_ACREG, ACREG_STEREO);
            or_in_fpga_val(board, FPGA_DCREG, DCREG_STEREO);
        }
    }
    udelay(1);

    /*****************************************
     * Set Addside buffer threshold register *
     *****************************************/
    set_fpga_val(board, FPGA_ALATTHRES0, thres0);

    /***************************************************
     * Set Addside latency control and slice registers *
     ***************************************************/
    set_fpga_val(board, FPGA_ALATCONTROL, 0);
    set_fpga_val(board, FPGA_ALATSLICE3, 0);
    set_fpga_val(board, FPGA_ALATSLICE4, 0);
    set_fpga_val(board, FPGA_ALATSLICE5, 0);
    if (vctl->av_flag & AVFLAG_LATENCY) {
        set_fpga_val(board, FPGA_ALATSLICE0, vctl->num_slices_per_frame/4-1);
        set_fpga_val(board, FPGA_ALATSLICE1, vctl->num_slices_per_frame/2-1);
        set_fpga_val(board, FPGA_ALATSLICE2, vctl->num_slices_per_frame*3/4-1);
        set_fpga_val(board, FPGA_ALATCONTROL, 0x00ff);
    }
    else {
        set_fpga_val(board, FPGA_ALATSLICE0, 0);
        set_fpga_val(board, FPGA_ALATSLICE1, 0);
        set_fpga_val(board, FPGA_ALATSLICE2, 0);
    }

    /****************
     * Set 444 Mode *
     ****************/
    if (enable_444) {
        or_in_fpga_val(board, FPGA_ACREG, ACREG_444_SEL);
        mdelay(1);
    }
    if (enable_motion444) {
        or_in_fpga_val(board, FPGA_ACREG, ACREG_MOTION_444_SEL);
        mdelay(1);
    }

    /*******************
     * Set Color Space *
     *******************/
    if (lutp->a_cspace == 0)
        and_in_fpga_val(board, FPGA_VIDEOINPUT, ~VIDEOINPUT_YUVBIT);
    else
        or_in_fpga_val(board, FPGA_VIDEOINPUT, VIDEOINPUT_YUVBIT);

    /**************************************
     * Set Addside LUT register (again?)  *
     **************************************/
    set_fpga_val(board, FPGA_ALUTREG, lutp->a_alutreg);
    udelay(1);

    /**********************************************************
     * Enable Add Side of FPGA with no slice drop for 1 frame *
     **********************************************************/
    or_in_fpga_val(board, FPGA_ACREG, (ACREG_ADD_ENABLE |
                                       ACREG_VID_ENABLE |
                                       ACREG_AUD_ENABLE |
                                       ACREG_LAN_ENABLE |
                                       ACREG_IFRAME));
}

static int v2_configure_dropside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp) {
    struct v2d_ctl *vctl;
    int rc = 0;

    /***************************
     * Set Video LED on I50 RX *
     ***************************/
    if (lutp == NULL) {
        LEDblink(board, VIDEO_LED, LED_BLINK_FAST);
        return -1;
    }
    LEDon(board, VIDEO_LED);

    vctl = board->v_ctl;

    /*****************************
     * Clear all flags initially *
     *****************************/
    and_in_fpga_val(board, FPGA_DCREG, ~(DCREG_VID_ENABLE |
                                         DCREG_STEREO |
                                         DCREG_WAIT_FOR_SKIP |
                                         DCREG_VID_BYPASS |
                                         DCREG_HSYNC_POL |
                                         DCREG_VSYNC_POL));


    /**************************
     * Reset Drop Side DCMs.  *
     **************************/
    set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_DROP_RESET_VIDEO_DCM);

    mdelay(1);

    /********************************
     * Set Drop Side Audio Register *
     ********************************/
    set_fpga_val(board, FPGA_DAFDREG, lutp->d_dafdreg);

    /******************************************
     * Set Dropside Horiz and Vert Resolution *
     ******************************************/
    set_fpga_val(board, FPGA_DVHAREG , lutp->d_dvhareg);

    set_fpga_val(board, FPGA_DVVAREG , lutp->d_dvvareg);

    /*****************************
     * Set Dropside HFP, HS, HBP *
     *****************************/
/*     LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "HFP =%x\n", lutp->d_dvhfpreg); */
    set_fpga_val(board, FPGA_DVHFPREG, lutp->d_dvhfpreg);
    udelay(1);
/*     LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "HSYNC =%x\n", lutp->d_dvhsreg); */
    set_fpga_val(board, FPGA_DVHSREG, lutp->d_dvhsreg);
    udelay(1);
/*     LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "HBP =%x\n", lutp->d_dvhbpreg); */
    set_fpga_val(board, FPGA_DVHBPREG, lutp->d_dvhbpreg);
    udelay(1);

    /*****************************
     * Set Dropside VFP, VS, VBP *
     *****************************/
/*     LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "VFP =%x\n", lutp->d_dvvfpreg); */
    set_fpga_val(board, FPGA_DVVFPREG, lutp->d_dvvfpreg);
    udelay(1);
/*     LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "VSYNC =%x\n", lutp->d_dvvsreg); */
    set_fpga_val(board, FPGA_DVVSREG, lutp->d_dvvsreg);
    udelay(1);
/*     LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "VBP =%x\n", lutp->d_dvvbpreg); */
    set_fpga_val(board, FPGA_DVVBPREG, lutp->d_dvvbpreg);
    udelay(1);

    {
        int tmpval = get_fpga_val(board, FPGA_DNOBREG);

        if (tmpval > 0)
            tmpval = (lutp->d_dvhareg * lutp->d_dvvareg)/(64 * tmpval);
        else
            tmpval = 160;
/*      LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "Setting DNSLICEREG to %x\n",tmpval);  */
        set_fpga_val(board, FPGA_DNSLICEREG, tmpval);
    }

    /***************************************
     * Set Dropside Video Buffer Watermark *
     ***************************************/
    set_fpga_val(board, FPGA_DVBWREG, lutp->d_dvbwreg);

    mdelay(1);

    /************************
     * Wait for PLL to lock *
     ************************/
    if (wait_till_drop_pll_locked(board) != 0) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_WARNING,
            "Dropside DCM remained unlocked.\n");
        rc = -1;
    }
    else {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
            "Dropside DCM locked.\n");
    }


    /*****************************
     * Set stereo flag if needed *
     *****************************/
    if(lutp->a_stereo)
        or_in_fpga_val(board, FPGA_DCREG, (DCREG_STEREO | DCREG_WAIT_FOR_SKIP));

    /**********************************************************
     * Set video bypass, Hsync/vsync polarity flags as needed *
     **********************************************************/
    or_in_fpga_val(board, FPGA_DCREG, (lutp->d_dcreg &
                                       (DCREG_VID_BYPASS)));
/*                              DCREG_HSYNC_POL |  */
/*                              DCREG_VSYNC_POL))); */

    /**********************************************************
     * PSCW 082505 - Reset Drop Side 2nd SRAM DCM and wait    *
     * till the PLL locks.                                    *
     **********************************************************/
    if (lutp->d_dcreg & DCREG_VID_BYPASS) {
        if (reset_and_wait_till_drop_sram2_pll_locked(board) < 0) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_WARNING,
                     "Channel %d. Dropside 2nd SRAM DCM remained unlocked.\n",
                     board->v_id+1);
            rc = -1;
        }
    }

    {
        int tval;
        if (vctl->pix_adjust >= 0) {
            set_fpga_val(board, FPGA_DVHFPREG, lutp->d_dvhfpreg +
                    vctl->pix_adjust/2);
            udelay(1);
            set_fpga_val(board, FPGA_DVHBPREG, lutp->d_dvhbpreg +
                    vctl->pix_adjust/2 + vctl->pix_adjust%2);
            tweak_frequency(board, vctl->pix_tweak_hi, vctl->pix_tweak_lo);
        }
        else if (vctl->pix_adjust < 0) {
            tval = -vctl->pix_adjust;
            set_fpga_val(board, FPGA_DVHFPREG, lutp->d_dvhfpreg - (tval/2));
            udelay(1);
            set_fpga_val(board, FPGA_DVHBPREG, lutp->d_dvhbpreg -
                    (tval/2 + tval%2));
            tweak_frequency(board, vctl->pix_tweak_hi, vctl->pix_tweak_lo);
        }
    }

    /******************
     * Enable VID DAC *
     ******************/
    or_in_fpga_val(board, FPGA_DCREG,  DCREG_VID_DAC_ENABLE);

    /****************************
     * Enable Drop Side of FPGA *
     ****************************/
    or_in_fpga_val(board, FPGA_DCREG, (DCREG_VID_ENABLE |
                                       DCREG_AUD_ENABLE |
                                       DCREG_LAN_ENABLE));


    return rc;

}

static void compute_actual_overlay_params(struct v2d_board *board,
                                          struct ov_param_io *a_cfg,
                                          struct ov_param_io *cfg) {

    struct v2d_ctl *vctl = board->v_ctl;
    int srchres, srcvres, newhres, newvres, monhres, monvres;

    memcpy(a_cfg, cfg, sizeof(struct ov_param_io));

    /* Check for valid dropside and display LUTs */
    if (vctl->current_dropside_lut == NULL ||
            vctl->current_display_lut == NULL) {
        return;
    }

    /* Set actual overlay source to remote if no current addside LUT */
    if (vctl->current_addside_lut == NULL && cfg->flags & OV_FLAG_SRC_OVERLAY) {
       a_cfg->flags &= ~OV_FLAG_SRCMASK;
       a_cfg->flags |= OV_FLAG_SRC_REMOTE;
    }

    /* Compute actual overlay H/V scale for current display LUT */
    if (a_cfg->flags & OV_FLAG_SRC_LOCAL) {
        a_cfg->hscale = a_cfg->vscale = RX_MAX_SCALE_VAL;
        a_cfg->hoffset = a_cfg->voffset = 0;
    }
    else {
        srchres = vctl->current_dropside_lut->a_hres;
        srcvres = vctl->current_dropside_lut->a_vres;
        if (vctl->oinfo.flags & OV_FLAG_CROP) {
            if (srchres > vctl->oinfo.hcropleft + vctl->oinfo.hcropright)
                srchres -= (vctl->oinfo.hcropleft + vctl->oinfo.hcropright);
            if (srcvres > vctl->oinfo.vcroptop + vctl->oinfo.vcropbottom)
                srcvres -= (vctl->oinfo.vcroptop + vctl->oinfo.vcropbottom);
        }
        monhres = vctl->current_display_lut->a_hres;
        monvres = vctl->current_display_lut->a_vres;

        if (a_cfg->flags & OV_FLAG_HRES)
            a_cfg->hscale = (a_cfg->hres * RX_MAX_SCALE_VAL)/srchres;
        else if (a_cfg->flags & OV_FLAG_HSCALEDISPLAY)
            a_cfg->hscale = (cfg->hscale * monhres)/srchres;
        if (a_cfg->hscale > RX_MAX_SCALE_VAL)
            a_cfg->hscale = RX_MAX_SCALE_VAL;
        if (a_cfg->hscale < RX_MIN_SCALE_VAL)
            a_cfg->hscale = RX_MIN_SCALE_VAL;

        if (a_cfg->flags & OV_FLAG_VRES)
            a_cfg->vscale = (a_cfg->vres * RX_MAX_SCALE_VAL)/srcvres;
        else if (a_cfg->flags & OV_FLAG_VSCALEDISPLAY)
            a_cfg->vscale = (cfg->vscale * monvres)/srcvres;
        if (a_cfg->vscale > RX_MAX_SCALE_VAL)
            a_cfg->vscale = RX_MAX_SCALE_VAL;
        if (a_cfg->vscale < RX_MIN_SCALE_VAL)
            a_cfg->vscale = RX_MIN_SCALE_VAL;

        newhres = srchres * a_cfg->hscale / RX_MAX_SCALE_VAL;
        while ((newhres > monhres) && a_cfg->hscale > RX_MIN_SCALE_VAL) {
            a_cfg->hscale--;
            newhres = srchres * a_cfg->hscale / RX_MAX_SCALE_VAL;
        }

        newvres = srcvres * a_cfg->vscale / RX_MAX_SCALE_VAL;
        while ((newvres > monvres) && a_cfg->vscale > RX_MIN_SCALE_VAL) {
            a_cfg->vscale--;
            newvres = srcvres * a_cfg->vscale / RX_MAX_SCALE_VAL;
        }

        if (a_cfg->flags & OV_FLAG_KEEPASPECT) {
            if (a_cfg->hscale > a_cfg->vscale)
                a_cfg->hscale = a_cfg->vscale;
            if (a_cfg->vscale > a_cfg->hscale)
                a_cfg->vscale = a_cfg->hscale;
        }
        newhres = srchres * a_cfg->hscale / RX_MAX_SCALE_VAL;
        newvres = srcvres * a_cfg->vscale / RX_MAX_SCALE_VAL;

        if (a_cfg->flags & OV_FLAG_HPOSLOCATION) {
            if (a_cfg->hlocation == OV_RIGHT_ADJUSTED)
                a_cfg->hoffset = monhres - newhres;
            else if (a_cfg->hlocation == OV_LEFT_ADJUSTED)
                a_cfg->hoffset = 0;
            else
                a_cfg->hoffset = (monhres - newhres) / 2;
        }
        if (a_cfg->flags & OV_FLAG_VPOSLOCATION) {
            if (a_cfg->vlocation == OV_BOTTOM_ADJUSTED)
                a_cfg->voffset = monvres - newvres;
            else if (a_cfg->vlocation == OV_TOP_ADJUSTED)
                a_cfg->voffset = 0;
            else
                a_cfg->voffset = (monvres - newvres) / 2;
        }
    }

    return;
}

static void update_overlay_info(struct v2d_board *board) {

    struct v2d_ctl *vctl;

    if (board->v_pci_dev_type != PLX_DEVICE)
        return;

    if ((vctl = board->v_ctl) == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL,
            "Channel %d Control not initialized\n", board->v_id + 1);
        return;
    }

    compute_actual_overlay_params(board, &vctl->oinfo, &vctl->ocfg);
    return;
}

static int v4_configure_dropside_fpga(struct v2d_board *board,
                                      struct LUT_struct *lutp) {
    struct v2d_ctl *vctl;
    int rc = 0, enable444 = 0, enablestereo = 0, blocksinbank = 0;
    int hres, vres, hfp, hsync, hbp, vfp, vsync, vbp;
    int avhdreg, avvdreg, hds, vds, backgroundcolor = 0;
    int oalpha, srchres, srcvres, newhres, newvres;
    int hcropleft = 0, hcropright = 0, vcroptop = 0, vcropbottom = 0;
    int hcrop, vcrop, dummy_pixels, addr_in_line;

    vctl = board->v_ctl;
    if (vctl->current_display_lut == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                "Channel %d. NULL display LUT.\n", board->v_id+1);
        return -1;
    }
    if ((vctl->oinfo.flags & OV_FLAG_SRC_REMOTE) && lutp == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                "Channel %d. NULL dropside LUT for remote display.\n",
                board->v_id+1);
        return -1;
    }

    /*****************************************
     * Clear add & drop side flags initially *
     *****************************************/
    and_in_fpga_val(board, FPGA_DCREG, ~(DCREG_DROP_ENABLE |
                                         DCREG_VID_ENABLE |
                                         DCREG_STEREO));

    /********************************
     * Put add & drop DCMs in reset *
     ********************************/
    if (vctl->av_flag & AVFLAG_LOCALRESET) {
        and_in_fpga_val(board, FPGA_ACREG, ~(ACREG_ADD_ENABLE |
                                             ACREG_VID_ENABLE |
                                             ACREG_LAN_ENABLE));
        set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_ADD_RESET_VIDEO_DCM |
                                           CDCMRREG_ADD_RESET_SONET_DCM |
                                           CDCMRREG_DROP_RESET_VIDEO_DCM |
                                           CDCMRREG_DROP_RESET_SONET_DCM);
    }
    else {
        set_fpga_val(board, FPGA_CDCMRREG, CDCMRREG_DROP_RESET_SONET_DCM);
    }

    /***********************************************
     * Set dropside horiz and vert res and porches *
     ***********************************************/
    hres = vctl->current_display_lut->a_hres;
    vres = vctl->current_display_lut->a_vres;
    hfp = vctl->current_display_lut->a_hfp;
    hsync = vctl->current_display_lut->a_hsync;
    hbp = vctl->current_display_lut->a_hbp;
    vfp = vctl->current_display_lut->a_vfp;
    vsync = vctl->current_display_lut->a_vsync;
    vbp = vctl->current_display_lut->a_vbp;

    if ((vctl->oinfo.flags & OV_FLAG_SRC_REMOTE) ||
            ((vctl->active_interface & INTERFACE_DIGITAL) &&
            (vctl->av_flag & AVFLAG_DVIAUTO))) {
        hds = vds = 0;
    }
    else {
        if (vctl->hd_shift >= hbp)
            hds = hbp - 1;
        else
            hds = vctl->hd_shift;
        if (vctl->vd_shift >= vbp)
            vds = vbp - 1;
        else
            vds = vctl->vd_shift;
    }

    set_fpga_val(board, FPGA_DVHAREG, vctl->current_display_lut->a_orig_hres);
    udelay(1);
    set_fpga_val(board, FPGA_DVVAREG, vctl->current_display_lut->a_orig_vres);
    udelay(1);
    set_fpga_val(board, FPGA_DVHFPREG, hfp + hds);
    udelay(1);
    set_fpga_val(board, FPGA_DVHSREG, hsync);
    udelay(1);
    set_fpga_val(board, FPGA_DVHBPREG, hbp - hds);
    udelay(1);
    set_fpga_val(board, FPGA_DVVFPREG, vfp + vds);
    udelay(1);
    set_fpga_val(board, FPGA_DVVSREG, vsync);
    udelay(1);
    set_fpga_val(board, FPGA_DVVBPREG, vbp - vds);
    udelay(1);

    /****************
     * Set 444 Mode *
     ****************/
    if (lutp != NULL && lutp->d_dcreg & DCREG_444_SEL) {
        or_in_fpga_val(board, FPGA_DCREG, DCREG_444_SEL);
        enable444 = 1;
    }
    else {
        and_in_fpga_val(board, FPGA_DCREG, ~DCREG_444_SEL);
    }

    /***********************
     * Set Sync Polarities *
     ***********************/
    if (vctl->oinfo.flags & OV_FLAG_SRC_REMOTE) {
        if (vctl->current_display_lut->d_dcreg & DCREG_HSYNC_POL)
            or_in_fpga_val(board, FPGA_DCREG, DCREG_HSYNC_POL);
        else
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_HSYNC_POL);
        if (vctl->current_display_lut->d_dcreg & DCREG_VSYNC_POL)
            or_in_fpga_val(board, FPGA_DCREG, DCREG_VSYNC_POL);
        else
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VSYNC_POL);
    }

    /*********************************
     * Set dropside slices per frame *
     *********************************/
    if (lutp != NULL) {
        if (lutp->d_slicereg != 0) {
            /*********************************************************
             * Set slice size register to value from TX if it's good *
             *********************************************************/
            if ((lutp->a_hres*lutp->a_vres)%(64*lutp->d_slicereg) != 0) {
                lutp->d_slicereg = 0;
            }
            else {
                int tmpval = (lutp->a_hres*lutp->a_vres)/(64*lutp->d_slicereg);
                set_fpga_val(board, FPGA_DNSLICEREG, lutp->d_slicereg);
                set_fpga_val(board, FPGA_DNOBREG, tmpval);
                blocksinbank = tmpval * (lutp->d_slicereg / 4);
            }
        }
        if (lutp->d_slicereg == 0) {
            int i, tmpval = MAX_NOBPSLICE;
            /*********************************************
             * Compute slice per frame if not set in LUT *
             *********************************************/
            for (i = MAX_NOBPSLICE; i >= MIN_NOBPSLICE; i--) {
                if ((((lutp->a_hres * lutp->a_vres)/128) % i) == 0) {
                    tmpval = i;
                    break;
                }
            }
            if ((lutp->a_hres * lutp->a_vres)/(64 * tmpval) >
                    MAX_NUM_SLICES_PER_FRAME) {
                for (i = MAX_NOBPSLICE+1; i <= MAX_NOBPSLICE2; i++) {
                    if ((((lutp->a_hres * lutp->a_vres)/128) % i) == 0) {
                        tmpval = i;
                        break;
                    }
                }
            }
            blocksinbank = tmpval;
            set_fpga_val(board, FPGA_DNOBREG, tmpval);
            tmpval = (lutp->a_hres * lutp->a_vres)/(64 * tmpval);
            set_fpga_val(board, FPGA_DNSLICEREG, tmpval);
            blocksinbank *= (tmpval / 4);
        }
        else {
            /*************************************
             * Otherwise set it to value from TX *
             *************************************/
            int tmpval = (lutp->a_hres * lutp->a_vres)/(64 * lutp->d_slicereg);
            set_fpga_val(board, FPGA_DNSLICEREG, lutp->d_slicereg);
            set_fpga_val(board, FPGA_DNOBREG, tmpval);
            blocksinbank = tmpval * (lutp->d_slicereg / 4);
        }
    }

    /*****************************
     * Set stereo flag if needed *
     *****************************/
    if (lutp != NULL && lutp->a_stereo) {
        or_in_fpga_val(board, FPGA_DCREG, DCREG_STEREO);
        enablestereo = 1;
    }
    else {
        and_in_fpga_val(board, FPGA_DCREG, ~DCREG_STEREO);
    }

    /******************************************************************
     * Set threshold for AM(DVI)LREG change to trigger FPGA interrupt *
     ******************************************************************/
    set_fpga_val(board, FPGA_AMLTHRES, 3);

    /***********************************
     * Set dropside override registers *
     ***********************************/
    if ((vctl->oinfo.flags & OV_FLAG_SRC_REMOTE) &&
            (vctl->current_display_lut->d_ov_control & DOVCONTROL_ENABLE)) {
        backgroundcolor = DOVCONTROL_ENABLE |
            (vctl->current_display_lut->d_ov_control & 0xff);
    }
    set_fpga_val(board, FPGA_DOVCONTROL, backgroundcolor);

    /******************************************
     * Set dropside shift and scale registers *
     ******************************************/
    if (vctl->oinfo.flags & OV_FLAG_SRC_LOCAL) {
        srchres = newhres = hres;
        srcvres = newvres = vres;
        vctl->oinfo.hoffset = vctl->oinfo.voffset = 0;
    }
    else {
        srchres = (lutp) ? lutp->a_hres : 0;
        srcvres = (lutp) ? lutp->a_vres : 0;
        newhres = srchres * vctl->oinfo.hscale / RX_MAX_SCALE_VAL;
        newvres = srcvres * vctl->oinfo.vscale / RX_MAX_SCALE_VAL;
    }

    set_fpga_val(board, FPGA_DSRCHRES, srchres);
    set_fpga_val(board, FPGA_DSRCVRES, srcvres);
    set_fpga_val(board, FPGA_DHSHIFT, vctl->oinfo.hoffset);
    set_fpga_val(board, FPGA_DVSHIFT, vctl->oinfo.voffset);
    set_fpga_val(board, FPGA_DNHRES, newhres);
    set_fpga_val(board, FPGA_DNVRES, newvres);
    set_fpga_val(board, FPGA_DHSCALE, vctl->oinfo.hscale);
    set_fpga_val(board, FPGA_DVSCALE, vctl->oinfo.vscale);

    if (vctl->oinfo.hscale == RX_MAX_SCALE_VAL &&
            vctl->oinfo.vscale == RX_MAX_SCALE_VAL) {
        set_fpga_val(board, FPGA_DSCALECTL, 0);
    }
    else {
        int k, hs = vctl->oinfo.hscale, vs = vctl->oinfo.vscale;
        unsigned long addr = DSCALE_COEFF_RAM_ADDR;

        set_fpga_val(board, FPGA_DSCALECTL, 1);

        /****************************
         * Load the coefficient ram *
         ****************************/
        for (k = 0; k < 64; k++) {
            set_fpga_val(board, addr, *(unsigned long *)&c[hs - 1][vs - 1][k]);
            addr += sizeof(unsigned long);
            udelay(1);
        }
        for (k = 0; k < 64; k++) {
            set_fpga_val(board, addr, *(unsigned long *)&ct[hs - 1][vs - 1][k]);
            addr += sizeof(unsigned long);
            udelay(1);
        }
    }

    /***********************************
     * Program alpha channel registers *
     ***********************************/
    if (vctl->oinfo.flags & OV_FLAG_SRC_REMOTE)
        oalpha = 0;
    else if (vctl->oinfo.flags & OV_FLAG_SRC_LOCAL)
        oalpha = 1024;
    else
        oalpha = (1024 * vctl->oinfo.oalpha) / 100;

    set_fpga_val(board, FPGA_DOVLPALPHA1, 1024 - oalpha);
    set_fpga_val(board, FPGA_DOVLPALPHA2, oalpha);
    set_fpga_val(board, FPGA_DNOVLPALPHA1, 0);
    set_fpga_val(board, FPGA_DNOVLPALPHA2, 1024);

    /*******************************
     * Set scale and DDR registers *
     *******************************/
    dummy_pixels = 0;
    if (vctl->oinfo.hscale != RX_MAX_SCALE_VAL ||
            vctl->oinfo.vscale != RX_MAX_SCALE_VAL) {
        dummy_pixels = ((newhres % 8) == 0) ? 0 : 8 - (newhres % 8);
        if (((newhres + dummy_pixels) / 8) % vctl->oinfo.hscale == 1)
            dummy_pixels += 8;
    }
    addr_in_line = (newhres + dummy_pixels) / 8;

    hcrop = vcrop = 0;
    if (lutp && vctl->oinfo.flags & OV_FLAG_CROP) {
        if (vctl->oinfo.hcropleft > 0 &&
                vctl->oinfo.hcropleft < lutp->a_orig_hres) {
            hcropleft = (vctl->oinfo.hcropleft * vctl->oinfo.hscale +
                RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            hcrop = 1;
        }
        if (vctl->oinfo.hcropright > 0 &&
                vctl->oinfo.hcropright < lutp->a_orig_hres) {
            hcropright = (vctl->oinfo.hcropright * vctl->oinfo.hscale +
                RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            hcrop = 1;
        }
        if (vctl->oinfo.vcroptop > 0 &&
            vctl->oinfo.vcroptop < lutp->a_orig_vres) {
            vcroptop = (vctl->oinfo.vcroptop * vctl->oinfo.vscale +
                RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            vcrop = 1;
        }
        if (vctl->oinfo.vcropbottom > 0 &&
                vctl->oinfo.vcropbottom < lutp->a_orig_vres) {
            vcropbottom = (vctl->oinfo.vcropbottom * vctl->oinfo.vscale +
                RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            vcrop = 1;
        }
    }
    if (lutp && lutp->a_hres != lutp->a_orig_hres) {
        hcropright += ((lutp->a_hres - lutp->a_orig_hres) *
                vctl->oinfo.hscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
        hcrop = 1;
    }
    if (lutp && lutp->a_vres != lutp->a_orig_vres) {
        vcropbottom += ((lutp->a_vres - lutp->a_orig_vres) *
                vctl->oinfo.vscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
        vcrop = 1;
    }
    or_in_fpga_val(board, FPGA_DSCALECTL, (dummy_pixels) << 4);
    set_fpga_val(board, FPGA_DDR2RDSTART, vcroptop * addr_in_line +
            hcropleft / 8 + 1);
    if (hcrop) {
        set_fpga_val(board, FPGA_DDR2RDEND, (newvres - vcropbottom) *
                addr_in_line - (hcropright + dummy_pixels) / 8 - 1);
    }
    else {
        set_fpga_val(board, FPGA_DDR2RDEND, (newvres - vcropbottom) *
                addr_in_line - 1);
    }

    /**********************
     * Set crop registers *
     **********************/
    if (hcrop || vcrop) {
        set_fpga_val(board, FPGA_DCROPVRES, newvres - vcroptop - vcropbottom);
        set_fpga_val(board, FPGA_DCROPHSTART, (vcroptop + 1) * addr_in_line +
                hcropleft / 8);
        if (hcrop)
            set_fpga_val(board, FPGA_DCROPHEND, (vcroptop + 1) * addr_in_line -
                (hcropright + dummy_pixels) / 8 - 2);
        else
            set_fpga_val(board, FPGA_DCROPHEND, (vcroptop + 1) * addr_in_line -
                2);
        set_fpga_val(board, FPGA_DCROPHRES, newhres - hcropleft - hcropright);
        set_fpga_val(board, FPGA_DCROPHLEFT, hcropleft);
        set_fpga_val(board, FPGA_DCROPHRIGHT, hcropright);
    }

    /******************************
     * Program FPGA_ATESTREG bits *
     ******************************/
    if (vctl->active_interface & INTERFACE_DIGITAL) {
        or_in_fpga_val(board, FPGA_ATESTREG, DVIINPUT_ENABLE);
        and_in_fpga_val(board, FPGA_ATESTREG, ~LPBK_DATA_ENABLE);
    }
    else {
        and_in_fpga_val(board, FPGA_ATESTREG, ~DVIINPUT_ENABLE);
        or_in_fpga_val(board, FPGA_ATESTREG, LPBK_DATA_ENABLE);
    }

    if (vctl->oinfo.flags & OV_FLAG_SRC_LOCAL) {
        or_in_fpga_val(board, FPGA_ATESTREG, (RXLPBK_ENABLE|RXLPBK_CONTROL));
    }
    else if (vctl->oinfo.flags & OV_FLAG_SRC_OVERLAY) {
        or_in_fpga_val(board, FPGA_ATESTREG, RXLPBK_ENABLE);
        and_in_fpga_val(board, FPGA_ATESTREG, ~RXLPBK_CONTROL);
    }
    else {
        and_in_fpga_val(board, FPGA_ATESTREG, ~(RXLPBK_ENABLE|RXLPBK_CONTROL));
    }

    if (vctl->active_interface & INTERFACE_48BIT) {
        and_in_fpga_val(board, FPGA_ATESTREG, ~FPGA_VIDEO_24BIT);
        if (vctl->active_interface & INTERFACE_DUALLINK)
            or_in_fpga_val(board, FPGA_ATESTREG, VIDEO_DUALLINK);
        else
            and_in_fpga_val(board, FPGA_ATESTREG, ~VIDEO_DUALLINK);
    }
    else {
        and_in_fpga_val(board, FPGA_ATESTREG, ~VIDEO_DUALLINK);
        or_in_fpga_val(board, FPGA_ATESTREG, FPGA_VIDEO_24BIT);
    }

    if (vcrop) {
        or_in_fpga_val(board, FPGA_ATESTREG, VCROP_ENABLE);
    }
    else {
        and_in_fpga_val(board, FPGA_ATESTREG, ~VCROP_ENABLE);
    }
    if (hcrop) {
        or_in_fpga_val(board, FPGA_ATESTREG, HCROP_ENABLE);
    }
    else {
        and_in_fpga_val(board, FPGA_ATESTREG, ~HCROP_ENABLE);
    }

    /**********************************
     * Set RX bank toggling registers *
     **********************************/
    if (enable444 || enablestereo) {
        set_fpga_val(board, FPGA_DDR2THRES0, blocksinbank * 32 - 1);
        set_fpga_val(board, FPGA_DDR2THRES1, 2 * blocksinbank * 32 - 1);
        set_fpga_val(board, FPGA_DDR2THRES2, 3 * blocksinbank * 32 - 1);
    }
    else {
        set_fpga_val(board, FPGA_DDR2THRES0, blocksinbank * 16 - 1);
        set_fpga_val(board, FPGA_DDR2THRES1, 2 * blocksinbank * 16 - 1);
        set_fpga_val(board, FPGA_DDR2THRES2, 3 * blocksinbank * 16 - 1);
    }

    /***************************************
     * Take add and drop DCMs out of reset *
     ***************************************/
    udelay(100);
    set_fpga_val(board, FPGA_CDCMRREG, 0);

    /************************************************************
     * Wait for addside PLL to lock (We ignore the return value *
     * Bit 0 in ASREG doesn't mean the same for RX as for TX.)  *
     ************************************************************/
    if (wait_till_drop_pll_locked(board) != 0) {
        LOGPRINT(LOG_MOD_FPGA|LOG_LEV_WARNING,
            "Channel %d Dropside DCM remained unlocked.\n", board->v_id+1);
        rc = -1;
    }
    else {
        LOGPRINT(LOG_MOD_FPGA|LOG_LEV_DEBUG,
            "Channel %d Dropside DCM locked.\n", board->v_id+1);
    }

    if (vctl->oinfo.flags & OV_FLAG_SRC_REMOTE) {
        /*************************************************
         * Set sync polarity to that for the display LUT *
         *************************************************/
        if (vctl->current_display_lut->d_dcreg & DCREG_HSYNC_POL)
            or_in_fpga_val(board, FPGA_DCREG, DCREG_HSYNC_POL);
        else
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_HSYNC_POL);
        if (vctl->current_display_lut->d_dcreg & DCREG_VSYNC_POL)
            or_in_fpga_val(board, FPGA_DCREG, DCREG_VSYNC_POL);
        else
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VSYNC_POL);
    }
    else {
        /******************************************************
         * Invert drop sync polarity if needed - Must do this *
         * before reading FPGA_AAVHDREG and FPGA_AAVVDREG     *
         ******************************************************/
        if (vctl->active_interface & INTERFACE_DIGITAL) {
            if (get_fpga_val(board, FPGA_DVIHPOLREG)) {
                or_in_fpga_val(board, FPGA_ATESTREG, DVIHPOL_INVERT);
                or_in_fpga_val(board, FPGA_DCREG, DCREG_HSYNC_POL);
            }
            else {
                and_in_fpga_val(board, FPGA_ATESTREG, ~DVIHPOL_INVERT);
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_HSYNC_POL);
            }
            if (get_fpga_val(board, FPGA_DVIVPOLREG)) {
                or_in_fpga_val(board, FPGA_ATESTREG, DVIVPOL_INVERT);
                or_in_fpga_val(board, FPGA_DCREG, DCREG_VSYNC_POL);
            }
            else {
                and_in_fpga_val(board, FPGA_ATESTREG, ~DVIVPOL_INVERT);
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VSYNC_POL);
            }
        }
        else {
            if (get_fpga_val(board, FPGA_ADCHPOLREG))
                or_in_fpga_val(board, FPGA_DCREG, DCREG_HSYNC_POL);
            else
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_HSYNC_POL);
            if (get_fpga_val(board, FPGA_ADCVPOLREG))
                or_in_fpga_val(board, FPGA_DCREG, DCREG_VSYNC_POL);
            else
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VSYNC_POL);
        }
    }
    mdelay(50);

    /*****************************
     * Program genlock registers *
     *****************************/
    if (vctl->oinfo.flags & OV_FLAG_SRC_REMOTE) {
        if (vctl->av_flag & AVFLAG_GENLOCKMASTER) {
            and_in_fpga_val(board, FPGA_DCREG,
                    ~(DCREG_GENLOCK_MODE|DCREG_LRSYNC_DISABLE));
        }
        else if (vctl->av_flag & AVFLAG_GENLOCKSLAVE) {
            /* Set genlock to Slave if stereo is detected on input */
            if (get_fpga_val(board, FPGA_AMTREG) & AMTREG_STEREO) {
                or_in_fpga_val(board, FPGA_DCREG, DCREG_GENLOCK_MODE);
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_LRSYNC_DISABLE);
            }
            /* Otherwise set genlock to Master */
            else {
                LOGPRINT(LOG_MOD_FPGA|LOG_LEV_WARNING,
                        "Channel %d Stereo not detected. "
                        "Setting genlock to Master\n", board->v_id+1);
                and_in_fpga_val(board, FPGA_DCREG,
                        ~(DCREG_GENLOCK_MODE|DCREG_LRSYNC_DISABLE));
            }
        }
        else {
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_GENLOCK_MODE);
            or_in_fpga_val(board, FPGA_DCREG, DCREG_LRSYNC_DISABLE);
        }
    }

    /*****************************
     * Program addside registers *
     *****************************/
    if (vctl->active_interface & INTERFACE_DIGITAL) {
        avhdreg = get_fpga_val(board, FPGA_AAVHDREG);
        avvdreg = get_fpga_val(board, FPGA_AAVVDREG);
        /*****************************************
         * Double the horiz delay in 48 bit mode *
         *****************************************/
        if (vctl->active_interface & INTERFACE_48BIT)
            avhdreg <<= 1;
    }
    else {
        avhdreg = hsync + hbp;
        avvdreg = vsync + vbp;
    }

    set_fpga_val(board, FPGA_AVHAREG, hres);
    udelay(1);
    set_fpga_val(board, FPGA_AVVAREG, vres);
    udelay(1);
    set_fpga_val(board, FPGA_AALHDELAY, avhdreg - hds);
    udelay(1);
    set_fpga_val(board, FPGA_AALVDELAY, avvdreg - vds);
    udelay(1);

    /**********************************
     * Do drop pixel adjust if needed *
     **********************************/
    if (vctl->pix_adjust && (vctl->oinfo.flags & OV_FLAG_SRC_REMOTE)) {
        int tval;
        if (vctl->pix_adjust >= 0) {
            tval = vctl->pix_adjust;
            set_fpga_val(board, FPGA_DVHFPREG,
                    vctl->current_display_lut->a_hfp + (tval/2));
            udelay(1);
            set_fpga_val(board, FPGA_DVHBPREG,
                    vctl->current_display_lut->a_hbp + (tval/2+tval%2));
        }
        else if (vctl->pix_adjust < 0) {
            tval = -vctl->pix_adjust;
            set_fpga_val(board, FPGA_DVHFPREG,
                    vctl->current_display_lut->a_hfp - (tval/2));
            udelay(1);
            set_fpga_val(board, FPGA_DVHBPREG,
                    vctl->current_display_lut->a_hbp - (tval/2+tval%2));
        }
    }

    /******************
     * Enable VID DAC *
     ******************/
    or_in_fpga_val(board, FPGA_DCREG,  DCREG_VID_DAC_ENABLE);

    /***********************
     * Enable addside FPGA *
     ***********************/
    or_in_fpga_val(board, FPGA_ACREG, (ACREG_ADD_ENABLE |
                                       ACREG_VID_ENABLE |
                                       ACREG_LAN_ENABLE));

    /************************
     * Enable dropside FPGA *
     ************************/
    or_in_fpga_val(board, FPGA_DCREG, (DCREG_DROP_ENABLE |
                                       DCREG_VID_ENABLE |
                                       DCREG_AUD_ENABLE |
                                       DCREG_LAN_ENABLE));
    return 0;
}

void adjust_hd(struct v2d_board *board, int shift) {

    struct v2d_ctl *vctl;
    vctl = board->v_ctl;

#if 0
    // PSCW -- Don't do range check here - the menu will take care of it
    if (shift < -135)
        shift = -135;
    else if (shift > 375)
        shift = 375;
#endif

    vctl->hd_shift = shift;
    if (board->v_type == HARDWARE_V2D_TX) {
        if (vctl->current_addside_lut != NULL) {
            configure_addside(board, vctl->current_addside_lut);
            if (wait_till_add_pll_locked(board) < 0)
                LOGPRINT(LOG_MOD_FPGA|LOG_LEV_WARNING,
                    "Channel %d Addside DCM remained unlocked\n",
                    board->v_id+1);
        }
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        if (vctl->current_display_lut != NULL) {
            configure_dropside(board, vctl->current_dropside_lut);
            if (wait_till_drop_pll_locked(board) < 0)
                LOGPRINT(LOG_MOD_FPGA|LOG_LEV_WARNING,
                    "Channel %d Dropside DCM remained unlocked\n",
                    board->v_id+1);
        }
    }
}

int get_hd(struct v2d_board *board) {
    struct v2d_ctl *vctl;
    vctl = board->v_ctl;

    return vctl->hd_shift;
}

static void adjust_vd(struct v2d_board *board,int shift) {
    struct v2d_ctl *vctl;
    vctl = board->v_ctl;

#if 0
    // PSCW -- Don't do range check here - the menu will take care of it
    if (shift < -25)
        shift = -25;
    else if (shift > 50)
        shift = 50;
#endif

    vctl->vd_shift = shift;
    if (board->v_type == HARDWARE_V2D_TX) {
        if (vctl->current_addside_lut != NULL) {
            configure_addside(board, vctl->current_addside_lut);
            if (wait_till_add_pll_locked(board) < 0)
                LOGPRINT(LOG_MOD_FPGA|LOG_LEV_WARNING,
                    "Channel %d Addside DCM remained unlocked\n",
                    board->v_id+1);
        }
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        if (vctl->current_display_lut != NULL) {
            configure_dropside(board, vctl->current_dropside_lut);
            if (wait_till_drop_pll_locked(board) < 0)
                LOGPRINT(LOG_MOD_FPGA|LOG_LEV_WARNING,
                    "Channel %d Dropside DCM remained unlocked\n",
                    board->v_id+1);
        }
    }
}

static int get_vd(struct v2d_board *board) {
    struct v2d_ctl *vctl;
    vctl = board->v_ctl;

    return vctl->vd_shift;
}

static void set_phase_adjust(struct v2d_board *board,unsigned char val) {

    struct v2d_ctl *vctl;
    vctl = board->v_ctl;
    vctl->phase_shift = val << 3;
    if (board->v_hardware_version == HARDWARE_VERSION_A)
        set_ad9888_val(VIF_PHASE_ADJ, vctl->phase_shift);  /* user adjustable */
    else
        set_ad9887_val(VIF_9887_PHASE_ADJ, vctl->phase_shift);
}

static unsigned char get_phase_adjust( struct v2d_board *board) {

    struct v2d_ctl *vctl;
    vctl = board->v_ctl;
    return vctl->phase_shift >> 3;
}

static void set_pixel_adjust(struct v2d_board *board,
                             int pixels,
                             int tweak_hi,
                             int tweak_lo) {
    struct LUT_struct *lutp;
    struct v2d_ctl *vctl;
    int tval;

    vctl = board->v_ctl;

    vctl->pix_adjust = pixels;
    vctl->pix_tweak_hi = tweak_hi;
    vctl->pix_tweak_lo = tweak_lo;

    if ((lutp = vctl->current_dropside_lut) != NULL) {
        if (vctl->pix_adjust >= 0) {
            set_fpga_val(board, FPGA_DVHFPREG, lutp->d_dvhfpreg +
                    vctl->pix_adjust/2);
            udelay(1);
            set_fpga_val(board, FPGA_DVHBPREG, lutp->d_dvhbpreg +
                    vctl->pix_adjust/2 + vctl->pix_adjust%2);
            tweak_frequency(board, vctl->pix_tweak_hi, vctl->pix_tweak_lo);
        }
        else if (vctl->pix_adjust < 0) {
            tval = -vctl->pix_adjust;
            set_fpga_val(board, FPGA_DVHFPREG, lutp->d_dvhfpreg - (tval/2));
            udelay(1);
            set_fpga_val(board, FPGA_DVHBPREG, lutp->d_dvhbpreg -
                    (tval/2 + tval%2));
            tweak_frequency(board, vctl->pix_tweak_hi, vctl->pix_tweak_lo);
        }
        and_in_fpga_val(board, FPGA_DCREG, ~DCREG_VID_ENABLE);
        udelay(1000);
        or_in_fpga_val(board, FPGA_DCREG, DCREG_VID_ENABLE);
    }
}

/*******************************************************
 * Returns true is FPGA is capable of generating video *
 *******************************************************/
int video_available(struct v2d_board *board) {
    struct v2d_ctl *vctl;
    vctl = board->v_ctl;

    if (vctl->force_no_video > 0)   /* Return false if configured for ignoring video */
        return 0;
    if (vctl->current_addside_lut != NULL)
        return 1;
    return 0;
}

#define PROC_FILE_SIZE  0x2000

/***************************************************************
 * Provide interrupt statistics to user via proc file system.  *
 ***************************************************************/
static int fpga_procfile_reg_read(char* page, char** start, off_t off,
                                  int count, int* eof, void* data)
{
    int lwm, hwm;
    int writelen;
    struct v2d_board *board= (struct v2d_board*) data;
    struct v2d_ctl *vctl;

    static int len = 0;
    static char *filebuf;

    vctl = board->v_ctl;
    if (off > 0)
        goto showdata;

    filebuf = kmalloc(PROC_FILE_SIZE, GFP_KERNEL);
    if (filebuf == NULL)
        goto showdata;

    len += sprintf(filebuf + len, "*** FPGA Common Registers ***\n");
    len += sprintf(filebuf + len, "FPGA_IMREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_IMREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_ISREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_ISREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_CSREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_CSREG));
    len += sprintf(filebuf + len, "\n");

    len += sprintf(filebuf + len, "\n*** FPGA Add Side Registers ***\n");
    len += sprintf(filebuf + len, "FPGA_ACREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_ACREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_ASREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_ASREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_AVHAREG = 0x%08lx",
                   get_fpga_val(board, FPGA_AVHAREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_AVVAREG = 0x%08lx",
                   get_fpga_val(board, FPGA_AVVAREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_AVHDREG = 0x%08lx",
                   get_fpga_val(board, FPGA_AVHDREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_AVVDREG = 0x%08lx",
                   get_fpga_val(board, FPGA_AVVDREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_AAFDREG = 0x%08lx",
                   get_fpga_val(board, FPGA_AAFDREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_ALFLREG = 0x%08lx",
                   get_fpga_val(board, FPGA_ALFLREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_AMHREG  = 0x%08lx",
                   get_fpga_val(board, FPGA_AMHREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_AMVREG  = 0x%08lx",
                   get_fpga_val(board, FPGA_AMVREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_AMTREG  = 0x%08lx",
                   get_fpga_val(board, FPGA_AMTREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_AMLREG  = 0x%08lx",
                   get_fpga_val(board, FPGA_AMLREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_ALUTREG = 0x%08lx",
                   get_fpga_val(board, FPGA_ALUTREG));
    len += sprintf(filebuf + len, "\n");

    len += sprintf(filebuf + len, "\n*** FPGA Drop Side Registers ***\n");
    len += sprintf(filebuf + len, "FPGA_DCREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_DCREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DSREG   = 0x%08lx",
                   get_fpga_val(board, FPGA_DSREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DVHAREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DVHAREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DVHFPREG= 0x%08lx",
                   get_fpga_val(board, FPGA_DVHFPREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DVHSREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DVHSREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DVHBPREG= 0x%08lx",
                   get_fpga_val(board, FPGA_DVHBPREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DVVAREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DVVAREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DVVFPREG= 0x%08lx",
                   get_fpga_val(board, FPGA_DVVFPREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DVVSREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DVVSREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DVVBPREG= 0x%08lx",
                   get_fpga_val(board, FPGA_DVVBPREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DAFDREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DAFDREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DLFLREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DLFLREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DTSREG  = 0x%08lx",
                   get_fpga_val(board, FPGA_DTSREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DTLREG  = 0x%08lx",
                   get_fpga_val(board, FPGA_DTLREG));
    len += sprintf(filebuf + len, "\n");
    len += sprintf(filebuf + len, "FPGA_DLUTREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DLUTREG));
    len += sprintf(filebuf + len, "        ");
    len += sprintf(filebuf + len, "FPGA_DVBWREG = 0x%08lx",
                   get_fpga_val(board, FPGA_DVBWREG));
    len += sprintf(filebuf + len, "\n");

    if (board->v_pci_dev_type == V2DI50_DEVICE &&
            board->v_type == HARDWARE_V2D_TX) {
        len += sprintf(filebuf + len, "\n*** id18125fifo Registers ***\n");
        id18125_read(&lwm, &hwm);
        len += sprintf(filebuf + len, "Low  WMark   = 0x%05x", lwm);
        len += sprintf(filebuf + len, "       ");
        len += sprintf(filebuf + len, "High WMark   = 0x%05x", hwm);
        len += sprintf(filebuf + len, "\n");

        if (board->v_hardware_version == HARDWARE_VERSION_A) {
            len += sprintf(filebuf + len, "\n*** ad9888vif Registers ***\n");
            len += sprintf(filebuf + len, "VIF_PHASE_ADJ = 0x%02x",
                       get_ad9888_val(VIF_PHASE_ADJ));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_PLL_MSB   = 0x%02x",
                       get_ad9888_val(VIF_PLL_MSB));
            len += sprintf(filebuf + len, "\n");
            len += sprintf(filebuf + len, "VIF_PLL_LSB   = 0x%02x",
                       get_ad9888_val(VIF_PLL_LSB));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_VCO_CPMP  = 0x%02x",
                       get_ad9888_val(VIF_VCO_CPMP));
            len += sprintf(filebuf + len, "\n");
            len += sprintf(filebuf + len, "VIF_MODE_MUX  = 0x%02x",
                       get_ad9888_val(VIF_MODE_MUX));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_TEST1     = 0x%02x",
                       get_ad9888_val(VIF_TEST1));
            len += sprintf(filebuf + len, "\n");

            len += sprintf(filebuf + len, "VIF_SYNC_DETECT = 0x%02x",
                       get_ad9888_val(VIF_SYNC_DETECT));
            len += sprintf(filebuf + len, "      ");
            len += sprintf(filebuf + len, "VIF_SYNC_CNTL   = 0x%02x",
                       get_ad9888_val(VIF_SYNC_CNTL));
            len += sprintf(filebuf + len, "\n");

            len += sprintf(filebuf + len, "VIF_CLAMP_COAST = 0x%02x",
                       get_ad9888_val(VIF_CLAMP_COAST));
            len += sprintf(filebuf + len, "        ");

            len += sprintf(filebuf + len, "VIF_CHIP_REV    = 0x%02x",
                       get_ad9888_val(VIF_CHIP_REV));
            len += sprintf(filebuf + len, "\n");
        }
        else {
            len += sprintf(filebuf + len, "\n*** ad9887vif Registers ***\n");
            len += sprintf(filebuf + len, "VIF_9887_PHASE_ADJ = 0x%02x",
                       get_ad9887_val(VIF_9887_PHASE_ADJ));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_9887_PLL_MSB   = 0x%02x",
                       get_ad9887_val(VIF_9887_PLL_MSB));
            len += sprintf(filebuf + len, " \n");
            len += sprintf(filebuf + len, "VIF_9887_PLL_LSB   = 0x%02x",
                       get_ad9887_val(VIF_9887_PLL_LSB));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_9887_VCO_CPMP  = 0x%02x",
                       get_ad9887_val(VIF_9887_VCO_CPMP));
            len += sprintf(filebuf + len, "\n");
            len += sprintf(filebuf + len, "VIF_9887_MODE_CNTL1= 0x%02x",
                       get_ad9887_val(VIF_9887_MODE_CNTL1));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_9887_MODE_MUX  = 0x%02x",
                       get_ad9887_val(VIF_9887_MODE_MUX));
            len += sprintf(filebuf + len, "\n");
            len += sprintf(filebuf + len, "VIF_9887_TEST1     = 0x%02x",
                       get_ad9887_val(VIF_9887_TEST1));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_9887_TEST2     = 0x%02x",
                       get_ad9887_val(VIF_9887_TEST2));
            len += sprintf(filebuf + len, "\n");
            len += sprintf(filebuf + len, "VIF_9887_TEST3     = 0x%02x",
                       get_ad9887_val(VIF_9887_TEST3));
            len += sprintf(filebuf + len, "        ");
            len += sprintf(filebuf + len, "VIF_9887_CHIP_REV  = 0x%02x",
                       get_ad9887_val(VIF_9887_CHIP_REV));
            len += sprintf(filebuf + len, "\n");
        }
        len += sprintf(filebuf + len,
                    "Add Side Mismatch Count = %ld \n",
                    vctl->no_addside_match_count);
    }
    else if (board->v_pci_dev_type == V2DI50_DEVICE) {
        len += sprintf(filebuf + len, "\n*** DDS Registers ***\n");
        len += sprintf(filebuf + len, "DDS_FREQ_WORD1_01 = 0x%02x",
                    get_ad9852_val(board,DDS_FREQ_WORD1_01));
        len += sprintf(filebuf + len, "    ");
        len += sprintf(filebuf + len, "DDS_FREQ_WORD1_02 = 0x%02x",
                    get_ad9852_val(board,DDS_FREQ_WORD1_02));
        len += sprintf(filebuf + len, "\n");
        len += sprintf(filebuf + len, "DDS_FREQ_WORD1_03 = 0x%02x",
                    get_ad9852_val(board,DDS_FREQ_WORD1_03));
        len += sprintf(filebuf + len, "    ");
        len += sprintf(filebuf + len, "DDS_FREQ_WORD1_04 = 0x%02x",
                    get_ad9852_val(board,DDS_FREQ_WORD1_04));
        len += sprintf(filebuf + len, "\n");
        len += sprintf(filebuf + len, "DDS_FREQ_WORD1_05 = 0x%02x",
                    get_ad9852_val(board,DDS_FREQ_WORD1_05));
        len += sprintf(filebuf + len, "    ");
        len += sprintf(filebuf + len, "DDS_FREQ_WORD1_06 = 0x%02x",
                    get_ad9852_val(board,DDS_FREQ_WORD1_06));
        len += sprintf(filebuf + len, "\n");
    }

    if (board->v_pci_dev_type == PLX_DEVICE ||
            board->v_type == HARDWARE_V2D_TX) {
        unsigned long tim;
        int days, hours, mins, secs;
        tim = vctl->addside_lock_count;
        days = tim/(60*60*24);
        tim = tim%(60*60*24);
        hours = tim/(60*60);
        tim = tim%(60*60);
        mins = tim/60;
        secs = tim%60;

        len += sprintf(filebuf + len,
                    "\nAdd Side Video Running for %d d, %d h, %d m, %d s\n",
                    days, hours, mins, secs);
    }

    len += sprintf(filebuf + len, "\n*** Global Variables ***\n");
    len += sprintf(filebuf + len, "videofreeze_check   = %d",
                vctl->video_freeze_check);
    len += sprintf(filebuf + len, "          ");
    len += sprintf(filebuf + len, "default_vco_cpmp    = 0x%02x\n",
                vctl->default_vco_cpmp);
    len += sprintf(filebuf + len, "external_sync       = %d",
                vctl->external_sync);
    len += sprintf(filebuf + len, "          ");
    len += sprintf(filebuf + len, "force no video      = %d\n",
                vctl->force_no_video);
    len += sprintf(filebuf + len, "force blk per slc   = %d",
                vctl->force_anobreg);
    len += sprintf(filebuf + len, "          ");
    len += sprintf(filebuf + len, "noise thres mode    = %d\n",
                vctl->noise_thres_mode);
    len += sprintf(filebuf + len, "prog compression    = %d",
                vctl->progressive_compression);
    len += sprintf(filebuf + len, "          ");
    len += sprintf(filebuf + len, "active interface    = 0x%x\n",
                    vctl->active_interface);
    len += sprintf(filebuf + len, "board status        = 0x%08lx ",
                    board->v_status);
    len += sprintf(filebuf + len, "audio video flag    = 0x%04x\n",
                    vctl->av_flag);
    if (board->v_pci_dev_type == PLX_DEVICE &&
            board->v_type == HARDWARE_V2D_RX) {
        len += sprintf(filebuf + len, "OVC flags           = 0x%08x",
                vctl->ocfg.flags);
        len += sprintf(filebuf + len, " ");
        len += sprintf(filebuf + len, "OVI flags           = 0x%08x\n",
                vctl->oinfo.flags);
        len += sprintf(filebuf + len, "OVC Scale           = %03d/%03d",
                vctl->ocfg.hscale, vctl->ocfg.vscale);
        len += sprintf(filebuf + len, "    ");
        len += sprintf(filebuf + len, "OVI Scale           = %03d/%03d\n",
                vctl->oinfo.hscale, vctl->oinfo.vscale);
        len += sprintf(filebuf + len, "OVC Location        = %04d/%04d",
                vctl->ocfg.hlocation, vctl->ocfg.vlocation);
        len += sprintf(filebuf + len, "  ");
        len += sprintf(filebuf + len, "OVI Location        = %04d/%04d\n",
                vctl->oinfo.hlocation, vctl->oinfo.vlocation);
        len += sprintf(filebuf + len, "OVC Offset          = %04d/%04d",
                vctl->ocfg.hoffset, vctl->ocfg.voffset);
        len += sprintf(filebuf + len, "  ");
        len += sprintf(filebuf + len, "OVI Offset          = %04d/%04d\n",
                vctl->oinfo.hoffset, vctl->oinfo.voffset);
        len += sprintf(filebuf + len, "OVI Hcrop           = %04d/%04d",
                vctl->oinfo.hcropleft, vctl->oinfo.hcropright);
        len += sprintf(filebuf + len, "  ");
        len += sprintf(filebuf + len, "OVI Vcrop           = %04d/%04d\n",
                vctl->oinfo.vcroptop, vctl->oinfo.vcropbottom);
    }
    if (vctl->current_addside_lut == NULL)
        len += sprintf(filebuf + len, "current addside LUT = NULL\n");
    else
        len += sprintf(filebuf + len, "current addside LUT = 0x%x/%s\n",
                vctl->current_addside_lut->a_alutreg,
                vctl->current_addside_lut->comment);
    if (vctl->current_dropside_lut == NULL)
        len += sprintf(filebuf + len, "current dropside LUT = NULL\n");
    else
        len += sprintf(filebuf + len, "current dropside LUT = 0x%x/%s\n",
                vctl->current_dropside_lut->a_alutreg,
                vctl->current_dropside_lut->comment);
    if (vctl->current_display_lut == NULL)
        len += sprintf(filebuf + len, "current display LUT = NULL\n");
    else
        len += sprintf(filebuf + len, "current display LUT = 0x%x/%s\n",
                vctl->current_display_lut->a_alutreg,
                vctl->current_display_lut->comment);
    len += sprintf(filebuf + len, "\n");

showdata:

    if (filebuf == NULL) {
        *eof = 1;
        return 0;
    }

    writelen = count > len ? len : count;

    memcpy(page + off, filebuf + off, writelen);
    len -= writelen;

    if (len == 0) {
        kfree(filebuf);
        filebuf = NULL;
        *eof = 1;
    }

    *start = page + off;

    return writelen;
}


