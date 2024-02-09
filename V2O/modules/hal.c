
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <linux/videodev2.h>
#include <linux/kd.h>
#include <errno.h>
#include <syslog.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "vutils.h"
#include "kbm_control.h"
#include "configureSystem.h"
#include "lut_utils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "led_control.h"
#include "usbutils.h"
#include "kbms_utils.h"
#include "kbconsole_config.h"
#include "rx_kbm_config.h"
#include "strmsrv_comm.h"
#include "standalone.h"
#include "system_utils.h"
#include "override_config.h"
#include "override_utils.h"
#include "display_config.h"
#ifdef __XPI__
#include "netvizxpi/netvizxpi.h"
#include "netvizxpi/lut_table.h"
#include "OmniTekIoctl_linux.h"
#include "hal_v4l2.h"
#include "xpi_pll_control.h"
#include "xpi_i2c_control.h"
#include "rx_splash_config.h"
#else
#include "xp_i2c_control.h"
#include "lut_table.h"		    /* for XPI_CLOCK_FREQ */
#endif
#include "ad9852dds.h"
#include "ad9888vif.h"
#include "ad9887vif.h"
#include "vlib.h"
#include "slice_size.h"
#include "lut_utils.h"

#include "vlog.h"

struct regval_st {
    int reg;
    int val;
};
/* Create a handle to a HAL object based on the given board number */
HAL *hal_acquire(int board_number)
{
    HAL *hp;
    char buf[MAXDEVLEN];
    int hardware_category = get_hardware_category();

    if (hardware_category == HARDWARE_CATEGORY_XPI) {
        strcpy(buf, "/dev/video0");
    }
    else {
        /* Board number cannot be less than zero */
        if (board_number < 0)
            return NULL;
        /* Try to open the device for the board */
        sprintf(buf, "/dev/fpga%d_video", board_number);
    }

    int fd = open(buf, O_RDWR);
    if (fd == -1) {
        VLOG_WARNING_CAT(VLOG_HAL, 
                "Could not open device %s. errno %d. %m.", buf, errno);
        return NULL;
    }

    /* Ok we were able to open the device.  Allocate and initilize the
       HAL structure */
    hp = (HAL *) calloc(1, sizeof(HAL));
    if (hp == NULL) {
        VLOG_CRITICAL_CAT(VLOG_HAL, "Memory allocation failed  %s", buf);
        close(fd);
        return NULL;
    }

    /* Set the kernel fd as the just opened fd */
    hp->kernel_fd = fd;

    /* All other fds are lazily opened as needed */
    hp->board_number = board_number;
    hp->audio_fd = -1;
    hp->keyboard_fd = -1;
    hp->mouse_fd = -1;
    hp->i2c_fd = -1;
    hp->console_info.console_fd = -1;
    memset(hp->serial_info.serial_name, '\0', MAXDEVLEN);
    hp->serial_info.serial_fd = -1;
    INIT_LIST_HEAD(&(hp->event_head));

    /* initialize hw_info */
    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) == 0) 
        return hp;
    else if (hp != NULL) {
        if (hp->kernel_fd > 0)
            close(hp->kernel_fd);
        free(hp);
    }
    return NULL;
}

/* Destroy the HAL object */
void hal_release(HAL * hp)
{
    /* We should make this an assert eventually */
    if (hp == NULL) {
        VLOG_WARNING_CAT(VLOG_HAL,
                "Null pointer passed to hal_release.");
        return;
    }

    if (hp->hw_info.hardware_type == XPI_DEVICE) {
        VLOG_TRACE_CAT(VLOG_HAL, "Releasing HAL for XPi");
        if (hp->kernel_fd > 0) {
            close(hp->kernel_fd);
        }
    }
    else {
        VLOG_TRACE_CAT(VLOG_HAL,
            "Releasing HAL for board %d", hp->board_number);

        /* video_fd will always be open */
        close(hp->kernel_fd);

        /* Close all other fds if ever opened */
        if (hp->audio_fd > 0) {
            VLOG_TRACE_CAT(VLOG_HAL,
                "Releasing Audio FD for board %d", hp->board_number);
            close(hp->audio_fd);
        }
        if (hp->keyboard_fd > 0) {
            VLOG_TRACE_CAT(VLOG_HAL,
                "Releasing Keyboard FD for board %d", hp->board_number);
            close(hp->keyboard_fd);
        }
        if (hp->mouse_fd > 0) {
            VLOG_TRACE_CAT(VLOG_HAL,
                "Releasing Mouse FD for board %d", hp->board_number);
            close(hp->mouse_fd);
        }
        if (hp->i2c_fd > 0) {
            VLOG_TRACE_CAT(VLOG_HAL,
                "Releasing I2C FD for board %d", hp->board_number);
            close(hp->i2c_fd);
        }
        if (hp->console_info.console_fd > 0) {
            VLOG_TRACE_CAT(VLOG_HAL,
                "Releasing Console FD for board %d", hp->board_number);
            close(hp->console_info.console_fd);
        }
        if (hp->serial_info.serial_fd > 0) {
            VLOG_TRACE_CAT(VLOG_HAL,
                "Releasing Serial FD for board %d", hp->board_number);
            close(hp->serial_info.serial_fd);
        }
        usb_release_event_devices(hp);
    }

    /* Free the hal pointer */
    free(hp);
}

/* Get the video device for the board. It is the same as the already
 * opened kernel_fd */
int hal_get_video_fd(HAL * hp)
{
    if (hp == NULL) {
        return -1;
    }
    return hp->kernel_fd;
}

/* Get the audio device for the board */
int hal_get_audio_fd(HAL * hp)
{
    char buf[MAXDEVLEN];

    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE) {
        return -1;
    }
    else {
        if (hp->audio_fd > 0)
            return hp->audio_fd;
        sprintf(buf, "/dev/fpga%d_audio", hp->board_number);
        hp->audio_fd = open(buf, O_RDWR);
        if (hp->audio_fd <= 0) {
            VLOG_CRITICAL_CAT(VLOG_HAL,
                "Channel %d. Could not open audio device %s", 
                hp->board_number+1, buf);
        }
        return hp->audio_fd;
    }
}

/* Get the keyboard device for the board */
int hal_get_keyboard_fd(HAL * hp)
{
    char buf[MAXDEVLEN];

    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE) {
        return -1;
    }
    else {
        if (hp->keyboard_fd > 0)
            return hp->keyboard_fd;
        sprintf(buf, "/dev/fpga%d_keyboard", hp->board_number);
        hp->keyboard_fd = open(buf, O_RDWR);
        if (hp->keyboard_fd <= 0) {
            VLOG_CRITICAL_CAT(VLOG_HAL,
                "Channel %d. Could not open keyborad device %s", 
                hp->board_number+1, buf);
        }
        return hp->keyboard_fd;
    }
}

/* Get the mouse device for the board */
int hal_get_mouse_fd(HAL * hp)
{
    char buf[MAXDEVLEN];

    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE) {
        return -1;
    }
    else {
        if (hp->mouse_fd > 0)
            return hp->mouse_fd;
        sprintf(buf, "/dev/fpga%d_mouse", hp->board_number);
        hp->mouse_fd = open(buf, O_RDWR);
        if (hp->mouse_fd <= 0) {
            VLOG_CRITICAL_CAT(VLOG_HAL,
                "Channel %d. Could not open mouse device %s", 
                hp->board_number+1,buf);
        }
        return hp->mouse_fd;
    }
}

/* Routines to access XPi registers from the OmniTek driver */
unsigned long hal_get_omnitek_val_k(HAL *hp, unsigned long reg)
{
#ifdef __XPI__
    struct omnitek_io omniio;
    omniio.type = OMNITEKIO_TYPE_GET;
    omniio.barno = FPGA_BARNO;
    omniio.reg = reg;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_OMNITEK, &omniio);
    return omniio.value;
#else
    return 0;
#endif
}

void hal_set_omnitek_val_k(HAL *hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct omnitek_io omniio;
    omniio.type = OMNITEKIO_TYPE_SET;
    omniio.barno = FPGA_BARNO;
    omniio.reg = reg;
    omniio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_OMNITEK, &omniio);
#endif
}

/* Routines to access FPGA registers */
void hal_set_fpga_val_k(HAL * hp, unsigned short reg, unsigned short val)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_SET;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
}

unsigned short hal_get_fpga_val_k(HAL * hp, unsigned short reg)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_GET;
    regio.reg = reg;
    regio.value = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return (unsigned short) regio.value;
}

void hal_and_in_fpga_val_k(HAL * hp, unsigned short reg, unsigned long val)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_ANDIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
}

void hal_or_in_fpga_val_k(HAL * hp, unsigned short reg, unsigned long val)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_ORIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
}

void hal_set_fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_SET;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
}
    
unsigned long hal_get_fpga_val32_k(HAL * hp, unsigned long reg)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_GET;
    regio.reg = reg;
    regio.value = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return regio.value;
}

void hal_and_in_fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_ANDIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
}

void hal_or_in_fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
    struct reg_io regio;
    regio.type = REGIO_TYPE_FPGA_ORIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
}

int hal_get_mdio_val_k(HAL *hp, 
                      int off,
                      int page,
                      int regad, 
                      unsigned long *val) 
{
#ifdef __XPI__
    struct pagedmdio_io pmio;
    unsigned long result;

    if (off == MAIN_OFFSET)
        pmio.type = PAGED_MAINMDIO_TYPE_GET;
    else if (off == EXPANSION_OFFSET)
        pmio.type = PAGED_EXPANSIONMDIO_TYPE_GET;
    else 
        return 0;
    pmio.page = page;
    pmio.reg = (regad & 0x1f) | off;
    pmio.value = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_PAGEDMDIO, &pmio) < 0)
        return 0;
    *val = pmio.value;
    return (int) pmio.value;
#endif
    return 0;
}

void hal_set_mdio_val_k(HAL *hp, 
                        int off,
                        int page, 
                        int regad, 
                        unsigned long val) 
{
#ifdef __XPI__
    struct pagedmdio_io pmio;

    if (off == MAIN_OFFSET)
        pmio.type = PAGED_MAINMDIO_TYPE_SET;
    else if (off == EXPANSION_OFFSET)
        pmio.type = PAGED_EXPANSIONMDIO_TYPE_SET;
    else 
        return;
    pmio.page = page;
    pmio.reg = regad;
    pmio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_PAGEDMDIO, &pmio);
#endif
}

int hal_get_mac_val_k(HAL *hp, 
                      int off, 
                      int dummy2, 
                      int regad, 
                      unsigned long *val) 
{
#ifdef __XPI__
    struct reg_io regio;

    regio.type = REGIO_TYPE_MAC_GET;
    regio.reg = regad + off;
    regio.value = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return regio.value;
#endif
    return 0;
}

void hal_set_mac_val_k(HAL *hp, 
                       int off, 
                       int dummy2, 
                       int regad, 
                       unsigned long val) 
{
#ifdef __XPI__
    struct reg_io regio;

    regio.type = REGIO_TYPE_MAC_SET;
    regio.reg = regad + off;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

unsigned long hal_get_s6fpga_val32_k(HAL *hp, unsigned long reg)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S6FPGA_GET;
    regio.reg = reg;
    regio.value = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return regio.value;
#endif
    return (unsigned long) 0;
}
    
void hal_set_s6fpga_val32_k(HAL *hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S6FPGA_SET;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

void hal_and_in_s6fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S6FPGA_ANDIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

void hal_or_in_s6fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S6FPGA_ORIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

unsigned long hal_get_s3fpga_val32_k(HAL *hp, unsigned long reg)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S3FPGA_GET;
    regio.reg = reg;
    regio.value = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return regio.value;
#endif
    return (unsigned long) 0;
}
    
void hal_set_s3fpga_val32_k(HAL *hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S3FPGA_SET;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

void hal_and_in_s3fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S3FPGA_ANDIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

void hal_or_in_s3fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
#ifdef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_S3FPGA_ORIN;
    regio.reg = reg;
    regio.value = val;
    ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
#endif
}

int hal_get_s6fpga_version(HAL * hp, char *buf, int maxlen)
{
    unsigned short fpga_reg = 0;

    if (hp == NULL)
        return -1;
#ifdef __XPI__
    fpga_reg = (unsigned short) hal_get_s6fpga_val32_k(hp, S6FPGA_IDREG);
    snprintf(buf, maxlen, "%d.%d", (fpga_reg & 0xFF00) >> 8, fpga_reg & 0x00FF);
#endif
    return fpga_reg;
}

