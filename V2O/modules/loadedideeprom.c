#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <sys/types.h>
#include    <sys/ioctl.h>
#include    <sys/stat.h>
                                                                                
#include    "hal.h"
#include    "hal_hwinfo.h"
#include    "commonutils.h"
#include    "lut_utils.h"
#include    "edid_list_menu.h"
#include    "edid_utils.h"
#include    "hal_edid.h"
#include    "system_utils.h"
#include    "vlog.h"
#include    "vutils.h"

/************************************************************
 * Routine to load the MicroChip 24LC21 EEPROM with EDID on * 
 * Main Board. Default data is as follows:                  *
 *                                                          *
 * Product Information                                      *
 *       Manufacturer: IPV                                  *
 *       Product ID: IPV0102                                *
 *       Serial Number: 00000001                            *
 *       Week of Manufacture: 16                            *
 *       Year of Manufacture: 2008                          *
 * EDID Version 1  Revison 3                                *
 * Basic Display Parameters                                 *
 *       Video Input Definition                             *
 *               Digital  DFP 1.x compatible                *
 *       Maximum Horiz Size 520 cm                          *
 *       Maximum Vert Size 320 cm                           *
 *       Display Gamma 2.20                                 *
 *       Power Management Features                          *
 *               Active Off/Low Power                       *
 *       Supported Features                                 *
 *               RGB Color                                  *
 *               Default GTF Supported                      *
 *               Preferred Timing Mode                      *
 * Chroma Information                                       *
 *       Red X: 0.639    Red Y: 0.339                       *
 *       Green X: 0.289  Green Y: 0.609                     *
 *       Blue X: 0.139   Blue Y: 0.068                      *
 *       White X: 0.310  White Y: 0.326                     *
 * Established Timing I                                     *
 *       800x600@60Hz                                       *
 *       800x600@56Hz                                       *
 *       720x400@70Hz                                       *
 * Established Timing II                                    *
 *       1280x1024@75Hz                                     *
 *       1024x768@75Hz                                      *
 *       1024x768@70Hz                                      *
 *       1024x768@60Hz                                      *
 *       832x624@75Hz                                       *
 *       800x600@75Hz                                       *
 *       800x600@72Hz                                       *
 * Manufacturer's Reserved Timing: 0x80                     *
 * Standard Timing Identification:                          *
 *       1280x720@73Hz                                      *
 *       1280x960@60Hz                                      *
 *       1440x900@60Hz                                      *
 *       1680x1050@60Hz                                     *
 *       1280x1024@60Hz                                     *
 *       1600x1200@60Hz                                     *
 *       1152x864@60Hz                                      *
 * Descriptor Block #1:                                     *
 *       Horiz Active 1920       Horiz Blanking 280         *
 *       Vert Active 1080        Vert Blanking 44           *
 *       Horiz Sync Offset 88    Hsync Pulse Width 44       *
 *       Vert Sync Offset 4      Vsync Pulse Width 10       *
 *       Horiz Image Size 708    Vert Image Size 398        *
 *       Horiz Border 0          Vert Border 0              *
 *       Hsync Polarity Pos      Vsync Polarity Pos         *
 *       Separate Sync                                      *
 *                                                          *
 *       Pixel Clock 148.500 MHz, Refresh Rate 60.053 Hz    *
 *       Horiz Resolution 1920, FP 148, Sync 44, BP 88      *
 *       Vert Resolution 1080, FP 30, Sync 10, BP 4         *
 *       Hsync Polarity +, Vsync Polarity +                 *
 *                                                          *
 * Descriptor Block #2: Monitor Range Limits                *
 *       Horiz Freq 31 - 80 KHz                             *
 *       Vert Freq 56 - 65                                  *
 *       Pixel Clock 170 MHz                                *
 * Descriptor Block #3: Monitor Name                        *
 *       IPV164.1                                           *
 * Descriptor Block #4: Monitor Name                        *
 *       V2D XP200                                          *
 ************************************************************/ 
static void print_load_edid_eeprom_help(void) {
    printf("\nUsage: loadedideeprom [-f file] [-l] [-m] [-D 0|1] [-A 0-7f] [-d] [-h] -c N\n");
    printf("Load the EDID EEPROM for XP200 channel N from the file 'file'.\n");
    printf("   if the -f option is present otherwise load default content.\n");
    printf("The -D 0|1 option sets video input definition to digital not DFP\n");
    printf("   compatible or DFP compatible.\n");
    printf("The -A X option sets video input definition to analog with setting\n");
    printf("   X where X is a Hex value in the range 0 to 7f with bits 6-5 =\n");
    printf("   video level, bit 4 = blank-to-black setup, bit 3 = separate syncs,\n");
    printf("   bit 2 = composite sync, bit 1 = SOG, bit 0 = serration vsync.\n");
    printf("The -l option says data in 'filename' is longhex\n");
    printf("The -h option prints this message\n");
}

