#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <netinet/in.h> 
#include <ctype.h> 
#include <sys/stat.h> 
#include <linux/input.h> 

#include "hal.h" 
#include "hal_serial.h"
#include "vlib.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "kbms_utils.h"
#include "vlog.h"
#include "loadnet.h"
#include "vutils.h"
#include "generatedHeaders.h"
#include "plx9056pciif.h"
#include "ad9852dds.h"
#include "fpga.h"
#include "i2c_control.h"
#include "osd_control.h"
#include "temperature_control.h"
#include "lut_table.h"
#include "lut_utils.h"
#include "system_utils.h"
#include "override_config.h"
#include "chromakey_control.h"
#include "math.h"

#define MAX_PLX_EEPROM_SIZE             256
#define USB_EEPROM_SIZE                 128

/***********************************************
 * Routine to display and modify I2C registers *
 ***********************************************/
static void print_i2creg_help(void) {
    printf("\nUsage: i2creg -c N [Options]\n\n");
    printf("Displays or modifies XP200 I2C registers on Channel N interactively.\n");
    printf("Options:\n");
    printf("\t-E \t Display or modify register on EDID EEPROM.\n");
    printf("\t-e \t Display monitor EDID EEPROM.\n");
    printf("\t-m \t Display or modify register on I2C device on Main Board.\n");
    printf("\t-p \t Display or modify register on I2C device on Personality Module.\n");
    printf("\t-a Addr\t Display or modify register on I2C device \"Addr\" (in hex)\n");
    printf("\t-R Reg\t Display or modify register \"Reg\" (in hex) on I2C device.\n");
    printf("\t-s\t Run command silently (i.e. don't read back).\n");
    printf("\t-l\t List value of all registers on device.\n");
    printf("\t-d\t Turn on I2C debug.\n");
}

static int i2creg_main(int argc, char **argv) {
    int cno = -1, reg = 0, do_main_i2c = 0, do_pm_i2c = 0;
    int do_edid_eeprom = 0, do_mon_edid = 0;
    int opt, debug = 0, silent = 0, do_list = 0, rc = 0;
    HAL *hp; unsigned short addr = 0; 
    unsigned char cval, b[1];
    char buff[64], hwdesc[32]; 
    
    /******************* 
     * Allow only root *
     *******************/ 
    if (getuid() != 0) { 
        printf("You must be root to execute this command\n"); 
        return 0; 
    } 

    while ((opt = getopt(argc, argv, "c:a:EeR:slmpdh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'd':
            debug = 1;
            break;
        case 'E':
            do_edid_eeprom = 1;
            addr = EDID_EEPROM_I2C_ADDR;
            break;
        case 'e':
            do_mon_edid = 1;
            addr = MON_EDID_I2C_ADDR;
            break;
        case 'R':
            reg = strtoul(optarg, NULL, 16);
            break;
        case 'l':
            do_list = 1;
            break;
        case 'm':
            do_main_i2c = 1;
            break;
        case 's':
            silent = 1;
            break;
        case 'p':
            do_pm_i2c = 1;
            break;
        case 'a':
            addr = strtoul(optarg, NULL, 16);
            break;
        case 'x':
            reg = strtoul(optarg, NULL, 16);
            break;
        case 'h':
        default:
            print_i2creg_help();
            return 0;
        }
    }

    if (cno <= 0 || cno > MAX_FPGA_BOARDS) {
        print_i2creg_help();
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid Channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        hal_release(hp);
        return -1;
    }

    hal_get_hardware_description(hp, hwdesc, sizeof(hwdesc));

    if (!VALID_I2C_ADDR(addr)) {
        printf("Failed: I2C address out of range (0x%x).\n", addr);
        hal_release(hp);
        return -1;
    } 
    
    if (reg > MAX_I2C_REG(addr)) {
        printf("Failed: I2C register out of range (0x%x).\n", reg);
        hal_release(hp);
        return -1;
    }

    if (!do_main_i2c && !do_pm_i2c && addr == LM63_I2C_ADDR) {
        printf("Failed: Please specify -m or -p for LM63 I2C device\n");
        hal_release(hp);
        return -1;
    }

    if ((do_main_i2c && !VALID_MAIN_I2C_ADDR(addr)) ||
          (do_pm_i2c && strcasecmp(hwdesc, "V2D-XP100") 
           && addr == LM63_I2C_ADDR) ||
          (do_pm_i2c && !VALID_PM_I2C_ADDR(addr))) {
        printf("Failed: I2C address out of range (0x%x)\n", addr);
        hal_release(hp);
        return -1;
    }

    hal_set_i2c_debug(hp, debug);

    if (do_list) {
        while (reg <= MAX_I2C_REG(addr)) {
            if (do_edid_eeprom)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_EDIDEEPROM_READ, addr, 
                    (unsigned short) reg, 1, b);
            else if (do_mon_edid)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MONEDID_READ, addr, 
                    (unsigned short) reg, 1, b);
            else if (do_main_i2c)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, addr, 
                    (unsigned short) reg, 1, b);
            else if (do_pm_i2c)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, addr, 
                    (unsigned short) reg, 1, b);
            else
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_READ, addr, 
                    (unsigned short) reg, 1, b);
            if (rc < 0) {
                printf("error reading I2C addr 0x%x reg 0x%x\n", addr, reg);
                goto out;
            }
            cval = b[0];
            printf("0x%02x -> 0x%02x \n", reg, cval);
            reg += 1;
        }
        goto out;
    }

    strcpy(buff, "");
    do {
        if (buff[0] == '.')
            break;
                                                                                
        if (strlen(buff) > 1) {
                                                                                
            reg -= 1;
                                                                                
            cval = (unsigned char) strtoul(buff, NULL, 16);
            printf("0x%02x -> 0x%02x ? ", reg, cval);
                                                                                
            if (fgets(buff, 64, stdin) == NULL)
                break;
                                                                                
            if (buff[0] == '.')
                break;
            if (buff[0] == 'n' || buff[0] == 'N') {
                reg += 1;
                continue;
            }
            b[0] = cval;
            if (do_edid_eeprom)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_EDIDEEPROM_WRITE, addr, 
                                                (unsigned short) reg, 1, b);
            else if (do_main_i2c) 
                rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_WRITE, addr,
                                                (unsigned short) reg, 1, b);
            else if (do_pm_i2c)
                rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_PM_WRITE, addr,
                                                (unsigned short) reg, 1, b);
            else if (!do_mon_edid)
                rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_WRITE, addr,
                                                (unsigned short) reg, 1, b);
        }
        if (rc < 0) {
            printf("error writing I2C addr 0x%x reg 0x%x\n", addr, reg);
            goto out;
        }
        if (reg > MAX_I2C_REG(addr))
            break;

        if (silent) {
            printf("0x%02x -> ", reg);
        }
        else {
            if (do_edid_eeprom)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_EDIDEEPROM_READ, addr, 
                    (unsigned short) reg, 1, b);
            else if (do_mon_edid) 
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MONEDID_READ, addr,
                                                (unsigned short) reg, 1, b);
            else if (do_main_i2c)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, addr,
                                                (unsigned short) reg, 1, b);
            else if (do_pm_i2c)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, addr,
                                                (unsigned short) reg, 1, b);
            else
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_READ, addr,
                                                (unsigned short) reg, 1, b);
            if (rc < 0) {
                printf("error reading I2C addr 0x%x reg 0x%x\n", addr, reg);
                goto out;
            }
            cval = b[0];
            printf("0x%02x == 0x%02x -> ", reg, cval);
        }
        reg += 1;
                                                                                
    } while (fgets(buff, 64, stdin) != NULL);
                                                                                
    hal_set_i2c_debug(hp, 0);
out:
    hal_release(hp);

    return 0;
}

/***********************************************
 * Routine to display and modify ADC registers *
 ***********************************************/
#define MAX_ADC_REG     0x19

static void print_adcreg_help(void) {
    printf("\nUsage: adcreg [Options]\n\n");
    printf("Displays or modifies ADC registers interactively.\n");
    printf("Options:\n");
    printf("\t-c N\t Read and modify I2C device on Channel N (default 1).\n");
    printf("\t-R Reg\t Set first register to be displayed to Reg (default 0)\n");
    printf("\t-s\t Run command silently (i.e. don't read back).\n");
    printf("\t-d\t Turn on I2C debug\n");
}

static int adcreg_main(int argc, char **argv) {
    int opt, cno = -1, is_i50 = 0, reg = 0, debug = 0, silent = 0;
    int hardware_type, rc = 0;
    unsigned char cval, b[1];
    char buff[64];
    HAL *hp;

    /******************* 
     * Allow only root * 
     *******************/
    if (getuid() != 0) {
        printf("You must be root to execute this command\n");
        return 0;
    }

    hp = hal_acquire(0);
    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }

    while ((opt = getopt(argc, argv, "c:R:dsh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'R':
            reg = strtoul(optarg, NULL, 16);
            break;
        case 'd':
            debug = 1;
            break;
        case 's':
            silent = 1;
            break;
        case 'h':
        default:
            print_adcreg_help();
            return 0;
        }
    }
    if (is_i50) {
        if (cno != 1) {
            if (hp) hal_release(hp);
            print_adcreg_help();
            return -1;
        }
    }
    else {
        if (cno != 1) {
            if (hp) hal_release(hp);
            hp = hal_acquire(cno - 1);
        }
    }
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid Channel number (%d).\n", cno);
        return -1;
    }

    hardware_type = hal_get_hardware_type(hp);

    if (hardware_type  != PLX_DEVICE && hardware_type != V2DI50_DEVICE) {
        printf("Failed: Channel %d not an I50 or XP200.\n", cno);
        hal_release(hp);
        return -1;
    }
                                                                                
    if (reg < 0 || reg > MAX_ADC_REG) {
        printf("Failed: ADC register address out of range (0x%x).\n", reg);
        hal_release(hp);
        return 0;
    }

    hal_set_i2c_debug(hp, debug);

    strcpy(buff, "");
    do {
        if (buff[0] == '.')
            break;
                                                                                
        if (strlen(buff) > 1) {
                                                                                
            reg -= 1;
                                                                                
            cval = (unsigned char) strtoul(buff, NULL, 16);
            printf("0x%02x -> 0x%02x ? ", reg, cval);
                                                                                
            if (fgets(buff, 64, stdin) == NULL)
                break;
                                                                                
            if (buff[0] == '.')
                break;
            if (buff[0] == 'n' || buff[0] == 'N') {
                reg += 1;
                continue;
            }
            if (hardware_type == V2DI50_DEVICE)
                hal_set_ad9888_val_k(hp, reg, cval);
            else {
                b[0] = cval;
                rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_WRITE, 
                        AD9888_I2C_ADDR, (unsigned short) reg, 1, b);
            }
            if (rc < 0) { 
                printf("error writing ADC9888 reg 0x%x\n", reg);
                break;
            }
        }
        if (reg > MAX_ADC_REG)
            break;

        if (silent) {
            printf("0x%02x -> ", reg);
        }
        else {
            if (hardware_type == V2DI50_DEVICE)
                cval = hal_get_ad9888_val_k(hp, reg);
            else {
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, 
                        AD9888_I2C_ADDR, (unsigned short) reg, 1, b);
                cval = b[0];
            }
            if (rc < 0) {
                printf("error reading ADC9888 reg 0x%x\n", reg);
                break;
            }
            printf("0x%02x == 0x%02x -> ", reg, cval);
        }
        reg += 1;
                                                                                
    } while (fgets(buff, 64, stdin) != NULL);
                                                                                
    hal_set_i2c_debug(hp, 0);
    hal_release(hp);
    return 0;
}

/*************************************************
 * Utility to read and write RAM locations       *
 *************************************************/
#define RD_BIT          0x00000001
#define WR_BIT          0x00000002
#define DATA_MASK       0x000000ff
#define ADDR_MASK       0x00003fff
#define RDATA_SHIFT     2
#define WDATA_SHIFT     10
#define ADDR_SHIFT      18

#define MAX_DATA_SIZE   16384

static void print_ramread_help(void) {
    printf("Usage: ramread -c N -x Reg -s start -e end [-v] [-h]\n");
    printf("Displays XP200 Channel N content of RAM locations\n");
    printf("from 'start' to 'end' (in hex)\n");
}

static int ramread_main(int argc, char **argv) {
    int addr = 0, opt, cno = -1, reg = -1, start = -1, end = -1;
    unsigned long regval;
    int verbose = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "c:x:s:e:vh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'x':
            reg = strtoul(optarg, NULL, 16);
            break;
        case 's':
            start = (int) strtoul(optarg, NULL, 16);
            break;
        case 'e':
            end = (int) strtoul(optarg, NULL, 16);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        default:
            print_ramread_help();
            return 0;
        }
    }

    hp = hal_acquire(cno - 1);
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        goto out;
    }

    if (reg < 0) {
        printf("Failed: Invalid register location.\n");
        goto out;
    }

    if (start < 0 || end < 0 || start < end) {
        printf("Failed: Invalid start and/or end address.\n");
        goto out;
    }
                                                                                
    for (addr = start; addr <= end; addr++) {
        /* Make up content for register 'reg' */
        regval = ((addr & ADDR_MASK) << ADDR_SHIFT) | RD_BIT;
        hal_set_fpga_val_k(hp, reg, regval);
        usleep(1);

        /* Set RD bit to 0   */
        regval = hal_get_fpga_val_k(hp, reg);
        regval &= ~RD_BIT;
        hal_set_fpga_val_k(hp, reg, regval);
        usleep(1);

        /* Read the data */
        regval = hal_get_fpga_val_k(hp, reg);
        usleep(1);

        if (verbose)
            printf("Read 0x%08lx from register 0x%x\n", regval, reg);

        printf(" %02x", (unsigned char) ((regval >> RDATA_SHIFT) & DATA_MASK));
        if ((addr - start) % 16 == 15) printf("\n");
    }
    printf("\n");

out:
    hal_release(hp);
    return -1;
}

static void print_ramwrite_help(void) {
    printf("Usage: ramwrite -c N -x Reg -s start -e end -f filename [-v]\n");
    printf("Modifies XP200 Channel N content of RAM locations\n");
    printf("from 'start' to 'end' (in hex)\n");
}