unsigned long hal_get_s6fpga_date(HAL *hp, char *buf, int maxlen)
{
    int rc = 0, pause = 1000;
    unsigned long ts_u, ts_l, seconds = 0;
    char tbuf[40];

    if (hp == NULL)
        return 0;

#ifdef __XPI__
    ts_u = hal_get_s6fpga_val32_k(hp, S6FPGA_TIMESTAMP_U);
    ts_l = hal_get_s6fpga_val32_k(hp, S6FPGA_TIMESTAMP_L);
    seconds = (ts_u << 16) | ts_l;

    snprintf(tbuf, sizeof(tbuf), "%s", ctime((time_t *) & seconds));

    /* rearrange string to make it look like software date */
    strncpy(buf, tbuf + 4, 7);
    strncpy(buf + 7, tbuf + 20, 4);
    strncpy(buf + 11, tbuf + 10, 9);
    *(buf + 20) = '\0';
#endif
    return seconds; 
}

int hal_get_s3fpga_version(HAL * hp, char *buf, int maxlen)
{
    unsigned short fpga_reg = 0;

    if (hp == NULL)
        return -1;

#ifdef __XPI__
    fpga_reg = (unsigned short) hal_get_s3fpga_val32_k(hp, S3FPGA_IDREG);
    snprintf(buf, maxlen, "%d.%d", (fpga_reg & 0xFF00) >> 8, fpga_reg & 0x00FF);
#endif
    return fpga_reg;
}

unsigned long hal_get_s3fpga_date(HAL *hp, char *buf, int maxlen)
{
    int rc = 0;
    unsigned long ts_u, ts_l, seconds = 0;
    char tbuf[40];

    if (hp == NULL)
        return 0;

#ifdef __XPI__
    ts_u = hal_get_s3fpga_val32_k(hp, S3FPGA_TIMESTAMP_U);
    ts_l = hal_get_s3fpga_val32_k(hp, S3FPGA_TIMESTAMP_L);
    seconds = (ts_u << 16) | ts_l;

    snprintf(tbuf, sizeof(tbuf), "%s", ctime((time_t *) & seconds));

    /* rearrange string to make it look like software date */
    strncpy(buf, tbuf + 4, 7);
    strncpy(buf + 7, tbuf + 20, 4);
    strncpy(buf + 11, tbuf + 10, 9);
    *(buf + 20) = '\0';
#endif
    return seconds; 
}


/* Routines to access I50 AD9888 ADC registers */
void hal_set_ad9888_val_k(HAL * hp, int reg, unsigned char value)
{
#ifndef __XPI__
    int hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == PLX_DEVICE) {
        hal_set_v4_ad9888_val_k(hp, (unsigned short) reg, value);
    }
    else if (hardware_type == V2DI50_DEVICE) {
        struct reg_io regio;
        regio.type = REGIO_TYPE_AD9888_SET;
        regio.reg = reg;
        regio.value = value;
        ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
    }
#endif
}

unsigned char hal_get_ad9888_val_k(HAL * hp, int reg)
{
#ifndef __XPI__
    int hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == PLX_DEVICE) {
        return hal_get_v4_ad9888_val_k(hp, (unsigned short) reg);
    }
    else if (hardware_type == V2DI50_DEVICE) {
        struct reg_io regio;
        regio.type = REGIO_TYPE_AD9888_GET;
        regio.reg = reg;
        regio.value = 0;
        if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
            return 0;
        return (unsigned char) regio.value;
    }
#endif
    return 0; 
}

/* Routines to access I50 AD9852 DDS registers */
void hal_set_ad9852_val_k(HAL * hp, unsigned short reg, unsigned char val)
{
#ifndef __XPI__
    struct reg_io regio;

    regio.type = REGIO_TYPE_AD9852_SET;
    regio.reg = reg;
    regio.value = val;
    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            hal_set_driver_enable(hp, 0);
            hal_set_bar2_val_k(hp, DDS_BASE | reg, val);
            hal_set_driver_enable(hp, 1);
            break;
        case V2DI50_DEVICE:
            ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio);
            break;
        default:
            break;
    }
#endif
}

unsigned char hal_get_ad9852_val_k(HAL * hp, unsigned short reg)
{
    unsigned char c=0;
#ifndef __XPI__
    struct reg_io regio;
    regio.type = REGIO_TYPE_AD9852_GET;
    regio.reg = reg;
    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            hal_set_driver_enable(hp, 0);
            c = (unsigned char ) hal_get_bar2_val_k(hp, DDS_BASE | reg);
            hal_set_driver_enable(hp, 1);
            break;
        case V2DI50_DEVICE:
            if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0) {
                c = 0;
            }
            else {
               c = (unsigned char) regio.value;
            }
            break;
        default:
            break;
   }
#endif
    return c;
}

/* Routines to access I50 ID18125 FIFO registers */
int hal_id18125_write_k(HAL * hp, int empty_offset, int full_offset)
{
#ifndef __XPI__
    struct reg_io regio;

    if (hal_get_hardware_type(hp) != V2DI50_DEVICE)
        return -1;
    regio.type = REGIO_TYPE_ID18125_SET;
    regio.reg = (unsigned long) empty_offset;
    regio.value = (unsigned long) full_offset;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return -1;
#endif
    return 0;
}

int hal_id18125_read_k(HAL * hp, int *empty_offset, int *full_offset)
{

#ifndef __XPI__
    struct reg_io regio;

    if (hal_get_hardware_type(hp) != V2DI50_DEVICE)
        return -1;
    memset(&regio, 0x00, sizeof(regio));
    regio.type = REGIO_TYPE_ID18125_GET;
    if (ioctl(hp->kernel_fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return -1;
    *empty_offset = (int) regio.reg;
    *full_offset = (int) regio.value;
#endif
    return 0;
}

int hal_getAudioVideoFlag(HAL *hp) 
{
    int flag = 0, hardware_type;

    hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == V2DI50_DEVICE)
        return 0;
    if (hardware_type == XPI_DEVICE) {
#ifdef __XPI__
        int intf = hal_getActiveIntf(hp);
        if (intf & INTERFACE_ADDSIDE_LATENCY)
            flag |= AVFLAG_LATENCY;
        if (intf & INTERFACE_COMPMODE_MOTION444)
            flag |= AVFLAG_MOTION444;
        if (intf & INTERFACE_GENLOCKMASTER)
            flag |= AVFLAG_GENLOCKMASTER;
        else
            flag |= AVFLAG_GENLOCKSLAVE;
#endif
    }
    else {
        ioctl(hp->kernel_fd, FPGA_IOC_GET_AVFLAG, &flag);
    }
    return flag;
}

void hal_setAudioVideoFlag(HAL *hp, int flag) 
{
    int hardware_type;

    hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == V2DI50_DEVICE) 
        return;

    if (hardware_type == XPI_DEVICE) {
#ifdef __XPI__
        int intf = hal_getActiveIntf(hp);
        intf &= ~(INTERFACE_ADDSIDE_LATENCY | 
                  INTERFACE_COMPMODE_MOTION444 | 
                  INTERFACE_GENLOCKMASTER);
        if (flag & AVFLAG_LATENCY)
            intf |= INTERFACE_ADDSIDE_LATENCY;
        if (flag & AVFLAG_MOTION444)
            intf |= INTERFACE_COMPMODE_MOTION444;
        if (flag & AVFLAG_GENLOCKMASTER)
            intf |= INTERFACE_GENLOCKMASTER;
        else
            intf &= ~AVFLAG_GENLOCKMASTER;
        if (flag & AVFLAG_GENLOCKSLAVE)
            intf &= ~INTERFACE_GENLOCKMASTER;
        else
            intf |= AVFLAG_GENLOCKMASTER;
        hal_setActiveIntf(hp, intf);
#endif
    }
    else { 
        ioctl(hp->kernel_fd, FPGA_IOC_SET_AVFLAG, &flag);
    }
}

int hal_getAudioMixerLocalVolume(HAL *hp) 
{
    int hardware_type = hal_get_hardware_type(hp);

    if (hardware_type == PLX_DEVICE)
        return hal_get_fpga_val_k(hp, FPGA_AUDIOALPHA2C);
    else if (hardware_type == XPI_DEVICE)
        return hal_get_fpga_val_k(hp, FPGA_AUDIOALPHA1);
    else
        return 0;
}

int hal_getAudioMixerRemoteVolume(HAL *hp) 
{
    int hardware_type = hal_get_hardware_type(hp);

    if (hardware_type == PLX_DEVICE)
        return hal_get_fpga_val_k(hp, FPGA_AUDIOALPHA2);
    else if (hardware_type == XPI_DEVICE)
        return hal_get_fpga_val_k(hp, FPGA_AUDIOALPHA1C);
    else
        return 0;
}

void hal_setAudioMixer(HAL *hp, int nLocalVolume, int nRemoteVolume, int AudioInMic) 
{
    int hardware_type = hal_get_hardware_type(hp);
    int audio_in[2],  count, rc;
    struct regval_st buff[] = {
            {0x00, 0x01},
            {0x3b, 0x0},
            {0x3c, 0x0},
            {0x34, 0x0},
            {0x37, 0x0}
        };

    if (hardware_type == PLX_DEVICE) {
        hal_set_fpga_val_k(hp, FPGA_AUDIOALPHA2, nRemoteVolume);
        hal_set_fpga_val_k(hp, FPGA_AUDIOALPHA2C, nLocalVolume);
    }
    else if (hardware_type == XPI_DEVICE) { 
#ifdef __XPI__
        hal_set_fpga_val_k(hp, FPGA_AUDIOALPHA1C, nRemoteVolume);
        hal_set_fpga_val_k(hp, FPGA_AUDIOALPHA1, nLocalVolume);

        if(AudioInMic) {
            audio_in[0] = 0x36;
            audio_in[1] = 0x08;
        }
        else {
            audio_in[0] = 0x09;
            audio_in[1] = 0x80;
        }

        buff[1].val  = audio_in[0] ;
        buff[2].val  = audio_in[0] ;
        buff[3].val  = audio_in[1] ;
        buff[4].val  = audio_in[1] ;


        for (count = 0; count < sizeof(buff)/sizeof(struct regval_st); count++) {
            rc = xpi_set_expansion_i2c_val(hp, TLV320AIC3254_I2C_PORT,
                    TLV320AIC3254_I2C_ADDR, buff[count].reg, buff[count].val);
            if (rc < 0) {
                printf("error %d writing I2C addr 0x%x reg 0x%x\n", 
                        rc, buff[count].reg, buff[count].val);
            }
        }
#endif
    }
}

void hal_resetAddsideAudio(HAL *hp)
{
    if (hal_get_hardware_type(hp) != V2DI50_DEVICE) {
        hal_and_in_fpga_val_k(hp, FPGA_ACREG, ~ACREG_AUD_ENABLE);
        usleep(1);
        hal_or_in_fpga_val_k(hp, FPGA_ACREG, ACREG_AUD_ENABLE);
    }
}

void hal_resetDropsideAudio(HAL *hp)
{
    if (hal_get_hardware_type(hp) != V2DI50_DEVICE) { 
        hal_and_in_fpga_val_k(hp, FPGA_DCREG, ~DCREG_AUD_ENABLE);
        usleep(1);
        hal_or_in_fpga_val_k(hp, FPGA_DCREG, DCREG_AUD_ENABLE);
    }
}

int hal_setVideoCheck(HAL *hp, int mode)
{
    int fd = hal_get_video_fd(hp);
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return 0;
    if (ioctl(fd, FPGA_IOC_SET_VIDEOCHECK, &mode) < 0)
        return -1;
    return 0;
}

#ifndef __XPI__
struct stdi_io {
    int dummy;
};
#endif

static int xpi_get_stdi(HAL *hp, struct stdi_io *stdi)
{
    int rc = 0;
    if (hp == NULL)
        return -1;
#ifdef __XPI__
    rc = ioctl(hp->kernel_fd, FPGA_IOC_QUERY_STDI, stdi);
#endif
    return rc;
}

static int xpi_getHorizontalLines(HAL *hp)
{
    int rc = 0, active = hal_getActiveIntf(hp);

#ifdef __XPI__
    if (active & INTERFACE_DIGITAL) {
        struct v4l2_dv_timings timings;
        if (hal_v4l2_g_dv_timings(hp, &timings) == 0) 
            rc = timings.bt.height + timings.bt.vfrontporch + 
                timings.bt.vsync + timings.bt.vbackporch;
    }
    else if (active & INTERFACE_HV) {
        struct stdi_io stdiio;
        stdiio.subdev = V4L2_SUBDEV_ADV7604;
        if (xpi_get_stdi(hp, &stdiio) == 0)
            rc = stdiio.stdi.lcf + 1;
    }
#endif
    return rc;
}

