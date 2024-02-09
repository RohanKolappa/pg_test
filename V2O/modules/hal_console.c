#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/kd.h>

#include "hal.h"
#include "hal_console.h"
#include "hal_hwinfo.h"
#include "device_detection.h"
#include "commonutils.h"
#include "system_utils.h"
#include "kbms_utils.h"
#include "kbconsole_config.h"
#include "rx_kbm_config.h"

#include "vlog.h"

/* Routines for virtual console switching */
#define VC_BASE         12
#define VT100_CLRSCRN   "\033[2J"
#define VT100_HOME      "\033[H"
#define KBD_MESSAGE     "Switching Keyboard/Mouse to User Mode... "
#define DEFAULT_TYPEMATIC_DELAY     500
#define DEFAULT_TYPEMATIC_PERIOD    91

static int create_virtual_console_fd(HAL *hp, char *tty) {
    int kbfd;
    int cno = hal_get_channel_number(hp);

    /********************************************
     * Get Keyboard file discriptor for console *
     *********************************************/
    if ((kbfd = open(tty, O_RDWR)) == -1) {
        VLOG_WARNING_CAT(VLOG_HAL,
            "Channel %d. Unable to open Keyboard TTY", cno);
        return -1;
    }
    VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Opened %s for keyboard input. FD = %d", 
            cno, tty, kbfd);

    return kbfd;
}

static void restore_virtual_console(HAL *hp) {
    int cno = hal_get_channel_number(hp);

    if (hp == NULL || hp->console_info.console_fd < 0)
        return;

    VLOG_INFO_CAT(VLOG_HAL,
        "Channel %d. Restoring keyboard state", cno);
    close(hp->console_info.console_fd);
    hp->console_info.console_fd = -1;
}

int hal_get_console_fd(HAL * hp)
{
    char buf[MAXDEVLEN];
    int ttyno = hal_get_channel_number(hp) + VC_BASE;

    if (hp == NULL )
        return -1;

    if (hal_get_hardware_type(hp) == PLX_DEVICE || 
        hal_get_hardware_type(hp) == XPI_DEVICE)
    {
      if (hp->console_info.console_fd > 0)
         return hp->console_info.console_fd;

      sprintf(buf, "/dev/tty%d", ttyno);
      hp->console_info.console_fd = create_virtual_console_fd(hp, buf);

      return hp->console_info.console_fd;
    }
    else 
    {
       return -1;
    }
}

void hal_close_console(HAL * hp) {
    if (hp->console_info.console_fd)
        close(hp->console_info.console_fd);
}

/* Remote & local switching escape sequence switch terminal to user mode */
int hal_kbm_switchin(HAL *hp) {
    int cno;
    struct in_addr addr, mask;

    if (hal_get_console_fd(hp) <= 0) {
        VLOG_INFO_CAT(VLOG_HAL, "Error switching KBM to User. Bad console FD.");
        return -1;
    }
    cno = hal_get_channel_number(hp);

    VLOG_INFO_CAT(VLOG_HAL, "Channel %d. KBM Switching To User Mode.", cno);

    /* Don't switch if network interface not configured since */
    /* we won't be able to make network connections.          */
    if (get_if_info("eth0", &addr, &mask) < 0) {
        VLOG_INFO_CAT(VLOG_HAL, 
            "Channel %d. KBM not switched. Network interface not configured.", 
            cno);
        return -1;
    }

    /* Display message on virtual terminal */
    write(hp->console_info.console_fd, VT100_CLRSCRN, sizeof VT100_CLRSCRN);
    write(hp->console_info.console_fd, VT100_HOME, sizeof VT100_HOME);
    write(hp->console_info.console_fd, KBD_MESSAGE, sizeof KBD_MESSAGE);

    /* Change virtual terminal */
    if (ioctl(hp->console_info.console_fd, VT_ACTIVATE, cno+VC_BASE) < 0) {
        VLOG_DEBUG_CAT(VLOG_HAL,
                "Channel %d VT_ACTIVATE failed terminal %d", cno, cno+1);
    }
    if (ioctl(hp->console_info.console_fd, VT_WAITACTIVE, cno+VC_BASE) < 0) { 
        VLOG_DEBUG_CAT(VLOG_HAL,
                "Channel %d VT_WAITACTIVE failed terminal %d", 
                cno, cno+1); 
    }
    hp->console_info.kb_active = 1;

    return 0;
}

