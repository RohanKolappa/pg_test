#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_console.h"
#include "vutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "serverutils.h"
#include "configureSystem.h"
#include "vlog.h"
#include "kbms_utils.h"
#include "kbconsole_config.h"
#include "kbmode_config.h"
#include "tx_kbm_config.h"

#define S_AUDIO_DSCPCLASS       "AUDIO_DSCPCLASS"
#define S_VIDEO_DSCPCLASS       "VIDEO_DSCPCLASS"
#define S_TCP_DSCPCLASS         "TCP_DSCPCLASS"

static int dscpclass_name_to_value(char *name)
{
    if (strcasecmp(name, "EF") == 0)
        return 46;
    else if (strcasecmp(name, "CS7") == 0)
        return 56;
    else if (strcasecmp(name, "CS6") == 0)
        return 48;
    else if (strcasecmp(name, "CS5") == 0)
        return 40;
    else if (strcasecmp(name, "CS4") == 0)
        return 32;
    else if (strcasecmp(name, "CS3") == 0)
        return 24;
    else if (strcasecmp(name, "CS2") == 0)
        return 16;
    else if (strcasecmp(name, "CS1") == 0)
        return 8;
    else if (strcasecmp(name, "AF43") == 0)
        return 38;
    else if (strcasecmp(name, "AF42") == 0)
        return 36;
    else if (strcasecmp(name, "AF41") == 0)
        return 34;
    else if (strcasecmp(name, "AF33") == 0)
        return 30;
    else if (strcasecmp(name, "AF32") == 0)
        return 28;
    else if (strcasecmp(name, "AF31") == 0)
        return 26;
    else if (strcasecmp(name, "AF23") == 0)
        return 22;
    else if (strcasecmp(name, "AF22") == 0)
        return 20;
    else if (strcasecmp(name, "AF21") == 0)
        return 18;
    else if (strcasecmp(name, "AF13") == 0)
        return 14;
    else if (strcasecmp(name, "AF12") == 0)
        return 12;
    else if (strcasecmp(name, "AF11") == 0)
        return 10;
    else if (strcasecmp(name, "BE") == 0)
        return 0;
    else
        return -1;
}

char *dscpclass_value_to_name(int value)
{
    if (value == 46)
        return "EF";
    else if (value == 56)
        return "CS7";
    else if (value == 48)
        return "CS6";
    else if (value == 40)
        return "CS5";
    else if (value == 32)
        return "CS4";
    else if (value == 24)
        return "CS3";
    else if (value == 16)
        return "CS2";
    else if (value == 8)
        return "CS1";
    else if (value == 38)
        return "AF43";
    else if (value == 36)
        return "AF42";
    else if (value == 34)
        return "AF41";
    else if (value == 30)
        return "AF33";
    else if (value == 28)
        return "AF32";
    else if (value == 26)
        return "AF31";
    else if (value == 22)
        return "AF23";
    else if (value == 20)
        return "AF22";
    else if (value == 18)
        return "AF21";
    else if (value == 14)
        return "AF13";
    else if (value == 12)
        return "AF12";
    else if (value == 10)
        return "AF11";
    else
        return "BE";
}

/***************************************
 * Create and initialize a SRVR object *
 ***************************************/