static double xpi_getVerticalFrequency(HAL *hp)
{
    int active = hal_getActiveIntf(hp);
    double freq = (double) 0;
#ifdef __XPI__
    if (active & INTERFACE_DIGITAL) {
        struct v4l2_dv_timings timings;
        if (hal_v4l2_g_dv_timings(hp, &timings) == 0) {
            int htotal = timings.bt.width + timings.bt.hfrontporch + 
                timings.bt.hsync + timings.bt.hbackporch;
            int vtotal = timings.bt.height + timings.bt.vfrontporch + 
                timings.bt.vsync + timings.bt.vbackporch;
            freq = (double) timings.bt.pixelclock / (double) htotal / 
                (double) vtotal;
        }
    }
    else if (active & INTERFACE_HV) {
        struct stdi_io stdiio;
        stdiio.subdev = V4L2_SUBDEV_ADV7604;
        if (xpi_get_stdi(hp, &stdiio) == 0)
            freq = (double) ADV7604_fsc / (double) (stdiio.stdi.fcl * 256);
    }
#endif
    return freq;
}

static double xpi_getHorizontalFrequency(HAL *hp)
{
    int active = hal_getActiveIntf(hp);
    double freq = (double) 0;
#ifdef __XPI__
    if (active & INTERFACE_DIGITAL) {
        struct v4l2_dv_timings timings;
        if (hal_v4l2_g_dv_timings(hp, &timings) == 0) {
            int htotal = timings.bt.width + timings.bt.hfrontporch + 
                timings.bt.hsync + timings.bt.hbackporch;
            freq = (double) timings.bt.pixelclock / (double) htotal;
        }
    }
    else if (active & INTERFACE_HV) {
        struct stdi_io stdiio;
        stdiio.subdev = V4L2_SUBDEV_ADV7604;
        if (xpi_get_stdi(hp, &stdiio) == 0)
            freq = (double) ADV7604_fsc / (double) stdiio.stdi.bl * (double) 8;
    }
#endif
    return freq;
}

unsigned long hal_getVerticalFrequencyVal(HAL *hp)
{
    unsigned long val = 0;

    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) == 
                    AMTREG_HVSYNC)
                val = hal_get_fpga_val32_k(hp, FPGA_AMVREG);
            else if ((hal_get_fpga_val32_k(hp, FPGA_AMDVITREG) & AMTREG_HVSYNC) 
                    == AMTREG_HVSYNC)
                val = hal_get_fpga_val32_k(hp, FPGA_AMDVIVREG);
            break;
        case V2DI50_DEVICE:
            if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) == 
                    AMTREG_HVSYNC)
                val = hal_get_fpga_val32_k(hp, FPGA_AMVREG);
            break;
        case XPI_DEVICE: 
            VLOG_INFO("%s: not supported on XPI devices", __func__);
            break;
        default:
            break;
    }
    return val;                    
}

double hal_getVerticalFrequency(HAL *hp)
{
    int val = 0;
    double freq = (double) 0;

    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            val = hal_getVerticalFrequencyVal(hp);
            if (val != 0) {
                freq = ((double) V4_CLOCK_FREQ / (double) val) / 256.0;
            }
            break;
        case V2DI50_DEVICE:
            val = hal_getVerticalFrequencyVal(hp);
            if (val != 0) {
                freq = ((double) V2_CLOCK_FREQ / (double) val) / 256.0;
            }
            break;
        case XPI_DEVICE: {
            freq = xpi_getVerticalFrequency(hp);
            break;
        }
        default:
            break;
    }
    return freq;
}

unsigned long hal_getHorizontalFrequencyVal(HAL *hp)
{
    unsigned long val = 0;

    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) 
                    == AMTREG_HVSYNC)
                val = hal_get_fpga_val32_k(hp, FPGA_AMHREG);
            else if ((hal_get_fpga_val32_k(hp, FPGA_AMDVITREG) & AMTREG_HVSYNC) 
                    == AMTREG_HVSYNC)
                val = hal_get_fpga_val32_k(hp, FPGA_AMDVIHREG);
            break;
        case V2DI50_DEVICE:
            if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) 
                    == AMTREG_HVSYNC)
                val = hal_get_fpga_val32_k(hp, FPGA_AMHREG);
            break;
        case XPI_DEVICE: 
            VLOG_INFO("%s: not supported on XPI devices", __func__);
            break;
        default:
            break;
    }
    return val;
}

double hal_getHorizontalFrequency(HAL *hp)
{
    int val = 0;
    double freq = (double) 0;

    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            val = hal_getHorizontalFrequencyVal(hp);
            if (val != 0) {
                freq = ((double) V4_CLOCK_FREQ / (double) val) / 1.0E3;
            }
            break;
        case V2DI50_DEVICE:
            val = hal_getHorizontalFrequencyVal(hp);
            if (val != 0) {
                freq = ((double) V2_CLOCK_FREQ / (double) val) / 1.0E3;
            }
            break;
        case XPI_DEVICE: {
            freq = xpi_getHorizontalFrequency(hp) / 1.0E3;
        }
        default:
            break;
    }
    return freq;
}

unsigned long hal_getHorizontalLinesVal(HAL *hp)
{
    unsigned long hsyncc=0;

    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
            if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) 
                    == AMTREG_HVSYNC)
                hsyncc = hal_get_fpga_val32_k(hp, FPGA_AMLREG);
            else if ((hal_get_fpga_val32_k(hp, FPGA_AMDVITREG) & 
                        AMTREG_HVSYNC) == AMTREG_HVSYNC)
                hsyncc = hal_get_fpga_val32_k(hp, FPGA_AMDVILREG);
            break;
        case V2DI50_DEVICE:
            if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) 
                    == AMTREG_HVSYNC)
                hsyncc = hal_get_fpga_val32_k(hp, FPGA_AMLREG);
            break;
        case XPI_DEVICE: 
            VLOG_INFO("%s: not supported on XPI devices", __func__);
            break;
        default:
            break;
    }
    return hsyncc;
}

unsigned long hal_getHorizontalLines(HAL *hp)
{
    unsigned long hsyncc=0;
    switch (hal_get_hardware_type(hp)) {
        case PLX_DEVICE:
        case V2DI50_DEVICE:
            hsyncc = hal_getHorizontalLinesVal(hp);
            break;
        case XPI_DEVICE: 
            hsyncc = xpi_getHorizontalLines(hp);
            break;
        default:
            break;
    }
    return hsyncc;
}

int hal_getKeyboardType(HAL *hp) 
{
    int kbtype;
    if (hal_get_hardware_type(hp) == XPI_DEVICE ||
			hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_KB_TYPE_GET, &kbtype) < 0)
        return -1;
#endif
    return kbtype;
}

void hal_setKeyboardType(HAL *hp, int kbtype)
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE ||
			hal_get_board_type(hp) != BOARDTYPE_RX)
        return;
#ifndef __XPI__
    if (kbtype != hal_getKeyboardType(hp))
        ioctl(hp->kernel_fd, FPGA_IOC_KB_TYPE_SET, &kbtype);
#endif
}

int hal_getMouseType(HAL *hp) 
{
    int mstype;
    if (hal_get_hardware_type(hp) == XPI_DEVICE ||
			hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_MS_TYPE_GET, &mstype) < 0)
        return -1;
#endif
    return mstype;
}

int hal_getGenlockMode(HAL *hp)
{
    unsigned int dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG); 
    int hardware_type = hal_get_hardware_type(hp);

    if (hardware_type == PLX_DEVICE) {
        if (dcreg & DCREG_LRSYNC_DISABLE) 
            return GENLOCK_MODE_NONE;
        else if (dcreg & DCREG_GENLOCK_MODE) 
            return GENLOCK_MODE_SLAVE;
        else  
            return GENLOCK_MODE_MASTER;
    }
    else if (hardware_type == V2DI50_DEVICE) {
        if (dcreg & DCREG_FRLOCK_EXT)
            return GENLOCK_MODE_SLAVE;
        else
           return GENLOCK_MODE_MASTER;
    }
    return GENLOCK_MODE_MASTER;
}


void hal_setGenlockMode(HAL *hp, int mode)
{

    unsigned short dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
    int hardware_type = hal_get_hardware_type(hp);

    if (hardware_type == PLX_DEVICE) {
        VLOG_INFO_CAT(VLOG_HAL, 
            "Channel %d. Failed. Genlock mode must be set by HALD on XPs",
            hal_get_channel_number(hp));
    }
    else if (hardware_type == V2DI50_DEVICE) {
        dcreg &= ~(DCREG_FRLOCK_EXT | DCREG_DROP_ENABLE | DCREG_VID_ENABLE |
              DCREG_LAN_ENABLE);
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);

        /***************************
         * Set to slave mode first *
         ***************************/
        dcreg |= (DCREG_FRLOCK_EXT | DCREG_DROP_ENABLE | DCREG_VID_ENABLE |
              DCREG_LAN_ENABLE);
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);

        /**************************************************
         * If should set to master mode, check if there's *         
         * external clock and actually setting it.        *
         **************************************************/
        if (mode == GENLOCK_MODE_MASTER) {
            unsigned short pxcnt = hal_get_fpga_val_k(hp, FPGA_DGLKSTATUS2);
            dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
            /****************************************************
             * In slave mode, two consecutive reads of register *
             * FPGA_DGLKSTATUS2 will give different results if  *
             * there's clock on the lower Genlock connector.    *
             ****************************************************/
            pxcnt = hal_get_fpga_val_k(hp, FPGA_DGLKSTATUS2);
            usleep(1000);
            if (hal_get_fpga_version(hp) <= 4 ||
                    hal_get_fpga_val_k(hp, FPGA_DGLKSTATUS2) == pxcnt) {
                dcreg &= ~DCREG_FRLOCK_EXT;
                VLOG_INFO_CAT(VLOG_HAL,
                    "Channel %d. No external clock detected. "
                    "Setting to Genlock Master.", hal_get_channel_number(hp));
            }
            else {
                dcreg |= DCREG_FRLOCK_EXT;
                VLOG_INFO_CAT(VLOG_HAL,
                    "Channel %d. External clock detected. "
                    "Setting to Genlock Slave.", hal_get_channel_number(hp));
            }
            hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
        }
    }
}

int hal_getActiveIntf(HAL *hp)
{
    int fd = hal_get_video_fd(hp);
    int active_intf;
    if (ioctl(fd, FPGA_IOC_GET_INTERFACE, &active_intf) < 0)
        return -1;
    return active_intf;
}

int hal_setActiveIntf(HAL *hp, int intf)
{
    int fd = hal_get_video_fd(hp);
    if (ioctl(fd, FPGA_IOC_SET_INTERFACE, &intf) < 0)
        return -1;
    return 0;
}

int hal_forceActiveIntf(HAL *hp, int intf)
{
    int fd = hal_get_video_fd(hp);
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
    if (ioctl(fd, FPGA_IOC_FORCE_INTERFACE, &intf) < 0)
        return -1;
    return 0;
}

int hal_resetAddsideVideo(HAL *hp)
{
    int fd = hal_get_video_fd(hp);
    if (ioctl(fd, FPGA_IOC_RESET_ADDSIDE, 1) < 0)
        return -1;
    return 0;
}

int hal_resetDropsideVideo(HAL *hp)
{
    int fd = hal_get_video_fd(hp);
    if (ioctl(fd, FPGA_IOC_RESET_DROPSIDE, 1) < 0)
        return -1;
    return 0;
}

int hal_resetOutputVideo(HAL *hp)
{
    int fd = hal_get_video_fd(hp);
#ifdef __XPI__
    if (ioctl(fd, FPGA_IOC_RESET_OUTPUT, 1) < 0)
        return -1;
#endif
    return 0;
}

int hal_setDropsideVideo(HAL *hp, 
                         struct LUT_struct *rlutp,
                         struct LUT_struct *dlutp)
{
    int fd = hal_get_video_fd(hp);
    int adjust;
    double ddsfreq;
    struct pixadjust_io pio;

    if (hal_get_hardware_type(hp) != V2DI50_DEVICE) 
        return -1;

    if ((adjust = hal_get_pixel_adjust_k(hp)) < 0)
        return -1;

    if (dlutp == NULL)
        return -1;

    /* Push clock frequencies to kernel */
    hal_compute_dds_frequency(dlutp, adjust, &ddsfreq);
#ifndef __XPI__
    pio.pix_tweak_hi = ddsfreq / TWORAISED24;
    pio.pix_tweak_lo = ddsfreq - ((float) pio.pix_tweak_hi) * TWORAISED24;
    pio.pix_adjust = adjust;
    ioctl(fd, FPGA_IOC_SET_PIXADJUST, &pio);
#endif

    /* Configure display LUT */
    if (ioctl(fd, FPGA_IOC_SET_DISPLAYLUT, dlutp) < 0)
        return -1;

