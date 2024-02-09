#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <ctype.h> 
#include <errno.h>
#include <strings.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <linux/input.h> 
#include <linux/types.h>

#include "hal.h" 
#include "vlib.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "kbms_utils.h"
#include "edid_utils.h"
#include "vlog.h"
#include "loadnet.h"
#include "vutils.h"
#include "generatedHeaders.h"
#include "v2d.h"
#include "hal_edid.h"
#include "hal_nethdr.h"
#include "hal_rtpfilter.h"
#include "strmsrv_comm.h"

#ifdef __XPI__
#include <asm/types.h>      /* for videodev2.h */
#include <linux/videodev2.h>
#include "i2c-dev.h"
#include "platform-netvizxpi.h"
#include "netvizxpi/adv7612.h"
#include "netvizxpi/netvizxpi.h"
#include "netvizxpi_si5338_register_map.h"
#include "xpi_i2c_control.h"
#include "hal_v4l2.h"
#include "license_verifier.h"
#else
#include "plx9056pciif.h"
#include "ad9852dds.h"
#include "xp_i2c_control.h"
#include "lut_table.h"
#endif

#include "override_config.h"
#include "temperature_control.h"
#include "lut_utils.h"
#include "system_utils.h"
#include "math.h"
#include "hal_rtpfilter.h"
#include "xpi_gpio_control.h"
#include "xpi_pll_control.h"
#include "osd_control.h"

#define OPTION_INVALID  -1
#define OPTION_NONE     0
#define OPTION_READ     1
#define OPTION_WRITE    2

static int debug = 0;

/***************************************************************
 * Routine to value of integer arguments to the 'reg' commands *
 ***************************************************************/
static unsigned long getint(char *arg) 
{
    if (strncasecmp(arg, "0x", 2) == 0) 
        return strtoul(&arg[2], NULL, 16);
    return strtoul(arg, NULL, 10);
}

/*********************************************
 * Display/modify Omnitek and FPGA registers *
 *********************************************/
static void print_xpi_version(HAL *hp)
{
    char info[80];
    int boardtype = hal_get_board_type(hp);
    unsigned short flags;

    if (hal_get_xpi_info(hp, info, sizeof(info)) < 0) {
        printf("Failed to get XPI information\n");
    }
    else if (hp->hw_info.hardware_type != XPI_DEVICE) {
        printf("Not an XPI device.\n");
    }
    else if (!hp->hw_info.hardware_detected) {
        printf("No XPI device detected.\n");
    }
    else {
        int hw_type, bd_type, fanout_type, fanout_rev, rc;
        char fpga_version[80];
         
        hw_type = hal_get_hardware_type(hp);
        bd_type = hal_get_board_type(hp);
        printf("Hardware type %s, %s, %s, board type %s\n",
                (hw_type == V2DI50_DEVICE) ? "I50" : 
                ((hw_type == PLX_DEVICE) ? "XP" : 
                ((hw_type == XPI_DEVICE) ? "XPI" : "Unknown")),
                (hal_check_main_lm63(hp)? "main LM63" :
                 (hal_check_expansion_lm63(hp) ? "expansion LM63" : 
                 "no LM63")), 
                (hal_check_max6650(hp) ? "expansion MAX6650" : 
                 "no expansion MAX6650"),
                (bd_type == BOARDTYPE_TX) ? "Transmitter" :
                ((bd_type == BOARDTYPE_RX)? "Receiver" : "Unknown"));
        (void) hal_get_firmware_version(hp, fpga_version, sizeof(fpga_version));
        (void) hal_get_firmware_date(hp, info, sizeof(info));
        printf("Main FPGA Version %s (%s) Date %s", fpga_version,
            (boardtype == BOARDTYPE_TX) ? "Encoder" : "Decoder", info);
        
        if (hal_get_s6fpga_version(hp, info, sizeof(info)) <= 0) 
            printf("Failed to read Fanout FPGA version\n"); 
        else {
            sscanf(info, "%d.%d", &fanout_type, &fanout_rev);
            printf("Fanout FPGA Version %s (%s) ", info, 
                    (fanout_type & 0xf0) == 0xc0 ? 
                    "Encoder" : "Decoder");
            if (hal_get_s6fpga_date(hp, info, sizeof(info)) <= 0) 
                printf("Failed to read Fanout FPGA date\n"); 
            else
                printf("Date %s\n", info); 
        }
        if (hal_get_s3fpga_version(hp, info, sizeof(info)) <= 0) 
            printf("Failed to read Expansion FPGA version\n"); 
        else {
            printf("Expansion FPGA Version %s ", info);
            if (hal_get_s3fpga_date(hp, info, sizeof(info)) <= 0) 
                printf("Failed to read Expansion FPGA date\n"); 
            else
                printf("Date %s\n", info); 
        }
    } 
}

static void print_features(HAL *hp) 
{
    int license = hal_get_configured_license(hp);
    if (license == PRO3D_LICENSE)
        printf("Features: basic, dual-link, stereo\n");
    else if (license == PRO_LICENSE)
        printf("Features: basic, dual-link\n");
    else if (license == LITE_LICENSE)
        printf("Features: basic\n");
    else
        printf("Features: (basic)\n");
}

static void print_dreg_maint_help(void)
{
    printf("\nUsage: dreg [-v|-o|-6|-3] [-s] [-r reg] [-R|-W] [-c] [-d] \n\n");
    printf("\nDisplay version information if the -v option is set. Otherwise \n");
    printf("display or modify registers starting at register 'reg' interactively,\n");

    printf("or, if the -R or -W options are present, non-interactively.\n");
    printf("\n");
    printf("Options:\n");
    printf("\t-R Display the register value specified in the '-r' option\n");
    printf("\t-W val Write 'val' to the register specified in the '-r' option\n");
    printf("\t-o     Display/modify OmniTek registesrs.\n");
    printf("\t-6     Display/modify S6 registesrs.\n");
    printf("\t-3     Display/modify S3 registesrs.\n");
    printf("\t-c     Read the register 'c' times, works with -R.\n");
    printf("\t-d     Delay in millisecond between reads, works with -R.\n");
    printf("\t-i     increment registers by count 'i', works with -R.\n");
    printf("\t-s     Don't print response (i.e. read silently)\n");
    printf("\n");
    printf("\t'reg' and 'val' are decimal numbers unless prefixed with '0x' in\n");
    printf("\twhich case they are hexadecimal.\n");
} 

static int dreg_maint_main(int argc, char **argv)
{
    int opt, cno = -1, rw_option = OPTION_NONE, count = 1, counter, inc = 0;
    int do_omnitek = 0, silent = 0, print_version = -1;
    unsigned long lreg = 0, lval = 0, sleepms = 0;
    unsigned short val = 0;
    HAL *hp;
    unsigned long (*hal_get_func)(HAL *, unsigned long);
    void (*hal_set_func)(HAL *, unsigned long, unsigned long);
    int board_type, hardware_type, ival;
    char buff[80];

    /*******************
     * Allow only root *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to execute this command\n");
        return 0;
    }

    hp = hal_acquire(0);
    hardware_type = hal_get_hardware_type(hp);
    if (hp == NULL) {
        printf("Failed. Could not access hardware.\n");
        return -1;
    }
    if (hp && hardware_type != XPI_DEVICE) {
        print_dreg_maint_help();
        hal_release(hp);
        return 0;
    }
    hal_get_func = hal_get_fpga_val32_k;
    hal_set_func = hal_set_fpga_val32_k;

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "r:o63RW:c:d:i:svh")) > 0) {
        switch (opt) {
        case 'o':
            do_omnitek = 1;
            hal_get_func = hal_get_omnitek_val_k;
            hal_set_func = hal_set_omnitek_val_k;
            break;
        case '6':
            hal_get_func = hal_get_s6fpga_val32_k;
            hal_set_func = hal_set_s6fpga_val32_k;
            break;
        case '3':
            hal_get_func = hal_get_s3fpga_val32_k;
            hal_set_func = hal_set_s3fpga_val32_k;
            break;
        case 'R':
            rw_option = OPTION_READ;
            break;
        case 'r':
            lreg = getint(optarg);
            break;
        case 's':
            silent = 1;
            break;
        case 'v':
            print_version = 1;
            break;
        case 'W':
            rw_option = OPTION_WRITE;
            lval = getint(optarg);
            break;
        case 'c':
            count = getint(optarg);
            break;
        case 'd':
            sleepms = getint(optarg);
            break;
        case 'i':
            inc = getint(optarg);
            break;
        default:
            print_dreg_maint_help();
            if (hp) hal_release(hp);
            return -1;
        }
    }

    if (optind != argc) {
        printf("Bad Usage.\n");
        print_dreg_maint_help();
        if (hp) hal_release(hp);
        return -1;
    }

    /* Get hardware description */
    hal_get_hardware_description(hp, buff, sizeof(buff));

    /* Print version information */
    if (print_version != -1) {
        char version[80], timestamp[80];
        get_software_version(version, sizeof(version));
        get_software_date(timestamp, sizeof(timestamp));
        printf("%s Software Version %s Date %s\n", buff, version, timestamp);  
        print_xpi_version(hp);
        print_features(hp);
        hal_release(hp);
        return 0;
    }

    /* Handle non-interactive register read/write */
    if (rw_option == OPTION_READ) {
        for (counter = 0; counter < count; counter ++) {
            lval = hal_get_func(hp, lreg+(counter*inc));
            printf("0x%08lx == 0x%08lx\n", lreg+(counter*inc), lval); 
            usleep (1000*sleepms) ;
        }
        hal_release(hp);
        return 0;
    }

    if (rw_option == OPTION_WRITE) {
        hal_set_func(hp, lreg, lval);
        hal_release(hp);
        return 0;
    }

    /* Handle interactive register display/modify */
    strcpy(buff, "");
    do {
        if (buff[0] == '.') {
            break;
        }
        if (strlen(buff) > 1) {
            lreg -= (do_omnitek) ? 1 : 4; 
            lval = strtoul(buff, NULL, 16);
            if (do_omnitek)
                printf("0x%04x -> 0x%08lx ? ", (unsigned short) lreg, lval);
            else
                printf("0x%08lx -> 0x%08lx ? ", lreg, lval);

            if (fgets(buff, 80, stdin) == NULL)
                break;

            if (buff[0] == '.')
                break;

            if (buff[0] != 'y' && buff[0] != 'Y') {
                lreg += 4; 
                continue;
            }
            hal_set_func(hp, lreg, lval);
        }

        lval = hal_get_func(hp, lreg);
        if (silent)
            printf("0x%08lx -> ", lreg); 
        else
            printf("0x%08lx == 0x%08lx -> ", lreg, lval); 

        lreg += (do_omnitek) ? 1 : 4; 

    } while (fgets(buff, 80, stdin) != NULL);

    hal_release(hp);
    return 0;
}

/***************************************
 * Display/modify device I2C registers *
 ***************************************/
static void print_i2creg_help(void) 
{
    printf("\nUsage: i2creg [-b bus | [-x] [-p port]] [-s] -a i2c_address -r reg [-R|-W val] [-V|-Q]\n");
    printf("Options: -V \t Print chip version,\n");
    printf("         -Q \t Query DV timings on ADV7604\n");
    printf("         -R \t Read register 'reg' non-interactively.\n");
    printf("         -W \t Write 'val' to register 'reg' non-interactively.\n");
    printf("         -x \t Display/Modify registers on expansion board\n");
    printf("         -s \t Don't print response (i.e. read silently)\n");
    printf("Otherwise display or modify registers starting at 'reg' on I2C bus 'bus'.\n");
    printf("All numbers are decimal, unless prefixed with '0x' in which case\n");
    printf("they're hexadecimal.\n");
    printf("Valid I2C addresses are:\n");
    printf("ADV7612: 0x4c, ADV7604: 0x20, SII7172: 0x38.\n");
    printf("Register maps for ADV7612, ADV7604 and SII7172 are:\n");
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
    printf("MONEDID          : Bus 1 Addr 0x50\n");
    printf("Si5338           : Bus 5 Addr 0x70\n");
    printf("LM63             : Bus 6 Addr 0x4c\n");
    printf("ICS9FG104        : Bus 6 Addr 0x6e\n");
    printf("Max6650          : Bus 6 Addr 0x1b\n");
    printf("Expansion Switch : Bus 7 Addr 0x70\n");
};

static int is_v4l2_subdev(int addr)
{
    if (addr == ADV7604_I2C_ADDR || addr == ADV7612_I2C_ADDR || 
            addr == SII7172_I2C_ADDR)
        return 1;
    return 0;
}

static int find_default_port(int addr, int expansion) 
{
    if (expansion) {
        if (addr == TLV320AIC3254_I2C_ADDR)
            return TLV320AIC3254_I2C_PORT;
        else if (addr == SPARTAN3AN_I2C_ADDR)
            return SPARTAN3AN_I2C_PORT;
        else if (addr == BOARDIDEEPROM_I2C_ADDR)
            return BOARDIDEEPROM_I2C_PORT;
        else if (addr == LM63_I2C_ADDR)
            return EXPANSIONLM63_I2C_PORT;
        else if (addr == MAX6650_I2C_ADDR)
            return EXPANSIONMAX6650_I2C_PORT;
        else
            return -1;
    }
    else if (addr == ADV7604_I2C_ADDR || addr == ADV7612_I2C_ADDR)
        return ADV7612_I2C_PORT;
    else if (addr == SII7172_I2C_ADDR)
        return SII7172_I2C_PORT;
    else if (addr == LM63_I2C_ADDR)
        return LM63_I2C_PORT;
    else if (addr == ICS9FG104_I2C_ADDR)
        return ICS9FG104_I2C_PORT;
    else if (addr == MAX6650_I2C_ADDR)
        return MAX6650_I2C_PORT;
    else if (addr == SII7172_SLAVE_I2C_ADDR)
        return SII7172_SLAVE_I2C_PORT;
    else
        return -1;
}

