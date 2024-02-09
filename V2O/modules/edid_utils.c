#include    <stdio.h> 
#include    <string.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <sys/types.h>
#include    <sys/ioctl.h>
#include    <sys/stat.h>
#include    <syslog.h>

#include    "hal.h"
#include    "hal_hwinfo.h"
#include    "device_detection.h"
#include    "commonutils.h"
#include    "hal_vpd.h"
#include    "lut_utils.h"
#include    "edid_list_menu.h"
#include    "edid_utils.h"
#include    "vlib.h"
#include    "hal_edid.h"
#include    "text_utils.h"
#include    "system_utils.h"
#include    "vlog.h"

#define EDID_DESCR_SIZE     18  /* EDID timing descriptor size */
#define EDID_DESCR_START    54  /* Timing descriptor #1 start pos. */

/*********************************
 * Default IP Video Systems EDID *
 *********************************/
static unsigned long default_xp200_eeprom_data[] = {
    0xffffff00, 0x00ffffff, 0x01011626, 0x00000001,
    0x03011110, 0x78203480, 0xa3a6b02b, 0x239c4a56,
    0x03534f11, 0xcd8180cf, 0x00954081, 0x010100b3,
    0x40a98081, 0x00004071, 0x3100fe00, 0x30303030,
    0x20203030, 0x20202020, 0xfd000000, 0x1f413800,
    0x0a001150, 0x20202020, 0x00002020, 0x3000fe00,
    0x30303030, 0x00303030, 0x2020200a, 0xfc000000,
    0x32505800, 0x442d3030, 0x200a442c, 0xa2002020
};

static unsigned long default_i50_eeprom_data[] = {
    0xffffff00, 0x00ffffff, 0x01011626, 0x00000001,
    0x03011110, 0x78203480, 0xa3a6b02b, 0x239c4a56,
    0xbf534f11, 0xcd8180cf, 0x00954081, 0x010100b3,
    0x40a98081, 0x00004071, 0x3100fe00, 0x30303030,
    0x20203030, 0x20202020, 0xfd000000, 0x1f413800,
    0x0a001150, 0x20202020, 0x00002020, 0x3000fe00,
    0x30303030, 0x00303030, 0x2020200a, 0xfc000000,
    0x30354900, 0x2041442d, 0x0a202020, 0x41002020
};

static unsigned long default_xpi_eeprom_data[] = {
    0xffffff00, 0x00ffffff, 0x00019308, 0x00030001,
    0x03011732, 0x78090c80, 0x98ac1e0a, 0x28855a59,
    0x03575229, 0xcd8180cf, 0x00954081, 0x010100b3,
    0x40a98081, 0x00004071, 0x2020fe00, 0x20202020,
    0x20202020, 0x20202020, 0xfe000000, 0x20202020,
    0x20202020, 0x20202020, 0x00002020, 0x2020fe00,
    0x20202020, 0x20202020, 0x20202020, 0xfc000000,
    0x69505800, 0x69676944, 0x006c6174, 0x26000000
};

