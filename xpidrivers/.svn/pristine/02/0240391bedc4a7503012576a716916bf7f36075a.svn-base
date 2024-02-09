#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <syslog.h>

#include "../../include/OmniTekIoctl_linux.h"

#define FPGA_BARNO                  1

#define MDIO_OFFSET                 0x2800

#define MDIO_OP_SETADDR             (0x0)
#define MDIO_OP_WRITE               (0x1)
#define MDIO_OP_READINC             (0x2)
#define MDIO_OP_READ                (0x3)

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

#define EMAC10GB                    (0)
#define TEMAC                       (1)

#define MDIO_DIVIDER                (0x0D)

int speed_settings_1G[][2]= {
	{0x00, 0xE008},
	{0x04, 0x7C00},
	{0x05, 0xC400},
	{0x0D, 0xFDBC},
	{0x0E, 0x0050},
	{0x0F, 0x3900},
	{0x10, 0x4900},
	{0x11, 0x7DBC},
	{0x12, 0x04B5},
	{0x13, 0x5500},
	{0x14, 0x2D00},
	{0x17, 0x909B},
	{0x1B, 0x78C0},
	{0x1F, 0x79C0},
	{0x23, 0x1064},
	{0x25, 0x0119},
	{0x26, 0x0149},
	{0x27, 0xC3DB},
	{0x28, 0x4149},
	{0x2F, 0x287F},
	{0x30, 0xDE83},
	{0x31, 0x9D7C},
	{0x39, 0x24C4},
	{0x43, 0x0C0B},
	{0x4D, 0x80EE}
};

static int debug = 0;

static int get_omnitek_fd(void)
{
    int fd;
    
    fd = open("/dev/OmniTekBAR1", O_RDWR);
    if (fd <= 0) {
        printf("Could not open device /dev/OmniTekBAR1\n");
        return -1;
    }
    return fd;
}

static void close_omnitek_fd(int fd)
{
    if (fd > 0)
        close(fd);
}

unsigned long get_omnitek_val(int fd, unsigned long reg)
{
    OmniTekBARIoctlBuffer buffer;
    int rc;

    buffer.Offset = reg;
    buffer.Buffer = NULL;
    buffer.Size = 0;
    rc = ioctl(fd, OMNITEK_BAR_IOCQRAWREADREGISTER, &buffer);
    if (rc < 0)
        printf("Error reading Bar 0x%x Offset 0x%x errno %d/%m\n",
                FPGA_BARNO, buffer.Offset, errno); 
    if (rc < 0)
        printf("Read 0x%x from Bar 0x%x Offset 0x%x errno %d/%m\n",
                buffer.Value, FPGA_BARNO, buffer.Offset, errno); 

    return buffer.Value;
}

void set_omnitek_val(int fd, unsigned long reg, unsigned long val)
{
    OmniTekBARIoctlBuffer buffer;
    int rc;

    buffer.Offset = reg;
    buffer.Value = val;
    buffer.Buffer = NULL;
    buffer.Size = 0;

    rc = ioctl(fd, OMNITEK_BAR_IOCQRAWWRITEREGISTER, &buffer);
    if (rc < 0)
        printf("Error writing 0x%lx to Bar 0x%x Offset 0x%x errno %d/%m\n",
                val, FPGA_BARNO, buffer.Offset, errno); 
    if (debug > 1)
        printf("Wrote 0x%lx to Bar 0x%x Offset 0x%x errno %d/%m\n",
                val, FPGA_BARNO, buffer.Offset, errno); 
}

int mdio_wait(int fd)
{
    int countstart = 10000;
    int count = countstart;
    int result = 0;

    while (--count) {
	    result = get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMREADY);

        if ((result & 0x1) == 0x1) {
	        break;
        }
        usleep(100);
    }
    if (count == 0) {
        printf("mdio_rdy never went high\n");
        return -EINVAL;
    }
    else {
        printf("mdio_rdy went high after %d\n", countstart - count);
    }
    return 0;
}

int mdio_set_address(int fd, int prtad, int devad, int addr)
{
    //Write out the set addresss control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 1);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, 
            (prtad << 5) | (devad & 0x1f));
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_WRITEDATA, addr);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_SETADDR);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    //Wait for MDIO to complete
    mdio_wait(fd);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 1);

    return 0;
}

int mdio_write(int fd, int prtad, int devad, int addr, unsigned short val)
{
    if (debug > 1)
        printf("MDIO write to prtad %x devad %x addr 0x%x, val %x\n", 
            prtad, devad, addr, val);

    mdio_set_address(fd, prtad, devad, addr);

    //Write out the write control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 1);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, 
            (prtad << 5) | (devad & 0x1f));
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_WRITEDATA, val);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 1);

    return 0;
}

