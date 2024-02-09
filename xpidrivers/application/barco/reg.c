/*
 * To compile, run:
 *   arm-none-linux-gnueabi-gcc genreg.c -o genreg 
 *   ln -s mreg genreg
 *   ln -s macreg genreg
 *   ln -s drpreg genreg
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
#include <asm/types.h>  /* for videodev2.h */
#include <linux/videodev2.h>

#include "../../include/OmniTekIoctl_linux.h"


#define OPTION_INVALID  -1
#define OPTION_NULL     0
#define OPTION_READ     1
#define OPTION_WRITE    2

#define FPGA_BARNO      1
#define MDIO_OFFSET     0x2800

#define MDIO_OP_SETADDR (0x0)
#define MDIO_OP_WRITE   (0x1)
#define MDIO_OP_READ    (0x2)

#define MDIO_DIVIDER    (0x0D)

#define BARCO_MDIO_OPCODE           (0x00)
#define BARCO_MDIO_ADDRESS          (0x02)
#define BARCO_MDIO_WRITEDATA        (0x04)
#define BARCO_MDIO_READDATA         (0x06)
#define BARCO_MDIO_MIIMSEL          (0x08)
#define BARCO_MDIO_REQUEST          (0x0A)
#define BARCO_MDIO_MIIMREADY        (0x0C)
#define BARCO_MDIO_DRP_ADDRESS      (0x0E)
#define BARCO_MDIO_DRP_ENABLE       (0x10)
#define BARCO_MDIO_DRP_WRITEDATA    (0x12)
#define BARCO_MDIO_DRP_WRITEENABLE  (0x14)
#define BARCO_MDIO_DRP_READDATA     (0x16)
#define BARCO_MDIO_DRP_READY        (0x18)
#define BARCO_MDIO_MAC_IFG          (0x1A)
#define BARCO_MDIO_MAC_10GSEL       (0x1C)

int debug = 0;

static int omnitek_open(char *dev_name) {
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

static void omnitek_close(int fd) {
    if (fd > 0)
        close(fd);
}

static unsigned long get_omnitek_val_k(int fd, unsigned long reg)
{
    OmniTekBARIoctlBuffer buffer;
    int rc;

    buffer.Offset = reg;
    buffer.Buffer = NULL;
    buffer.Size = 0;
    rc = ioctl(fd, OMNITEK_BAR_IOCQRAWREADREGISTER, &buffer);
    if (rc < 0) {
        printf("Error read from Bar 0x%x Offset 0x%x errno %d/%m\n",
            	FPGA_BARNO, buffer.Offset, errno);
        return 0;
    }
    if (debug)
        printf("Read from Bar 0x%x Offset 0x%x, returning 0x%x\n",
            	FPGA_BARNO, buffer.Offset, buffer.Value);
    return buffer.Value;
}

void set_omnitek_val_k(int fd, unsigned long reg, unsigned long val)
{
    OmniTekBARIoctlBuffer buffer;
    int rc;

    buffer.Offset = reg;
    buffer.Value = val;
    buffer.Buffer = NULL;
    buffer.Size = 0;

    rc = ioctl(fd, OMNITEK_BAR_IOCQRAWWRITEREGISTER, &buffer);
    if (rc < 0)
        printf("Writing 0x%lx to bar 0x%x offset 0x%x errno %d/%m\n", 
            	val, FPGA_BARNO, buffer.Offset, errno);
    if (debug)
        printf("Wrote 0x%lx to Bar 0x%x Offset 0x%x\n", 
                val, FPGA_BARNO, buffer.Offset);
}

static int mdio_wait(int fd)
{
    int countstart = 10000;
    int count = countstart;
    int result = 0;

    while (--count) {
        result = get_omnitek_val_k(fd, MDIO_OFFSET + BARCO_MDIO_MIIMREADY);
        if ((result & 0x1) == 0x1) {
            break;
        }
        usleep(100);
    }
    if (count == 0) {
        return -ETIMEDOUT;
    }
    else if (debug > 1) {
        printf("%s: mdio_rdy went high after %d\n", __func__, 
                countstart - count);
    }
    return 0;
}

static int read_mdio_val_k(int fd, int phyad, int regad, unsigned long *val) 
{
    int mdio_offset = MDIO_OFFSET;
    unsigned long result = 0;

    if (debug > 1)
        printf("MDIO read from phyad: %x, regad: %x\n", phyad, regad);

    //Write out the read control word
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_ADDRESS, 
                (phyad << 5) | (regad & 0x1f));
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_REQUEST, 0x00);

    //Wait for MDIO to compltete
    if (mdio_wait(fd) < 0) {
        printf("%s: MDIO timed out\n", __func__);
        return -1;
    }
    else {
        //Read back the value
        result = get_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_READDATA);

        //Set to default
        set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
        set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);

        if (debug > 1)
            printf("Read 0x%08lx from phyad 0x%x regad 0x%x\n", result, 
                phyad, regad);
        *val = result;
    }
    return 0;
}

