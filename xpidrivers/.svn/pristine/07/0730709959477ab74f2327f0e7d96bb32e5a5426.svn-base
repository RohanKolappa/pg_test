/*
 *   make CROSS_COMPILE=arm-none-linux-gnueabi- ARCH=arm \
 *   KERNELDIR=/home/pwang/work.44599/buildroot/output/build/linux-svn
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <asm/types.h>      /* for videodev2.h */
#include <linux/videodev2.h>

#include "i2c-dev.h"
#include "../../../kernel/include/media/platform-netvizxpi.h"
#include "netvizxpi.h"

static int is_v4l2_subdev(int bus, int addr)
{
    if (bus == XPI_SWITCH_I2C_BUS && 
            (addr == ADV7604_I2C_ADDR || addr == ADV7612_I2C_ADDR ||
             addr == SII7172_I2C_ADDR))
        return 1;
    return 0;
}

static int do_open(char *dev_name) {
    struct stat st;
    int fd = -1;

    if (stat(dev_name, &st) == -1) {
        printf("Failed to identify %s: %m errno %d\n", dev_name, errno);
    }
    else if (!S_ISCHR(st.st_mode)) {
        printf("%s is not a device: %m errno %d\n", dev_name, errno);
    }
    else {
        fd = open(dev_name, O_RDWR|O_NONBLOCK, 0);
        if (fd == -1) {
            printf("Failed to open %s: %m errno %d\n", dev_name, errno);
        } 
    }
    return fd;
}

static void do_close(int fd) {
    if (fd > 0)
        close(fd);
}

static int netvizxpi_get_i2c_val(int fd, 
                                 unsigned char addr, 
                                 unsigned short reg,
                                 unsigned char *val) {
    struct reg_io regio;
    int rc = 0;

    if (addr == ADV7604_I2C_ADDR)
        regio.type = REGIO_TYPE_ADV7604_GET;
    else if (addr == ADV7612_I2C_ADDR)
        regio.type = REGIO_TYPE_ADV7612_GET;
    else if (addr == SII7172_I2C_ADDR)
        regio.type = REGIO_TYPE_SII7172_GET;
    else if (addr == PXA9548A_I2C_ADDR) 
        regio.type = REGIO_TYPE_PCA9548A_GET;
    else if (addr == SI5338_I2C_ADDR)
        regio.type = REGIO_TYPE_SI5338_GET;
    else if (addr == LM63_I2C_ADDR)
        regio.type = REGIO_TYPE_LM63_GET;
    else if (addr == ICS9FG104_I2C_ADDR)
        regio.type = REGIO_TYPE_ICS9FG104_GET;
    else if (addr == MAX6550_I2C_ADDR)
        regio.type = REGIO_TYPE_MAX6550_GET;
    else 
        return -1;

    regio.reg = reg;

    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) 
        return rc;

    *val = regio.value;

    return 0;
}

static int netvizxpi_set_i2c_val(int fd,
                                 unsigned char addr, 
                                 unsigned short reg, 
                                 unsigned char val) {
    struct reg_io regio;

    if (addr == ADV7604_I2C_ADDR)
        regio.type = REGIO_TYPE_ADV7604_SET;
    else if (addr == ADV7612_I2C_ADDR)
        regio.type = REGIO_TYPE_ADV7612_SET;
    else if (addr == SII7172_I2C_ADDR)
        regio.type = REGIO_TYPE_SII7172_SET;
    else if (addr == PXA9548A_I2C_ADDR)
        regio.type = REGIO_TYPE_PCA9548A_SET;
    else if (addr == SI5338_I2C_ADDR)
        regio.type = REGIO_TYPE_SI5338_SET;
    else if (addr == LM63_I2C_ADDR)
        regio.type = REGIO_TYPE_LM63_SET;
    else if (addr == ICS9FG104_I2C_ADDR)
        regio.type = REGIO_TYPE_ICS9FG104_SET;
    else if (addr == MAX6550_I2C_ADDR)
        regio.type = REGIO_TYPE_MAX6550_SET;
    else 
        return -1;

    regio.reg = reg;
    regio.value = val;
    return ioctl(fd, FPGA_IOC_MOD_REG, &regio);
}