static int setup_audio_main(int argc, char **argv) {
    int rc, count;
    HAL *hp;
    REGVAL buff[] = {
        {0x01, 0x01},
        {0x00, 0x00},
        {0x12, 0x88},
        {0x13, 0x82},
        {0x14, 0x80},
        {0x0b, 0x88},
        {0x0c, 0x82},
        {0x0d, 0x00},
        {0x0e, 0x80},
        {0x1d, 0x06},
        {0x1e, 0x84},
        {0x21, 0x10},
        {0x1b, 0xcc},
        {0x1c, 0x01},
        {0x00, 0x01},
        {0x01, 0x08},
        {0x02, 0x00},
        {0x00, 0x00},
        {0x51, 0xc0},
        {0x52, 0x00}, 
        {0x00, 0x01},
        {0x34, 0x80},
        {0x36, 0x80},
        {0x37, 0x80},
        {0x39, 0x80},
        {0x3b, 0x09},
        {0x3c, 0x09},
        {0x00, 0x00},
        {0x3f, 0xd6},
        {0x40, 0x00},
        {0x00, 0x01},
        {0x33, 0x50},
        {0x0e, 0x08},
        {0x0f, 0x08}, 
        {0x0c, 0x08},
        {0x0d, 0x08},
        {0x12, 0x00},
        {0x13, 0x00}, 
        {0x10, 0x3a},
        {0x11, 0x3a},
        {0x09, 0x3f} 
    };

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed: No hardware detected\n");
        return 0;
    }

    //reset audio chip
   hal_and_in_s3fpga_val32_k(hp, 0x18, 0xffffffef);
   usleep(1000);
   hal_or_in_s3fpga_val32_k(hp, 0x18, 0x10);
   //enable sync to fpga
   hal_or_in_s3fpga_val32_k(hp, 0x10, 0x800);
   usleep(1000);

    for (count = 0; count < sizeof(buff)/sizeof(REGVAL); count++) {
        rc = xpi_set_expansion_i2c_val(hp, TLV320AIC3254_I2C_PORT,
                TLV320AIC3254_I2C_ADDR, buff[count].reg, buff[count].val);
        if (rc < 0) {
            printf("error %d writing I2C addr 0x%x reg 0x%x\n", 
                    rc, buff[count].reg, buff[count].val);
            hal_release(hp);
            return -1;
        }
    }

    hal_release(hp);
    return 0;
}

static int i2creg_main(int argc, char **argv) {
    int opt, addr = -1, bus = -1, port = -1, reg = 0, fd, rc = 0;
    int chipid = 0, query = 0, rw_option = OPTION_NONE;
    int silent = 0, expansion = 0, verbose = 0;
    char *dev_name;
    unsigned char cval;
    char buff[80];
    HAL *hp;

    while ((opt = getopt(argc, argv, "b:p:a:r:RW:xsQVvh")) > 0) {
        switch (opt) {
            case 'b':
                bus = getint(optarg);
                break;
            case 'p':
                port = getint(optarg);
                break;
            case 'a':
                addr = getint(optarg);
                break;
            case 'r':
                reg = getint(optarg);
                break;
            case 'R':
                rw_option = OPTION_READ;
                break;
            case 'W':
                rw_option = OPTION_WRITE;
                cval = (unsigned char) getint(optarg);
                break;
            case 'x':
                expansion = 1;
                break;
            case 's':
                silent = 1;
                break;
            case 'Q':
                query = 1;
                break;
            case 'V':
                chipid = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
            case '?':
            default:
                print_i2creg_help();
                return 0;
        }
    }
    if (chipid && !is_v4l2_subdev(addr)) {
        printf("-V is only supported on ADV7604, ADV7612 and SII7172\n");
        return -1;
    }
    if (query && addr != ADV7604_I2C_ADDR && addr != ADV7612_I2C_ADDR) {
        printf("-Q is only supported on ADV7604 and ADV7612\n");
        return -1;
    }
    if (expansion && bus != -1) {
        printf("-b is not allowed when -x is specified. use -p with -x.\n");
        return -1;
    }
    if (bus >= 2) {
        port = bus - 2;
    }
    if (bus != 1 && port < 0) {
        port = find_default_port(addr, expansion);
    }
    if (bus == 0 || (bus != 1 && port < 0)) {
        print_i2creg_help();
        return -1;
    }

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed: No hardware detected\n");
        return 0;
    }

    /* Handle non-interactive commands */
    if (chipid) {
        unsigned char val_lo, val_hi;
        if (addr == ADV7604_I2C_ADDR) {
            if (xpi_get_main_i2c_val(hp, port, addr, 0x11, &val_lo) < 0)
                printf("Failed to read ADV7604 RD_INFO register\n");
            else
                printf("Revision = %02x\n", val_lo);
        }
        else if (addr == ADV7612_I2C_ADDR) {
            if (xpi_get_main_i2c_val(hp, port, addr, 0xeb, &val_lo) < 0)
                printf("Failed to read ADV7612 RD_INFO register\n");
            else
                printf("Revision = %02x%02x\n", val_hi, val_lo);
        }
        else {
            if (xpi_get_main_i2c_val(hp, port, addr, 0x04, &val_hi) < 0)
                printf("Failed to read SII7172 Master DEV_REV register\n");
            else if (xpi_get_main_i2c_val(hp, port, addr, 0x104, &val_lo) < 0)
                printf("Failed to read SII7172 Slave DEV_REV register\n");
            else
                printf("Revision = %02x (Master), %02x (Slave)\n", 
			        val_hi, val_lo);
        }
        hal_release(hp);
        return 0;
    } 

    if (query && (addr == ADV7604_I2C_ADDR || addr == ADV7612_I2C_ADDR)) {
        int active = hal_getActiveIntf(hp);
        int fd = hal_get_video_fd(hp);
        struct v4l2_dv_timings timings;
        if (fd < 0) {
            printf("Failed to open /dev/video0 as I2C device\n");
            hal_release(hp);
            return 0;
        }
        if (hal_v4l2_g_dv_timings(hp, &timings) == 0) {  
            struct v4l2_bt_timings *bt = &(timings.bt);
            printf("DV timings:\n");
            switch(timings.type) {
            case V4L2_DV_BT_656_1120:
                printf("\tWidth: %d\n", bt->width);
                printf("\tHeight: %d\n", bt->height);
                printf("\tFrame format: %s\n",
                    bt->interlaced ? "Interlaced" : "Progressive");
                printf("\tPolarities: %d\n", bt->polarities);
                printf("\tPixel Clock: %lld\n", bt->pixelclock);
                printf("\tHorizontal Front Porch: %d\n", bt->hfrontporch);
                printf("\tHorizontal Synch: %d\n", bt->hsync);
                printf("\tHorizontal Back Porch: %d\n", bt->hbackporch);
                printf("\tVertical Front Porch: %d\n", bt->vfrontporch);
                printf("\tVertical Synch: %d\n", bt->vsync);
                printf("\tVertical Back Porch: %d\n", bt->vbackporch);
                break;
            default:
                printf("Timing type not defined\n");
                break;
            }
        }
        if (active & INTERFACE_HV) {
            struct stdi_io stdiio;
            stdiio.subdev = V4L2_SUBDEV_ADV7604;
            printf("STDI timings:\n");
            if (ioctl(fd, FPGA_IOC_QUERY_STDI, &stdiio) < 0) {
                printf("FPGA_IOC_QUERY_STDI failed. errno %d %m\n", errno);
            }
            else {
                printf("\tlcf (frame height - 1) = %d\n", stdiio.stdi.lcf);
                printf("\tbl = %d\n", stdiio.stdi.bl);
                printf("\tfcl = %d\n", stdiio.stdi.fcl);
                printf("\tlcvs (vsync) = %d\n", stdiio.stdi.lcvs);
                printf("\t%s\n", 
                        stdiio.stdi.interlaced ? "interlaced" : "progressive");
                printf("\t%chsync, %cvsync\n", 
                        stdiio.stdi.hs_pol, stdiio.stdi.vs_pol);
            }
        }
        hal_release(hp);
        return 0;
    }

    if (rw_option == OPTION_READ) {
        if (bus == 1) 
            rc = xpi_get_monedid_val(hp, reg, &cval);
        else if (!expansion) 
            rc = xpi_get_main_i2c_val(hp, port, addr, reg, &cval);
        else 
            rc = xpi_get_expansion_i2c_val(hp, port, addr, reg, &cval);
        if (rc < 0) 
            printf("error %d reading I2C addr 0x%x reg 0x%x\n", rc, addr, reg);
        else 
            printf("0x%02x\n", cval);
        hal_release(hp);
        return rc;
    }
    if (rw_option == OPTION_WRITE) {
        if (bus == 1) 
            rc = xpi_set_monedid_val(hp, reg, cval);
        else if (!expansion) 
            rc = xpi_set_main_i2c_val(hp, port, addr, reg, cval);
        else 
            rc = xpi_set_expansion_i2c_val(hp, port, addr, reg, cval);
        if (rc < 0) 
            printf("error %d writing I2C addr 0x%x reg 0x%x\n", rc, addr, reg);
        hal_release(hp);
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
                if (bus == 1) 
                    rc = xpi_set_monedid_val(hp, reg, cval);
                else if (!expansion) 
                    rc = xpi_set_main_i2c_val(hp, port, addr, reg, cval);
                else
                    rc = xpi_set_expansion_i2c_val(hp, port, addr, reg, cval);
                if (rc < 0) {
                    printf("error %d writing I2C addr 0x%x reg 0x%x\n", 
                            rc, addr, reg);
                    break;
                }
            }
        }
        if (bus == 1) 
            rc = xpi_get_monedid_val(hp, reg, &cval);
        else if (!expansion) 
            rc = xpi_get_main_i2c_val(hp, port, addr, reg, &cval);
        else
            rc = xpi_get_expansion_i2c_val(hp, port, addr, reg, &cval);
        if (rc < 0) {
            printf("error %d reading I2C addr 0x%x reg 0x%x\n", rc, addr, reg);
            break;
        }
        if (silent)
            printf("0x%04x -> ", reg);
        else
            printf("0x%04x == 0x%02x -> ", reg, cval);
        reg += 1;
    } while (fgets(buff, 64, stdin) != NULL);

    hal_release(hp);

    return 0;
}

/*****************************************
 * Display/modify MDIO/MAC/DRP registers *
 *****************************************/
static void print_reg_help(char *progname) {
    char prg[4];

    if (strcasecmp(progname, "mdioreg") == 0) {
        printf("\nUsage: mdioreg [-o offset] [-p page] [-r regad] [-D] [-R] [-W val] \n");
        printf("Display or modify MDIO registers on PHYAD 1 offsetted from\n");
        printf("MDIO_BASE of 0x2800 by 'off', page 'page' starting at REGAD 'regad'.\n");
        printf("'off', 'page', 'regad' and 'val' are decimal, or if prefixed\n");
        printf("with '0x', hexadecimal numbers.\n");
    }
    else if (strcasecmp(progname, "macreg") == 0) {
        printf("\nUsage: macreg [-o offset] [-r regad] [-D] [-R] [-W val] \n");
        printf("Display or modify MAC registers at offset 'off' from MDIO_BASE\n");
        printf("of 0x2800, starting at REGAD 'regad'.\n");
        printf("'off', 'regad' and 'val' are decimal, or if prefixed with '0x',\n");
        printf("hexadecimal numbers.\n");
    }
    else {
        prg[0] = toupper(progname[0]);
        prg[1] = toupper(progname[1]); 
        prg[2] = toupper(progname[2]); 
        prg[3] = 0;
        printf("\nUsage: %s [-r regad] [-o off] [-D] [-R] [-W val] \n", progname);
        printf("Display or modify %s registers at offset 'off' from MDIO_BASE\n", prg);
        printf("of 0x2800, starting at REGAD 'regad'.\n");
        printf("'off', 'regad' and 'val' are decimal, or if prefixed with '0x',\n");
        printf("'regad' and 'val' are decimal, or if prefixed with '0x',\n");
        printf("hexadecimal numbers.\n");
    };
    printf("Options: -D      Turn on debug\n"); 
    printf("         -R      Read register 'reg' non-interactively.\n");
    printf("         -W val  Write 'val' to register 'reg' non-interactively.\n");
};

static int parse_mdioreg_args(int argc, char **argv, int *off, int *page, 
        int *regad, unsigned long *val) 
{
    int opt, rc = OPTION_NONE;

    while ((opt = getopt(argc, argv, "p:r:o:RW:h")) > 0) {
        switch (opt) {
        case 'o':
            *off = getint(optarg);
            break;
        case 'p':
            *page = getint(optarg);
            break;
        case 'r':
            *regad = getint(optarg);
            break;
        case 'R':
            rc = OPTION_READ;
            break;
        case 'W':
            rc = OPTION_WRITE;
            *val = getint(optarg);
            break;
        case 'h':
        case '?':
        default:
            print_reg_help(argv[0]);
            return OPTION_INVALID;
        }
    }
    if (*regad < 0) {
        print_reg_help(argv[0]);
        return OPTION_INVALID;
    }
    return rc;
}

static int parse_genreg_args(int argc, char **argv, int *off, int *regad, 
        unsigned long *val) 
{
    int opt, rc = OPTION_NONE;

    while ((opt = getopt(argc, argv, "r:o:RW:h")) > 0) {
        switch (opt) {
        case 'o':
            *off = getint(optarg);
            break;
        case 'r':
            *regad = getint(optarg);
            break;
        case 'R':
            rc = OPTION_READ;
            break;
        case 'W':
            rc = OPTION_WRITE;
            *val = getint(optarg);
            break;
        case 'h':
        case '?':
        default:
            print_reg_help(argv[0]);
            return OPTION_INVALID;
        }
    }
    if (*regad < 0) {
        print_reg_help(argv[0]);
        return OPTION_INVALID;
    }
    return rc;
}

static int reg_main(int argc, char **argv) {
    int fd, rc = 0, off = 0, page = 0, regad = -1, rw_option = OPTION_INVALID;
    unsigned long val;
    char buff[80];
    int (*get_func) (HAL *, int, int, int, unsigned long *);
    void (*set_func) (HAL *, int, int, int, unsigned long);
    int delta = 1;
    HAL *hp;

    if (strcasecmp(argv[0], "mdioreg") == 0) {
        if ((rw_option = 
                parse_mdioreg_args(argc, argv, &off, &page, &regad, &val)) < 0)
            return -1;
        get_func = hal_get_mdio_val_k;
        set_func = hal_set_mdio_val_k;
    }
    else {
        if ((rw_option = 
                parse_genreg_args(argc, argv, &off, &regad, &val)) < 0)
            return -1;
        if (strcasecmp(argv[0], "macreg") == 0) {
            get_func = hal_get_mac_val_k;
            set_func = hal_set_mac_val_k;
        }
        else {
            printf("%s not implmented\n", argv[0]);
            return -1;
        }
    }

    if (rw_option == OPTION_INVALID)
        return -1;

    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed to access hardware.\n");
        return -1;
    }

    if (rw_option == OPTION_READ) {
        rc = get_func(hp, off, page, regad, &val);
        printf("0x%04x == 0x%08lx\n", regad, val);
        hal_release(hp);
        return rc;
    }
    else if (rw_option == OPTION_WRITE) {
        set_func(hp, off, page, regad, val);
        hal_release(hp);
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
                set_func(hp, off, page, regad, val);
            }
        }
        rc = get_func(hp, off, page, regad, &val);
        if (rc < 0) {
            printf("error reading page %d regad 0x%x. errno %d (%m).\n", 
                    page, regad, errno);
            break;
        }
        printf("0x%04x == 0x%08lx -> ", regad, val);
        regad += delta;
    } while (fgets(buff, 64, stdin) != NULL);

    hal_release(hp);

    return 0;
}
static void print_network_init_help(void) 
{
    printf("\nUsage: network_init [-p phy_to_init] 0 for PHY0, "
            "1 for PHY1 and 2 for Both, Default is 2.\n");
}

