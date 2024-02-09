#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#ifdef __XPI__
#include <gd.h>
#endif

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "osd_control.h"

#define ICON_COLOR_3_4BIT    0xe
#define ICON_COLOR_2_4BIT    0xa
#define ICON_COLOR_1_4BIT    0x8
#define ICON_COLOR_3        0xd6
#define ICON_COLOR_2        0xac
#define ICON_COLOR_1        0x82
#define ICON_COLOR_0        0x00


static unsigned int get_osd_fpga_val32_k(HAL * hp, unsigned long reg)
{
     if (hp == NULL)
         return 0;

#ifdef __XPI__
     return hal_get_s6fpga_val32_k(hp, reg);
#else
     return hal_get_fpga_val32_k(hp, reg);
#endif 

}

static void  set_osd_fpga_val32_k(HAL * hp, unsigned long reg, unsigned long val)
{
    if(hp == NULL)
      return ;
#ifdef __XPI__
     hal_set_s6fpga_val32_k(hp, reg, val);
#else
     hal_set_fpga_val32_k(hp, reg, val);
#endif

}


/*******************************************************
 * Keyboard and guide ruler icon routines              *
 *******************************************************/
static int read_icon_bmp(FILE *fd, 
                         unsigned char *buffer,
                         int *sizex,
                         int *sizey) {
    unsigned char buff[8192], *rp;
    int i, j, len, start, width, height;
    unsigned short bitsperpixel;

    len = fread(buff, 1, 8192, fd);

    if (len < 54) {
        VLOG_INFO_CAT(VLOG_VLIB, "Bad BMP file: length = %d", len);
        return -1;
    }
    if (buff[0] != 0x42 || buff[1] != 0x4d) { 
        VLOG_INFO_CAT(VLOG_VLIB, "Bad BMP file: magic number = 0x%02x%02x", 
                buff[0], buff[1]);
        return -1;
    }
    start = *(int *) &buff[10];
    *sizex = width = *(int *) &buff[18];
    *sizey = height = *(int *) &buff[22];
    bitsperpixel = *(unsigned short *) &buff[28];

    if (width % 4 != 0) {
        VLOG_INFO_CAT(VLOG_VLIB, 
                "Bad BMP file: width %d must be multiples of 4", width);
        return -1;
    }

    if (width * height > MAX_OSD_PIXELS) {
        VLOG_INFO_CAT(VLOG_VLIB, "Bad BMP file: too many pixels %d", 
                width * height);
        return -1;
    }

    if (bitsperpixel == 4) {
        for (rp = &buff[start], j = 0; j < height; j++) {
            for (i = 0; i < width; i++) {
                unsigned char c;
                if (i % 2) {
                    c = (*rp) & 0x0f; rp++;
                }
                else {
                    c = (*rp) >> 4;
                }
                if (c >= ICON_COLOR_3_4BIT)
                    buffer[(height - j - 1) * width + i] = 3;
                else if (c >= ICON_COLOR_2_4BIT)
                    buffer[(height - j - 1) * width + i] = 2;
                else if (c >= ICON_COLOR_1_4BIT)
                    buffer[(height - j - 1) * width + i] = 1;
                else 
                    buffer[(height - j - 1) * width + i] = 0;
            }
        }
    } 
    else if (bitsperpixel > 8) { 
        rp = &buff[start + (bitsperpixel / 8) - 1]; 
        for (j = 0; j < height; j++) {
            for (i = 0; i < width; i++) {
                if (*rp >= ICON_COLOR_3)
                    buffer[(height - j - 1) * width + i] = 3;
                else if (*rp >= ICON_COLOR_2)
                    buffer[(height - j - 1) * width + i] = 2;
                else if (*rp >= ICON_COLOR_1)
                    buffer[(height - j - 1) * width + i] = 1;
                else 
                    buffer[(height - j - 1) * width + i] = 0;
                rp += (bitsperpixel / 8);
            }
        }
    }
    else {
        VLOG_INFO_CAT(VLOG_VLIB, "BMP file: bits per pixel %d not supported", 
                bitsperpixel);
        return -1;
    }

    VLOG_DEBUG_CAT(VLOG_VLIB, "%s: returning %d bytes:", __FUNCTION__, 
            width * height);
    VLOG_DEBUG_HEXDATA(buffer, width * height);

    return (width * height);
}