static int expansion_get_i2c_val(int fd, 
                                 unsigned char addr, 
                                 unsigned short reg,
                                 unsigned char *val) {
    struct reg_io regio;
    int rc = 0;
    if ((rc = ioctl(fd, FPGA_IOC_SET_EXPANSION, &addr)) < 0) {
        printf("%s: failed to set I2C address on expansion port. rc = %d\n", 
                __func__, rc);
        *val = 0;
    }
    else {
        regio.type = REGIO_TYPE_XPIEXPANSION_GET;
        regio.reg = reg;
        rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio);
        if (rc == 0)
            *val = regio.value;
    }
    return rc;
}

static int expansion_set_i2c_val(int fd,
                                 unsigned char addr, 
                                 unsigned short reg, 
                                 unsigned char val) {
    struct reg_io regio;
    int rc = 0;
    if ((rc = ioctl(fd, FPGA_IOC_SET_EXPANSION, &addr)) < 0) {
        printf("%s: failed to set I2C address on expansion port. rc = %d\n",
                __func__, rc);
    }
    else {
        regio.type = REGIO_TYPE_XPIEXPANSION_SET;
        regio.reg = reg;
        regio.value = val;
        rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio);
    }
    return rc;
}

/*
 *  Reads byte data from regiser 'reg' of DDC EEPROM 
 */
static int ddcedid_get_i2c_val(int file, 
                               unsigned char reg,
                               unsigned char *val)
{
    struct reg_io regio;
    int rc = 0;

    regio.type = REGIO_TYPE_DDCI2C_GET;
    regio.reg = reg;
    rc = ioctl(file, FPGA_IOC_MOD_REG, &regio);
    if (rc == 0)
        *val = regio.value;

    return rc;
}

/*
 *  Writes byte data to regiser 'reg' of DDC EEPROM 
 */
static int ddcedid_set_i2c_val(int file,
                               unsigned char reg,
                               unsigned char val)
{
    struct reg_io regio;

    regio.type = REGIO_TYPE_DDCI2C_SET;
    regio.reg = reg;
    regio.value = val;

    return ioctl(file, FPGA_IOC_MOD_REG, &regio);
}

static void print_i2creg_help(void) {
    printf("\nUsage: i2creg -a i2c_address -r reg [-x] [-R|-W val] [-b bus] [-V|-Q]\n");
    printf("Options: -V \t Print chip version,\n");
    printf("         -Q \t Query DV timings on ADV7604\n");
    printf("         -R \t Read register 'reg' non-interactively.\n");
    printf("         -W \t Write 'val' to register 'reg' non-interactively.\n");
    printf("Otherwise display or modify registers starting at 'reg' (hex)\n");
    printf("Valid I2C addresses are:\n");
    printf("ADV7612: 0x4c, ADV7604: 0x20\n");
    printf("Register maps for ADV7612 and ADV7604 are:\n");
    printf("ADV7612: 0x0000 - 0x00ff    IO Map\n");
    printf("         0x0100 - 0x01ff    CEC Map\n");
    printf("         0x0200 - 0x02ff    InfoFrame Map\n");
    printf("         0x0300 - 0x03ff    DPLL Map\n");
    printf("         0x0400 - 0x04ff    Repeater Map\n");
    printf("         0x0500 - 0x05ff    EDID Map\n");
    printf("         0x0600 - 0x06ff    HDMI Map\n");
    printf("         0x0700 - 0x07ff    CP Map\n");
    printf("ADV7604: 0x0000 - 0x00ff    IO Map\n");
    printf("         0x0100 - 0x01ff    AVLINK Map\n");
    printf("         0x0200 - 0x02ff    CED Map\n");
    printf("         0x0300 - 0x03ff    InfoFrame Map\n");
    printf("         0x0400 - 0x04ff    ESDP Map\n");
    printf("         0x0500 - 0x05ff    DPP Map\n");
    printf("         0x0600 - 0x06ff    AFE Map\n");
    printf("         0x0700 - 0x07ff    Repeator Map\n");
    printf("         0x0800 - 0x08ff    EDID Map\n");
    printf("         0x0900 - 0x09ff    HDMI Map\n");
    printf("         0x0a00 - 0x0aff    Test Map\n");
    printf("         0x0b00 - 0x0bff    CP Map\n");
    printf("         0x0c00 - 0x0cff    VDP Map\n");
    printf("SII7172: 0x0000 - 0x00ff    Master Map\n");
    printf("         0x0100 - 0x01ff    Slave Map\n");
    printf("PCA9548A     : Bus 0 switch, Addr 0x73\n");
    printf("Si5338       : Bus 0, Port 3, Addr 0x70\n");
    printf("LM63         : Bus 0, Port 4, Addr 0x4c\n");
    printf("ICS9FG104    : Bus 0, Port 4, Addr 0x6e\n");
    printf("Max6550      : Bus 0, Port 4, Addr 0x1b\n");
    printf("DDC          : Bus 1, Addr 0x50 (Bus 1)\n");
    printf("The -x option reads/writes I2C devices on the expansion board\n");
};

