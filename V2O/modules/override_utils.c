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
#include    "vlib.h"
#include    "hal_edid.h"
#include    "vlog.h"
#include    "commonutils.h"
#include    "lut_utils.h"
#include    "display_config.h"
#include    "override_config.h"
#include    "override_list_menu.h"
#include    "override_utils.h"
#include    "edid_list_menu.h"
#include    "edid_utils.h"

#define     REFRESH_DELTA   (double) 0.2
#define     PCR_DELTA       (double) 0.2

/* EDID v1.3 Established Timings I & II */
static struct LUT_struct stdLUT1[] = {

 { "STD 800x600@60Hz", FALSE, 800, 600, 60.32, (unsigned long) 0, 
    40, 128, 88, 1, 4, 23, 40.0, (unsigned long) 0, 800, 600, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 800x600@56Hz", FALSE, 800, 600, 56.25, (unsigned long) 0,
    24, 72, 128, 1, 2, 22, 36.0, (unsigned long) 0, 800, 600, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 640x480@75Hz", FALSE, 640, 480, 75.0, (unsigned long) 0,
    16, 64, 120, 1, 3, 16, 37.5, (unsigned long) 0, 640, 480, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 640x480@72Hz", FALSE, 640, 480, 72.81, (unsigned long) 0,
    24, 40, 120, 9, 3, 28, 31.5, (unsigned long) 0, 640, 480, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 640x480@67Hz", FALSE, 640, 480, (double) 0, (unsigned long) 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 640, 480, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 640x480@60Hz", FALSE, 640, 480, 59.94, (unsigned long) 0,
    16, 96, 48, 10, 10, 25, 25.175, (unsigned long) 0, 640, 480, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 720x400@88Hz", FALSE, 720, 400, (double) 0, (unsigned long) 0,
     0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 720, 400, 0, 
     0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 720x400@70Hz", FALSE, 720, 400, (double) 0, (unsigned long) 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 720, 400, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1280x1024@75Hz", FALSE, 1280, 1024, 75.03, (double) 0,
    16, 144, 248, 1, 3, 38, 135.0, (unsigned long) 0, 1280, 1024, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1024x768@75Hz", FALSE, 1024, 768, 75.03, (unsigned long) 0,
    16, 96, 176, 1, 3, 28, 78.75, (unsigned long) 0, 1024, 768, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1024x768@70Hz", FALSE, 1024, 768, 70.07, (unsigned long) 0,
    24, 136, 144, 3, 6, 29, 75.0, (unsigned long) 0, 1024, 768, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1024x768@60Hz", FALSE, 1024, 768, 60.0, (unsigned long) 0,
    24, 136, 160, 3, 6, 29, 65.0, (unsigned long) 0, 1024, 768, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1024x768@87Hz(Interleaved)", FALSE, 1024, 768, 0.0, (unsigned long) 0, 
    0, 0, 0, 0, 0, 0, 0.0, (unsigned long) 0, 1024, 768, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0},

 { "STD 832x624@75Hz", FALSE, 832, 624, 0.0, (unsigned long) 0, 
    0, 0, 0, 0, 0, 0, 0.0, (unsigned long) 0, 832, 624, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 800x600@75Hz", FALSE, 800, 600, 75.0, (unsigned long) 0, 
    16, 80, 160, 1, 3, 21, 49.5, (unsigned long) 0, 800, 600, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 800x600@72Hz", FALSE, 800, 600, 72.19, (unsigned long) 0,
    56, 120, 64, 37, 6, 23, 50.0, (unsigned long) 0, 800, 600, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},
};

/* DMT Timings for EDID v1.3 Standard Timing */
static struct LUT_struct stdLUT2[] = {

 { "STD 1280x720@60Hz", FALSE, 1280, 720, 60.0, (unsigned long) 0,
    112, 208, 48, 5, 5, 20, 74.25, (unsigned long) 0, 1280, 720, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},
 