static void write_mdio_val_k(int fd, int phyad, int regad, unsigned long val) 
{
    int mdio_offset = MDIO_OFFSET;

    if (debug > 1)
       printf("MDIO write to phyad %x regad %x, value: %08lx\n", 
               phyad, regad, val);
    
    //Write out the write control word
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 1);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_ADDRESS,
            (phyad << 5) | (regad & 0x1f));
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_WRITEDATA, val);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    
    //Wait for MDIO to compltete
    if (mdio_wait(fd) < 0) {
        printf("%s: MDIO timed out\n", __func__);
    }
    else {
        //Set to default
        set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
        set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);

        if (debug > 1)
            printf("Wrote 0x%08lx to phyad 0x%x regad 0x%0x\n", 
                    val, phyad, regad);
    }
}

static int read_mac_val_k(int fd, int dummy, int regad, unsigned long *val) 
{
    int mdio_offset = MDIO_OFFSET;
    unsigned long result = 0;

    if (debug > 1)
        printf("MAC read from regad: %x\n", regad);

    //Select host bus
    set_omnitek_val_k(fd, mdio_offset+BARCO_MDIO_ADDRESS, regad);

    //Set to default
    set_omnitek_val_k(fd, mdio_offset+BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val_k(fd, mdio_offset+BARCO_MDIO_MIIMSEL, 1);
    set_omnitek_val_k(fd, mdio_offset+BARCO_MDIO_REQUEST, 0);

    //Select host interface
    set_omnitek_val_k(fd, mdio_offset+BARCO_MDIO_MIIMSEL, 0);

    //Read value
    result = get_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_READDATA);

    //Set to default
    set_omnitek_val_k(fd, mdio_offset+BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);

    if (debug > 1)
        printf("Read 0x%08lx from regad 0x%0x\n", result, regad);

    *val = result;

    return 0;
}

static void write_mac_val_k(int fd, int dummy, int regad, unsigned long val) 
{
    int mdio_offset = MDIO_OFFSET;

    if (debug > 1)
       printf("MDIO write to regad: %x, value: %08lx\n", regad, val);

    // Set to default
    set_omnitek_val_k(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val_k(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val_k(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    //Write the value
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 0);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_ADDRESS, regad);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_WRITEDATA, val);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);

    //Set to default
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_MIIMSEL, 1);

    if (debug > 1)
        printf("Wrote 0x%08lx to addr 0x%x\n", val, regad);
}

static int read_drp_val_k(int fd, int dummy, int regad, unsigned long *val) 
{
    int mdio_offset = MDIO_OFFSET;
    unsigned long result = 0;

    if (debug > 1)
        printf("DRP read from addr: %x\n", regad);

    //Write out the read control word
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_WRITEENABLE, 0x00);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_ADDRESS, regad);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_ENABLE, 0x01);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_ENABLE, 0x00);

    // Read value
    result = get_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_READDATA);

    if (debug > 1)
        printf("Read 0x%08lx from addr 0x%x\n", result, regad);

    *val = result;

    return 0;
}

static void write_drp_val_k(int fd, int dummy, int regad, unsigned long val) 
{
    int mdio_offset = MDIO_OFFSET;

    if (debug > 1)
       printf("DRP write to addr: %x, value: %08lx\n", regad, val);
    
    //Write out the read control word
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_WRITEENABLE, 0x00);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_ADDRESS, regad);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_WRITEDATA, val);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_ENABLE, 0x01);
    set_omnitek_val_k(fd, mdio_offset + BARCO_MDIO_DRP_ENABLE, 0x00);

    if (debug > 1)
        printf("Wrote 0x%08lx to addr 0x%x\n", val, regad);
}

static void print_genreg_help(char *progname) {
    char prg[4];

    if (strcasecmp(progname, "mreg") == 0) {
        printf("\nUsage: mreg [-p phyad] [-r regad] [-D] [-R] [-W val] \n");
        printf("Display or modify MDIO registers on PHYAD 'phyad' starting at");
        printf("\nREGAD 'regad'. 'phyad', 'regad' and 'val' should be in HEX\n");
    }
    else {
        prg[0] = toupper(progname[0]);
        prg[1] = toupper(progname[1]); 
        prg[2] = toupper(progname[2]); 
        prg[3] = 0;
        printf("\nUsage: %s [-r regad] [-D] [-R] [-W val] \n", progname);
        printf("Display or modify %s registers starting at address 'regad'\n", 
                prg);
        printf("\n'regad' and 'val' should be in HEX\n");
    };
    printf("Options: -D      Turn on debug\n"); 
    printf("         -R      Read register 'reg' non-interactively.\n");
    printf("         -W val  Write 'val' to register 'reg' non-interactively.\n");
};

