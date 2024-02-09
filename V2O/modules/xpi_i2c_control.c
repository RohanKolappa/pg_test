#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <syslog.h>

#include "hal.h"
#include "i2c-dev.h"
#include "vlog.h"
#include "xpi_i2c_control.h"

int xpi_get_main_i2c_val(HAL *hp,
                         unsigned char port,
                         unsigned char addr, 
                         unsigned short reg,
                         unsigned char *val) 
{
    int rc = -1;

#ifdef __XPI__
    struct reg_io regio;
    int fd = hal_get_video_fd(hp);

    if (fd < 0) { 
        VLOG_INFO("%s: failed to open /dev/video0 as a I2C device.", __func__);
        return rc;
    }

    if (addr == ADV7604_I2C_ADDR && port == ADV7604_I2C_PORT)
        regio.type = REGIO_TYPE_ADV7604_GET;
    else if (addr == ADV7612_I2C_ADDR && port == ADV7612_I2C_PORT)
        regio.type = REGIO_TYPE_ADV7612_GET;
    else if (addr == SII7172_I2C_ADDR && port == SII7172_I2C_PORT)
        regio.type = REGIO_TYPE_SII7172_GET;
    else if (addr == SI5338_I2C_ADDR && port == SI5338_I2C_PORT)
        regio.type = REGIO_TYPE_SI5338_GET;
    else if (addr == LM63_I2C_ADDR && port == LM63_I2C_PORT)
        regio.type = REGIO_TYPE_LM63_GET;
    else if (addr == ICS9FG104_I2C_ADDR && port == ICS9FG104_I2C_PORT)
        regio.type = REGIO_TYPE_ICS9FG104_GET;
    else if (addr == MAX6650_I2C_ADDR && port == MAX6650_I2C_PORT)
        regio.type = REGIO_TYPE_MAX6650_GET;
    else if (addr == EXPANSIONSWITCH_I2C_ADDR && 
                          port == EXPANSIONSWITCH_I2C_PORT)
        regio.type = REGIO_TYPE_EXPANSIONSWITCH_GET;
    else {
        VLOG_INFO("%s: invalid addr 0x%x", __func__, addr);
        return rc;
    }

    regio.reg = reg;
    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) {
        VLOG_INFO("%s: FPGA_IOC_MOD_REG error %d.", __func__, rc);
    }
    else 
        *val = regio.value;
#endif
    return rc;
}

int xpi_set_main_i2c_val(HAL *hp,
                         unsigned char port,
                         unsigned char addr, 
                         unsigned short reg, 
                         unsigned char val) 
{
    int rc = -1;

#ifdef __XPI__
    struct reg_io regio;
    int fd = hal_get_video_fd(hp);

    if (fd < 0) { 
        VLOG_INFO("%s: failed to open /dev/video0 as a I2C device.", __func__);
        return rc;
    }

    if (addr == ADV7604_I2C_ADDR && port == ADV7604_I2C_PORT)
        regio.type = REGIO_TYPE_ADV7604_SET;
    else if (addr == ADV7612_I2C_ADDR && port == ADV7612_I2C_PORT)
        regio.type = REGIO_TYPE_ADV7612_SET;
    else if (addr == SII7172_I2C_ADDR && port == SII7172_I2C_PORT)
        regio.type = REGIO_TYPE_SII7172_SET;
    else if (addr == SI5338_I2C_ADDR && port == SI5338_I2C_PORT)
        regio.type = REGIO_TYPE_SI5338_SET;
    else if (addr == LM63_I2C_ADDR && port == LM63_I2C_PORT)
        regio.type = REGIO_TYPE_LM63_SET;
    else if (addr == ICS9FG104_I2C_ADDR && port == ICS9FG104_I2C_PORT)
        regio.type = REGIO_TYPE_ICS9FG104_SET;
    else if (addr == MAX6650_I2C_ADDR && port == MAX6650_I2C_PORT)
        regio.type = REGIO_TYPE_MAX6650_SET;
    else if (addr == EXPANSIONSWITCH_I2C_ADDR && 
                      port == EXPANSIONSWITCH_I2C_PORT)
        regio.type = REGIO_TYPE_EXPANSIONSWITCH_SET;
    else {
        VLOG_INFO("%s: invalid addr 0x%x", __func__, addr);
        return rc;
    }

    regio.reg = reg;
    regio.value = val;
    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) 
        VLOG_INFO("%s: FPGA_IOC_MOD_REG error %d.", __func__, rc);
#endif
    return rc;
}