static unsigned char edid_header[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

static char *analog_sync_str[] = {
    "Serration Vsync",
    "Sync on Green", 
    "Composite Sync", 
    "Separate Syncs", 
    "Blank-to-Black Setup", 
};

static char *std_timing_str[] = {
    "800x600@60Hz", "800x600@56Hz", "640x480@75Hz", "640x480@72Hz", 
    "640x480@67Hz", "640x480@60Hz", "720x400@88Hz", "720x400@70Hz" 
};

static char *std_timing_2_str[] = {
    "1280x1024@75Hz", "1024x768@75Hz", "1024x768@70Hz", "1024x768@60Hz",
    "1024x768@87Hz Interlaced", "832x624@75Hz", "800x600@75Hz", "800x600@72Hz" 
};

static char *power_mngt_str[] = {
    "Active Off/Low Power", "Suspend", "Standby" 
};

static char *display_type_str[] = {
    "Monochrome", "RGB Color", "Non-RGB Color", "Undefined"
};

static char *supported_feature_str[] = {
    "Default GTF Supported", 
    "Preferred Timing Mode",
    "Standard Color Space"
};

static unsigned char blank_text[] = {
    0x00, 0x00, 0x00, 0xfe, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20
};

/*****************************************************************
 * Replace Descriptor Block 'descriptor' in 'edidbuf' with 'buf' *
 *****************************************************************/
static void replace_descr_block(unsigned char *edidbuf, 
                                unsigned char *buf,
                                int descriptor) {
    int i, start;
    unsigned char csum;

    /* EDID bytes 54 thru 71 is descriptor block #1 etc.*/
    start = EDID_DESCR_START;
    if (descriptor > 1) start += EDID_DESCR_SIZE;
    if (descriptor > 2) start += EDID_DESCR_SIZE;
    if (descriptor > 3) start += EDID_DESCR_SIZE;

    /* EDID byte 127 is checksum */
    csum = edidbuf[127];

    /* Replace descriptor block in EDID data */
    for (i = 0; i < EDID_DESCR_SIZE; i++) {
        csum += edidbuf[start + i];
        edidbuf[start + i] = buf[i];
        csum -= buf[i];
    }
    edidbuf[127] = csum;
}

/*********************************************************************
 * Replace Manufacture Year and Week in 'edidbuf' with what's in vpd *
 *********************************************************************/
static void replace_manufacture_date(HAL *hp, unsigned char *edidbuf) {
    unsigned char csum, yy, mm, dd;
    int i, dayofyear = 0, weekofyear;
    unsigned long mdate;
    static int days_in_mon[] = {31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31};

    if ((mdate = hal_get_board_vpd_date(hp)) == 0)
        return;

    yy = (unsigned char) VPD_YEAR(mdate) + 10;
    mm = (unsigned char) VPD_MONTH(mdate);
    dd = (unsigned char) VPD_DAY(mdate);
    if (mm > 11) mm = 11;
    if ((yy % 4) == 0 && (yy % 100) != 0)
        days_in_mon[1] = 29;
    else
        days_in_mon[1] = 28;

    if (mm > 0) {
        for (i = 0; i < mm; i++)
            dayofyear += days_in_mon[i];
    }
    dayofyear += dd;
    weekofyear = dayofyear / 7 + 1;

    /* Replace bytes 16 and 17 */
    csum = edidbuf[127] + edidbuf[16] + edidbuf[17] - yy - weekofyear;
    edidbuf[16] = weekofyear;
    edidbuf[17] = yy;
    edidbuf[127] = csum;
}

/**************************************
 * Replace Serial Number in 'edidbuf' *
 **************************************/
static void replace_serial_number(HAL *hp, unsigned char *edidbuf) {
    char str[16];
    int i, serial = 0;
    unsigned char ser[4], csum; 

    if (hal_get_hardware_type(hp) == PLX_DEVICE) {
        /* get serial number from VPD */
        if (hal_get_board_serial_number(hp, str, sizeof str) < 0)
            return; 
        sscanf(str, "IPV%d", &serial);
    }
    else {
        /* get serial number from serial number file */
        get_device_serial_number(str, sizeof str);
        sscanf(str, "%d", &serial);
    }

    ser[0] = ((unsigned long) serial & 0x000000ff);
    ser[1] = ((unsigned long) serial & 0x0000ff00) >> 8;
    ser[2] = ((unsigned long) serial & 0x00ff0000) >> 16;
    ser[3] = ((unsigned long) serial & 0xff000000) >> 24;

    /* EDID byte 127 is checksum */
    csum = edidbuf[127];

    /* Replace bytes 12 to 14 */
    for (i = 0; i < 4; i++) {
        csum += edidbuf[12 + i];
        edidbuf[12 + i] = ser[i];
        csum -= ser[i];
    }
    edidbuf[127] = csum;
}

/***********************************
 * Replace product id in 'edidbuf' *
 ***********************************/
static void replace_product_id(unsigned char *edidbuf, 
                               unsigned short id) {
    unsigned char csum; 

    /* Replace byte 10 and 11 */
    csum = edidbuf[127] + edidbuf[11] + edidbuf[10];
    edidbuf[11] = (unsigned char) ((id & 0xff00) >> 8);
    edidbuf[10] = (unsigned char) (id & 0x00ff);
    edidbuf[127] = csum - edidbuf[11] - edidbuf[10];
}

/***********************************
 * Replace videoinput in 'edidbuf' *
 ***********************************/
static void replace_video_input(unsigned char *edidbuf, 
                                unsigned char videoinput) {
    /* Replace byte 20 */
    edidbuf[127] += edidbuf[20] - videoinput; 
    edidbuf[20] = videoinput;        
}

/***************************************************************
 * Replace monitor name descriptor in 'edidbuf' with 'monname' *
 * appended with '-D' if digital or '-A' otherwise.            *
 ***************************************************************/
#define MON_NAME_OFFSET         5
#define MON_NAME_DESCRIPTOR     4

static void replace_monitor_name(unsigned char *edidbuf, 
                                 char *monname,
                                 int isdigital)
{
    unsigned char buf[EDID_DESCR_SIZE];
    int i, len;
    char *s, *analog = "-A", *digital = "-D";

    /* create descr block for monitor name */
    memset(buf, 0, sizeof(buf));
    buf[3] = 0xfc;

    /* append '-A' or '-D' to monitor name */
    if (strlen(monname) > EDID_DESCR_SIZE - 2 - MON_NAME_OFFSET)
        memcpy(&buf[MON_NAME_OFFSET], monname, 
                EDID_DESCR_SIZE - 2 - MON_NAME_OFFSET);
    else
        memcpy(&buf[MON_NAME_OFFSET], monname, strlen(monname));
    strcat((char *) &buf[MON_NAME_OFFSET], (isdigital) ? digital : analog);

    /* replace descr block MON_NAME_DESCRIPTOR in edid */
    replace_descr_block(edidbuf, buf, MON_NAME_DESCRIPTOR);
}

static int other_mon_descr(unsigned char descr[]) {
    if (descr[0] == 0x00 && descr[1] == 0x00 && descr[2] == 0x00 &&
            descr[3] >= 0xfa)
        return 1;
    return 0;
}

/*****************************************
 * Populates 'edidbuf' with default data *
 *****************************************/
#define PRODID_I50              50
#define PRODID_XP200            200
#define PRODID_XP220            220
#define PRODID_XP100            100
#define PRODID_XPI              0xD200

static void get_default_edid(HAL *hp, 
                             unsigned char *edidbuf, 
                             unsigned char videoinput) 
{
    char hwdesc[32], monname[12];
    unsigned int prodid = PRODID_XP200;
    int hardware_type = hal_get_hardware_type(hp);

    if (hardware_type == V2DI50_DEVICE) {
        memcpy(edidbuf, (char *) default_i50_eeprom_data, EDID_DATA_LEN);
        return;
    }
    else if (hardware_type == XPI_DEVICE) {
        memcpy(edidbuf, (char *) default_xpi_eeprom_data, EDID_DATA_LEN);
        strcpy(monname, "NGS-D200");
        prodid = PRODID_XPI;
    }
    else {
        memcpy(edidbuf, (char *) default_xp200_eeprom_data, EDID_DATA_LEN);
        if (hal_get_hardware_description(hp, hwdesc, sizeof(hwdesc)) == 0) {
            if (strcasecmp(hwdesc, "V2D-XP200-S") == 0) {
                strcpy(monname, "XP200S");
                prodid = PRODID_XP200;
            }
            else if (strcasecmp(hwdesc, "V2D-XP220-D") == 0) {
                strcpy(monname, "XP220D");
                prodid = PRODID_XP220;
            }
            else if (strcasecmp(hwdesc, "V2D-XP220-S") == 0) { 
                strcpy(monname, "XP220S");
                prodid = PRODID_XP220;
            }
            else if (strcasecmp(hwdesc, "V2D-XP100") == 0) {
                strcpy(monname, "XP100");
                prodid = PRODID_XP100;
            }
        }
        else {
            strcpy(monname, "XP200D");
            prodid = PRODID_XP200;
        }
    }

    replace_monitor_name(edidbuf, monname, (videoinput & 0x80) ? 1 : 0);
    replace_serial_number(hp, edidbuf);
    replace_manufacture_date(hp, edidbuf);
    replace_product_id(edidbuf, prodid);
    replace_video_input(edidbuf, videoinput);
}

/********************************************************************
 * Returns EDID list params for entry with LUT id 'lutid'. Must     *
 * initialize EDID list params table before calling this routine    *
 ********************************************************************/
int get_edid_list_params_by_lutid(struct edid_list_params *params, 
                                  int lutid) {
    struct edid_list_params *paramsp;
    int rc = 0;

    foreach_edid_list_entry(paramsp) {
        if (paramsp->edid_alutreg_value == lutid) {
            memcpy(params, paramsp, sizeof(struct edid_list_params));
            rc = 1;
            break;
        }
    }
    return rc;
}

/****************************
 * Routines to decode EDID  *
 ****************************/
static char *get_vendor_sign(unsigned char byte0, unsigned char byte1) {
   static char sign[4];
   unsigned short h = ((unsigned short) byte0 << 8) | (unsigned short) byte1;

   sign[0] = ((h >> 10) & 0x1f) + 'A' - 1; 
   sign[1] = ((h >> 5) & 0x1f) + 'A' - 1; 
   sign[2] = (h & 0x1f) + 'A' - 1; 
   sign[3] = 0;

   return sign;
}

static int display_standard_timing(unsigned char c1, 
                                   unsigned char c2,
                   char *txt) {
    unsigned short horiz, vert;
    double asprat;
    int len = 0;

    if (c1 == 0x01 && c2 == 0x01) return len;

    horiz = c1 * 8 + 248;
    asprat = ((c2 & 0xc0) == 0x00) ? ((double) 10 / (double) 16) : 
             (((c2 & 0xc0) == 0x40) ? ((double) 3 / (double) 4) :
             (((c2 & 0xc0) == 0x80) ? ((double) 4 / (double) 5) :
             (double) 9 / (double) 16));
    vert =  asprat * (double) horiz;
    len = sprintf(txt, "  %dx%d@%dHz\n", horiz, vert, (c2 & 0x3f) + 60);
    return len;
}

/* 'txt' must not be NULL if verbose != 0 */
static int decode_descr_block(unsigned char *buf, 
                              struct LUT_struct *lutp,
                              int verbose,
                              char *txt) {
                                  
    int len = 0;
    unsigned short x, y, hblank = 0, vblank = 0, pix;
    float pixclk;

    x = (((unsigned short) buf[4] & 0xf0) << 4) | (unsigned short) buf[2];
    y = (((unsigned short) buf[4] & 0x0f) << 8) | (unsigned short) buf[3];
    
    if (verbose && txt != NULL) {
        len += sprintf(txt + len, 
           "  Horiz Active %-4d       Horiz Blanking %-4d\n", x, y);
    }
    if (lutp) {
        lutp->a_orig_hres = x; hblank = y;
    }

    x = (((unsigned short) buf[7] & 0xf0) << 4) | (unsigned short) buf[5];
    y = (((unsigned short) buf[7] & 0x0f) << 8) | (unsigned short) buf[6];
    if (verbose && txt != NULL) {
        len += sprintf(txt + len, 
        "  Vert Active  %-4d       Vert Blanking  %-4d\n", x, y);
    }
    if (lutp) {
        lutp->a_orig_vres = x; vblank = y;
    }
    
    x = (((unsigned short) buf[11] & 0xc0) << 2) | (unsigned short) buf[8]; 
    y = (((unsigned short) buf[11] & 0x30) << 4) | (unsigned short) buf[9]; 
    if (verbose && txt != NULL) {
        len += sprintf(txt + len, 
        "  Horiz Sync Offset %-4d  Hsync Pulse Width %-4d\n", x, y);
    }
    if (lutp) {
        lutp->a_hfp = x; lutp->a_hsync = y; lutp->a_hbp = hblank - x - y;
    }

    x = (((unsigned short) buf[11] & 0x0c) << 2) | 
                (((unsigned short) buf[10] & 0xf0) >> 4); 
    y = (((unsigned short) buf[11] & 0x03) << 4) | 
                ((unsigned short) buf[10] & 0x0f); 
    if (verbose && txt != NULL) {
        len += sprintf(txt + len, 
        "  Vert Sync Offset %-4d   Vsync Pulse Width %-4d\n", x, y);
    }
    if (lutp) {
        lutp->a_vfp = x; lutp->a_vsync = y; lutp->a_vbp = vblank - x - y;
    }

    x = (((unsigned short) buf[14] & 0xf0) << 4) |
                ((unsigned short) buf[12]);
    y = (((unsigned short) buf[14] & 0x0f) << 8) | 
                ((unsigned short) buf[13]);
    if (verbose && txt != NULL) {
        len += sprintf(txt + len,
        "  Horiz Image Size %-4d   Vert Image Size %-4d\n", x, y);
        len += sprintf(txt + len, 
        "  Horiz Border %-4d       Vert Border %-4d\n", 
        buf[15], buf[16]);
        len += sprintf(txt + len,
        "  Hsync Polarity %s      Vsync Polarity %s\n", 
            (buf[17] & 0x02) ? "Pos" : "Neg",  
        (buf[17] & 0x04) ? "Pos" : "Neg");
    }
    if (lutp) {
        if (buf[17] & 0x02)
            lutp->d_dcreg &= ~DCREG_HSYNC_POL;
        else 
            lutp->d_dcreg |= DCREG_HSYNC_POL;
        if (buf[17] & 0x04)
            lutp->d_dcreg &= ~DCREG_VSYNC_POL;
        else
            lutp->d_dcreg |= DCREG_VSYNC_POL;
    }
    if (verbose && txt != NULL) {
        pix = ((unsigned short) buf[1] << 8) + (unsigned short) buf[0];
        pixclk = (float) pix / (float) 100;
        if (buf[17] & 0x80) 
            len += sprintf(txt + len, 
                "  Interlaced              Pixel Clock %4.1f", pixclk);
        else if (buf[17] & 0x20) 
            len += sprintf(txt + len, 
                "  Stereo                  Pixel Clock %4.1f", pixclk);
        else if (buf[17] & 0x18) 
            len += sprintf(txt + len, 
                "  Separate Sync           Pixel Clock %4.1f", pixclk);
        else if (buf[17] & 0xd8) 
            len += sprintf(txt + len,
                "                          Pixel Clock %4.1f", pixclk);
    }

    return len;
}

static int display_mon_range(unsigned char *buf, char *txt) {
    int len = 0;
 
    if (buf[5] == 0x00 && buf[6] == 0x0a) {
        len += sprintf(txt, "  Horiz Freq %d - %d KHz\n", buf[2], buf[3]);
        len += sprintf(txt + len, "  Vert Freq %d - %d\n", buf[0], buf[1]);
        len += sprintf(txt + len, 
        "  Pixel Clock %d MHz\n", (unsigned short) buf[4] * 10);
    } 
    else if (buf[5] == 0x02 && buf[6] == 0x00) {
        len += sprintf(txt + len, "  Start Horiz Freq %d\n", buf[7] / 2);
        len += sprintf(txt + len, "  %02x %02x %02x %02x %02x\n", 
        buf[8], buf[9], buf[10], buf[11], buf[12]); 
    }
    else {
        int i;
        len += sprintf(txt + len, "Invalid monitor range:\n");
        for (i = 0; i < 8; i++)
            len += sprintf(txt + len, "%02x ", buf[i]);
        len += sprintf(txt + len, "\n");
    }
    return len;
}

static int display_color_data(unsigned char *buf, char *txt) {
    int len = 0;
    unsigned short wx = 0, wy = 0;
    double gamma;
    
    if (buf[0] == 0x01) {
        wx = ((unsigned short) buf[2] << 2) | ((buf[1] & 0x0c) >> 2); 
        wy = ((unsigned short) buf[3] << 2) | (buf[1] & 0x03); 
        gamma = (double) buf[4] / 100.0 + 1.0;   
        len += sprintf(txt + len, 
            "White Point Index #1: X %4.3f\tY %4.3f\tGamma %4.2f\n",
            (double) wx / 1024.0, (double) wy / 1024.0, gamma);
    }
    if (buf[5] == 0x02) {
        wx = ((unsigned short) buf[7] << 2) | ((buf[6] & 0x0c) >> 2); 
        wy = ((unsigned short) buf[8] << 2) | (buf[6] & 0x03); 
        gamma = (double) buf[9] / 100.0 + 1.0;   
        len += sprintf(txt + len, 
            "White Point Index #2: X %4.3f\tY %4.3f\tGamma %4.2f\n",
            (double) wx / 1024.0, (double) wy / 1024.0, gamma);
    }

    return len;
}

void get_edid_product_id(unsigned char *edidbuf, char *prodid) {
    unsigned short id;
    id = ((unsigned short) edidbuf[11] << 8) | (unsigned short) edidbuf[10];
    sprintf(prodid, "%s%d\n", get_vendor_sign(edidbuf[8], edidbuf[9]), id);
    trim_string(prodid);
}

/**************************************************************
 * Interprets raw EDID data in 'buf' writes interpreted data  *
 * (in readable text) into buffer 'edidtxt'. The buffer is    *
 * broken into blocks of lines with the string 'pager' so we  *
 * can display it using 'displayEepromEdid'. The 'print_all'  * 
 * flag should be turned off for output pages <= 12 lines.    *
 **************************************************************/
int print_edid_to_buffer(unsigned char *buf, 
                         char *edidtxt,
                         char *pager,
                         int print_all) {
    int i, j, len, pages = 1;
    unsigned char checksum = 0;
    unsigned short x, y, pix, id;
    char *sign;
    unsigned long serial;


    len = sprintf(edidtxt, "\nEDID Checksum\n");

    for (i = 0; i < sizeof(edid_header); i++) {
        if (buf[i] != edid_header[i]) {
            sprintf(edidtxt + len, 
                "\nWrong EDID header (byte %d should 0x%02x)\n",
                i, edid_header[i]);
            return 0;
        }
    }

    for (i = 0; i < EDID_DATA_LEN; i++) 
        checksum += buf[i];

    if (checksum != 0) 
        len += sprintf(edidtxt + len, 
        "  EDID checksum failed (= 0x%02x)\n", checksum);
    else
        len += sprintf(edidtxt + len, "  EDID checksum passed.\n");

    len += sprintf(edidtxt + len, "\nProduct Information\n");

    sign = get_vendor_sign(buf[8], buf[9]);
    len += sprintf(edidtxt + len, "  Manufacturer: %s\n", sign);
    id = ((unsigned short) buf[11] << 8) | (unsigned short) buf[10];
    len += sprintf(edidtxt + len, "  Product ID: %s%d\n", sign, id);
    serial = *((unsigned long *) &buf[12]);
    len += sprintf(edidtxt + len, "  Serial Number: %08lu\n", serial);
    len += sprintf(edidtxt + len, "  Manufacture Date: %d Week %d\n", 
        buf[17] + 1990, buf[16]);
    len += sprintf(edidtxt + len, "\nEDID Version %d  Revison %d\n", 
        buf[18], buf[19]);

    ++pages;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, "\nBasic Display Parameters\n");
    len += sprintf(edidtxt +len, "  Video Input Definition\n");

    if (buf[20] & 0x80) {
        len += sprintf(edidtxt + len, "    Digital %s DFP 1.x compatible\n", 
                    (buf[20] & 0x01) ? "" : "Not");
    }
    else {
        len += sprintf(edidtxt + len, "    Analog Video Level %s\n", 
        ((buf[20] & 0x60) == 0x00) ? "0.7, 0.3" :
        (((buf[20] & 0x60) == 0x20) ? "0.714, 0.286" :
        (((buf[20] & 0x60) == 0x40) ? "1, 0.4" : "0.7, 0")));

        if (print_all) {
            for (i = 0; i < 5; i++)
                if (buf[20] & (1 << i)) 
                    len += sprintf(edidtxt + len, "    %s\n", 
                            analog_sync_str[i]);
        }
    }

    len += sprintf(edidtxt + len, "  Max Horiz Size %d mm", buf[21] * 10);
    len += sprintf(edidtxt + len, "  Vert Size %d mm\n", buf[22] * 10);
    len += sprintf(edidtxt + len, 
        "  Display Gamma %4.2f\n", (double) buf[23] / (double) 100.0 + 1.0);
    len += sprintf(edidtxt + len, "  Power Management Features\n");

    len += sprintf(edidtxt + len, "    ");
    for (i = 0; i < 3; i++) {
        if (buf[24] & (1 << (5 + i))) {
            len += sprintf(edidtxt + len, "%s ", power_mngt_str[i]);
        }
    }
    len += sprintf(edidtxt + len, "\n");

    len += sprintf(edidtxt + len, "  Supported Features\n");
    len += sprintf(edidtxt + len, 
        "    %s\n", display_type_str[(buf[24] & 0x18) >> 3]);

    for (i = 0; i < 3; i++) {
        if (buf[24] & (1 << i)) {
            len += sprintf(edidtxt + len, "    %s\n", supported_feature_str[i]);
        }
    }

    ++pages;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, "\nChroma Information\n");
    x = (unsigned short) buf[27] << 2 | ((buf[25] & 0xc0) >> 6);
    y = (unsigned short) buf[28] << 2 | ((buf[25] & 0x30) >> 4);
    len += sprintf(edidtxt + len, "  Red X:   %4.3f  Red Y:   %4.3f\n", 
            (double) x / 1024.0, (double) y / 1024.0);
    x = (unsigned short) buf[29] << 2 | ((buf[25] & 0x0c) >> 2);
    y = (unsigned short) buf[30] << 2 | ((buf[25] & 0x03));
    len += sprintf(edidtxt + len, "  Green X: %4.3f  Green Y: %4.3f\n", 
            (double) x / 1024.0, (double) y / 1024.0);
    x = (unsigned short) buf[31] << 2 | ((buf[26] & 0xc0) >> 6);
    y = (unsigned short) buf[32] << 2 | ((buf[26] & 0x30) >> 4);
    len += sprintf(edidtxt + len, "  Blue X:  %4.3f  Blue Y:  %4.3f\n", 
            (double) x / 1024.0, (double) y / 1024.0);
    x = (unsigned short) buf[33] << 2 | ((buf[26] & 0x0c) >> 2);
    y = (unsigned short) buf[34] << 2 | ((buf[26] & 0x03));
    len += sprintf(edidtxt + len, "  White X: %4.3f  White Y: %4.3f\n", 
            (double) x / 1024.0, (double) y / 1024.0);

    ++pages;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, "\nEstablished Timing I\n");
    for (i = 0; i < 8; i++) {
        if (buf[35] & (1 << i)) {
            len += sprintf(edidtxt + len, "  %s\n", std_timing_str[i]);
        }
    }

    ++pages;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, "\nEstablished Timing II\n");
    for (i = 0; i < 8; i++) {
        if (buf[36] & (1 << i)) {
            len += sprintf(edidtxt + len, "  %s\n", std_timing_2_str[i]);
        }
    }

    ++pages;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, 
        "\nManufacturer's Reserved Timing: 0x%02x\n", buf[37]);
    len += sprintf(edidtxt + len, "Standard Timing Identification:\n");

    for (i = 38; i < 54; i += 2) {
        len += display_standard_timing(buf[i], buf[i + 1], edidtxt + len);
    }

    for (i = 54; i < 126; i += 18) {
        ++pages;
        if (pager != NULL) {
            len += sprintf(edidtxt + len, "\n%s\n", pager);
        }
        len += sprintf(edidtxt + len, 
            "\nDescriptor Block #%d: ", (i - 54) / 18 + 1);
        pix = ((unsigned short) buf[i + 1] << 8) + (unsigned short) buf[i];
        if (pix) {
            len += sprintf(edidtxt + len, "\n");
            len += decode_descr_block(&buf[i], NULL, 1, edidtxt + len);
            len += sprintf(edidtxt + len, "\n");
        }
        else {
            unsigned char tmp[14];

            switch (buf[i + 3]) {
            case 0xff:
                memcpy(tmp, &buf[i + 5], 13);
                tmp[13] = '\0';
                len += sprintf(edidtxt + len, "Monitor Serial Number\n");
                len += sprintf(edidtxt + len, "  %s\n", tmp);
                break;
            case 0xfe:
                {
                    memcpy(tmp, &buf[i + 5], 13);
                    tmp[13] = '\0';
                    len += sprintf(edidtxt + len, "Ascii String\n");
                    len += sprintf(edidtxt + len, "  %s\n", tmp);
                }
                break;
            case 0xfd: 
                len += sprintf(edidtxt + len, "Monitor Range Limits\n");
                len += display_mon_range(&buf[i + 5], edidtxt + len);
                break;
            case 0xfc: 
                memcpy(tmp, &buf[i + 5], 13);
                tmp[13] = '\0';
                len += sprintf(edidtxt + len, "Monitor Name\n");
                len += sprintf(edidtxt + len, "  %s\n", tmp);
                break;
            case 0xfb: 
                len += sprintf(edidtxt + len, "Color Point Data\n");
                len += display_color_data(&buf[i + 5], edidtxt + len);
                break;
            case 0xfa: 
                len += sprintf(edidtxt + len, "Standard Timing Data\n");
                for (j = i + 5; j < i + 16; j += 2) {
                    len += display_standard_timing(buf[j], buf[j + 1], 
                                edidtxt + len); 
                }
                break;
            case 0xf9: 
                len += sprintf(edidtxt + len, "Defined Block Type\n");
                for (j = 0; j < 18; j++) {
                    if (j % 8 == 0) len += sprintf(edidtxt + len, "\t");
                    len += sprintf(edidtxt + len, "%02x ", buf[i + j]);
                    if (j % 8 == 7) {
                        len += sprintf(edidtxt + len, "\n");
                    }
                }
                len += sprintf(edidtxt + len, "\n");
                break;
            case 0xf8: 
                len += sprintf(edidtxt + len, "Vendor Defined\n");
                for (j = 0; j < 18; j++) {
                    if (j % 8 == 0) len += sprintf(edidtxt + len, "\t");
                    len += sprintf(edidtxt + len, "%02x ", buf[i + j]);
                    if (j % 8 == 7) {
                        len += sprintf(edidtxt + len, "\n");
                    }
                }
                len += sprintf(edidtxt + len, "\n");
                break;
            default: 
                len += sprintf(edidtxt + len, 
                    "Invalid Block Type (0x%02x)\n", buf[i + 3]);
                for (j = 0; j < 18; j++) {
                    if (j % 8 == 0) len += sprintf(edidtxt + len, "\t");
                    len += sprintf(edidtxt + len, "%02x ", buf[i + j]);
                    if (j % 8 == 7) {
                        len += sprintf(edidtxt + len, "\n");
                    }
                }
                len += sprintf(edidtxt + len, "\n");
                break;
            }
        }
    }

    pages++;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, "\nRaw EDID Data\n\n");
    for (i = 0; i < EDID_DATA_LEN; i++) {
        len += sprintf(edidtxt + len, "%02x ", buf[i]);
        if (i % 16 == 15) len += sprintf(edidtxt + len, "\n");
    }
    edidtxt[len - 1] = '\0';
    return pages;
}