static int ramwrite_main(int argc, char **argv) {
    int addr, opt, cno = -1, reg = -1, start = -1, end = -1, i, datasize;
    unsigned long regval;
    unsigned char wdata[4*MAX_DATA_SIZE];
    char *filename = NULL;
    FILE *file;
    int verbose = 0, longformat = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "c:x:s:e:f:lvh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'x':
            reg = strtoul(optarg, NULL, 16);
            break;
        case 's':
            start = (int) strtoul(optarg, NULL, 16);
            break;
        case 'e':
            end = (int) strtoul(optarg, NULL, 16);
            break;
        case 'f':
            filename = optarg;
            break;
        case 'l':
            longformat = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        default:
            print_ramwrite_help();
            return 0;
        }
    }

    hp = hal_acquire(cno - 1);
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        goto out;
    }

    if (reg < 0) {
        printf("Failed: Invalid register location.\n");
        goto out;
    }

    if (start < 0 || end < 0 || start > end) {
        printf("Failed: Invalid start and/or end address.\n");
        goto out;
    }

    if (end - start > MAX_DATA_SIZE) {
        printf("This program can only write up to %d RAM locations\n", 
                MAX_DATA_SIZE);
        goto out;
    }

    if (filename == NULL) {
        print_ramwrite_help();
        goto out;
    }

    if ((file = fopen(filename, "r")) < 0) {
        printf("Can't open input file %s. %m.\n", filename);
        goto out;
    }

    datasize = read_hex_from_file(file, wdata, MAX_DATA_SIZE, longformat);

    fclose(file);

    for (i = 0, addr = start; i < datasize/4 && i <= end - start; i++, addr++) {
        /* Set WR bit to 0   */
        regval = hal_get_fpga_val_k(hp, reg);
        regval &= ~WR_BIT;
        hal_set_fpga_val_k(hp, reg, regval);

        /* Make up content for register 'reg' */
        regval = ((addr & ADDR_MASK) << ADDR_SHIFT) |
                                    (wdata[i*4] << WDATA_SHIFT) | WR_BIT;
        if (verbose)
            printf("Writing 0x%08lx to register 0x%x\n", regval, reg);

        hal_set_fpga_val_k(hp, reg, regval);
        usleep(1);
    }
    printf("\n");

out:
    hal_release(hp);
    return 0;
}

/***************************************************************************
 * Routine to read and write PLX VPD: The VPD is a 28-byte field starting  *
 * with a 4 byte date field (1 byte year# past the year 2000, 1 byte month *
 * of year, 1 byte day of month, 1 byte unused), followed by a 16 byte (a  *
 * string) serial number, followed by a 4 byte hardware revision (a 2-byte *
 * major revision and a 2-byte minor revision). Only the major revison is  *
 * used by the software to decide, e.g. PCI bus width used for reading     *
 * FPGA registers, the minor revision is for display only.                 *
 ***************************************************************************/
static void print_read_plx_vpd_help(void) {
    printf("\nUsage: readplxvpd -c N [-h]\n");
    printf("Displays the contents of the PLX VPD for XP200 Channel N\n");
}

static int read_plx_vpd_main(int argc, char **argv) {
    int i, opt, cno = -1, rc = 0;
    unsigned long serial[VPD_SERIAL_SIZE / 4];
    unsigned long mdate, revision;
    unsigned char ser_str[VPD_SERIAL_SIZE + 1];
    static char *mon_str[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this command.\n");
        return -1;
    }

    while ((opt = getopt(argc, argv, "c:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'h':
        default:
             print_read_plx_vpd_help();
             return 0;
        }
    }

    hp = hal_acquire(cno - 1);
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        goto out;
    }

    /********************
     * Check the PLX VPD *
     ********************/
    if (hal_get_board_vpd(hp, &mdate, serial, &revision) < 0) {
        printf("Failed: NCP disabled or invalid ID\n");
    }
    else {
        int mm, dd, yy;
        yy = (int) VPD_YEAR(mdate) + 2000;
        mm = (int) VPD_MONTH(mdate);
        dd = (int) VPD_DAY(mdate);
        if (mm > 11) mm = 11;
        printf("Manufactured on %d %s %d\n", dd, mon_str[mm], yy);

        memset(ser_str, 0, sizeof(ser_str));
        for (i = 0; i < VPD_SERIAL_SIZE / 4; i++) {
            ser_str[i*4] = (serial[i] & 0xff000000) >> 24;
            ser_str[i*4+1] = (serial[i] & 0x00ff0000) >> 16;
            ser_str[i*4+2] = (serial[i] & 0x0000ff00) >> 8;
            ser_str[i*4+3] = (serial[i] & 0x000000ff);
        }
        printf("Serial Number = %s\n", ser_str);

        printf("Revision = %d.%d\n", VPD_MAJ_REV(revision), 
                        VPD_MIN_REV(revision));
    }

out:
    hal_release(hp);
    return rc;
}

static void print_load_plx_vpd_help(void) {
    printf("\nUsage: loadplxvpd -c N [-m month] [-d day] [-y year] [-R major] [-r minor] [-h] serial\n");
    printf("Loads the VPD data to XP200 Channel N\n");
    printf("'serial' is the serial number for the channel (string upto 16 chars\n");
    printf("The -m, -d and -y switches specify the date of manufacture (defaults to\n");
    printf("current month, day and year)\n");
    printf("The -R switch specifies the major revision (default 2)\n");
    printf("The -r switch specifies the minor revision (default 0)\n");
    printf("The -h switch print this message\n");
} 

static int load_plx_vpd_main(int argc, char **argv) {
    int i, opt, cno = -1, rc = 0;
    unsigned long serial[VPD_SERIAL_SIZE / 4], mdate, revision;
    char ser_str[VPD_SERIAL_SIZE + 1];
    int mm, dd, yy;
    time_t tm;
    struct tm tm_time;
    unsigned short maj_rev = 2, min_rev = 0;
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to load to the PLX EEPROM\n");
        return -1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:m:d:y:R:r:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'm':
            mm = atoi(optarg) - 1;    /* Month since January */    
            break;
        case 'y':
            yy = atoi(optarg);    
            if (yy >= 2000) yy -= 2000;    /* Year since 2000 */
            break;
        case 'd':
            dd = atoi(optarg);        /* Day of month */
            break;
        case 'r':
            min_rev = (unsigned short) atoi(optarg);
            break;
        case 'R':
            maj_rev = (unsigned short) atoi(optarg);
            break;
        case 'h':
        default:
            print_load_plx_vpd_help();
            return 0;
        }
    }

    hp = hal_acquire(cno - 1);
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        rc = -1;
        goto out;
    }

    if (optind != argc - 1) {
        print_load_plx_vpd_help();
        rc = -1;
        goto out;
    }
    
    /*********************
     * Set serial number *
     *********************/
    memset(ser_str, 0, sizeof(ser_str));
    strncpy(ser_str, argv[optind], VPD_SERIAL_SIZE);

    /********************
     * Get current date *
     ********************/
    time(&tm);
    memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
    mm = tm_time.tm_mon;    /* Month since January */
    dd = tm_time.tm_mday;    /* Day of month, from 1 to 31 */
    yy = tm_time.tm_year - 100;    /* Year since 2000 */

    mdate = VPD_DATE(yy, (mm > 11) ? 11 : mm, dd);
    for (i = 0; i < VPD_SERIAL_SIZE / 4; i++) {
        serial[i] = ((unsigned long) ser_str[i * 4]) << 24;
        serial[i] |= ((unsigned long) ser_str[i * 4 + 1]) << 16;
        serial[i] |= ((unsigned long) ser_str[i * 4 + 2]) << 8;
        serial[i] |= ((unsigned long) ser_str[i * 4 + 3]);
    } 
    revision = VPD_REV(maj_rev, min_rev);

    if (hal_set_board_vpd(hp, mdate, serial, revision) < 0) {
        printf("Failed: error writing revision\n");
        rc = -1;
    }

out:
    hal_release(hp);
    return rc;
}

/*********************************************************************
 * Load the PLX-9000 EEPROM from a hex file.                         *
 *                                                                   *
 * The hex file should contain lines of 32-bit hex values (in little *
 * endian) separated by blank characters. The max line size is 128.  *
 *                                                                   *
 * If no file_name is given, the PLX EEPROM will be loaded with the  *         
 * default contents below. (See Sec. 2.4.1 of the PCI 9056RDK-LITE   *
 * Hardware Reference Manual for description of EEPROM content.)     *
 *********************************************************************/
static unsigned long default_plx_eeprom_data[] = {
    0x10b55601, 0x00ba0680, 0x01000000, 0x00000000,
    0x00000000, 0x0000ffc0, 0x00010000, 0x00000120,
    0x85002030, 0x00000000, 0x00000000, 0x00044343,
    0x00000000, 0x00005000, 0x00004000, 0x00000000,
    0x00000000, 0x10b59056, 0x0000ff80, 0x00010040,
    0x00410007, 0x4c060000, 0x00000000, 0x48017a02,
    0x00000000
};

/***************************************
 * Routine to read the PLX-9000 EEPROM *
 ***************************************/
#define DEFAULT_PLX_EEPROM_SIZE         25

static void print_read_plx_eeprom_help(void) {
    printf("\nUsage: readplxeeprom -c N [-h]\n");
    printf("Displays the contents of the PLX EEPROM for XP200 Channel N\n");
}

static int read_plx_eeprom_main(int argc, char **argv) {
    int i, opt, cno = -1;
    unsigned long eeprom_data[MAX_PLX_EEPROM_SIZE / 4];
    HAL *hp;                                                                                         
    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this command.\n");
        return -1;
    }

    while ((opt = getopt(argc, argv, "c:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'h':
        default:
             print_read_plx_eeprom_help();
             return 0;
        }
    }

    hp = hal_acquire(cno - 1);
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno+1);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno+1);
        goto out;
    }

    /***********************
     * Read the PLX EEPROM *
     ***********************/
    for (i = 0; i < sizeof(default_plx_eeprom_data) / 4; i++) {
        eeprom_data[i] = hal_get_plx_eeprom_val_k(hp, i * 4);
    }

    /********************
     * Print on console *
     ********************/
    for (i = 0; i < sizeof(default_plx_eeprom_data) / 4; i++) {
        if (i % 4 != 3)
            printf("%08lx  ", eeprom_data[i]);
        else
            printf("%08lx\n", eeprom_data[i]);
    }
    printf("\n");
 
out:
    hal_release(hp);
    return 0;
}

static void print_load_plx_eeprom_help(void) {
    printf("\nUsage: loadplxeeprom -c N [-f filename] [-l] [-v] [-h]\n");
    printf("Loads the PLX EEPROM on XP200 Channel N from the file 'filename'\n");
    printf("\t-l data in file 'filename' is longhex\n");
    printf("\t-v verify after load\n");
    printf("\t-h print this message\n");
} 

static int load_plx_eeprom_main(int argc, char **argv) {
    int opt, cno = -1, rc = 0, verify = 0, longformat = 0;
    char *name = NULL;
    FILE *file;
    int i, data_size; 
    unsigned long val;
    unsigned char eeprom_data[MAX_PLX_EEPROM_SIZE];
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to load to the PLX EEPROM\n");
        return -1;
    }
   
    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:lf:vh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'v':
            verify = 1;
            break;
        case 'f':
            name = optarg;
            break;
        case 'l':
            longformat = 1;
            break;
        case 'h':
        default:
            print_load_plx_eeprom_help();
            return rc;
        }
    }

    hp = hal_acquire(cno - 1);
                                                                                
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        goto out;
    }

    if (name != NULL) {            /* read EEPROM data from file */
        if ((file = fopen(name, "r")) == NULL) {
            printf("Can't open input file %s. %m.\n", name);
            hal_release(hp); 
            return -1;
        }
        memset(eeprom_data, 0, MAX_PLX_EEPROM_SIZE);
        data_size = read_hex_from_file(file, eeprom_data, 
                            MAX_PLX_EEPROM_SIZE, longformat);
        fclose(file);
        if (data_size <= 0 || data_size > MAX_PLX_EEPROM_SIZE) {
            printf("Failed to read data from input file %s.\n", name);
            goto out;
        } 
    } 
    else {                /* use default data */
        data_size = sizeof(default_plx_eeprom_data);
        memcpy(eeprom_data, default_plx_eeprom_data, data_size);
    }

    for (i = 0; i < data_size / 4; i++) {
        hal_set_plx_eeprom_val_k(hp, i * 4, *(unsigned long*) &eeprom_data[i * 4]);
        usleep(1000);
    }

    if (verify) {
        printf("Verifying eeprom data...");
        for (i = 0; i < data_size / 4 ; i++) {
            val = hal_get_plx_eeprom_val_k(hp, i * 4);
            if (val != *(unsigned long *) &eeprom_data[i * 4]) {
                printf("\nBad value 0x%08lx at addr 0x%02x (should be 0x%08lx)\n",
                    val, i * 4, *(unsigned long *) &eeprom_data[i * 4]);
                rc = -1;
                break;
            }
        } 
    }
    if (rc == 0) printf("OK\n");

out:
    hal_release(hp);
    return rc;
}

/*********************************************************************
 * Routines to load or read the CDCE706/906 PLL Synthesizer EERPOM   *
 *********************************************************************/
#define NUM_CDCE_REGS    27
static unsigned char eeprom_906_data[] = {    /* default CDCE 906 eeprom data */
    0x01, 0x09, 0x2c, 0x60, 0x01, 0x01, 0x00, 0x01,
    0x01, 0x20, 0x20, 0x09, 0x01, 0x20, 0x10, 0x16, 
    0x0b, 0x16, 0x01, 0x38, 0x38, 0x3a, 0x3b, 0x3a,
    0x35, 0x00, 0x1b, 0x00
};
static unsigned char eeprom_706_data[] = {    /* default CDCE 706 eeprom data */
    0x01, 0x01, 0x06, 0x60, 0x01, 0x01, 0x00, 0x01,
    0x01, 0x20, 0x2f, 0x00, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x38, 0x38, 0x3a, 0x3a, 0x3a,
    0x3a, 0x00, 0x1b, 0x00
};

static void print_read_cdce_eeprom_help(void) {
    printf("\nUsage: readcdceeeprom -c N [-7|-9] [-a addr] [-h]\n");
    printf("Displays the contents of the CDCE706 or 906 on XP200 Channel N\n");
    printf("Options: -7 selects CDCE706, -9 selects CDCE906 (default 906),\n");
    printf("         -a addr sets I2C address to 'addr' in Hex (default 0x69)\n");
    printf("         -d turn on I2C debug\n");
    printf("         -h prints this message\n");
    
}