static int network_init_main(int argc, char **argv)  {
    int fd, rc = 0, off = 0;
    int i, ostart = 0, ostop = 0x401, opt;
    int phy_to_init = 2;
    unsigned long val;
    REGVAL buff12[] = {
        {0x14, 0x8001}
    };
    REGVAL buff1[] = {
        {0x1a, 0x0046},
        {0x10, 0xc00e},
        {0x00, 0x1340}
    };
    REGVAL buff0[] = {
        {0x12, 0x6400}
    };
    REGVAL buff3[] = {
        {0x10, 0x17aa},
        {0x11, 0x4415},
        {0x12, 0x0885}
    };
    HAL *hp;

    while ((opt = getopt(argc, argv, "p:h")) > 0) {
        switch (opt) {
        case 'p':
            phy_to_init = getint(optarg);
            break;
        case 'h':
            print_network_init_help();
            exit(0);
        }
    }
    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed to access hardware.\n");
        return -1;
    }

    switch (phy_to_init) {
    case 0:
        ostart = 0;
        ostop  = 0x1;
        break;
    case 1:
        ostart = 0x400;
        ostop  = 0x401;
        break;
    case 2:
        ostart = 0x0;
        ostop  = 0x401;
        break;
    default : 
        print_network_init_help();
        hal_release(hp);
        exit(0);
    }

    //reset PHY
    if ((phy_to_init == 0) || (phy_to_init == 2))
        xpi_set_gpio_pin_value(GPIO_PHY0_RSTn, 0);
    if ((phy_to_init == 1) || (phy_to_init == 2))
        hal_and_in_s3fpga_val32_k(hp,0x18,0xFFFFFFF7); 

    //setting mac clock divider and enabling mdio
    for (off = ostart; off < ostop; off = off + 0x400) {
        hal_set_mac_val_k(hp, off, XPI_MDIO_PHYAD, 0x340, 0x4d);
    }

    usleep(500000);

    //remove PHY reset
    if ((phy_to_init == 0) || (phy_to_init == 2))
        xpi_set_gpio_pin_value(GPIO_PHY0_RSTn, 1);
    if ((phy_to_init == 1) || (phy_to_init == 2))
        hal_or_in_s3fpga_val32_k(hp,0x18,0x8); 

    usleep(5000);

    for(off = ostart; off < ostop; off = off + 0x400) {
        hal_get_mdio_val_k(hp, off, 0x0, 0x0, &val);
        printf("%s, Phy %d Page 0x0 Reg 0x0 Value 0x%x.\n", __func__,  
                XPI_MDIO_PHYAD, (int) val);
    } 

    for (off = ostart; off < ostop; off = off + 0x400) {
        for (i=0; i < sizeof(buff12)/sizeof(REGVAL); i++) 
            hal_set_mdio_val_k(hp, off, 0x12, buff12[i].reg, buff12[i].val);
        for (i=0; i < sizeof(buff1)/sizeof(REGVAL); i++) 
            hal_set_mdio_val_k(hp, off, 0x1, buff1[i].reg, buff1[i].val);
        for (i=0; i < sizeof(buff0)/sizeof(REGVAL); i++) 
            hal_set_mdio_val_k(hp, off, 0x0, buff0[i].reg, buff0[i].val);
        for (i=0; i < sizeof(buff3)/sizeof(REGVAL); i++) 
            hal_set_mdio_val_k(hp, off, 0x3, buff3[i].reg, buff3[i].val);
    }

    hal_release(hp);

    return 0;
}

/**********************************
 * Routine to get/set addside LUT *
 **********************************/
static void print_getaddsidelut_help(void)
{
    printf("\nUsage: getaddsidelut [-d]\n");
    printf("Displays current addside LUT. The -d option turns on debug.\n");
}

static int getaddsidelut_main(int argc, char **argv) 
{
    int opt;
    HAL *hp;
    struct LUT_struct lut;

    while ((opt = getopt(argc, argv, "dh")) > 0) {
        switch (opt) {
        case 'd':
            debug = 1;
            break;
        case 'h':
            print_getaddsidelut_help();
            exit(0);
        }
    }

    hp = hal_acquire(0);
   
    if (hp == NULL) {
        printf("Failed. Unable to access hardware\n");
        return -1;
    }
    if (hal_get_addside_lut(hp, &lut) == 0) {
        printf("\nCurrent Addside LUT:\n");
        printf("LUT ID = %d (%dx%d@%f) (%s) PCR = %f\n", lut.a_alutreg, 
                lut.a_hres, lut.a_vres, lut.a_refresh, 
                lut.comment, lut.a_pcr);
        if (debug) {
            printf("HRES = %d, HFP = %d, HSYNC = %d, HBP = %d\n",
                lut.a_hres, lut.a_hfp, lut.a_hsync, lut.a_hbp);
            printf("VRES = %d, VFP = %d, VSYNC = %d, VBP = %d\n",
                lut.a_vres, lut.a_vfp, lut.a_vsync, lut.a_vbp);
        }
    }
    else {
        printf("No addside LUT found.\n");
    }

    hal_release(hp);

    return 0;
}

/**************************
 * Routine to get/set PLL *
 **************************/
static void print_setpll_help(void)
{
    printf("\nUsage: setpll -f freq (in Hz)\n");
    printf("Set current output frequency to 'freq' (Hz) for PLL 1 port 2\n");
}

static int setpll_main(int argc, char **argv) 
{
    int opt;
    unsigned int freq = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "f:h")) > 0) {
        switch (opt) {
        case 'f':
            freq = getint(optarg);
            break;
        case '?':
        case 'h':
            print_setpll_help();
            break;
        }
    }

    if (freq == 0) {
        print_setpll_help();
        return -1;
    }
    
    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed. Unable to access hardware\n");
        return -1;
    }

    printf("Setting PLL %d output %d frequency to %d (Hz), stepsize 500\n", 
            PLL_1, SI_PLL_2, freq);

    xpi_set_pll_frequency(hp, (double) freq / (double) 1000000);

    hal_release(hp);

    return 0;
}

static void print_getpll_help(void)
{
    printf("\nUsage: getpll [-P port]\n");
    printf("Display current output frequency for port 2\n");
}

static int getpll_main(int argc, char **argv) 
{
    int opt, port = SI_PLL_2;
    unsigned long freq = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "h")) > 0) {
        switch (opt) {
        case '?':
        case 'h':
            print_getpll_help();
            break;
        }
    }

    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed. Unable to access hardware\n");
        return -1;
    }

    printf("Retrieving PLL %d output %d frequency...\n", PLL_1, port);
    freq = xpi_get_pll_frequency(hp);
    printf("PLL %d output %d frequency = %lu (Hz)\n", PLL_1, port, freq);
    hal_release(hp);
    return 0;
}

/******************************************
 * Routine to get and set interface flags *
 ******************************************/


/*******************************
 * Routine to get and set EDID *
 *******************************/
#define MAX_LINE_SIZE	80

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

static void print_setedid_help(void) {
    printf("\nUsage: setedid [-a address] [-f filename | -A]\n");
    printf("\tIf the -f switch is present, sets the EDID for the device at\n");
    printf("\tI2C address 'address' to that in 'filename', otherwise sets \n");
    printf("\tit to the configured EDID. If -A is present when setting to \n");
    printf("\tto configured EDID, set the EDID to analog instead of digital\n");
};

static int setedid_main(int argc, char **argv) {
    int opt, addr = -1, rc = 0, len = 0, analog = 0;
    unsigned char buf[256];
    char *filename = NULL;
    HAL *hp;
    FILE *file;

    while ((opt = getopt(argc, argv, "f:a:Ah")) > 0) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        case 'a':
            addr = getint(optarg);
            break;
        case 'A':
            analog = 1;
            break;
        case 'h':
        case '?':
        default:
            print_setedid_help();
            return 0;
        }
    }
    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (filename == NULL) {
        load_edid_eeprom_with_descriptor(hp, (analog) ? 0x00 : 0x80);
        hal_release(hp);
        return 0;
    }
    if ((file = fopen(filename, "r")) == NULL) {
        printf("Can't open input file %s. %m.\n", filename);
        hal_release(hp);
        return -1;
    }
    memset(buf, 0, 256);
    if (((len = read_hex_from_file(file, buf, 256, 0)) != 256) &&
            len != 128) {
        printf("Data file must have 128 or 256 bytes of data\n");
        fclose(file);
        hal_release(hp);
        return -1;
    }
    fclose(file);
    if (hal_load_edid_eeprom(hp, buf, 256) < 0) {
        printf("Failed to load EDID to ADV7604\n");
        rc = -1;
    }
    else {
        printf("Successfully wrote EDID to ADV7604\n");
    }
    hal_release(hp);
    return rc;
}

#define PRINTBUFF_SIZE  (80 * 24 * 10)

static void print_printedid_help(void) {
    printf("\nUsage: printedid [-f filename | -a address | -m]\n");
    printf("\tOne of -f, -a or -m MUST be present.\n");
    printf("\t-f filename  Prints EDID data from file 'filename'\n");
    printf("\t-a address   Prints EDID for the chip at the 2C addresss \n");
    printf("\t             'address' on the main board.\n");
    printf("\t-m           Prints EDID of the attached monitor\n");
    printf("'address' is a decimal number unless prefixed with '0x'\n");
    printf("The only 'address' supported right now is the ADV7604 address\n");
};

static void print_hex(unsigned char *buff, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buff[i]);
        if (i % 16 == 15) printf("\n");
    }
    printf("\n");
}

static int printedid_main(int argc, char **argv) {
    char *name = NULL;
    int opt, i, data_size, address = -1, fd, monedid = 0;
    unsigned char edid_data[256];
    char *printbuff;
    HAL *hp;

    while ((opt = getopt(argc, argv, "hf:a:m")) > 0) {
        switch (opt) {
        case 'f':
            name = optarg;
            break;
        case 'a':
            address = getint(optarg);
            break;
        case 'm':
            monedid = 1;
            break;
        case '?':
        case 'h':
            print_printedid_help();
            return -1;
        }
    }
    if (name == NULL && address != ADV7604_I2C_ADDR && !monedid) {
        print_printedid_help();
        return -1;
    }
    if ((printbuff = malloc(PRINTBUFF_SIZE)) == NULL) {
        printf("Failed to allocate print buffer. Exiting.\n");
        return -1;
    }
    if (name) {
        FILE *file = NULL;
        memset(edid_data, 0, sizeof(edid_data));
        if ((file = fopen(name, "r")) < 0 || file == NULL) {
            printf("Can't open input file %s. %m.\n", name);
            return -1;
        }
        data_size = read_hex_from_file(file, edid_data, 256, 0);
        fclose(file);
        printf("\nRead %d bytes data from file %s:\n\n", data_size, name);
        print_edid_to_buffer(edid_data, printbuff, NULL, 1);
        printf("%s\n", printbuff);
        if (data_size > 128) {
            if (print_edid_extension_to_buffer(&(edid_data[128]), printbuff, 
                    NULL, 1) >= 0) {
                printf("\nExtended EDID Data\n");
                printf("%s\n", printbuff);
            }
            else {
                print_hex(&(edid_data[128]), data_size - 128);
            }
        }
    }
    else {
        HAL *hp = hal_acquire(0);
        if (hp == NULL) {
            printf("Failed to access hardware. errno %d. %m\n", errno);
            return -1;
        }
        data_size = sizeof(edid_data);
        if (monedid) {
            if (hal_read_mon_edid(hp, edid_data, data_size) < 0) {
                printf("Failed to detect monitor\n");
                hal_release(hp);
                return -1;
            }
        }
        else 
            hal_read_edid_eeprom(hp, edid_data, data_size);
        hal_release(hp);
        print_edid_to_buffer(edid_data, printbuff, NULL, 1);
        printf("%s\n", printbuff);
        if (data_size > 128) {
            if (print_edid_extension_to_buffer(&edid_data[128], printbuff, 
                        NULL, 1) >= 0) {
                printf("\nExtended EDID Data\n");
                printf("%s\n", printbuff);
            }
            else {
                print_hex(&edid_data[128], data_size - 128);
            }
        }
    }
    return 0;
}

/*************************************
 * Routine to get and set USB EEPROM *
 *************************************/
static void print_seteeprom_help(void) {
    printf("\nUsage: seteeprom -f filename -p port -a addr\n");
    printf("Sets EEPROM for the chip at the 2C port 'port', address 'addr' on\n");
    printf("the expansion board to data in file 'filename'. 'port' and 'addr'\n");
    printf("are decimal unless prefixed with '0x' in which case they're hex.\n");
    printf("Host and Device USBEEPROMs are on port 2 and 7, resp. on expansion switch\n");
};

static int seteeprom_main(int argc, char **argv) {
    int opt, fd, addr = -1, port = -1, rc = 0, i;
    char *filename = NULL;
    unsigned char buff[256];
    HAL *hp;
    FILE *file;
    struct expansion_io expio;
    unsigned long s3regval;

    while ((opt = getopt(argc, argv, "a:p:f:h")) > 0) {
        switch (opt) {
        case 'a':
            addr = getint(optarg);
            break;
        case 'p':
            port = getint(optarg);
            break;
        case 'f':
            filename = optarg;
            break;
        case 'h':
        case '?':
        default:
            print_seteeprom_help();
            return 0;
        }
    }
    if (filename == NULL || port < 0 || addr < 0) {
        print_seteeprom_help();
        return -1;
    }
    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed: No hardware detected\n");
        return 0;
    }
    if ((file = fopen(filename, "r")) < 0) {
        printf("Can't open input file %s. %m.\n", filename);
        hal_release(hp);
        return -1;
    }
    if (read_hex_from_file(file, buff, 256, 0) != 256) {
        printf("Data file must have 256 bytes of data\n");
        fclose(file);
        hal_release(hp);
        return -1;
    }
    fclose(file);

    fd = hal_get_video_fd(hp);

    if (fd < 0) {
        printf("Failed to open  V4L2 device. errno %d. %m.\n", errno);
        hal_release(hp);
        return -1;
    }

    /* write to eeprom */
    expio.type = EXPANSIONIO_TYPE_SET;
    expio.port = port;
    expio.addr = addr;
    for (i = 0; i < 256; i++) {
        expio.reg = i;
        expio.value = buff[i];
        if ((rc = ioctl(fd, FPGA_IOC_MOD_EXPANSION, &expio)) < 0) {
            printf("FPGA_IOC_MOD_EXPANSION failed. reg = 0x%x. rc = %d\n",
                    expio.reg, rc);
            hal_release(hp);
            return rc;
        }
    }
    printf("Successfully set chip at port %d I2C address 0x%x to data in %s\n",
            port, addr, filename);
    hal_release(hp);
    return 0;
}