 { "STD 1280x800@60Hz", FALSE, 1280, 800, 59.91, (unsigned long) 0,
    48, 80, 32, 3, 14, 6, 71.0, (unsigned long) 0, 1280, 800, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0,  
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1280x960@60Hz", FALSE, 1280, 960, 60.0, (unsigned long) 0,
    96, 112, 312, 1, 3, 36, 108.0, (unsigned long) 0, 1280, 960, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1280x1024@60Hz", FALSE, 1280, 1024, 60.02, (unsigned long) 0,
    48, 112, 248, 1, 3, 38, 108.0, (unsigned long) 0, 1280, 1024, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1600x1200@60Hz", FALSE, 1600, 1200, 60.0, (unsigned long) 0,
    64, 192, 304, 1, 3, 46, 162.0, (unsigned long) 0, 1600, 1200, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1600x1200@75Hz", FALSE, 1600, 1200, 75.0, (unsigned long) 0,
    64, 192, 304, 1, 3, 46, 202.5, (unsigned long) 0, 1600, 1200, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1600x1200@85Hz", FALSE, 1600, 1200, 85.0, (unsigned long) 0,
    64, 192, 304, 1, 3, 46, 229.5, (unsigned long) 0, 1600, 1200, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1680x1050@60Hz", FALSE, 1680, 1050, 59.95, (unsigned long) 0,
    104, 176, 280, 3, 6, 30, 146.25, (unsigned long) 0, 1680, 1050, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1680x1050@75Hz", FALSE, 1680, 1050, 74.89, (unsigned long) 0,
    120, 176, 296, 3, 6, 40, 187.0, (unsigned long) 0, 1680, 1050, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1680x1050@85Hz", FALSE, 1680, 1050, 84.94, (unsigned long) 0,
    128, 176, 304, 3, 6, 46, 214.75, (unsigned long) 0, 1680, 1050, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1792x1344@60Hz", FALSE, 1792, 1344, 60.0, (unsigned long) 0,
    128, 200, 328, 1, 3, 46, 204.75, (unsigned long) 0, 1792, 1344, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1852x1392@60Hz", FALSE, 1852, 1392, 59.995, (unsigned long) 0,
    96, 224, 352, 1, 3, 43, 218.25, (unsigned long) 0, 1852, 1392, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0, 
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1920x1080@60Hz", FALSE, 1920, 1080, 60.0, (unsigned long) 0,
    88, 144, 48, 4, 36, 5, 148.5, (unsigned long) 0, 1920, 1080, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1920x1200@60Hz", FALSE, 1920, 1200, 59.95, (unsigned long) 0,
    48, 80, 32, 3, 26, 6, 154.0, (unsigned long) 0, 1920, 1200, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},

 { "STD 1920x1440@60Hz", FALSE, 1920, 1440, 60.0, (unsigned long) 0,
    128, 208, 344, 1, 3, 56, 234.0, (unsigned long) 0, 1920, 1440, 0, 
    0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0, 0, 0, 0, 0, 0, 0xff, 
    0, 0, (double) 0, (unsigned long) 0, (double) 0, (unsigned long) 0,
    0xfffe, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 
    0x69, 0xd0, 0x98, 0x46, 0xfe,
    0, 0x400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, (double) 0, (unsigned long) 0, 0, 0},
};

/********************************************************************
 * Returns Override list params for entry with LUT id 'lutid'. Must *
 * initialize Override list params table before calling this routine*
 ********************************************************************/
int get_override_list_params_by_lutid(struct override_list_params *params,
                                      int lutid) {
    struct override_list_params *paramsp;
    int found = 0;

    foreach_override_list_entry(paramsp) {
        if (paramsp->alutreg_value == lutid) {
            memcpy(params, paramsp, sizeof(struct override_list_params));
            found = 1;
            break;
        }
    }
    return found;
}

/***********************************************************************
 * Find and return override list params matching given hres, vres etc. *
 ***********************************************************************/
static int override_list_params_match(struct override_list_params *ovparamsp, 
                                        unsigned short orig_hres,
                                      unsigned short orig_vres,
                                      unsigned short stereo,
                                      unsigned short lutidx,
                                      double refresh,
                                      double pcr) 

{
   if ((ovparamsp->ov_alutreg_value == OVERRIDE_LIST_OV_ALUTREG_ANY || 
        ovparamsp->ov_alutreg_value == lutidx) && 
        (ovparamsp->ov_hres_value == OVERRIDE_LIST_OV_HRES_ANY || 
         ovparamsp->ov_hres_value == orig_hres) && 
        (ovparamsp->ov_vres_value == OVERRIDE_LIST_OV_VRES_ANY || 
         ovparamsp->ov_vres_value == orig_vres) && 
        (ovparamsp->ov_stereo_value == OVERRIDE_LIST_OV_STEREO_ANY || 
         ovparamsp->ov_stereo_value == stereo) && 
        (ovparamsp->ov_refresh_value < 0 || 
         (ovparamsp->ov_refresh_value >= (refresh - REFRESH_DELTA) && 
          ovparamsp->ov_refresh_value <= (refresh + REFRESH_DELTA))) && 
        (ovparamsp->ov_pcr_value < 0 || 
         (ovparamsp->ov_pcr_value >= (pcr - REFRESH_DELTA) && 
          ovparamsp->ov_pcr_value <= (pcr + REFRESH_DELTA)))) {
        return 1;
    }
    return 0;
}