static int setup_icon_controlmap(HAL *hp, 
                                 int startx,
                                 int starty,
                                 char *filename, 
                                 int color)
{
    unsigned char buffer[8192];
    int i, len, sizex, sizey, iter, pixelcnt = 0, rc = -1;
    FILE *fd;
    int osdctl =  FPGA_DOSD2CTL;
    int osdnumlocs =  FPGA_DOSD2NUMLOCS;
    int osdmemaddr =  OSD2MEMADDR;
    int cno = hal_get_board_number(hp) + 1;

    sizex = 64; sizey = 64; len = 4096;

    memset(buffer, 0, sizeof(buffer));
    if (filename != NULL) {
        if ((fd = fopen(filename, "rb")) == NULL) {
            VLOG_WARNING_CAT(VLOG_VLIB, 
                    "Channel %d. Failed to open file %s. %m", 
                    cno, filename);
            return -1;
        }

        len = read_icon_bmp(fd, buffer, &sizex, &sizey);

        if (len < 0) {
            VLOG_WARNING_CAT(VLOG_VLIB, 
                    "Channel %d. Failed to read icon BMP file %s.", 
                    cno, filename);

        }

        fclose(fd);
    }

    if (len <= 0) return -1;

    /* Disable OSD */
    set_osd_fpga_val32_k(hp, osdctl, 0x0);

    /* Wait till software can write to OSD memory */
    for (i = 0; i < 100; i++) {
        if (get_osd_fpga_val32_k(hp, osdctl) & OSDCTL_HANDSHAKE) 
            usleep(1000);
        else 
            break;
    }

    if (i == 100) { 
        VLOG_WARNING_CAT(VLOG_VLIB, 
                "Channel %d. Error program OSD memory (Reg 0x%x = 0x%x)",
                cno, osdctl, get_osd_fpga_val32_k(hp, osdctl));
        return -1; 
    }
     
    rc = 0;
    while (pixelcnt < 8192) {
        unsigned short ctrlcode = 0;
        if (pixelcnt < sizex * sizey) {
            for (ctrlcode = 0, i = 0; i < 8; i++) {          
                ctrlcode |= buffer[pixelcnt + i] << (2 * i);
            }
        }
        else {
            ctrlcode = 0xffff;
        }
        for (iter = 0; iter < 3; iter++) {
            if ((get_osd_fpga_val32_k(hp, osdctl) & 0x8000) == 0) 
                break;
            else usleep(1);
        }
        if (iter < 3) {
#ifdef __XPI__
            set_osd_fpga_val32_k(hp, osdmemaddr + pixelcnt / 8, ctrlcode);
#else
            set_osd_fpga_val32_k(hp, osdmemaddr + pixelcnt / 2, ctrlcode);
#endif
        }
        else {
            VLOG_WARNING_CAT(VLOG_VLIB,
                    "Channel %d. Error  write to OSD memory.", cno);
            rc = -1;
        }
        pixelcnt += 8;
    }

    /* Set color */
    hal_set_icon_color(hp, color);

    /* Set position and size */
    hal_set_icon_xpos(hp, startx, sizex);
    hal_set_icon_ypos(hp, starty, sizey);

    /* Set OSD number of locations programmed by software */
    set_osd_fpga_val32_k(hp, osdnumlocs, pixelcnt / 8);

    /* Tell FPGA to read OSD memory */
    set_osd_fpga_val32_k(hp, osdctl, OSDCTL_ENABLE|OSDCTL_HANDSHAKE);

    return rc;
}

