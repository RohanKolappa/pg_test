#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>

#include "vutils.h"
#include "lut_utils.h"
#include "vlog.h"
#include "hal.h"
#include "hal_console.h"
#include "commonutils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "clientutils.h"
#include "loadnet.h"
#include "osd_control.h"
#include "kbm_control.h"
#include "kbconsole_config.h"
#include "rx_kbm_config.h"
#include "kbmode_config.h"
#include "override_config.h"
#include "override_utils.h"
#include "usbutils.h"
#include "xkb_utils.h"


#define CLN         "clnt"
#define KBM         "kbm"
#define KBMKB       "kbm.keyboard"
#define KBMMS       "kbm.mouse"
#define KBMEV       "kbm.event"
#define KBMRFB      "kbm.rfb"

/******************************************
 * Client Dropside Video Routines         *
 ******************************************/
int configure_dropside_video(HAL *hp, CLNT *clnt,
                                 struct LUT_struct *lutp,
                                 int slicesize,
                                 int compmode,
                                 int motioncompmode) {
    int lutid = RESERVED_RX_LUTID;
    int cno = hal_get_channel_number(hp);
    int hardware_type = hal_get_hardware_type(hp);

    if (lutp->a_alutreg > 0)
        lutid = lutp->a_alutreg;
    else
        lutp->a_alutreg = lutid;
    VLOG_INFO_CAT(VLOG_CLN,
            "Channel %d. Drop LUT ID %d %s(%dx%d), ssize %d, stereo %s, "
            "444 %s, motion %s", cno, lutp->a_alutreg, lutp->comment, 
            lutp->a_orig_hres, lutp->a_orig_vres, slicesize, 
            (lutp->a_stereo) ? "Yes" : "No", (compmode) ? "Yes" : "No", 
            (motioncompmode) ? "Yes" : "No");

    /***********************************************
     * Flush all driver buffers on the drop side.  *
     ***********************************************/
    hal_flush_fc_queues_k(hp);

    /******************************
     * Compute all LUT parameters *
     ******************************/
    build_lut_entry(lutp, hardware_type);

    /**************************************************
     * For I50, find the LUT we should use for actual *
     * display and re-configure dropside video .      *
     **************************************************/
    if (hardware_type == V2DI50_DEVICE) {
        struct LUT_struct override_lut;
        struct override_config override_cfg;
        int hscale, vscale;

        memset(&override_lut, '\0', sizeof(struct LUT_struct));
        get_override_config(hp, &override_cfg);

        if (get_override_lut_and_scale(hp, lutp, &override_lut, &override_cfg,
                &hscale, &vscale) < 0) {
            /* Not overriding. Use remote LUT for display */
            memcpy(&override_lut, lutp, sizeof(struct LUT_struct));
        }
        else {
            char lutname[64];
            VLOG_INFO_CAT(VLOG_CLN,
                    "Overriding %s with %s. Scale %d/%d. Sync %s/%s.",
                    lutp->comment, override_lut.comment, hscale, vscale,
                    (override_lut.d_dcreg & DCREG_HSYNC_POL) ? "Neg" : "Pos",
                    (override_lut.d_dcreg & DCREG_VSYNC_POL) ? "Neg" : "Pos");
            /* Set LUT to override LUT except for name and id */
            lutid = lutp->a_alutreg;
            strncpy(lutname, lutp->comment, sizeof(lutname));
            memcpy(lutp, &override_lut, sizeof(struct LUT_struct));
            lutp->a_alutreg = lutid;
            strncpy(lutp->comment, lutname, sizeof(lutname));
        }

        hal_set_dropside_nobpslice(hp, slicesize);
        hal_set_dropside_compmode(hp, compmode, motioncompmode);
        hal_setDropsideVideo(hp, lutp, &override_lut);
    }
    /*******************************************************
     * For XP and XPi, make up the new dropside LUT & push *
     * into the driver. hald will do the rest.             *
     *******************************************************/
    else {
        if (hardware_type == XPI_DEVICE) {
            hal_set_dropside_compmode(hp, compmode, motioncompmode);
        }
        else {
            int avflag = hal_getAudioVideoFlag(hp);
            /* 444 and motion 444 are programmed the same way on XP RX */
            if (compmode || motioncompmode)  
                lutp->d_dcreg |= DCREG_444_SEL;
            else 
                lutp->d_dcreg &= ~DCREG_444_SEL;
            lutp->d_slicereg = (lutp->a_hres * lutp->a_vres) / (64 * slicesize);
            if (motioncompmode)
                avflag |= AVFLAG_MOTION444;
            hal_setAudioVideoFlag(hp, avflag);
        }
        hal_set_dropside_lut(hp, lutp);
    }

    return 0;
}

/******************************************
 * Create and initialize a CLNT object    *
 ******************************************/