static int read_cdce_eeprom_main(int argc, char **argv) {
    int i, opt, cno = -1, cdce706 = -1, rc = 0, addr = -1, debug = 0;
    unsigned char eeprom_data[NUM_CDCE_REGS], b[1];
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this command.\n");
        return -1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:79a:dh")) > 0) {
        switch (opt) { 
        case 'c':
             cno = atoi(optarg);
             break;
        case 'd':
             debug = 1;
             break;
        case '7':
             cdce706 = 1;
             break;
        case '9':
             cdce706 = 0;
             break;
        case 'a':
             addr = (int) strtoul(optarg, NULL, 16);
             break;
        case 'h':
        default:
             print_read_cdce_eeprom_help();
             return 0;
        }
    }

    if (cdce706 < 0 || addr < 0) {
        printf("Failed: -7 or -9 and an I2C address are required.\n");
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        rc = -1;
        goto out;
    }

    hal_set_i2c_debug(hp, debug);

    /***************************
     * Read the CDCE registers *
     ***************************/
    printf("Reading from %s I2C device at address 0x%02x...\n", 
        (cdce706) ? "Main Board" : "PM", addr);

    for (i = 0; i < NUM_CDCE_REGS; i++) {
        if (cdce706)  
            rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, addr, i, 1, b);
        else
            rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, addr, i, 1, b);
        if (rc < 0) 
            printf("Failed to read register 0x%02x\n", i);
        else 
            eeprom_data[i] = b[0];
    } 

    /********************
     * Print on console *
     ********************/
    for (i = 0; i < NUM_CDCE_REGS;  i++) {
        printf("%02x  ", eeprom_data[i]);
        if (i % 16 == 15) printf("\n");
    }
    printf("\n");

out:
    hal_set_i2c_debug(hp, 0);
    hal_release(hp);
    return rc;
}

/*********************************************************************
 * FILE_HEXTYES are text files with each byte in Hex separated by    *
 * blanks: e.g. "00 02 03 06 08 0a 0c 0e"                            *
 * FILE_LONGHEX are text files with each long word (Big Endian) in   *
 * Hex separated by blanks: e.g. "06030200 0e0c0a08"                 *
 * FILE_CLOCKPRO are TI ClockPro output files starting with the line *
 * "Bit  ==>  76543210" followed by e.g. "Byte 00 - 00000000" etc.   *
 *********************************************************************/
#define FILE_HEXBYTES   0
#define FILE_LONGHEX    1
#define FILE_CLOCKPRO   2

#define MAX_LINE_SIZE   128

static int get_datafile_format(FILE *file) {
    char buf[MAX_LINE_SIZE + 1];
    int rc = -1;

    memset(buf, 0, MAX_LINE_SIZE + 1);

    if (fgets(buf, MAX_LINE_SIZE, file) != NULL) {
        if (isxdigit(buf[0]) && isxdigit(buf[1])) {
            if (buf[2] == ' ')
                rc = FILE_HEXBYTES;
            if (isxdigit(buf[2]) && isxdigit(buf[3]) && isxdigit(buf[4]) && 
        isxdigit(buf[5]) && isxdigit(buf[6]) && isxdigit(buf[7]) && 
        (buf[8] == ' ')) 
                rc = FILE_LONGHEX;
        }
        else if (strncasecmp(buf, "Bit  ==>  76543210", 18) == 0)
            rc = FILE_CLOCKPRO;
    }
    return rc;
}

static int get_binary(int val) {
    int i, retval = 0, divisor = 10000000;
    for (i = 0; i < 8; i++) {
        retval *= 2;
        if (val / divisor) retval += 1;
        val -= (val / divisor) * divisor;
        divisor /= 10;
    }
    return retval;
}

static int read_ti_file(FILE *file, unsigned char *dat, int datsize) {
    char buf[MAX_LINE_SIZE + 1];
    int i, n = 0, bytenum, byteval;

    while (fgets(buf, MAX_LINE_SIZE, file) != NULL) {
        for (i = 0; i < MAX_LINE_SIZE; i++) {
            if (buf[i] == '\n' || buf[i] == '\r') {
                buf[i] = '\0'; break;
            }
        }
        buf[MAX_LINE_SIZE] = '\0';
        if (strncmp(buf, "Byte", 4) != 0)
            continue;
        sscanf(buf, "Byte %d - %d", &bytenum, &byteval);
        dat[n] = get_binary(byteval);
        if (++n >= datsize) break;
    }
    return n;
}

static void print_load_cdce_eeprom_help(void) {
    printf("\nUsage: loadcdceeeprom -c N [OPTION] \n");
    printf("Loads the CDCE 706 or 906 EEPROM on XP200 Channel N\n");
    printf("Mandatory arguments are:\n");
    printf("\t-7|-9\t selects CDCE 706 or CDCE 906\n");
    printf("\t[-a addr]\t use I2C addr 'addr' (in Hex, default 69\n");
    printf("\t-f filename\t load from Hex or TI ClockPro output file 'filename'\n");
    printf("\t-d\t turn on I2C debug\n");
    printf("\t-v\t verify after load\n");
    printf("\t-h\t print this message\n");
} 

static int load_cdce_eeprom_main(int argc, char **argv) {
    int opt, cno = -1, rc = -1, verify = 0, cdce706 = -1, fileformat = -1;
    char *name = NULL;
    FILE *file;
    int i, data_size, addr = -1, debug = 1; 
    unsigned char eeprom_data[NUM_CDCE_REGS], b[1];
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this command.\n");
        return rc;
    }
   
    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:79a:lf:vh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'd':
            debug = 1;
            break;
        case '7':
            cdce706 = 1;
            break;
        case '9':
            cdce706 = 0;
            break;
        case 'a':
            addr = (int) strtoul(optarg, NULL, 16);
            break;
        case 'l':        /* does nothing. obsolete */
            break;
        case 'f':
            name = optarg;
            break;
        case 'v':
            verify = 1;
            break;
        case 'h':
        default:
            print_load_cdce_eeprom_help();
            return rc;
        }
    }

    if (cdce706 < 0 || addr < 0) {
        printf("Failed: -7 or -9 and an I2C address are required.\n");
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        hal_release(hp);
        return -1;
    }

    if (name != NULL) {            /* read EEPROM data from file */
        if ((file = fopen(name, "r")) == NULL) {
            printf("Can't open input file %s. %m.\n", name);
            hal_release(hp);
            return rc;
        }

        if ((fileformat = get_datafile_format(file)) < 0) {
            printf("Unrecognized data file format. Exiting.\n"); 
        }

        memset(eeprom_data, 0, sizeof(eeprom_data));
        if (fileformat == FILE_CLOCKPRO) {
            data_size = read_ti_file(file, eeprom_data, sizeof(eeprom_data));
        }
        else {
            rewind(file);
            data_size = read_hex_from_file(file, eeprom_data, 
                sizeof(eeprom_data), (fileformat == FILE_LONGHEX) ? 1 : 0);
        } 
        fclose(file);
        if (data_size < 0) {
            printf("Failed to read data from input file %s.\n", name);
            hal_release(hp);
            return rc;
        }
    } 
    else if (cdce706) 
        memcpy(eeprom_data, eeprom_706_data, NUM_CDCE_REGS);
    else 
        memcpy(eeprom_data, eeprom_906_data, NUM_CDCE_REGS);

    printf("Writing to %s I2C device at address 0x%02x...\n", 
            (cdce706) ? "Main" : "PM", addr);

    hal_set_i2c_debug(hp, debug);

    for (i = 1; i < NUM_CDCE_REGS; i++) {/* skip register 0 -- it's read only */
        if (i == 25) 
            b[0] = eeprom_data[i] & 0x7f;/* don't lock the EEPROM */
        else
            b[0] = eeprom_data[i]; 
        if (cdce706) 
            rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_WRITE, addr, i, 1, b);
        else
            rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_PM_WRITE, addr, i, 1, b);
        if (rc < 0) {
            printf("Failed to write %s CDCE register 0x%02x\n", 
                    (cdce706) ? "Main" : "PM", i);
            goto out;  
        }
        usleep(1000);
    }
    
    printf("Initiating EEPROM write cycle...\n");
    b[0] = 0x80 | eeprom_data[26];
    if (cdce706)  
        rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_WRITE, addr, 26, 1, b);
    else
        rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_PM_WRITE, addr, 26, 1, b);

    if (rc < 0) {
        printf("Failed to initiate EEPROM write cycle\n");
        goto out;
    }
    for (i = 0; i < 1000; i++) {
        if (cdce706)
            rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, addr, 24, 1, b);
        else
            rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, addr, 24, 1, b);
        if (rc < 0) printf("Failed to read register 0x18\n");
        else if (b[0] & 0x80) usleep(1000);
        else break;
    }
    if (i == 1000) {
        printf("Failed to write to EEPROM. Reg 0x18 = 0x%02x\n", b[0]);
        goto out;
    }

    b[0] = 0x7f & eeprom_data[26];
    if (cdce706)
        rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_WRITE, addr, 26, 1, b);
    else
        rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_PM_WRITE, addr, 26, 1, b);
    if (rc < 0) {
        printf("Failed to end EEPROM write cycle\n");
        goto out;
    }
    rc = 0;
    if (verify) {
        printf("Verifying eeprom data...");
        for (i = 0; i < NUM_CDCE_REGS ; i++) {
            if (cdce706)
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, addr, i, 1, b);
            else
                rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, addr, i, 1, b);
            if (rc < 0 || b[0] != eeprom_data[i]) {
                printf("\nBad value 0x%02x reg. 0x%02x (should be 0x%02x)\n",
                    b[0], i, eeprom_data[i]);
                rc = -1;
                break;
            }
            printf("%02x ", b[0]);
            if (i % 16 == 15) printf("\n");
        } 
        printf("\n");
    }
    if (rc == 0) printf("OK\n");

out:
    hal_set_i2c_debug(hp, 0);

    hal_release(hp);

    return rc;
}

/*********************************************************************
 * Load the MicroChip 24LC21 EEPROM for the USB controller on the    *
 * Personality Module from a hex file.                               *
 *                                                                   *
 * The hex file should contain lines of 32-bit hex values (in little *
 * endian) separated by blank characters. The max line size is 128.  *
 *                                                                   *
 * If no file_name is given, the EEPROM will be loaded with the      *
 * default given below.                                              *
 *********************************************************************/
#define USB_BYTES_PER_PAGE      8

static unsigned long default_24lc21_data[] = {
    0x04ccaa55, 0x02001520, 0x0304ff00, 0x031e0409,
    0x00500049, 0x00690056, 0x00650064, 0x0020006f,
    0x00790053, 0x00740073, 0x006d0065, 0x0056030e,
    0x00440032, 0x00350031, 0x033a0030, 0x00500046,
    0x00410047, 0x00620020, 0x0061006f, 0x00640072, 
    0x00310020, 0x003d0020, 0x00770020, 0x00720069, 
    0x00640065, 0x00730020, 0x00700075, 0x006f0070, 
    0x00740072, 0xffffffff, 0xffffffff, 0xffffffff,
};

static void print_read_usb_eeprom_help(void) {
    printf("\nUsage: readusbeeprom -c N [-d] [-h]\n");
    printf("Displays the contents of the USB EEPROM for XP200 Channel N\n");
}


static int read_usb_eeprom_main(int argc, char **argv) {
    int i, opt, cno = -1, rc = 0, debug = 0;
    unsigned char eeprom_data[USB_EEPROM_SIZE]; 
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this command.\n");
        return -1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:h")) > 0) {
        switch (opt) { 
        case 'c':
            cno = atoi(optarg);
            break;
        case 'd':
            debug = 1;
            break;
        case 'h':
        default:
            print_read_usb_eeprom_help();
            return 0;
        }
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        hal_release(hp);
        return -1;
    }

    hal_set_i2c_debug(hp, debug);

    /*****************************************
     * Switch to port 3 on the PM I2C switch * 
     *****************************************/
    hal_set_pca9545a_val_k(hp, I2C_REG_NONE, 0x08);

    /*****************************************
     * Read the USB EEPROM a page at a time  *
     *****************************************/
    for (i = 0; i < USB_EEPROM_SIZE; i += USB_BYTES_PER_PAGE) {
        rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, USB_EEPROM_I2C_ADDR, 
                i, USB_BYTES_PER_PAGE, &eeprom_data[i]);
        if (rc < 0) {
            printf("Failed to read USB_EEPROM register 0x%02x\n", i); 
            goto out;
        }
        usleep(1000);
    }

    /******************************************
     * Don't let PM I2C switch stay on port 3 * 
     ******************************************/
    hal_set_pca9545a_val_k(hp, I2C_REG_NONE, 0x04);

    for (i = 0; i < USB_EEPROM_SIZE/4; i++) {
        if (i % 4 != 3)
            printf("%08lx  ", *((unsigned long *) &eeprom_data[4 * i]));
        else
            printf("%08lx\n", *((unsigned long *) &eeprom_data[4 * i]));
    }
    printf("\n");

out:
    hal_set_i2c_debug(hp, 0);
    hal_release(hp);
    return 0;
}

static void print_load_usb_eeprom_help(void) {
    printf("\nUsage: loadusbeeprom -c N [-f filename] [-s M] [-l] [-d] [-h]\n");
    printf("Load the USB EEPROM for XP200 Channel N from file 'file'\n");
    printf("if the -f option is present otherwise load default content.\n");
    printf("The -s option uses 'M' instead of 'N' as Channel # in EEPROM)\n");
    printf("The -d option turns on I2C debug\n");
    printf("The -h option prints this message\n");
}