static int parse_mreg_args(int argc, char **argv, int *phyad, 
        int *regad, unsigned long *val) 
{
    int opt, rc = OPTION_NULL;

    while ((opt = getopt(argc, argv, "p:r:RW:h")) > 0) {
        switch (opt) {
        case 'p':
            *phyad = strtoul(optarg, NULL, 16);
            break;
        case 'r':
            *regad = strtoul(optarg, NULL, 16);
            break;
        case 'R':
            rc = OPTION_READ;
            break;
        case 'W':
            rc = OPTION_WRITE;
            *val = strtoul(optarg, NULL, 16);
            break;
        case 'h':
        case '?':
        default:
            print_genreg_help(argv[0]);
            return OPTION_INVALID;
        }
    }
    if (*regad < 0) {
        print_genreg_help(argv[0]);
        return OPTION_INVALID;
    }
    return rc;
}

static int parse_genreg_args(int argc, char **argv, int *phyad, int *regad,
        unsigned long *val) 
{
    int opt, rc = OPTION_NULL;

    while ((opt = getopt(argc, argv, "r:RW:h")) > 0) {
        switch (opt) {
        case 'r':
            *regad = strtoul(optarg, NULL, 16);
            break;
        case 'R':
            rc = OPTION_READ;
            break;
        case 'W':
            rc = OPTION_WRITE;
            *val = strtoul(optarg, NULL, 16);
            break;
        case 'h':
        case '?':
        default:
            print_genreg_help(argv[0]);
            return OPTION_INVALID;
        }
    }
    if (*regad < 0) {
        print_genreg_help(argv[0]);
        return OPTION_INVALID;
    }
    return rc;
}

int main(int argc, char **argv) {
    int fd, rc = 0, phyad = 1, regad = -1, rw_option = -1;
    unsigned long val;
    char dev_name[80], buff[80];
    int (*read_func) (int, int, int, unsigned long *);
    void (*write_func) (int, int, int, unsigned long);
    int delta = 1;

    if (strcasecmp(argv[0], "mreg") == 0) {
        if ((rw_option = parse_mreg_args(argc, argv, &phyad, &regad, &val)) < 0)
            return -1;
        read_func = read_mdio_val_k;
        write_func = write_mdio_val_k;
    }
    else {
        if ((rw_option = parse_genreg_args(argc, argv, &phyad, &regad, &val)) < 0)
            return -1;
        else if (strcasecmp(argv[0], "macreg") == 0) {
            read_func = read_mac_val_k;
            write_func = write_mac_val_k;
        }
        else if (strcasecmp(argv[0], "drpreg") == 0) {
            read_func = read_drp_val_k;
            write_func = write_drp_val_k;
        }
        else {
            printf("%s not implmented\n", argv[0]);
            return -1;
        }
    }

    if (rw_option == OPTION_INVALID)
        return -1;

    sprintf(dev_name, "/dev/OmniTekBAR%d", FPGA_BARNO);

    fd = omnitek_open(dev_name);
    if (fd < 0) {
        printf("Failed to open %s as a V4L2 device.\n", dev_name);
        return -1;
    }

    if (rw_option == OPTION_READ) {
        rc = read_func(fd, phyad, regad, &val);
        printf("0x%04x == 0x%08lx\n", regad, val);
        omnitek_close(fd);
        return rc;
    }
    else if (rw_option == OPTION_WRITE) {
        write_func(fd, phyad, regad, val);
        omnitek_close(fd);
        return 0;
    }

    /* Handle interactive register display/modify */
    strcpy(buff, "");
    do {
        if (buff[0] == '.')
            break;
        if (strlen(buff) > 1) {
            regad -= delta;
            val = strtoul(buff, NULL, 16);
            printf("0x%04x -> 0x%08lx ? ", regad, val);
            if (fgets(buff, 64, stdin) == NULL)
                break;
            if (buff[0] == '.')
                break;
            if (buff[0] == 'y' || buff[0] == 'Y') {
                write_func(fd, phyad, regad, val);
            }
        }
        rc = read_func(fd, phyad, regad, &val);
        if (rc < 0) {
            printf("error reading regad 0x%x. errno %d (%m).\n", regad, errno);
            break;
        }
        printf("0x%04x == 0x%08lx -> ", regad, val);
        regad += delta;
    } while (fgets(buff, 64, stdin) != NULL);

    omnitek_close(fd);

    return 0;
}