CLNT *clnt_acquire(HAL *hp) {
    CLNT *clnt;
    struct rx_kbm_config kbm_cfg;
    struct kbmode_config kbmode_cfg;
    int i, cno;

    /* Board must be PLX device and must be RX */
    if (hp == NULL || get_supported_hardware(hp) == 0 ||
           hal_get_board_type(hp) != BOARDTYPE_RX) {
        return NULL;
    }

    cno = hal_get_channel_number(hp);

    /* Allocate the CLNT structure */
    VLOG_TRACE_CAT(VLOG_CLN, "Allocating CLNT object for Channel %d", cno);
    clnt = (CLNT *) calloc(1, sizeof(CLNT));
    if (clnt == NULL) {
        VLOG_CRITICAL_CAT(VLOG_CLN,
                "Memory allocation failed for Channel %d. %m.", cno);
        return NULL;
    }

    /* Initialize local keyboard and mouse types */
    get_rx_kbm_config(hp, &kbm_cfg);
    clnt_set_local_keyboard_type(hp, clnt, kbm_cfg.local_keyboard_type);
    clnt_set_local_mouse_type(hp, clnt, kbm_cfg.local_mouse_type);

    /* Initialize current KBM mode */
    hal_update_virtual_console(hp);
    clnt->curr_kbmode = hal_get_current_kbmode(hp);

    /* Initialize keyboard console mode escape sequence */
    get_kbcons_settings(hp, &kbmode_cfg);
    for (i = 0; i < MAX_ESC_CHARS; i++)
        clnt->console_escape_keycode[i] = kbmode_cfg.esc[i];

    clnt->console_escape_len = get_keyboard_escape(clnt->console_escape,
            kbmode_cfg.esc, clnt->local_keyboard_type);

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. Setting keyboard console escape to (len %d):",
            cno, clnt->console_escape_len);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->console_escape, clnt->console_escape_len);

    /* Initialize keyboard local mode key escape sequence */
    get_rx_kblocal_settings(hp, &kbmode_cfg);
    for (i = 0; i < MAX_ESC_CHARS; i++)
        clnt->local_escape_keycode[i] = kbmode_cfg.esc[i];

    clnt->local_escape_len = get_keyboard_escape(clnt->local_escape,
            kbmode_cfg.esc, clnt->local_keyboard_type);

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. Setting keyboard local escape to (len %d):",
            cno, clnt->local_escape_len);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->local_escape, clnt->local_escape_len);

    /* Initialize keyboard remote mode key escape sequence */
    get_rx_kbremote_settings(hp, &kbmode_cfg);
    for (i = 0; i < MAX_ESC_CHARS; i++)
        clnt->remote_escape_keycode[i] = kbmode_cfg.esc[i];

    clnt->remote_escape_len = get_keyboard_escape(clnt->remote_escape,
            kbmode_cfg.esc, clnt->local_keyboard_type);

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. Setting keyboard remote escape to (len %d):",
            cno, clnt->remote_escape_len);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->remote_escape, clnt->remote_escape_len);

    clnt->esc_offset = 0;
    clnt->mouse_active = 0;
    clnt->local_mouse_pointer_enable = 1;

    clnt_init_xkb(clnt, hp);

    return clnt;
}

void clnt_update_xkb_kb_layout(HAL *hp, CLNT *clnt)
{
	if (clnt == NULL)
        return;

    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX) {
        return;
    }

   clnt_init_xkb(clnt, hp);
}
/******************************************
 * Destroy the CLNT object                *
 ******************************************/
void clnt_release(CLNT *clnt) {

    if (clnt == NULL)
        return;

    VLOG_TRACE_CAT(VLOG_CLN, "Releasing CLNT object");
	xkb_CleanUp(clnt);

    free(clnt);
}

/***************************************
 * Client Override Parameters Routines *
 ***************************************/
void clnt_set_override_parameters(HAL *hp, CLNT *clnt) {
    struct LUT_struct lut;
    int slicesize, compmode, motioncompmode;

    if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE || clnt == NULL)
        return;

    if (hal_get_dropside_lut(hp, &lut) < 0)
        return;

    hal_get_dropside_nobpslice(hp, &slicesize);
    hal_get_dropside_compmode(hp, &compmode, &motioncompmode);

    configure_dropside_video(hp, clnt, &lut, slicesize, compmode, motioncompmode);
}

/**********************************
 * Client Keyboard/Mouse Routines *
 **********************************/
void clnt_set_local_keyboard_type(HAL *hp, CLNT *clnt, int kbtype) {
    int cno = hal_get_channel_number(hp);

    /* Do nothing for V2DI50 */
    if (hp == NULL || get_supported_hardware(hp) == 0 || clnt == NULL)
       return;

    /* XP200 - process based on KBM mode */
    if (clnt == NULL)
        return;

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. Setting local KB type to %s", cno,
            (kbtype == KB_TYPE_PC) ? "PC" : "SGI");
    clnt->local_keyboard_type = kbtype;
    if (clnt->curr_kbmode != KBM_MODE_REMOTE)
        hal_setKeyboardType(hp, kbtype);
}

void clnt_set_remote_keyboard_type(HAL *hp, CLNT *clnt, int kbtype) {
    int cno = hal_get_channel_number(hp);

    /* V2DI50 - set KB type in FPGA */
    if (get_supported_hardware(hp) == 0) {
        hal_setKeyboardType(hp, kbtype);
        return;
    }

    /* XP200 - process based on KBM mode */
    if (clnt == NULL)
        return;

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. Setting remote KB type to %s", cno,
            (kbtype == KB_TYPE_PC) ? "PC" : "SGI");
    clnt->remote_keyboard_type = kbtype;
    if (clnt->curr_kbmode == KBM_MODE_REMOTE)
        hal_setKeyboardType(hp, kbtype);
}

void clnt_set_local_mouse_type(HAL *hp, CLNT *clnt, int mstype) {
    int cno = hal_get_channel_number(hp);

    /* Do nothing for V2DI50 */
    if (hp == NULL || get_supported_hardware(hp) == 0 || clnt == NULL)
       return;

    VLOG_DEBUG_CAT(VLOG_KBM_MS,
            "Channel %d. Setting local MS type to %s", cno,
            (mstype == MS_TYPE_PS2) ? "PS2" : "IMPS2");
    clnt->local_mouse_type = mstype;
}

void clnt_set_remote_mouse_type(HAL *hp, CLNT *clnt, int mstype) {
    int cno = hal_get_channel_number(hp);

    /* Do nothing for V2DI50 */
    if (hp == NULL || get_supported_hardware(hp) == 0 || clnt == NULL)
       return;

    VLOG_DEBUG_CAT(VLOG_KBM_MS,
            "Channel %d. Setting local MS type to %s", cno,
            (mstype == MS_TYPE_PS2) ? "PS2" : "IMPS2");
    clnt->remote_mouse_type = mstype;
}

char *clnt_get_keyboard_mode(HAL *hp, CLNT *clnt, char *kbbuf) {

    sprintf(kbbuf, "????");
    if ((clnt != NULL) && (hp != NULL)) {
        if (hp->console_info.kb_active) {
            if(clnt->curr_kbmode == KBM_MODE_LOCAL)
                sprintf(kbbuf, "Local");
            else
                sprintf(kbbuf, "Remote");
        }
        else {
            sprintf(kbbuf, "Console");
        }
    }

    return kbbuf;
}

static int get_curr_keyboard_type(HAL *hp, CLNT *clnt) {
    if (!hp->console_info.kb_active && clnt->curr_kbmode == KBM_MODE_REMOTE)
        return clnt->remote_keyboard_type;
    return clnt->local_keyboard_type;
}