/*****************************************************************
 * Return the (fixratio) highest scaling factor and the override *
 * LUT 'overridelutp' the function get_override_lut() finds for  *
 * the LUT 'lutp' on channel 'hp'.                               *
 *****************************************************************/
static int get_scaled_override_lut(HAL *hp,
    struct LUT_struct *lutp,
    struct LUT_struct *overridelutp,
    int keepaspect,
    int *hscale,
    int *vscale,
    int (*get_override_lut)(HAL *, struct LUT_struct *, struct LUT_struct *)) {                                                                                
    int i, j, rc;
    struct LUT_struct tmplut;

    if (lutp == NULL || overridelutp == NULL)
        return -1;

    memcpy(overridelutp, lutp, sizeof (struct LUT_struct));

    memcpy(&tmplut, lutp, sizeof (struct LUT_struct));

    if (keepaspect) {
        for (i = (*hscale > *vscale) ? *vscale : *hscale; i >= 2; i--) {
            tmplut.a_orig_hres = lutp->a_orig_hres * i / 8;
            tmplut.a_orig_vres = lutp->a_orig_vres * i / 8;
            rc = (*get_override_lut)(hp, &tmplut, overridelutp);
            if (rc == 0) {
                *hscale = *vscale = i;
                return 0;
            }
            else if (rc == -2) {
                return -2;
            }
        }
    }
    else {
        for (i = *hscale; i >= 2; i--) {
            for (j = *vscale; j >= 2; j--) {
                tmplut.a_orig_hres = lutp->a_orig_hres * i / 8;
                tmplut.a_orig_vres = lutp->a_orig_vres * j / 8;
                rc = (*get_override_lut)(hp, &tmplut, overridelutp);
                if (rc == 0) {
                    *hscale = i;
                    *vscale = j;
                    return 0;
                }
                else if (rc == -2) {
                    return -2;
                }
            }
        }
    }
    return -1;
}

/*****************************************************************
 * Find and return in 'overridelutp' from the list of override   *
 * LUTs for Board 'hp' the one 'lutp' with the same hres and     *
 * a bigger or equal 'vres'.                                     *
 *****************************************************************/
static int get_i50_auto_override_lut(HAL *hp, 
                                     struct LUT_struct *lutp, 
                                     struct LUT_struct *overridelutp) {
    struct LUT_struct lut;
    struct override_list_params *paramsp;
    int rc = -1, firsttime = 1, tmphres = 0, tmpvres = 0, hres, vres, count;

    /* Read override list from config file */
    if ((count = retrieveOverrideListValues(hp)) <= 0) {
        return rc;
    }

    /* Find entry in override list that fits 'lutp' the best. Note */
    /* this is for decoding so we use h/vres not h/v_orig_res.     */
    hres = lutp->a_hres;
    vres = lutp->a_vres;

    foreach_override_list_entry(paramsp) {
        if (override_list_params_match(paramsp, hres, vres, lutp->a_stereo, 
                    lutp->a_alutreg, lutp->a_refresh, lutp->a_pcr) == 0) {
            continue;
        }

        if (get_lut_entry_by_lutid(hp, &lut, paramsp->alutreg_value) == 0) {
            continue;
        }

        build_lut_entry(&lut, V2DI50_DEVICE);

        if ((hres <= lut.a_hres) && vres <= lut.a_vres) {
            if (firsttime) {
                firsttime = 0;
                memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                tmphres = lut.a_vres;
                tmpvres = lut.a_vres;
                rc = 0;
            }
            else {
                if (lut.a_hres < tmphres || lut.a_vres < tmpvres) {
                    memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                    tmphres = lut.a_hres;
                    tmpvres = lut.a_vres;
                }
            }
        }
    }

    return rc;
}

/******************************************************************
 * Find and return in 'overridelutp' from the list of override    *
 * LUTs for Board 'hp' the one 'lutp' will fit the best (i.e.     *
 * differs minimally in area covered by active pixels.) Returns 0 *
 * if found matching override LUT, -1 if no matching override LUT *
 ******************************************************************/