static int load_usb_eeprom_main(int argc, char **argv) {
    int opt, cno = -1, data_size, i, longformat, debug = 0;
    int serial_cno = -1;
    char *name = NULL;
    FILE *file;
    unsigned char eeprom_data[USB_EEPROM_SIZE];
    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to run this command.\n");
        return -1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:f:s:dh")) > 0) {
        switch (opt) { 
        case 'c':
            cno = atoi(optarg);
            break;
        case 'f':
            name = optarg;
            break;
        case 's':
            serial_cno = atoi(optarg);
            break;
        case 'd':
            debug = 1;
            break;
        case 'h':
        default:
             print_load_usb_eeprom_help();
             return -1;
        }
    }

    if (serial_cno == -1) {
        serial_cno = cno - 1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno+1);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno+1);
        hal_release(hp);
        return -1;
    }

    if (name != NULL) {            /* read EEPROM data from file */
        if ((file = fopen(name, "r")) == NULL) {
            printf("Can't open input file %s. %m.\n", name);
            hal_release(hp);
            return -1;
        }

        longformat = (get_datafile_format(file) == FILE_LONGHEX) ? 1 : 0;

        data_size = read_hex_from_file(file, eeprom_data, 
                                    USB_EEPROM_SIZE, longformat);
        fclose(file);
        if (data_size != USB_EEPROM_SIZE) {
            printf("Failed to read data from input file %s.\n", name);
            hal_release(hp);
            return -1;
        } 
    } 
    else {
        /* Copy the default 24lc21 data */
        memcpy(eeprom_data, default_24lc21_data, USB_EEPROM_SIZE);

        /* Set channel number byte in serial number string */
        eeprom_data[82] = serial_cno + '0';
    }

    printf("Writing %d bytes to USB EEPROM:\n", USB_EEPROM_SIZE);

    for (i = 0; i < USB_EEPROM_SIZE/4; i++) {
        if (i % 4 != 3)
            printf("%08lx  ", *((unsigned long *) &eeprom_data[4 * i]));
        else
            printf("%08lx\n", *((unsigned long *) &eeprom_data[4 * i]));
    }
    printf("\n");

    hal_set_i2c_debug(hp, debug);

    /************************************
     * Hold USB Hub in reset. This will *
     * set the PM I2C switch to port 2. *
     ************************************/
    hal_set_pm_pca9555_val_k(hp, 3, 0xde);
    hal_set_pm_pca9555_val_k(hp, 7, 0xde);

    /*****************************************
     * Switch to port 3 on the PM I2C switch * 
     *****************************************/
    hal_set_pm_pca9555_val_k(hp, I2C_REG_NONE, 0x08);

    /*****************************************
     * Write the USB EEPROM a page at a time *
     *****************************************/
    for (i = 0; i < USB_EEPROM_SIZE; i += USB_BYTES_PER_PAGE) {
        int rc;
        rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_PM_WRITE, USB_EEPROM_I2C_ADDR, 
                i, USB_BYTES_PER_PAGE, &eeprom_data[i]);
        if (rc < 0) {
            printf("Failed to write to USB EEPROM register 0x%02x\n", i); 
            goto out;
        }
        usleep(1000);
    }

    /*************************************************
     * Take USB Hub out of reset. Do this last since * 
     * this will set the PM I2C switch to port 2.    *
     *************************************************/
    hal_set_pm_pca9555_val_k(hp, 3, 0xdf);

out:
    hal_set_i2c_debug(hp, 0);
    hal_release(hp);

    return 0;
}

/****************************************************
 * Serial test - Reads serial data from FTDI and    *
 * writes to a file.                                * 
 ****************************************************/
static void print_testserial_help(void) {
    printf("\nUsage: testserial -c N [OPTIONS]\n");
    printf("Reads serial data from FTDI on XP200 Channel N & writes it out.\n\n");
    printf("Options:\n");
    printf("\t-n Num\t Read \"Num\" bytes (default 100000).\n");
    printf("\t-B Baud\t Set baudrate to \"Baud\" (default 9600)\n");
    printf("\t-f [n|x|h]\t Set flowcontrol to none, sw or hw (default none)\n");
    printf("\t-F File\t Write output to \"File\" (instead of stdout).\n");
    printf("\n");
}

static int testserial_main(int argc, char **argv) {
    int cno = -1, opt, rc = 0, serfd, ofd = -1, max_fd, num = 100, nd;
    unsigned char buff[257];
    char *outfile = NULL;
    fd_set read_fdset;
    struct timeval timeout;
    unsigned long total = 0;
    int baud = 9600;
    char flowcontrol = 'n';
    HAL *hp;

    while ((opt = getopt(argc, argv, "c:B:n:f:F:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'n' :
            num = atoi(optarg);
            break;
        case 'B':
            baud = atoi(optarg);
            if (baud != 300 || baud != 600 || baud != 1200 ||
                baud != 2400 || baud != 4800 || baud != 9600 ||
                baud != 19200 || baud != 38400 || baud != 57600 ||
                baud != 115200)
                baud = 9600;    
            break;
        case 'f': 
            flowcontrol = tolower(*optarg);
            if (flowcontrol != 'n'  && flowcontrol != 'x' && flowcontrol != 'h')
                flowcontrol = 'n';
            break;
        case 'F':
            outfile = optarg;        
            break;
        case 'h': 
        default:
            print_testserial_help();;
            return 0;
        }   
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid Channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        hal_release(hp);
        return -1;
    }

    printf("Stopping the Streaming Server...\n");
    system_command("/bin/strmsrvctl stop");

    if ((serfd = hal_get_serial_fd(hp)) < 0) {
        printf("Failed to open serial device. %m.\n");
        hal_release(hp);
        return -1;
    }
        
    printf("Channel %d: opened serial fd %d\n", cno, serfd);

    if (outfile) {
        ofd = open(outfile, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH);
        if (ofd < 0) {
            printf("Failed to open file %s. %m.", outfile);
            hal_release(hp);
            return 0;
        }
    }
    printf("Setting serial port to %d baud, flow control %s\n", baud,
         (flowcontrol == 'n') ? "none" : ((flowcontrol == 'x') ? "sw" : "hw"));

    set_serial_parameters(serfd, baud, 8, 1, 'n', flowcontrol);

    printf("Reading %d bytes from serial port.\n", num);
    total = 0;

    while (total < num) {
        FD_ZERO(&read_fdset);
        max_fd = -1;
        FD_SET(serfd, &read_fdset);
        max_fd = serfd;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        nd = select(max_fd + 1, &read_fdset, NULL, NULL, &timeout);
        if (nd < 0) {
            if (errno == EINTR)
                continue;
            printf("Select returned with error %m\n");
            break;
        }
        else if (nd == 0) {
            //printf("Channel %d: Select timed out\n", cno);
            continue;
        }

        if (FD_ISSET(serfd, &read_fdset)) {
            rc = read(serfd, buff, 256);
            if (rc < 0) {
                printf("Serial read error %d\n", rc);
                break;
            }
            else if (rc == 0) continue;

            if (ofd > 0) {
                if (write(ofd, buff, rc) < 0) {
                    printf("Failed to write to file %s. %m.\n", outfile);
                    break;
                }
            }
            else {
                buff[rc] = 0;
                puts((char *) buff);
            }
        }
        total += rc;
    }

    if (ofd > 0) close(ofd);

    hal_release(hp);

    return 0;
}

/****************************************************
 * Audio test - Reads the FPGA audio read FIFO then *
 * writes to the FPGA audio write FIFO.             * 
 ****************************************************/
#define    ABUF_SIZE    1040 * 256

static void print_testaudio_help(void) {
    printf("Usage: testaudio -c N [-h]\n");
    printf("Loops back audio on XP200 Channel N\n");
}

static void async_testaudio(int afd) {
    int len, nd, max_fd = -1;
    unsigned char abuf[ABUF_SIZE];
    unsigned long read_len = 0, write_len = 0, read_offset, write_offset;
    unsigned long read_cnt = 0, write_cnt = 0;
    fd_set read_fdset, write_fdset;
    struct timeval start, elapsed, timeout;
    float bitrate;

    gettimeofday(&start, (struct timezone *) 0);
    read_offset = 0; write_offset = 16;

    while (1) {
        FD_ZERO(&read_fdset);
        FD_ZERO(&write_fdset);
        max_fd = -1;

        FD_SET(afd, &read_fdset);
        FD_SET(afd, &write_fdset);
        if (afd > max_fd) max_fd = afd;

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        nd = select(max_fd + 1, &read_fdset, &write_fdset, NULL, &timeout);

        if (nd < 0) {
            if (errno == EINTR) {
                continue;
            }
            printf("Select returned with error %m.\n");
            break;
        }
        else if (nd == 0) {
            //printf("Select timed out.\n");
            continue;
        }
        
        if (FD_ISSET(afd, &read_fdset)) {
            if ((len = read(afd, &abuf[read_offset], 1040)) != 1040) {
                printf("Failed to read audio device. %m.\n");
                break;
            }
            read_cnt++; read_offset += 1040; read_len += 1024;
            if (read_offset >= ABUF_SIZE) read_offset = 0;    
        }

        if (read_cnt > 10 && write_cnt < read_cnt) {

            if (FD_ISSET(afd, &write_fdset)) {
                if ((len = write(afd, &abuf[write_offset], 1024)) != 1024) {
                    printf("Failed to write audio device. %m.\n");
                    break;
                }
                write_cnt++; write_offset += 1040; write_len += 1024;
                if (write_offset >= ABUF_SIZE) write_offset = 0;    
            }
        }

        gettimeofday(&elapsed, (struct timezone *) 0);
        if (elapsed.tv_usec < start.tv_usec) {
            elapsed.tv_usec += 1000000;
            --elapsed.tv_sec;
        }  
        elapsed.tv_usec = (elapsed.tv_sec - start.tv_sec) * 1000000 +
        (elapsed.tv_usec - start.tv_usec);
        if (elapsed.tv_usec >= 1000000) {
            bitrate = ((float) read_len * 8) / elapsed.tv_usec;
            printf("Read bit rate = %f Mbps (read cnt %ld).\n", 
                    bitrate, read_cnt);
            read_len = 0; 
            bitrate = ((float) write_len * 8) / elapsed.tv_usec;
            printf("Write Bit rate = %f Mbps (write cnt %ld).\n", 
                    bitrate, write_cnt);
            write_len = 0; 
            gettimeofday(&start, (struct timezone *) 0);
        }
    }

}

static int testaudio_main(int argc, char **argv) {
    int opt, cno = -1, afd;
    HAL *hp;

    /*****************
     * Parse Options *
     *****************/
    while ((opt = getopt(argc, argv, "c:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'h':
        default:
            print_testaudio_help();
            return 0;
        }
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        hal_release(hp);
        return -1;
    }

    if (!is_standalone_enabled()) {
        printf("Stopping the Streaming Server...\n");
        system_command("/bin/strmsrvctl stop");
    }
    else {
        printf("Resetting standalone mode...\n");
        system_command("/V2O/bin/standalone -r");
    }

    afd = hal_get_audio_fd(hp);

    if (afd < 0) {
        printf("Channel %d: Failed to open audio device. %m.\n", cno);
        return -1;
    } 

    printf("Waiting for audio input on FD %d...\n", afd);

    async_testaudio(afd);

    hal_release(hp);
   
    return 0;
}

/***********************************************
 * Test routines to set I50 & XP DDS frequency * 
 ***********************************************/
static void dump_dds_registers(HAL *hp) 
{
    printf("DDS_FREQ_WORD1_01 = 0x%02x\n", 
            hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_01));
    printf("DDS_FREQ_WORD1_02 = 0x%02x\n", 
            hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_02));
    printf("DDS_FREQ_WORD1_03 = 0x%02x\n", 
            hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_03));
    printf("DDS_FREQ_WORD1_04 = 0x%02x\n", 
            hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_04));
    printf("DDS_FREQ_WORD1_05 = 0x%02x\n", 
            hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_05));
    printf("DDS_FREQ_WORD1_06 = 0x%02x\n", 
            hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_06));
}

static double get_pixel_clock_from_dds(HAL *hp) {
    double ddsfreq, ffactor;
    unsigned int vf_hi, vf_lo;

    if (hal_get_hardware_type(hp) == PLX_DEVICE)
        ffactor = V4_DDSFFACTOR;
    else
        ffactor = V2_DDSFFACTOR;

    vf_hi  = (unsigned int)hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_01) << 16;
    vf_hi += (unsigned int)hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_02) << 8;
    vf_hi += (unsigned int)hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_03);
    vf_lo  = (unsigned int)hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_04) << 16;
    vf_lo += (unsigned int)hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_05) << 8;
    vf_lo += (unsigned int)hal_get_ad9852_val_k(hp, DDS_FREQ_WORD1_06);

    ddsfreq = (double) vf_hi * TWORAISED24 + (double) vf_lo;

    return (ddsfreq * ffactor / TWORAISED48);
}

static int getdds_main(int argc, char **argv) {
    int opt, cno = -1, is_i50 = 0, verbose = 0;
    HAL *hp;

    /*******************
     * Allow only root *
     *******************/
    if (getuid() != 0)  {
        printf("You must be root to execute this command.\n");
        return 0;
    }
 
    hp = hal_acquire(0);

    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:vh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            if (is_i50 && cno != 1) {
                printf("Invalid option -c.\n");
                if (hp) hal_release(hp);
                return -1;
            }
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h': 
        default:
            printf("Usage: getdds [-v] [-c cno]\n");
            if (hp) hal_release(hp);
            return -1;
        }   
    }

    if (optind != argc) {
        printf("Usage: getdds [-v] [-c cno]\n");
        if (hp) hal_release(hp);
        return -1;
    }

    if (!is_i50) {
        if (cno < 1 || cno > MAX_FPGA_BOARDS) {
            printf("Failed: Invalid channel number (%d).\n", cno);
            if (hp) hal_release(hp);
            return -1;
        }
        if (cno != 1) {
            if (hp) hal_release(hp);
            hp = hal_acquire(cno - 1);
            if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE) {
                printf("Channel %d. Failed to open FPGA device\n", cno);
                if (hp) hal_release(hp);
                return -1;
            }
        }
    }

    if (hp == NULL) {
        printf("Failed to open FPGA device\n");
        return -1;
    }

    /* Get current DDS output frequency */
    if (verbose) {
        if (is_i50)
            printf("Current DDS registers:\n");
        else
            printf("Current Channel %d DDS registers:\n", cno);
        dump_dds_registers(hp);
    }

    printf("Current Pixel Clock = %7.2f\n", get_pixel_clock_from_dds(hp));

    hal_release(hp);

    return 0;
}