static void update_keyboard_escape(HAL *hp, CLNT *clnt) {
    int kbtype = get_curr_keyboard_type(hp, clnt);

    VLOG_DEBUG_CAT(VLOG_KBM_KB, 
            "Channel %d. Updating keyboard escape sequences", 
            hal_get_channel_number(hp));

    /* Update escape scan code sequences for current keyboard type */
    clnt->console_escape_len = get_keyboard_escape(clnt->console_escape, 
            clnt->console_escape_keycode, kbtype);
    clnt->local_escape_len = get_keyboard_escape(clnt->local_escape, 
            clnt->local_escape_keycode, kbtype);
    clnt->remote_escape_len = get_keyboard_escape(clnt->remote_escape, 
            clnt->remote_escape_keycode, kbtype);
}

void clnt_set_keyboard_mode(HAL *hp, CLNT *clnt, int mode) {
    int mode_changed = 0, cno = hal_get_board_number(hp) + 1;

    if (hp == NULL || clnt == NULL)
        return;

    VLOG_DEBUG_CAT(VLOG_KBM_KB, "Channel %d. Setting KBM mode to %s", cno,
            (mode == KBM_MODE_LOCAL) ? "Local" : 
            ((mode == KBM_MODE_REMOTE) ? "Remote" : "Console"));

    if (mode == KBM_MODE_CONSOLE && hp->console_info.kb_active) {
        if (set_keyboard_in_console_mode(1) < 0)
            return;
        mode_changed = 1;
    }
    else {
        if (!hp->console_info.kb_active && !is_console_kbmode(mode)) {
            if (set_keyboard_in_console_mode(0) < 0)
                return;
            mode_changed = 1;
        }
        if ((mode == KBM_MODE_REMOTE && clnt->curr_kbmode == KBM_MODE_LOCAL) ||
            (mode == KBM_MODE_LOCAL && clnt->curr_kbmode == KBM_MODE_REMOTE)) {
            mode_changed = 1;
        }
    }
    /* Update current mode and escape sequences if necessary */
    if (mode_changed) {
        if (mode == KBM_MODE_REMOTE) 
            hal_setKeyboardType(hp, clnt->remote_keyboard_type);
        else
            hal_setKeyboardType(hp, clnt->local_keyboard_type);
        clnt->curr_kbmode = mode;
        update_keyboard_escape(hp, clnt);
    }
}

void clnt_set_console_escape(HAL *hp, 
                             CLNT *clnt, 
                             int e1, 
                             int e2, 
                             int e3, 
                             int e4) {
    if (clnt == NULL)
        return;

    /* Set console escape keycodes */
    clnt->console_escape_keycode[0] = e1;
    clnt->console_escape_keycode[1] = e2;
    clnt->console_escape_keycode[2] = e3;
    clnt->console_escape_keycode[3] = e4;
    /* Set console escape scan code sequences  */
    clnt->console_escape_len = get_keyboard_escape(clnt->console_escape, 
            clnt->console_escape_keycode, get_curr_keyboard_type(hp, clnt));
    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. console escape 0x%x/0x%x/0x%x/0x%x. scan code:",
            hal_get_channel_number(hp), e1, e2, e3, e4);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->console_escape, clnt->console_escape_len);
}

void clnt_set_local_escape(HAL *hp,
                           CLNT *clnt, 
                           int e1, 
                           int e2, 
                           int e3, 
                           int e4) {
    if (clnt == NULL)
        return;
    /* Set local escape keycodes */
    clnt->local_escape_keycode[0] = e1;
    clnt->local_escape_keycode[1] = e2;
    clnt->local_escape_keycode[2] = e3;
    clnt->local_escape_keycode[3] = e4;
    /* Set local escape scan code sequences  */
    clnt->local_escape_len = get_keyboard_escape(clnt->local_escape, 
            clnt->local_escape_keycode, get_curr_keyboard_type(hp, clnt));
    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. local escape 0x%x/0x%x/0x%x/0x%x. scan code:",
            hal_get_channel_number(hp), e1, e2, e3, e4);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, 
            clnt->local_escape, clnt->local_escape_len);
}

void clnt_set_remote_escape(HAL *hp,
                            CLNT *clnt, 
                            int e1, 
                            int e2, 
                            int e3, 
                            int e4) {
    if (clnt == NULL)
        return;
    /* Set remote escape keycodes */
    clnt->remote_escape_keycode[0] = e1;
    clnt->remote_escape_keycode[1] = e2;
    clnt->remote_escape_keycode[2] = e3;
    clnt->remote_escape_keycode[3] = e4;
    /* Set remote escape scan code sequences  */
    clnt->remote_escape_len = get_keyboard_escape(clnt->remote_escape, 
            clnt->remote_escape_keycode, get_curr_keyboard_type(hp, clnt));
    VLOG_DEBUG_CAT(VLOG_KBM_KB,
            "Channel %d. remote escape 0x%x/0x%x/0x%x/0x%x. scan code:",
            hal_get_channel_number(hp), e1, e2, e3, e4);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, 
            clnt->remote_escape, clnt->remote_escape_len);
}

static void compute_break_code_len( CLNT *clnt, 
                                   int *local_last_break_len, 
                                   int *remote_last_break_len,
                                   int *console_last_break_len)
{
    int max_esc_char_len = MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS;

    /* The max break code len for a key release is 6,and the min is 4
     * if the esc sequence buffer is full , it means the last 6 bytes 
     *  of the buffer is filled with break code */

    if (clnt->local_escape_len < max_esc_char_len)
        *local_last_break_len = clnt->local_escape[clnt->local_escape_len];
    else
        *local_last_break_len =  max_esc_char_len - 6;
    
    if (clnt->remote_escape_len < max_esc_char_len)
        *remote_last_break_len = clnt->remote_escape[clnt->remote_escape_len];
    else
        *remote_last_break_len = max_esc_char_len - 6;

    if (clnt->console_escape_len < max_esc_char_len)
        *console_last_break_len = clnt->console_escape[clnt->console_escape_len];
    else
        *console_last_break_len = max_esc_char_len - 6;
}