static void print_printeeprom_help(void) {
    printf("\nUsage: printeeprom -p port -a addr\n");
    printf("Prints EEPROM data for the chip at port 'port' address 'addr' on the\n");
    printf("the expansion board. 'port' and 'addr' are decimal unless prefixed with\n");
    printf("'0x' in which case they'e hex.\n");
    printf("Host and Device USBEEPROMs are on port 2 and 7, resp. on expansion switch\n");
    printf("E.g. printeeprom -p 2 -a 0x50 will print the content of the host usbeeprom\n"); 
};

static int printeeprom_main(int argc, char **argv) {
    char *name = NULL;
    int opt, i, addr = -1, port = -1, fd, rc;
    HAL *hp;
    struct expansion_io expio;

    while ((opt = getopt(argc, argv, "ha:p:")) > 0) {
        switch (opt) {
        case 'a':
            addr = getint(optarg);
            break;
        case 'p':
            port = getint(optarg);
            break;
        case '?':
        case 'h':
            print_printeeprom_help();
            return -1;
        }
    }
    if (port < 0 || addr < 0) {
        print_printeeprom_help();
        return -1;
    }

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if ((fd  = hal_get_video_fd(hp)) < 0) {
        printf("Failed to open V4L2 device. errno %d. %m\n", errno);
        hal_release(hp);
        return -1;
    }                                
    expio.type = EXPANSIONIO_TYPE_GET;
    expio.port = port;
    expio.addr = addr;
    for (i = 0; i < 256; i++) {
        expio.reg = i;
        if ((rc = ioctl(fd, FPGA_IOC_MOD_EXPANSION, &expio)) < 0) {
            printf("FPGA_IOC_MOD_EXPANSION failed. reg = 0x%x. rc = %d\n",
                    expio.reg, rc);
            hal_release(hp);
            return rc;
        }
        else {
            printf("%02x ", expio.value);
            if (i % 16 == 15) printf("\n");
        }
    }
    hal_release(hp);
    return 0;
}

/*******************************
 * Routine to set monitor EDID *
 *******************************/
static void print_setmonedid_help(void) {
    printf("\nUsage: setmonedid -f filename\n");
    printf("\tSets the EDID for the attached monitor to the content in the\n");
    printf("\tfile 'filename'.\n");
};

static int setmonedid_main(int argc, char **argv) {
    int opt, addr = -1, rc = 0, len = 0, i;
    unsigned char buf[256];
    char *filename = NULL;
    HAL *hp;
    FILE *file;

    while ((opt = getopt(argc, argv, "f:")) > 0) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        case 'h':
        case '?':
        default:
            print_setmonedid_help();
            return 0;
        }
    }
    if (filename == NULL) {
        print_setmonedid_help();
        return -1;
    }
    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if ((file = fopen(filename, "r")) == NULL) {
        printf("Can't open input file %s. %m.\n", filename);
        hal_release(hp);
        return -1;
    }
    memset(buf, 0, 256);
    if (((len = read_hex_from_file(file, buf, 256, 0)) != 256) &&
            len != 128) {
        printf("Data file must have 128 or 256 bytes of data\n");
        fclose(file);
        hal_release(hp);
        return -1;
    }
    fclose(file);
    for (i = 0; i < len; i++) {
        if ((rc = xpi_set_monedid_val(hp, i, buf[i]) < 0)) {
            printf("Error writing to monitor EDID\n");
            break;
        }
    }
    if (rc == 0)
        printf("Successfully wrote '%s' to monitor EDID\n", filename);
    hal_release(hp);
    return rc;
}


/*******************************************
 * Routine to display temperature readings *
 *******************************************/
static void print_temperature_help(void)
{
    printf("Usage: temperature [-f] [-p] [-c] [-l]\n");
    printf("Displays the FPGA, PHY, CPU and LM63 temperatures on the XPi, resp.\n");
    printf("All temperatures are displayed if no option is present.\n");
}

static int temperature_main(int argc, char **argv) {
    char *name = NULL;
    int opt, do_fpga = 0, do_phy = 0, do_cpu = 0, do_lm63 = 0;
    HAL *hp;
    float temperature;

    while ((opt = getopt(argc, argv, "hfpcl")) > 0) {
        switch (opt) {
        case 'f':
            do_fpga = 1;
            break;
        case 'p':
            do_phy = 1;
            break;
        case 'c':
            do_cpu = 1;
            break;
        case 'l':
            do_lm63 = 1;
            break;
        case '?':
        case 'h':
            print_temperature_help();
            return -1;
        }
    }
    if (!do_fpga && !do_phy && !do_cpu && !do_lm63) 
        do_fpga = do_phy = do_cpu = do_lm63 = 1;

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (do_fpga) {
        hal_get_xpi_fpga_temperature(hp, &temperature);
        printf("FPGA temperature = %f (centigrade)\n", temperature);
    }
    if (do_phy) {
        hal_get_xpi_phy_temperature(hp, &temperature);
        printf("PHY temperature = %f (centigrade)\n", temperature);
    }
    if (do_cpu) {
        hal_get_xpi_cpu_temperature(hp, &temperature);
        printf("CPU internal = %f (centigrade)\n", temperature);
    }
    if (do_lm63) {
        float remote_temperature;
        hal_get_board_temperature(hp, &temperature, &remote_temperature);
        printf("LM63 temperature = %f / %f (centigrade)\n", temperature, 
                remote_temperature);
    }
    hal_release(hp);
    return 0;
}

/* 
 * bit2fcp - convert bit files to a binary file we can flashcp to the flash
 *           (i.e. remove the first 100 bytes of the .bit file)
 */
static int bit2fcp_main(int argc, char **argv) {
    FILE *ifile, *ofile;
    int opt, rc;
    unsigned char ch;
    char *ifilename = NULL, *ofilename = NULL;

    while ((opt = getopt(argc, argv, "i:o:h")) > 0) {
        switch (opt) {
            case 'i':
                ifilename = optarg;
                break;
            case 'o':
                ofilename = optarg;
                break;
            case '?':
            case 'h':
            default:
                printf("Usage: bit2fcp -i input-file -o output-file\n");
                return -1;
        }
    }
    if (ifilename == NULL || ofilename == NULL) {
        printf("Usage: bit2fcp -i input-file -o output-file\n");
        return -1;
    }
    if ((ifile = fopen(ifilename, "r")) < 0) {
        printf("bit2fcp: can't open input file %s. %m.\n", ifilename);
        return -1;
    }
    if ((ofile = fopen(ofilename, "wb")) < 0) {
        printf("bit2fcp: can't open output file %s. %m.\n", ofilename);
        return -1;
    }

    (void) fseek(ifile, (long) 101, SEEK_SET);
    while (1) {
        if (fread(&ch, sizeof(unsigned char), 1, ifile) == 1) 
            (void) fwrite(&ch, sizeof(unsigned char), 1, ofile);
        else
            break;
    }
    fclose(ifile);
    fclose(ofile);

    return 0;
}


/* 
 * s3_upgrade  - takes a bit file as input and programs Spartan 3 ISF with it
 * at given starting address
 */
static unsigned long isf_wait(HAL *hp, unsigned long count, unsigned long interval)
{
    unsigned long lval = 0;
    do {
        usleep(interval);
        hal_set_s3fpga_val32_k(hp, S3FPGA_ADDR, (ISF_CMD_STS_RD << 24 ) + 8);
        hal_set_s3fpga_val32_k(hp, S3FPGA_INIT_SM, 0x2);
        lval = hal_get_s3fpga_val32_k(hp, S3FPGA_DATA_IN);
        count-- ;

        if(count == 0) {
            printf("Status Register reports not ready.\n");
            break;
        }
    } while ((lval & 0x80) == 0) ;
    return count;
}
static int s3_upgrade_main (int argc, char **argv) {
    FILE *ifile ;
    int opt;
    unsigned char *cfile ;
    char *ifilename = NULL ;
    unsigned long  lval = 0, filesize;
    unsigned char myarray[1024] ;
    HAL *hp;
    unsigned long (*hal_get_func)(HAL *, unsigned long);
    int hardware_type;
    unsigned int discard_bytes = 100,PREPEND_BYTES= 39 ;
    unsigned long dec_count;
    unsigned int page, word_add;
    unsigned long count;
    unsigned int sector,sub_sector, partition;
    unsigned int MAX_PAGE,last_page, byte_count ; 
    unsigned int NO_OF_SSECTOR;


    while ((opt = getopt(argc, argv, "i:a:h")) > 0) {
        switch (opt) {
            case 'i':
                ifilename = optarg;
                break;
            case 'a':
                partition = atoi(optarg);
                break;
            case '?':
            case 'h':
            default:
                printf("Usage: s3upgrade -i input-file -a partition (0 for primary, 1 for secondary)\n");
                return -1;
        }
    }
    if (ifilename == NULL ) {
        printf("Usage: s3upgrade -i input-file -a partition (0 for primary, 1 for secondary)\n");
        return -1;
    }
    if ((ifile = fopen(ifilename, "r")) < 0) {
        printf("s3upgrade: can't open input file %s. %m.\n", ifilename);
        return -1;
    }
    (void) fseek(ifile, (long) 0, SEEK_END);
    filesize = ftell(ifile) - discard_bytes + PREPEND_BYTES ; //extra 39 bytes of 0xFF
    printf("Size of Input file to process is %ld.\n",filesize) ;

    rewind(ifile);

    cfile = calloc (filesize, sizeof(unsigned char)) ;

    if(cfile == NULL) {
        printf("\nInsufficient memory to read file.\n") ;
        fclose(ifile);
        return -1; 
    }
    else {

        (void) fseek(ifile, (long) discard_bytes + 1-PREPEND_BYTES, SEEK_SET);
        fread(cfile, sizeof(unsigned char), filesize, ifile) ;

        fclose(ifile);
        //adding dummy 39 bytes of 0xFF to fix mcs
        for (count = 0 ; count < (PREPEND_BYTES+1) ; count++)
            cfile[count] = 0xFF ;

        MAX_PAGE = (filesize / ISF_PAGESIZE) + 1 ;
        printf("MAX_PAGE is %d\n",MAX_PAGE);

        hp = hal_acquire(0);
        hardware_type = hal_get_hardware_type(hp);
        if (hp == NULL) {
            printf("Failed. Could not access hardware.\n");
            free(cfile);
            return -1;
        }
        if (hp && hardware_type != XPI_DEVICE) {
            print_dreg_maint_help();
            hal_release(hp);
            free(cfile);
            return 0;
        }

        //code to program flash
        //Sector erase
        byte_count = 4;
        for (sector = (partition*2); sector <((partition*2)+2); sector ++) {

            if(sector == 0) NO_OF_SSECTOR = 2;
            else            NO_OF_SSECTOR = 1;

            for(sub_sector = 0; sub_sector <NO_OF_SSECTOR; sub_sector ++) {
                printf("Erasing sector %d, sub_sector %d..\n",sector,sub_sector);
                hal_set_s3fpga_val32_k(hp, S3FPGA_ADDR, (ISF_CMD_SEC_ER << 24) + 
                        (sector << 22) + (sub_sector << 18) + byte_count);
                //printf("cmd reg prepared is %x.\n",(ISF_CMD_SEC_ER << 24) + 
                //(sector << 22) + byte_count);
                //printf("sector erase cmd reg %8lx.\n",hal_get_s3fpga_val32_k(hp,S3FPGA_ADDR) );
                hal_set_s3fpga_val32_k(hp, S3FPGA_INIT_SM, 0x2);
                dec_count = isf_wait(hp,100,100000); //max wait 10s, each loop 100ms
                //printf("Sector Erase of sector %d, sub sector %d took %ld00 ms.\n",sector, sub_sector, 100-dec_count );
            }
        }

        last_page = (partition*256) +MAX_PAGE  ;
        printf("Last page is %d.\n",last_page) ;

        byte_count = 8;

        for (page = (partition*256), count = 0;  page < last_page ; page++)  {
            //printf("Page count is %d.\n",page) ;
            //for last page, copy contents of Page into buffer
            if(page == (last_page-1) ) {
             //   printf("Page count for which sram is init from page is %d.\n",page) ;
                hal_set_s3fpga_val32_k(hp, S3FPGA_ADDR, (ISF_CMD_PAGE_TO_BUF_RD << 24) + (page << 15) + 4);
                hal_set_s3fpga_val32_k(hp, S3FPGA_INIT_SM, 0x2);
                //check status
                dec_count = isf_wait(hp,200,10); //max wait 2ms, each loop 10us
            }

            //for each page, prepare SRAM Buffer
            for (word_add =0; word_add  < ISF_PAGESIZE; word_add = word_add + 4  ) {
                //printf("word_add is %d\n ",word_add);
                lval =  (cfile[count] << 24) + (cfile[count+1] << 16)
                    + (cfile[count+2] <<8) + cfile[count+3] ;
                //set data
                hal_set_s3fpga_val32_k(hp, S3FPGA_DATA_OUT, lval);
                //printf("dataout %8lx.\n",hal_get_s3fpga_val32_k(hp,S3FPGA_DATA_OUT) );
                //set byte address
                hal_set_s3fpga_val32_k(hp, S3FPGA_ADDR, (ISF_CMD_BUF_WR << 24) + 
                        (word_add << 6) + byte_count);
                //printf("cmd reg %8lx.\n",hal_get_s3fpga_val32_k(hp,S3FPGA_ADDR) );
                hal_set_s3fpga_val32_k(hp, S3FPGA_INIT_SM, 0x2);
                //printf("init reg %8lx.\n",hal_get_s3fpga_val32_k(hp,S3FPGA_INIT_SM) );

                count = count + 4 ;
                if (filesize < count)
                    break ;
            }
            //buffer write
            //printf("After Loop, Page no. is %d, count is %d\n",page, count);
            hal_set_s3fpga_val32_k(hp, S3FPGA_ADDR, (ISF_CMD_BUF_TO_PAGE_NE << 24) + (page << 15) + byte_count);
            //printf("buff write to page cmd %8lx.\n",hal_get_s3fpga_val32_k(hp,S3FPGA_ADDR) );
            hal_set_s3fpga_val32_k(hp, S3FPGA_INIT_SM, 0x2);
            //printf("buf write to page init reg %8lx.\n",hal_get_s3fpga_val32_k(hp,S3FPGA_INIT_SM) );
            if(page == 1000) {
                //printf("buf to page write is 0x%x.\n",(ISF_CMD_BUF_TO_PAGE_NE << 24) + (page << 15) + byte_count);
            }
            //check status
            dec_count = isf_wait(hp,50000,1); //max wait 50ms, each loop 1us
            //printf("Page write to %d took %ld us.\n",page, 50000-dec_count);

        }
        printf("Page count is %d, Byte Count is %ld.\n",page,count);
        free(cfile);
        hal_release(hp);
        return 0;
    }
}
/*routine to dump ddr on XPi*/
static int dumpddr_main (int argc, char **argv) {
    FILE *ofile, *ifile ;
    int opt;
    int rc = 0,timeout;
    int partition = 0;
    char *ofilename = NULL, *ifilename = NULL ;
    unsigned long  lval = 0;
    unsigned long  word_count = 0;
    unsigned long  maxcount ;
    unsigned long vreg[4] ;
    char buf[100] ;
    char buf1[50] ;
    HAL *hp;
    int hardware_type;
    unsigned long count;


    while ((opt = getopt(argc, argv, "o:ph")) > 0) {
        switch (opt) {
            case 'o':
                ofilename = optarg;
                break;
            case 'p':
                partition = atoi(optarg);
                break;
            case '?':
            case 'h':
            default:
                printf("Usage: dumpddr -o output-file -p fb_num (fb_num = 0 for frame_buffer)\n");
                return -1;
        }
    }
    if (ofilename == NULL ) {
        printf("Usage: dumpddr -o output-file -p fb_num (fb_num = 0 for frame_buffer)\n");
        return -1;
    }
    if ((ofile = fopen(ofilename, "w")) < 0) {
        printf("dumpddr: can't open ouptut file %s. %m.\n", ofilename);
        return -1;
    }

    hp = hal_acquire(0);
    hardware_type = hal_get_hardware_type(hp);
    if (hp == NULL) {
        printf("Failed. Could not access hardware.\n");
        return -1;
    }
    if (hp && hardware_type != XPI_DEVICE) {
        print_dreg_maint_help();
        hal_release(hp);
        return 0;
    }

    //code to dump
    //init test
    hal_and_in_fpga_val_k(hp, 0x40, 0xfffffffd) ;//disable video enable
    hal_or_in_fpga_val_k(hp, 0x2c8, 0x2) ; //enable test bit to dump ddr
    hal_or_in_fpga_val_k(hp, 0x40, 0x2) ; //video enable
    printf("hres = %ld , vres = %ld.\n", hal_get_fpga_val32_k(hp,0x48) , hal_get_fpga_val32_k(hp,0x4c)) ;
    maxcount = (( hal_get_fpga_val32_k(hp,0x48) * hal_get_fpga_val32_k(hp,0x4c) * 24 ) /128) + 4 ;
    printf("%s, maxcount = %ld.\n",__func__,maxcount);

    sleep(1);
    timeout = 1000;
    while (word_count < maxcount ) {
        //dump
        if (hal_get_fpga_val32_k(hp, 0x8f0) > 0) {
            hal_and_in_fpga_val_k(hp, 0x2a4, 0xfffffffe) ;
            hal_or_in_fpga_val_k (hp, 0x2a4, 0x1) ;

            for(count=0; count <4; count ++)
                vreg[count] = hal_get_fpga_val32_k(hp, 0x284+count*4) ;
            //vreg[0] = 0x12345678;
            //vreg[1] = 0xdeadbeef;
            

            sprintf(buf,"%08lx%08lx%08lx%08lx\n",vreg[0], vreg[1], vreg[2], vreg[3]);
            (void) fwrite(buf, strlen(buf),  1, ofile);
            //(void) fwrite(vreg, 4 , 4, ofile);

            word_count = word_count + 1 ;
            timeout = 1000;
        }
        else {
            usleep(1);
            timeout --;
        }
        if(timeout == 0) {
            //printf("%s, timeout, fifo is empty, level of 0x8f0 is 0x%08lx.\n",__func__,hal_get_fpga_val32_k(hp, 0x8f0) );
            printf("%s, timeout, fifo is empty,\n",__func__);
            rc = -1;
            break;
        }
    } 
    //stop test
    printf("%s, word_count = %ld.\n",__func__,word_count);
    hal_and_in_fpga_val_k(hp, 0x2c8, 0xfffffffd) ;//disable test
    hal_and_in_fpga_val_k(hp, 0x40, 0xfffffffd) ;//disable video enable
    hal_or_in_fpga_val_k(hp, 0x40, 0x2) ; //video enable
    fclose(ofile);
    
#if 0
        if ((ifile = fopen(ofilename, "rb")) < 0 || ifile == NULL) {
            printf("Can't open input file %s. %m.\n", ofilename);
            return -1;
        }
        if ((ofile = fopen(ifilename, "w")) < 0 || ofile == NULL) {
            printf("Can't open input file %s. %m.\n", ifilename);
            return -1;
        }
    while (1) {
        if (fread(buf, sizeof(unsigned char), 3, ifile) == 3) 
        {
            sprintf(buf1,"%02x%02x%02x\n",buf[2],buf[1],buf[0]) ;
            (void) fwrite(buf1, strlen(buf1),  1, ofile);
            printf("%s\n",buf1);
        }
        else
            break;
    }
        
    fclose(ifile);
    fclose(ofile);

#endif
    hal_release(hp);
    return rc;
}
/**********************************
 * Routine to load FPGA bit files *
 **********************************/