static int get_unscaled_auto_override_lut(HAL *hp, 
                                          struct LUT_struct *lutp, 
                                          struct LUT_struct *overridelutp) {
    struct LUT_struct lut;
    struct override_list_params *paramsp;
    int rc = -1, firsttime = 1, area = 0, tmparea, hres, vres, count;

    /* Read override list from config file */
    if ((count = retrieveOverrideListValues(hp)) <= 0) {
        return rc;
    }

    /* Find entry in override list that fits 'lutp' the best */
    hres = lutp->a_orig_hres;
    vres = lutp->a_orig_vres;

    foreach_override_list_entry(paramsp) {

        if (override_list_params_match(paramsp, hres, vres, lutp->a_stereo, 
                    lutp->a_alutreg, lutp->a_refresh, lutp->a_pcr) == 0) {
            continue;
        }

        if (get_lut_entry_by_lutid(hp, &lut, paramsp->alutreg_value) == 0) {
            continue;
        }

        if ((hres <= lut.a_orig_hres) && vres <= lut.a_orig_vres) {
            if (firsttime) {
                firsttime = 0;
                memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                area = lut.a_orig_hres * lut.a_orig_vres;
                rc = 0;
            }
            else {
                tmparea = lut.a_orig_hres * lut.a_orig_vres;
                if (area > tmparea) {
                    memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                    area = tmparea;
                }
            }
        }
    }

    return rc;
}

static int get_scaled_auto_override_lut(HAL *hp,
                                        struct LUT_struct *lutp,
                                        struct LUT_struct *overridelutp,
                                        int keepaspect,
                                        int *hscale,
                                        int *vscale) {
    return get_scaled_override_lut(hp, lutp, overridelutp, keepaspect,
                            hscale, vscale, get_unscaled_auto_override_lut);
}

static int get_reserved_timing(unsigned char c1, unsigned char c2, 
                     int *hres, int *vres, double *refresh) {
    unsigned short horiz, vert;
    double asprat;
                                                                                
    if (c1 == 0x01 && c2 == 0x01) return -1;
                                                                                
    horiz = c1 * 8 + 248;
    asprat = ((c2 & 0xc0) == 0x00) ? ((double) 10 / (double) 16) :
             (((c2 & 0xc0) == 0x40) ? ((double) 3 / (double) 4) :
             (((c2 & 0xc0) == 0x80) ? ((double) 4 / (double) 5) :
             (double) 9 / (double) 16));
    vert =  asprat * (double) horiz;

    *hres = horiz;
    *vres = vert;
    *refresh = (double) ((c2 & 0x3f) + 60);

    return 0;
}

/******************************************************************
 * Find and return in 'overridelutp' from the list of LUTs in the *
 * monitor EDID for Board 'hp' the one matching the stereo, HRES, *
 * VRES, refresh rate and pixel clock of the LUT pointed to by    *
 * 'lutp'. Returns -2 if unable to read monitor EDID, 0 if found  * 
 * matching override LUT, -1 if no matching override LUT.         *
 ******************************************************************/
#define MON_NAME_SIZE           8