SRVR *srvr_acquire(HAL *hp) {
    SRVR *srvr;
    int i, cno;
    struct tx_kbm_config kbm_cfg;
    struct kbmode_config kbmode_cfg;

    /* Board must be XP100/200 TX */
    if (hp == NULL ||  get_supported_hardware(hp) == 0 ||
            hal_get_board_type(hp) != BOARDTYPE_TX) {
        return NULL;
    }

    cno = hal_get_channel_number(hp);

    /* Allocate the SRVR structure */
    VLOG_TRACE_CAT(VLOG_SVR, "Allocating SRVR object for Channel %d", cno);
    srvr = (SRVR *) calloc(1, sizeof(SRVR));
    if (srvr == NULL) {
        VLOG_CRITICAL_CAT(VLOG_SVR, 
                "Memory allocation failed for Channel %d. %m.", cno);
        return NULL;
    }

    /* Initialize KBM mode */
    hal_update_virtual_console(hp);

    /* Initialize keyboard console mode escape sequence */
    get_tx_kbm_config(hp, &kbm_cfg);
    get_kbcons_settings(hp, &kbmode_cfg);
    for (i = 0; i < MAX_ESC_CHARS; i++)
        srvr->console_escape_keycode[i] = kbmode_cfg.esc[i];

    srvr->console_escape_len = get_keyboard_escape(srvr->console_escape, 
                kbmode_cfg.esc, kbm_cfg.keyboard_type);

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
        "Channel %d. Setting keyboard console escape to (len %d):", 
        cno, srvr->console_escape_len);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
        srvr->console_escape, srvr->console_escape_len);

    /* Initialize keyboard local mode escape sequence */
    get_tx_kblocal_settings(hp, &kbmode_cfg);
    for (i = 0; i < MAX_ESC_CHARS; i++)
        srvr->local_escape_keycode[i] = kbmode_cfg.esc[i];

    srvr->local_escape_len = get_keyboard_escape(srvr->local_escape, 
                kbmode_cfg.esc, kbm_cfg.keyboard_type);

    VLOG_DEBUG_CAT(VLOG_KBM_KB,
        "Channel %d. Setting keyboard local escape to (len %d):", 
        cno, srvr->local_escape_len);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
        srvr->local_escape, srvr->local_escape_len);

    return srvr;
}

/******************************************
 * Destroy the SRVR object                *
 ******************************************/
void srvr_release(SRVR *srvr) {
    if (srvr == NULL)
        return;
    VLOG_TRACE_CAT(VLOG_SVR, "Releasing SRVR object");
    free(srvr);
}