#ifdef __XPI__
static int setup_string_controlmap(HAL *hp, 
                                   int startx, 
                                   int starty, 
                                   int color,
                                   char *str)
{
    unsigned char buffer[8192];
    int i, j, iter, pixelcnt = 0, rc = -1;
    int osdctl =  FPGA_DOSD2CTL;
    int osdnumlocs =  FPGA_DOSD2NUMLOCS;
    int osdmemaddr =  OSD2MEMADDR;
    int cno = hal_get_board_number(hp) + 1;
    int black, white, brect[9];
    int sizex = DEFAULT_STRING_ICON_WIDTH, sizey = DEFAULT_STRING_ICON_HEIGHT;
    gdImagePtr im;
    char *fontpath = (char *) "/usr/share/fonts/ipvs.ttf";

    /* create an image, sizex by sizey pixels */
    im = gdImageCreate(sizex, sizey);

    /* allocate black color */
    black = gdImageColorAllocate(im, 0, 0, 0);

    /* allocate white color */
    white = gdImageColorAllocate(im, 255, 255, 255);

    /* draw the string in white on the image */
    if (gdImageStringFT(im, &brect[0], white, fontpath, 
                16.0, 0.0, 0, sizey/2 + 8, str) != NULL) {
        VLOG_WARNING_CAT(VLOG_VLIB, 
                "Channel %d. Error gdImageStringFT %d, %m", cno, errno);
        return -1;
    }

    /* fill the buffer with default color */
    memset(buffer, 0, sizeof(buffer));
  
    /* write text from the image over the buffer */
    for (j = 0; j < sizey; j++) {
        for (i = 0; i < sizex; i++) {
            int c = gdImageGetPixel(im, i, j);
            if (c != black) {
                /* use 3 instead of 2 if you want transparent foreground */
                buffer[j * sizex + i] = 2;   
            }
        }
    } 
    gdImageDestroy(im);

    /* Disable OSD */
    set_osd_fpga_val32_k(hp, osdctl, 0x0);

    /* Wait till software can write to OSD memory */
    for (i = 0; i < 100; i++) {
        if (get_osd_fpga_val32_k(hp, osdctl) & OSDCTL_HANDSHAKE) 
            usleep(1000);
        else 
            break;
    }

    if (i == 100) { 
        VLOG_WARNING_CAT(VLOG_VLIB, 
                "Channel %d. Error program OSD memory (Reg 0x%x = 0x%x)",
                cno, osdctl, get_osd_fpga_val32_k(hp, osdctl));
        return -1; 
    }
     
    rc = 0;
    while (pixelcnt < 8192) {
        unsigned short ctrlcode = 0;
        if (pixelcnt < sizex * sizey) {
            for (ctrlcode = 0, i = 0; i < 8; i++) {          
                ctrlcode |= buffer[pixelcnt + i] << (2 * i);
            }
        }
        else {
            ctrlcode = 0xffff;
        }
        for (iter = 0; iter < 3; iter++) {
            if ((get_osd_fpga_val32_k(hp, osdctl) & 0x8000) == 0) 
                break;
            else usleep(1);
        }
        if (iter < 3) {
            set_osd_fpga_val32_k(hp, osdmemaddr + pixelcnt / 8, ctrlcode);
        }
        else {
            VLOG_WARNING_CAT(VLOG_VLIB,
                    "Channel %d. Error  write to OSD memory.", cno);
            rc = -1;
        }
        pixelcnt += 8;
    }

    /* Set color */
    hal_set_icon_color(hp, color);

    /* Set position and size */
    hal_set_icon_xpos(hp, startx, sizex);
    hal_set_icon_ypos(hp, starty, sizey);

    /* Set OSD number of locations programmed by software */
    set_osd_fpga_val32_k(hp, osdnumlocs, pixelcnt / 8);

    /* Tell FPGA to read OSD memory */
    set_osd_fpga_val32_k(hp, osdctl, OSDCTL_ENABLE|OSDCTL_HANDSHAKE);

    return rc;
}
#endif

void hal_move_icon_xpos(HAL *hp, unsigned short x) {
    int osdstartx = FPGA_DOSD2STARTX;
    int osdstopx = FPGA_DOSD2STOPX;
    unsigned int osdsizex;

    osdsizex = get_osd_fpga_val32_k(hp, FPGA_DOSD2STOPX) - 
        get_osd_fpga_val32_k(hp, FPGA_DOSD2STARTX);
    set_osd_fpga_val32_k(hp, FPGA_DOSD2STARTX, x);
    set_osd_fpga_val32_k(hp, FPGA_DOSD2STOPX, x + osdsizex);
}