static int get_unscaled_mon_override_lut(HAL *hp,
                                         struct LUT_struct *lutp,
                                         struct LUT_struct *overridelutp) {
    int rc = -1, firsttime = 1, area = 0, tmparea = 0, i, j, tmphres, tmpvres;
    unsigned char buf[EDID_DATA_LEN];
    unsigned short stdsupport, CEAblocks;
    double tmprfrsh;
    struct LUT_struct lut;
    char monname[MON_NAME_SIZE + 1];

    if (lutp == NULL || overridelutp == NULL)
        return -1;

    memcpy(overridelutp, lutp, sizeof(struct LUT_struct));

    if (hal_read_mon_edid(hp, buf, EDID_DATA_LEN) < 0) {
        return -2;
    }

    /* Check timing dscriptors */
    for (i = 1; i < 5; i++) {
        memset(&lut, 0, sizeof(lut));
        if (get_lut_from_edid_descriptor(buf, i, &lut) <= 0)     
            continue;
        if ((lutp->a_orig_hres <= lut.a_orig_hres) && 
                (lutp->a_orig_vres <= lut.a_orig_vres)) {
            if (firsttime) {
                firsttime = 0;
                memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                area = lut.a_orig_hres * lut.a_orig_vres;
                rc = 0;
            }
            else {
                tmparea = lut.a_orig_hres * lut.a_orig_vres;
                if (area > tmparea) {
                    memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                    area = tmparea;
                }
            }
        } 
    }

    /* Get monitor name */
    get_edid_product_id(buf, monname);

    /* Check CEA timing */
    CEAblocks = buf[126];

    if (CEAblocks) {
        for (i = 0; i < CEAblocks; i++) {
            if (hal_read_mon_edid_extension(hp, i+1, buf, EDID_DATA_LEN) < 0)
                return -1; 
            for (j = 0; j < (EDID_DATA_LEN - 17) / 18; j++) {
                if (get_lut_from_edid_descriptor_extension(buf, j, monname,
                            &lut) <= 0) 
                    continue;
                if ((lutp->a_orig_hres <= lut.a_orig_hres) && 
                          (lutp->a_orig_vres <= lut.a_orig_vres)) {
                    if (firsttime) {
                        firsttime = 0;
                        memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                        area = lut.a_orig_hres * lut.a_orig_vres;
                        rc = 0;
                    }
                    else {
                        tmparea = lut.a_orig_hres * lut.a_orig_vres;
                        if (area > tmparea) {
                            memcpy(overridelutp, &lut, sizeof(struct LUT_struct));
                            area = tmparea;
                        }
                    }
                }
            } 
        }
    }

    /* Check manufacturer's reserved timing */
    for (j = 38; j < 54; j += 2) {
        if (get_reserved_timing(buf[j], buf[j+1], &tmphres, &tmpvres, 
                    &tmprfrsh) < 0 || 
                    lutp->a_orig_hres > tmphres || 
                    lutp->a_orig_vres > tmpvres) {
            continue; 
        }

        for (i = 0; i < sizeof(stdLUT2) / sizeof(struct LUT_struct); i++) {
            if ((tmphres == stdLUT2[i].a_orig_hres) && 
                    (tmpvres == stdLUT2[i].a_orig_vres) && 
                    (tmprfrsh < stdLUT2[i].a_refresh + 0.2) &&
                    (tmprfrsh > stdLUT2[i].a_refresh - 0.2)) {
                if (firsttime) {
                    firsttime = 0;
                    memcpy(overridelutp, &stdLUT2[i], 
                            sizeof(struct LUT_struct));
                    area = stdLUT2[i].a_orig_hres * stdLUT2[i].a_orig_vres;
                    rc = 0;
                }
                else {
                    tmparea = stdLUT2[i].a_orig_hres * stdLUT2[i].a_orig_vres;
                    if (area > tmparea) {
                        memcpy(overridelutp, &stdLUT2[i], 
                                sizeof(struct LUT_struct));
                        area = tmparea;
                    }
                }
            }
        }
    }

    /* Check standard timing */
    stdsupport = ((unsigned short) buf[36] << 8) | (unsigned short) buf[35];

    for (i = 0; i < 16; i++) {
        /* Skip the LUTs with 0 PCR since we don't have timing info  */
        /* for them, also skip the LUTs not supported by the monitor */
        if (stdLUT1[i].a_pcr < (double) 1.0 || !(stdsupport & (1 << i))) 
            continue;

        if ((lutp->a_orig_hres <= stdLUT1[i].a_orig_hres) && 
            (lutp->a_orig_vres <= stdLUT1[i].a_orig_vres)) {
            if (firsttime) {
                firsttime = 0;
                memcpy(overridelutp, &stdLUT1[i], sizeof(struct LUT_struct));
                area = stdLUT1[i].a_orig_hres * stdLUT1[i].a_orig_vres;
                rc = 0;
            }
            else {
                tmparea = stdLUT1[i].a_orig_hres * stdLUT1[i].a_orig_vres;
                if (area > tmparea) {
                     memcpy(overridelutp, &stdLUT1[i], 
                             sizeof(struct LUT_struct));
                     area = tmparea;
                 }
            }
        } 
    }

    /* Set LUT ID to the reserved override LUT ID */
    if (rc == 0) {
        overridelutp->a_alutreg = RESERVED_OVERRIDE_LUTID;
        sprintf(overridelutp->comment, "%s %dx%d@%d", monname, 
                overridelutp->a_orig_hres, overridelutp->a_orig_vres, 
                (int) (overridelutp->a_refresh + 0.5));
    }
    return rc;
}

static int get_scaled_mon_override_lut(HAL *hp,
                                       struct LUT_struct *lutp,
                                       struct LUT_struct *overridelutp,
                                       int keepaspect,
                                       int *hscale,
                                       int *vscale) {
    return get_scaled_override_lut(hp, lutp, overridelutp, keepaspect,
                            hscale, vscale, get_unscaled_mon_override_lut);
}