static int server_config_handler(char *name, char *value, void *ptr) {

    struct server_config *s_cfg = ptr;
    
    if (strcasecmp(name, S_MCAST_ADDRESS) == 0) {
        strncpy(s_cfg->mcast_address, value, SERVER_ADDRESS_LEN);
    }
    else if (strcasecmp(name, S_MULTICAST) == 0) {
        if (strcasecmp(value, "yes") == 0)
            s_cfg->multicast = 1;
        else
            s_cfg->multicast = 0;
    }
    else if (strcasecmp(name, S_CTL_PORT) == 0) {
        s_cfg->ctl_port = atoi(value);
    }
    else if (strcasecmp(name, S_REV_AUDIO_PORT) == 0) {
        s_cfg->rev_audio_port = atoi(value);
    }
    else if (strcasecmp(name, S_MCAST_TTL) == 0) {
        s_cfg->mcast_ttl = atoi(value);
    }
    else if (strcasecmp(name, S_ENABLE_444) == 0) {
        if (strcasecmp(value, "yes") == 0)
            s_cfg->enable_444 = 1;
        else
            s_cfg->enable_444 = 0;
    }
    else if (strcasecmp(name, S_ALLOW_KBMSWITCH) == 0) {
        if (strcasecmp(value, "yes") == 0)
            s_cfg->allow_kbmswitch= 1;
        else
            s_cfg->allow_kbmswitch = 0;
    }
    else if (strcasecmp(name, S_KBM_IDLETIME) == 0) {
        s_cfg->kbm_idletime = atoi(value);
    }
    else if (strcasecmp(name, S_KBM_PACING_DELAY) == 0) {
#ifdef TODO
        if ((get_fpga_val_k(FPGA_IDREG) & 0xff) == 0x01) 
#endif
            s_cfg->kbm_pacing_delay = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_STEREO) == 0) {
        if (strcasecmp(value, "yes") == 0)
            s_cfg->splash_stereo = 1;
        else if (strcasecmp(value, "no") == 0)
            s_cfg->splash_stereo = 0;
        else
            s_cfg->splash_stereo = 0xff;
    }
    else if (strcasecmp(name, S_SPLASH_HRES) == 0) {
        s_cfg->splash_orig_hres = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_VRES) == 0) {
        s_cfg->splash_orig_vres = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_REFRESH) == 0) {
        sscanf(value, "%lf", &(s_cfg->splash_refresh));
    }
    else if (strcasecmp(name, S_SPLASH_PCR) == 0) {
        sscanf(value, "%lf", &(s_cfg->splash_pcr));
    }
    else if (strcasecmp(name, S_SPLASH_HFP) == 0) {
        s_cfg->splash_hfp = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_HBP) == 0) {
        s_cfg->splash_hbp = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_HSYNC) == 0) {
        s_cfg->splash_hsync = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_VFP) == 0) {
        s_cfg->splash_vfp = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_VBP) == 0) {
        s_cfg->splash_vbp = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_VSYNC) == 0) {
        s_cfg->splash_vsync = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_MQUANT) == 0) {
        s_cfg->splash_mquant = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_DCQUANT) == 0) {
        s_cfg->splash_dcquant = atoi(value);
    }
    else if (strcasecmp(name, S_SPLASH_RATE) == 0) {
        strncpy(s_cfg->splash_rate, value, BANDWIDTH_LEN);
        s_cfg->splash_rate[BANDWIDTH_LEN] = 0;
    }
    else if (strcasecmp(name, S_ENABLE_SPLASH) == 0) {
        if (strcasecmp(value, "yes") == 0)
            s_cfg->enable_splash = 1;
        else
            s_cfg->enable_splash = 0;
    }
    else if (strcasecmp(name, S_SPLASH_SCREEN) == 0) {
        if (strncasecmp(value, "grid", 4) == 0)
            s_cfg->tx_splash = SPLASH_TYPE_GRID;
        else if (strncasecmp(value, "blue", 4) == 0)
            s_cfg->tx_splash = SPLASH_TYPE_BLUE;
        else
            s_cfg->tx_splash = SPLASH_TYPE_BLUE;
    }
    else if (strcasecmp(name, S_SERIAL_PORT) == 0) {
        s_cfg->serialport = atoi(value);
    }
    else if (strcasecmp(name, S_SERIAL_BAUDRATE) == 0) {
        s_cfg->baudrate = atoi(value);
    }
    else if (strcasecmp(name, S_SERIAL_DATABITS) == 0) {
        s_cfg->databits = atoi(value);
    }
    else if (strcasecmp(name, S_SERIAL_STOPBITS) == 0) {
        s_cfg->stopbits = atoi(value);
    }
    else if (strcasecmp(name, S_SERIAL_PARITY) == 0) {
        s_cfg->parity = (value[0] == 'e' || value[0] == 'E') ? 'e' :
            ((value[0] == 'o' || value[0] == 'O') ? 'o' : 'n');
    }
    else if (strcasecmp(name, S_SERIAL_FLOWCONTROL) == 0) {
        s_cfg->flowcontrol = (value[0] == 'h' || value[0] == 'H') ? 'h' :
            ((value[0] == 's' || value[0] == 'S') ? 's' : 'n');
    }
    else if (strcasecmp(name, S_SERIAL_WAIT) == 0) {
        s_cfg->serialwait = atoi(value);
    }
    else if (strcasecmp(name, S_KB_TYPE) == 0) {
        s_cfg->kb_type = (strcasecmp(value, "SGI") == 0) ? KB_TYPE_SGI :
            KB_TYPE_PC;
    }
    else if (strcasecmp(name, S_MS_TYPE) == 0) {
        s_cfg->ms_type = (strcasecmp(value, "IMPS2") == 0) ? MS_TYPE_IMPS2 :
            MS_TYPE_PS2;
    }
    else if (strcasecmp(name, S_VIDEO_DSCPCLASS) == 0) {
        s_cfg->video_dscpclass = dscpclass_name_to_value(value);
    }
    else if (strcasecmp(name, S_AUDIO_DSCPCLASS) == 0) {
        s_cfg->audio_dscpclass = dscpclass_name_to_value(value);
    }
    else if (strcasecmp(name, S_TCP_DSCPCLASS) == 0) {
        s_cfg->tcp_dscpclass = dscpclass_name_to_value(value);
    }
    else if (strcasecmp(name, S_VIDEO_DUMP) == 0) {
        s_cfg->debugflags |= (!strcasecmp(value, "Yes") ? DEBUG_FLAG_VIDEODUMP : 0);
    }
    else if (strcasecmp(name, S_AUDIO_DUMP) == 0) {
        s_cfg->debugflags |= (!strcasecmp(value, "Yes") ? DEBUG_FLAG_AUDIODUMP : 0);
    }
    else if (strcasecmp(name, S_REV_AUDIO_DUMP) == 0) {
        s_cfg->debugflags |= (!strcasecmp(value, "Yes") ? DEBUG_FLAG_REVAUDIODUMP : 0);
    }
    else
        return -1;

    return 0;
}