static void print_loadfpga_help(void)
{
    printf("Usage: loadfpga [-x xfile] [-f ffile] [-e efile] [-v]\n");
}

static int loadfpga_main(int argc, char **argv) {
    char *xfile = NULL, *ffile = NULL, *efile = NULL;
    int opt, verbose = 0;
    char command[80];
    HAL *hp;

    while ((opt = getopt(argc, argv, "hx:f:e:v")) > 0) {
        switch (opt) {
        case 'x':
            xfile = optarg;
            break;
        case 'f':
            ffile = optarg;
            break;
        case 'e':
            efile = optarg;
            break;
        case 'v':
            verbose = 1;
            break;
        case '?':
        case 'h':
            print_loadfpga_help();
            return -1;
        }
    }

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (xfile) {
        printf("%s: not implemented yet\n", __func__);
    }
    if (ffile) {
        char *fext = strrchr(ffile, '.');
        if (!strncmp(fext, ".fcp", strlen(".fcp"))) {
            printf("Data files for fanout FPGA must have .fcp extension\n");
            hal_release(hp);
            return -1;
        }
        printf("Loading %s to Fanout FPGA...\n", ffile);
        xpi_set_gpio_pin_value(GPIO_S6_PROGN, 0);
        xpi_set_gpio_pin_value(GPIO_S6_FMUX_SEL, 1);
        sprintf(command, "/usr/sbin/flashcp %s /dev/mtd13", ffile);
        if (system_command(command) < 0) 
            printf("Failed to run command '%s'\n", command);
        xpi_set_gpio_pin_value(GPIO_S6_FMUX_SEL, 0);
        xpi_set_gpio_pin_value(GPIO_S6_PROGN, 0);
        printf("Finished loading %s to Fanout FPGA\n", ffile);
    }
    if (efile) {
        printf("%s: not implemented yet\n", __func__);
    }
    hal_release(hp);
    return 0;
}

/******************************
 * Routine to set driver mode *
 ******************************/
static void print_setdriver_help(void)
{
    printf("\nUsage: setdriver [-m 0 | 1]\n");
    printf("\nSets driver mode to 0 (idle) or 1 (normal)\n");
}

static int setdriver_main(int argc, char **argv) 
{
    int opt, fd, mode = -1;
    HAL *hp;

    while ((opt = getopt(argc, argv, "m:h")) > 0) {
        switch (opt) {
        case 'm':
            mode = atoi(optarg);
            break;
        case 'h':
            print_setdriver_help();
            exit(0);
        }
    }
    if (mode < 0) {
        print_setdriver_help();
        exit(0);
    }

    hp = hal_acquire(0);
   
    if (hp == NULL) {
        printf("Failed. Unable to access hardware\n");
        return -1;
    }

    if ((fd = hal_get_video_fd(hp)) < 0) {
        printf("Failed to open /dev/video0 as I2C device\n");
        hal_release(hp);
        return -1;
    }
    mode = (mode) ? 0 : 1;
    if (ioctl(fd, FPGA_IOC_SET_ADD_DRIVERIDLE, &mode) < 0) 
        printf("SET_ADD_DRIVERIDLE ioctl failed errno %d. %m\n", errno);
    else
        printf("Successfully set interface state to %s\n", 
                (mode) ? "normal" : "idle");
    hal_release(hp);

    return 0;
}

static void print_setlut_help(void)
{
    printf("Usage: setlut -a lutid [-h]\n");
    printf("Forces addside LUT to LUT ID 'lutid'\n");
    printf("Run \"maint setdriver -m 1 to restore to normal\"\n");
}

