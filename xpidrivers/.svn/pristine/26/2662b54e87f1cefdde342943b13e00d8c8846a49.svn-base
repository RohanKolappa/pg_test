/*
 * make CROSS_COMPILE=arm-none-linux-gnueabi- ARCH=arm \
 * KERNELDIR=/home/pwang/ipvs_work/buildroot/output/build/linux-svn
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <asm/types.h>      /* for videodev2.h */
#include <linux/videodev2.h>

#include "/home/pwang/work.44599/buildroot/output/build/linux-svn/include/media/platform-netvizxpi.h"
#include "netvizxpi.h"

#define MAX_LINE_SIZE	80

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

static unsigned long readhex(unsigned char **bufptr) {
    unsigned char *cptr = *bufptr;
    unsigned char ch = *cptr;
    unsigned long val = 0;

    if (isxdigit(ch)) {
        do {
            val <<= 4;
            val += (isdigit(ch)) ? (ch - '0') : (toupper(ch) - 'A' + 10);
            cptr++;
            ch = *cptr;
        } while (isxdigit(ch));
        *bufptr = cptr;
    }
    else if (!isspace(ch)) {
        *bufptr = NULL;
    }
    return val;
}

static int read_hex_from_file(FILE *file,
                              unsigned char dat[],
                              int maxlen,
                              int longformat) {

    unsigned char buf[MAX_LINE_SIZE], *bufptr, ch;
    int n = 0;

    /* round up 'maxlen' to 4-byte boundary */
    if (longformat)
        maxlen = ((maxlen + 3) / 4) * 4;

    /* read from file 'file', if longformat = 1 convert to long values, */
    /* store at most 'maxlen' of the values into the array 'dat'        */
    rewind(file);
    while (fgets((char *)buf, MAX_LINE_SIZE, file) != NULL && n < maxlen) {
        buf[MAX_LINE_SIZE - 1] = '\0';
        if (strlen((char *)buf) == MAX_LINE_SIZE-1 && 
					buf[MAX_LINE_SIZE-2] != '\n') {
             return -1;
        }
        if (buf[0] == '\n') continue;

        bufptr = buf;
        while ((ch = *bufptr) && isspace(ch)) bufptr++;

        while ((ch = *bufptr) != 0 && ch != '\n') {
            if (longformat) {
                unsigned long result;
                if (n > maxlen - 4) break;
                result = readhex(&bufptr);
                dat[n++] = (unsigned char) (result & 0x000000ff);
                dat[n++] = (unsigned char) ((result & 0x0000ff00) >> 8);
                dat[n++] = (unsigned char) ((result & 0x00ff0000) >> 16);
                dat[n++] = (unsigned char) ((result & 0xff000000) >> 24);
            }
            else {
                dat[n++] = (unsigned char) readhex(&bufptr);
            }

            if (bufptr == NULL)
                return 0;
            while ((ch = *bufptr) && isspace(ch)) bufptr++;
        }
    }

    /* return number of values stored in 'dat' */
    return n;
}

static void print_setedid_help(void) {
    printf("\nUsage: setedid -a i2c_address -f hexfile\n");
    printf("Set EDID for I2C device at address i2c_address to data\n");
    printf("in file 'hexfile'\n"); 
};

int main(int argc, char **argv) {
    char *dev_name;
    int opt, fd, addr = -1, rc = 0, do_ddcedid = 0;
    struct edid_io edidio;
    char *filename = NULL;
    FILE *file;

    while ((opt = getopt(argc, argv, "a:f:h")) > 0) {
        switch (opt) {
        case 'a':
            addr = strtoul(optarg, NULL, 16);
            break;
        case 'f':
            filename = optarg;
            break;
        case 'h':
        case '?':
        default:
            print_setedid_help();
            return 0;
        }
    }
    if (filename == NULL) {
        print_setedid_help();
        return -1;
    }
    if ((file = fopen(filename, "r")) < 0) {
        printf("Can't open input file %s. %m.\n", filename);
        return -1;
    }
    if (read_hex_from_file(file, edidio.edid, 256, 0) != 256) {
        printf("Data file must have 256 bytes of data\n");
        fclose(file);
        return -1;
    }
    fclose(file);

    if (addr == ADV7604_I2C_ADDR)
        edidio.subdev = V4L2_SUBDEV_ADV7604;
    else if (addr == ADV7612_I2C_ADDR)
        edidio.subdev = V4L2_SUBDEV_ADV7612;
    else {
        printf("Invalid I2C address 0x%x\n", addr);
        return -1;
    }

    dev_name = "/dev/video0";

    fd = netvizxpi_open(dev_name);
    if (fd < 0) {
        printf("Failed to open %s as a V4L2 device. errno %d. %m.\n", 
                dev_name, errno);
        return -1;
    }
    if ((rc = ioctl(fd, FPGA_IOC_SUBDEV_SET_EDID, &edidio)) < 0) {
        printf("SUBDEV_SET_EDID failed for device %s.\n", dev_name);
        return rc;
    }
    printf("Successfully wrote EDID to device at I2C address 0x%x\n", addr);
    netvizxpi_close(fd);
    return 0;
}
