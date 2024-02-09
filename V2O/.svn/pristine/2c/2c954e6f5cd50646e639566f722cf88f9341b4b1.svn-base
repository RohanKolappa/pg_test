#include <stdio.h>
#include <sys/ioctl.h>
                                                                                
#include "plx9056pciif.h"
#include "hal.h"
#include "hal_vpd.h"
#include "vlog.h"
#include "commonutils.h"
#include "vlib.h"

int hal_plx_vpd_read_k(HAL *hp, unsigned long offset, unsigned long *value) {
    int rc = -1;
#ifndef __XPI__
    struct reg_io regio;
    int fd;

    regio.type = REGIO_TYPE_VPD_READ;
    regio.reg = offset;
    if ((fd = hp->kernel_fd) != -1) {
        if (ioctl(fd, FPGA_IOC_MOD_PLX, &regio) != -1) {
            *value = regio.value;
            rc = 0;
        }
    }
#endif
    return rc;
}

int hal_plx_vpd_write_k(HAL *hp, unsigned long offset, unsigned long value) {

    int rc = -1;
#ifndef __XPI__
    struct reg_io regio;
    int fd;

    regio.type = REGIO_TYPE_VPD_WRITE;
    regio.reg = offset;
    regio.value = value;
    if ((fd = hp->kernel_fd) != -1) {
        if (ioctl(fd, FPGA_IOC_MOD_PLX, &regio) != -1) {
            rc = 0;
        }
    }
#endif
    return rc;
}

/***************************************************************************
 * Routines to read and write PLX VPD: The VPD is a 28-byte field starting *
 * with a 4 byte date field followed by a 16 type serial number, followed  *
 * by a 4 byte hardware revision.                                          *
 ***************************************************************************/
#define VPD_ERR_NCP_DISABLE     -1
#define VPD_ERR_NCP_INVALID     -2
#define VPD_ERR_VPD_READ        -3
#define VPD_ERR_ID_INVALID      -4

static int check_vpd(HAL *hp) {
    unsigned long regsave, regval;
    int capability = 0, fd;

    if (hal_get_hardware_type(hp) != PLX_DEVICE)
        return 0;
#ifndef __XPI__
    regsave = hal_get_plx_config_val_k(hp, PCI9056_VPD_CAP_ID);

    if ((fd = hp->kernel_fd) == -1) 
        return -1;

    ioctl(fd, FPGA_IOC_GET_PLXCAP, &capability);

    if (!(capability & PCI9056_CAPABILITY_VPD)) {
        VLOG_INFO_CAT(VLOG_VLIB, "bad capability = 0x%lx (should be 0x%x).", 
            regval, PCI9056_CAPABILITY_VPD);
        return VPD_ERR_NCP_DISABLE;
    }

    regval = hal_get_plx_config_val_k(hp, PCI9056_VPD_CAP_ID);
    if (((regval >> 8) & 0xff) != ((regsave >> 8) & 0xff)) {
        VLOG_INFO_CAT(VLOG_VLIB, "bad NCP. regval = 0x%lx (should be 0x%lx).", 
            regval, regsave);
        return VPD_ERR_NCP_INVALID;
    }

    if (hal_plx_vpd_read_k(hp, 0, &regval) < 0)
        return VPD_ERR_VPD_READ;

    if (regval != PCI9056_ID) {
        VLOG_INFO_CAT(VLOG_VLIB, "bad ID. regval = 0x%lx (should be 0x%x).", 
            regval, PCI9056_ID);
        return VPD_ERR_ID_INVALID;
    }
#endif
    return 0;
}

int hal_get_board_serial_number(HAL *hp, char *buf, int maxlen) {
#ifndef __XPI__ 
    unsigned short writeprotect;
    int i, rc;
    unsigned long tmpval;

    /***************
     * Skip if I50 *
     ***************/
    if (hal_get_hardware_type(hp) != PLX_DEVICE) 
        return 0;

    if ((rc = check_vpd(hp)) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d checkvpd error %d\n", 
                hal_get_board_number(hp) + 1, rc);
        return -1;
    }

    writeprotect = ((hal_get_plx_val_k(hp, PCI9056_ENDIAN_DESC)>>16)&0xff)*4;                                                                                
    for (i = 0; i < VPD_SERIAL_SIZE / 4; i++) {
        if (hal_plx_vpd_read_k(hp, writeprotect + 4 * i + 4, &tmpval) < 0) 
            return -1;
        buf[i*4] = (tmpval & 0xff000000) >> 24;
        buf[i*4 + 1] = (tmpval & 0x00ff0000) >> 16;
        buf[i*4 + 2] = (tmpval & 0x0000ff00) >> 8;
        buf[i*4 + 3] = (tmpval & 0x000000ff);
    }
#endif
    return 0;
}

