#ifndef __OVERRIDE_CONFIG_H_
#define __OVERRIDE_CONFIG_H_

#define OV_OPTION_INVALID       0
#define OV_OPTION_NONE          1
#define OV_OPTION_AUTO          2
#define OV_OPTION_MONITOR       3

#define OV_COLOR_BLACK          0x00
#define OV_COLOR_WHITE          0xff
#define OV_COLOR_GRAY           0x49
#define OV_COLOR_RED            0xe0
#define OV_COLOR_GREEN          0x1c
#define OV_COLOR_BLUE           0x03
#define OV_COLOR_YELLOW         0xfc
#define OV_COLOR_MAGENTA        0xd3
#define OV_COLOR_CYAN           0x1b

#define OV_POSITION_LOCATION    0
#define OV_POSITION_OFFSET      1

#define override_value_from_code(x) \
    ((x == OVERRIDE_PARM_OV_OPTION_AUTO) ? OV_OPTION_AUTO : \
     OV_OPTION_NONE)

#define xp_override_value_from_code(x) \
    ((x == DISPLAY_PARM_OV_OPTION_AUTO) ? OV_OPTION_AUTO : \
    ((x == DISPLAY_PARM_OV_OPTION_MONITOR) ? OV_OPTION_MONITOR : \
     OV_OPTION_NONE))

#define color_value_from_code(x) \
    ((x == DISPLAY_PARM_OV_COLOR_BLACK) ? OV_COLOR_BLACK : \
    ((x == DISPLAY_PARM_OV_COLOR_WHITE) ? OV_COLOR_WHITE : \
    ((x == DISPLAY_PARM_OV_COLOR_GRAY) ? OV_COLOR_GRAY : \
    ((x == DISPLAY_PARM_OV_COLOR_RED) ? OV_COLOR_RED : \
    ((x == DISPLAY_PARM_OV_COLOR_GREEN) ? OV_COLOR_GREEN : \
    ((x == DISPLAY_PARM_OV_COLOR_BLUE) ? OV_COLOR_BLUE : \
    ((x == DISPLAY_PARM_OV_COLOR_YELLOW) ? OV_COLOR_YELLOW : \
    ((x == DISPLAY_PARM_OV_COLOR_MAGENTA) ? OV_COLOR_MAGENTA : \
    ((x == DISPLAY_PARM_OV_COLOR_CYAN) ? OV_COLOR_CYAN : \
     OV_COLOR_BLACK)))))))))

#define color_code_from_name(x)  \
    (!strcasecmp(x, "Black") ? OV_COLOR_BLACK : \
    (!strcasecmp(x, "White") ? OV_COLOR_WHITE : \
    (!strcasecmp(x, "Gray") ? OV_COLOR_GRAY : \
    (!strcasecmp(x, "Red") ? OV_COLOR_RED : \
    (!strcasecmp(x, "Green") ? OV_COLOR_GREEN : \
    (!strcasecmp(x, "Blue") ? OV_COLOR_BLUE : \
    (!strcasecmp(x, "Yellow") ? OV_COLOR_YELLOW : \
    (!strcasecmp(x, "Magenta") ? OV_COLOR_MAGENTA : \
    (!strcasecmp(x, "Cyan") ? OV_COLOR_CYAN : \
     OV_COLOR_BLACK)))))))))

struct override_config {
    int ov_option;
    int ov_color;
    int ov_keepaspect;
    int hscale_scheme;
    int vscale_scheme;
    int hscale;
    int hres;
    int vscale;
    int vres;
    int hpos_scheme;
    int vpos_scheme;
    int hlocation;
    int vlocation;
    int hoffset;
    int voffset;
};

#ifdef __cplusplus
extern "C" {
#endif
extern int get_override_config(HAL *hp, 
                              struct override_config *override_cfg);
extern int xp_get_override_config(HAL *hp, 
                              struct override_config *override_cfg);
#ifdef __cplusplus
}
#endif
#endif                          /* __DISPLAY_CONFIG_H_ */
