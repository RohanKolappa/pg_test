#ifndef __DISPLAY_CONFIG_H_
#define __DISPLAY_CONFIG_H_

#ifndef __FPGA_H_INCLUDED__
#include "fpga.h"
#endif

#define DISPLAY_SOURCE_INVALID  0       
#define DISPLAY_SOURCE_REMOTE   1       
#define DISPLAY_SOURCE_OVERLAY  2       
#define DISPLAY_SOURCE_LOCAL    3       

#define SCALE_BY_DISPLAY        0       
#define SCALE_BY_SOURCE         1       
#define SCALE_BY_RESOLUTION     2       

#define POS_BY_LOCATION         0
#define POS_BY_OFFSET           1

#define HORIZ_LOCATION_LEFT     OV_LEFT_ADJUSTED     
#define HORIZ_LOCATION_RIGHT    OV_RIGHT_ADJUSTED     
#define HORIZ_LOCATION_CENTER   OV_CENTER_ADJUSTED     

#define VERT_LOCATION_TOP       OV_TOP_ADJUSTED     
#define VERT_LOCATION_BOTTOM    OV_BOTTOM_ADJUSTED     
#define VERT_LOCATION_MIDDLE    OV_CENTER_ADJUSTED     

#define display_source_from_code(x) \
    ((x == DISPLAY_PARM_VIDEO_SOURCE_LOCAL) ? DISPLAY_SOURCE_LOCAL : \
    ((x == DISPLAY_PARM_VIDEO_SOURCE_OVERLAY) ? DISPLAY_SOURCE_OVERLAY : \
     DISPLAY_SOURCE_REMOTE))

#define hscale_scheme_from_code(x) \
    ((x == DISPLAY_PARM_HORIZ_SCHEME_SCALE_BY_MONITOR) ? SCALE_BY_DISPLAY : \
    ((x == DISPLAY_PARM_HORIZ_SCHEME_SCALE_BY_SOURCE) ? SCALE_BY_SOURCE : \
     SCALE_BY_RESOLUTION))

#define vscale_scheme_from_code(x) \
    ((x == DISPLAY_PARM_VERT_SCHEME_SCALE_BY_MONITOR) ? SCALE_BY_DISPLAY : \
    ((x == DISPLAY_PARM_VERT_SCHEME_SCALE_BY_SOURCE) ? SCALE_BY_SOURCE : \
     SCALE_BY_RESOLUTION))

#define hpos_scheme_from_code(x) \
    ((x == DISPLAY_PARM_HORIZ_POSITION_LOCATION) ? POS_BY_LOCATION : \
     POS_BY_OFFSET)

#define vpos_scheme_from_code(x) \
    ((x == DISPLAY_PARM_VERT_POSITION_LOCATION) ? POS_BY_LOCATION : \
     POS_BY_OFFSET)

#define hlocation_from_code(x) \
    ((x == DISPLAY_PARM_HORIZ_LOCATION_LEFT) ? HORIZ_LOCATION_LEFT : \
    ((x == DISPLAY_PARM_HORIZ_LOCATION_RIGHT) ? HORIZ_LOCATION_RIGHT : \
     HORIZ_LOCATION_CENTER))

#define vlocation_from_code(x) \
    ((x == DISPLAY_PARM_VERT_LOCATION_TOP) ? VERT_LOCATION_TOP : \
    ((x == DISPLAY_PARM_VERT_LOCATION_BOTTOM) ? VERT_LOCATION_BOTTOM : \
     VERT_LOCATION_MIDDLE))

struct display_config {
    int  source;
    int  transparency;
    int  cropping;
    int  hscale_scheme;
    int  hscale;
    int  hres;
    int  vscale_scheme;
    int  vscale;
    int  vres;
    int  keep_aspect;
    int  hpos_scheme;
    int  vpos_scheme;
    int  hlocation;
    int  vlocation;
    int  hoffset;
    int  voffset;
    int  hcropleft;
    int  hcropright;
    int  vcroptop;
    int  vcropbottom;
};

#ifdef __cplusplus
extern "C" {
#endif
extern void get_display_config(HAL *hp, 
                               struct display_config *display_cfg);
#ifdef __cplusplus
}
#endif
#endif                          /* __DISPLAY_CONFIG_H_ */