/******************************************
 * Prints EDID extension blocks to buffer *
 ******************************************/
#define CEA_EDID_TAG                        0x02
#define AUDIO_DATA_BLOCK_TAG                0x01
#define VIDEO_DATA_BLOCK_TAG                0x02
#define VENDOR_SPECIFIC_DATA_BLOCK_TAG      0x03
#define SPEAKER_ALLOCATION_DATA_BLOCK_TAG   0x04

#define CEA_AUDIO_CODE(x)                   (((x) & 0x78) >> 3)
#define CEA_AUDIO_CHANNELS(x)               (((x) & 0x07) + 1)

static char *short_video_descriptor[] = {
    "DMT0659", "480p", "480pH", "720p", 
    "1080i", "480i", "240p", "240pH", 
    "480i4x", "480i4xH", "240p4x", "240p4xH", 
    "480p2x", "480p2xH", "480p2xH", "1080p",
    "576p", "576pH", "720p50", "1080i25", 
    "576i", "576iH", "288p", "288pH",
    "576i4x", "576i4xH", "288p4x", "288p4xH",
    "576p2x", "576p2xH", "1080p50", "1080p24",
    "1080p25", "1080p30", "480p4x", "480p4xH",
    "576p4x", "576p4xH", "1080i25", "1080i50",
    "720p100", "576p100", "576p100H", "576i50",
    "576i50H", "1080i60", "720p120", "480p119",
    "480p119H", "480i59", "480i59H", "576p200", 
    "576p200H", "576i100", "576i100H", "480p239",
    "480p239H", "480i119", "480i119H", "720p24",
    "720p25", "720p30", "100p120", "1080p120"
};