unsigned long hal_get_board_vpd_date(HAL *hp) {
    unsigned long mdate = 0;
#ifndef __XPI__ 
    unsigned short writeprotect;
    int rc;

    if (hal_get_hardware_type(hp) != PLX_DEVICE) 
        return 0;

    if ((rc = check_vpd(hp)) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d ceckvpd error %d\n", 
                hal_get_board_number(hp) + 1, rc);
        return 0;
    }

    writeprotect = ((hal_get_plx_val_k(hp, PCI9056_ENDIAN_DESC)>>16)&0xff)*4;                                                                                
    if (hal_plx_vpd_read_k(hp, writeprotect, &mdate) < 0) {
        return 0;
    }
#endif
    return mdate;
}

int hal_get_board_manufacture_date(HAL *hp, char *buf, int maxlen) {
#ifndef __XPI__ 
    unsigned long mdate;
    int yy, mm, dd;
    static char *mon_str[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    if ((mdate = hal_get_board_vpd_date(hp)) == 0) 
        return -1;

    yy = (int) VPD_YEAR(mdate) + 2000;
    mm = (int) VPD_MONTH(mdate);
    dd = (int) VPD_DAY(mdate);
    if (mm > 11) mm = 11;
    snprintf(buf, maxlen, "%d %s %d", dd, mon_str[mm], yy);
#endif
    return 0;
}

int hal_get_board_revision(HAL *hp, char *buf, int maxlen) {
#ifndef __XPI__ 
    unsigned short writeprotect;
    unsigned long revision;
    int rc;

    if ((rc = check_vpd(hp)) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d check_vpd error %d\n", 
                hal_get_board_number(hp) + 1, rc);
        return -1;
    }

    writeprotect = ((hal_get_plx_val_k(hp, PCI9056_ENDIAN_DESC)>>16)&0xff)*4;                                                                                
    if (hal_plx_vpd_read_k(hp, writeprotect+VPD_DATE_SIZE+VPD_SERIAL_SIZE, 
                        &revision) < 0) {
        return -1;
    }
    snprintf(buf, maxlen, "%d.%d", (int) ((revision & 0xffff0000) >> 16),
            (int) (revision & 0x0000ffff));
#endif
    return 0;
}

int hal_get_board_vpd(HAL *hp, 
                      unsigned long *mdate, 
                      unsigned long *serial,
                      unsigned long *revision) 
{
#ifndef __XPI__ 
    unsigned short writeprotect;
    int i, rc, cno = hal_get_board_number(hp) + 1;

    if (hal_get_hardware_type(hp) != PLX_DEVICE) 
        return 0;

    if ((rc = check_vpd(hp)) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d checkvpd error %d\n", cno, rc);
        return -1;
    }

    writeprotect = ((hal_get_plx_val_k(hp, PCI9056_ENDIAN_DESC)>>16)&0xff)*4;                                                                                
    if (hal_plx_vpd_read_k(hp, writeprotect, mdate) < 0) {
        return -1;
    }
    for (i = 0; i < VPD_SERIAL_SIZE / 4; i++) {
        if (hal_plx_vpd_read_k(hp, writeprotect + 4 * i + 4, &serial[i]) < 0) 
            return -1;
    }
    if (hal_plx_vpd_read_k(hp, writeprotect+VPD_DATE_SIZE+VPD_SERIAL_SIZE, 
                        revision) < 0) {
        return -1;
    }
#endif
    return 0;
}

int hal_set_board_vpd(HAL *hp, 
                      unsigned long mdate, 
                      unsigned long *serial,
                      unsigned long revision) 
{
#ifndef __XPI__ 
    unsigned short writeprotect;
    int i, rc, cno = hal_get_board_number(hp) + 1;

    /***************
     * Skip if I50 *
     ***************/
    if (hal_get_hardware_type(hp) != PLX_DEVICE) 
        return 0;

    /********************
     * Check the PLX VPD *
     ********************/
    if ((rc = check_vpd(hp)) < 0) { 
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d checkvpd error %d\n", cno, rc);
        return -1;
    }

    writeprotect = ((hal_get_plx_val_k(hp, PCI9056_ENDIAN_DESC)>>16)&0xff)*4;

    if (hal_plx_vpd_write_k(hp, writeprotect, mdate) < 0) {
        return -1;
    }
    for (i = 0; i < VPD_SERIAL_SIZE / 4; i++) {
        if (hal_plx_vpd_write_k(hp, writeprotect+VPD_DATE_SIZE + 4 * i,
                    serial[i]) < 0)
            return -1;
    }
    if (hal_plx_vpd_write_k(hp, writeprotect+VPD_DATE_SIZE+VPD_SERIAL_SIZE, 
                    revision) < 0) {
        return -1;
    }
#endif
    return 0;
}