    /* Configure dropside remote LUT */
    if (ioctl(fd, FPGA_IOC_SET_DROPSIDELUT, rlutp) < 0)
        return -1;

    return hal_resetDropsideVideo(hp);
}

int hal_getStereo(HAL *hp)
{
    int val = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        val = hal_get_s6fpga_val32_k(hp, S6FPGA_AMDVITREG);
#endif
    }
    else {
        val = hal_get_fpga_val_k(hp, FPGA_AMTREG);
    }
    return (val & AMTREG_STEREO) ? 1 : 0;
}

int hal_get_framerate_divider(HAL *hp)
{
    int val = hal_get_fpga_val_k(hp, FPGA_ASKIPCNT);
    int numbits = hal_get_fpga_val_k(hp, FPGA_ASKIPCNTBITS) + 1;
    int hardware_category = get_hardware_category();

    if (hardware_category == HARDWARE_CATEGORY_XP || 
            hardware_category == HARDWARE_CATEGORY_XPI) {
        if (val == 1) {
            val = hal_get_fpga_val_k(hp, FPGA_ASKIPCNTBITS);
            return ((val & 0xff) + 1);
        }
        else {
            int i, bitval = 0;
            for (i = 0; i < numbits; i++)
                if (val & (1 << i)) bitval++;
            return bitval / numbits;
        }
    }
    else if (hardware_category == HARDWARE_CATEGORY_I50) {
        if ((val & ASKIPCNT_ENABLE) == 0)
            return 1;
        else 
            return ((val & 0xff) + 1);
    }
    return 0;
}

void hal_set_framerate_divider(HAL * hp, int val)
{
    int hardware_category = get_hardware_category();
    if (hardware_category == HARDWARE_CATEGORY_XP || 
            hardware_category == HARDWARE_CATEGORY_XPI) { 
        val = (val <= 0) ? 1 : val;
        val = (val > 16) ? 16 : val;
        hal_set_fpga_val_k(hp, FPGA_ASKIPCNT, 0x01);
        hal_set_fpga_val_k(hp, FPGA_ASKIPCNTBITS, val - 1);
    }
    else if (hardware_category == HARDWARE_CATEGORY_I50) {
        val = (val > 256) ? 256 : val;
        hal_set_fpga_val_k(hp, FPGA_ASKIPCNT, ASKIPCNT_ENABLE | (val - 1));
    }
}

void hal_adjustCompression(HAL *hp, 
                           int low_comp,
                           int low_comp_delta,
                           int high_comp,
                           int high_comp_delta)
{

    unsigned short val;
    int mq, dcq, mql, dcql, comp, compl;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return;
    compl = low_comp + low_comp_delta;
    comp = high_comp + high_comp_delta;

    if (compl < 0)
        compl = 0;
    if (compl > 10)
        compl = 10;
    if (comp < 0)
        comp = 0;
    if (comp > 10)
        comp = 10;

    mq = mquant_from_compression(comp);
    dcq = dcquant_from_compression(comp);
    mql = mquant_from_compression(compl);
    dcql = dcquant_from_compression(compl);

    val = (mq << MQUANT_SHIFT) | (dcq << DCQUANT_SHIFT) |
        (mql << MQUANT_LOW_SHIFT) | (dcql << DCQUANT_LOW_SHIFT);

    hal_set_fpga_val_k(hp, FPGA_AQUANT, val);
}

int hal_get_hd_k(HAL *hp)
{
    int shift = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        shift = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_HORIZ_POS);
#endif
    }
    else {
        int fd = hal_get_video_fd(hp);
        if (ioctl(fd, FPGA_IOC_GET_HDELAY, &shift) < 0) 
            return 0;
    }
    return shift;
}

void hal_adjust_hd_k(HAL *hp, int shift)
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_HORIZ_POS, shift);
#endif
    }
    else {
        int fd = hal_get_video_fd(hp);
        ioctl(fd, FPGA_IOC_SET_HDELAY, &shift);
    }
}

int hal_get_vd_k(HAL *hp)
{
    int shift = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        shift = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_VERT_POS);
#endif
    }
    else {
        int fd = hal_get_video_fd(hp);
        if (ioctl(fd, FPGA_IOC_GET_VDELAY, &shift) < 0)
            return 0;
    }
    return shift;
}

void hal_adjust_vd_k(HAL *hp, int shift)
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_VERT_POS, shift);
#endif
    }
    else {
        int fd = hal_get_video_fd(hp);
        ioctl(hp->kernel_fd, FPGA_IOC_SET_VDELAY, &shift);
    }
}

unsigned char hal_get_phase_adjust_k(HAL *hp)
{
    unsigned char val = 0;
    int hardware_type = hal_get_hardware_type(hp);
#ifndef __XPI__
    if (hardware_type == PLX_DEVICE) 
        val = hal_get_v4_ad9888_val_k(hp, VIF_PHASE_ADJ) >> 3;
    else if (hardware_type == V2DI50_DEVICE) {
        if (ioctl(hp->kernel_fd, FPGA_IOC_GET_PSHIFT, &val) < 0)
            return 0;
    }
#else
    val = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_SAMPLING_PHASE);
#endif
    return val;
}


void hal_set_phase_adjust_k(HAL *hp, unsigned char val)
{
    int hardware_type = hal_get_hardware_type(hp);
#ifndef __XPI__
    if (hardware_type == PLX_DEVICE) 
        hal_set_v4_ad9888_val_k(hp, VIF_PHASE_ADJ, val << 3);
    else if (hardware_type == V2DI50_DEVICE)
        ioctl(hp->kernel_fd, FPGA_IOC_SET_PSHIFT, &val);
#else
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_SAMPLING_PHASE, val);
#endif
}

int hal_get_pixel_adjust_k(HAL *hp)
{
#ifndef __XPI__
    struct pixadjust_io pio;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_PIXADJUST, &pio) < 0)
        return -1;

    return pio.pix_adjust;
#else
    return 0;
#endif
}


/***********************************************************
 * Compute frequency equivalent for 'tweak' pixels offset  *
 * to 'lutp' (must already have all the DDS values already *
 * computed). Returns freq values in 'freq_hi' & 'freq_lo' *
 ***********************************************************/
int hal_compute_dds_frequency(struct LUT_struct *lp, 
                              int tweak,
                              double *ddsfreq) {

    double vf_ddsfreq;
    int vf_hi, vf_lo;

    if (lp == NULL)
        return -1;

    vf_hi = (unsigned int) (lp->d_dds5 << 16);
    vf_hi += (unsigned int) (lp->d_dds4 << 8);
    vf_hi += (unsigned int) lp->d_dds3;
    vf_lo = (unsigned int) (lp->d_dds2 << 16);
    vf_lo += (unsigned int) (lp->d_dds1 << 8);
    vf_lo += (unsigned int) lp->d_dds0;

    vf_ddsfreq = ((double) vf_hi * TWORAISED24) + vf_lo;

    vf_ddsfreq += (double) tweak * (double) 10000 * lp->d_chgperbit;

    *ddsfreq = vf_ddsfreq;

    return 0; 
}

void hal_set_pixel_adjust_k(HAL *hp, int pixels)
{
#ifndef __XPI__
    struct pixadjust_io pio;
    struct LUT_struct lut;
    double ddsfreq;

    if (hal_get_hardware_type(hp) != V2DI50_DEVICE)
        return;  

    if (hal_get_dropside_lut(hp, &lut) < 0) {
        pio.pix_tweak_hi = pio.pix_tweak_lo = 0;
    }
    else {
        hal_compute_dds_frequency(&lut, pixels, &ddsfreq);
        pio.pix_tweak_hi = ddsfreq / TWORAISED24;
        pio.pix_tweak_lo = ddsfreq - ((float) pio.pix_tweak_hi) * TWORAISED24;
    }
    pio.pix_adjust = pixels;

    ioctl(hp->kernel_fd, FPGA_IOC_SET_PIXADJUST, &pio);
#endif
}

int hal_get_charge_pump_curr_k(HAL *hp)
{
#ifndef __XPI__
    unsigned short vcocpmp;
    int hardware_type = hal_get_hardware_type(hp);
    int hwversion = hal_get_hardware_version(hp);
    
    if (hardware_type == PLX_DEVICE) {
        vcocpmp = hal_get_v4_ad9888_val_k(hp, VIF_VCO_CPMP);
        return (vcocpmp & 0x38) >> 3;
    }
    else if (hardware_type == V2DI50_DEVICE) {
        vcocpmp = hal_get_ad9888_val_k(hp, VIF_VCO_CPMP);

        if (hwversion == HARDWARE_VERSION_A) {      /* A version */
            vcocpmp = hal_get_ad9888_val_k(hp, VIF_VCO_CPMP);
            return (vcocpmp & 0x38) >> 3;
        }
        else {                                      /* DA version */
            vcocpmp = hal_get_ad9888_val_k(hp, VIF_VCO_CPMP);
            return (vcocpmp & 0x1c) >> 2;
        }
    }
#endif
    return 0;
}

void hal_set_default_charge_pump_curr_k(HAL *hp, int val)
{
#ifndef __XPI__
    int hardware_type = hal_get_hardware_type(hp);
    if (val < 0 || (val != 0xff && val > 7))
        return;
    if (hardware_type == PLX_DEVICE) {
        int cpmp = DEFAULT_VCO_CPMP;
        if (val >= 0 && val <= 7) 
            cpmp = (DEFAULT_VCO_CPMP & 0xc7) | (val << 3);
        hal_set_v4_ad9888_val_k(hp, VIF_VCO_CPMP, cpmp);
    }
    else if (hardware_type == V2DI50_DEVICE)
        ioctl(hp->kernel_fd, FPGA_IOC_SET_DEFAULT_CPMP, &val);
#endif
}

int hal_get_default_charge_pump_curr_k(HAL *hp)
{
    int cpmp = 0;
#ifndef __XPI__
    int hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == PLX_DEVICE) {
        cpmp = hal_get_v4_ad9888_val_k(hp, VIF_VCO_CPMP);
        if (cpmp == DEFAULT_VCO_CPMP) 
            cpmp = 0xff;
        else      
            cpmp = (cpmp & 0x38) >> 3;
    }
    else if (hardware_type == V2DI50_DEVICE) {
        if (ioctl(hp->kernel_fd, FPGA_IOC_GET_DEFAULT_CPMP, &cpmp) < 0)
            return -1;
    }
#endif
    return cpmp;
}

/* Returns # 32-bit words FPGA has sent or received on XPi */
unsigned long hal_get_xpi_videowords(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_XPI_BWCNT);
    }
#endif
    return rc;
}

unsigned long hal_get_xpi_audiowords(HAL *hp, int transmit)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        if (transmit)
            rc = hal_get_fpga_val32_k(hp, FPGA_XPI_TX_ABWCNT);
        else
            rc = hal_get_fpga_val32_k(hp, FPGA_XPI_RX_ABWCNT);
    }
#endif
    return rc;
}

/* Returns # 16-bit packets FPGA has received on XPi */
unsigned long hal_get_xpi_videopkts(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_XPI_RX_VPKTCNT);
    }
#endif
    return rc;
}

unsigned long hal_get_xpi_audiopkts(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_XPI_RX_APKTCNT);
    }
#endif
    return rc;
}

unsigned long hal_get_xpi_videoloss(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_AVPACKETLOSS);
    }
#endif
    return rc;
}

unsigned long hal_get_xpi_videodrop(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_XPI_VIDEO_DROP_1) +
            hal_get_fpga_val32_k(hp, FPGA_XPI_VIDEO_DROP_2);
    }
#endif
    return rc;
}

unsigned long hal_get_xpi_audioloss(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_XPI_AUDIO_LOSS);
    }
#endif
    return rc;
}

unsigned long hal_get_xpi_audiodrop(HAL *hp)
{
    unsigned long rc = 0;
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
        rc = hal_get_fpga_val32_k(hp, FPGA_XPI_AUDIO_DROP);
    }
#endif
    return rc;
}

int hal_get_fc_params(HAL *hp, struct fc_param_io *fcpiop)
{
    int rc = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        fcpiop->frame_cnt = hal_get_fpga_val_k(hp, FPGA_AFRMCNTREG);
        fcpiop->slice_cnt = hal_get_fpga_val_k(hp, FPGA_ASLICECNTREG);
    }
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_PARAM_GET, fcpiop) < 0) 
        return -1;
#endif
    return rc;
}


int hal_get_fc_wparams(HAL *hp, struct fc_wparam_io *fcwpiop)
{
    int rc = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        fcwpiop->wframe_cnt = hal_get_fpga_val_k(hp, FPGA_DFRMCNTREG);
        fcwpiop->wslice_cnt = hal_get_fpga_val_k(hp, FPGA_DSLICECNTREG);
    }
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_WPARAM_GET, fcwpiop) < 0)
        return -1;