void  hal_move_icon_ypos(HAL *hp, unsigned short y) {
    unsigned int osdsizey;

    osdsizey = get_osd_fpga_val32_k(hp, FPGA_DOSD2STOPY) - 
        get_osd_fpga_val32_k(hp, FPGA_DOSD2STARTY);
    set_osd_fpga_val32_k(hp, FPGA_DOSD2STARTY, y);
    set_osd_fpga_val32_k(hp, FPGA_DOSD2STOPY, y + osdsizey);
}

void  hal_set_icon_xpos(HAL *hp, unsigned short x, unsigned short sizex) {
    int osdstartx =  FPGA_DOSD2STARTX;
    int osdstopx =  FPGA_DOSD2STOPX;

    set_osd_fpga_val32_k(hp, osdstartx, x);
    set_osd_fpga_val32_k(hp, osdstopx, x + sizex);
}

void hal_set_icon_ypos(HAL *hp, unsigned short y, unsigned short sizey) {
    int osdstarty =  FPGA_DOSD2STARTY;
    int osdstopy =  FPGA_DOSD2STOPY;

    set_osd_fpga_val32_k(hp, osdstarty, y);
    set_osd_fpga_val32_k(hp, osdstopy, y + sizey);
}

void hal_set_icon_color(HAL *hp, unsigned short color) {
    int osdcolor0 =  FPGA_DOSD2COLOR0;
    int osdcolor1 =  FPGA_DOSD2COLOR1;
    int osdcolor2 =  FPGA_DOSD2COLOR2;

    if (color == 0) color = 1;
    if (color == 255) color = 254;
    set_osd_fpga_val32_k(hp, osdcolor0, color);
    set_osd_fpga_val32_k(hp, osdcolor1, color);
    set_osd_fpga_val32_k(hp, osdcolor2, 255 - color);
}

void hal_set_icon_on(HAL *hp, int color, int x, int y, int sizex, int sizey) {
    int osdctl =  FPGA_DOSD2CTL;

    hal_set_icon_color(hp, color);
    hal_set_icon_xpos(hp, x, sizex);
    hal_set_icon_ypos(hp, y, sizey);
    set_osd_fpga_val32_k(hp, osdctl, OSDCTL_ENABLE|OSDCTL_HANDSHAKE);
}

void hal_set_icon_off(HAL *hp) {
    int osdctl =  FPGA_DOSD2CTL;

    set_osd_fpga_val32_k(hp, osdctl, 0x0);
}

int hal_init_keyboard_icon(HAL *hp, int state, int color) {
    if (setup_icon_controlmap(hp, 
            DEFAULT_KEYBOARD_ICON_STARTX, DEFAULT_KEYBOARD_ICON_STARTY,
            DEFAULT_KEYBOARD_ICON_FILE, color) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d. Error set OSD2 memory map", 
                hal_get_board_number(hp) + 1);
        return -1;
    }
    if (state == 0) hal_set_icon_off(hp);
    return 0; 
}

int hal_init_guideruler_icon(HAL *hp, int state, int color) {
    if (setup_icon_controlmap(hp, 
            DEFAULT_GUIDERULER_ICON_STARTX, DEFAULT_GUIDERULER_ICON_STARTY,
            DEFAULT_GUIDERULER_ICON_FILE, color) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d. Error set OSD2 memory map",
                hal_get_board_number(hp) + 1);
        return -1;
    }
    if (state == 0) hal_set_icon_off(hp);
    return 0; 
}

int hal_init_string_icon(HAL *hp, int state, int color, char *str)
{
#ifdef __XPI__
    if (setup_string_controlmap(hp, DEFAULT_STRING_ICON_STARTX, 
                DEFAULT_STRING_ICON_STARTY, color, str) < 0) {
        VLOG_INFO_CAT(VLOG_VLIB, "Channel %d. Error set OSD2 memory map",
                hal_get_board_number(hp) + 1);
        return -1;
    }
    if (state == 0) hal_set_icon_off(hp);
#endif
    return 0; 
}