static int setdds_main(int argc, char **argv) {
    int i, opt, cno = -1, is_i50 = 0, verbose = 0;
    double ddsfreq, ffactor = V4_DDSFFACTOR, pixclk = (double) -1;
    unsigned int vf_hi, vf_lo;
    HAL *hp;

    /*******************
     * Allow only root *
     *******************/
    if (getuid() != 0)  {
        printf("You must be root to execute this command.\n");
        return 0;
    }
 
    hp = hal_acquire(0);

    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
        ffactor = V2_DDSFFACTOR;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:p:vh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            if (is_i50 && cno != 1) {
                printf("Invalid option -c.\n");
                if (hp) hal_release(hp);
                return -1;
            }
            break;
        case 'p':
            pixclk = atof(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h': 
        default:
            printf("Usage: setdds [-v] -p pixclk [-c cno]\n");
            if (hp) hal_release(hp);
            return -1;
        }   
    }

    if (optind != argc) {
        printf("Usage: setdds [-v] -p pixclk [-c cno]\n");
        if (hp) hal_release(hp);
        return -1;
    }

    if (!is_i50) {
        ffactor = V4_DDSFFACTOR;
        if (cno < 1 || cno > MAX_FPGA_BOARDS) {
            printf("Failed: Invalid channel number (%d).\n", cno);
            if (hp) hal_release(hp);
            return -1;
        }
        if (cno != 1) {
            hal_release(hp);
            hp = hal_acquire(cno - 1);
            if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE) {
                printf("Channel %d. Failed to open FPGA device\n", cno);
                if (hp) hal_release(hp);
                return -1;
            }
        }
    }
 
    if (hp == NULL) {
        printf("Failed to open FPGA device\n");
        return -1;
    }
 
    if (pixclk < (double) 0) {
        printf("Usage: setdds [-v] -p pixclk -c cno\n");
        return -1;
    }

    /* reset dds */
    hal_or_in_fpga_val_k(hp, FPGA_PDDSREG, PDDSREG_RESET);

    /* delay 100 usec */
    for (i = 0; i < 10000; i++);

    /* complete reset */
    hal_and_in_fpga_val_k(hp, FPGA_PDDSREG, ~PDDSREG_RESET);
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_03, 0x00);

    /* provision UPD as an output clock */
    hal_or_in_fpga_val_k(hp, FPGA_PDDSREG, PDDSREG_P_UDCK); 

    /* initialize dds */
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_01, 0x00);
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_02, 0x49);
    hal_set_ad9852_val_k(hp, DDS_CNTL_REG_04, 0x60);
    hal_set_ad9852_val_k(hp, DDS_OSK_MULT_01, 0x0F);
    hal_set_ad9852_val_k(hp, DDS_OSK_MULT_02, 0xFF);
    hal_set_ad9852_val_k(hp, DDS_CNTL_DAC_01, 0x2F);
    hal_set_ad9852_val_k(hp, DDS_CNTL_DAC_02, 0xFF);

    /* set DDS clock frequency values */
    if (!is_i50)
        hal_set_pca9555_clk_val_k(hp, 0x06, 0xe0);

    if (verbose) {
        if (is_i50)
            printf("Original DDS registers:\n");
        else
            printf("Original Channel %d DDS registers:\n", cno);
        dump_dds_registers(hp);
    }

    /* Compute frequency word equivalent for 'dval' divided by 2 */
    ddsfreq = pixclk / (double) 2 / ffactor * TWORAISED48;
    vf_hi = ddsfreq / TWORAISED24;
    vf_lo = ddsfreq - ((double) vf_hi * TWORAISED24);

    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_01,(vf_hi >> 16) & 0x7f);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_02,(vf_hi >>  8) & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_03,(vf_hi      ) & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_04,(vf_lo >> 16) & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_05,(vf_lo >>  8)  & 0xff);
    hal_set_ad9852_val_k(hp, DDS_FREQ_WORD1_06, vf_lo        & 0xff);

    hal_or_in_fpga_val_k(hp, FPGA_PDDSREG, PDDSREG_P_UDCK | PDDSREG_UDCK); 
    for (i = 0; i < 10000; i++);    /* delay 100 us */
    hal_and_in_fpga_val_k(hp, FPGA_PDDSREG, ~PDDSREG_UDCK); 

    usleep(1000);

    if (verbose) {
        if (is_i50)
            printf("Current DDS registers:\n");
        else
            printf("Current Channel %d DDS registers:\n", cno);
        dump_dds_registers(hp);
        printf("Current Pixel Clock = %7.2f\n", get_pixel_clock_from_dds(hp));
    }

    hal_release(hp);

    return 0;
}

/*******************************************************************
 * Routines to test I50 & XP200 Progressive Compression parameters * 
 *******************************************************************/
static int getpcparams_main(int argc, char **argv) {
    int opt, cno = -1, is_i50 = 0;
    struct pc_param_io pcpio;
    HAL *hp;

    /*******************
     * Allow only root *
     *******************/
    if (getuid() != 0)  {
        printf("You must be root to execute this command.\n"); 
        return 0;
    }
 
    hp = hal_acquire(0);

    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:h")) > 0) {
        switch (opt) {
        case 'c':
            if (is_i50 && cno != 1) {
                printf("Invalid option -c.\n");
                if (hp) hal_release(hp);
                return -1;
            }
            cno = atoi(optarg);
            break;
        case 'h': 
        default:
            printf("Usage: getpcparams [-c cno]\n");
            if (hp) hal_release(hp);
            return -1;
        }   
    }

    if (optind != argc) {
        printf("Usage: getpcparams [-c cno]\n");
        if (hp) hal_release(hp);
        return -1;
    }

    if (!is_i50) {
        if (cno < 1 || cno > MAX_FPGA_BOARDS) {
            printf("Failed: Invalid channel number (%d).\n", cno);
            if (hp) hal_release(hp);
            return -1;
        }
        if (cno != 1) {
            if (hp) hal_release(hp);
            hp = hal_acquire(cno - 1);
            if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE) {
                printf("Channel %d. Failed to open FPGA device\n", cno);
                if (hp) hal_release(hp);
                return -1;
            }
        }
    }

    if (hp == NULL) {
        printf("Failed to open FPGA device\n");
        return -1;
    }

    if (hal_get_pc_params(hp, &pcpio) < 0) {
        printf("Channel %d Failed to get progressive compression params\n",
                hal_get_channel_number(hp));
    }
    else {
        printf("Channel %d\t\tXBLOCKS = %d\n", hal_get_channel_number(hp),
                pcpio.pc_xblocks);
        printf("YLOW = %d\t\tYHIGH = %d\n", pcpio.pc_ylow, pcpio.pc_yhigh);
        printf("RLOW = %d\t\tRHIGH = %d\n", pcpio.pc_rlow, pcpio.pc_rhigh);
        printf("FPGA reg 0x%x = 0x%x\n", FPGA_ALATCONTROL,
                hal_get_fpga_val_k(hp, FPGA_ALATCONTROL));
        printf("FPGA reg 0x%x = 0x%x\n", FPGA_AISLICEREG, 
                hal_get_fpga_val_k(hp, FPGA_AISLICEREG));
        printf("FPGA reg 0x%x = 0x%x\n", FPGA_ARSLICEREG, 
                hal_get_fpga_val_k(hp, FPGA_ARSLICEREG));
    }

    hal_release(hp);

    return 0;
}

static int setpcparams_main(int argc, char **argv) {
    int opt, cno = -1, is_i50 = 0, verbose = 0;
    int latency = -1, xblocks = -1;
    int rhigh = -1, rlow = -1, yhigh = -1, ylow = -1;
    struct pc_param_io pcpio;
    HAL *hp;

    /*******************
     * Allow only root *
     *******************/
    if (getuid() != 0)  {
        printf("You must be root to execute this command.\n");
        return 0;
    }
 
    hp = hal_acquire(0);

    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:x:r:R:y:Y:l:vh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            if (is_i50 && cno != 1) {
                printf("Invalid option -c.\n");
                if (hp) hal_release(hp);
                return -1;
            }
            break;
        case 'x':
            xblocks = atoi(optarg);
            break;
        case 'r':
            rlow = atoi(optarg);
            break;
        case 'R':
            rhigh = atoi(optarg);
            break;
        case 'y':
            ylow = atoi(optarg);
            break;
        case 'Y':
            yhigh = atoi(optarg);
            break;
        case 'l':
            latency = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h': 
        default:
            printf("Usage: getpcparams [-c cno]\n");
            if (hp) hal_release(hp);
            return -1;
        }   
    }

    if (optind != argc) {
        printf("Usage: getpcparams [-c cno]\n");
        if (hp) hal_release(hp);
        return -1;
    }

    if (!is_i50) {
        if (cno < 1 || cno > MAX_FPGA_BOARDS) {
            printf("Failed: Invalid channel number (%d).\n", cno);
            if (hp) hal_release(hp);
            return -1;
        }
        if (cno != 1) {
            if (hp) hal_release(hp);
            hp = hal_acquire(cno - 1);
            if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE) {
                printf("Channel %d. Failed to open FPGA device\n", cno);
                if (hp) hal_release(hp);
                return -1;
            }
        }
    }

    if (hp == NULL) {
        printf("Failed to open FPGA device\n");
        return -1;
    }

    if (verbose) {
        if (hal_get_pc_params(hp, &pcpio) < 0) {
            printf("Channel %d Failed to get progressive compression params\n",
                hal_get_channel_number(hp));
        }
        else {
            printf("Channel %d\t\tXBLOCKS = %d\n", hal_get_channel_number(hp),
                pcpio.pc_xblocks);
            printf("YLOW = %d\t\tYHIGH = %d\n", pcpio.pc_ylow, pcpio.pc_yhigh);
            printf("RLOW = %d\t\tRHIGH = %d\n", pcpio.pc_rlow, pcpio.pc_rhigh);
            printf("FPGA reg 0x%x = 0x%x\n", FPGA_ALATCONTROL,
                hal_get_fpga_val_k(hp, FPGA_ALATCONTROL));
            printf("FPGA reg 0x%x = 0x%x\n", FPGA_AISLICEREG, 
                hal_get_fpga_val_k(hp, FPGA_AISLICEREG));
            printf("FPGA reg 0x%x = 0x%x\n", FPGA_ARSLICEREG, 
                hal_get_fpga_val_k(hp, FPGA_ARSLICEREG));
        }
    }

    memset(&pcpio, 0, sizeof(struct pc_param_io));

    if (xblocks >= 1 && xblocks <= 32) {
        pcpio.setmask |= PC_SET_PARAM_XBLOCKS;
        pcpio.pc_xblocks = xblocks;
    }
    if (rlow >= 0 && rlow <= 255) {
        pcpio.setmask |= PC_SET_PARAM_RLOW;
        pcpio.pc_rlow = rlow;
    }
    if (rhigh >= 0 && rhigh <= 255) {
        pcpio.setmask |= PC_SET_PARAM_RHIGH;
        pcpio.pc_rhigh = rhigh;
    }
    if (ylow >= 0 && ylow <= 255) {
        pcpio.setmask |= PC_SET_PARAM_YLOW;
        pcpio.pc_ylow = ylow;
    }
    if (yhigh >= 0 && yhigh <= 255) {
        pcpio.setmask |= PC_SET_PARAM_YHIGH;
        pcpio.pc_yhigh = yhigh;
    }
    if (latency >= 0) {
        int numslices = hal_get_fpga_val_k(hp, FPGA_ANSLICEREG);
        struct LUT_struct lut;
        unsigned long framesize, thres0, thres1, thres2;
        double maxframerate, inputframerate;

        if (hal_get_addside_lut(hp, &lut) < 0) {
            printf("Channel %d. Failed to get LUT. Can't set latency control\n",
                    hal_get_channel_number(hp)); 
            return -1;
        }
        framesize = (unsigned long) lut.a_hres * lut.a_vres;
        maxframerate = (double) 100000000 / (double) framesize;
        inputframerate = lut.a_refresh;
        if (maxframerate >= inputframerate) {
            thres0 = framesize / 32;
            thres1 = framesize / 16;
            thres2 = framesize * 3 / 32;
        }
        else {
            double divisor = maxframerate + inputframerate;
            thres0 = 
                (unsigned long) (maxframerate/divisor*(double)(framesize/16));
            thres1 = framesize / 16;
            thres2 = framesize / 8 - thres0;
        }

        hal_set_fpga_val_k(hp, FPGA_ALATTHRES0, thres0 | 3);
        hal_set_fpga_val_k(hp, FPGA_ALATTHRES1, thres1 | 3);
        hal_set_fpga_val_k(hp, FPGA_ALATTHRES2, thres2 | 3);
        hal_set_fpga_val_k(hp, FPGA_ALATCONTROL, 0);
        hal_set_fpga_val_k(hp, FPGA_ALATSLICE4, 0);
        hal_set_fpga_val_k(hp, FPGA_ALATSLICE5, 0);

        if (latency > 0) {
            hal_set_fpga_val_k(hp, FPGA_ALATSLICE0, numslices / 4 - 1);
            hal_set_fpga_val_k(hp, FPGA_ALATSLICE1, numslices / 2 - 1);
            hal_set_fpga_val_k(hp, FPGA_ALATSLICE3, numslices * 3 / 4 - 1);
            hal_set_fpga_val_k(hp, FPGA_ALATCONTROL, 0xff);
        }
        else {
            hal_set_fpga_val_k(hp, FPGA_ALATSLICE0, 0x00);
            hal_set_fpga_val_k(hp, FPGA_ALATSLICE1, 0x00);
            hal_set_fpga_val_k(hp, FPGA_ALATSLICE2, 0x00);
        }
    }

    if (verbose) {
        if (hal_get_pc_params(hp, &pcpio) < 0) {
            printf("Channel %d Failed to get progressive compression params\n",
                hal_get_channel_number(hp));
        }
        else {
            printf("New XBLOCKS = %d\n", pcpio.pc_xblocks);
            printf("YLOW = %d\t\tYHIGH = %d\n", pcpio.pc_ylow, pcpio.pc_yhigh);
            printf("RLOW = %d\t\tRHIGH = %d\n", pcpio.pc_rlow, pcpio.pc_rhigh);
            printf("FPGA reg 0x%x = 0x%x\n", FPGA_ALATCONTROL,
                hal_get_fpga_val_k(hp, FPGA_ALATCONTROL));
            printf("FPGA reg 0x%x = 0x%x\n", FPGA_AISLICEREG, 
                hal_get_fpga_val_k(hp, FPGA_AISLICEREG));
            printf("FPGA reg 0x%x = 0x%x\n", FPGA_ARSLICEREG, 
                hal_get_fpga_val_k(hp, FPGA_ARSLICEREG));
        }
    }

    hal_release(hp);

    return 0;
}