static char *audio_format[] = {
    "", "LPCM", "AC-3", "MPEG1", "MP3", "MPEG2", "AAC", "DTS", 
    "ATRAC", "SACD", "DD+", "DTS-HD", "MLP", "DSTAudio", "WMAPro", ""
};

static void get_audio_bitrate_string(unsigned char c, char *str, int is_lpcm) {
    if (is_lpcm) {
        *str = '\0';
        if (c & 0x01) strcat(str, "16 ");
        if (c & 0x02) strcat(str, "20 ");
        if (c & 0x04) strcat(str, "24 ");
        strcat(str, "bit");
    }
    else {
        sprintf(str, "%3d kbps", c * 8);
    }
}

int print_edid_extension_to_buffer(unsigned char *buf, 
                                   char *edidtxt,
                                   char *pager,
                                   int block) {

    int dbc_type, dbc_bytes, dtd_start, i, j, CEAblock = 0;
    int len = 0, pages = 1, dbclines = 0, adbcnum = 0, vdbcnum = 0;
    unsigned short pix;

    /* Only CEA EDID extensions are supported in this implementation */
    if (buf[0]!= CEA_EDID_TAG)
        return -1;

    /* Only CEA revision 1 to 3 are supported in this implementation */
    if (buf[1] < 1 || buf[1] > 3) 
        return -1;

    /* Byte 2 is start byte of Detailed Timing Descriptor */
    dtd_start = buf[2]; 

    pages++;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }
    len += sprintf(edidtxt + len, "EIA/CEA-861 Information\n\n");
    len += sprintf(edidtxt + len, "   DTV underscan %s\n", 
            (buf[3] & 0x80) ? "Supported" : "Not Supported");
    len += sprintf(edidtxt + len, "   Basic Audio %s\n",    
            (buf[3] & 0x40) ? "Supported" : "Not Supported");
    len += sprintf(edidtxt + len, "   YCbCr 4:4:4 %s\n",
            (buf[3] & 0x20) ? "Supported" : "Not Supported");
    len += sprintf(edidtxt + len, "   YCbCr 4:2:2 %s\n",   
            (buf[3] & 0x10) ? "Supported" : "Not Supported");
    len += sprintf(edidtxt + len, "   Native formats %d\n", buf[3] & 0x0f);;

    /* Decode Data Block Collections */
    if (dtd_start > 4) {
        pages++;
        if (pager != NULL) {
            len += sprintf(edidtxt + len, "\n%s\n", pager);
        }
        i = 4;
        while (i < dtd_start) {
            dbc_type = (buf[i] & 0xe0) >> 5;
            dbc_bytes = (buf[i] & 0x1f);
            if (dbc_bytes <= 0)
                break;
            i += dbc_bytes;
            if (dbc_type == AUDIO_DATA_BLOCK_TAG) {
                if (adbcnum == 0) {
                    len += sprintf(edidtxt + len, 
                            "\nCEA Audio Formats Supported:\n");
                    dbclines += 2;
                }
                for (j = 0; j <= dbc_bytes; j += 3) {
                    int code, channels;
                    char bitratestr[20];
                    code = CEA_AUDIO_CODE(buf[j]);
                    channels = CEA_AUDIO_CHANNELS(buf[j]);
                    get_audio_bitrate_string(buf[j+2], bitratestr, 
                            (code == 1) ? 1 : 0);
                    len += sprintf(edidtxt + len, 
                            "%s %d-channel, bit rate %s\n",
                            audio_format[code], channels, bitratestr);
                    dbclines++;
                    VLOG_DEBUG_CAT(VLOG_VLIB,
                        "Audio Data Block: 0x%02x 0x%02x 0x%02x",
                        buf[j], buf[j+1], buf[j+2]);
                }
            }
            else if (dbc_type == VIDEO_DATA_BLOCK_TAG) {
                int code;
                if (vdbcnum == 0) {
                    len += sprintf(edidtxt + len, 
                            "\nCEA Short Video Descriptors:\n");
                    dbclines += 2;
                }
                code = (buf[i] & 0x7f);
                if (code >= sizeof(short_video_descriptor) / sizeof (char *))
                    continue;
                if (buf[i] & 0x80)
                    len += sprintf(edidtxt + len, "(native) %s", 
                        (short_video_descriptor[buf[i] & 0x7f]));
                else
                    len += sprintf(edidtxt + len, " %s", 
                        (short_video_descriptor[buf[i] & 0x7f]));
                vdbcnum++;
                if (vdbcnum % 3 == 0) {
                    len += sprintf(edidtxt + len, "\n");
                    dbclines++;
                }
                VLOG_DEBUG_CAT(VLOG_VLIB,
                        "Video Data Block: 0x%02x 0x%02x 0x%02x",
                        buf[i], buf[i+1], buf[i+2]);
            }
            else {
                VLOG_WARNING_CAT(VLOG_VLIB, 
                    "DBC Tag 0x%x not supported", dbc_type);
            }
            if (dbclines % 12 == 0) {
                dbclines = 0;
                pages++;
                if (pager != NULL) {
                    len += sprintf(edidtxt + len, "\n%s\n", pager);
                }
            }
        }
    }

    /* Decode Detailed Timing Descriptors */
    for (i = dtd_start; i < EDID_DATA_LEN - 17; i += 18) {
        pix = ((unsigned short) buf[i + 1] << 8) + (unsigned short) buf[i];
        if (pix) {
            ++pages;
            if (pager != NULL) {
                len += sprintf(edidtxt + len, "\n%s\n", pager);
            }
            len += sprintf(edidtxt + len, "\nCEA Descriptor Block #%d:\n", 
                ++CEAblock);
            len += decode_descr_block(&buf[i], NULL, 1, edidtxt + len);
            len += sprintf(edidtxt + len, "\n");
        }
    }

    pages++;
    if (pager != NULL) {
        len += sprintf(edidtxt + len, "\n%s\n", pager);
    }

    len += sprintf(edidtxt + len, "\nRaw Extension Block %d\n\n", block);
    for (i = 0; i < EDID_DATA_LEN; i++) {
        len += sprintf(edidtxt + len, "%02x ", buf[i]);
        if (i % 16 == 15) len += sprintf(edidtxt + len, "\n");
    }
    edidtxt[len - 1] = '\0';

    return pages;
}