static int check_keyboard_escape(CLNT *clnt, unsigned char *buf, int len) {
    int rc = ESC_NONE, rc_console, rc_local, rc_remote;
    int local_last_break_len, remote_last_break_len, console_last_break_len;

    compute_break_code_len(clnt, &local_last_break_len, 
                           &remote_last_break_len,
                           &console_last_break_len);


    rc_console = memcmp(buf, clnt->console_escape,
        (len < clnt->console_escape_len) ? len : clnt->console_escape_len);
    rc_local = memcmp(buf, clnt->local_escape,
        (len < clnt->local_escape_len) ? len : clnt->local_escape_len);
    rc_remote = memcmp(buf, clnt->remote_escape,
        (len < clnt->remote_escape_len) ? len : clnt->remote_escape_len);

    if (rc_console == 0 && len == clnt->console_escape_len) {
        rc = ESC_CONSOLE;
    }
    else if (rc_local == 0 && len == clnt->local_escape_len) {
        rc = ESC_LOCAL;
    }
    else if (rc_remote == 0 && len == clnt->remote_escape_len) {
        rc = ESC_REMOTE;
    }
    else if ((rc_console == 0 &&  (len == console_last_break_len)) ||
        (rc_local == 0 &&  (len == local_last_break_len )) ||
        (rc_remote == 0 &&  (len == remote_last_break_len ))) {
        rc = NEED_MORE_CHARS;
    } else if ((rc_console == 0 && len < clnt->console_escape_len) ||
        (rc_local == 0 && len < clnt->local_escape_len) ||
        (rc_remote == 0 && len < clnt->remote_escape_len)) {
        rc = ESC_PARTIAL_MATCH;
    }
    VLOG_TRACE_CAT(VLOG_KBM_KB, "%s: console escape (%d bytes)", 
            __func__, clnt->console_escape_len);
    VLOG_TRACE_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->console_escape, clnt->console_escape_len);
    VLOG_TRACE_CAT(VLOG_KBM_KB, "%s: local escape (%d bytes)", 
            __func__, clnt->local_escape_len);
    VLOG_TRACE_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->local_escape, clnt->local_escape_len);
    VLOG_TRACE_CAT(VLOG_KBM_KB, "%s: remote escape (%d bytes)", 
            __func__, clnt->remote_escape_len);
    VLOG_TRACE_HEXDATA_CAT(VLOG_KBM_KB,
            clnt->remote_escape, clnt->remote_escape_len);
    VLOG_TRACE_CAT(VLOG_KBM_KB, "%s returning %d", __func__, rc);
    return rc;
}

static int set_rx_kbmode(HAL *hp, int curr_mode, int new_mode) {
    char buff1[128], buff2[128];
    
    buff1[0] = buff2[0] = '\0';

    if (!is_console_kbmode(curr_mode) && is_console_kbmode(new_mode)) {
        sprintf(buff1, "set_kbconsole -kbconsole Yes");
    }
    else if (is_console_kbmode(curr_mode) && !is_console_kbmode(new_mode)) {
        sprintf(buff1, "set_kbconsole -kbconsole No");
    }
    if (!is_console_kbmode(new_mode)) {
        struct rx_kbm_config kbm_cfg;
        get_rx_kbm_config(hp, &kbm_cfg);
        if (new_mode != kbm_cfg.kbm_mode) 
            sprintf(buff2, "set_rx_kbm_parm -channelNumber %d -kbm_mode %s",
                hal_get_channel_number(hp), 
                (new_mode == KBM_MODE_REMOTE) ? "Remote" : "Local"); 
    }
    VLOG_DEBUG_CAT(VLOG_KBM_KB, "Channel %d. Setting KBM to %s",
        hal_get_channel_number(hp), 
        (new_mode == KBM_MODE_CONSOLE) ? "Console" :
        ((new_mode == KBM_MODE_REMOTE) ? "Remote" : "Local"));
    if (buff1[0] || buff2[0]) {
        if (buff1[0] != 0)
            (void) system_command(buff1);
        if (buff2[0] != 0) 
            (void) system_command(buff2);
    }
    return 0;
}

static int process_kbmode_change(HAL *hp,
                                 CLNT *clnt,
                                 int kbtype) {
    int rc, keycode, isbrk, esc, newmode;

    /***********************************
     * Do nothing if not enough chars  *
     ***********************************/
    keycode = keycode_match(clnt->kb_buf, clnt->kb_len, kbtype, &isbrk);
    if (keycode == NEED_MORE_CHARS) {
        return NEED_MORE_CHARS;
    }

    /* copy the key code in to local buffer to hold and compare 
     * the esc sequence */
    memcpy(&clnt->esc_buf[clnt->esc_offset], clnt->kb_buf, clnt->kb_len);
    clnt->esc_offset += clnt->kb_len;

    /*************************************
     * Check if switching among KB modes *
     *************************************/
    esc = check_keyboard_escape(clnt, clnt->esc_buf, clnt->esc_offset);
    if (esc ==  NEED_MORE_CHARS) {
        clnt->kb_offset = 0;
        clnt->kb_len = 0;
      //  clnt->esc_offset -= clnt->kb_len; /* with the last key code when the escape seq is expected to match we hold the last pressed key, not  sent to either remote or local mode just ignore the key after the match detected*/                                              
        return NEED_MORE_CHARS;
    }
    else if (esc == ESC_PARTIAL_MATCH) {
        return 0; /* processing only partial matched esc sequences*/
    }

    /*After scanning for escape sequences flush the buffers */
    memset(clnt->esc_buf, 0, MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS);
    clnt->esc_offset = 0;

    newmode = kbmode_from_escape_code(esc);

    /**********************************************
     * Do nothing if not switching keyboard modes *
     **********************************************/
    if (!is_escape_code(esc) || 
            (!hp->console_info.kb_active && (newmode == KBM_MODE_CONSOLE)) ||
            newmode == clnt->curr_kbmode) {
        clnt->last_keycode = (isbrk) ? 0 : keycode;
        return 0;
    }

     
    /********************************************************
     * Switching modes. Throw away everything in KB buffer. *
     ********************************************************/
    clnt->kb_len = clnt->kb_offset = clnt->last_keycode = 0;

    /**************************************************
     * If KBM is currently in local or remote modes,  *
     * send 'filler' break code if necessary.         *
     **************************************************/
    rc = 0;
    if (clnt->last_keycode && hp->console_info.kb_active) {
        unsigned char brkcode[12];
        int brklen, kbtype;

        if (clnt->curr_kbmode == KBM_MODE_REMOTE)
            kbtype = clnt->remote_keyboard_type;
        else 
            kbtype = clnt->local_keyboard_type;

        brklen = get_break_code(clnt->last_keycode, kbtype, brkcode);
        memcpy(clnt->kb_buf, brkcode, brklen);
        if (clnt->curr_kbmode == KBM_MODE_REMOTE) {
            rc = clnt->kb_len = brklen;
        }
        else {
            write_to_kb_fpga(hp, clnt->kb_buf, brklen);
        }
    }

    /**************************
     * Switch to new KBM mode *
     **************************/
    set_rx_kbmode(hp, hal_get_current_kbmode(hp), newmode);
    clnt->curr_kbmode = newmode;

    return rc;
}