/****************************************************
 * PS2 keyboard and Mouse test - Reads PS2 keyboard *
 * and mouse then writes the data to the FPGA       * 
 ****************************************************/
static void print_testps2_help(void) {
    printf("Usage: testps2 -c N [OPTIONS]\n");
    printf("Loops back PS2 keyboard and mouse data on XP200 Channel N\n");
    printf("Options:\n");
    printf("\t-k pc|sgi\t Set keyboard type to PC or SGI (default PC)\n");
    printf("\t-m ps2|imps2\t Set mouse type to PS2 or IMPS2 (default PS2)\n");
    printf("\t-w Don't write keyboard and mouse data to output.\n");  
    printf("\t-v Turn on verbose mode.\n");
    printf("\n");
}

static void print_hexdata(unsigned char *dat, int len) {
   int i = 0, j = 0;
   unsigned char c1, c2;
   char buff[256];
                                                                                
   for (i = j = 0; i < len; i++) {
       c1 = dat[i] / 16;
       c2 = dat[i] % 16;
       buff[j++] = (c1 < 10) ? (c1 + '0') : (c1 - 10 + 'a');
       buff[j++] = (c2 < 10) ? (c2 + '0') : (c2 - 10 + 'a');
       if (j < 23)
           buff[j++] = ' ';
       else {
           buff[j] = '\0'; puts(buff); j = 0;
       }
   }
   if (j > 0) {
       buff[j] = '\0'; puts(buff);
   }
}

static int testps2_main(int argc, char **argv) {
    int opt, cno = -1, verbose = 0, dontwrite = 0, kb_len, ms_len;
    int kb_roffset, kb_woffset, ms_roffset, ms_woffset;
    int len, kbfd, msfd, nd, max_fd = 0;
    fd_set read_fdset;
    struct timeval timeout;
    unsigned char kbbuf[1024], msbuf[1024], val;
    int mstype = MS_TYPE_IMPS2;
    int kbtype = KB_TYPE_PC;
    HAL *hp;
 
    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "c:k:m:wvh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'm': 
            if (strcasecmp(optarg, "ps2") == 0) 
                mstype =  MS_TYPE_PS2;
            break;
        case 'k': 
            if (strcasecmp(optarg, "sgi") == 0) 
                kbtype = KB_TYPE_SGI;
            break;
        case 'w':
            dontwrite = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h': 
        default:
            print_testps2_help();
            return 0;
        }   
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Channel %d not an XP200.\n", cno);
        hal_release(hp);
        return -1;
    }


    if (!is_standalone_enabled()) {
        printf("Stopping the Streaming Server...\n");
        system_command("/bin/strmsrvctl stop");
    }
    else {
        printf("Resetting standalone mode...\n");
        system_command("/V2O/bin/standalone -r");
    }

    if ((kbfd = hal_get_keyboard_fd(hp)) == -1) {
         printf("Failed to open keyboard device. %m.\n");
         hal_release(hp);
         return -1;
    }

    if ((msfd = hal_get_mouse_fd(hp)) == -1) {
        printf("Failed to open PS2 mouse device. %m.\n");
        hal_release(hp);
        return -1;
    }

    printf("Waiting for %s keyboard and %s mouse input...\n", 
                    (kbtype == KB_TYPE_PC) ? "PC" : "SGI",
                    (mstype == MS_TYPE_PS2) ? "PS2" : "IMPS2");

    kb_len = kb_roffset = kb_woffset = 0;
    ms_len = ms_roffset = ms_woffset = 0;

    /* Choose TX USB KBM: set PCA9555 at I2C addr */
    /* 0x20 bit 5 for registers 6 and 2 to 0      */
    val = hal_get_pm_pca9555_val_k(hp, 0x06);
    hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xdf);
    val = hal_get_pm_pca9555_val_k(hp, 0x02);
    hal_set_pm_pca9555_val_k(hp, 0x02, val & 0xdf);

    /* Enable TX Y-MOUSE KBM */
    hal_set_fpga_val_k(hp, FPGA_MKCREG_Y, 
            0x7900 | MKCREG_XP_MSEOUT_ENABLE | MKCREG_XP_KBDOUT_ENABLE); 

    /* Choose RX PS2 KBM: set PCA9555 at I2C addr */
    /* 0x20 bit 6 & 2 for registers 6 and 2 to 0 */
    val = hal_get_pm_pca9555_val_k(hp, 0x06);
    hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xbb);
    val = hal_get_pm_pca9555_val_k(hp, 0x02);
    hal_set_pm_pca9555_val_k(hp, 0x02, val & 0xbb);

    /* Enable RX KBM */
    hal_set_fpga_val_k(hp, FPGA_KMSEL_REG, 0x01);
    if (kbtype == KB_TYPE_SGI)
        hal_set_fpga_val_k(hp, FPGA_MKCREG, 
            0x7900 | MKCREG_XP_MSEIN_ENABLE | MKCREG_XP_KBDIN_ENABLE); 
    else
        hal_set_fpga_val_k(hp, FPGA_MKCREG, 
            0x7910 | MKCREG_XP_MSEIN_ENABLE | MKCREG_XP_KBDIN_ENABLE); 

    while (1) {
        FD_ZERO(&read_fdset);
        max_fd = 1;

        if (kbfd > 0) {
            FD_SET(kbfd, &read_fdset);
            if (kbfd > max_fd) max_fd = kbfd;
        }
        if (msfd > 0) {
            FD_SET(msfd, &read_fdset);
            if (msfd > max_fd) max_fd = msfd;
        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
                                                                                
        nd = select(max_fd + 1, &read_fdset, NULL, NULL, &timeout);
                                                                                
        if (nd < 0) {
            if (errno == EINTR) {
                continue;
            }
            printf("Channel %d. Select returned with error %m.\n", cno);
            break;
        }
        else if (nd == 0) {
            //printf("Channel %d. Select timed out.\n", cno);
            continue;
        }
        if (kbfd > 0) {
            if (FD_ISSET(kbfd, &read_fdset)) {
                int size = sizeof(kbbuf) - kb_roffset;

                if (verbose) 
                    printf("Reading KB data\n");
                len = read(kbfd, &kbbuf[kb_roffset], size);
                if (len < 0) {
                    printf("Failed to read from KB. %m.\n");
                    break;
                }
                if (verbose) {
                    printf("Read %d bytes KB data\n", len);
                    print_hexdata(&kbbuf[kb_roffset], len);
                }
                kb_len += len; 
                kb_roffset += len; 
                if (kb_roffset >= 1024) kb_roffset -= 1024;
            }
        }
        if (msfd > 0) {
            if (FD_ISSET(msfd, &read_fdset)) {
                int size = sizeof(msbuf) - ms_roffset;
                if (verbose)
                    printf("Reading MS data\n");
                len = read(msfd, &msbuf[ms_roffset], size);
                if (len < 0) {
                    printf("Failed to read from PS2 MS. %m.\n");
                    break;
                }
                if (verbose) {
                    printf("Read %d bytes MS data\n", len);
                    print_hexdata(&msbuf[ms_roffset], len);
                }
                ms_len += len; 
                ms_roffset += len; 
                if (ms_roffset >= 1024) ms_roffset -= 1024;
            }
        }

        if (dontwrite) {
            kb_roffset = kb_len = ms_roffset = ms_len = 0;
            continue;
        }

        if (kb_len > 0 || ms_len > 0) {

            /* Select TX KBM */
            hal_set_fpga_val_k(hp, FPGA_KMSEL_REG, 0x00);

            if (kb_len > 0) {
                if (verbose)
                    printf("Writing %d bytes KB data\n", kb_len);
                len = write_to_kb_fpga(hp, &kbbuf[kb_woffset], kb_len);
                if (len < 0) {
                    printf("Channel %d. Error writing to KB FPGA. %m.\n", cno);
                    kb_roffset = kb_woffset = kb_len = 0;
                }
                if (verbose) {
                    printf("Wrote %d bytes KB data\n", len);
                    print_hexdata(&kbbuf[kb_woffset], len);
                }
                kb_woffset += len;
                if (kb_woffset >= 1024) kb_woffset -= 1024;
                kb_len -= len;
            }
            if (ms_len > 0) {
                if (verbose)
                    printf("Writing %d bytes MS data\n", ms_len);
                len = write_to_ms_fpga(hp, &msbuf[ms_woffset], ms_len, mstype);
                if (len < 0) {
                    printf("Channel %d. Error writing to MS FPGA. %m.\n", cno);
                    ms_roffset = ms_woffset = ms_len = 0;
                }
                if (verbose) {
                    printf("Wrote %d bytes MS data\n", len);
                    print_hexdata(&kbbuf[ms_woffset], len);
                }
                ms_woffset += len;
                if (ms_woffset >= 1024) ms_woffset -= 1024;
                ms_len -= len;
            }
            else 
                printf("Channel %d. Failed to write to MS FPGA.\n", cno);
        }
    }

    /* Put PS2 KBM back for TX if necessary */
    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        hal_set_fpga_val_k(hp, FPGA_KMSEL_REG, 0x00);
        hal_set_fpga_val_k(hp, FPGA_MKCREG_Y, 
            0x7900 | MKCREG_XP_MSEOUT_ENABLE | MKCREG_XP_KBDOUT_ENABLE); 
        val = hal_get_pm_pca9555_val_k(hp, 0x06);
        hal_set_pm_pca9555_val_k(hp, 0x06, val & 0xbf);
        val = hal_get_pm_pca9555_val_k(hp, 0x02);
        hal_set_pm_pca9555_val_k(hp, 0x02, val & 0xbf);
    }

    hal_release(hp);

    return 0;
}

/****************************************
 * Test utility for OSD icons & cursors * 
 ****************************************/
static void print_testicon_help(void) {
    printf("Usage: testicon -c N [-g|-k|-m|-l|-r] [-x X] [-y Y] [-h]\n");
    printf("where the '-c N' switch specifies the FPGA channel number\n");
    printf("the '-g', '-k', '-m', '-l' and '-r' switches, if present, \n");
    printf("specifies whether to display guideruler, keyboard, mouse, local\n");
    printf("chromakey pixel or remote chromakey pixel icon.\n");
    printf("The '-x' and '-y' switches specify the position of the icon.\n");
};

static int testicon_main(int argc, char **argv) {
    int opt, cno = -1, x = 100, y = 100, sizex = 0, sizey = 0;
    int kb_icon = 0, ms_icon = 0, pix_icon = 0, local = 0, i, dir = 1;
    int color = color_code_from_name("Red");
    unsigned char rval, gval, bval;
    HAL *hp;

    while ((opt = getopt(argc, argv, "c:kmlrx:y:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'k':
            kb_icon = 1;
            break;
        case 'm':
            ms_icon = 1;
            break;
        case 'l':
            local = 1; pix_icon = 1;
            break;
        case 'r':
            pix_icon = 1;
            break;
        case 'x':
            x = atoi(optarg);
            break;
        case 'y':
            y = atoi(optarg);
            break;
        case '?': 
        case 'h': 
            print_testicon_help();
            return -1;
        }
    }   

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE || 
                    hal_get_board_type(hp) != BOARDTYPE_RX) {
        printf("Failed: Not an XP200 or channel %d not V2D RX.\n", cno);
        hal_release(hp);
        return -1;
    }

    /* Turn off all icons */
    hal_set_icon_off(hp);
    hal_set_mouse_cursor_status(hp, 0);
    hal_set_pix_cursor_status(hp, 0, LOCALPIX1CURSOR);
    hal_set_pix_cursor_status(hp, 0, LOCALPIX2CURSOR);
    hal_set_pix_cursor_status(hp, 0, LOCALPIX3CURSOR);
    hal_set_pix_cursor_status(hp, 0, REMOTEPIX1CURSOR);
    hal_set_pix_cursor_status(hp, 0, REMOTEPIX2CURSOR);
    hal_set_pix_cursor_status(hp, 0, REMOTEPIX3CURSOR);
    for (i = PIX3CURSOR_NUM+1; i < MAX_MULTIMOUSE; i++) {
        hal_set_multimouse_cursor_status(hp, 0, i);
    }

    /* Initialize icon */
    if (kb_icon) {
        sizex = sizey = DEFAULT_KEYBOARD_ICON_SIZE; 
        if (hal_init_keyboard_icon(hp, 0, color) < 0) {
            printf("Failed to set OSD memory map.\n"); 
            hal_release(hp);
            return -1;
        }
    }
    else {
        struct LUT_struct lut;
        if (pix_icon) {
            if (local) {
                if (hal_get_display_lut(hp, &lut) < 0) {
                    printf("Failed: Cannot get display LUT from driver\n");
                    hal_release(hp);
                    return -1;
                }
                hal_init_pix_cursor(hp, x, y, lut.a_hres, lut.a_vres, 
                    LOCALPIX1CURSOR);
                hal_get_chromakey_pixel_color_k(hp, MATTE_LOCAL, x, y, 
                        &rval, &gval, &bval);
                printf("Local Matte (%d, %d). R 0x%02x, G 0x%02x, B 0x%2x\n",
                        x, y, rval, gval, bval);
            }
            else {
                if (hal_get_dropside_lut(hp, &lut) < 0) {
                    printf("Failed: Cannot get display LUT from driver\n");
                    hal_release(hp);
                    return -1;
                }
                hal_init_pix_cursor(hp, x, y, lut.a_hres, lut.a_vres, 
                    REMOTEPIX1CURSOR);
                hal_get_chromakey_pixel_color_k(hp, MATTE_REMOTE, x, y, 
                        &rval, &gval, &bval);
                printf("Remote Matte (%d, %d). R 0x%02x, G 0x%02x, B 0x%2x\n",
                        x, y, rval, gval, bval);
            }
        }
        else {
            if (hal_get_display_lut(hp, &lut) < 0) {
                printf("Failed: Cannot get display LUT from driver\n");
                hal_release(hp);
                return -1;
            }
            if (ms_icon) {
                hal_init_mouse_cursor(hp, x, y, lut.a_hres, lut.a_vres);
                hal_set_mouse_cursor_status(hp, 1);
            }
            else {
                for (i = 0; i < 12; i++) {
                    double radian = 3.1416 * (double) i / (double) 6;
                    hal_init_multimouse_cursor(hp, 
                            x + (double) 40 * sin(radian) + (double) 0.5, 
                            y + (double) 40 * cos(radian) + (double) 0.5, 
                            lut.a_hres, lut.a_vres, i + PIX3CURSOR_NUM + 1);
                    hal_set_multimouse_cursor_status(hp, 1, i);
                }
            }
            sizex = lut.a_hres;
            sizey = lut.a_vres;
        }
    }

    while (1) {
        if (pix_icon) {
            hal_get_chromakey_pixel_color_k(hp, 
                    (local) ? MATTE_LOCAL : MATTE_REMOTE, x, y, 
                    &rval, &gval, &bval);
            printf("%s Matte (%d, %d). R 0x%02x, G 0x%02x, B 0x%2x\n",
                (local) ? "Local" : "Remote", x, y, rval, gval, bval);
        }
        else if (kb_icon) {
            printf("Displaying Icon\n");
            hal_set_icon_on(hp, color++, x, y, sizex, sizey);
            sleep(3);
            printf("Hiding Icon\n");
            hal_set_icon_off(hp);
        }
        else if (ms_icon) {
            for (i = 0; i < 30; i++) {
                hal_set_mouse_cursor_pos(hp, x, y, sizex, sizey);
                usleep(100000);
                x = (dir) ? (x + 5) : (x - 5);
                y = (dir) ? (y + 5) : (y - 5);
                if (x < 0) x = 0;
                else if (x > sizex) x = sizex;
                if (y < 0) y = 0;
                else if (y > sizey) y = sizey;
            }
        }
        else {
            int j;
            for (j = 0; j < 30; j++) {
                for (i = PIX3CURSOR_NUM+1; i < MAX_MULTIMOUSE; i++) {
                    double radian = 3.1416 * (double) i / (double) 6;
                    hal_set_multimouse_cursor_pos(hp, 
                            x + (double) 40 * sin(radian) + (double) 0.5, 
                            y + (double) 40 * cos(radian) + (double) 0.5, 
                            sizex, sizey, i);
                }
                usleep(100000);
                x = (dir) ? (x + 5) : (x - 5);
                y = (dir) ? (y + 5) : (y - 5);
                if (x < 10) x = 10;
                else if (x > sizex - 10) x = sizex - 10;
                if (y < 10) y = 10;
                else if (y > sizey - 10) y = sizey - 10;
            }
        }
        sleep(3);
        dir = (dir) ? 0 : 1;
    }
    hal_release(hp);

    return 0;
}

