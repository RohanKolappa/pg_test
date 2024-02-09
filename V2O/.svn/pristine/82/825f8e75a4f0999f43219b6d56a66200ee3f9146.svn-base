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
                                                                                
#include "plx9056pciif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_LED.h"
#include "vlog.h"
#include "commonutils.h"
#include "xp_i2c_control.h"
#include "vlib.h"
#include "tx_kbm_config.h"
#include "rx_kbm_config.h"
#include "led_control.h"
#include "device_detection.h"
#include "display_config.h"

void hal_set_bar2_val_k(HAL *hp, unsigned long reg,  unsigned long val) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;
    
    regio.type = REGIO_TYPE_BAR2_SET;
    regio.reg = reg;
    regio.value = val;
    if ((fd = hp->kernel_fd) != -1) {
        ioctl(fd, FPGA_IOC_MOD_REG, &regio);
    }
#endif
}

unsigned long hal_get_bar2_val_k(HAL *hp, unsigned long reg) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_BAR2_GET;
    regio.reg = reg;

    if ((fd = hp->kernel_fd) == -1) {
        return 0;
    }
    if (ioctl(fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return regio.value;
#else
    return 0;
#endif
}

void hal_set_bar3_val_k(HAL *hp, unsigned long reg,  unsigned long val) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;
    
    regio.type = REGIO_TYPE_BAR3_SET;
    regio.reg = reg;
    regio.value = val;
    if ((fd = hp->kernel_fd) != -1) {
        ioctl(fd, FPGA_IOC_MOD_REG, &regio);
    }
#endif
}

unsigned long hal_get_bar3_val_k(HAL *hp, unsigned long reg) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_BAR3_GET;
    regio.reg = reg;

    if ((fd = hp->kernel_fd) == -1) {
        return 0;
    }
    if (ioctl(fd, FPGA_IOC_MOD_REG, &regio) < 0)
        return 0;
    return regio.value;
#else
    return 0;
#endif
}

void hal_set_plx_config_val_k(HAL *hp, unsigned short reg, unsigned long value) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_PCICONFIG_SET;
    regio.reg = reg;
    regio.value = value;
    if ((fd = hp->kernel_fd) != -1) {
        ioctl(fd, FPGA_IOC_MOD_PLX, &regio);
    }
#endif
}

unsigned long hal_get_plx_config_val_k(HAL *hp, unsigned short reg) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_PCICONFIG_GET;
    regio.reg = reg;
    if ((fd = hp->kernel_fd) == -1)
        return 0;
    if (ioctl(fd, FPGA_IOC_MOD_PLX, &regio) < 0)
        return 0;
    return regio.value;
#else
    return 0;
#endif
}

void hal_set_plx_val_k(HAL *hp, unsigned short reg,  unsigned long val) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;
    
    regio.type = REGIO_TYPE_PLX_SET;
    regio.reg = reg;
    regio.value = val;
    if ((fd = hp->kernel_fd) != -1) {
        ioctl(fd, FPGA_IOC_MOD_PLX, &regio);
    }
#endif
}

unsigned long hal_get_plx_val_k(HAL *hp, unsigned short reg) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_PLX_GET;
    regio.reg = reg;
    if ((fd = hp->kernel_fd) == -1)
        return 0;
    if (ioctl(fd, FPGA_IOC_MOD_PLX, &regio) < 0)
        return 0;

    return (unsigned long) regio.value;
#else
    return 0;
#endif
}

int hal_plx_eeprom_present(HAL *hp) {
#ifndef __XPI__
    if (hal_get_plx_val_k(hp, PCI9056_EEPROM_CTRL_STAT) & (1 << 28))
        return 1;
#endif
    return 0; 
}

void hal_set_plx_eeprom_val_k(HAL *hp, unsigned short reg, unsigned long value) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_EEPROM_SET;
    regio.reg = reg;
    regio.value = value;
    if ((fd = hp->kernel_fd) != -1)
        ioctl(fd, FPGA_IOC_MOD_PLX, &regio);
#endif
}

unsigned long hal_get_plx_eeprom_val_k(HAL *hp, unsigned short reg) {
#ifndef __XPI__
    int fd;
    struct reg_io regio;

    regio.type = REGIO_TYPE_EEPROM_GET;
    regio.reg = reg;
    if ((fd = hp->kernel_fd) == -1)
        return (unsigned long) 0;
    if (ioctl(fd, FPGA_IOC_MOD_PLX, &regio) < 0)
        return (unsigned long) 0;
    return regio.value;
#else
    return 0;
#endif
}

