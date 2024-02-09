#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>
                                                                                
#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "vlib.h"
#ifdef __XPI__
#include "netvizxpi/netvizxpi.h"
#include "xpi_i2c_control.h"
#include "xpi_gpio_control.h"
#include "hal_v4l2.h"
#else
#include "fpga.h"
#include "xp_i2c_control.h"
#endif
#include "system_utils.h"
#include "hal_edid.h"

#define EDID_DATA_LEN   128

/********************
 * Read EDID EEPROM *
 ********************/
int hal_read_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize) 
{
    int i, rc = 0, hardware_type = hal_get_hardware_type(hp);

    if (hp == NULL) 
        return -1;
#ifndef __XPI__
    if (hardware_type == V2DI50_DEVICE) {
        for (i = 0; i < EDID_DATA_LEN; i += 2) {
            unsigned short val;
            val = hal_get_fpga_val_k(hp, I50_EDID_DATA_START + i * 2);
            buf[i] = (val & 0xff00) >> 8;
            buf[i + 1] = (val & 0x00ff);
        }
    }
    else if (hardware_type == PLX_DEVICE) {
        rc = xp_read_edid_eeprom(hp, buf, bufsize);
    }
#else
    if (hardware_type == XPI_DEVICE) {
        rc = xpi_read_edid_eeprom(hp, V4L2_SUBDEV_ADV7604, buf, bufsize);
    }
#endif
    return rc;
}

/*********************
 * Write EDID EEPROM *
 *********************/
int hal_load_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize) 
{
    int i, rc = -1, hardware_type = hal_get_hardware_type(hp);

    if (hp == NULL || bufsize > 256) 
        return -1;

#ifndef __XPI__
    if (hardware_type == V2DI50_DEVICE) {
        for (i = 0; i < EDID_DATA_LEN; i += 2) {
            hal_set_fpga_val_k(hp, I50_EDID_DATA_START + i * 2, 
                                        (buf[i] << 8) + buf[i + 1]);
        }
    }
    else if (hardware_type == PLX_DEVICE) {
        rc = xp_load_edid_eeprom(hp, buf, bufsize);
    }
#else
    if (hardware_type == XPI_DEVICE) {
        unsigned char xpibuf[256];
        int fd;

        if ((fd = hal_get_video_fd(hp)) < 0) 
            return -1;

        /* disable video input to PC */
        xpi_disable_video_input_to_pc();
        
        /* write content of buffer to ADV7604 edid */
        memset(xpibuf, 0, sizeof(xpibuf));
        memcpy(xpibuf, buf, bufsize);
        rc = xpi_load_edid_eeprom(hp, V4L2_SUBDEV_ADV7604, xpibuf, 256);
        if (rc == 0) {
            /* set ADV7604 to digital or analog based on edid */
            if (xpibuf[20] & 0x80)  /* byte 0x20 bit 7 = 1 -> digital */
                rc = hal_v4l2_s_input(hp, NETVIZXPI_MODE_HDMI);
            else                    /* byte 0x20 bit 7 = 0 -> analog */
                rc = hal_v4l2_s_input(hp, NETVIZXPI_MODE_GR);
        }

        sleep(2);

        /* enable video input to PC */
        xpi_enable_video_input_to_pc();
        
    }
#endif
    return rc;
}

int hal_check_mon_edid(HAL *hp)
{
    unsigned char buf[EDID_DATA_LEN];
    if (hal_read_mon_edid(hp, buf, EDID_DATA_LEN) == 0)
        return 1;
    return 0;
}

int hal_read_mon_edid(HAL *hp, unsigned char *buf, int bufsize)
{
    int rc = -1, hardware_type = hal_get_hardware_type(hp);

#ifndef __XPI__
    if (hardware_type == PLX_DEVICE) {
        rc = xp_read_mon_edid(hp, buf, bufsize);
    }
#else
    rc = xpi_read_mon_edid(hp, buf, bufsize);
#endif
    return rc;
}

/************************************************
 * Read extended EDID block(s) from the monitor *
 ************************************************/
int hal_read_mon_edid_extension(HAL *hp, 
                                int block,
                                unsigned char *buf, 
                                int bufsize) {
    int rc = -1, hardware_type = hal_get_hardware_type(hp);

#ifndef __XPI__
    if (hardware_type == PLX_DEVICE) {
        rc = xp_read_mon_edid_extension(hp, block, buf, bufsize);
    }
#endif
    return rc;
}