/***********************************************************
 * Local Mouse Cursor Routines. Hardcoded for 5x5 cursors, *
 * i.e. cursors that center at pixel position (2, 2).      *
 ***********************************************************/
static unsigned char mousecursor_pattern[] = {
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static unsigned char pixcursor_pattern[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static unsigned char default_cursor_color[] = {
    0xff, 0xe0, 0xe0, 0xe0, 0x1c, 0x03, 0xfc, 0xd3,
    0x1b, 0xff, 0x49, 0xe0, 0x1c, 0x03, 0xfc, 0xd3
};

#define DEFAULTCURSOR_SIZE      9
#define DEFAULTCURSOR_CENTER    4

static void init_multimouse_cursor(HAL *hp, 
                                   unsigned short x, 
                                   unsigned short y,
                                   unsigned short maxx,
                                   unsigned short maxy,
                                   int cursor) {
    int i, cursor_center, cursor_size, remote_flag, cursor_num;
    unsigned char patterny[FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE];
    unsigned char pattern[FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE];
    unsigned char *patt;
    unsigned long val32;
    int k = 0;
    unsigned char default_pattern[FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE];

    /* Save which mice are enabled for later */
    val32 = get_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE) & 0xffff0000;

    /* Set cursor global disable */
    set_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE, 0);

    remote_flag = cursor & REMOTECURSOR_FLAG;
    cursor_num = cursor & ~REMOTECURSOR_FLAG;

    if (cursor_num >= MAX_MULTIMOUSE)
        return;

    if (cursor_num == MOUSECURSOR_NUM) {
        patt = mousecursor_pattern;
        cursor_size = MOUSECURSOR_SIZE;
        cursor_center = MOUSECURSOR_CENTER;
    }
    else if (cursor_num == PIX1CURSOR_NUM || cursor_num == PIX2CURSOR_NUM ||
            cursor_num == PIX3CURSOR_NUM) {
        patt = pixcursor_pattern;
        cursor_size = PIXCURSOR_SIZE;
        cursor_center = PIXCURSOR_CENTER;
    }
    else {
        int j;
        cursor_size = DEFAULTCURSOR_SIZE;
        cursor_center = DEFAULTCURSOR_CENTER;
        memset(default_pattern, 0, sizeof(default_pattern));
        for (i = 0; i < cursor_size; i++) {
            for (j = 0; j < cursor_size; j++) {
                default_pattern[FPGA_CURSOR_PATTSIZE * i + j] = 
                    default_cursor_color[cursor_num];
            }
        }
        patt = default_pattern;
    }

    /* Load cursor OSD RAM */
    memset(patterny, '\0', sizeof(patterny));
    if (y < cursor_center) {
        memcpy(patterny, &patt[FPGA_CURSOR_PATTSIZE * (cursor_center - y)],
                FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE - 
                FPGA_CURSOR_PATTSIZE * (cursor_center - y));
    }
    else if (y < maxy - cursor_center) {
        memcpy(patterny, patt, FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE);
    }
    else {
        memcpy(patterny, patt, FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE - 
            FPGA_CURSOR_PATTSIZE * 
            (y - maxy + cursor_center + FPGA_CURSOR_PATTSIZE - cursor_size));
    }

    memset(pattern, '\0', sizeof(pattern));
    if (x < cursor_center) {
        for (i = 0; i < FPGA_CURSOR_PATTSIZE; i++) {
            memcpy(&pattern[FPGA_CURSOR_PATTSIZE * i], 
                &patterny[FPGA_CURSOR_PATTSIZE * i + cursor_center - x], 
                FPGA_CURSOR_PATTSIZE - cursor_center + x);
        }
    }
    else if (x < maxx - cursor_center) {
        memcpy(pattern, patterny, sizeof(patterny));
    }
    else {
        for (i = 0; i < FPGA_CURSOR_PATTSIZE; i++) {
            memcpy(&pattern[FPGA_CURSOR_PATTSIZE * i], 
                    &patterny[FPGA_CURSOR_PATTSIZE * i], 
                    cursor_size - x + maxx - cursor_center);
        }
    }

    for (i = 0, k = 0; i < FPGA_CURSOR_PATTSIZE * FPGA_CURSOR_PATTSIZE; 
            i += 2, k += 1) {
        unsigned long val;
        val = (((unsigned long) pattern[i+1]) << 8) | pattern[i];
#ifdef __XPI__
        hal_set_s6fpga_val32_k(hp, 
                OSD1MEMADDR + cursor_num * FPGA_CURSOR_MEMSIZE + k, val);
#else
        hal_set_fpga_val32_k(hp, 
                OSD1MEMADDR + cursor_num * FPGA_CURSOR_MEMSIZE + i * 2, val);
#endif
    }

    /* Initialize cursor priorities and positions */
    x = (x > cursor_center) ? (x - cursor_center) : 0;
    x = (x < maxx - cursor_size) ? x : (maxx - cursor_size);
    y = (y > cursor_center) ? (y - cursor_center) : 0;
    y = (y < maxy - cursor_size) ? y : (maxy - cursor_size);

    /* Position mouse & remote chromakey cursors relative to remote window */
    if (remote_flag) {
        x = (x * hal_get_fpga_val32_k(hp, FPGA_DHSCALE)) / RX_MAX_SCALE_VAL +
            get_osd_fpga_val32_k(hp, FPGA_DHSHIFT);
        y = (y * hal_get_fpga_val32_k(hp, FPGA_DVSCALE)) / RX_MAX_SCALE_VAL +
            get_osd_fpga_val32_k(hp, FPGA_DVSHIFT);
    }

    /* Set cursor global enable and size */
    set_osd_fpga_val32_k(hp, FPGA_DOSDMMCTL_START + 4 * cursor_num, 
            ((unsigned long) cursor_num << 28) | 
            ((unsigned long) y << 14) | (unsigned long) x);
#ifdef __XPI__    
    set_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE, 
            val32 | (1 << (16+cursor_num)) | 0x20 | FPGA_CURSOR_PATTSIZE);