#endif
    return rc;
}

int hal_get_fc_read_stats(HAL *hp, struct fc_stat_io *fcsiop)
{
    if (fcsiop == NULL)
        return 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        memset(fcsiop, 0, sizeof(struct fc_stat_io));
    }
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_RSTAT_GET, fcsiop) < 0)
        return -1;
#endif
    return 0;
}

int hal_get_fc_write_stats(HAL *hp, struct fc_stat_io *fcsiop)
{
    if (fcsiop == NULL)
        return 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        fcsiop->video_stats.buffer_drops = 
            hal_get_fpga_val_k(hp, FPGA_XPI_VIDEO_DROP_1);
        fcsiop->video_stats.queue_full = 
            hal_get_fpga_val_k(hp, FPGA_XPI_VIDEO_DROP_2);
#endif

    }
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_WSTAT_GET, fcsiop) < 0)
        return -1;
#endif
    return 0;
}

int hal_get_kbm_params(HAL *hp, struct kbm_param_io *kbmpiop)
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_KBM_PARAM_GET, kbmpiop) < 0)
        return -1;
#endif
    return 0;
}

int hal_reset_kbm_params(HAL *hp)
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_KBM_PARAM_SET, NULL) < 0)
        return -1;
#endif
    return 0;
}


int hal_flush_kbm_queues_k(HAL *hp)
{
    int dummyval = 0;
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_KBM_FLUSH, &dummyval) < 0)
        return -1;
#endif
    return 0;
}


int hal_flush_fc_queues_k(HAL *hp)
{
    int dummyval = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_FLUSH, &dummyval) < 0)
        return -1;
#endif
    return 0;
}

int hal_get_fc_queue_limits(HAL *hp, struct fc_queue_limits_io *fcqlimitiop)
{
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_QUEUE_GET, fcqlimitiop) < 0)
        return -1;
#endif
    return 0;
}

int hal_set_fc_queue_limits(HAL *hp, struct fc_queue_limits_io *fcqlimitiop)
{
    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_FC_QUEUE_SET, fcqlimitiop) < 0)
        return -1;
#endif
    return 0;
}

int hal_set_ext_sync(HAL *hp, int val)
{
    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_SET_EXTSYNC, &val) < 0)
        return -1;
#endif
    return 0;
}

/* Determine if FPGA is outputting any video */
int hal_real_video_available(HAL *hp)
{
    int ival = 1;
    if (hp == NULL)
        return 0;

    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_VID_AVL, &ival) < 0)
        return 0;
    return ival;
}

int hal_set_video_available(HAL * hp, int val)
{
    if (hp == NULL)
        return -1;
    if (ioctl(hp->kernel_fd, FPGA_IOC_SET_VID_AVL, &val) < 0)
        return -1;
    return val;
}

/* Routine to enable FPGA addside video on XPi TX */
int hal_enable_addside_video(HAL *hp) 
{
    int intf, rc = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE &&
            hal_get_board_type(hp) == BOARDTYPE_TX) {
#ifdef __XPI__
        intf = hal_getActiveIntf(hp);
        rc = hal_setActiveIntf(hp, intf | INTERFACE_ADDSIDE_VIDEO_ENABLE);
#endif
    }
    return rc; 
}

/* Routine to disable addside video on XPi TX */
int hal_disable_addside_video(HAL *hp) 
{
    int intf, rc = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE &&
            hal_get_board_type(hp) == BOARDTYPE_TX) {
#ifdef __XPI__
        intf = hal_getActiveIntf(hp);
        rc = hal_setActiveIntf(hp, intf & ~INTERFACE_ADDSIDE_VIDEO_ENABLE);
#endif
    }
    return rc;
}

/* Routine to enable addside audio on XPi */
int hal_enable_addside_audio(HAL *hp) 
{
    int intf, rc = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        intf = hal_getActiveIntf(hp);
        rc = hal_setActiveIntf(hp, intf | INTERFACE_ADDSIDE_AUDIO_ENABLE);
#endif
    }
    return rc; 
}

/* Routine to disable addside audio on XPi */
int hal_disable_addside_audio(HAL *hp) 
{
    int intf, rc = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        intf = hal_getActiveIntf(hp);
        rc = hal_setActiveIntf(hp, intf & ~INTERFACE_ADDSIDE_AUDIO_ENABLE); 
#endif
    }
    return rc; 
} 

/* Routine to enable dropside audio on XPi */
int hal_enable_dropside_audio(HAL *hp) 
{
    int intf, rc = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        intf = hal_getActiveIntf(hp);
        rc = hal_setActiveIntf(hp, intf | INTERFACE_DROPSIDE_AUDIO_ENABLE);
#endif
    }
    return rc; 
}

/* Routine to disable dropside audio on XPi */
int hal_disable_dropside_audio(HAL *hp) 
{
    int intf, rc = 0;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        intf = hal_getActiveIntf(hp);
        rc = hal_setActiveIntf(hp, intf & ~INTERFACE_DROPSIDE_AUDIO_ENABLE); 
#endif
    }
    return rc; 
} 

static int configure_driver_splash(HAL *hp, int type, struct LUT_struct *lutp)
{
    int rc = 0, fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;
    
    build_lut_entry(lutp, XPI_DEVICE);

#ifdef __XPI__
    /* set splash type */
    hal_set_s6fpga_val32_k(hp, S6FPGA_SPLASHPATT, (type == SPLASH_TYPE_BLUE) ?
            S6FPGA_SPLASHPATT_VALUE_BLUE : S6FPGA_SPLASHPATT_VALUE_GRID);

    /* enable splash */
   rc = ioctl(fd, FPGA_IOC_SET_SPLASH, lutp);
#endif
   return rc;
}

static void enable_driver_splash(HAL *hp)
{
#ifdef __XPI__
    if (hal_get_board_type(hp) == BOARDTYPE_RX) {
        hal_or_in_fpga_val_k(hp, FPGA_DSPLASHCTRL, DSPLASHCTRL_SPLASH);
    }
    else {
        hal_or_in_s6fpga_val32_k(hp, S6FPGA_ASPLASHCTRL, ASPLASHCTRL_SPLASH);
    }
#endif
}

static void disable_driver_splash(HAL *hp)
{
#ifdef __XPI__
    if (hal_get_board_type(hp) == BOARDTYPE_RX) {
        hal_and_in_fpga_val_k(hp, FPGA_DSPLASHCTRL, ~DSPLASHCTRL_SPLASH);
    }
    else {
        hal_and_in_s6fpga_val32_k(hp, S6FPGA_ASPLASHCTRL, ~ASPLASHCTRL_SPLASH);
    }
#endif
}

void hal_enable_splash_in_driver(HAL *hp, int type)
{
    /* set splash type */
#ifdef __XPI__
    hal_set_s6fpga_val32_k(hp, S6FPGA_SPLASHPATT, (type == SPLASH_TYPE_BLUE) ?
            S6FPGA_SPLASHPATT_VALUE_BLUE : S6FPGA_SPLASHPATT_VALUE_GRID);
    /* enable splash in driver */
    enable_driver_splash(hp);
#endif
}

void hal_enable_splash_video(HAL *hp, int type, struct LUT_struct *lutp) {

    if (hal_get_hardware_type(hp) != XPI_DEVICE) 
        return;
#ifdef __XPI__ 
    if (hal_get_board_type(hp) == BOARDTYPE_RX) {
        struct display_config dcfg;

        /* set LUT ID so hald can identify it as splash */
        lutp->a_alutreg = RESERVED_RX_LUTID;
        build_lut_entry(lutp, XPI_DEVICE);
        get_display_config(hp, &dcfg); 

        /* no need to do anything if not remote mode */
        if (dcfg.source == DISPLAY_SOURCE_REMOTE) {
            struct override_config ocfg;
            struct LUT_struct olut;

            xp_get_override_config(hp, &ocfg);
            if (get_override_display_lut(hp, lutp, &olut, &ocfg) < 0) {
                VLOG_INFO("%s: Failed to get override LUT. Using LUT ID %d "
                        "(%s) for display", __func__, lutp->a_alutreg, 
                        lutp->comment);
                hal_set_display_lut(hp, lutp);
                xpi_set_pll_frequency(hp, lutp->a_pcr);
            }
            else {
                VLOG_INFO("Setting display LUT to ID %d (%s)", olut.a_alutreg, 
                        olut.comment);
                hal_set_display_lut(hp, &olut);
                xpi_set_pll_frequency(hp, olut.a_pcr);
            }
        }
    }
    configure_driver_splash(hp, type, lutp);
#endif
}

void hal_disable_splash_video(HAL *hp)
{
    if (hal_get_hardware_type(hp) != XPI_DEVICE)
        return;
#ifdef __XPI__
    disable_driver_splash(hp);
#endif
}

int hal_set_adc_free_run(HAL *hp, int lutid)
{
    if (hal_get_hardware_type(hp) != XPI_DEVICE)
        return -1;
#ifdef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_SET_FREE_RUN, &lutid) < 0) {
        return -1;
    }
#endif
    return 0;
}

int hal_force_panic(HAL *hp)
{
    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
#ifndef __XPI__
    if (ioctl(hp->kernel_fd, FPGA_IOC_RESET) < 0)
        return -1;
#endif
    return 0;
}

int hal_get_addside_lut(HAL * hp, struct LUT_struct *lutp)
{
    int fd;

    if (lutp == NULL || (fd = hal_get_video_fd(hp)) < 0) {
        return -1;
    }
    if (ioctl(fd, FPGA_IOC_GET_ADDSIDELUT, lutp) < 0) {
        return -1;
    }
    else if (hal_get_hardware_type(hp) == XPI_DEVICE) 
        build_lut_entry(lutp, XPI_DEVICE);
    return 0;
}

int hal_configure_addside_lut(HAL *hp, 
                              unsigned short hsyncc, 
                              unsigned short amhreg,
                              unsigned short amvreg, 
                              unsigned short amtreg, 
                              unsigned short duallink,
                              struct LUT_struct *lutp) 
{
    int rc = 0;

    if (hal_get_hardware_type(hp) != XPI_DEVICE) {
        struct video_param_io vio;
        vio.hsyncc = hsyncc;
        vio.amhreg = amhreg;
        vio.amvreg = amvreg;
        vio.amtreg = amtreg;
        vio.duallink = duallink;
        if (ioctl(hp->kernel_fd, FPGA_IOC_SET_ADDSIDELUT, &vio) < 0) 
            rc = -1;
        else
            memcpy(lutp, &(vio.alut), sizeof(struct LUT_struct));
    }

    return rc;
}

int hal_get_dropside_lut(HAL *hp, struct LUT_struct *lutp)
{
    int retries = 0;
    int fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;

    while (retries < 3) {
        if (ioctl(fd, FPGA_IOC_GET_DROPSIDELUT, lutp) < 0) {
            if (errno == EBUSY) {
                usleep(10000);
                retries++;
            }
            else
                break;
        }
        else {
            return 0;
        }
    }
    return -1;
}

static int apply_license_check(HAL *hp, struct LUT_struct *lutp)
{
#ifdef __XPI__
    int license;

    if (lutp == NULL) 
        return 0;
    if ((license = hal_get_configured_license(hp)) == LITE_LICENSE) {
        if ((lutp->a_pcr > PCR_DUALLINK_XPI / 1000)) {
            return -1;
        }
        lutp->a_stereo = LUT_STEREO_NO;
    }
    else if (license == PRO_LICENSE) {
        lutp->a_stereo = LUT_STEREO_NO;
    }
#endif

    return 0;
}

int hal_set_dropside_lut(HAL *hp, struct LUT_struct *lutp) 
{
    int rc = 0, val, fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;

    /* Lock access to dropside LUT */
    val = 1;
    if (ioctl(fd, FPGA_IOC_LOCK_DROPSIDELUT, &val) < 0) 
        return -1;
#ifdef __XPI__
    /* Check license restrictions */
    {
        int flags = hal_get_fpga_flags(hp);
        if (apply_license_check(hp, lutp) < 0) {
            struct LUT_struct splash_lut;
            (void) get_rx_splash_lut(hp, &splash_lut);
            /* 
             * Setting splash type to blue does nothing right now since
             * the FPGA only supports grid line splash
             */
            configure_driver_splash(hp, SPLASH_TYPE_BLUE, &splash_lut);
            memcpy(lutp, &splash_lut, sizeof(struct LUT_struct));
            hal_set_fpga_flags(hp, flags | FLAG_XPI_LICENSE_VIOLATION); 
        }
        else {
            disable_driver_splash(hp);
            hal_set_fpga_flags(hp, flags & ~FLAG_XPI_LICENSE_VIOLATION); 
        }
    }
#endif

    rc = ioctl(fd, FPGA_IOC_SET_DROPSIDELUT, lutp);

    /* Unlock access to dropside LUT */
    val = 0;
    if (ioctl(fd, FPGA_IOC_LOCK_DROPSIDELUT, &val) < 0) 
        rc = -1;

    return rc;
}