int get_server_config(struct server_config *s_cfg) {
    int i;

    bzero(s_cfg, sizeof(struct server_config));

    strcpy(s_cfg->mcast_address, "226.1.1.1");
    s_cfg->ctl_port = SERVER_PORT;
    s_cfg->multicast = 0;
    s_cfg->mcast_ttl = 1;
    s_cfg->rev_audio_port = AUDIO_PORT;
    s_cfg->enable_444 = 0;
    s_cfg->allow_kbmswitch = 0;
    s_cfg->kbm_idletime = 5;
#ifdef TODO
    s_cfg->kbm_pacing_delay = ((get_fpga_val_k(FPGA_IDREG) & 0xff) == 0x01) ? 25 : 0;
#else
    s_cfg->kbm_pacing_delay = 0;
#endif
    s_cfg->kb_type = KB_TYPE_PC;
    s_cfg->ms_type = MS_TYPE_PS2;
    s_cfg->splash_orig_hres = 1280;
    s_cfg->splash_orig_vres = 1024;
    s_cfg->splash_refresh = 72.0;
    s_cfg->splash_pcr = 128.822;
    s_cfg->splash_hfp = 40;
    s_cfg->splash_hbp = 240;
    s_cfg->splash_hsync = 120;
    s_cfg->splash_vfp = 3;
    s_cfg->splash_vbp = 35;
    s_cfg->splash_vsync = 3;
    s_cfg->splash_mquant = 3;
    s_cfg->splash_dcquant = 3;
    s_cfg->splash_slicesize = 32;
    s_cfg->enable_splash = 1;
    s_cfg->tx_splash = SPLASH_TYPE_BLUE;
    strcpy(s_cfg->splash_rate, "1.00M");
    s_cfg->serialport = 1;
    s_cfg->baudrate = 9600;
    s_cfg->databits = 8;
    s_cfg->stopbits = 1;
    s_cfg->parity = 'n';
    s_cfg->flowcontrol = 'n';
    s_cfg->serialwait = 0;
    s_cfg->tcp_dscpclass = 0;
    s_cfg->video_dscpclass = 0;
    s_cfg->audio_dscpclass = 0;
    s_cfg->debugflags = 0;
    
    parse_config_file(SERVER_CONFIG_FILE, server_config_handler, s_cfg);

    s_cfg->splash_hres = (s_cfg->splash_orig_hres + 15) / 16 * 16;
    s_cfg->splash_hres = (s_cfg->splash_orig_vres + 7) / 8 * 8;
    for (i = 32; i >= 20; i--) {
        if ((((s_cfg->splash_hres * s_cfg->splash_vres) / 128) % i) == 0) {
            s_cfg->splash_slicesize = i;
            break;
        }
    }
    
    /*************************************
     * Check for valid multicast address *
     *************************************/
    {
        struct in_addr mcastAddr;
        if (!inet_aton(s_cfg->mcast_address, &mcastAddr) ||
            !IN_MULTICAST(ntohl(mcastAddr.s_addr))) {
            VLOG_CRITICAL_CAT(VLOG_SVR,
                    "Invalid multicast addrress %s.", s_cfg->mcast_address);
            strcpy(s_cfg->mcast_address, "226.1.1.1");
        }
    }

    return 0;
}