int retrieve_client_keyboard_data(HAL *hp,
                                  CLNT *clnt,
                                  unsigned char *buf,
                                  int buflen) {
    int kbfd, len, cno = hal_get_channel_number(hp);

    if (hp == NULL || (kbfd = hal_get_keyboard_fd(hp)) < 0)
       return 0;

    if ((len = read(kbfd, buf, buflen)) <= 0) {
        if (len < 0)
            VLOG_WARNING_CAT(VLOG_KBM_KB,
                    "Channel %d. Error reading KB data %m (%d).", cno, len);
        return len;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        /* V2DI50 - hand every thing to caller */
        return len;
    }
    else {
        /* XP200 - process keyboard data */
        int rc = 0, kbtype;

        if (clnt == NULL)
            return 0;

        /* Update KBM mode */
        hal_update_virtual_console(hp);
        clnt->curr_kbmode = hal_get_current_kbmode(hp);

        VLOG_DEBUG_CAT(VLOG_KBM_KB,
                "Channel %d. Read %d bytes KB data", cno, len);
        VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, buf, len);

        /* No room to store data for processing, cleanup & start over */
        if (KB_BUFFER_SIZE < clnt->kb_offset + len) {
            VLOG_WARNING_CAT(VLOG_KBM_KB,
                    "Channel %d. KB buffer overflow. offset %d, len %d",
                    cno, clnt->kb_offset, len);
            clnt->kb_len = clnt->kb_offset = 0;
            return 0;
        }

        /* Copy data into buffer for processing */
        memcpy(&clnt->kb_buf[clnt->kb_offset], buf, len);
        clnt->kb_offset += len;
        clnt->kb_len += len;

        kbtype = (clnt->curr_kbmode == KBM_MODE_REMOTE) ?
            clnt->remote_keyboard_type : clnt->local_keyboard_type;

        /* Process KBM mode change escape sequences */
        rc = process_kbmode_change(hp, clnt, kbtype);

        /* Wait for enough chars to determine escape sequence */
        if (rc == NEED_MORE_CHARS) {
            return 0;
        }

        /* Ignore all keyboard inputs if in console mode */
        if (hp->console_info.kb_active == 0) {
            clnt->kb_len = clnt->kb_offset = 0;
            return 0;
        }

        /* If there's break code to send. Send it. */
        if (rc > 0) {
            memcpy(buf, clnt->kb_buf, clnt->kb_len);
            clnt->kb_len = clnt->kb_offset = 0;
            return rc;
        }

        /* Otherwise process what's in the KB buffer */
        if (clnt->curr_kbmode == KBM_MODE_LOCAL) {
            /* Local mode - write data to FPGA, i.e. loop back */
            write_to_kb_fpga(hp, clnt->kb_buf, clnt->kb_len);
        }
        else if (clnt->curr_kbmode == KBM_MODE_REMOTE) {
            /* Remote mode - hand everything to caller */
            memcpy(buf, clnt->kb_buf, clnt->kb_len);
            rc = clnt->kb_len;
        }
        else {
            VLOG_DEBUG_CAT(VLOG_KBM_KB, 
                    "Channel %d. mode %d. Discarded %d bytes KB data", 
                    cno, clnt->curr_kbmode, clnt->kb_len);
        }

        clnt->kb_len = clnt->kb_offset = 0;

        return rc;
    }
}

int retrieve_client_mouse_data(HAL *hp,
                               CLNT *clnt,
                               unsigned char *buf,
                               int buflen)
{
    int msfd, len, cno = hal_get_board_number(hp) + 1;

    if (hp == NULL || (msfd = hal_get_mouse_fd(hp)) < 0)
       return 0;

    if ((len = read(msfd, buf, buflen)) <= 0) {
        if (len < 0)
            VLOG_WARNING_CAT(VLOG_KBM_MS,
                    "Channel %d. Error reading MS data %m (%d).", cno, len);
        return len;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        /* V2DI50 - hand every thing to caller */
        return len;
    }
    else {
        /* XP200 - process mouse data */
        int rc = 0;

        if (clnt == NULL)
            return 0;

        VLOG_DEBUG_CAT(VLOG_KBM_MS,
                "Channel %d. Read %d bytes MS data", cno, len);
        VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_MS, buf, len);

        /* Update KBM mode */
        hal_update_virtual_console(hp);
        clnt->curr_kbmode = hal_get_current_kbmode(hp);

        /* Ignore all mouse inputs if in console mode */
        if (hp->console_info.kb_active == 0) {
            VLOG_DEBUG_CAT(VLOG_KBM_KB, 
                    "Channel %d. console mode. Discarded %d bytes MS data", 
                    cno, len);
            return 0;
        }

        /* Otherwise process what's in the KB buffer */
        if (clnt->curr_kbmode == KBM_MODE_LOCAL) {
            /* Local mode - write data to FPGA, i.e. loop back */
            write_to_ms_fpga(hp, buf, len, clnt->local_mouse_type);
        }
        else if (clnt->curr_kbmode == KBM_MODE_REMOTE) {
            /* Remote mode - hand everything to caller */
            rc = len;
        }
        else {
            VLOG_DEBUG_CAT(VLOG_KBM_MS, 
                    "Channel %d. mode %d. Discarded %d bytes MS data", 
                    cno, clnt->curr_kbmode, len);
        }
        return rc;
    }
}