int mdio_read(int fd, int prtad, int devad, int addr)
{
    int result = 0;

    mdio_set_address(fd, prtad, devad, addr);

    //Write out the read control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, 
            (prtad << 5) | (devad & 0x1f));
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    //Read back the value
    result = get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_READDATA);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    if (debug > 1)
        printf("Value read from prtad 0x%x devad 0x%x addr 0x%x = 0x%x\n",
            prtad, devad, addr, result);

    return result;
}

int sgmii_write(int fd, unsigned short addr, unsigned short val)
{
    printf("SGMII write to register: %x, value: %x\n", addr, val);

    addr |= 0x440;

    //Write out the write control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 1);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, addr);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_WRITEDATA, val);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    mdio_wait(fd);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 1);

    return 0;
}

int sgmii_read(int fd, unsigned short addr)
{
    int result = 0;

    printf("SGMII read from register: %x\n", addr);

    addr |= 0x440;

    //Write out the read control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, addr);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READINC);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    mdio_wait(fd);

    //Read back the value
    result = get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_READDATA);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);

    printf("SGMII value read from addr 0x%08X = 0x%08X \n", addr, result);

    return result;
}

int mac_read(int fd, int address)
{
    int result = 0;

    // select host bus 
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, address);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    // Select host interface
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0);

    //Read a value back
    result = get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_READDATA);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);

    if (debug > 1)
        printf("MAC read at 0x%04x - value=0x%08X\n", address,result);

    return result;
}

void mac_write(int fd, int address, int value)
{
    if (debug > 1)
        printf("MAC write at 0x%04x with 0x%04x\n", address, value);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    // Write the value
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x00);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_ADDRESS, address);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_WRITEDATA, value);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);

    // Set to default
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
}

int drp_read(int fd, int address)
{
    int result = 0;

    printf("DRP read at 0x%04x\n", address);

    //Write out the read control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_WRITEENABLE, 0x00);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_ADDRESS, address);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_ENABLE, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_ENABLE, 0x00);

    //Read a value back
    result = get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_READDATA);

    return result;
}

void drp_write(int fd, int address, int value)
{
    printf("DRP write at 0x%04x with 0x%04x\n", address, value);

    //Write out the read control word
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_WRITEENABLE, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_ADDRESS, address);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_WRITEDATA, value);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_ENABLE, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_DRP_ENABLE, 0x00);
}

int mdio_set_speed_to_1G(int fd, int is_netviz, int skip)
{
    int settings, regvalue = 0;

	printf("mdio_set_speed_to_1G entered\n");

    if (skip >= 9) {
        printf("Skipping all of %s\n", __func__);
        return 0;
    }

	/* disable SGMII AutoNegotiation in TEMAC - reg 0 bit 12*/
	regvalue = sgmii_read(fd, 0);
	printf("AutoNegotiation register in TEMAC: 0x%08X\n", regvalue);
    if (skip >= 8) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }
	if (regvalue & 0x1000) {
		regvalue &= ~0x1000;
		sgmii_write(fd, 0, regvalue);
		printf("AutoNegotiation register in TEMAC set to 0x%08X\n", regvalue);
	}
    if (skip >= 7) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }

	/* disable SGMII AutoNegotiation in PHY - dev 30 reg 30 bit 0 */
	regvalue = mdio_read(fd, 3, 30, 30);
	printf("Phy autonegotiation register value: 0x%08X\n", regvalue);
    if (skip >= 6) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }
	if (regvalue & 0x0001) {
		printf("Disabling AutoNegotiation in phy\n");
		regvalue &= ~0x0001;
	    mdio_write(fd, 3, 30, 30, regvalue);
		printf("AutoNegotiation register in phy written to 0x%08X\n", regvalue);
	}
    if (skip >= 5) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }

	/* configure TEMAC speed - host address 300[31:30] */
    if (is_netviz)
	    regvalue = mac_read(fd, 0x700);
    else
	    regvalue = mac_read(fd, 0x300);
	printf("TEMAC speed register = 0x%08X\n", regvalue);
    if (skip >= 4) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }
	regvalue &= 0x3FFFFFFF;
	regvalue |= 0x80000000;
    if (is_netviz)
	    mac_write(fd, 0x700, regvalue);
    else
	    mac_write(fd, 0x300, regvalue);
	printf("TEMAC speed register set to 0x%08X\n", regvalue);
    if (skip >= 3) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }

	/* DRP write settings */
	printf("Setting DRP write settings\n");
	for (settings = 0 ; settings < 
               (sizeof(speed_settings_1G)/(sizeof(int)*2)); settings++) {
		drp_write(fd, speed_settings_1G[settings][0], 
                speed_settings_1G[settings][1]);
	}
    if (skip >= 2) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }

	/* Issue a GTX reset on GTX0-3 */
	printf("Issuing GTX reset on GTX0-3\n");
	set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MAC_10GSEL, 0);
    if (skip >= 1) {
        printf("Skipping the rest of %s\n", __func__);
        return 0;
    }
	mdio_write(fd, 1, 1, 0, 41024);

    return 0;
}