static int setlut_main(int argc, char **argv) {
    int opt, fd, lutid = 0, idle = 1;
    HAL *hp;
    struct LUT_struct lut;

    while ((opt = getopt(argc, argv, "a:h")) > 0) {
        switch (opt) {
        case 'a':
            lutid = getint(optarg);
            break;
        case 'h':
        default:
            print_setlut_help();
            break;
        }
    }

    if (lutid == 0) {
        print_setlut_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if ((fd = hal_get_video_fd(hp)) < 0) {
        printf("Failed to open /dev/video0 as I2C device\n");
        hal_release(hp);
        return -1;
    }

    printf("Forcing Addside LUT to ID %d...\n", lutid);
    if (ioctl(fd, FPGA_IOC_SET_ADD_DRIVERIDLE, &idle) < 0) 
        printf("SET_ADD_DRIVERIDLE ioctl failed errno %d. %m\n", errno);
    else if (ioctl(fd, FPGA_IOC_SET_ADDSIDELUT, &lutid) < 0) 
        printf("SET_ADDSIDELUT ioctl failed errno %d. %m\n", errno);
    else {
        printf("Successfully forced addside LUT to ID %d\n", lutid);
        printf("Run \"maint setdriver -m 1 to restore to normal\"\n");
    }

    hal_release(hp);

    return 0;
}

static void print_getinterface_help(void)
{
    printf("Usage: getinterface [-h]\n");
}

static int getinterface_main(int argc, char **argv) {
    int opt, intf = 0;
    char *input = NULL;
    HAL *hp;

    if (argc != 1) {
        print_getinterface_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    printf("Current interface flags = 0x%x.\n", hal_getActiveIntf(hp));

    hal_release(hp);

    return 0;
}

static void print_setinterface_help(void)
{
    printf("Usage: setinterface flags [-h]\n");
}

static int setinterface_main(int argc, char **argv) {
    int opt, intf = 0;
    char *input = NULL;
    HAL *hp;

    hp = hal_acquire(0);

    if (argc != 2) {
        print_setinterface_help();
        return 0;
    }

    intf = getint(argv[1]);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if (hal_setActiveIntf(hp, intf) < 0) 
        printf("Failed to set interface flags into driver\n");
    else
        printf("Successfully set interface flags to 0x%x\n", intf);

    hal_release(hp);

    return 0;
}

static void print_resetvideo_help(void)
{
    printf("Usage: resetvideo [-a|-d|o] [-h]\n");
    printf("Resets addside, dropside or output video\n");
}

static int resetvideo_main(int argc, char **argv) {
    int fd, opt, addside = 0, dropside = 0, output = 0;
    HAL *hp;

    while ((opt = getopt(argc, argv, "adoh")) > 0) {
        switch (opt) {
        case 'a':
            addside = 1;
            break;
        case 'd':
            dropside = 1;
            break;
        case 'o':
            output = 1;
            break;
        case 'h':
        default:
            print_resetvideo_help();
            return 0;
        }
    }
    if (addside == 0 && dropside == 0 && output == 0) {
        print_resetvideo_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (addside)
        hal_resetAddsideVideo(hp);
    else if (dropside)
        hal_resetDropsideVideo(hp);
    else 
        hal_resetOutputVideo(hp);

    hal_release(hp);

    return 0;
}

static void print_setovconfig_help(void)
{
    printf("Usage: setovconfig [-F flags] [-H hres] [-V vres] [-S hscale] [-s vscale] [-h]\n");
}

static int setovconfig_main(int argc, char **argv) {
    int fd, opt, flags = -1, hres = -1, vres = -1;
    int hscale = -1, vscale = -1;
    HAL *hp;
    struct ov_param_io ov, oldov;

    while ((opt = getopt(argc, argv, "F:H:V:S:s:h")) > 0) {
        switch (opt) {
        case 'F':
            flags = getint(optarg);
            break;
        case 'H':
            hres = getint(optarg);
            break;
        case 'V':
            vres = getint(optarg);
            break;
        case 'S':
            hscale = getint(optarg);
            break;
        case 's':
            vscale = getint(optarg);
            break;
        case 'h':
        default:
            print_setovconfig_help();
            return 0;
        }
    }

    if (flags < 0 && hres < 0 && vres < 0 && hscale < 0 && vscale < 0) {
        print_setovconfig_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (hal_get_overlay_params(hp, &oldov) < 0) {
        printf("Failed to get overlay params from driver\n");
        hal_release(hp);
        return -1;
    }
    if (flags != -1)
        ov.flags = flags;
    else
        ov.flags = oldov.flags;
    if (hres != -1)
        ov.hres = hres;
    else
        ov.hres = oldov.hres;
    if (vres != -1)
        ov.vres = vres;
    else
        ov.vres = oldov.vres;
    if (hscale != -1)
        ov.hscale = (hscale > RX_MAX_SCALE_VAL) ? RX_MAX_SCALE_VAL : hscale;
    else
        ov.hscale = oldov.hscale;
    if (vscale != -1)
        ov.vscale = (vscale > RX_MAX_SCALE_VAL) ? RX_MAX_SCALE_VAL : vscale;
    else
        ov.vscale = oldov.vscale;
    ov.setmask = OV_SET_PARAM_OVERLAY;

    if (hal_set_overlay_params(hp, &ov)) 
        printf("Failed to set overlay params into driver\n");
    else {
        printf("Overlay Configuration:\n");
        printf("Flags       = 0x%x\n", ov.flags);
        printf("Resolution  = %d\t%d\n", ov.hres, ov.vres);
        printf("Location    = %d\t%d\n", ov.hlocation, ov.vlocation);
        printf("Offset      = %d\t%d\n", ov.hoffset, ov.voffset);
        printf("Alpha       = %d\n", ov.oalpha);
        printf("Horiz Crop  = %d\t%d\n", ov.hcropleft, ov.hcropright);
        printf("Vert Crop   = %d\t%d\n", ov.vcroptop, ov.vcropbottom);
    }
    hal_release(hp);
    return 0;
}

static void print_setlicense_help(void)
{
    printf("Usage: setlicense [-h]\n");
}

static int setlicense_main(int argc, char **argv) {
    int fd, opt, license = NO_LICENSE, flags;
    HAL *hp;
    unsigned long ppscapacity = 0;

    while ((opt = getopt(argc, argv, "h")) > 0) {
        switch (opt) {
        case 'h':
        default:
            print_setlicense_help();
            return 0;
        }
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    flags = hal_get_fpga_flags(hp) & ~(FLAG_XPI_VIDPROC_BASIC | 
                                       FLAG_XPI_VIDPROC_DUALLINK | 
                                       FLAG_XPI_VIDPROC_STEREO |
                                       FLAG_XPI_VIDPROC_PPSCONTROL);
    if (have_stereo_license()) {
        license = PRO3D_LICENSE;
        flags |= FLAG_XPI_VIDPROC_BASIC | FLAG_XPI_VIDPROC_DUALLINK | 
            FLAG_XPI_VIDPROC_STEREO;
    }
    else if (have_duallink_license()) {
        license = PRO_LICENSE;
        flags |= FLAG_XPI_VIDPROC_BASIC | FLAG_XPI_VIDPROC_DUALLINK |
            FLAG_XPI_VIDPROC_PPSCONTROL; 
        ppscapacity = MAX_PPSCAPACITY_PRO;
    }
    else {
        license = LITE_LICENSE;
        flags |= FLAG_XPI_VIDPROC_BASIC | FLAG_XPI_VIDPROC_PPSCONTROL;
        ppscapacity = MAX_PPSCAPACITY_LITE;
    }

    fd = hal_get_video_fd(hp);
    printf("Setting license to %s\n", (license == LITE_LICENSE) ? "Lite" :
                ((license == PRO_LICENSE) ? "Pro" : "Pro3D"));
    if (hal_set_fpga_flags(hp, flags) < 0) {
        printf("Error setting video processing flag to 0x%x\n", flags);
    }
    else if (flags & FLAG_XPI_VIDPROC_PPSCONTROL) {
        if (hal_set_ppscapacity(hp, ppscapacity) < 0)
            printf("Error setting ppscapacity to %ld\n", ppscapacity);
    }
    hal_release(hp);

    return 0;
}

static void print_setdriveridle_help(void)
{
    printf("Usage: setautographics -[y|n] [-h]\n");
}

static int setdriveridle_main(int argc, char **argv) {
    int fd, opt, idle = -1;
    HAL *hp;

    while ((opt = getopt(argc, argv, "ynh")) > 0) {
        switch (opt) {
        case 'y':
            idle = 1;
            break;
        case 'n':
            idle = 0;
            break;
        case 'h':
        default:
            print_setdriveridle_help();
            return 0;
        }
    }

    if (idle == -1) {
        print_setdriveridle_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if ((fd = hal_get_video_fd(hp)) < 0) {
        printf("Failed to open /dev/video0 as I2C device\n");
        hal_release(hp);
        return -1;
    }
    if (ioctl(fd, FPGA_IOC_SET_ADD_DRIVERIDLE, &idle) < 0) 
        printf("FPGA_IOC_SET_ADD_DRIVERIDLE failed errno %d (%m).\n", errno);

    hal_release(hp);

    return 0;
}

static void print_setrouting_help(void)
{
    printf("Usage: setrouting [-s] -r [a|d|t] [-h]\n");
}

static int setrouting_main(int argc, char **argv) {
    int fd, opt, slave = 0;
    char mode = 'd';
    HAL *hp;
    struct routing_io rio;

    while ((opt = getopt(argc, argv, "sr:h")) > 0) {
        switch (opt) {
        case 's':
            slave = 1;
            break;
        case 'r':
            if (optarg[0] == 'a' || optarg[0] == 'A')
                mode = 'a';
            else if (optarg[0] == 'd' || optarg[0] == 'D')
                mode = 'd';
            else if (optarg[0] == 't' || optarg[0] == 'T')
                mode = 't';
            break;
        case 'h':
        default:
            print_setrouting_help();
            return 0;
        }
    }

    if (getuid() != 0) {
        printf("You must be root to execute this command\n");
        return 0;
    }

    if (mode != 'a' && mode != 'd' && mode != 't') {
        print_setrouting_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if ((fd = hal_get_video_fd(hp)) < 0) {
        printf("Failed to open /dev/video0 as I2C device\n");
        hal_release(hp);
        return -1;
    }
    memset(&rio, 0, sizeof(struct routing_io));
    if (slave) {
        rio.subdev = V4L2_SUBDEV_ADV7612;
        rio.input = (mode == 'a') ? NETVIZXPI_MODE_GR : 
            ((mode == 'd') ? NETVIZXPI_MODE_HDMI : NETVIZXPI_MODE_TEST);
    }
    else {
        rio.subdev = V4L2_SUBDEV_ADV7604;
        rio.input = NETVIZXPI_MODE_HDMI;
    }
    if (ioctl(fd, FPGA_IOC_SUBDEV_S_ROUTING, &rio) < 0) 
        printf("FPGA_IOC_SUBDEV_S_ROUTING failed errno %d (%m).\n", errno);
    else 
        printf("%s routing mode successfully set to %s\n", 
                (slave) ? "ADV7612" : "ADV7604", 
                (mode == 'a') ? "Analog" : ((mode == 'd') ? "Digital" : "Test"));

    hal_release(hp);

    return 0;
}

static void print_setfreerun_help(void)
{
    printf("Usage: setfreerun -l [0|173|174|175] [-h]\n");
}

static int setfreerun_main(int argc, char **argv) {
    int fd, opt, lutid = -1;
    HAL *hp;

    while ((opt = getopt(argc, argv, "l:h")) > 0) {
        switch (opt) {
        case 'l':
            lutid = getint(optarg);
            break;
        case 'h':
        default:
            print_setfreerun_help();
            return 0;
        }
    }

    if (getuid() != 0) {
        printf("You must be root to execute this command\n");
        return 0;
    }

    if (lutid != 0 && (lutid < 173 || lutid > 175)) {
        print_setfreerun_help();
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (hal_set_adc_free_run(hp, lutid) < 0) 
        printf("hal_set_adc_free_run returned errno %d (%m).\n", errno);
    else 
        printf("ADV7604 free run set to %d\n", lutid);

    hal_release(hp);

    return 0;
}

static void print_querycap_help(void)
{
    printf("Usage: querycap [-h]\n");
}

static int querycap_main(int argc, char **argv) {
    int fd, opt;
    HAL *hp;
    struct v4l2_capability cap;

    while ((opt = getopt(argc, argv, "h")) > 0) {
        switch (opt) {
        case 'h':
        default:
            print_querycap_help();
            return 0;
        }
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if (hal_v4l2_querycap(hp, &cap) < 0) 
        printf("Failed v4l2 querycap errno %d. %m\n", errno);
    else {
        printf("V4L2 capability:\n");
        printf("     driver: '%s'\n", cap.driver);
        printf("     card: '%s'\n", cap.card);
        printf("     bus_info: '%s'\n", cap.bus_info);
        printf("     version: %d\n", cap.version);
        printf("     capabilities: 0x%x\n", cap.capabilities);
    }
    hal_release(hp);

    return 0;
}

static void print_g_ctrl_help(void)
{
    printf("Usage: g_ctrl -i id [-h]\n");
}

static int g_ctrl_main(int argc, char **argv) {
    int fd, opt, id = -1, value = 0;
    HAL *hp;
    struct v4l2_control ctrl;

    while ((opt = getopt(argc, argv, "i:h")) > 0) {
        switch (opt) {
        case 'i':
            id = getint(optarg);
            break;
        case 'h':
        default:
            print_g_ctrl_help();
            return 0;
        }
    }

    if (id < 0) {
        print_g_ctrl_help();
        return -1;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    value = hal_v4l2_g_ctrl(hp, id);

    if (value < 0)
        printf("Failed v4l2 s_ctrl errno %d. %m\n", errno);

    printf("ADV7604 control %d has the value 0x%x\n", id, value);

    hal_release(hp);

    return 0;
}

static void print_s_ctrl_help(void)
{
    printf("Usage: s_ctrl -i id -v value [-h]\n");
}

static int s_ctrl_main(int argc, char **argv) {
    int fd, opt, id = -1, value = 0, got_value = 0;
    HAL *hp;
    struct v4l2_control ctrl;

    while ((opt = getopt(argc, argv, "i:v:h")) > 0) {
        switch (opt) {
        case 'i':
            id = getint(optarg);
            break;
        case 'v':
            got_value = 1;
            value = getint(optarg);
            break;
        case 'h':
        default:
            print_s_ctrl_help();
            return 0;
        }
    }

    if (id < 0 || !got_value) {
        print_s_ctrl_help();
        return 0;
    }
    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if (hal_v4l2_s_ctrl(hp, id, value) < 0) 
        printf("Failed v4l2 s_ctrl errno %d. %m\n", errno);
    else
        printf("ADV7604 control %d set to value 0x%x\n", id, ctrl.value);

    hal_release(hp);

    return 0;
}

static void print_s_input_help(void)
{
    printf("Usage: s_input [-a | -d] [-h]\n");
}

static int s_input_main(int argc, char **argv) {
    int fd, opt, input = NETVIZXPI_MODE_NONE;
    HAL *hp;

    while ((opt = getopt(argc, argv, "adh")) > 0) {
        switch (opt) {
        case 'a':
            input = NETVIZXPI_MODE_GR;
            break;
        case 'd':
            input = NETVIZXPI_MODE_HDMI;
            break;
        case 'h':
        default:
            print_s_input_help();
            return 0;
        }
    }

    if (input != NETVIZXPI_MODE_COMP && input != NETVIZXPI_MODE_GR &&
            input != NETVIZXPI_MODE_HDMI) {
        print_s_input_help();
        return 0;
    }
    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if (hal_v4l2_s_input(hp, input) < 0) 
        printf("Failed v4l2 s_ctrl errno %d. %m\n", errno);
    else
        printf("ADV7604 input set to value 0x%x\n", input);

    hal_release(hp);

    return 0;
}

static void print_timings(struct v4l2_dv_timings *timings)
{
    int htot, vtot, vfreq;

    htot = timings->bt.width + timings->bt.hfrontporch +
        timings->bt.hsync + timings->bt.hbackporch;
    vtot = timings->bt.height + timings->bt.vfrontporch +
        timings->bt.vsync + timings->bt.vbackporch;
    vfreq = (htot * vtot > 0) ? timings->bt.pixelclock / (htot * vtot) : 0;
    printf("LUT %dx%d%s%d (%dx%d)\n", 
            timings->bt.width, timings->bt.height, 
            timings->bt.interlaced ? "i" : "p", vfreq, htot, vtot);
    printf("    horizontal fp = %d, %ssync = %d, bp = %d\n",
            timings->bt.hfrontporch, 
            (timings->bt.polarities & V4L2_DV_HSYNC_POS_POL) ? "+" : "-", 
            timings->bt.hsync, timings->bt.hbackporch);
    printf("    vertical   fp = %d, %ssync = %d, bp = %d\n",
            timings->bt.vfrontporch, 
            (timings->bt.polarities & V4L2_DV_VSYNC_POS_POL) ? "+" : "-", 
            timings->bt.vsync, timings->bt.vbackporch);
    printf("    pixelclock %lld\n", timings->bt.pixelclock);
}

static void print_g_dv_timings_help(void)
{
    printf("Usage: g_dv_timings [-h]\n");
}

static int g_dv_timings_main(int argc, char **argv) {
    int fd, opt;
    struct v4l2_dv_timings timings;
    HAL *hp;
    int intf, vtotal, htotal;
    double pixclk, hfreq, vfreq;

    while ((opt = getopt(argc, argv, "h")) > 0) {
        switch (opt) {
        case 'h':
        default:
            print_g_dv_timings_help();
            return 0;
        }
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    if (hal_v4l2_g_dv_timings(hp, &timings) < 0) {
        printf("Failed v4l2 g_dv_timings. errno %d (%m)\n", errno);
    }
    else {
        printf("ADV7604 timings:\n");
        print_timings(&timings); 
    }

    intf = hal_getActiveIntf(hp);
    vtotal = timings.bt.height + timings.bt.vfrontporch + timings.bt.vsync + 
        timings.bt.vbackporch;
    htotal = timings.bt.width + timings.bt.hfrontporch + timings.bt.hsync + 
        timings.bt.hbackporch;
    pixclk = (double) timings.bt.pixelclock / (double) 1000000;
    vfreq = (double) timings.bt.pixelclock / (double) htotal / (double) vtotal;
    hfreq = (double) timings.bt.pixelclock / (double) htotal / (double) 1000;
    if (intf & INTERFACE_DUALLINK) {
        hfreq *= (double) 2;
        pixclk *= (double) 2;
    }
    printf("LUT: stereo = %s, duallink = %s\n", 
            (intf & INTERFACE_STEREO) ? "Yes" : "No", 
            (intf & INTERFACE_DUALLINK) ? "Yes" : "No");
    printf("hfreq = %5.2fKHz, vfreq = %5.2fHz, pixelclock = %7.2fMHz\n", 
            hfreq, vfreq, pixclk);

    hal_release(hp);

    return 0;
}

static int test_multimouse_main(int argc, char **argv)
{
    int x = 100, y = 100;
    int i;
    int sizex = 0, sizey = 0;
    int dir = 1;
    HAL *hp;
    struct LUT_struct lut;


    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", 0);
        return -1;
    }

    if (hal_get_hardware_type(hp) != XPI_DEVICE) {
        printf("Failed: Not an XPI.\n");
        hal_release(hp);
        return -1;
    }

  //  hal_set_icon_off(hp);
  //  hal_set_mouse_cursor_status(hp, 0);
	 
    if (hal_get_display_lut(hp, &lut) < 0) {
                    printf("Failed: Cannot get display LUT from driver\n");
                    hal_release(hp);
                    return -1;
    }
    
    hal_init_mouse_cursor(hp, x, y, lut.a_hres, lut.a_vres);
    hal_set_mouse_cursor_status(hp, 1);
    sizex = lut.a_hres;
    sizey = lut.a_vres;
	  
    while (1) {
        for (i = 0; i < 10; i++) {
            hal_set_mouse_cursor_pos(hp, x, y, sizex, sizey);
               //hal_init_mouse_cursor(hp, x, y, lut.a_hres, lut.a_vres);
            hal_set_mouse_cursor_status(hp, 1);
            //printf( " x = %d y = %d \n", x, y);
            sleep(3);
            x = (dir) ? (x + 5) : (x - 5);
            y = (dir) ? (y + 5) : (y - 5);
            if (x < 0) x = 0;
            else if (x > sizex) x = sizex;
            if (y < 0) y = 0;
            else if (y > sizey) y = sizey;
            //hal_set_fpga_val32_k(hp, FPGA_DOSDMMCTL_START + 4 * cursor_num, 
            //hal_set_s6fpga_val32_k(hp, 0xf8, ((unsigned long) y << 14) | (unsigned long) x);
        }
    }
  
    hal_release(hp);
    return 0;
}

/* dump ddr on xpi(frame buffer video data) */
static int test_duddr_main(int argc, char **argv)
{

    HAL *hp;
    FILE *fp;
    FILE *fp1;
    struct LUT_struct lut;
    int acreg;
    int width = 0;
    int height = 0;
    int iteration;
    unsigned long i = 0;
    int BytesRead[4] = {0,0,0,0};
   // char *bytewrite;
    char bytewrite[18];
    int n = 0;
    char buf[4];
    int cnt = 0;
    int k;
    unsigned long bytescnt = 1;
    int bytesremove = 0;

 #define BIT0   1

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed: Invalid channel number (%d).\n", 0);
        return -1;
    }

    if (hal_get_hardware_type(hp) != XPI_DEVICE) {
        printf("Failed: Not an XPI.\n");
        hal_release(hp);
        return -1;
    }

    fp =fopen("dumpddr.dat", "w");
    if (fp == NULL)
    {
        printf("cannot open file to write \n");
        return 0;
    }
#if DEBUG
    fp1 =fopen("dumpddr_1.dat", "w");
    if (fp1 == NULL)
    {
        printf("cannot open file to write \n");
        return 0;
    }
#endif
    acreg = hal_get_fpga_val32_k(hp, FPGA_ACREG);
   
    hal_and_in_fpga_val_k(hp, FPGA_ACREG, ~ACREG_VID_ENABLE);
    usleep(1000);
    hal_or_in_fpga_val_k(hp, 0x2c8, 2);
   // hal_or_in_fpga_val_k(hp, FPGA_ACREG, ACREG_VID_ENABLE);
    hal_or_in_fpga_val_k(hp, FPGA_ACREG, 0x22);

    sleep(10);

    width  =  hal_get_fpga_val32_k(hp, FPGA_AVHAREG);
    height = hal_get_fpga_val32_k(hp, FPGA_AVVAREG);

    iteration = ((width * height) *24)/128;
    printf("width = %d height = %d  iteration = %d\n", width, height, iteration);
     
    for (i = 0 ; i < (iteration + 3) ; i++)
    {
        #define BIT0   1
    
       hal_and_in_fpga_val_k(hp, 0x2a4, ~BIT0);
       hal_or_in_fpga_val_k(hp, 0x2a4, BIT0); 

       usleep(1);

       BytesRead[0] = hal_get_fpga_val32_k(hp, 0x284);
       BytesRead[1] =  hal_get_fpga_val32_k(hp, 0x288);
       BytesRead[2] =  hal_get_fpga_val32_k(hp, 0x28c);
       BytesRead[3] =  hal_get_fpga_val32_k(hp, 0x290);
       bytesremove += 1;
       cnt = 0;
      // bytewrite = (char *)BytesRead;
       for (k = 0 ; k<4; k++) {

           bytewrite[k*cnt+0] = (BytesRead[k] >> 24 ) & 0xFF;
           bytewrite[k*cnt+1] = (BytesRead[k] >> 16 ) & 0xFF;
           bytewrite[k*cnt+2] = (BytesRead[k] >> 8 ) & 0xFF;
           bytewrite[k*cnt+3] = (BytesRead[k] >> 0 ) & 0xFF;
           cnt = 4;
       }
        #if DEBUG
               fprintf(fp1, "%08x \n",(BytesRead[0]));
               fprintf(fp1, "%08x \n",(BytesRead[1]));
               fprintf(fp1, "%08x \n",(BytesRead[2]));
               fprintf(fp1, "%08x \n",(BytesRead[3]));
        #endif
        if ( bytesremove > 3) {

           for (n = 0; n<16; n++) {
               if ((bytescnt % 3))
                  fprintf(fp, "%02x ",bytewrite[n]);
               else
                   fprintf(fp, "%02x \n",bytewrite[n]);
               bytescnt += 1;
           }
       }
    }
    fclose(fp);
#if DEBUG    
    fclose(fp1);
#endif    
    printf(" done the iteration \n");
    hal_and_in_fpga_val_k(hp, FPGA_ACREG, ~ACREG_VID_ENABLE);
    hal_and_in_fpga_val_k(hp, 0x2c8, 0);
    hal_or_in_fpga_val_k(hp, FPGA_ACREG, acreg);
    hal_release(hp);
 
   return 1;

}

static struct option testled_options[] = {
    { "start", 1, 0, 's'},
    { "stop", 1, 0, 'S'},
    { "led", 1, 0, 'l'},
    { "print", 0, 0, 'p'},
    { "on", 0, 0, 'n'},
    { "off", 0, 0, 'f'},
    { "blink", 0, 0, 'b'},
    { 0, 0, 0, 0}
};

static void print_testled_help(void) 
{
    printf("Usage: testled [-s|-S] [-l num -[p|n|f|b]]\n");
    printf("Prints status of LED #led or sets it to on, off or blink\n");
    printf("Options: \n");
    printf("    -s     Starts the LED test\n");
    printf("    -S     Stops the LED test\n");
    printf("    -l num Sets LED number to 'num'\n");
    printf("    -p     Prints the LED status\n");
    printf("    -n     Turns the LED on\n");
    printf("    -f     Turns the LED off\n");
    printf("    -b     Sets the LED to blinking\n");
    printf("Note blinking for RJ45 LEDs are not supported\n");
    printf("Valid values for 'num' are:\n");
    printf("    0 - Encodeer LED\n");
    printf("    1 - Decodeer LED\n");
    printf("    2 - Video Out LED Green\n");
    printf("    3 - Video Out LED Red\n");
    printf("    4 - Video In LED Green\n");
    printf("    5 - Video In LED Red\n");
    printf("    6 - Expansion RJ45 Orange\n");
    printf("    7 - Expansion RJ45 Green\n");
    printf("    8 - Expansion RJ45 Yellow\n");
    printf("    9 - Main RJ45 Green\n");
    printf("   10 - Main RJ45 Yellow\n");
    printf("   11 - Main RJ45 Orange\n");
}

static int testled_main(int argc, char **argv) {
    int status, rc = 0, c, opt_index = 0, options = 0;
    int do_start = 0, do_stop = 0;
    int led = -1, do_print = 0, do_on = 0, do_off = 0, do_blink = 0;
    char *ledname[] = { "encoder green", "decoder green", "videoout green",
        "videoout red", "videoin green", "videoout red", 
        "expansion RJ45 orange", "expansion RJ45 green", 
        "expansion RJ45 yellow", "main RJ45 green", "main RJ45 yellow",
        "main RJ45 orange" };
    HAL *hp;

    while (1) {
        c = getopt_long(argc, argv, "sSl:pnfb", testled_options, &opt_index);
        if (c == -1) 
            break;
        switch (c) {
        case 's':
            do_start = 1;
            break;
        case 'S':
            do_stop = 1;
            break;
        case 'l':
            led = getint(optarg);
            options++;
            break;
        case 'p':
            do_print = 1;
            options++;
            break;
        case 'n':
            do_on = 1;
            options++;
            break;
        case 'f':
            do_off = 1;
            options++;
            break;
        case 'b':
            do_blink = 1;
            options++;
            break;
        default:
            print_testled_help();
            exit(-1);
        }
    }

    if (!do_start && !do_stop && (options != 2 ||  
                led >= (sizeof(ledname) / sizeof(char *)))) {
        print_testled_help();
        return 0;
    }

    if (do_start) {
        struct stat buf;
        if (stat("/tmp/.testLED", &buf) == 0) {
            printf("LED testing already started.\n");
            rc = -1; 
        }
        else {
            system_command("/bin/touch /tmp/.testLED");
        }
        return rc;
    }
    else {
        struct stat buf;
        if (stat("/tmp/.testLED", &buf) != 0) {
            printf("LED testing not started.\n");
            return -1; 
        }
    }

    if (do_stop) {
        system_command("/bin/rm /tmp/.testLED");
        return 0;
    }

    hp = hal_acquire(0);

    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    /* convert LED numbers for Main RJ45 to GPIO numbers */ 
    if (led == MAINRJ45_GREEN_BIT)
        led = MAINRJ45_GREEN;
    else if (led == MAINRJ45_YELLOW_BIT)
        led = MAINRJ45_YELLOW;
    else if (led == MAINRJ45_ORANGE_BIT)
        led = MAINRJ45_ORANGE;

    if (do_on) {
        hal_LED_on(hp, led);
    }
    else if (do_off) {
        hal_LED_off(hp, led);
    }
    else if (do_blink) {
        if (led >= ENCODER_LED_GREEN && led <= VIDEOIN_LED_RED)
            hal_LED_blink(hp, led, 0);
        else
            printf("NOT SUPPORTED\n");
    }
    else {
        rc = hal_LED_status(hp, led);
        printf("%s\n", (rc == LED_ON) ? "ON" : 
                ((rc == LED_OFF) ? "OFF" : "BLINK"));
    }

    hal_release(hp);

    return rc;
}

/********************************************
 * Routine that waits for user to enter ESC * 
 ********************************************/
static int getuserinput(void) {
    fd_set read_fdset;
    int nd, max_fd = -1, rc = 0;
    struct timeval timeout;
    unsigned char buf[1];

    system("/bin/stty -echo -cooked > /dev/null 2>&1");
    
    while (1) {
        FD_ZERO(&read_fdset);
        max_fd = -1;
        FD_SET(0, &read_fdset);
        max_fd = 0;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        nd = select(max_fd + 1, &read_fdset, NULL, NULL, &timeout);
        if (nd < 0) {
            if (errno != EINTR) {
                rc = -1; break;
            }
        }
        else if (nd == 0) {
            break;
        }
        else {
            fread(buf, 1, 1, stdin);
            fflush(stdin);
            if (buf[0] == 0x1b) {
                rc = 1; break;
            }
        }
    } 
    system("/bin/stty echo cooked > /dev/null 2>&1");
    fflush(stdout);
    return rc;
} 

/************************
 * Test utility for OSD * 
 ************************/
static void print_testosd_help(void) {
    printf("Usage: testosd [-s string | -g | -k ] [-c color] [-h]\n");
    printf("Displays OSD of the specified type and color (allowed values for\n");
    printf("color are: red, green, blue, white, black, gray, yellow, cyan, magenta\n");
};

static int supported_color(char *color)
{
    if (strcasecmp(color, "red") == 0 ||
        strcasecmp(color, "green") == 0 ||
        strcasecmp(color, "blue") == 0 ||
        strcasecmp(color, "black") == 0 ||
        strcasecmp(color, "white") == 0 ||
        strcasecmp(color, "yellow") == 0 ||
        strcasecmp(color, "magenta") == 0 ||
        strcasecmp(color, "cyan") == 0 ||
        strcasecmp(color, "grey") == 0)
        return 1;
    return 0;
}

static int testosd_main(int argc, char **argv) {
    int opt, hardware_type, x = -1, y = -1;
    int color = -1, do_str = 0, do_guide = 0, do_kbm = 1;
    struct LUT_struct lut;
    char *str = NULL;
    HAL *hp;

    while ((opt = getopt(argc, argv, "s:f:c:gkx:y:h")) > 0) {
        switch (opt) {
        case 's':
            do_guide = 0, do_str = 1, do_kbm = 0;
            str = optarg;
            break;
        case 'g':
            do_guide = 1, do_str = 0, do_kbm = 0;
            break;
        case 'k':
            do_guide = 0, do_str = 0, do_kbm = 1;
            break;
        case 'c':
            if (supported_color(optarg)) 
                color = color_code_from_name(optarg);
            else {
                print_testosd_help();
                return -1;
            }
            break;
        case 'x':
            x = getint(optarg);
            break;
        case 'y':
            y = getint(optarg);
            break;
        case '?': 
        case 'h': 
            print_testosd_help();
            return -1;
        }
    }   

    if (getuid() != 0) {
        printf("You must be root to execute this command\n");
        return 0;
    }

    if ((hp = hal_acquire(0)) == NULL) {
        printf("Failed to access hardware\n");
        return -1;
    }

    if ((hardware_type = hal_get_hardware_type(hp)) != XPI_DEVICE && 
            hardware_type != PLX_DEVICE) {
        printf("Failed: Not an XP or XPI\n");
        hal_release(hp);
        return -1;
    }

    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        if (hal_get_addside_lut(hp, &lut) == 0) {
            if (x < 0)  /* encoder osd is 6 pixels off horizontally */ 
                x = (lut.a_hres - DEFAULT_STRING_ICON_WIDTH) / 2 - 6;
            if (y < 0)  /* encoder osd is (vbp + vsync) pixels off vertically */
                y = (lut.a_vres - DEFAULT_STRING_ICON_HEIGHT) / 2 + 
                    lut.a_vbp + lut.a_vsync;
        }
    }
    else if (hal_get_display_lut(hp, &lut) == 0) {
        if (x < 0) 
            x = (lut.a_hres - DEFAULT_STRING_ICON_WIDTH) / 2;
        if (y < 0)  
            y = (lut.a_vres - DEFAULT_STRING_ICON_HEIGHT) / 2;
    }

    printf("Displaying %s icon. Press <ESC> to exit.\n", 
            (do_str) ? "string" : ((do_kbm) ? "KBM" : "guideruler"));

    if (do_str) {
        if (str == NULL) {
            print_testosd_help();
            return -1;
        }
        if (color < 0)
            color = color_code_from_name(DEFAULT_STRING_ICON_COLOR);
        hal_init_string_icon(hp, 1, color, str);
    }
    else if (do_kbm) {
        if (color < 0)
            color = color_code_from_name(DEFAULT_KEYBOARD_ICON_COLOR);
        hal_init_keyboard_icon(hp, 1, color);
    }
    else if (do_guide) {
        if (color < 0)
            color = color_code_from_name(DEFAULT_GUIDERULER_ICON_COLOR);
        hal_init_guideruler_icon(hp, 1, color);
    }
    hal_move_icon_xpos(hp, x);
    hal_move_icon_ypos(hp, y);

    while (getuserinput() != 1) 
        sleep(1);

    hal_set_icon_off(hp);

    hal_release(hp);

    return 0;
}

/*****************************************************************************
 * Routine to set FPGA network header and filter for multicast sync channel  *
 *                                                                           *
 * "mcasthdr -m mcastgroup [-s srcport] [ -d dstport] [-c context] [-t ttl]" *
 * sets the nethdr field in the FPGA to mcastgroup with source port srcport  *
 * destination port dstport context-id context and time-to-live ttl.         *
 *                                                                           *
 * "mcastfilter -m mcastgroup [-d dstport] [-c context]" sets the netfilter  *
 * field in the FPGA to mcastgroup with destination port dstport context-id  *
 * context.                                                                  *
 *                                                                           *
 * "setmcast -m mcastgroup [-p port] [-t ttl]" creates a multicast socket    *
 * and joins the multicast group mcastgroup. The program will prompt you to  * 
 * enter <esc> to leave the group and exit the program.                      *
 *                                                                           *
 * To use these commands for testing,                                        * 
 * 1. run mcasthdr on the transmitter to set up the nethdr on the FPGA,      *
 * 2. run mcastfilter on the receiver to set up the filter on the FPGA,      *
 * 3. run setmcast on the receiver to join the multicast group,              *
 * 4. enter <esc> when done to leave the multicast group and exit 'setmcast'.*
 *****************************************************************************/
#define DEFAULT_MCAST_PORT           6060
#define DEFAULT_MCAST_CONTEXT        2
#define DEFAULT_MCAST_FILTERINDEX    2
#define DEFAULT_MCAST_TTL            1

static int convert_mac_addr(char *mac_str, unsigned short mac_addr[])
{
    unsigned int mac[6];

    mac_addr[0] = mac_addr[1] = mac_addr[2] = 0;

    if (sscanf(mac_str, "%x:%x:%x:%x:%x:%x", &mac[5], &mac[4], &mac[3],
        	&mac[2], &mac[1], &mac[0]) != 6) {
        return -1; 
    }
    mac_addr[2] = mac[5] << 8 | mac[4];  
    mac_addr[1] = mac[3] << 8 | mac[2];  
    mac_addr[0] = mac[1] << 8 | mac[0];  

    return 0;
}

static int get_mac_info(char *ifname, unsigned short mac[])
{
    char filename[64];
    char str[128];
    int fd, rc, i;
    struct stat astat;

    mac[0] = mac[1] = mac[2] = 0;
    sprintf(filename, "/sys/class/net/%s/address", ifname);
    if (lstat(filename, &astat) != 0) {
        return -1;
    }
    if (S_ISREG(astat.st_mode) != 1) 
        return -1;

    if ((fd = open(filename, O_RDONLY)) < 0) 
        return -1;

    rc = read(fd, str, 128);
    for (i = 0; i < 128; i++) {
        if (str[i] != ':' && !isxdigit(str[i])) {
            str[i] = 0;
            break;
        }
    }
    close(fd);

    if (rc < 0) 
        return -1;

    return convert_mac_addr(str, mac);
}

static unsigned int get_ip_info(char *ifname)
{
    int rc;
    struct in_addr ip_addr;
    struct in_addr netmask_addr;

    if ((rc = get_if_info(ifname, &ip_addr, &netmask_addr)) < 0)
        return 0;

    return (unsigned int) ip_addr.s_addr;
}

static int get_multicast_mac(unsigned int mcastaddr, unsigned short mac[])
{
    /* 
     * Multicast MAC is of the form 01:00:5e:7X:XX:XX with the lowest
     * 23 bits mapped with the lowest 23 bits of the multicast address 
     */ 
    mac[0] = ((0xff & (mcastaddr >> 16)) << 8) | (0xff & (mcastaddr >> 24));
    mac[1] = (0x5e << 8) | (0x7f & (mcastaddr >> 8));
    mac[2] = (0x01 << 8) | 00;
    return 0;
}

static void print_mcasthdr_help(void)
{
    printf("Usage: mcasthdr -m mcast-group [-s srcport] [-d dstport] [-c context] [-t ttl]\n");
    printf("Set network header to the parameters on the command line. The header\n");
    printf("will be cleared if mcast-group is\"0.0.0.0\"\n");
}

static int mcasthdr_main(int argc, char **argv) {
    char *mcastgroup = NULL;
    struct in_addr mcastinaddr;
    int opt, verbose = 0;
    int ttl = DEFAULT_MCAST_TTL, context = DEFAULT_MCAST_CONTEXT;
    int srcport = DEFAULT_MCAST_PORT, dstport = DEFAULT_MCAST_PORT;
    int do_clear = 0;
    unsigned int clientipaddr;
    unsigned short clientmac[3];
    char ifname[32];
    unsigned int serveripaddr;
    HAL *hp;

    while ((opt = getopt(argc, argv, "m:c:s:d:t:vh")) > 0) {
        switch (opt) {
        case 'm':
            mcastgroup = optarg;
            break;
        case 'c':
            context = getint(optarg);
            break;
        case 's':
            srcport = getint(optarg);
            break;
        case 'd':
            dstport = getint(optarg);
            break;
        case 't':
            ttl = getint(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        default:
            print_mcasthdr_help();
            return 0;
        }
    }
    if (mcastgroup == NULL || context < 0 || context > 15) {
        print_mcasthdr_help();
        return 0;
    }
    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if (!strcasecmp(mcastgroup, "0.0.0.0"))
        do_clear = 1;

    strcpy(ifname, DEFAULT_IF_NAME);
    if ((serveripaddr = get_ip_info(ifname)) == 0) { 
        printf("Failed to get host IP for %s\n", ifname);
        return -1;
    }
    else {
        serveripaddr = htonl(serveripaddr);
    }
  
    if (inet_aton(mcastgroup, &mcastinaddr) == 0) {
        printf("Failed to determine multicast network address\n");
        return -1;
    }
    clientipaddr = mcastinaddr.s_addr;
    if (get_multicast_mac(clientipaddr, clientmac) < 0) {
        printf("Failed to get client MAC address\n");
        return -1;
    }

    if (!do_clear) {
        if (verbose) {
            printf("%s: setting nethdr. mcast group %s, context %d, ttl %d, "
                    "srcport %d, dstport %d\n", __func__, mcastgroup,
                    context, ttl, srcport, dstport);
            printf("               "
                    "ipaddr %08x, mac %02x:%02x:%02x:%02x:%02x:%02x\n",
                    clientipaddr, (unsigned char) (clientmac[2] >> 8),
                    (unsigned char) (clientmac[2] & 0xff),
                    (unsigned char) (clientmac[1] >> 8),
                    (unsigned char) (clientmac[1] & 0xff),
                    (unsigned char) (clientmac[0] >> 8),
                    (unsigned char) (clientmac[0] & 0xff));
        }
        if (hal_set_nethdr(hp, context, srcport, ttl, clientipaddr, clientmac, 
                    dstport) < 0) {
            printf("%s: failed to set nethdr\n", __func__);
            return 0;
        }
        if (verbose)
            printf("%s: setting source IP address to %08x\n", __func__,
                    serveripaddr);
        hal_set_fpga_val_k(hp, FPGA_XPI_SRC_IPADDR, serveripaddr);
    }
    else {
        if (hal_clear_nethdr(hp, context) < 0) {
            printf("%s: failed to clear nethdr\n", __func__);
            return 0;
        }
    }
    hal_release(hp);

    return 0;
}

static void print_mcastfilter_help(void)
{
    printf("Usage: mcastfilter -m mcast-group [-p port] [-c context]\n");
    printf("Set network header to the parameters on the command line. The filter\n");
    printf("will be cleared if mcast-group is\"0.0.0.0\"\n");
}

static int mcastfilter_main(int argc, char **argv) {
    char *mcastgroup = NULL;
    struct in_addr mcastinaddr;
    int opt, verbose = 0;
    int context = DEFAULT_MCAST_CONTEXT, port = DEFAULT_MCAST_PORT;
    int do_clear = 0;
    struct ip_mreq mreq;
    unsigned int serveripaddr;
    HAL *hp;

    while ((opt = getopt(argc, argv, "m:c:p:vh")) > 0) {
        switch (opt) {
        case 'm':
            mcastgroup = optarg;
            break;
        case 'c':
            context = getint(optarg);
            break;
        case 'p':
            port = getint(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        default:
            print_mcastfilter_help();
            return 0;
        }
    }
    if (mcastgroup == NULL || context < 0 || context > 15) {
        print_mcastfilter_help();
        return 0;
    }
    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }

    if (!strcasecmp(mcastgroup, "0.0.0.0"))
        do_clear = 1;

    if (inet_aton(mcastgroup, &mreq.imr_multiaddr) == 0) { 
        printf("%s is not a valid IP address\n", mcastgroup);
        return -1;
    }
    else {
        serveripaddr = htonl(mreq.imr_multiaddr.s_addr);
    }
  
    if (!do_clear) {
        if (verbose) {
            printf("%s: setting filter. index %d, ipaddr %08x, port %d\n",
                __func__, context, serveripaddr, port);
        }
        if (hal_set_rtpfilter(hp, context, serveripaddr, port, 0) < 0) {
            printf("%s: failed to set filter\n", __func__);
            return 0;
        }
    }
    else {
        if (hal_clear_rtpfilter(hp, context) < 0) {
            printf("%s: failed to clear filter\n", __func__);
            return 0;
        }
    }
    hal_release(hp);

    return 0;
}

#define DEFAULT_MCAST_BUFLEN      1024
#define DEFAULT_MCAST_DELAY       2

static void print_setmcast_help(void) {
    printf("Usage: setmcast -m mcastgroup [-p port]\n");
    printf("Creates receive multicast sockets and joins the multicast group\n");
    printf("'mcastgroup' port 'port'\n");
}

static int setmcast_main(int argc, char **argv)
{
    unsigned char no = 0;
    unsigned int yes = 1;
    struct sockaddr_in mcast_recv;
    char *mcastgroup = NULL;
    int opt, verbose = 0, send = 1;
    unsigned short port = DEFAULT_MCAST_PORT;
    int recv_s = 0;
    struct ip_mreq mreq;

    while ((opt = getopt(argc, argv, "m:p:vh")) > 0) {
        switch (opt) {
        case 'm':
            mcastgroup = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        default:
            printf("Usage: %s -m mcastgroup [-p port]\n", argv[0]);
            exit(1);
        }
    }

    if (mcastgroup == NULL) {
        printf("Usage: %s -m mcastgroup [-p port]\n", argv[0]);
        exit(1);
    }

    printf("Creating receive multicast sockets for group %s port %d\n", 
            mcastgroup, port);

    if ((recv_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Error opening receive socket. %m (%d).\n", errno);
        exit(1);
    }

    memset(&mcast_recv, 0, sizeof(mcast_recv));
    mcast_recv.sin_family = AF_INET;
    mcast_recv.sin_port = port;
    mcast_recv.sin_addr.s_addr = inet_addr(mcastgroup);

    if (verbose) {
        struct sockaddr_in rin;
        socklen_t rlen;
        unsigned short rport;
        printf("Created receive socket %d\n", recv_s);
        getsockname(recv_s, (struct sockaddr *) &rin, &rlen);
        rport = htons(rin.sin_port);
        printf("Created receive socket %d addr %s port %d\n", recv_s, 
            inet_ntoa(rin.sin_addr), rport);
    }

    if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        printf("Error setsockopt REUSEADDR. %m (%d).\n", errno);
        close(recv_s);
        exit(1);
    }

    if (bind(recv_s, (struct sockaddr *) &mcast_recv, sizeof(mcast_recv)) < 0) {
        printf("Error bind. %m (%d).\n", errno);
        close(recv_s);
        exit(1);
    }

    // tell kernel we want to join the multicast group
    mreq.imr_multiaddr = mcast_recv.sin_addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); 
    if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, 
                sizeof(mreq)) < 0) {
       printf("Error setsockopt IP_ADD_MEMBERSHIP. %m (%d).\n", errno);
       close(recv_s);
       exit(1);
    }

    printf("Press <ESC> to close multicast sockets and drop membership.\n");
    while (getuserinput() != 1) {
        sleep(1);
    }

    if (setsockopt(recv_s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, 
                sizeof(mreq)) < 0) {
        printf("Error setsockopt IP_DROP_MEMBERSHIP. %m (%d).\n", errno);
    }

    close(recv_s);

    return 0;
}

static int kernelpanic_main(int argc, char **argv)
{
    HAL *hp = hal_acquire(0);
    int dummy = 0;
    if (hp == NULL) {
        printf("Failed to access hardware. errno %d. %m\n", errno);
        return -1;
    }
    printf("This command will cause kernel panic. Press <ESC> to continue.\n");
    while (getuserinput() != 1) {
        sleep(1);
    }
    if (ioctl(hp->kernel_fd, FPGA_IOC_RESET, &dummy) < 0)
        printf("Failed to force kernel panic\n");
    return 0;
}

/************************************
 * Maint command                    *
 ************************************/
static void run_util(const char *util_name, int argc, char **argv);
static int maint_main(int argc, char **argv);

const struct v_util maint_utils[] = {
    {"maint", maint_main},
    {"dreg", dreg_maint_main},
    {"i2creg", i2creg_main},
    {"setup_audio", setup_audio_main},
    {"network_init", network_init_main},
    {"mdioreg", reg_main},
    {"macreg", reg_main},
    {"setpll", setpll_main},
    {"getpll", getpll_main},
    {"setlut", setlut_main},
    {"setedid", setedid_main},
    {"printedid", printedid_main},
    {"seteeprom", seteeprom_main},
    {"printeeprom", printeeprom_main},
    {"setmonedid", setmonedid_main},
    {"temperature", temperature_main},
    {"bit2fcp", bit2fcp_main},
    {"s3_upgrade", s3_upgrade_main},
    {"dumpddr", dumpddr_main},
    {"loadfpga", loadfpga_main},
    {"setdriver", setdriver_main},
    {"getinterface", getinterface_main},
    {"setinterface", setinterface_main},
    {"resetvideo", resetvideo_main},
    {"setovconfig", setovconfig_main},
    {"setlicense", setlicense_main},
    {"setdriveridle", setdriveridle_main},
    {"setrouting", setrouting_main},
    {"setfreerun", setfreerun_main},
    {"querycap", querycap_main},
    {"s_ctrl", s_ctrl_main},
    {"g_ctrl", g_ctrl_main},
    {"s_input", s_input_main},
    {"g_dv_timings", g_dv_timings_main},
    {"testmouse", test_multimouse_main},
    {"testled", testled_main},
    {"testosd", testosd_main},
    {"mcasthdr", mcasthdr_main},
    {"mcastfilter", mcastfilter_main},
    {"setmcast", setmcast_main},
    {"kernelpanic", kernelpanic_main},

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