#else 
    set_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE, 
            0x20 | FPGA_CURSOR_PATTSIZE);
#endif     
}

static void set_multimouse_cursor_pos(HAL *hp, 
                                      unsigned short x, 
                                      unsigned short y,
                                      unsigned short maxx,
                                      unsigned short maxy,
                                      int cursor) {
    int cursor_center, cursor_size, remote_flag, cursor_num;

    cursor_num = cursor & ~REMOTECURSOR_FLAG;
    remote_flag = cursor & REMOTECURSOR_FLAG;
    if (cursor_num == MOUSECURSOR_NUM) {
        cursor_size = MOUSECURSOR_SIZE;
        cursor_center = MOUSECURSOR_CENTER;
    }
    else if (cursor_num == PIX1CURSOR_NUM || cursor_num == PIX2CURSOR_NUM ||
            cursor_num == PIX3CURSOR_NUM) {
        cursor_size = PIXCURSOR_SIZE;
        cursor_center = PIXCURSOR_CENTER;
    }
    else {
        cursor_size = DEFAULTCURSOR_SIZE;
        cursor_center = DEFAULTCURSOR_CENTER;
    }

    x = (x > cursor_center) ? (x - cursor_center) : 0;
    x = (x < maxx - cursor_size) ? x : (maxx - cursor_size);
    y = (y > cursor_center) ? (y - cursor_center) : 0;
    y = (y < maxy - cursor_size) ? y : (maxy - cursor_size);

    /* Position mouse & remote chromakey cursors relative to remote window */
    if (remote_flag) {
        x = (x * hal_get_fpga_val32_k(hp, FPGA_DHSCALE)) / RX_MAX_SCALE_VAL + 
            get_osd_fpga_val32_k(hp, FPGA_DHSHIFT);
        y = (y * hal_get_fpga_val32_k(hp, FPGA_DVSCALE)) / RX_MAX_SCALE_VAL + 
            get_osd_fpga_val32_k(hp, FPGA_DVSHIFT);
    }
    set_osd_fpga_val32_k(hp, FPGA_DOSDMMCTL_START + 4 * cursor_num, 
            ((unsigned long) cursor_num << 28) | 
            ((unsigned long) y << 14) | (unsigned long) x);
}