int hal_set_driver_enable(HAL *hp, int mode) {
#ifndef __XPI__
    int fd;
    if ((fd = hp->kernel_fd) == -1)
        return -1;
    if (ioctl(fd, FPGA_IOC_SET_DRIVERENABLE, &mode) < 0)
        return -1;
    return mode;
#else
    return 0;
#endif
}

int hal_get_board_status(HAL *hp) {
#ifndef __XPI__
    int fd, status;
    if ((fd = hp->kernel_fd) != -1) {
        if (ioctl(fd, FPGA_IOC_BOARD_STATUS, &status) == 0)
            return status;
    }
#endif
    return -1;
}

void hal_soft_reset_fpga(HAL *hp) {
#ifndef __XPI__
    int fd, dummyval = 0;
    if ((fd = hp->kernel_fd) == -1)
        return;
    if (ioctl(fd, FPGA_IOC_RESET_FPGA, &dummyval) < 0)
        return;
#endif
}

int hal_activate_board(HAL *hp) {
#ifndef __XPI__
    int fd, board_type;
    unsigned short val;

    /* Soft reset FPGA */
    hal_soft_reset_fpga(hp);

    /* Initialze FPGA driver and FPGA registers for the board */
    if ((fd = hp->kernel_fd) != -1) {
        if (ioctl(fd, FPGA_IOC_BOARD_INIT, NULL) < 0)
            return -1;
    }
    else
        return -1;

    if (!hal_get_hardware_detected(hp))
        return -1;

    /* Initialize I2C bus */
    hal_initialize_i2c(hp);

    /* Initialize DVI switch at I2C addr 0x21 */
    hal_set_pca9555_clk_val_k(hp, 0x06, 0xb0);
    hal_set_pca9555_clk_val_k(hp, 0x07, 0xbf);

    /* Initialize DVI switch at I2C addr 0x22 */
    hal_set_pca9555_dvi_val_k(hp, 0x06, 0xff);
    hal_set_pca9555_dvi_val_k(hp, 0x07, 0xf7);

    /* Turn off LEDs */
    hal_LED_off(hp, VIDEO_LED);
    hal_LED_off(hp, CONNECTION_LED);

    if (hal_get_hardware_revision(hp) < 2) {
        printf("Rev. 1 hardware. Not initializing audio ADC and Keyboard/Mouse.\n");
        return 0;
    }

    /* Don't do this! This will either disconnect */
    /* all USB devices or hang the system.        */
    /* Reset Personality Module */
#if 0
    val = hal_get_pm_pca9555_val_k(hp, 0x03);
    hal_set_pm_pca9555_val_k(hp, 0x03, val & 0xdf);
    usleep(100);
    val = hal_get_pm_pca9555_val_k(hp, 0x07);
    hal_set_pm_pca9555_val_k(hp, 0x07, val & 0xdf);
    usleep(100);
    val = hal_get_pm_pca9555_val_k(hp, 0x03);
    hal_set_pm_pca9555_val_k(hp, 0x03, val | 0x20);
    sleep(1);
#endif

    /* Disable Audio in FPGA */
    hal_and_in_fpga_val_k(hp, FPGA_ACREG, ~ACREG_AUD_ENABLE);

    /* Set Audio ADC */
    /* TODO - set ak4683 register 0x0c and 0x0f  */
    /* for audio volumne control                 */
    hal_set_ak4683aa_val_k(hp, 0x00, 0xa0);
    usleep(1000);
    hal_set_ak4683aa_val_k(hp, 0x04, 0x41);
    usleep(1000);
    hal_set_ak4683aa_val_k(hp, 0x08, 0x22);
    usleep(1000);
    hal_set_ak4683aa_val_k(hp, 0x09, 0x02);
    usleep(1000);
    hal_set_ak4683aa_val_k(hp, 0x0a, 0x33);
    usleep(1000);
    hal_set_ak4683aa_val_k(hp, 0x00, 0xa1);
    usleep(1000);

    board_type = hal_get_board_type(hp);

    /* Initialize for PS2 and USB Keyboard/Mouse */
    if (board_type == BOARDTYPE_TX) {
        struct tx_kbm_config kbm_cfg;
        /* Choose TX PS2 KBM: set PCA9555 at I2C addr */
        /* 0x20 bit 6 for registers 6 and 2 to 0      */
        val = hal_get_pm_pca9555_val_k(hp, 0x06);
        hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xbf);
        usleep(100);
        val = hal_get_pm_pca9555_val_k(hp, 0x02);
        hal_set_pm_pca9555_val_k(hp, 0x02, val & 0xbf);
        usleep(100);

        hal_set_fpga_val_k(hp, FPGA_KMSEL_REG, 0x00);

        /* Choose TX USB KBM: set PCA9555 at I2C addr */
        /* 0x20 bit 5 for registers 6 and 2 to 0      */
        val = hal_get_pm_pca9555_val_k(hp, 0x06);
        hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xdf);
        usleep(100);
        val = hal_get_pm_pca9555_val_k(hp, 0x02);
        hal_set_pm_pca9555_val_k(hp, 0x02, val & 0xdf);

        /* Enable TX PS2 and USB KBM */
        hal_set_fpga_val_k(hp, FPGA_MKCREG, 0x7900 |
                                        MKCREG_XP_MSEOUT_ENABLE |
                                        MKCREG_XP_KBDOUT_ENABLE);
        hal_set_fpga_val_k(hp, FPGA_MKCREG_Y, 0x7900 |
                                        MKCREG_XP_MSEOUT_ENABLE |
                                        MKCREG_XP_KBDOUT_ENABLE);

        /* Initialize Keyboard and Mouse Type */
        get_tx_kbm_config(hp, &kbm_cfg);
        hal_setKeyboardType(hp, kbm_cfg.keyboard_type);
    }
    else if (board_type == BOARDTYPE_RX) {
        struct rx_kbm_config kbm_cfg;
        /* Choose RX USB KBM: set PCA9555 at I2C addr */
        /* 0x20 bits 6 & 2 for registers 6 and 2 to 0 */
        val = hal_get_pm_pca9555_val_k(hp, 0x06);
        hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xbb);
        usleep(100);
        val = hal_get_pm_pca9555_val_k(hp, 0x02);
        hal_set_pm_pca9555_val_k(hp, 0x02, val & 0xbb);

        hal_set_fpga_val_k(hp, FPGA_KMSEL_REG, 0x01);

        /* Enable RX PS2 KBM */
        hal_set_fpga_val_k(hp, FPGA_MKCREG, 0x7900 |
                                        MKCREG_XP_SCANCODE_MASK |
                                        MKCREG_XP_MSEIN_ENABLE |
                                        MKCREG_XP_KBDIN_ENABLE);
        /* Enable TX USB KBM for loopback */
        hal_set_fpga_val_k(hp, FPGA_MKCREG_Y, 0x7900 |
                                        MKCREG_XP_MSEOUT_ENABLE |
                                        MKCREG_XP_KBDOUT_ENABLE);

        /* Initialize Keyboard Type */
        get_rx_kbm_config(hp, &kbm_cfg);
        hal_setKeyboardType(hp, kbm_cfg.local_keyboard_type);
    }

    /* Set audio volume */
    hal_set_pm_pca9555_val_k(hp, 0x03, 0xb7);
    hal_set_pm_pca9555_val_k(hp, 0x07, 0x87);

    /* Since resetting the Personality resets the USB devices, */
    /* we wait 1 sec to give the devices a chance to re-start  */
    sleep(1);

    /* Enable Audio in FPGA */
    hal_or_in_fpga_val_k(hp, FPGA_ACREG, ACREG_AUD_ENABLE);

    /* Turn off Mode LED on XP100 */
    if (get_hardware_profile() == HARDWARE_PROFILE_XP100) {
        val = hal_get_pm_pca9555_val_k(hp, 0x06);
        hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xf9);
        val = hal_get_pm_pca9555_val_k(hp, 0x02);
        hal_set_pm_pca9555_val_k(hp, 0x02, val | 0x06);
    }
#endif
    return 0;
}

int hal_deactivate_board(HAL *hp) {
    int rc = 0;
#ifndef __XPI__
    int fd;

    if ((fd = hp->kernel_fd) != -1) {
        /* Release FPGA driver resources for the board */
        if (hal_get_board_status(hp) & FPGA_BOARD_INITIALIZED)
            rc = ioctl(fd, FPGA_IOC_BOARD_EXIT, NULL);
    }
    else
        rc = -1;
#endif
    return rc;
}