/***********************************************
 * Test utility to read chromakey pixel colors * 
 ***********************************************/
#define RDPIXEL_LOCK_RETRY_COUNT    10
static int chromakey_rdpixel_lock(HAL *hp) {
    int val = 1;
    if (ioctl(hp->kernel_fd, FPGA_IOC_LOCK_RDPIXEL, &val) < 0)
        return -1;
    return 0;
}
                                                                                
static int chromakey_rdpixel_unlock(HAL *hp) {
    int val = 0;
    if (ioctl(hp->kernel_fd, FPGA_IOC_LOCK_RDPIXEL, &val) < 0)
        return -1;
    return 0;
}

static int readpixel_main(int argc, char **argv) {
    int opt, cno, i, x = -1, y = -1, active_interface; 
    HAL *hp;
    unsigned long lval;

    while ((opt = getopt(argc, argv, "c:x:y:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'x':
            x = atoi(optarg);
            break;
        case 'y':
            y = atoi(optarg);
            break;
        case '?': 
        case 'h': 
            printf("Usage: readpixel -c cno -x X -y Y\n");
            return -1;
        }
    }   

    if (x < 0 || y < 0) {
        printf("Usage: readpixel -c cno [-x X] [-y Y]\n");
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE || 
                    hal_get_board_type(hp) != BOARDTYPE_RX) {
        printf("Failed: Not an XP200 or channel %d not V2D RX.\n", cno);
        hal_release(hp);
        return -1;
    }

    if ((active_interface = hal_getActiveIntf(hp)) < 0) {
        printf("Failed: unable to get active interface from driver\n");
        return -1;
    }

    /* Half the horiz pixel position if display is in 48 bit mode */
    if (active_interface & INTERFACE_48BIT)
        x /= 2;

    lval = (unsigned long) (x << 16) | (unsigned long) y;

    /* Lock RDPIXEL register */
    for (i = 0; i < RDPIXEL_LOCK_RETRY_COUNT; i++) {
        if (chromakey_rdpixel_lock(hp) < 0)
            usleep(5000);
        else
            break;
                                }
    if (i >= RDPIXEL_LOCK_RETRY_COUNT) {
        printf("Failed to lock RDPIXEL register\n");
    }
    else {
        hal_set_fpga_val32_k(hp, FPGA_RDPIXELLOC, lval);
        /* wait for at least 1 frame to go by */
        usleep(100000);
        printf("LPIXINEVEN = 0x%lx\n", 
                hal_get_fpga_val32_k(hp, FPGA_LPIXINEVEN));
        printf("LPIXINODD  = 0x%lx\n", 
                hal_get_fpga_val32_k(hp, FPGA_LPIXINODD));
        printf("RPIXINEVEN = 0x%lx\n", 
                hal_get_fpga_val32_k(hp, FPGA_RPIXINEVEN));
        printf("RPIXINODD  = 0x%lx\n", 
                hal_get_fpga_val32_k(hp, FPGA_RPIXINODD));
        printf("PIXOUTEVEN = 0x%lx\n", 
                hal_get_fpga_val32_k(hp, FPGA_PIXOUTEVEN));
        printf("PIXOUTODD  = 0x%lx\n", 
                hal_get_fpga_val32_k(hp, FPGA_PIXOUTODD));
        chromakey_rdpixel_unlock(hp);
    }
    hal_release(hp);

    return 0;
}


/******************************
 * Test utility for Croppping * 
 ******************************/
static void print_testcrop_help(void) {
    printf("Usage: testcrop -c N [OPTIONS]\n");
    printf("Crops remote video to area specified by options\n");
    printf("Options:\n");
    printf("\t-X xstart\t Set start horizontal offset from boundary\n");
    printf("\t-W width\t Set width of cropped area\n");
    printf("\t-Y ystart\t Set start vertical offset from boundary\n");
    printf("\t-H height\t Set height of cropped area\n");
    printf("\t-d \t\t Disable horizontal and vertical cropping\n");
    printf("\t(Offsets are in UNSCALED pixels or lines)\n"); 
    printf("\n");
}

static int testcrop_main(int argc, char **argv) {
    int opt, cno, xstart = 0, width = -1, ystart = 0, height = -1; 
    int disable = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "c:X:W:Y:H:dh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'X':
            xstart = atoi(optarg);
            break;
        case 'W':
            width = atoi(optarg);
            break;
        case 'Y':
            ystart = atoi(optarg);
            break;
        case 'H':
            height = atoi(optarg);
            break;
        case 'd':
            disable = 1;
            break;
        case 'h':
        case '?': 
            print_testcrop_help();
            return -1;
        }
    }   

    if ((xstart < 0 || ystart < 0 || width < 0 || height < 0) && !disable) {
        print_testcrop_help();
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE || 
                    hal_get_board_type(hp) != BOARDTYPE_RX) {
        printf("Failed: Not an XP200 or channel %d not V2D RX.\n", cno);
        hal_release(hp);
        return -1;
    }

    if (disable) {
        hal_disable_cropping(hp);
    }
    else {
        hal_enable_cropping(hp, xstart, width, ystart, height);
    }

    hal_release(hp);

    return 0;
}

#define DEFAULT_FRMCNT_INTERVAL   5
#define DEFAULT_FRMCNT_RETRIES    3
#define DEFAULT_FRMCNT_MULTIPLIER 3

static int printframecnt_main(int argc, char **argv) {
    int opt, cno = 0, i, interval, fifolevel = 0;
    HAL *hp;
    unsigned char buf[FRAMECNT_FIFO_SIZE];
    char frametxt[FRAMECNT_TEXT_SIZE];

    while ((opt = getopt(argc, argv, "c:h")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case '?': 
        default:
            printf("Usage: printframecnt -c cno\n");
            return -1;
        }
    }   
    if (cno <= 0 || cno > MAX_FPGA_BOARDS) {
        printf("Usage: printframecnt -c cno\n");
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE || 
                    hal_get_board_type(hp) != BOARDTYPE_RX) {
        printf("Failed: Not an XP200 or channel %d not V2D RX.\n", cno);
        hal_release(hp);
        return -1;
    }

    interval = DEFAULT_FRMCNT_INTERVAL;
    for (i = 0; i < DEFAULT_FRMCNT_RETRIES; i++) {
        fifolevel = hal_read_framecnt_fifo(hp, buf, interval);
        if (fifolevel > 5) break;
        interval *= DEFAULT_FRMCNT_MULTIPLIER;
    }
    if (fifolevel <= 5) {
        printf("Failed to read frame counter FIFO\n");
    }
    else if (print_framecnt_to_buffer(buf, frametxt, "--PAGE BREAK--",
                fifolevel, interval, hal_get_board_type(hp)) >= 0) {
       printf(frametxt);
    }
    else {
        printf("Error compute frame count statistics. See log for details.\n");
    }
    hal_release(hp);

    return 0;
}

#define SII_FLAG_NONE       0x00
#define SII_FLAG_48BIT      0x01
#define SII_FLAG_DUALLINK   0x02
#define SII_FLAG_EZONEHIGH  0x04

/******************************************************************
 * This routine executes the SII1178 programming sequence that's  *
 * necessary after every resolution switch or video clock change. *
 * Note (1) FPGA video DCM shoud be disabled before calling this  *
 * routine as stated on SII1178 data sheet Sil-DS-0127-A p. 23.   *
 * (2) video output is disabled by this routine. The caller needs *
 * to enable it afterwards (w/some delay). This is to ensure the  *
 * SII, the DDS and the FPGA registers are all set to the final   *
 * values when it's enabled so it's always stable (TP Bug 18761). *
 ******************************************************************/
static void program_sii_registers(HAL *hp, int flags) {

    if (!(flags & SII_FLAG_48BIT)) {        /* 24 bit interface */
        unsigned char b;
#if 0
        unsigned char c;
        /* set sii1178 for DVI output */
        c = hal_get_pca9555_clk_val_k(hp, 0x07);
        hal_set_pca9555_clk_val_k(hp, 0x07, c & 0x5f);
        usleep(1000);

        b = hal_get_pca9555_clk_val_k(hp, 0x03);
        hal_set_pca9555_clk_val_k(hp, 0x03, b & 0x5f);
        usleep(1000);

        hal_set_pca9555_clk_val_k(hp, 0x03, b | 0xa0);
        usleep(1000);
#endif

        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x08, 0x35);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x0f, 0x4c);
        usleep(1000);

        hal_set_sii1178_1_val_k(hp, 0x0f, 0x44);
        usleep(1000);

        /* power down the master and slave */
        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);
        hal_set_sii1178_2_val_k(hp, 0x08, 0x34);
        usleep(1000);

        /* set slave in reset */
        b = hal_get_pca9555_clk_val_k(hp, 0x03);
        hal_set_pca9555_clk_val_k(hp, 0x03, b & 0xdf);
    }
    else {				                 /* 48 bit interface */
#if 0
        unsigned char b, c;
        c = hal_get_pca9555_clk_val_k(hp, 0x07);
        hal_set_pca9555_clk_val_k(hp, 0x07, c & 0x5f);
        usleep(1000);

        b = hal_get_pca9555_clk_val_k(hp, 0x03);
        hal_set_pca9555_clk_val_k(hp, 0x03, b & 0x5f);
        usleep(1000);
        if (flags & SII_FLAG_DUALLINK) 
            hal_set_pca9555_clk_val_k(hp, 0x03, b | 0xa0);
        else 
            hal_set_pca9555_clk_val_k(hp, 0x03, b | 0xa0);
        //check above for redundancy -- Piyush
        //06May09 
       usleep(1000);
#endif

       /* Dual link. Program the slave. */
       if (flags & SII_FLAG_DUALLINK) {		
            hal_set_sii1178_2_val_k(hp, 0x0f, 0x44);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0f, 0x4c);
            usleep(1);
            if (flags & SII_FLAG_EZONEHIGH) 
                hal_set_sii1178_2_val_k(hp, 0x0e, 0x34);
            else
                hal_set_sii1178_2_val_k(hp, 0x0e, 0x10);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0a, 0x80);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x09, 0x30);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0c, 0x89);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x0d, 0x70);
            usleep(1);
            hal_set_sii1178_2_val_k(hp, 0x08, 0x34);
        }

        /* Program the master */
        hal_set_sii1178_1_val_k(hp, 0x0f, 0x44);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0f, 0x4c);
        usleep(1);
        if (flags & SII_FLAG_EZONEHIGH)
            hal_set_sii1178_1_val_k(hp, 0x0e, 0x34);
        else
            hal_set_sii1178_1_val_k(hp, 0x0e, 0x10);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0a, 0x80);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x09, 0x30);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0c, 0x89);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x0d, 0x70);
        usleep(1);
        hal_set_sii1178_1_val_k(hp, 0x08, 0x34);

        /* Check master/slave setting option */
        if ((hal_get_sii1178_1_val_k(hp, 0x0c) & 0x40) == 0) {
            printf("Error: device at I2C addr 0x38 not a master."); 
        } 
        if (flags & SII_FLAG_DUALLINK) {
            if ((hal_get_sii1178_2_val_k(hp, 0x0c) & 0x40) == 1) {
                printf("Error: device at I2C addr 0x39 not a slave.");
            }
        }
        usleep(1000);
    }
    hal_set_sii1178_1_val_k(hp, 0x08, 0x35);
    if (flags & SII_FLAG_DUALLINK) 		
        hal_set_sii1178_2_val_k(hp, 0x08, 0x35);
}