static void set_multimouse_cursor_status(HAL *hp, int status, int cursor) { 
    unsigned long val32 = get_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE);
    int cursor_num = cursor & ~REMOTECURSOR_FLAG;

    if (status) 
        set_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE, val32|(1<<(16+cursor_num)));
    else
        set_osd_fpga_val32_k(hp, FPGA_DOSDMMENABLE, val32&~(1<<(16+cursor_num)));
}

void hal_init_mouse_cursor(HAL *hp, 
                           unsigned short x, 
                           unsigned short y,
                           unsigned short maxx,
                           unsigned short maxy) {
    if (x <= maxx && y <= maxy)
        return init_multimouse_cursor(hp, x, y, maxx, maxy, MOUSECURSOR);
}

void hal_set_mouse_cursor_pos(HAL *hp, 
                              unsigned short x, 
                              unsigned short y,
                              unsigned short maxx,
                              unsigned short maxy) {
    if (x <= maxx && y <= maxy)
        return set_multimouse_cursor_pos(hp, x, y, maxx, maxy, MOUSECURSOR);
}

void hal_set_mouse_cursor_status(HAL *hp, int status) { 
    return set_multimouse_cursor_status(hp, status, MOUSECURSOR);
}

void hal_init_pix_cursor(HAL *hp, 
                         unsigned short x, 
                         unsigned short y,
                         unsigned short maxx,
                         unsigned short maxy,
                         int cursor) {
    if ((cursor == LOCALPIX1CURSOR || cursor == LOCALPIX2CURSOR ||
            cursor == LOCALPIX3CURSOR || cursor == REMOTEPIX1CURSOR ||
            cursor == REMOTEPIX2CURSOR || cursor == REMOTEPIX3CURSOR) &&
            x <= maxx && y <= maxy)
        return init_multimouse_cursor(hp, x, y, maxx, maxy, cursor);
}

void hal_set_pix_cursor_status(HAL *hp, int status, int cursor) { 
    if (cursor == LOCALPIX1CURSOR || cursor == LOCALPIX2CURSOR ||
            cursor == LOCALPIX3CURSOR || cursor == REMOTEPIX1CURSOR ||
            cursor == REMOTEPIX2CURSOR || cursor == REMOTEPIX3CURSOR)
        return set_multimouse_cursor_status(hp, status, cursor);
}

void hal_init_multimouse_cursor(HAL *hp, 
                                unsigned short x, 
                                unsigned short y,
                                unsigned short maxx,
                                unsigned short maxy,
                                int cursor) {
    int cursor_num = cursor & ~REMOTECURSOR_FLAG;
    if (cursor_num > PIX3CURSOR_NUM && cursor_num < MAX_MULTIMOUSE && 
            x <= maxx && y <= maxy)
        return init_multimouse_cursor(hp, x, y, maxx, maxy, cursor);
}

void hal_set_multimouse_cursor_pos(HAL *hp, 
                                   unsigned short x, 
                                   unsigned short y,
                                   unsigned short maxx,
                                   unsigned short maxy,
                                   int cursor) {
    int cursor_num = cursor & ~REMOTECURSOR_FLAG;
    if (cursor_num > PIX3CURSOR_NUM && cursor_num < MAX_MULTIMOUSE)
        return set_multimouse_cursor_pos(hp, x, y, maxx, maxy, cursor);
}

void hal_set_multimouse_cursor_status(HAL *hp, int status, int cursor) { 
    int cursor_num = cursor & ~REMOTECURSOR_FLAG;
    if (cursor_num > PIX3CURSOR_NUM && cursor_num < MAX_MULTIMOUSE)
        return set_multimouse_cursor_status(hp, status, cursor);
}