void srvr_set_console_escape(HAL *hp,
                             SRVR *srvr, 
                             int e1, 
                             int e2, 
                             int e3, 
                             int e4,
                             int kbtype) {
    if (srvr == NULL)
        return;
    /* Set console escape keycodes */
    srvr->console_escape_keycode[0] = e1;
    srvr->console_escape_keycode[1] = e2;
    srvr->console_escape_keycode[2] = e3;
    srvr->console_escape_keycode[3] = e4;
    /* Set console escape scan code sequences */
    srvr->console_escape_len = get_keyboard_escape(srvr->console_escape, 
                srvr->console_escape_keycode, kbtype);
    VLOG_DEBUG_CAT(VLOG_KBM_KB, 
            "Channel %d. console escape 0x%x/0x%x/0x%x/0x%x. scan code:", 
            hal_get_channel_number(hp), e1, e2, e3, e4);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
            srvr->console_escape, srvr->console_escape_len);
}

void srvr_set_local_escape(HAL *hp,
                           SRVR *srvr, 
                           int e1, 
                           int e2, 
                           int e3, 
                           int e4,
                           int kbtype) {
    if (srvr == NULL)
        return;
    /* Set console escape keycodes */
    srvr->local_escape_keycode[0] = e1;
    srvr->local_escape_keycode[1] = e2;
    srvr->local_escape_keycode[2] = e3;
    srvr->local_escape_keycode[3] = e4;
    /* Set local escape scan code sequences  */
    srvr->local_escape_len = get_keyboard_escape(srvr->local_escape, 
                srvr->local_escape_keycode, kbtype);
    VLOG_DEBUG_CAT(VLOG_KBM_KB, 
            "Channel %d. local escape 0x%x/0x%x/0x%x/0x%x. scan code:", 
            hal_get_channel_number(hp), e1, e2, e3, e4);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB,
            srvr->local_escape, srvr->local_escape_len);
}

static int check_keyboard_escape(SRVR *srvr, unsigned char *buf, int len) {
    int rc = ESC_NONE, rc_console, rc_local;

    rc_console = memcmp(buf, srvr->console_escape,
        (len < srvr->console_escape_len) ? len : srvr->console_escape_len);
    rc_local = memcmp(buf, srvr->local_escape,
        (len < srvr->local_escape_len) ? len : srvr->local_escape_len);

    if (rc_console == 0 && len == srvr->console_escape_len) {
        rc = ESC_CONSOLE;
    }
    else if (rc_local == 0 && len == srvr->local_escape_len) {
        rc = ESC_LOCAL;
    }
    else if ((rc_console == 0 && len < srvr->console_escape_len) ||
        (rc_local == 0 && len < srvr->local_escape_len)) {
        rc = NEED_MORE_CHARS;
    }
    VLOG_DEBUG_CAT(VLOG_KBM_KB, "check_keyoard_escape returning %d", rc);
    return rc;
}

static int set_tx_kbmode(HAL *hp, int mode) {
    char buff[128];
    if (is_console_kbmode(mode)) 
        sprintf(buff, "set_kbconsole -kbconsole Yes");
    else
        sprintf(buff, "set_kbconsole -kbconsole No");
    return system_command(buff);
}

