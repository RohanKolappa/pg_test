#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/input.h>

#include "hal.h"
#include "hal_serial.h"
#include "hal_hwinfo.h"
#include "commonutils.h"
#include "device_detection.h"
#include "usbutils.h"

#include "vlog.h"

/* Get the serial device for the board */
int hal_get_serial_fd(HAL * hp)
{

    if (hp->serial_info.serial_fd > 0) {
        return hp->serial_info.serial_fd;
    }

    /* On I50 Tx/Rx we open /dev/ttyS0 */
    if (get_hardware_category() == HARDWARE_CATEGORY_I50) {
        struct stat buf;
        if (stat("/tmp/.rs232", &buf) == 0) {
            strcpy(hp->serial_info.serial_name, "/dev/ttyS0");
            hp->serial_info.serial_fd = open("/dev/ttyS0", O_RDWR);
            if (hp->serial_info.serial_fd <= 0)
                VLOG_CRITICAL_CAT(VLOG_HAL,
                    "Opening /dev/ttyS0 failed for Serial communication.");
        }
        else {
            // If apply process failed to create .rs232 then we ignore
            // serial configuration since getty probably grabbed it and we
            // will hang on the open
            VLOG_WARNING_CAT(VLOG_HAL,
                "Ignoring Serial Configuration: Did not detect /tmp/.rs232");
        }
    }

    /* On XP200 open builtin USB serial, others open external USB serial */
    if (get_hardware_category() == HARDWARE_CATEGORY_XP) {
        int builtin, hwprofile, bno = hal_get_board_number(hp);
        char buf[MAXDEVLEN];

        hwprofile = get_hardware_profile();
        builtin = (hwprofile == HARDWARE_PROFILE_XP200 || 
                hwprofile == HARDWARE_PROFILE_XP200S) ? 1 : 0;
        if (usb_find_tty_device_name_for_board(bno, buf, MAXDEVLEN, 
                    builtin) < 0) { 
            if (builtin)
                VLOG_CRITICAL_CAT(VLOG_HAL,
                    "Channel %d. Error. No builtin Serial Device.", bno+1);
            else
                VLOG_CRITICAL_CAT(VLOG_HAL,
                    "Channel %d. Error. No external Serial Device.", bno+1);
        }
        else {
            hp->serial_info.serial_fd = open(buf, O_RDWR|O_NOCTTY|O_NDELAY);
            if (hp->serial_info.serial_fd <= 0)
                VLOG_CRITICAL_CAT(VLOG_HAL,
                    "Channel %d. Opening %s failed for Serial communication.",
                    bno+1, buf);
            else {
                strcpy(hp->serial_info.serial_name, buf);
                VLOG_CRITICAL_CAT(VLOG_HAL,
                    "Channel %d. Opened %s for Serial communication.",
                    bno+1, buf);
            }
        }
    }

    return hp->serial_info.serial_fd;
}

/* Open the serial device 'num' if it's on the board */
int hal_add_serial_device(HAL *hp, char *device) {
    int hardware_profile = get_hardware_profile();
    int cno = hal_get_board_number(hp) + 1, rc = 0;

    /* On I50 and XP200 we ignore adding or removing serial devices */
    if (hardware_profile == HARDWARE_PROFILE_V2D_TX ||
        hardware_profile == HARDWARE_PROFILE_V2D_RX ||
        hardware_profile == HARDWARE_PROFILE_XP200 ||
        hardware_profile == HARDWARE_PROFILE_XP200S) {
        return rc;
    }

    if (hp->serial_info.serial_fd > 0) {
        VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Not adding %s. Serial device already open.",
            cno, device);
    }
    else {
        hp->serial_info.serial_fd = open(device, O_RDWR|O_NOCTTY|O_NDELAY);
        if (hp->serial_info.serial_fd <= 0)
            VLOG_WARNING_CAT(VLOG_HAL,
                "Channel %d. Failed to add serial device %s.", cno, device) ;
        else {
            strcpy(hp->serial_info.serial_name, device);
            VLOG_INFO_CAT(VLOG_HAL,
                    "Channel %d. Added serial device %s.", cno, device);
        }
        rc = 1;
    }
    return rc;
}

/* Close the serial device 'num' if it's open on the board */
int hal_remove_serial_device(HAL *hp, char *device) {
    int hardware_profile = get_hardware_profile();
    int cno = hal_get_board_number(hp) + 1, rc = 0;

    /* On I50 and XP200 we ignore adding or removing serial devices */
    if (hardware_profile == HARDWARE_PROFILE_V2D_TX ||
        hardware_profile == HARDWARE_PROFILE_V2D_RX ||
        hardware_profile == HARDWARE_PROFILE_XP200 ||
        hardware_profile == HARDWARE_PROFILE_XP200S) {
        return 0;
    }

    if (hp->serial_info.serial_fd <= 0) {
        VLOG_INFO_CAT(VLOG_HAL,
            "Channel %d. Not removing %s. No serial device open.",
            cno, device);
    }
    else if (strcmp(hp->serial_info.serial_name, device) != 0) {
        VLOG_INFO_CAT(VLOG_HAL,
                "Channel %d. %s not being used for Serial Communication.",
            cno, device);
    }
    else {
        close(hp->serial_info.serial_fd);
        hp->serial_info.serial_fd = -1;
        memset(hp->serial_info.serial_name, '\0', MAXDEVLEN);
        rc = 1;
        VLOG_INFO_CAT(VLOG_HAL,
                "Channel %d. Removed serial device %s.", cno, device);
    }
    return rc;
}