int retrieve_client_event_data(HAL *hp,
                               CLNT *clnt,
                               struct event_s *eventp,
                               unsigned char *buf,
                               int *buflen,
                               int maxbuflen,
                               unsigned char *xkbbuf,
                               int *xkblen) {

    int len = 0, rc = 0, evlen, i;
    int cno = hal_get_board_number(hp) + 1, kbtype, mstype;
    unsigned char evbuf[EVENT_BUFFER_SIZE + 1];
    struct input_event *ev;

    if (hp == NULL || get_supported_hardware(hp) == 0) {
       /* V2DI50 - no USB event support */
       return 0;
    }

    /* XP200 - read and process event data */
    if ((evlen = read(eventp->event_fd, evbuf, EVENT_BUFFER_SIZE)) <= 0)
        return evlen;

    VLOG_DEBUG_CAT(VLOG_KBM_EV,
            "Channel %d. Eventfd %d. Read %d bytes Event data",
            cno, eventp->event_fd, evlen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_EV, evbuf, evlen);

    /* Update KBM mode */
    hal_update_virtual_console(hp);
    clnt->curr_kbmode = hal_get_current_kbmode(hp);

    /* Initialize data buffer lengths to be returned to caller */
    *buflen = 0;

    /* Skip forward to first keyboard or mouse event */
    for (i = 0; i < evlen; i += sizeof(struct input_event)) {
        ev = (struct input_event *) &evbuf[i];
        if (KB_EVENT(ev->type, ev->code) || MS_EVENT(ev->type, ev->code))
            break;
    }

    if (i >= evlen)
        return 0;

    evlen -= i;

    /* Convert events into keyboard or mouse data */
    if (KB_EVENT(ev->type, ev->code)) {

        kbtype = (clnt->curr_kbmode == KBM_MODE_REMOTE) ?
            clnt->remote_keyboard_type : clnt->local_keyboard_type;

        len = fill_kb_buffer(buf, maxbuflen, ev, evlen, kbtype, 1);
        if (len > 0) {
            VLOG_DEBUG_CAT(VLOG_KBM_KB,
                    "Channel %d. Read %d bytes USBKB data", cno, len);
            VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, buf, len);

            /* Copy data into buffer for processing */
            memcpy(&clnt->kb_buf[clnt->kb_offset], buf, len);
            clnt->kb_offset += len;
            clnt->kb_len += len;

            /* Process KBM mode change escape sequences  */
            rc = process_kbmode_change(hp, clnt, kbtype);

            /* Wait for enough chars to determine escape sequence */
            if (rc == NEED_MORE_CHARS)
                return 0;

            *xkblen = clnt_process_xkb(clnt, ev, evlen, xkbbuf);

            /* Ignore all keyboard inputs if in console mode */
            if (hp->console_info.kb_active == 0) {
                clnt->kb_len = clnt->kb_offset = 0;
                return 0;
            }

            /* If there's break code to send. Send it. */
            if (rc > 0) {
                memcpy(buf, clnt->kb_buf, clnt->kb_len);
                *buflen = clnt->kb_len;
                clnt->kb_len = clnt->kb_offset = 0;
                return KBM_TYPE_KEYBOARD;
            }

            rc = 0;

            /* Otherwise process what's in the KB buffer */
            if (clnt->curr_kbmode == KBM_MODE_LOCAL && hal_get_hardware_type(hp) != XPI_DEVICE) {
                /* Local mode - write data to FPGA, i.e. loop back */
                write_to_kb_fpga(hp, clnt->kb_buf, clnt->kb_len);
            }
            else if (clnt->curr_kbmode == KBM_MODE_LOCAL && hal_get_hardware_type(hp) == XPI_DEVICE) {
                *buflen = clnt->kb_len;
                rc = KBM_TYPE_KEYBOARD;

            }
            else if (clnt->curr_kbmode == KBM_MODE_REMOTE) {
                /* Remote mode - hand everything to caller */
                memcpy(buf, clnt->kb_buf, clnt->kb_len);
                *buflen = clnt->kb_len;
                rc = KBM_TYPE_KEYBOARD;
            }
            else {
                VLOG_DEBUG_CAT(VLOG_KBM_KB, 
                        "Channel %d. mode %d. Discarded %d bytes USBKB data", 
                        cno, clnt->curr_kbmode, len);
            }
            clnt->kb_len = clnt->kb_offset = 0;
        }
     }
     else if (MS_EVENT(ev->type, ev->code)) {
        int state;

        /* Ignore all mouse inputs if in console mode */
        /* To Do fix this for XPI_DEVICE */
        if (hal_get_hardware_type(hp) == PLX_DEVICE )
        {
          if (hp->console_info.kb_active == 0) {
            return 0;
          }
        }

        mstype = (clnt->curr_kbmode == KBM_MODE_REMOTE) ?
            clnt->remote_mouse_type : clnt->local_mouse_type;

        len = fill_ms_buffer(buf, maxbuflen, ev, evlen, mstype,
                                         eventp->event_state, &state);
        if (eventp->event_state != state)
            eventp->event_state = state;

        if (len > 0) {
            VLOG_DEBUG_CAT(VLOG_KBM_MS,
                    "Channel %d. Read %d bytes USBMS data", cno, len);
            VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_MS, buf, len);

            rc = 0;

            if (clnt->curr_kbmode == KBM_MODE_LOCAL && hal_get_hardware_type(hp) != XPI_DEVICE) {
                /* Local mode - write data to FPGA, i.e. loop back */
                write_to_ms_fpga(hp, buf, len, clnt->local_mouse_type);
            }
            else if (clnt->curr_kbmode == KBM_MODE_LOCAL && hal_get_hardware_type(hp) == XPI_DEVICE) {
                *buflen = len;
                rc = KBM_TYPE_MOUSE;

            }
            else if (clnt->curr_kbmode == KBM_MODE_REMOTE) {
                /* Otherwise hand everything to caller */
                *buflen = len;
                rc = KBM_TYPE_MOUSE;
                clnt->mouse_active = 1;
            }
            else {
                VLOG_DEBUG_CAT(VLOG_KBM_MS, 
                        "Channel %d. mode %d. Discarded %d bytes USBMS data", 
                        cno, clnt->curr_kbmode, len);
            }
        }
    }

    return rc;
}