int mdio_init(int fd, int mac_device, int is_netviz) {
    int divider = MDIO_DIVIDER;

    printf("Initializing MDIO for Device %d...\n", mac_device);

    //Set default values on the host interface
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_MIIMSEL, 0x01);
    set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_REQUEST, 0x00);

    // Write to management configuration register (clock divider + MDIO enable)
    // init 10GB EMAC MDIO
    if (mac_device == EMAC10GB) {
        printf("Setting 10G Ethernet MAC clock divider to 0x%02x\n", divider);
        mac_write(fd, 0x340, divider + 0x20);
    }
    
    // init TEMAC MDIO
    if (mac_device == TEMAC) {
        printf("Setting TEMAC clock divider to 0x%02x\n", divider);
        if (is_netviz)
            mac_write(fd, 0x740, divider + 0x40);
        else
            mac_write(fd, 0x340, divider + 0x40);
    }

    return 0;
}

static void print_help(void) 
{
    printf("Usage: testmdio [-x] [-M -p prtad -d devad -a addr] [-s 1-9] [-O] [-D]\n");
    printf("Options:\n");
    printf("\t-x     Run the program in XPi mode\n");
    printf("\t-M     Do MDIO read on prtad devad addr instead of 'set_speed'\n");
    printf("\t-s 0-9 Do part (or none) of 'set_speed' (0: none, 9: all)\n");
    printf("\t-D     Turn on debug\n");
    printf("\t-O     Test OmniTekRegAccess\n");
}

int main(int argc, char **argv)
{
    int opt, fd, skip = -1, is_netviz = 1, regval;
    int do_mdio = 0, do_omnitek = 0, prtad = -1, devad = -1, addr = -1;

    while ((opt = getopt(argc, argv, "xMp:d:a:s:ODh")) > 0) {
        switch (opt) {
        case 'x':
            is_netviz = 0;
            break;
        case 'p':
            prtad = strtoul(optarg, NULL, 16);
            break;
        case 'd':
            devad = strtoul(optarg, NULL, 16);
            break;
        case 'a':
            addr = strtoul(optarg, NULL, 16);
            break;
        case 's':
            skip = atoi(optarg);
            break;
        case 'M':
            do_mdio = 1;
            break;
        case 'O':
            do_omnitek = 1;
            break;
        case 'D':
            debug = 2;
            break;
        case 'h':
        default:
            printf("Usage: %s [-x] [-M -p prtad -d devad -a addr] [-s 1-9] [-O] [-D]\n", argv[0]);
            printf("Options:\n");
            printf("\t-x     Run the program in XPi mode\n");
            printf("\t-M     Do MDIO read on prtad devad addr instead of 'set_speed'\n");
            printf("\t-s 0-9 Do part (or none) of 'set_speed' (0: none, 9: all)\n");
            printf("\t-D     Turn on debug\n");
            printf("\t-O     Test OmniTekRegAccess\n");
            return 0;
        }
    }

    if ((fd = get_omnitek_fd()) < 0)
        return -1;

    if (do_omnitek) {
        printf("Omnitek Reg 0x00 = 0x%lx\n", get_omnitek_val(fd, 0));
        printf("Omnitek Reg 0x01 = 0x%lx\n", get_omnitek_val(fd, 1));
        printf("Omnitek Reg 0x0a = 0x%lx\n", get_omnitek_val(fd, 10));
        printf("Omnitek Reg 0x%x = 0x%lx\n", 
                MDIO_OFFSET + BARCO_MDIO_OPCODE,
                get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE));
        printf("Setting Omnitek Reg 0x%x to 0\n", MDIO_OFFSET + BARCO_MDIO_OPCODE);
        set_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE, 0);
        printf("Read back Omnitek Reg 0x%x = 0x%lx\n", 
                MDIO_OFFSET + BARCO_MDIO_OPCODE,
                get_omnitek_val(fd, MDIO_OFFSET + BARCO_MDIO_OPCODE));
    }
    else {
        if (is_netviz) {
            mdio_init(fd, EMAC10GB, 0);
        }
        mdio_init(fd, TEMAC, is_netviz);

        if (do_mdio) {
            if (prtad >= 0 && devad >= 0 && addr >= 0) {
	            regval = mdio_read(fd, prtad, devad, addr);
                printf("mdio_read PHYAD 0x%x REGAD 0x%x ADDR 0x%x returns 0x%x\n",
                    prtad, devad, addr, regval);
            }
            else {
                print_help();
            }
        }
        else if (skip >= 0) {
            mdio_set_speed_to_1G(fd, is_netviz, skip);    
        }
        else {
            mdio_set_speed_to_1G(fd, is_netviz, 0);    
        }
    }
    close_omnitek_fd(fd);

    return 0;
}