int load_edid_eeprom_main(int argc, char **argv) {
    int opt, cno = -1, data_size, longformat = 0;
    char *name = NULL;
    FILE *file;
    unsigned char eeprom_data[EDID_DATA_LEN];
    unsigned char videoinput = 0x80;
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
    while ((opt = getopt(argc, argv, "hdlf:A:D:c:")) > 0) {
        switch (opt) { 
        case 'c':
            cno = atoi(optarg);
            break;
        case 'f':
            name = optarg;
            break;
        case 'D':
            videoinput = atoi(optarg) ? 0x81 : 0x80;
            break;
        case 'A':
            videoinput = (unsigned char) strtoul(optarg, NULL, 16) & 0x7f;
            break;
        case 'l':
            longformat = 1;
            break;
        case '?':
        case 'h':
             print_load_edid_eeprom_help();
             return -1;
        }
    }
    hp = hal_acquire(0);
    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        cno = 1;
    }
    else {
        if (cno < 0 || cno > MAX_FPGA_BOARDS) {
            printf("Failed: invalid channel number %d\n", cno);
            return -1;
        }
        hal_release(hp);
        hp = hal_acquire(cno - 1);
    }
    if (hp == NULL) {
        printf("Failed: can't access hardware or hardware is not V2D\n");
        return -1;
    }

    if (name != NULL) {            /* read EEPROM data from file */
        if ((file = fopen(name, "r")) == NULL) {
            printf("Can't open input file %s. %m.\n", name);
            hal_release(hp);
            return -1;
        }
        data_size = read_hex_from_file(file, eeprom_data, 
                EDID_DATA_LEN, longformat);
        fclose(file);

        if (data_size != EDID_DATA_LEN) {
            printf("Failed to read data from input file %s.\n", name);
            goto out;
        } 
        eeprom_data[127] += eeprom_data[20] - videoinput;/* fix up checksum */
        eeprom_data[20] = videoinput;
        if (hal_load_edid_eeprom(hp, eeprom_data, EDID_DATA_LEN) < 0) {
            printf("Cannot write to EDID EEPROM\n");
            goto out;
        }
    } 
    else {                  /* load default EDID EEPROM data modified */
                            /* with descriptor blocks in the EDID list */
        if (load_edid_eeprom_with_descriptor(hp, videoinput) < 0) {
            printf("Cannot write to EDID EEPROM\n");
            goto out;
        }
    }
out:
    hal_release(hp);

    return 0;
}

/********************************
 * Utility to read EDID EEPROM  * 
 ********************************/
static void print_read_edid_eeprom_help(void) {
    printf("\nUsage: readedideeprom [-h] [-d] -c cno\n");
    printf("Displays the contents of the EDID EEPROM for Channel 'cno'\n");
    printf("The -h option prints this message.\n");
}
                                                                                
int read_edid_eeprom_main(int argc, char **argv) {
    int opt, cno = -1, rc;
    unsigned char buf[EDID_DATA_LEN];
    char edidtxt[EDID_TEXT_SIZE];
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
    while ((opt = getopt(argc, argv, "hc:d")) > 0) {
        switch (opt) { 
        case 'c':
            cno = atoi(optarg);
            break;
        case '?':
        case 'h':
             print_read_edid_eeprom_help();
             return -1;
        }
    }
    hp = hal_acquire(0);
    if (hp && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        cno = 1;
    }
    else {
        if (cno < 0 || cno > MAX_FPGA_BOARDS) {
            printf("Failed: invalid channel number %d\n", cno);
            return -1;
        }
        hal_release(hp);
        hp = hal_acquire(cno - 1);
    }
    if (hp == NULL) {
        printf("Failed: can't access hardware or hardware is not V2D\n");
        return -1;
    }

    rc = hal_read_edid_eeprom(hp, buf, EDID_DATA_LEN);

    if (rc < 0) {
        printf("Failed to read from Main Board 24LC21 EEPROM\n");
        return 0;
    }

    (void) print_edid_to_buffer(buf, edidtxt, EDID_PAGER, 1);

    printf("%s\n", edidtxt);
    
    hal_release(hp);

    return 0;
}