int main(int argc, char **argv) {
    int opt, addr = -1, bus = XPI_SWITCH_I2C_BUS, reg = 0, fd, rc = 0;
    int chipid = 0, query = 0, do_read = 0, do_write = 0, is_expansion = 0;
    char *dev_name;
    unsigned char cval;
    char buff[80];

    while ((opt = getopt(argc, argv, "b:a:r:xRW:QVh")) > 0) {
        switch (opt) {
            case 'b':
                bus = strtoul(optarg, NULL, 16);
                break;
            case 'a':
                addr = strtoul(optarg, NULL, 16);
                break;
            case 'r':
                reg = strtoul(optarg, NULL, 16);
                break;
            case 'x':
                is_expansion = 1;
                break;
            case 'R':
                do_read = 1;
                break;
            case 'W':
                do_write = 1;
                cval = (unsigned char) strtoul(optarg, NULL, 16);
                break;
            case 'Q':
                query = 1;
                break;
            case 'V':
                chipid = 1;
                break;
            case 'h':
            case '?':
            default:
                print_i2creg_help();
                return 0;
        }
    }
    
    if (chipid && (!is_v4l2_subdev(bus, addr) || bus != 0)) {
        printf("-V is only supported on ADV7604, ADV7612 and SII7172\n");
        return -1;
    }

    if (bus != 0 && bus != 1) {
        printf("Only bus 0 and bus 1 are supported\n");
        return -1;
    }

    if (bus == 1) {
        if (addr != -1 && addr != 0x50) {
            printf("Only I2C addr 0x50 (EEPROM) is supported on bus 1\n");
            return -1;
        }
        addr = 0x50;
    }

    if (query && (addr != ADV7604_I2C_ADDR)) {
        printf("-Q is only supported on ADV7604\n");
        return -1;
    }

    if (is_expansion) {
        bus = 0;
        is_expansion = 1;
    }

    dev_name = "/dev/video0";
    fd = do_open(dev_name);
    if (fd < 0) {
        printf("Failed to open %s as a I2C device.\n", dev_name);
        return -1;
    }

    /* Handle non-interactive commands */
    if (chipid) {
        unsigned char val_lo, val_hi;
        if (addr == ADV7604_I2C_ADDR) {
            if (netvizxpi_get_i2c_val(fd, addr, 0x11, 
                        &val_lo) < 0)
                printf("Failed to read ADV7604 RD_INFO register\n");
            else
                printf("Revision = %02x\n", val_lo);
        }
        else if (addr == ADV7612_I2C_ADDR) {
            if (netvizxpi_get_i2c_val(fd, addr, 0xeb, 
                        &val_lo) < 0)
                printf("Failed to read ADV7612 RD_INFO register\n");
            else
                printf("Revision = %02x%02x\n", val_hi, val_lo);
        }
        else {
            if (netvizxpi_get_i2c_val(fd, addr, 0x04, 
                        &val_hi) < 0)
                printf("Failed to read SII7172 Master DEV_REV register\n");
            else if (netvizxpi_get_i2c_val(fd, addr, 0x104, 
                        &val_lo) < 0)
                printf("Failed to read SII7172 Slave DEV_REV register\n");
            else
                printf("Revision = %02x (Master), %02x (Slave)\n", 
			        val_hi, val_lo);
        }
        do_close(fd);
        return 0;
    } 

    if (query && addr == ADV7604_I2C_ADDR) {
        if (ioctl(fd, FPGA_IOC_QUERY_DV_TIMINGS, &addr) < 0)
            printf("QUERY_DV_TIMINGS ioctl failed errno %d. %m\n", errno);
        do_close(fd);
        return 0;
    }

    if (do_read) {
        if (bus == 0) {
            if (is_expansion)
                rc = expansion_get_i2c_val(fd, addr, reg, &cval);
            else if (bus == 0)
                rc = netvizxpi_get_i2c_val(fd, addr, reg, &cval);
            if (rc < 0) {
                printf("error reading I2C addr 0x%x reg 0x%x\n", addr, reg);
            }
        }
        else {
            rc = ddcedid_get_i2c_val(fd, reg, &cval);
            if (rc < 0) {
                printf("error reading bus %d I2C addr 0x%x reg 0x%x\n", 
                        bus, addr, reg);
            }
        }
        if (rc == 0) 
            printf("0x%04x == 0x%02x\n", reg, cval);
        do_close(fd);
        return 0;
    }
    if (do_write) {
        if (bus == 0) {
            if (is_expansion)
                rc = expansion_set_i2c_val(fd, addr, reg, cval);
            else 
                rc = netvizxpi_set_i2c_val(fd, addr, reg, cval);
            if (rc < 0) {
                printf("error writing I2C addr 0x%x reg 0x%x\n", addr, reg);
            }
        }
        else {
            rc = ddcedid_set_i2c_val(fd, reg, cval);
            if (rc < 0) {
                printf("error writing bus %d I2C addr 0x%x reg 0x%x\n", 
                        bus, addr, reg);
            }
        }
        do_close(fd);
        return 0;
    }

    /* Handle interactive register display/modify */
    strcpy(buff, "");
    do {
        if (buff[0] == '.')
            break;
        if (strlen(buff) > 1) {
            reg -= 1;
            cval = (unsigned char) strtoul(buff, NULL, 16);
            printf("0x%04x -> 0x%02x ? ", reg, cval);
            if (fgets(buff, 64, stdin) == NULL)
                break;
            if (buff[0] == '.')
                break;
            if (buff[0] == 'y' || buff[0] == 'Y') {
                if (bus == 0 && is_expansion)
                    rc = expansion_set_i2c_val(fd, addr, reg, cval);
                else if (bus == 0)  
                    rc = netvizxpi_set_i2c_val(fd, addr, reg, cval);
                else
                    rc = ddcedid_set_i2c_val(fd, reg, cval);
                if (rc < 0) {
                    printf("error writing I2C addr 0x%x reg 0x%x\n", addr, reg);
                    break;
                }
            }
        }
        if (bus == 0 && is_expansion)
            rc = expansion_get_i2c_val(fd, addr, reg, &cval);
        else if (bus == 0)
            rc = netvizxpi_get_i2c_val(fd, addr, reg, &cval);
        else
            rc = ddcedid_get_i2c_val(fd, reg, &cval);

        if (rc < 0) {
            printf("error reading I2C addr 0x%x reg 0x%x\n", addr, reg);
            break;
        }
        printf("0x%04x == 0x%02x -> ", reg, cval);
        reg += 1;
    } while (fgets(buff, 64, stdin) != NULL);

    do_close(fd);

    return 0;
}