/* Console switching escape sequence switches terminal out of user mode */
int hal_kbm_switchout(HAL *hp) {
    int cno;

    if (hal_get_console_fd(hp) <= 0) {
        VLOG_INFO_CAT(VLOG_HAL, 
                "Error switching KBM to Console. Bad console FD.");
        return -1;
    }
    cno = hal_get_channel_number(hp);

    VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. KBM Switching Out Of User Mode.", cno);

    /* Change virtual terminal */
    if (ioctl(hp->console_info.console_fd, VT_ACTIVATE, 1) < 0) {
        VLOG_DEBUG_CAT(VLOG_HAL,
                "Channel %d VT_ACTIVATE failed terminal 1", cno);
    }
    if (ioctl(hp->console_info.console_fd, VT_WAITACTIVE, 1) < 0) { 
        VLOG_DEBUG_CAT(VLOG_HAL,
                "Channel %d VT_WAITACTIVE failed terminal 1", cno); 
    }
    hp->console_info.kb_active = 0;

    return 0;
}

int hal_update_virtual_console(HAL *hp) {
    int i, kb_active = 0;
    struct vt_stat stat;
    
    /* As there is no console mode in xpi we are ignoring the console mode forcebly 
       when box comes up either it will be in remote or loacl mode */
    if (get_hardware_category() == HARDWARE_CATEGORY_XPI)
    {
       hp->console_info.kb_active = 1;
       return 0;
    }
 
    if (get_hardware_category() != HARDWARE_CATEGORY_XP)
        return 0;

    if (hal_get_console_fd(hp) < 0) {
        VLOG_INFO_CAT(VLOG_HAL, 
                "Error update virtual console. Bad console FD.");
        return -1;
    }

    if (ioctl(hp->console_info.console_fd, VT_GETSTATE, &stat) < 0) {
        VLOG_DEBUG_CAT(VLOG_HAL,
                "Channel %d VT_WAITACTIVE failed terminal 1", 
                hal_get_channel_number(hp)); 
        return -1;
    }

    for (i = 0; i < MAX_FPGA_BOARDS; i++) {
        if (stat.v_active == (VC_BASE + i + 1))
            kb_active = 1;
    }

    if (hp->console_info.kb_active != kb_active) {
        VLOG_DEBUG_CAT(VLOG_HAL, 
            "Channel %d. Updating KBM mode to %s.",
            hal_get_channel_number(hp), (kb_active) ? "User" : "Console");
        hp->console_info.kb_active = kb_active;
    }

    return 0;
}

int hal_get_current_kbmode(HAL *hp) {
    int rc =  KBM_MODE_CONSOLE;
    if (hp->console_info.kb_active) {
        if (hal_get_board_type(hp) == BOARDTYPE_RX) {
            struct rx_kbm_config kbm_cfg;
            get_rx_kbm_config(hp, &kbm_cfg);
            rc = kbm_cfg.kbm_mode;
        }
        else 
            rc = KBM_MODE_LOCAL;
    }
    return rc;
}

void hal_initialize_console(HAL *hp) {
    int cno = hal_get_channel_number(hp);
    struct termios term_ios;

    /*********************************
     * Set terminal to uncooked mode *
     *********************************/
    VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Setting terminal to uncooked mode.", cno);
    term_ios.c_lflag &= ~ (ICANON | ECHO | ISIG);
    term_ios.c_iflag = 0;
    term_ios.c_cc[VMIN] = 1;

    if (tcsetattr(hp->console_info.console_fd, TCSAFLUSH, &term_ios) == -1) {
        VLOG_WARNING_CAT(VLOG_HAL,
                "Channel %d. Error: Keyboard: tcsetattr: %m", cno);
        restore_virtual_console(hp);
        return;
    }

    /****************************
     * Set keyboard to raw mode *
     ****************************/
    VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Setting keyboard to raw mode.", cno);
    if (ioctl(hp->console_info.console_fd, KDSKBMODE, K_MEDIUMRAW)) {
        VLOG_WARNING_CAT(VLOG_HAL,
                "Channel %d. Error. Keyboard: KDSKBMODE: %m", cno);
        restore_virtual_console(hp);
        return;
    }

    /***********************
     * Set virtual console *
     ***********************/
    if (is_keyboard_in_console_mode()) {
        VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Switching keyboard to console mode.", cno);
        (void) hal_kbm_switchout(hp); 
    }
    else {
        VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Switching keyboard to user mode.", cno);
        (void) hal_kbm_switchin(hp); 
    }
}