static int process_kbmode_change(HAL *hp,
                                 SRVR *srvr,
                                 int kbtype) {
    int keycode, isbrk, esc, newmode;

    /***********************************
     * Do nothing if not enough chars  *
     ***********************************/
    keycode = keycode_match(srvr->kb_buf, srvr->kb_len, kbtype, &isbrk);
    if (keycode == NEED_MORE_CHARS)
        return NEED_MORE_CHARS;

    /*************************************
     * Check if switching among KB modes *
     *************************************/
    if (hal_get_hardware_type(hp) == PLX_DEVICE) 
       esc =  check_keyboard_escape(srvr, srvr->kb_buf, srvr->kb_len);
    else 
        esc = -1;

    if (esc ==  NEED_MORE_CHARS)
       return NEED_MORE_CHARS;

    /**********************************************
     * Do nothing if not switching keyboard modes *
     **********************************************/
    newmode = kbmode_from_escape_code(esc);
    if (!is_escape_code(esc) || 
            (!hp->console_info.kb_active && (newmode == KBM_MODE_CONSOLE))) {
        srvr->last_keycode = (isbrk) ? 0 : keycode;
        return 0;
    }

    /********************************************************
     * Switching modes. Throw away everything in KB buffer. *
     ********************************************************/
    srvr->kb_len = srvr->kb_offset = srvr->last_keycode = 0;

    /**************************************************
     * If KBM is currently in local or remote modes,  *
     * send 'filler' break code if necessary.         *
     **************************************************/
    if (srvr->last_keycode && hp->console_info.kb_active) {
        unsigned char brkcode[12];
        struct tx_kbm_config kbm_cfg;
        int brklen;

        get_tx_kbm_config(hp, &kbm_cfg);
        brklen = get_break_code(srvr->last_keycode, kbm_cfg.keyboard_type, 
                brkcode);
        memcpy(srvr->kb_buf, brkcode, brklen);
        write_to_kb_fpga(hp, srvr->kb_buf, brklen);
    }

    /**********************
     * Switch to new mode *
     **********************/
    if ((hp->console_info.kb_active && newmode == KBM_MODE_CONSOLE) ||
        (!hp->console_info.kb_active && newmode != KBM_MODE_CONSOLE)) {
        return set_tx_kbmode(hp, newmode);
    }

    return 0;
}


int process_server_event_data(HAL *hp, 
                              SRVR *srvr,
                              struct event_s *eventp,
                              unsigned char *buf, 
                              int kbtype,
                              int mstype,
                              int *buflen)
{
    int len = 0, rc = 0, evlen, i, cno;
    //unsigned char evbuf[EVENT_BUFFER_SIZE + 1], buf[KB_BUFFER_SIZE + 1];
    unsigned char evbuf[EVENT_BUFFER_SIZE + 1];
    struct input_event *ev;

    /* Initialize data buffer lengths to be returned to caller */
    *buflen = 0;

    if (hp == NULL || get_supported_hardware(hp) == 0 || srvr == NULL) {
        /* V2DI50 - no USB event support */
        return 0;
    }

    cno = hal_get_channel_number(hp);

    /* XP200 - read and process event data */
    if ((evlen = read(eventp->event_fd, evbuf, EVENT_BUFFER_SIZE)) <= 0)
        return evlen;

    VLOG_DEBUG_CAT(VLOG_KBM_EV,
            "Channel %d. Eventfd %d. Read %d bytes Event data",
            cno, eventp->event_fd, evlen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_EV, evbuf, evlen);

    /* Skip forward to first keyboard or mouse event */
    for (i = 0; i < evlen; i += sizeof(struct input_event)) {
        ev = (struct input_event *) &evbuf[i];
        if (KB_EVENT(ev->type, ev->code) || MS_EVENT(ev->type, ev->code))
            break;
    }

    if (i >= evlen) return 0;

    evlen -= i;

    /* Convert events into keyboard or mouse data */
    if (KB_EVENT(ev->type, ev->code)) {
        len = fill_kb_buffer(buf, KB_BUFFER_SIZE, ev, evlen, kbtype, 1);
        if (len > 0) {
            VLOG_DEBUG_CAT(VLOG_KBM_KB,
                    "Channel %d. Read %d bytes USBKB data", cno, len);
            VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, buf, len);

            /* Update KBM mode */
            hal_update_virtual_console(hp);

            /* Copy data into buffer for processing */
            memcpy(&srvr->kb_buf[srvr->kb_offset], buf, len);
            srvr->kb_offset += len;
            srvr->kb_len += len;

            /* Check console switch escape sequences */
            //rc = check_console_switch(srvr->kb_buf, srvr->kb_len, kbtype);
            rc = process_kbmode_change(hp, srvr, kbtype);

            /* Wait for enough chars to determine escape sequence */
            if (rc == NEED_MORE_CHARS)
                return 0;

            if (hal_get_hardware_type(hp) != XPI_DEVICE) {
                /* Ignore all keyboard inputs if keyboard is console */
                if (hp->console_info.kb_active == 0) {
                    srvr->kb_len = srvr->kb_offset = 0;
                    return 0;
                }
            }

             if (hal_get_hardware_type(hp) != XPI_DEVICE) {
                 /* Write data to FPGA, i.e. loop back */
                 VLOG_DEBUG_CAT(VLOG_KBM_KB,
                    "Channel %d. Writing %d bytes KB data", cno, srvr->kb_len);
                  VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, srvr->kb_buf, srvr->kb_len);
                  write_to_kb_fpga(hp, srvr->kb_buf, srvr->kb_len); 
                   srvr->kb_len = srvr->kb_offset = 0;
              }
              else {
              
                 VLOG_INFO_CAT(VLOG_KBM_KB,
                    "Channel %d. Processed %d bytes KB data", cno, srvr->kb_len);
                  VLOG_INFO_HEXDATA_CAT(VLOG_KBM_KB, srvr->kb_buf, srvr->kb_len);
                  srvr->kb_len = srvr->kb_offset = 0;
              
              }
        }
        rc = KBM_TYPE_KEYBOARD;
    }

    if (hal_get_hardware_type(hp) != XPI_DEVICE) {

          /* Ignore all mouse inputs if keyboard is console */
         if (hp->console_info.kb_active == 0) { 
            return 0;
         }
    }

    if (MS_EVENT(ev->type, ev->code)) {
        int state;
        len = fill_ms_buffer(buf, KB_BUFFER_SIZE, ev, evlen, mstype,
                                            eventp->event_state, &state);
        if (eventp->event_state != state)
            eventp->event_state = state;

        if (len > 0) {
            VLOG_DEBUG_CAT(VLOG_KBM_MS,
                    "Channel %d. Read %d bytes USBMS data", cno, len);
            VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_MS, buf, len);
        }
        if (hal_get_hardware_type(hp) != XPI_DEVICE)
        { 
           /* Wite data to FPGA, i.e. loop back */
           VLOG_DEBUG_CAT(VLOG_KBM_MS,
                "Channel %d. Writing %d bytes MS data", cno, len);
           VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_MS, buf, len);
            write_to_ms_fpga(hp, buf, len, mstype);
        }

         rc = KBM_TYPE_MOUSE;
    }

    *buflen = len;

    return rc;
}