/****************************
 * Routines to modify EDID  *
 ****************************/
static void make_descr_block_1(unsigned char *buf, 
                               struct LUT_struct *lutp,
                               int hspol,
                               int vspol) {
    unsigned short x, y;
    double pix;
  
    /* Set pixel clock */
    pix = lutp->a_pcr * (double) 100;    
    buf[0] = ((unsigned short) pix % 256);
    buf[1] = ((unsigned short) pix / 256);

    /* Set horiz active and blanking */
    x = lutp->a_orig_hres; y = lutp->a_hfp + lutp->a_hbp + lutp->a_hsync;
    buf[2] = x % 256;
    buf[3] = y % 256;
    buf[4] = (((unsigned char) (x / 256) & 0x0f) << 4) | 
                ((unsigned char) (y / 256) & 0x0f); 
    
    /* Set vert active and blanking */
    x = lutp->a_orig_vres; y = lutp->a_vfp + lutp->a_vbp + lutp->a_vsync;
    buf[5] = x % 256;
    buf[6] = y % 256;
    buf[7] = (((unsigned char) (x / 256) & 0x0f) << 4) | 
                ((unsigned char) (y / 256) & 0x0f); 

    /* Set horiz sync offset and pulse width */
    x = lutp->a_hfp; y = lutp->a_hsync;
    buf[8] = x % 256;
    buf[9] = y % 256;
    buf[11] = (((unsigned char) (x / 256) & 0x03) << 6) |
                (((unsigned char) (y / 256) & 0x03) << 4); 

    /* Set vert sync offset and pulse width */
    x = lutp->a_vfp; y = lutp->a_vsync;
    buf[10] = (x % 16) << 4;
    buf[10] |= (y % 16);
    buf[11] |= (((unsigned char) (x / 16) & 0x03) << 2) |
                ((unsigned char) (y / 16) & 0x03); 

    /* Hard code image sizes to and borders  */
    buf[12] = 0xC4, buf[13] = 0x8E, buf[14] = 0x21; 
    buf[15] = 0x00, buf[16] = 0x00;

    /* Hardcode to non-interlace (bit 7), non-stereo */
    /* (bit 6-5), separate-sync (bits 4-3).          */
    buf[17] = 0x18;

    /* Set vsync polarity (bit 2) */
    if (vspol)     
        buf[17] |= 0x04;

    /* Set hsync polarity (bit 1) */
    if (hspol)
        buf[17] |= 0x02;
}

