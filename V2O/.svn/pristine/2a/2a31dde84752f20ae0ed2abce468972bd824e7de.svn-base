#ifndef __OVERRIDELIST_LIB_H__
#define __OVERRIDELIST_LIB_H__

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#ifndef __OVERRIDELIST_MENU_GEN_H_
#include "override_list_menu.h"
#endif

#ifndef __OVERRIDE_CONFIG_H_
#include "override_config.h"
#endif

/***********************
 * Function prototpyes *
 ***********************/
int get_override_list_params_by_lutid(struct override_list_params *params,
                                      int lutid);
int get_override_lut_and_scale(HAL *hp,
                               struct LUT_struct *lutp,
                               struct LUT_struct *overridelutp,
                               struct override_config *overridecfg,
                               int *horiz_scale,
                               int *vert_scale);
int set_overlay_params_for_override(HAL *hp, 
                                    struct override_config *overridecfg);
int get_override_display_lut(HAL *hp,
                             struct LUT_struct *lutp,
                             struct LUT_struct *olutp,
                             struct override_config *ocfg);
int check_lut_edid_match(struct LUT_struct *lutp,
                         unsigned char *edidbuf,
                         int edidbufsize,
                         int debug);

#endif          /* __OVERRIDELIST_LIB_H__ */