static void print_programsii_help(void) {
    printf("Usage: programsii -c cno [-4] [-d] [-e]\n");
    printf("Options: -4\t Set to 48 Bit mode\n");
    printf("         -d\t Set to dual link mode\n");
    printf("         -e\t Set SII external zone to high\n");
    printf("Note setting -d automatically sets SII to 48 bit, and\n");
    printf("setting -e automatically sets SII to dual link 48 bit\n");
}

static int programsii_main(int argc, char **argv) {
    int opt, cno = 0, flags, duallink = 0, fortyeightbit = 0, extzone = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "c:d4eh")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'd':
            duallink = 1;
            break;
        case '4':
            fortyeightbit = 1;
            break;
        case 'e':
            extzone = 1;
            break;
        case '?': 
        default:
            print_programsii_help();
            return -1;
        }
    }   
    if (cno <= 0 || cno > MAX_FPGA_BOARDS) {
        print_programsii_help();
        return -1;
    }

    hp = hal_acquire(cno - 1);

    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", cno);
        return -1;
    }

    if (hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Failed: Not an XP200 or channel %d.\n", cno);
        hal_release(hp);
        return -1;
    }

    flags = SII_FLAG_NONE;
    if (duallink) 
        flags |= SII_FLAG_DUALLINK | SII_FLAG_48BIT;
    if (fortyeightbit) 
        flags |= SII_FLAG_48BIT;
    else
        flags &= ~SII_FLAG_DUALLINK;
    if (extzone) 
        flags |= SII_FLAG_DUALLINK | SII_FLAG_48BIT | SII_FLAG_EZONEHIGH;
    
    printf("Programming Channel %d SII to %s %s %s\n", 
            hal_get_channel_number(hp), 
            (flags & SII_FLAG_48BIT) ? "48 Bit" : "24 Bit",
            (flags & SII_FLAG_DUALLINK) ? "Dual Link" : "Single Link",
            (flags & SII_FLAG_EZONEHIGH) ? "ezone High" : "ezone Low") ;
    program_sii_registers(hp, flags);
    //hal_set_video_output(hp, 1);

    hal_release(hp);

    return 0;
}

static void print_testfan_help(void) {
    printf("Usage: testfan [-s high|low|medium|off] [-i seconds] [-d minutes]\n");
    printf("Options: -s high|low|medium|off\t Set fan speed to high, low or medium (default high)\n");
    printf("         -i interval\t Set log interval to interval (default 60) secs)\n");
    printf("         -d duration\t Set test duration to duration (default infinite) mins)\n");
}

static int testfan_main(int argc, char **argv) {
    int opt, interval = 60, i, duration = -1, speed = FAN_SPEED_NONE;
    int cputemp, fanrpm;
    float localtemp, remotetemp;
    HAL *hp;

    while ((opt = getopt(argc, argv, "s:i:d:")) > 0) {
        switch (opt) {
        case 's':
            speed = !strcasecmp(optarg, "off") ? FAN_SPEED_OFF : 
                (!strcasecmp(optarg, "low") ? FAN_SPEED_LOW :
                (strcasecmp(optarg, "medium") ? FAN_SPEED_MEDIUM : 
                 FAN_SPEED_HIGH));
            break;
        case 'i':
            interval = atoi(optarg);
            break;
        case 'd':
            duration = atoi(optarg);
            break;
        case '?': 
        default:
            print_testfan_help();
            return -1;
        }
    }   
    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access FPGA board\n");
        return 0;
    }
    if (get_hardware_profile() == HARDWARE_PROFILE_XP100) {
        unsigned char b[1];
        if (interval <= 0) 
            interval = 60;
        if (speed != FAN_SPEED_NONE) {
            hal_set_xp100_fan_speed(hp, speed);
            printf("Setting Fan Speed to %s\n", 
                    (speed == FAN_SPEED_OFF) ? "Off" :
                    ((speed == FAN_SPEED_HIGH) ? "High" : 
                    ((speed == FAN_SPEED_MEDIUM) ? "Medium" : "Low")));
        }
        else {
            speed = hal_get_xp100_fan_speed(hp);
            printf("Current Fan Speed = %s\n", 
                    (speed == FAN_SPEED_OFF) ? "Off" :
                    ((speed == FAN_SPEED_HIGH) ? "High" : 
                    ((speed == FAN_SPEED_MEDIUM) ? "Medium" : "Low")));
        }

        printf("Elapsed Time    CPU   Local  Remote   Fan RPM  Reg.4C\n");
        i = 0;
        while (1) {
            cputemp = get_system_temperature(TEMPERATURE_PROC_FILE);
            if (hal_get_board_temperature(hp, &localtemp, &remotetemp) < 0) {
                printf("Failed to read board temperature.");
                return 0;
            }
            if (hal_get_xp100_fan_speed(hp) == FAN_SPEED_OFF) {
                printf("%7.1f          %d   %5.2f   %5.2f     N/A    N/A\n", 
                    ((float) i * (float) interval / (float) 60), 
                    cputemp, localtemp, remotetemp);
            }
            else {
                fanrpm = hal_get_xp100_fan_rpm(hp);
                hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, 
                                LM63_I2C_ADDR, LM63REG_PWM_VAL, 1, b);
                printf("%7.1f          %d   %5.2f   %5.2f     %d    0x%02x\n", 
                    ((float) i * (float) interval / (float) 60), 
                    cputemp, localtemp, remotetemp, fanrpm, b[0]);
            }
            i++;
            if (duration > 0 && (i * interval) >= duration * 60) 
                break;
            sleep(interval);
        }
    }
    hal_release(hp);
    return 0;
}

/*****************************************************************
 * Monitor XP100 temperature. If any of the temperature readings *
 * is above TEMP_VERYHIGH_SETPOINT, set fan to high, if fan is   *
 * high and all temperatures are below TEMP_HIGH_SETPOINT, set   *
 * fan to medium. If fan is low and any temperatures are above   *
 * TEMP_MEDIUM_SETPOINT, set fan to medium, if all temperatures  *
 * are below TEMP_LOW_SETPOINT, set fan to low.                  *
 *****************************************************************/
static void print_tempmon_help(void) {
    printf("Usage: tempmon [-i interval]\n");
    printf("'interval' is the interval between monitoring (default 60 secs)\n");
}

static int tempmon_main(int argc, char **argv) {
    int rc, opt, speed, interval = TEMPMON_INTERVAL;
    HAL *hp;

    while ((opt = getopt(argc, argv, "i:")) > 0) {
        switch (opt) {
        case 'i':
            interval = atoi(optarg);
            break;
        case '?': 
        default:
            print_tempmon_help();
            return -1;
        }
    }   
    if (get_hardware_profile() != HARDWARE_PROFILE_XP100) {
        printf("Wrong hardware: Not XP100\n");
        return 0;
    }

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access FPGA board\n");
        return 0;
    }

    printf("  CPU    Local    Remote    Reg.4C\n");
    while (1) {
        rc = hal_monitor_xp100_temperature(hp, TEMPMON_FLAG_PRINT);
        if (rc < 0) {
            printf("Unable to read XP100 temperature\n");
            break;
        }
        else if (rc != TEMP_NOCHANGE) {
            speed = hal_get_xp100_fan_speed(hp);
            if (speed != FAN_SPEED_HIGH && rc == TEMP_RANGE_HIGH) {
                hal_set_xp100_fan_speed(hp, FAN_SPEED_HIGH);
            }
            else if (speed == FAN_SPEED_HIGH && rc != TEMP_RANGE_HIGH &&
                    rc != TEMP_RANGE_MEDIUM_2) {
                hal_set_xp100_fan_speed(hp, FAN_SPEED_MEDIUM);
            }
            else if (speed == FAN_SPEED_LOW && rc != TEMP_RANGE_LOW_2 &&
                    rc != TEMP_RANGE_LOW_1) { 
                hal_set_xp100_fan_speed(hp, FAN_SPEED_MEDIUM);
            }
            else if (speed != FAN_SPEED_LOW && rc == TEMP_RANGE_LOW_1) {
                hal_set_xp100_fan_speed(hp, FAN_SPEED_LOW);
            }
        }
        sleep(interval);
    }

    hal_release(hp);
    return 0;
}

/*******************************************************************
 * Locks the I2C bus till user presses ENTER and exits the program *
 *******************************************************************/
static int i2clock_main(int argc, char **argv) {
    int cno = -1, opt;
    HAL *hp;
    char buf[32];

    while ((opt = getopt(argc, argv, "c:")) > 0) {
        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case '?': 
        default:
            printf("Usage: i2clock -c cno\n");
            return -1;
        }
    }   

    if (cno < 1 || cno > MAX_FPGA_BOARDS) {
        printf("Usage: Invalid channel number %d\n", cno);
    }

    hp = hal_acquire(cno - 1);
    if (hp == NULL) {
        printf("Failed to access FPGA board\n");
        return 0;
    }
    sprintf(buf, "/dev/fpga%d_i2c", cno  - 1);
    hp->i2c_fd = open(buf, O_RDWR);
    if (hp->i2c_fd <= 0) {
        printf("Channel %d. Could not get I2C lock %s", cno, buf);
        return -1;
    }
    printf("Acquired I2C lock on Channel %d. Press ENTER to exit.\n", cno);
    getchar();
    if (hp->i2c_fd)
        close(hp->i2c_fd);
    printf("Released I2C lock on Channel %d\n", cno);
    return 0;
}

/************************************
 * Maint command                    *
 ************************************/
static void run_util(const char *util_name, int argc, char **argv);
static int maint_main(int argc, char **argv);

const struct v_util maint_utils[] = {
    {"maint", maint_main},
    {"adcreg", adcreg_main},
    {"i2creg", i2creg_main},
    {"ramread", ramread_main},
    {"ramwrite", ramwrite_main},
    {"readplxvpd", read_plx_vpd_main},
    {"loadplxvpd", load_plx_vpd_main},
    {"readplxeeprom", read_plx_eeprom_main},
    {"loadplxeeprom", load_plx_eeprom_main},
    {"readcdceeeprom", read_cdce_eeprom_main},
    {"loadcdceeeprom", load_cdce_eeprom_main},
    {"readusbeeprom", read_usb_eeprom_main},
    {"loadusbeeprom", load_usb_eeprom_main},
    {"testserial", testserial_main},
    {"testaudio", testaudio_main},
    {"testps2", testps2_main},
    {"getdds", getdds_main},
    {"setdds", setdds_main},
    {"getpcparams", getpcparams_main},
    {"setpcparams", setpcparams_main},
    {"testicon", testicon_main},
    {"readpixel", readpixel_main},
    {"testcrop", testcrop_main},
    {"printframecnt", printframecnt_main},
    {"programsii", programsii_main},
    {"testfan", testfan_main},
    {"tempmon", tempmon_main},
    {"i2clock", i2clock_main},

    {NULL, NULL}
};

/************************************
 * Main entry point for maint utils *
 ************************************/
int main(int argc, char **argv)
{

    const char *s;
    const char *util_name;

    util_name = argv[0];

    /*****************************************
     * Strip path name prefix from util name *
     *****************************************/
    for (s = util_name; *s != '\0';) {
        if (*s++ == '/')
            util_name = s;
    }

    /************************************************************
     * If we have admin privileges, set the user and group to   *
     * root. This will only work if the SUID bit is set on this *
     * executable and non-root user is using this (e.g. admin)  *
     ************************************************************/
    int gid = getgid();
    if (gid == 0 || gid == 100) {
        setuid(0);
        setgid(0);
    }

    /* Initialise our logging library */
    vlog_init(util_name);

    /* Run the invoked utility -- never ruturns if utility found */
    run_util(util_name, argc, argv);

    printf("Utility %s not found!\n", util_name);

    return -1;
}

static int maint_main(int argc, char **argv)
{
    int col = 0, len, i;
    static int visited = 0;

    /***************************************************************
     * If --install requested, automatically create symbolic links *
     * for all current utils                                       *
     ***************************************************************/
    if (argc > 1 && (strcmp(argv[1], "--install") == 0)) {
        const struct v_util *u = maint_utils;
        char buf[80];
        while (u->name != 0) {
            if (strcmp(u->name, "maint") != 0) {
                sprintf(buf, "%s/bin/%s", INSTALL_PATH, u->name);
                symlink(INSTALL_PATH "/bin/maint", buf);
            }
            u++;
        }

        return 0;
    }

    /*******************
     * Display version *
     *******************/
    if (argc > 1 && (strcmp(argv[1], "--version") == 0)) {
        char version[80], timestamp[80];
        get_software_version(version, 80);
        get_software_date(version, 80);
        printf("%s: Version %s Date %s\n", argv[0], version, timestamp);
        return 0;
    }

    argc--;

    /*********************************************
     * If we've already been here once, exit now *
     *********************************************/
    if (visited == 1 || argc < 1) {
        const struct v_util *u = maint_utils;

        fprintf(stderr, "maint \n\n"
                "Usage: maint [function] [arguments]...\n"
                "   or: [function] [arguments]...\n\n"
                "Currently defined functions:\n");

        while (u->name != 0) {
            col +=
                fprintf(stderr, "%s%s", ((col == 0) ? "\t" : ", "),
                        (u++)->name);
            if (u->name != 0 && (strlen(u->name) + col) > 65) {
                fprintf(stderr, ",\n");
                col = 0;
            }
        }
        fprintf(stderr, "\n\n");
        exit(0);
    }


    /*************************************
     * Flag that we've been here already *
     *************************************/
    visited = 1;

    /**************************************
     * Move the command line down a notch *
     **************************************/
    len = argv[argc] + strlen(argv[argc]) - argv[1];
    memmove(argv[0], argv[1], len);
    memset(argv[0] + len, 0, argv[1] - argv[0]);

    /****************************
     * Fix up the argv pointers *
     ****************************/
    len = argv[1] - argv[0];
    memmove(argv, argv + 1, sizeof(char *) * (argc + 1));
    for (i = 0; i < argc; i++)
        argv[i] -= len;

    /***********************************************************
     * Call ourselves again to find the real utility requested *
     ***********************************************************/
    return (main(argc, argv));
}

/**************************************
 * Find and run the requested utility *
 **************************************/
static void run_util(const char *util_name, int argc, char **argv)
{
    const struct v_util *u = maint_utils;

    while (u->name != 0) {
        if (strcmp(util_name, u->name) == 0) {
            vlog_reinit();              /* re-init in case config changed */
            exit(u->main(argc, argv));  /* each utility exits on its own */
        }
        u++;
    }
}