static void write_lut_to_edid_buf(HAL *hp,
                                  unsigned char *edidbuf,
                                  int lutid,
                                  int descriptor,
                                  int hspol,
                                  int vspol) {
    struct LUT_struct lut;
    unsigned char buf[EDID_DESCR_SIZE];

    if (get_lut_entry_by_lutid(hp, &lut, lutid)) {
        /* Create descriptor block from LUT */
        make_descr_block_1(buf, &lut, hspol, vspol);
    }
    else {
        /* Use descriptor from default EDID */
        int start;

        /* EDID bytes 54 thru 71 is descriptor block #1 etc.*/
        start = EDID_DESCR_START;
        if (descriptor > 1) start += EDID_DESCR_SIZE;
        if (descriptor > 2) start += EDID_DESCR_SIZE;
        if (descriptor > 3) start += EDID_DESCR_SIZE;

        if (hal_get_hardware_type(hp) == PLX_DEVICE)
            memcpy(buf, (char *) &default_xp200_eeprom_data[start], 
                    EDID_DESCR_SIZE);
        else
            memcpy(buf, (char *) &default_i50_eeprom_data[start], 
                    EDID_DESCR_SIZE);
    }
    replace_descr_block(edidbuf, buf, descriptor);
}

/**************************************************************
 * Returns 1 if descriptor block 'block' (in EDID 'buf' with  *
 * length 'len') is video timing data otherwise returns 0 if  *
 * block 'block' is some other block type or -1 if error.     *
 * On return, if return code is 1, the LUT pointed to by lutp *
 * will be filled with the LUT from descriptor block 'block'  *
 **************************************************************/
