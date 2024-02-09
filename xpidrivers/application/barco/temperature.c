/*
 * To compile, run:
 *   arm-none-linux-gnueabi-gcc -I../../driver/netvizxpi \
 *   -I../../../ipvs_work/buildroot/output/build/linux-svn \
 *   temperature.c -o temperature 
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
#include "/home/pwang/ipvs_work/buildroot/output/build/linux-svn/include/media/platform-netvizxpi.h"
#include "netvizxpi.h"

#define LM63REG_LT              0x00
#define LM63REG_RTHB            0x01
#define LM63REG_CFG             0x03
#define LM63REG_RTLB            0x10
#define LM63REG_RMTCRI_SETPOINT 0x19
#define LM63REG_TACHCNT_LOW     0x46
#define LM63REG_TACHCNT_HIGH    0x47
#define LM63REG_PWM_RPM         0x4a
#define LM63REG_SPINUP_CFG      0x4b
#define LM63REG_PWM_VAL         0x4c
#define LM63REG_PWM_FREQ        0x4d

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
                                 unsigned char port,
                                 unsigned char addr, 
                                 unsigned short reg,
                                 unsigned char *val) {
    struct reg_io regio;
    int rc = 0;

    if (port == ADV7604_I2C_PORT && addr == ADV7604_I2C_ADDR)
        regio.type = REGIO_TYPE_ADV7604_GET;
    else if (port == ADV7612_I2C_PORT && addr == ADV7612_I2C_ADDR)
        regio.type = REGIO_TYPE_ADV7612_GET;
    else if (port == SII7172_I2C_PORT && addr == SII7172_I2C_ADDR)
        regio.type = REGIO_TYPE_SII7172_GET;
    else if (addr == PXA9548A_I2C_ADDR)
        regio.type = REGIO_TYPE_PCA9548A_GET;
    else if (port == SI5338_I2C_PORT && addr == SI5338_I2C_ADDR)
        regio.type = REGIO_TYPE_SI5338_GET;
    else if (port == LM63_I2C_PORT && addr == LM63_I2C_ADDR)
        regio.type = REGIO_TYPE_LM63_GET;
    else if (port == ICS9FG104_I2C_PORT && addr == ICS9FG104_I2C_ADDR)
        regio.type = REGIO_TYPE_ICS9FG104_GET;
    else if (port == MAX6550_I2C_PORT && addr == MAX6550_I2C_ADDR)
        regio.type = REGIO_TYPE_MAX6550_GET;
    else if (port == XPIEXPANSION_I2C_PORT && addr == XPIEXPANSION_I2C_ADDR)
        regio.type = REGIO_TYPE_XPIEXPANSION_GET;
    else 
        return -1;

    regio.reg = reg;

    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) 
        return rc;

    *val = regio.value;

    return 0;
}

static int get_board_temperature(int fd, float *ltemp, float *rtemp)
{
    int local = 0, remote = 0;
    unsigned char reglt[1], regrthb[1], regrtlb[1];
    int port = LM63_I2C_PORT, addr = LM63_I2C_ADDR;

    *ltemp = *rtemp = 0;
    if (netvizxpi_get_i2c_val(fd, port, addr, LM63REG_LT, reglt) < 0) {
        printf("%s: failed to read LM63 reg 0x%x\n", __func__, LM63REG_LT);
        return -1;
    }
    else if (netvizxpi_get_i2c_val(fd, port, addr, LM63REG_RTHB, regrthb) < 0) {
        printf("%s: failed to read LM63 reg 0x%x\n", __func__, LM63REG_RTHB);
        return -1;
    }
    else if (netvizxpi_get_i2c_val(fd, port, addr, LM63REG_RTLB, regrtlb) < 0) {
        printf("%s: failed to read LM63 reg 0x%x\n", __func__, LM63REG_RTLB);
        return -1;
    }

    local = (reglt[0] & 0x80) ? ((int) reglt[0] - 0x100) : (int) reglt[0];
    remote = ((int) (regrthb[0] & 0x7f) << 3) | ((int) regrtlb[0] >> 5);
    remote = (regrthb[0] & 0x80) ? (remote - 0x100) : remote;
    *ltemp = (float) local;
    *rtemp = (float) remote / (float) 8;
    return 0;
}

static void print_temperature_help(void) {
    printf("\nUsage: temperature -d duration\n");
    printf("The -t option runs temperature monitoring for 'duration' secs \n");
};

int main(int argc, char **argv) {
    int opt, fd, duration = -1, sleeptime = 5;
    char *dev_name;
    float ltemp, rtemp;

    while ((opt = getopt(argc, argv, "d:h")) > 0) {
        switch (opt) {
        case 'd':
            duration = atoi(optarg) * sleeptime;
            break;
        case 'h':
        case '?':
        default:
            print_temperature_help();
            return 0;
        }
    }

    dev_name = "/dev/i2c-0";
    fd = do_open(dev_name);
    if (fd < 0) {
        printf("Failed to open %s as a I2C device.\n", dev_name);
        return -1;
    }

    /* Monitoring loop */
    while (duration-- != 0) {
        if (get_board_temperature(fd, &ltemp, &rtemp) == 0) {
            printf("Board local temperature %f, remote temperature %f\n",
                    ltemp, rtemp);
        }
        else {
            printf("Failed to get board temperature. Exiting.\n");
            break;
        }
        sleep(sleeptime);
    }

    do_close(fd);

    return 0;
}