int srv_keyboard_data_to_rfb(SRVR *srvr,
                             unsigned char *kbbuf, 
                             unsigned char *rfbbuf,
                             int kblen) {
    int rfblen = 0;
    unsigned char numlock, capslock, shift;

    if (kbbuf == NULL || srvr == NULL)
        return 0;

    VLOG_DEBUG_CAT(VLOG_KBM_RFB, 
                   "Converting KB data (%d bytes) to RFB:", kblen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, kbbuf, kblen);

    rfblen = xlate_set2_to_rfb(kbbuf, rfbbuf,
                               &(srvr->numlock), &(srvr->capslock),
                               &(srvr->shift), kblen);

    VLOG_DEBUG_CAT(VLOG_KBM_RFB, "Converted RFB data (%d bytes):", rfblen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, rfblen);

    return rfblen;

}


int get_raw_keyboard_data(SRVR *srvr,
                           unsigned char *kbbuf, 
                           unsigned char *rfbbuf,
                           int kblen, 
                           unsigned int *scanCode, 
                           unsigned char *keydown) 
{
    
    int rfblen = 0;

    if (kbbuf == NULL || srvr == NULL)
        return rfblen;

    VLOG_DEBUG_CAT(VLOG_KBM_RFB,"Converting KB data (%d bytes) to Raw scan code :", kblen);
    //VLOG_INFO_CAT(VLOG_KBM_KB, "get_raw_keyboard_data : Printing kbbuf in hexa format.");
   //VLOG_INFO_HEXDATA_CAT(VLOG_KBM_KB, kbbuf, kblen);

    rfblen = get_set2_scancode(kbbuf,rfbbuf,&(srvr->numlock), &(srvr->capslock),&(srvr->shift), kblen,scanCode,keydown);

    //VLOG_DEBUG_CAT(VLOG_KBM_RFB, "Converted RFB data (%d bytes):", rfblen);
    //VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, rfblen);

    return rfblen;

}