int get_lut_from_edid_descriptor(unsigned char *buf, 
                                 int block,
                                 struct LUT_struct *lutp) {
    int i, rc = -1;
    unsigned char checksum = 0;
    unsigned short pix;
    char monname[64];

    if (lutp == NULL) {
        return rc;
    }

    for (i = 0; i < sizeof(edid_header); i++) 
        if (buf[i] != edid_header[i]) {
            return rc;
        }

    for (i = 0; i < EDID_DATA_LEN; i++) 
        checksum += buf[i];

    // PSCW 09/01/10 - commented out the following since 
    // since some monitors don't have correct checksums 
    // if (checksum != 0) {
    //    return rc;
    //}

    get_edid_product_id(buf, monname);

    i = 36 + block * 18;

    /* Initialize the LUT struct */
    memset(lutp, 0, sizeof(struct LUT_struct));
    lutp->a_phase = 16;
    lutp->a_cpmp = 0xff;
    lutp->a_noise = 0x0f09;
    lutp->a_rgain = lutp->a_ggain = lutp->a_bgain = 0x80;
    lutp->a_roffset = lutp->a_goffset = lutp->a_boffset= 0x40;
    lutp->a_vmargin = 2;
    lutp->d_tx_hres = lutp->d_tx_vres = 0xfff0;
    lutp->d_tx_refresh = lutp->d_tx_pcr = (double) -999999;

    /* Set pcr, refresh etc. in LUT struct */
    pix = ((unsigned short) buf[i + 1] << 8) + (unsigned short) buf[i];
    if (pix) {
        decode_descr_block(&buf[i], lutp, 0, NULL);
        lutp->a_pcr = (double) pix / (double) 100;
        lutp->a_refresh = (double) pix * (double) 10 * (double) 1000 /
            (double) (lutp->a_orig_hres+lutp->a_hsync+lutp->a_hfp+lutp->a_hbp)/
            (double) (lutp->a_orig_vres+lutp->a_vsync+lutp->a_vfp+lutp->a_vbp);
        sprintf(lutp->comment, "%s %dx%d@%d", monname, lutp->a_orig_hres,
            lutp->a_orig_vres, (int) (lutp->a_refresh + 0.5));
        build_lut_entry(lutp, PLX_DEVICE);
        rc = 1;
    }
    else if (buf[i + 3] == 0xfc) {
        strncpy(lutp->comment, (char *) &buf[i + 5], 63);
        lutp->comment[63] = '\0';
        rc = 0;
    }
    return rc; 
}

/**************************************************************
 * Returns 1 if descriptor block 'block' (in EDID 'buf' with  *
 * length 'len') is video timing data otherwise returns -1.   *
 **************************************************************/
int get_lut_from_edid_descriptor_extension(unsigned char *buf, 
                                           int block,
                                           char *monname,
                                           struct LUT_struct *lutp) {
    int i, rc = -1;
    unsigned short pix;

    /* Initialize the LUT struct */
    memset(lutp, 0, sizeof(struct LUT_struct));
    lutp->a_phase = 16;
    lutp->a_cpmp = 0xff;
    lutp->a_noise = 0x0f09;
    lutp->a_rgain = lutp->a_ggain = lutp->a_bgain = 0x80;
    lutp->a_roffset = lutp->a_goffset = lutp->a_boffset= 0x40;
    lutp->a_vmargin = 2;
    lutp->d_tx_hres = lutp->d_tx_vres = 0xfff0;
    lutp->d_tx_refresh = lutp->d_tx_pcr = (double) -999999;

    /* Set pcr, refresh etc. in LUT struct */
    i = buf[2] + block * 18;
    if (i > EDID_DATA_LEN - 17) 
        return rc;
    pix = ((unsigned short) buf[i + 1] << 8) + (unsigned short) buf[i];
    if (pix) {
        decode_descr_block(&buf[i], lutp, 0, NULL);
        lutp->a_pcr = (double) pix / (double) 100;
        lutp->a_refresh = (double) pix * (double) 10 * (double) 1000 /
            (double) (lutp->a_orig_hres+lutp->a_hsync+lutp->a_hfp+lutp->a_hbp)/
            (double) (lutp->a_orig_vres+lutp->a_vsync+lutp->a_vfp+lutp->a_vbp);
        sprintf(lutp->comment, "%s %dx%d@%d", monname, lutp->a_orig_hres,
            lutp->a_orig_vres, (int) (lutp->a_refresh + 0.5));
        build_lut_entry(lutp, PLX_DEVICE);
        rc = 1;
    }

    return rc; 
}

/**********************************************************************
 * Scan each of the descriptors in the edid buffer 'edidbuf' find the *
 * LUT the descriptor correspond to. Returns the array of LUT IDs for *
 * the descriptors in the edid-lut map 'edid-lut'. Also return the    *
 * array of new LUTs (i.e. not found in LUT table) in 'lutp', and the *
 * number of new LUTs in function value.                              *
 **********************************************************************/
int get_edid_lut_map(HAL *hp,
                     unsigned char *edidbuf, 
                     int extension,
                     struct LUT_struct *lutp,
                     struct edid_lut_map *edid_lut,
                     int mapsize)
{
    int rc, i, match, lutcnt = 0, max_descriptors;
    char monname[20];

    get_edid_product_id(edidbuf, monname);
    (void) trim_string(monname);

    if (extension)
        max_descriptors = (EDID_DATA_LEN - edidbuf[2]) / 18;
    else
        max_descriptors = MAX_EDID_DESC_COUNT;

    for (i = 0; i < max_descriptors && i < mapsize; i++) {
        if (extension) {
            rc = get_lut_from_edid_descriptor_extension(edidbuf, i, monname, 
                    &lutp[lutcnt]);
        }
        else {
            rc = get_lut_from_edid_descriptor(edidbuf, i+1, &lutp[lutcnt]);
        }
        if (rc == 1) {
            int match = get_matching_lut_entry(hp, &lutp[lutcnt]);
            if (match > 0) {
                edid_lut[i].type = EDID_LUT_TYPE_LUTID;
                edid_lut[i].value = match;
            } 
            else {
                snprintf(lutp[lutcnt].comment, 32, "%s %dx%d@%d", monname, 
                    lutp[lutcnt].a_orig_hres, lutp[lutcnt].a_orig_vres, 
                    (int)(lutp[lutcnt].a_refresh+0.1));
                edid_lut[i].type = EDID_LUT_TYPE_NEWLUT;
                edid_lut[i].value = lutcnt++;
            }
        }
        else {
            edid_lut[i].type = EDID_LUT_TYPE_NONE;
        }
    }

    return lutcnt;
}