/*********************************
 * Client RFB Keyboard Routines  *
 *********************************/
int clnt_keyboard_data_to_rfb(CLNT *clnt,
                              unsigned char *kbbuf,
                              unsigned char *rfbbuf,
                              int kblen) {
    int rfblen = 0;

    if (clnt == NULL)
        return 0;

    VLOG_DEBUG_CAT(VLOG_KBM_RFB,
            "Converting KB data (%d bytes) to RFB:", kblen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, kbbuf, kblen);

    if (clnt->remote_keyboard_type == KB_TYPE_SGI)
        rfblen = xlate_set3_to_rfb(clnt->kb_buf, rfbbuf,
                &(clnt->rfb_info.numlock), &(clnt->rfb_info.capslock),
                &(clnt->rfb_info.shift), kblen);
    else
        rfblen = xlate_set2_to_rfb(clnt->kb_buf, rfbbuf,
                &(clnt->rfb_info.numlock), &(clnt->rfb_info.capslock),
                &(clnt->rfb_info.shift), kblen);

    VLOG_DEBUG_CAT(VLOG_KBM_RFB, "Converted RFB data (%d bytes):", rfblen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, rfblen);

    return rfblen;
}

/***************************************
 * Client Local Mouse Cursor Routines  *
 ***************************************/
#define CURSOR_FLAG_T1      0x01  /* 1 top row cut off from cursor */
#define CURSOR_FLAG_T2      0x02  /* 2 top rows cut off */
#define CURSOR_FLAG_B1      0x04  /* 1 bottom row cut off */
#define CURSOR_FLAG_B2      0x08  /* 2 bottom rows cut off */
#define CURSOR_FLAG_L1      0x10  /* 1 left column cut off */
#define CURSOR_FLAG_L2      0x20  /* 2 left columns cut off */
#define CURSOR_FLAG_R1      0x40  /* 1 right column cut off */
#define CURSOR_FLAG_R2      0x80  /* 2 right columns cut off */

void clnt_init_mouse_cursor(HAL *hp, CLNT *clnt) {

    if (hp == NULL || clnt == NULL )
        return;
    hal_init_mouse_cursor(hp, clnt->rfb_info.abs_x, clnt->rfb_info.abs_y,
            clnt->rfb_info.size_x, clnt->rfb_info.size_y);
}

static int reserved_dropside_lut(HAL *hp)
{
    struct LUT_struct lut;
    if (hal_get_dropside_lut(hp, &lut) == 0) {
        if (lut.a_alutreg >= RESERVED_MIN_STD_LUTID &&
                lut.a_alutreg != RESERVED_OVERRIDE_LUTID &&
                lut.a_alutreg != RESERVED_AUTO_LUTID) {
            return 1;
        }
    }
    return 0;
}

void clnt_update_mouse_cursor_display(HAL *hp, CLNT *clnt) {
    struct ov_param_io oi;

    if (hp == NULL || clnt == NULL)
        return;

    if (hal_get_overlay_info(hp, &oi) < 0) {
        VLOG_WARNING_CAT(VLOG_KBM_RFB, 
                    "Channel %d. Failed to get overlay info", 
                    hal_get_channel_number(hp));
        return;
    }

    if (reserved_dropside_lut(hp)) {
        clnt->rfb_info.status &= ~RFB_STATUS_DISPLAY;
    }
    else if (hp->console_info.kb_active && 
            (clnt->curr_kbmode == KBM_MODE_REMOTE) && 
            (oi.flags & (OV_FLAG_SRC_REMOTE|OV_FLAG_SRC_OVERLAY)) && 
            (clnt->mouse_active) && clnt->local_mouse_pointer_enable ) {
        clnt->rfb_info.status |= RFB_STATUS_DISPLAY;
    }
    else {
        clnt->rfb_info.status &= ~RFB_STATUS_DISPLAY;
    }

    if ((clnt->rfb_info.status & (RFB_STATUS_ENABLE|RFB_STATUS_DISPLAY)) ==
           (RFB_STATUS_ENABLE|RFB_STATUS_DISPLAY)) {
        VLOG_DEBUG_CAT(VLOG_KBM_RFB,
                "Channel %d. Setting Local Mouse to %d, %d",
                hal_get_channel_number(hp), clnt->rfb_info.abs_x,
                clnt->rfb_info.abs_y);
        hal_set_mouse_cursor_pos(hp, clnt->rfb_info.abs_x,
                clnt->rfb_info.abs_y, clnt->rfb_info.size_x,
                clnt->rfb_info.size_y);
        hal_set_mouse_cursor_status(hp, 1);
    }
    else {
        VLOG_DEBUG_CAT(VLOG_KBM_RFB, "Channel %d. Disabling Local Mouse",
            hal_get_channel_number(hp));
        hal_set_mouse_cursor_status(hp, 0);
    }
}

void clnt_set_mouse_cursor_enable(HAL *hp, CLNT *clnt, int enable) {
    if (hp == NULL || clnt == NULL)
        return;
    if (((clnt->rfb_info.status & RFB_STATUS_ENABLE) && !enable) ||
        (!(clnt->rfb_info.status & RFB_STATUS_ENABLE) && enable)) {
        if (enable)
            clnt->rfb_info.status |= RFB_STATUS_ENABLE;
        else
            clnt->rfb_info.status &= ~RFB_STATUS_ENABLE;

        clnt_update_mouse_cursor_display(hp, clnt);
    }
}