/***********************************************************************
 * This routine returns either the LUT given in 'r_lutp' or the LUT    *
 * that should be used for override in 'd_lutp' and sets the scale     *
 * values 'horiz_scale' and 'vert_scale' that should be used with it.  *
 ***********************************************************************/
int get_override_lut_and_scale(HAL *hp, 
                               struct LUT_struct *r_lutp,
                               struct LUT_struct *d_lutp,
                               struct override_config *o_cfg,
                               int *horiz_scale,
                               int *vert_scale) {
    struct LUT_struct newlut;
    int option, keepaspect, hs, vs, rc = -1;
    int device = hal_get_hardware_type(hp);

    option = o_cfg->ov_option;
    keepaspect = o_cfg->ov_keepaspect;
    hs = vs = RX_MAX_SCALE_VAL;

    memcpy(d_lutp, r_lutp, sizeof(struct LUT_struct));

    if (option == OV_OPTION_NONE)
        return 0;

    if (option != OV_OPTION_AUTO && option != OV_OPTION_MONITOR) 
        return -1;

    if (option == OV_OPTION_AUTO && device == V2DI50_DEVICE &&
        get_i50_auto_override_lut(hp, r_lutp, &newlut) == 0) {

        int extra_pixels = newlut.a_hres - r_lutp->a_hres;
        int extra_lines = newlut.a_vres - r_lutp->a_vres;

        /**************************************
         * Compute rest of override LUT entry *
         **************************************/
        build_lut_entry(&newlut, V2DI50_DEVICE);

        /************************************************
         * Force override LUT to be same size as remote *
         ************************************************/
        newlut.a_hfp += extra_pixels / 2;
        newlut.a_hbp += extra_pixels - extra_pixels / 2;
        newlut.a_orig_hres = r_lutp->a_orig_hres;
        newlut.a_vfp += extra_lines / 2;
        newlut.a_vbp += extra_lines - extra_lines / 2;
        newlut.a_orig_vres = r_lutp->a_orig_vres;
        newlut.a_hres = r_lutp->a_hres;
        newlut.a_vres = r_lutp->a_vres;
        newlut.d_dvhfpreg = newlut.a_hfp;
        newlut.d_dvhbpreg = newlut.a_hbp;
        newlut.d_dvhareg = newlut.a_hres;
        newlut.d_dvvfpreg = newlut.a_vfp;
        newlut.d_dvvbpreg = newlut.a_vbp;
        newlut.d_dvvareg = newlut.a_vres;
        memcpy(d_lutp, &newlut, sizeof(struct LUT_struct));
        rc = 0;
    } 
    else if (device != V2DI50_DEVICE && ((option == OV_OPTION_AUTO && 
        get_scaled_auto_override_lut(hp, r_lutp, &newlut, keepaspect, 
            &hs, &vs) == 0) ||
        (option == OV_OPTION_MONITOR && 
        get_scaled_mon_override_lut(hp, r_lutp, &newlut, keepaspect,
            &hs, &vs) == 0))) {

        /**************************************
         * Compute rest of override LUT entry *
         **************************************/
        build_lut_entry(&newlut, PLX_DEVICE);

        /**********************
         * Set override LUT   *
         **********************/
        memcpy(d_lutp, &newlut, sizeof(struct LUT_struct));
        rc = 0;
    }

    /**************************
     * Set new override scale *
     **************************/
    *horiz_scale = hs;
    *vert_scale = vs;

    return rc;
}

/***********************************************************************
 * This routine sets the overlay parameters based on the override      *
 * configuration 'ocfg'.
 ***********************************************************************/
