/*
 * To compile, run:
 *   arm-none-linux-gnueabi-gcc xreg.c -o xreg 
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
#include <asm/types.h>  /* for videodev2.h */
#include <linux/videodev2.h>

#include "../../driver/netvizxpi/netvizxpi.h"

static int netvizxpi_open(char *dev_name) {
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

static void netvizxpi_close(int fd) {
    if (fd > 0)
        close(fd);
}

static int netvizxpi_get_val(int fd, 
                             unsigned long reg,
                             unsigned long *lval) {
    struct reg_io regio;
    int rc = 0;

    regio.type = REGIO_TYPE_FPGA_GET;
    regio.reg = reg;

    if ((rc = ioctl(fd, FPGA_IOC_MOD_REG, &regio)) < 0) 
        return rc;

    *lval = regio.value;

    return 0;
}

static int netvizxpi_set_val(int fd,
                             unsigned long reg, 
                             unsigned long val) {
    struct reg_io regio;

    regio.type = REGIO_TYPE_FPGA_SET;
    regio.reg = reg;
    regio.value = val;
    return ioctl(fd, FPGA_IOC_MOD_REG, &regio);
}

static int omnitek_get_val(int fd, 
                           int barno, 
                           unsigned long reg,
                           unsigned long *val) {
    struct omnitek_io omniio;
    int rc = 0;

    omniio.type = OMNITEKIO_TYPE_GET;
    omniio.barno = barno;
    omniio.reg = reg;

    if ((rc = ioctl(fd, FPGA_IOC_MOD_OMNITEK, &omniio)) < 0) 
        return rc;

    *val = omniio.value;

    return 0;
}

static int omnitek_set_val(int fd,
                           int barno, 
                           unsigned long reg, 
                           unsigned long val) {
    struct omnitek_io omniio;

    omniio.type = OMNITEKIO_TYPE_SET;
    omniio.barno = barno;
    omniio.reg = reg;
    omniio.value = val;
    return ioctl(fd, FPGA_IOC_MOD_OMNITEK, &omniio);
}

static void print_xreg_help(void) {
    printf("\nUsage: xreg [-o] [-b barno] [-r reg]\n");
    printf("If the -o switch is present, display or modify the Omnitek Bar\n");
    printf("'barno' registers starting at register 'reg' (Default barno 1)\n");
    printf("Otherwise display or modify FPGA registers starting at 'reg'.\n");
};

int main(int argc, char **argv) {
    int opt, fd, rc = 0;
    int barno = 1, do_omnitek = 0;
    char *dev_name;
    char buff[80];
    unsigned long reg = 0, lval;
    int delta = 4;

    while ((opt = getopt(argc, argv, "b:or:h")) > 0) {
        switch (opt) {
        case 'r':
            reg = strtoul(optarg, NULL, 16);
            break;
        case 'o':
            do_omnitek = 1;
            break;
        case 'b':
            barno = atoi(optarg);
            break;
        case 'h':
        case '?':
        default:
            print_xreg_help();
            return 0;
        }
    }

    dev_name = "/dev/video0";

    fd = netvizxpi_open(dev_name);
    if (fd < 0) {
        printf("Failed to open %s as a V4L2 device.\n", dev_name);
        return -1;
    }

    /* Handle interactive register display/modify */
    strcpy(buff, "");
    do {
        if (buff[0] == '.')
            break;
        if (strlen(buff) > 1) {
            reg -= delta;
            lval = strtoul(buff, NULL, 16);
            printf("0x%08lx -> 0x%08lx ? ", reg, lval);
            if (fgets(buff, 64, stdin) == NULL)
                break;
            if (buff[0] == '.')
                break;
            if (buff[0] == 'n' || buff[0] == 'N') {
                reg += 1;
                continue;
            }
            if (do_omnitek == 0) {
                rc = netvizxpi_set_val(fd, reg, lval);
                if (rc < 0) {
                    printf("error writing reg 0x%lx\n", reg);
                    break;
                }
            }
            else {
                rc = omnitek_set_val(fd, barno, reg, lval);
                if (rc < 0) {
                    printf("error writing bar %d reg 0x%lx\n", barno, reg);
                    break;
                }
            }
        }
        if (do_omnitek == 0) {
            rc = netvizxpi_get_val(fd, reg, &lval);
            if (rc < 0) {
                printf("error reading reg 0x%lx\n", reg);
                break;
            }
        }
        else {
            rc = omnitek_get_val(fd, barno, reg, &lval);
            if (rc < 0) {
                printf("error reading bar %d reg 0x%lx\n", barno, reg);
                break;
            }
        }
        printf("0x%08lx == 0x%08lx -> ", reg, lval);
        reg += delta;
    } while (fgets(buff, 64, stdin) != NULL);

    netvizxpi_close(fd);

    return 0;
}