int xpi_get_expansion_i2c_val(HAL *hp,
                              unsigned char port,
                              unsigned char addr, 
                              unsigned short reg, 
                              unsigned char *val)
{
    int rc = -1;
#ifdef __XPI__
    struct expansion_io expio;
    int fd = hal_get_video_fd(hp);

    if (fd < 0) { 
        VLOG_INFO("%s: failed to open /dev/video0 as a I2C device.", 
                __func__);
        return rc;
    }
    expio.type = EXPANSIONIO_TYPE_GET;
    expio.port = port;
    expio.addr = addr;
    expio.reg = reg;
    if ((rc = ioctl(fd, FPGA_IOC_MOD_EXPANSION, &expio)) < 0) 
        VLOG_INFO("%s: FPGA_IOC_MOD_REG error %d.", __func__, rc);
    else 
        *val = expio.value;
#endif
    return rc;
}

int xpi_set_expansion_i2c_val(HAL *hp,
                              unsigned char port,
                              unsigned char addr, 
                              unsigned short reg, 
                              unsigned char val)
{
    int rc = -1;
#ifdef __XPI__
    int fd = hal_get_video_fd(hp);
    struct expansion_io expio;

    if (fd < 0) { 
        VLOG_INFO("%s: failed to open /dev/video0 as a I2C device.", 
                __func__);
        return rc;
    }
    expio.type = EXPANSIONIO_TYPE_SET;
    expio.port = port;
    expio.addr = addr;
    expio.reg = reg;
    expio.value = val;
    
    if ((rc = ioctl(fd, FPGA_IOC_MOD_EXPANSION, &expio)) < 0)
        VLOG_INFO("%s: FPGA_IOC_MOD_REG error %d.", __func__, rc);
#endif
    return  rc;
}

int xpi_get_monedid_val(HAL *hp,
                        unsigned short reg,
                        unsigned char *val) 
{
    int rc = -1;
#ifdef __XPI__
    struct reg_io regio;
    int fd = hal_get_video_fd(hp);

    if (fd < 0) { 
        VLOG_INFO("%s: failed to open /dev/video0 as a I2C device.", 
                __func__);
        return rc;
    }
    regio.type = REGIO_TYPE_MONEDID_GET;
    regio.reg = reg;
    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) {
        VLOG_INFO("%s: FPGA_IOC_MOD_REG error %d.", __func__, rc);
    }
    else 
        *val = regio.value;
#endif
    return rc;
}

int xpi_set_monedid_val(HAL *hp,
                        unsigned short reg, 
                        unsigned char val) {
    int rc = -1;
#ifdef __XPI__
    struct reg_io regio;
    int fd = hal_get_video_fd(hp);

    if (fd < 0) { 
        VLOG_INFO("%s: failed to open /dev/video0 as a I2C device.", 
                __func__);
        return rc;
    }

    regio.type = REGIO_TYPE_MONEDID_SET;
    regio.reg = reg;
    regio.value = val;
    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) 
        VLOG_INFO("%s: FPGA_IOC_MOD_REG error %d.", __func__, rc);
#endif
    return rc;
}

int xpi_read_mon_edid(HAL *hp, unsigned char *buf, int bufsize) 
{
    int rc = -1;
#ifdef __XPI__
    int fd;
    struct edid_io edidio;

    if (hp == NULL || (fd = hal_get_video_fd(hp)) < 0)
        return -1;
    if ((rc = ioctl(fd, FPGA_IOC_GET_MONEDID, &edidio)) < 0) {
        VLOG_INFO("%s: Failed to communicate with device.", __func__);
    }
    else {
        memcpy(buf, edidio.edid, (bufsize > 256) ? 256 : bufsize);
    }
#endif
    return rc;
    
}

int xpi_read_edid_eeprom(HAL *hp, int subdev, unsigned char *buf, int bufsize)
{
    int rc = -1;
#ifdef __XPI__
    int fd;
    struct edid_io edidio;

    if (hp == NULL || (fd = hal_get_video_fd(hp)) < 0)
        return -1;
    edidio.subdev = subdev;
    if ((rc = ioctl(fd, FPGA_IOC_SUBDEV_GET_EDID, &edidio)) < 0) {
        VLOG_INFO("%s: Failed to communicate with device.", __func__);
    }
    else {
        memcpy(buf, edidio.edid, (bufsize > 256) ? 256 : bufsize);
    }
#endif
    return rc;
}

int xpi_load_edid_eeprom(HAL *hp, int subdev, unsigned char *buf, int bufsize)
{
    int rc = -1;
#ifdef __XPI__
    int fd;
    struct edid_io edidio;

    if (hp == NULL || (fd = hal_get_video_fd(hp)) < 0)
        return -1;
    edidio.subdev = subdev;
    memcpy(edidio.edid, buf, (bufsize > 256) ? 256 : bufsize);
    if ((rc = ioctl(fd, FPGA_IOC_SUBDEV_SET_EDID, &edidio)) < 0) {
        VLOG_INFO("%s: Failed to communicate with device.", __func__);
    }
#endif
    return rc;
}