static int configure_remote_mode_video(HAL *hp,
                                       struct override_config *ocfg,
                                       int hscale,
                                       int vscale)
{
    struct ov_param_io ovpio;

    /* Do nothing if not overriding */
    if (ocfg->ov_option == OV_OPTION_NONE)
        return 0;

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

    return 0;
}

int hal_set_dropside_video(HAL *hp, struct LUT_struct *lutp) 
{
    int fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;

#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        struct LUT_struct oldlut;
        struct ov_param_io ov;
        struct override_config ocfg;
        int hs, vs;

        if (hal_get_dropside_lut(hp, &oldlut) < 0) {
            memset(&oldlut, 0, sizeof(struct LUT_struct));
            oldlut.a_alutreg = RESERVED_INVALID_LUTID;
        }
        hal_set_dropside_lut(hp, lutp);

        if (hal_get_overlay_info(hp, &ov) < 0) {
            VLOG_INFO_CAT(VLOG_HAL, 
                    "%s: failed to get overlay info", __func__);
            return 0;
        }
        /* no need to do anything else if not remote mode */
        if (!(ov.flags & OV_FLAG_SRC_REMOTE)) 
            return 0;

        /* check override mode */
        xp_get_override_config(hp, &ocfg);

        /* update display LUT and update pixel clock if not overriding */
        if (ocfg.ov_option == OV_OPTION_NONE) {
            hal_set_display_lut(hp, lutp);
            if (lutp->a_alutreg != RESERVED_INVALID_LUTID) 
                xpi_set_pll_frequency(hp, lutp->a_pcr);
        }
        /* update override LUT if in auto or monitor override */
        else {
            struct LUT_struct olut;
            if (get_override_lut_and_scale(hp, lutp, &olut, &ocfg, &hs, &vs)) {
                VLOG_INFO_CAT(VLOG_HAL, "%s: failed to get override LUT",
                        __func__);
                memcpy(&olut, lutp, sizeof(struct LUT_struct));
            }

            VLOG_INFO_CAT(VLOG_HALD, 
                    "%s: overriding %s with %s. Scale %d/%d.", __func__, 
                    lutp->comment, olut.comment, hs, vs);

            olut.d_ov_control = DOVCONTROL_ENABLE | ocfg.ov_color;
            hal_set_display_lut(hp, &olut);
            configure_remote_mode_video(hp, &ocfg, hs, vs);
            if (olut.a_alutreg != RESERVED_INVALID_LUTID) 
                xpi_set_pll_frequency(hp, olut.a_pcr);
        }
    }
#endif
    return 0;
}

int hal_check_display_lut(HAL *hp)
{
    struct LUT_struct tmplut;
    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_DISPLAYLUT, &tmplut) < 0)
        return -1;
    return 0;
}

int hal_get_display_lut(HAL *hp, struct LUT_struct *lutp)
{
    int fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;

    if (ioctl(fd, FPGA_IOC_GET_DISPLAYLUT, lutp) < 0)
        return -1;

    return 0;
}

int hal_set_display_lut(HAL *hp, struct LUT_struct *lutp) {

    int fd = hal_get_video_fd(hp);
    int val;

    /* No need to test for NULL lutp. We allow display LUT to be NULL. */

    /* Lock access to display LUT */
    val = 1;
    if (ioctl(fd, FPGA_IOC_LOCK_DISPLAYLUT, &val) < 0)
        return -1;

    if (ioctl(fd, FPGA_IOC_SET_DISPLAYLUT, lutp) < 0) 
        return -1;

    /* Unlock access to display LUT */
    val = 0;
    if (ioctl(fd, FPGA_IOC_LOCK_DISPLAYLUT, &val) < 0)
        return -1;

    return 0;
}

int hal_get_fpga_flags(HAL *hp) 
{
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        int flags, fd = hal_get_video_fd(hp);
        if (ioctl(fd, FPGA_IOC_GET_FPGA_FLAGS, &flags) < 0) 
            return 0;
        return flags;
    }
#endif
    return 0;
}

int hal_set_fpga_flags(HAL *hp, int flags) 
{
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        int fd = hal_get_video_fd(hp);
        if (ioctl(fd, FPGA_IOC_SET_FPGA_FLAGS, &flags) < 0) 
            return -1;
        return 0;
    }
#endif
    return -1;
}

int hal_set_ppscapacity(HAL *hp, unsigned long ppscapacity)
{
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        int fd = hal_get_video_fd(hp);
        if (ioctl(fd, FPGA_IOC_SET_PPSCAPACITY, &ppscapacity) < 0) 
            return -1;
        return 0;
    }
#endif
    return -1;
}

int hal_set_display_video(HAL *hp, struct LUT_struct *lutp) 
{
    int fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;

    if (lutp->a_alutreg == RESERVED_INVALID_LUTID) {
        hal_set_display_lut(hp, NULL);
        return 0;
    }
#ifdef __XPI__
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        struct LUT_struct oldlut;
        struct ov_param_io ov;
        if (hal_get_display_lut(hp, &oldlut) < 0) {
            memset(&oldlut, 0, sizeof(struct LUT_struct));
            oldlut.a_alutreg = RESERVED_INVALID_LUTID;
        }
        if (compare_luts(lutp, &oldlut) < 0 || 
                (oldlut.a_alutreg != lutp->a_alutreg)) {
            hal_set_display_lut(hp, lutp);
            if (lutp->a_alutreg != RESERVED_INVALID_LUTID) {
                xpi_set_pll_frequency(hp, lutp->a_pcr);
            }
        }
        else if (oldlut.d_ov_control != lutp->d_ov_control) {
            hal_set_s6fpga_val32_k(hp, S6FPGA_DOVCONTROL, lutp->d_ov_control);
        }
    }
#endif
    return 0;
}

int hal_retrieve_lut(HAL *hp, struct LUT_struct *lutp)
{
    int fd = hal_get_video_fd(hp);

    if (lutp == NULL)
        return -1;
    if (ioctl(fd, FPGA_IOC_GET_LUT, lutp) < 0) {
        return -1;
    }

    /* fixup the pixel clock and refresh (non-integer) fields of the entry */
    if (lutp->a_lrefresh != 0)
        lutp->a_refresh = (double) lutp->a_lrefresh / (double) 1000;
    if (lutp->a_lpcr != 0)
        lutp->a_pcr = (double) lutp->a_lpcr / (double) 1000;

    /* fixup hres and vres fields of the entry */
    build_lut_entry(lutp, XPI_DEVICE);

    return 0;
}

int hal_insert_lut(HAL *hp, struct LUT_struct *lutp)
{
    int fd = hal_get_video_fd(hp);

    if (lutp == NULL) {
        return -1;
    }

    if (ioctl(fd, FPGA_IOC_SET_LUT, lutp) < 0) {
        return -1;
    }

    return 0;
}

int hal_reset_lut(HAL *hp)
{
    int fd = hal_get_video_fd(hp);

    if (ioctl(fd, FPGA_IOC_RESET_LUT, 0) < 0) {
        return -1;
    }
    return 0;
}

int hal_refresh_lut(HAL *hp)
{
    int flags, fd = hal_get_video_fd(hp);
    char buf[80];

    if (hal_get_hardware_type(hp) != XPI_DEVICE)
        return -1;
#ifdef __XPI__
    flags = hal_get_fpga_flags(hp);
    hal_set_fpga_flags(hp, flags | FLAG_XPI_REFRESH_LUT);
#endif
    return 0;
}

int hal_fire_interrupt(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);
    if (hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
    if (ioctl(fd, FPGA_IOC_INTR_FIRE, &val) < 0)
        return -1;
    return 0;
}

int hal_set_addside_mquant(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);
    if (hp == NULL)
        return -1;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = hal_get_fpga_val_k(hp, FPGA_AQUANT);
        if (val & 0x8000) {
            sval &= ~MQUANT_LOW_MASK;
            sval |= (val & 0x0f) << MQUANT_LOW_SHIFT;
            hal_set_fpga_val_k(hp, FPGA_AQUANT, sval);
        } 
        else {
            sval &= ~MQUANT_MASK;
            sval |= ((val & 0x0f) << MQUANT_SHIFT);
            hal_set_fpga_val_k(hp, FPGA_AQUANT, sval);
        }
    }
    else if (ioctl(fd, FPGA_IOC_SET_AMQUANT, &val) < 0)
        return -1;
    return 0;
}


int hal_get_addside_mquant(HAL *hp, int *val)
{
    int fd = hal_get_video_fd(hp);
    if (hp == NULL)
        return -1;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = hal_get_fpga_val_k(hp, FPGA_AQUANT);
        if (*val & 0x8000) {
            sval &= MQUANT_LOW_MASK;
            *val = (sval >> MQUANT_LOW_SHIFT);
        } 
        else {
            sval &= MQUANT_MASK;
            *val = (sval >> MQUANT_SHIFT);
        }
    }
    else {
        if (ioctl(fd, FPGA_IOC_GET_AMQUANT, val) < 0)
            return -1;
    }
    return 0;
}


int hal_set_addside_dcquant(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);
    if (hp == NULL)
        return -1;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = hal_get_fpga_val_k(hp, FPGA_AQUANT);
        if (val & 0x8000) {
            sval &= ~DCQUANT_LOW_MASK;
            sval |= ((val & 0x0f) << DCQUANT_LOW_SHIFT);
            hal_set_fpga_val_k(hp, FPGA_AQUANT, sval);
        }
        else {
            sval &= ~DCQUANT_MASK;
            sval |= ((val & 0x0f) << DCQUANT_SHIFT);
            hal_set_fpga_val_k(hp, FPGA_AQUANT, sval);
        }
    } 
    else {
        if (ioctl(fd, FPGA_IOC_SET_ADCQUANT, &val) < 0)
            return -1;
    }
    return 0;
}


int hal_get_addside_dcquant(HAL *hp, int *val)
{
    int fd = hal_get_video_fd(hp);
    if (hp == NULL)
        return -1;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = hal_get_fpga_val_k(hp, FPGA_AQUANT);
        if (*val & 0x8000) {
            sval &= DCQUANT_LOW_MASK;
            *val = (sval >> DCQUANT_LOW_SHIFT);
        } 
        else {
            sval &= DCQUANT_MASK;
            *val = (sval >> DCQUANT_SHIFT);
        }
    }
    else { 
        if (ioctl(fd, FPGA_IOC_GET_ADCQUANT, val) < 0)
            return -1;
    }
    return 0;
}

void hal_set_addside_bandwidth(HAL *hp, int bw, float burst_ratio, int burst_duration)
{
#ifdef __XPI__
    double pkt_time, bw_cnt, avail_bw_cnt;

    /* packet time (in secs) = bytes per packet / bandwidth in bytes per sec */
    pkt_time = (double) XPI_FPGA_PACKET_SIZE / (double) bw;  

    /* compute FPGA bandwidth counter value */
    bw_cnt = pkt_time * (double) XPI_CLOCK_FREQ; 

    hal_set_fpga_val32_k(hp, FPGA_ABANDWIDTHCNT, (unsigned long) bw_cnt); 

    /*program available bandwidth reg-avaiable bw per frame in 16bytes data path
    Note:!!!! change 60 in denominator by video refresh_rate*/
    avail_bw_cnt = (double) bw / (16*60);
    hal_set_fpga_val32_k(hp, FPGA_AVAILABLE_BW, (unsigned long) avail_bw_cnt); 

    /* TBD set burst ratio and duration in fpga */
#endif
}

int hal_set_addside_low_compression(HAL *hp, int val)
{
    int mq = mquant_from_compression(val);
    int dcq = dcquant_from_compression(val);

    if (hp == NULL)
        return -1;
    if (hal_set_addside_mquant(hp, mq | 0x8000) == -1)
        return -1;
    if (hal_set_addside_dcquant(hp, dcq | 0x8000) == -1)
        return -1;
    return 0;
}

int hal_set_addside_high_compression(HAL *hp, int val)
{
    int mq = mquant_from_compression(val);
    int dcq = dcquant_from_compression(val);

    if (hp == NULL)
        return -1;
    if (hal_set_addside_mquant(hp, mq) == -1)
        return -1;
    if (hal_set_addside_dcquant(hp, dcq) == -1)
        return -1;

    return 0;
}