int set_overlay_params_for_override(HAL *hp, struct override_config *ocfg)
{
    struct ov_param_io ovpio;

    /* Do nothing if not overriding */
    if (ocfg->ov_option == OV_OPTION_NONE)
        return 0;

    /* Change overlay settings in driver for override */
    memset(&ovpio, 0, sizeof(struct ov_param_io));
    ovpio.flags = OV_FLAG_SRC_REMOTE;
    if (ocfg->hscale_scheme == SCALE_BY_DISPLAY)
        ovpio.flags |= OV_FLAG_HSCALEDISPLAY;
    if (ocfg->hscale_scheme == SCALE_BY_SOURCE)
        ovpio.flags |= OV_FLAG_HSCALESRC;
    else
        ovpio.flags |= OV_FLAG_VRES;
    if (ocfg->vscale_scheme == SCALE_BY_DISPLAY)
        ovpio.flags |= OV_FLAG_VSCALEDISPLAY;
    if (ocfg->vscale_scheme == SCALE_BY_SOURCE)
        ovpio.flags |= OV_FLAG_VSCALESRC;
    else
        ovpio.flags |= OV_FLAG_VRES;
    if (ocfg->hpos_scheme == OV_POSITION_LOCATION)
        ovpio.flags |= OV_FLAG_HPOSLOCATION;
    else
        ovpio.flags |= OV_FLAG_HPOSOFFSET;
    if (ocfg->vpos_scheme == OV_POSITION_LOCATION)
        ovpio.flags |= OV_FLAG_VPOSLOCATION;
    else
        ovpio.flags |= OV_FLAG_VPOSOFFSET;
    if (ocfg->ov_keepaspect)
        ovpio.flags |= OV_FLAG_KEEPASPECT;
    ovpio.hscale = (ocfg->hscale * RX_MAX_SCALE_VAL) / 100;
    ovpio.vscale = (ocfg->vscale * RX_MAX_SCALE_VAL) / 100;
    if (ovpio.hscale < RX_MIN_SCALE_VAL)
        ovpio.hscale = RX_MIN_SCALE_VAL;
    if (ovpio.vscale < RX_MIN_SCALE_VAL)
        ovpio.vscale = RX_MIN_SCALE_VAL;
    ovpio.hres = ocfg->hres;
    ovpio.vres = ocfg->vres;
    ovpio.hlocation = ocfg->hlocation;
    ovpio.vlocation = ocfg->vlocation;
    ovpio.hoffset = ocfg->hoffset;
    ovpio.voffset = ocfg->voffset;
    ovpio.setmask = OV_SET_PARAM_OVERLAY;

    if (hal_set_overlay_params(hp, &ovpio) < 0)
        return -1;

    return 0;
}

/***********************************************************************
 * This routine returns the LUT that should be used to override the    *
 * LUT pointed to by 'lutp' when the override configuration is 'ocfg'  *
 ***********************************************************************/
int get_override_display_lut(HAL *hp,
                             struct LUT_struct *lutp,
                             struct LUT_struct *olutp,
                             struct override_config *ocfg)
{
    int hs, vs;

    memcpy(olutp, lutp, sizeof(struct LUT_struct));

    if (get_override_lut_and_scale(hp, lutp, olutp, ocfg, &hs, &vs) < 0) 
        return -1;

    if (ocfg->ov_option == OV_OPTION_NONE)
        olutp->d_ov_control = ocfg->ov_color;
    else
        olutp->d_ov_control = DOVCONTROL_ENABLE | ocfg->ov_color;

    return 0;
}

/*********************************************************************
 * This routine returns 1 if 'lutp' matches one of the descriptors   *
 * in the EDID buffer 'edidbuf'                                      *
 *********************************************************************/