/***************************************************************
 * Update LUT from the descriptors from EDID buffer 'edidbuf'  *
 * TODO: need to handle extended EDID descriptors              *
 ***************************************************************/
int update_lut_from_buffer(HAL *hp, unsigned char *edidbuf)
{
    struct LUT_struct lut[MAX_EDID_DESC_COUNT];
    struct edid_lut_map edid_lut[MAX_EDID_DESC_COUNT];
    int i, newluts;

    if (hp == NULL || edidbuf == NULL)
        return -1;

    newluts = get_edid_lut_map(hp, edidbuf, 0, lut, edid_lut, 
            MAX_EDID_DESC_COUNT);
    
    if (newluts) {
        for (i = 0; i < MAX_EDID_DESC_COUNT; i++) {
            if (edid_lut[i].type == EDID_LUT_TYPE_NEWLUT) {
                int index = edid_lut[i].value;
                if (add_lut_entry(hp, &lut[index]) != 0) {
                    VLOG_INFO("Failed to add LUT %d from EDID to LUT table",
                        index);
                    return -1;
                }
                edid_lut[i].type = EDID_LUT_TYPE_LUTID;
                edid_lut[i].value = lut[index].a_alutreg;
            }
        }
    }

    return 0;
}

/***************************************************************
 * Update EDID list from the descriptors in buffer 'edidbuf'   *
 * TODO: need to handle extended EDID descriptors              *
 ***************************************************************/
int update_edid_list_from_buffer(HAL *hp, unsigned char *edidbuf)
{
    struct LUT_struct lut[MAX_EDID_DESC_COUNT];
    struct edid_lut_map edid_lut[MAX_EDID_DESC_COUNT];
    char cmdbuf[80];
    int i;

    if (hp == NULL || edidbuf == NULL)
        return -1;

    get_edid_lut_map(hp, edidbuf, 0, lut, edid_lut, MAX_EDID_DESC_COUNT);
    
    for (i = 0; i < MAX_EDID_DESC_COUNT; i++) {
        sprintf(cmdbuf, "del_edid_list %d > /dev/null 2>&1", i+1);
        system(cmdbuf);
        if (edid_lut[i].type == EDID_LUT_TYPE_LUTID) {
            sprintf(cmdbuf, 
                    "add_edid_list -desc %d -alutreg %d > /dev/null 2>&1",
                    i+1, edid_lut[i].value);
            system(cmdbuf);
        }
    }

    return 0;
}


/*************************************************************
 * Routine to make a EDID using Descriptor#1 -3 in the EDID  *
 * from the attached monitor. Returns -3 if not V2D Board,   *
 * -2 failed to read monitor EDID, -1 failed to write EEPROM *
 * Note only the basic EDID block is loaded into the EEPROM. * 
 *************************************************************/
int load_edid_eeprom_from_monitor(HAL *hp) {
    int i, len = EDID_DATA_LEN;
    unsigned char monbuf[EDID_DATA_LEN], edidbuf[EDID_DATA_LEN];

    if (hp == NULL) {
        return -3;
    }

    /* read monitor edid */
    if (hal_read_mon_edid(hp, monbuf, len) < 0) {
        VLOG_WARNING("Failed to read monitor EDID.");
        return -2;
    }
    /* replace descr blocks 1-3 with text if they're not detailed timing */
    for (i = 54; i < 107; i += sizeof(blank_text)) {
        if (other_mon_descr(&monbuf[i]))  
            memcpy(&monbuf[i], blank_text, sizeof(blank_text));
    }

    get_default_edid(hp, edidbuf, monbuf[20]);
    edidbuf[127] += edidbuf[35] - monbuf[35];   /* fix up checksum */
    edidbuf[35] = monbuf[35];                   /* replace est timing I */
    edidbuf[127] += edidbuf[36] - monbuf[36];   /* fix up checksum */
    edidbuf[36] = monbuf[36];                   /* replace est timing II */
    edidbuf[127] += edidbuf[37] - monbuf[37];   /* fix up checksum */
    edidbuf[37] = monbuf[37];                   /* replace manufacturer's */
                                                /* reserved timing */
    for (i = 38; i <= 107; i++) {               /* replace std timing */
        edidbuf[127] += edidbuf[i] - monbuf[i]; /* identification and */
        edidbuf[i] = monbuf[i];                 /* descr block 1 to 3 */
    }

    /* write (modified) monitor edid to eeprom */
    return hal_load_edid_eeprom(hp, edidbuf, EDID_DATA_LEN);
}

/*************************************************************
 * Routine to load the EDID EEPROM with default EDID EEPROM  *
 * data modified with descriptors defined in the EDID list   * 
 *************************************************************/
int load_edid_eeprom_with_descriptor(HAL *hp, unsigned char videoinput) {
    int edidlistcount;
    struct edid_list_params *paramsp;
    unsigned char edidbuf[EDID_DATA_LEN];

    edidlistcount = retrieveEdidDescriptorListValues(hp);

    /* Get default EDID for the videoinput the caller wants */
    get_default_edid(hp, edidbuf, videoinput);

    /* Add 640x480 to std timing for analog */
    if ((videoinput & 0x80) == 0) {               
        edidbuf[127] += edidbuf[35] - 0x3f;
        edidbuf[35] = 0x3f;
    }

    /* Replace decriptors with those in config file */
    if (edidlistcount > 0) {
        int blk, lu, hs, vs;
        foreach_edid_list_entry(paramsp) {
            blk = paramsp->edid_desc_value;
            lu = paramsp->edid_alutreg_value;
            hs = paramsp->edid_hpol_value;
            vs = paramsp->edid_vpol_value;
            write_lut_to_edid_buf(hp, edidbuf, lu, blk, hs, vs);
        }
    }

    /* Write EDID to FPGA */
    return hal_load_edid_eeprom(hp, edidbuf, EDID_DATA_LEN);
}

/*************************************************************
 * Routine to load the EDID EEPROM with default EDID EEPROM  *
 *************************************************************/
int load_edid_eeprom_with_default(HAL *hp, unsigned char videoinput) {
    unsigned char edidbuf[EDID_DATA_LEN];

    /* Get default EDID for the videoinput the caller wants */
    get_default_edid(hp, edidbuf, videoinput);

    /* Add 640x480 to std timing for analog */
    if ((videoinput & 0x80) == 0) {               
        edidbuf[127] += edidbuf[35] - 0x3f;
        edidbuf[35] = 0x3f;
    }

    /* Write EDID to FPGA */
    return hal_load_edid_eeprom(hp, edidbuf, EDID_DATA_LEN);
}