int hal_get_addside_low_compression(HAL *hp, int *val)
{
    int mq = 0x8000, dcq = 0x8000, rc;

    if (hp == NULL)
        return -1;

    if (hal_get_addside_mquant(hp, &mq) == -1)
        return -1;

    if (hal_get_addside_dcquant(hp, &dcq) == -1)
        return -1;

    if ((rc = compression_from_quant(mq, dcq)) < 0)
        *val = 7;
    else
        *val = rc;

    return 0;
}

int hal_get_addside_high_compression(HAL *hp, int *val)
{
    int mq = 0, dcq = 0, rc;

    if (hp == NULL)
        return -1;

    if (hal_get_addside_mquant(hp, &mq) == -1)
        return -1;

    if (hal_get_addside_dcquant(hp, &dcq) == -1)
        return -1;

    if ((rc = compression_from_quant(mq, dcq)) < 0)
        *val = 7;
    else
        *val = rc;

    return 0;
}

/* Returns 0 on XP if analog & DVI video outputs are disabled, else 1 */
int hal_get_video_output(HAL *hp) {

    int rc = 0;

    /* I50 does not support video output so it's always disabled */
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE)
        return 0;
#ifndef __XPI__
    if (hal_get_fpga_val_k(hp, FPGA_ATESTREG) & SYNCOFF_ENABLE) 
        return 0;
#else
    rc = (hal_getActiveIntf(hp) & INTERFACE_VIDEO_OUTPUT_ENABLE) ? 1 : 0;
#endif
    return rc;
}

/* Disable both analog & DVI video outputs if mode is 0, else enable */
int hal_set_video_output(HAL *hp, int mode) {
    int intf, hardware_type;

    if (hp == NULL)
        return 0;
    intf = hal_getActiveIntf(hp);
    hardware_type = hal_get_hardware_type(hp);
    if (hardware_type != PLX_DEVICE && hardware_type != XPI_DEVICE)
        return 0;

#ifndef __XPI__
    /************************************************************
    * This piece of code might be needed if we only need to    *
    * disable ths Digital Output: siyer 05/20/2010             *
    * hal_set_sii1178_1_val_k(hp, 0x08, (sii_master & 0xfe));  *
    * hal_set_sii1178_2_val_k(hp, 0x08, (sii_slave & 0xfe));   *
    * hal_and_in_fpga_val_k(hp, FPGA_ATESTREG, ~SYNCOFF_ENABLE);
    ************************************************************/
    if (!mode) {
        /* Disable output on SII */
        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);
        if (intf & INTERFACE_DUALLINK)
            hal_set_sii1178_2_val_k(hp, 0x08, 0x34);
        /* Disable vsync, hsync output from FPGA */
        hal_or_in_fpga_val_k(hp, FPGA_ATESTREG, SYNCOFF_ENABLE);
    }
    else {
        /* Enable output on SII */
        hal_set_sii1178_1_val_k(hp, 0x08, 0x35);
        if (intf & INTERFACE_DUALLINK)
            hal_set_sii1178_2_val_k(hp, 0x08, 0x35);
        /* Enable vsync, hsync output from FPGA */
        hal_and_in_fpga_val_k(hp, FPGA_ATESTREG, ~SYNCOFF_ENABLE);
        /* Reset video */
        if (hal_get_board_type(hp) == BOARDTYPE_TX) 
            hal_resetAddsideVideo(hp);
        else 
            hal_resetDropsideVideo(hp);
    }
    return 0;
#else
    if (mode) {
        return hal_setActiveIntf(hp, intf | INTERFACE_VIDEO_OUTPUT_ENABLE); 
    }
    else {
        return hal_setActiveIntf(hp, intf & ~INTERFACE_VIDEO_OUTPUT_ENABLE); 
    }
#endif
}

int hal_get_firmware_version(HAL *hp, char *buf, int maxlen)
{
    unsigned short fpga_reg;
    unsigned short pld_reg;

    if (hp == NULL)
        return -1;

    fpga_reg = hal_get_fpga_val_k(hp, FPGA_IDREG);
    if (hal_get_hardware_type(hp) == XPI_DEVICE) 
        pld_reg = 0;
    else 
        pld_reg = hal_get_fpga_val_k(hp, FPGA_CPLDIDREG);
    snprintf(buf, maxlen, "%d.%d-%d", (fpga_reg & 0xFF00) >> 8,
             fpga_reg & 0x00FF, pld_reg & 0x00FF);
    return (((int) pld_reg << 16) | fpga_reg);
}

int hal_get_firmware_date(HAL *hp, char *buf, int maxlen)
{
    unsigned long seconds;
    char tbuf[40];

    if (hp == NULL) 
        return 0;

    if (maxlen > sizeof(tbuf))
        maxlen = sizeof(tbuf);

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        hal_get_xpi_info(hp, tbuf, sizeof(tbuf));
	    strncpy(buf, tbuf, maxlen);
        buf[maxlen - 1] = 0;
        return (unsigned long) strlen(buf);
    }
    else { 
        seconds = hal_get_fpga_val_k(hp, FPGA_TIMESTAMP_U) << 16;
        seconds += hal_get_fpga_val_k(hp, FPGA_TIMESTAMP_L);
        snprintf(tbuf, sizeof(tbuf), "%s", ctime((time_t *) & seconds));
        /* rearrange string to make it look like software date */
        strncpy(buf, tbuf + 4, 7);
        strncpy(buf + 7, tbuf + 20, 4);
        strncpy(buf + 11, tbuf + 10, 9);
        *(buf + 20) = '\0';
        return strlen(buf);
    }
}

int hal_set_addside_noslicedrop(HAL *hp, int framecnt)
{
    int fd = hal_get_video_fd(hp);
    int dummyval = framecnt;

    if (hp == NULL)
        return -1;
    if (ioctl(fd, FPGA_IOC_SET_ASLICEDROP, &dummyval) < 0)
        return -1;

    return 0;
}

int hal_set_addside_nobpslice(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);

    if (hp == NULL)
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        int tmpval = (hal_get_fpga_val_k(hp, FPGA_AVHAREG) *
		hal_get_fpga_val_k(hp, FPGA_AVVAREG))/(64 * val);
        hal_set_fpga_val_k(hp, FPGA_ANOBREG, val);
        hal_set_fpga_val_k(hp, FPGA_ANSLICEREG, tmpval); 
    }
    else {
        if (ioctl(fd, FPGA_IOC_SET_ANOBPSLICE, &val) < 0)
            return -1;
    }
    return 0;
}

int hal_get_addside_nobpslice(HAL *hp, int *val)
{
    int fd = hal_get_video_fd(hp);

    if (hp == NULL)
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        *val = hal_get_fpga_val_k(hp, FPGA_ANOBREG);
    }
    else {
        if (ioctl(fd, FPGA_IOC_GET_ANOBPSLICE, val) < 0)
            return -1;
    }
    return 0;
}

int hal_get_addside_compmode(HAL *hp, int *compmode, int *motioncompmode)
{
    int val = 0;
    int fd = hal_get_video_fd(hp);

    if (hp == NULL)
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
#ifdef __XPI__
        int intf = hal_getActiveIntf(hp);
        if (intf & INTERFACE_COMPMODE_444)
            val = COMPMODE_444;
        if (intf & INTERFACE_COMPMODE_MOTION444)
            val = COMPMODE_MOTION_444;
#endif
    }
    else {
        if (ioctl(fd, FPGA_IOC_GET_ACOMPMODE, &val) < 0)
            return -1;
    }
    *compmode = (val == COMPMODE_444 || val == COMPMODE_MOTION_444) ? 1 : 0;
    *motioncompmode = (val == COMPMODE_MOTION_444) ? 1 : 0; 

    return 0;
}


int hal_get_dropside_compmode(HAL *hp, int *compmode, int *motioncompmode)
{
    int fd = hal_get_video_fd(hp);
    int val;

    if (hp == NULL) 
        return -1;
    if (hal_get_hardware_type(hp) == XPI_DEVICE) { 
#ifdef __XPI__
        int intf = hal_getActiveIntf(hp);
        if (intf & INTERFACE_COMPMODE_444)
            val = COMPMODE_444;
#endif
    }
    else {
        if (ioctl(fd, FPGA_IOC_GET_DCOMPMODE, &val) < 0)
            return -1;
    }
    *compmode = (val == COMPMODE_444) ? 1 : 0;
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE)
        *motioncompmode = 0;
    else
        *motioncompmode = *compmode; 

    return 0;
}

int hal_set_addside_compmode(HAL *hp, int compmode, int motioncompmode)
{
    int fd = hal_get_video_fd(hp);
    int val = COMPMODE_NONE;

    if (hp == NULL) 
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
#ifdef __XPI__
        val = hal_getActiveIntf(hp);
        val &= ~(INTERFACE_COMPMODE_MOTION444 | INTERFACE_COMPMODE_444);
        if (motioncompmode) 
            val |= INTERFACE_COMPMODE_MOTION444;
        else if (compmode) 
            val |= INTERFACE_COMPMODE_444;
        return hal_setActiveIntf(hp, val);
#endif
    }
    else {
        if (motioncompmode) 
            val = COMPMODE_MOTION_444;
        else if (compmode) 
            val = COMPMODE_444;
        if (ioctl(fd, FPGA_IOC_SET_ACOMPMODE, &val) < 0)
            return -1;
    }
    return 0;
}


int hal_set_dropside_compmode(HAL *hp, int compmode, int motioncompmode)
{
    int fd = hal_get_video_fd(hp);
    int val = COMPMODE_NONE, hardware_type = hal_get_hardware_type(hp);

    /* Dropside compression mode is handled by hald for XPs */
    if (hp == NULL || hardware_type == PLX_DEVICE)
       return 0;
    if (hardware_type == XPI_DEVICE) {
#ifdef __XPI__
        val = hal_getActiveIntf(hp);
        val &= ~(INTERFACE_COMPMODE_MOTION444 | INTERFACE_COMPMODE_444);
        if (compmode || motioncompmode)
            val |= INTERFACE_COMPMODE_444;
        return hal_setActiveIntf(hp, val);
#endif
    }
    else if (compmode || motioncompmode) {
        /* Motion444 behaves the same as 444 on I50 */
        val = COMPMODE_444;
        if (ioctl(fd, FPGA_IOC_SET_DCOMPMODE, &val) < 0)
            return -1;
    }
    return 0;
}

int hal_set_dropside_nobpslice(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE)
       return -1;
    if (ioctl(fd, FPGA_IOC_SET_DNOBPSLICE, &val) < 0)
        return -1;
    return 0;
}

int hal_get_dropside_nobpslice(HAL *hp, int *val)
{
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE)
       return -1;
    if (ioctl(fd, FPGA_IOC_GET_DNOBPSLICE, val) < 0)
        return -1;
    return 0;
}

int hal_set_ac_noise_threshold(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);

    if (hp == NULL)
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = (val << ACNOISE_SHIFT);
        hal_set_fpga_val_k(hp, FPGA_ACNOISET, sval);
    }
    else {
        if (ioctl(fd, FPGA_IOC_SET_ACNOISET, &val) < 0)
            return -1;
    }
    return 0;
}

int hal_get_ac_noise_threshold(HAL *hp, int *val)
{
    int fd = hal_get_video_fd(hp);
    int hardware_type = hal_get_hardware_type(hp);

    if (hp == NULL)
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = hal_get_fpga_val_k(hp, FPGA_ACNOISET);
        sval &= ACNOISE_MASK;
        *val = (sval >> ACNOISE_SHIFT);
    }
    else {
        if (ioctl(fd, FPGA_IOC_GET_ACNOISET, val) < 0)
            return -1;
    }
    return 0;
}

int hal_set_dc_noise_threshold(HAL *hp, int val)
{
    int fd = hal_get_video_fd(hp);

    if (hp == NULL)
        return -1;

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        unsigned short sval = (val >> DCNOISE_SHIFT);
        hal_set_fpga_val_k(hp, FPGA_DCNOISET, sval);
    }
    else {
        if (ioctl(fd, FPGA_IOC_SET_DCNOISET, &val) < 0)
            return -1;
    }
    return 0;
}

int hal_get_dc_noise_threshold(HAL *hp, int *val)
{
    int fd = hal_get_video_fd(hp);
    int hardware_type = hal_get_hardware_type(hp);

    if (hp == NULL)
        return -1;
    if (hardware_type == XPI_DEVICE) {
        unsigned short sval = hal_get_fpga_val_k(hp, FPGA_DCNOISET);
        sval &= DCNOISE_MASK;
        *val = (sval >> DCNOISE_SHIFT);
    }
    else {
        if (ioctl(fd, FPGA_IOC_GET_DCNOISET, val) < 0)
            return -1;
    }
    return 0;
}