void clnt_update_mouse_cursor_pos(HAL *hp, CLNT *clnt) {
    unsigned char cursor_flag = 0;
    unsigned short x, y;

    if (hp == NULL || clnt == NULL )
        return;

    if (clnt->rfb_info.abs_x > clnt->rfb_info.size_x ||
            clnt->rfb_info.abs_y > clnt->rfb_info.size_y)
        return;

    if ((x = clnt->rfb_info.abs_x) < MOUSECURSOR_CENTER) {
        cursor_flag |= ((MOUSECURSOR_CENTER - x) == 1) ? CURSOR_FLAG_L1 :
            CURSOR_FLAG_L2;
    }
    else if (x > clnt->rfb_info.size_x - MOUSECURSOR_CENTER) {
        cursor_flag |=
            ((MOUSECURSOR_CENTER + x - clnt->rfb_info.size_x) == 1) ?
            CURSOR_FLAG_R1 : CURSOR_FLAG_R2;
    }
    if ((y = clnt->rfb_info.abs_y) < MOUSECURSOR_CENTER) {
        cursor_flag |= ((MOUSECURSOR_CENTER - y) == 1) ? CURSOR_FLAG_T1 :
            CURSOR_FLAG_T2;
    }
    else if (y > clnt->rfb_info.size_y - MOUSECURSOR_CENTER) {
        cursor_flag |=
            ((MOUSECURSOR_CENTER + y - clnt->rfb_info.size_y) == 1) ?
            CURSOR_FLAG_B1 : CURSOR_FLAG_B2;
    }

    if (cursor_flag != clnt->rfb_info.cursor_flag) {
        clnt->rfb_info.cursor_flag = cursor_flag;
        hal_init_mouse_cursor(hp, clnt->rfb_info.abs_x,
                clnt->rfb_info.abs_y, clnt->rfb_info.size_x,
                clnt->rfb_info.size_y);
    }
    else {
        hal_set_mouse_cursor_pos(hp, clnt->rfb_info.abs_x,
                clnt->rfb_info.abs_y, clnt->rfb_info.size_x,
                clnt->rfb_info.size_y);
    }
    clnt_update_mouse_cursor_display(hp, clnt);
}

void clnt_disable_mouse_pointer (HAL *hp, CLNT *clnt)
{
    if (hp == NULL || clnt == NULL)
        return;

     hal_set_mouse_cursor_status(hp, 0);

}

static int client_conn_config_handler(char *name, char *value, void *ptr) {

    struct client_conn_config *cc_cfg = ptr;

    if (strcasecmp(name, S_SERVER_ADDRESS) == 0) {
        strncpy(cc_cfg->server_address, value, SERVER_ADDRESS_LEN);
        cc_cfg->server_address[SERVER_ADDRESS_LEN - 1] = 0;
    }
    else if (strcasecmp(name, S_SERVER_PORT) == 0) {
        cc_cfg->server_port = atoi(value);
    }
    else if (strcasecmp(name, S_MAX_BW) == 0) {
        cc_cfg->av_max_bw = atoi(value);
    }
    else if (strcasecmp(name, S_MULTICAST) == 0) {
        if (strcasecmp(value, "yes") == 0)
            cc_cfg->multicast = 1;
        else
            cc_cfg->multicast = 0;
    }
    else if (strcasecmp(name, S_AVOPTION) == 0) {
        if (strcasecmp(value, "both") == 0)
            cc_cfg->avoption = AVOPTION_AUDIO_VIDEO;
        else if (strcasecmp(value, "audio") == 0)
            cc_cfg->avoption = AVOPTION_AUDIO;
        else
            cc_cfg->avoption = AVOPTION_VIDEO;
    }
    else if (strcasecmp(name, S_AUDIO_BIDIR) == 0) {
        if (strcasecmp(value, "yes") == 0)
            cc_cfg->bidir = 1;
        else
            cc_cfg->bidir = 0;
    }
    else if (strcasecmp(name, S_VIDEO_PORT) == 0) {
        cc_cfg->videoport = atoi(value);
    }
    else if (strcasecmp(name, S_AUDIO_PORT) == 0) {
        cc_cfg->audioport = atoi(value);
    }
    else if (strcasecmp(name, S_RFBCONNECTION) == 0) {
        if (strcasecmp(value, "yes") == 0)
            cc_cfg->rfb_connection = 1;
        else
            cc_cfg->rfb_connection = 0;
    }
    else if (strcasecmp(name, S_RFB_PORT) == 0) {
        cc_cfg->rfb_port = atoi(value);
    }
    else if (strcasecmp(name, S_RFB_PASSWORD) == 0) {
        strncpy(cc_cfg->rfb_password, value, 32);
        cc_cfg->rfb_password[32] = 0;
    }
    else
        return -1;

    return 0;
}


int get_client_conn_config(int board_number, struct client_conn_config *cc_cfg) {

    static char config_file_name[128];

    /*************************
     * Initialize to default *
     *************************/
    bzero(cc_cfg, sizeof(struct client_conn_config));
    strcpy(cc_cfg->server_address, "0.0.0.0");
    cc_cfg->server_port = SERVER_PORT;
    cc_cfg->connect_enable = 0;
    cc_cfg->av_max_bw = 5 * 1024;
    cc_cfg->avoption = DEFAULT_AVOPTION;
    cc_cfg->bidir = 0;
    cc_cfg->samplesize = DEFAULT_SAMPLESIZE;
    cc_cfg->monostereo = DEFAULT_MONOSTEREO;
    cc_cfg->videoport = 0;
    cc_cfg->audioport = 0;
    cc_cfg->rfb_connection = 0;
    cc_cfg->rfb_port = 0;
    strcpy(cc_cfg->rfb_password, "");

    /*************************
     * Read config from file *
     *************************/
    sprintf(config_file_name, "/V2O/config/clientconnconf_%d.txt", board_number);
    parse_configuration_file(config_file_name,
                              client_conn_config_handler, cc_cfg);

    if (cc_cfg->videoport == 0)
        cc_cfg->videoport = VIDEO_PORT;

    if (cc_cfg->audioport == 0)
        cc_cfg->audioport = cc_cfg->videoport + 1;

    if (strcmp(cc_cfg->server_address, "0.0.0.0") != 0)
        cc_cfg->connect_enable = 1;

    return 0;
}

int get_supported_hardware(HAL *hp)
{
  int rc = 0;
  int hw_type = hal_get_hardware_type(hp);
  switch (hw_type)
  {
    case PLX_DEVICE:
    case XPI_DEVICE:
      rc = 1;
    break;
    case V2DI50_DEVICE:
      rc = 0;
    break;
    default:
      rc =0;
    break;
  }

   return rc;
}