int check_lut_edid_match(struct LUT_struct *lutp, 
                         unsigned char *edidbuf,
                         int edidbufsize,
                         int debug) 
{
    int i, j, CEAblocks, stdsupport, tmphres, tmpvres;
    double tmprfrsh;
    struct LUT_struct lut;
    char monname[MON_NAME_SIZE + 1];

    if (lutp == NULL || edidbuf == NULL)
        return 0;

    if (debug)
        VLOG_INFO("%s: entered. LUT H %d/%d/%d/%d V %d/%d/%d/%d R %f P %f",
                __func__, lutp->a_orig_hres, lutp->a_hfp, lutp->a_hsync,
                lutp->a_hbp, lutp->a_orig_vres, lutp->a_vfp, lutp->a_vsync,
                lutp->a_vbp, lutp->a_refresh, lutp->a_pcr);

    /* check timing descriptor */
    for (i = 1; i < 5; i++) {
        if (get_lut_from_edid_descriptor(edidbuf, i, &lut) <= 0)
            continue;
        if (compare_luts_without_polarity(lutp, &lut) == 0) {
            if (debug)
                VLOG_INFO("%s: Match Desc %d LUT H %d/%d/%d/%d V %d/%d/%d/%d"
                    " R %f P %f", __func__, i, lut.a_orig_hres, lut.a_hfp, 
                    lut.a_hsync, lut.a_hbp, lut.a_orig_vres, lut.a_vfp, 
                    lut.a_vsync, lut.a_vbp, lut.a_refresh, lut.a_pcr);
            return 1;
        }
        else if (debug)
            VLOG_INFO("%s: No Match Desc %d LUT H %d/%d/%d/%d V %d/%d/%d/%d"
                " R %f P %f", __func__, i, lut.a_orig_hres, lut.a_hfp, 
                lut.a_hsync, lut.a_hbp, lut.a_orig_vres, lut.a_vfp, 
                lut.a_vsync, lut.a_vbp, lut.a_refresh, lut.a_pcr);
    }

    /* get monitor name */
    get_edid_product_id(edidbuf, monname);

    /* check CEA timing */
    CEAblocks = edidbuf[126];
    if (CEAblocks && edidbufsize > 128) {
        for (i = 0; i < CEAblocks; i++) {
            for (j = 0; j < (EDID_DATA_LEN - 17) / 18; j++) {
                if (get_lut_from_edid_descriptor_extension(edidbuf, j, 
                            monname, &lut) <= 0)
                    continue;
            }
            if (compare_luts_without_polarity(lutp, &lut) == 0) {
                if (debug)
                    VLOG_INFO("%s: Match XDesc %d LUT H %d/%d/%d/%d "
                        "V %d/%d/%d/%d R %f P %f", __func__, j, 
                        lut.a_orig_hres, lut.a_hfp, lut.a_hsync, lut.a_hbp, 
                        lut.a_orig_vres, lut.a_vfp, lut.a_vsync, lut.a_vbp, 
                        lut.a_refresh, lut.a_pcr);
                return 1;
            }
            else if (debug)
                VLOG_INFO("%s: No Match XDesc %d LUT H %d/%d/%d/%d "
                    "V %d/%d/%d/%d R %f P %f", __func__, j, 
                    lut.a_orig_hres, lut.a_hfp, lut.a_hsync, lut.a_hbp, 
                    lut.a_orig_vres, lut.a_vfp, lut.a_vsync, lut.a_vbp, 
                    lut.a_refresh, lut.a_pcr);
        }
    } 

    /* check manufacturer's reserverd timing */
    for (i = 38; i < 54; i += 2) {
        if (get_reserved_timing(edidbuf[i], edidbuf[i+1], &tmphres, &tmpvres, 
                    &tmprfrsh) < 0)
            continue;
        if ((tmphres == lutp->a_orig_hres) && (tmpvres == lutp->a_orig_vres) &&
                (tmprfrsh < lutp->a_refresh + 0.2) && 
                (tmprfrsh > lutp->a_refresh - 0.2)) {
            if (debug)
                VLOG_INFO("%s: Match Res Timing %d H %d V %d R %f", __func__,
                        i, tmphres, tmpvres, tmprfrsh);
            return 1;
        }
        else if (debug)
            VLOG_INFO("%s: No Match Res Timing %d H %d V %d R %f", __func__,
                        i, tmphres, tmpvres, tmprfrsh);
    }

    /* check standard timing */
    stdsupport = ((unsigned short) edidbuf[36] << 8) | 
        (unsigned short) edidbuf[35];
    for (i = 0; i < 16; i++) {
        /* skip the LUTs with 0 PCR since we don't have timing info  */
        /* for them, also skip the LUTs not supported in the edidbuf */
        if (stdLUT1[i].a_pcr < (double) 1.0 || !(stdsupport & (1 << i)))
            continue;
        if (compare_luts_without_polarity(lutp, &stdLUT1[i]) == 0) {
            if (debug)
                VLOG_INFO("%s: Match Std Timing %d H %d/%d/%d/%d "
                        "V %d/%d/%d/%d R %f P %f", __func__, i, 
                        stdLUT1[i].a_orig_hres, stdLUT1[i].a_hfp, 
                        stdLUT1[i].a_hsync, stdLUT1[i].a_hbp,
                        stdLUT1[i].a_orig_vres, stdLUT1[i].a_vfp,
                        stdLUT1[i].a_vsync, stdLUT1[i].a_vbp,
                        stdLUT1[i].a_refresh, stdLUT1[i].a_pcr);
            return 1;
        }
        else if (debug)
            VLOG_INFO("%s: No Match Std Timing %d H %d/%d/%d/%d "
                    "V %d/%d/%d/%d R %f P %f", __func__, i, 
                    stdLUT1[i].a_orig_hres, stdLUT1[i].a_hfp, 
                    stdLUT1[i].a_hsync, stdLUT1[i].a_hbp,
                    stdLUT1[i].a_orig_vres, stdLUT1[i].a_vfp,
                    stdLUT1[i].a_vsync, stdLUT1[i].a_vbp,
                    stdLUT1[i].a_refresh, stdLUT1[i].a_pcr);
    }
    return 0;
}