int hal_get_horiz_polarity(HAL *hp, int *val) {
    int hardware_type = hal_get_hardware_type(hp);
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
    if (hardware_type == V2DI50_DEVICE) {
        int dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        *val = (dcreg & DCREG_HSYNC_POL) ? 1 : 0;
    }
    else if (hardware_type == PLX_DEVICE) {
        int polarity;
        if (ioctl(fd, FPGA_IOC_GET_DISPLAY_HPOL, &polarity) < 0)
            return -1; 
        else
            *val = polarity;
    }
    return 0;
}

int hal_set_horiz_polarity(HAL *hp, int val) {
    int hardware_type = hal_get_hardware_type(hp);

    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
    if (hardware_type == V2DI50_DEVICE) {
        int dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        if (!val) 
            dcreg &= ~(DCREG_HSYNC_POL);
        else 
            dcreg |= (DCREG_HSYNC_POL);
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
    }
    else if (hardware_type == PLX_DEVICE) {
        VLOG_INFO_CAT(VLOG_HAL, 
                "Channel %d. Horizontal Polarity can only be set by HALD", 
                hal_get_channel_number(hp));
    }

    return 0;
}

int hal_get_vert_polarity(HAL *hp, int *val) {
    int hardware_type = hal_get_hardware_type(hp);
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
    if (hardware_type == V2DI50_DEVICE) {
        int dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        *val = (dcreg & DCREG_VSYNC_POL) ? 1 : 0;
    }
    else if (hardware_type == PLX_DEVICE) {
        int polarity;
        if (ioctl(fd, FPGA_IOC_GET_DISPLAY_VPOL, &polarity) < 0)
            return -1; 
        else
            *val = polarity;
    }
    return 0;
}

int hal_set_vert_polarity(HAL *hp, int val) {
    int hardware_type = hal_get_hardware_type(hp);

    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
    if (hardware_type == V2DI50_DEVICE) {
        int dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        if (!val) 
            dcreg &= ~(DCREG_VSYNC_POL);
        else 
            dcreg |= (DCREG_VSYNC_POL);
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
    }
    else if (hardware_type == PLX_DEVICE) {
        VLOG_INFO_CAT(VLOG_HAL, 
                "Channel %d. Vertical Polarity can only be set by HALD", 
                hal_get_channel_number(hp));
    }

    return 0;
}

int hal_get_pc_params(HAL *hp, struct pc_param_io *pcpiop)
{
    if (hp == NULL)
        return -1;
    if (ioctl(hp->kernel_fd, FPGA_IOC_PC_PARAM_GET, pcpiop) < 0)
        return -1;
    return 0;
}

int hal_set_pc_params(HAL *hp, struct pc_param_io *pcpiop)
{
    if (hp == NULL)
        return -1;
    if (ioctl(hp->kernel_fd, FPGA_IOC_PC_PARAM_SET, pcpiop) < 0)
        return -1;
    return 0;
}

int hal_set_log_level(HAL * hp, int val)
{
    int fd = hal_get_video_fd(hp);
    if (ioctl(fd, FPGA_IOC_SET_LOGLVL, val) < 0)
        return -1;
    return 0;
}

int hal_set_overlay_params(HAL*hp, struct ov_param_io *ov) {
    int fd = hal_get_video_fd(hp);
    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE) 
        return -1;
    if (ioctl(fd, FPGA_IOC_SET_OV_PARAM, ov) < 0) 
        return -1;
    return 0;
}

int hal_get_overlay_params(HAL *hp, struct ov_param_io *ov) {
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE)
        return -1;
    if (ioctl(fd, FPGA_IOC_GET_OV_PARAM, ov) < 0) 
        return -1;
    return 0;
}

int hal_get_overlay_info(HAL *hp, struct ov_param_io *oi) {
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE)
        return -1;
    if (ioctl(fd, FPGA_IOC_GET_OV_INFO, oi) < 0) 
        return -1;
    return 0;
}

int hal_get_overlay_mode(HAL *hp) {
    struct ov_param_io ov;
    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE)
        return -1;
    if (hal_get_overlay_params(hp, &ov) < 0)
        return -1;
    return ov.flags;
}

int hal_enable_cropping(HAL *hp, 
                        int hleft,
                        int hright, 
                        int vtop,
                        int vbottom) {
    int fd = hal_get_video_fd(hp);
    struct ov_param_io ovpio;

    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE ||
                    hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;
     
    if (fd == -1)
        return -1;

    if (ioctl(fd, FPGA_IOC_GET_OV_PARAM, &ovpio) < 0)  
        return -1;

    ovpio.setmask = OV_SET_PARAM_CROP;
    ovpio.flags |= OV_FLAG_CROP;
    ovpio.hcropleft = hleft;
    ovpio.hcropright = hright;
    ovpio.vcroptop = vtop;
    ovpio.vcropbottom = vbottom;

    if (ioctl(fd, FPGA_IOC_SET_OV_PARAM, &ovpio) < 0)
        return -1;

    return 0;
}

int hal_disable_cropping(HAL *hp) {
    int fd = hal_get_video_fd(hp);
    struct ov_param_io ovpio;

    if (hp == NULL || hal_get_hardware_type(hp) != V2DI50_DEVICE ||
                    hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;

    if (fd == -1)
        return -1;

    if (ioctl(fd, FPGA_IOC_GET_OV_PARAM, &ovpio) < 0)
        return -1;

    ovpio.setmask = OV_SET_PARAM_CROP;
    ovpio.flags &= ~OV_FLAG_CROP;

    if (ioctl(fd, FPGA_IOC_SET_OV_PARAM, &ovpio) < 0)
        return -1;

    return 0;
}

int hal_get_cropping_params(HAL *hp, 
                            int *hleft, 
                            int *hright, 
                            int *vtop, 
                            int *vbottom) {
    struct ov_param_io ovpio;
    int fd = hal_get_video_fd(hp);

    *hleft = *hright = *vtop = *vbottom = 0;

    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE ||
                    hal_get_board_type(hp) != BOARDTYPE_RX)
        return -1;

    if (ioctl(fd, FPGA_IOC_GET_OV_PARAM, &ovpio) < 0) {
        return -1;
    }
    if (ovpio.flags & OV_FLAG_CROP) {
        *hleft = ovpio.hcropleft;
        *hright = ovpio.hcropright;
        *vtop = ovpio.vcroptop;
        *vbottom = ovpio.vcropbottom;
    }
    return 0;
}

int hal_set_framecnt_enable(HAL *hp, int val) {
    int hardware_type;
    int fd = hal_get_video_fd(hp);

    if (hp == NULL)
        return -1;

    if ((hardware_type = hal_get_hardware_type(hp)) == XPI_DEVICE) {
        if (val) 
            hal_or_in_fpga_val_k(hp, FPGA_ATESTREG, FRAMECNT_ENABLE);
        else
            hal_and_in_fpga_val_k(hp, FPGA_ATESTREG, ~FRAMECNT_ENABLE);
    }
    else if (hardware_type == PLX_DEVICE) {
        if (ioctl(fd, FPGA_IOC_FRAMECNT_ENABLE, &val) < 0) 
            return -1;
    }
    return 0;
}

int hal_set_progressive_compression(HAL *hp, int val) {
    int fd = hal_get_video_fd(hp);

    if (hp == NULL || hal_get_hardware_type(hp) == XPI_DEVICE)
        return -1;
    if (ioctl(fd, FPGA_IOC_PROGCMPRS_ENABLE, &val) < 0) 
        return -1;
    return 0;
}

int hal_set_iframe_interval(HAL *hp, unsigned long interval) {

    int hardware_type, clkfreq;

    if (hp == NULL)
        return -1;

    if ((hardware_type = hal_get_hardware_type(hp)) == V2DI50_DEVICE)
        return -1;

    clkfreq = (hardware_type == XPI_DEVICE) ? XPI_CLOCK_FREQ : V4_CLOCK_FREQ;

    /* Reset FPGA internal iframe counter */
    hal_or_in_fpga_val_k(hp, FPGA_IFRM_RESET, IFRM_RESET_CLKG);
    usleep(1000);

    if (interval == IFRAME_INTERVAL_NONE) {
        /* De-select clkg based count */
        hal_and_in_fpga_val_k(hp, FPGA_IFRM_SELECT, ~IFRM_SELECT_CLKG);
    }
    else {
        /* Set iframe clkg count */
        hal_set_fpga_val32_k(hp, FPGA_IFRM_CLKGCNT, 
                interval * (unsigned long) (clkfreq / 1000));
        /* Select clkg based count */
        hal_or_in_fpga_val_k(hp, FPGA_IFRM_SELECT, IFRM_SELECT_CLKG);
    }
    hal_and_in_fpga_val_k(hp, FPGA_IFRM_RESET, ~IFRM_RESET_CLKG);
    return 0;
}

int hal_set_dvimode_auto(HAL *hp, int mode_auto) {
    int avflag;

    if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE)
        return -1;
    avflag = hal_getAudioVideoFlag(hp);
    if (((avflag & AVFLAG_DVIAUTO) && !mode_auto) ||
            (!(avflag & AVFLAG_DVIAUTO) && mode_auto)) {
        if (mode_auto) avflag |= AVFLAG_DVIAUTO;
        else avflag &= ~AVFLAG_DVIAUTO;
        hal_setAudioVideoFlag(hp, avflag);
        hal_resetAddsideVideo(hp);
    }
    return 0;
}

int hal_set_optimize_latency(HAL *hp, int optimize_latency) {
    int avflag;

    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE)
        return -1;
    avflag = hal_getAudioVideoFlag(hp);
    if (((avflag & AVFLAG_LATENCY) && !optimize_latency) ||
            (!(avflag & AVFLAG_LATENCY) && optimize_latency)) {
        if (optimize_latency) avflag |= AVFLAG_LATENCY;
        else avflag &= ~AVFLAG_LATENCY;
        hal_setAudioVideoFlag(hp, avflag);
    }
    return 0;
}

int hal_get_fpga_type(HAL *hp) {
#ifndef __XPI__
    char str[80];
    int serial = -1;

    if (hp == NULL)
        return -1;

    hal_get_board_serial_number(hp, str, sizeof str);
    sscanf(str, "IPV%d", &serial);

    return ((serial >= LX100_SERIAL_MIN && serial <= LX100_SERIAL_MAX) ? FPGA_TYPE_LX100
        : ((serial >= LX80_SERIAL_MIN && serial <= LX80_SERIAL_MAX) ? FPGA_TYPE_LX80
            : FPGA_TYPE_UNKNOWN));
#else
    return FPGA_TYPE_UNKNOWN;
#endif
}

int hal_get_codec_version(HAL *hp) {
    int val;
    int fd = hal_get_video_fd(hp);
   
    if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE)
        return CODEC_VERSION_STANDARD;

    if (ioctl(fd, FPGA_IOC_GET_CODEC_VERSION, &val) < 0) 
        return  UNKNOWN_CODEC_VERSION;   /* unknown codec version */

    return val;
}

int hal_configure_rx_codec_version(HAL *hp, int codecversion) {
    FILE *ffile;
    char cmdbuf[256];

    sprintf(cmdbuf, "/tmp/reload_fpga_%d", hal_get_channel_number(hp));

    if ((ffile = fopen(cmdbuf, "wb")) != NULL) {
        fwrite(&codecversion, 1, sizeof(int), ffile);
        fclose(ffile);
    }

    return 1;
}

int hal_get_configured_rx_codec_version(HAL *hp) {

    FILE *ffile;
    char cmdbuf[256];
    int codecversion = -1;

    sprintf(cmdbuf, "/tmp/reload_fpga_%d", hal_get_channel_number(hp));

    if((ffile = fopen(cmdbuf, "rb")) !=NULL) {
        fread(&codecversion, 1, sizeof(int), ffile);
        fclose(ffile);
    }

    return codecversion;
}

int hal_get_configured_license(HAL *hp)
{
    if (hp == NULL || hal_get_hardware_type(hp) != XPI_DEVICE)
        return NO_LICENSE;
    else {
#ifdef __XPI__
        int fd = hal_get_video_fd(hp);
        unsigned int flags;

        if (ioctl(fd, FPGA_IOC_GET_FPGA_FLAGS, &flags) < 0) 
            return  -1;
        if (flags & FLAG_XPI_VIDPROC_STEREO)
            return PRO3D_LICENSE;
        else if (flags & FLAG_XPI_VIDPROC_DUALLINK)
            return PRO_LICENSE;
        else if (flags & FLAG_XPI_VIDPROC_BASIC)
            return LITE_LICENSE;
#endif
    }
    return NO_LICENSE;
}
