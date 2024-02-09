#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "display_params_menu_gen.h"
#include "display_config.h"
#include "override_config.h"
#include "strmsrv_comm.h"

void get_display_config(HAL *hp, struct display_config *display_cfg) {
    struct display_parm_params params;

    memset(display_cfg, 0, sizeof(struct display_config));

    params.userptr = hp;

    if (hal_get_hardware_type(hp) != V2DI50_DEVICE) {
        retrieveDisplayParametersValues(&params);
        display_cfg->source = 
            display_source_from_code(params.video_source_value);
        display_cfg->hscale_scheme = 
            hscale_scheme_from_code(params.horiz_scheme_value);
        display_cfg->hscale = params.horiz_scale_value;
        display_cfg->hres = params.horiz_res_value;
        display_cfg->vscale_scheme = 
            vscale_scheme_from_code(params.vert_scheme_value);
        display_cfg->vscale = params.vert_scale_value;
        display_cfg->vres = params.vert_res_value;
        display_cfg->keep_aspect = params.keep_aspect_value;
        display_cfg->hpos_scheme =
            hpos_scheme_from_code(params.horiz_position_value);
        display_cfg->vpos_scheme =
            vpos_scheme_from_code(params.vert_position_value);
        display_cfg->hlocation =
            hlocation_from_code(params.horiz_location_value);
        display_cfg->vlocation =
            vlocation_from_code(params.vert_location_value);
        display_cfg->hoffset = params.horiz_offset_value;
        display_cfg->voffset = params.vert_offset_value;
        display_cfg->transparency = params.transparency_value;
    }
}

int xp_get_override_config(HAL *hp, struct override_config *override_cfg) {
    struct display_parm_params params;

    memset(override_cfg, '\0', sizeof(struct override_config));
    params.userptr = hp;

    if (hal_get_hardware_type(hp) != V2DI50_DEVICE) {
        retrieveDisplayParametersValues(&params);
        override_cfg->ov_option = 
            xp_override_value_from_code(params.ov_option_value);
        override_cfg->ov_color = 
            color_value_from_code(params.ov_color_value);
        override_cfg->ov_keepaspect = params.keep_aspect_value;
        override_cfg->hscale_scheme =
            hscale_scheme_from_code(params.horiz_scheme_value);
        override_cfg->hscale = params.horiz_scale_value;
        override_cfg->hres = params.horiz_res_value;
        override_cfg->vscale_scheme =
            vscale_scheme_from_code(params.vert_scheme_value);
        override_cfg->vscale = params.vert_scale_value;
        override_cfg->vres = params.vert_res_value;
        override_cfg->hpos_scheme =
            hpos_scheme_from_code(params.horiz_position_value);
        override_cfg->vpos_scheme =
            vpos_scheme_from_code(params.vert_position_value);
        override_cfg->hlocation =
            hlocation_from_code(params.horiz_location_value);
        override_cfg->vlocation =
            vlocation_from_code(params.vert_location_value);
        override_cfg->hoffset = params.horiz_offset_value;
        override_cfg->voffset = params.vert_offset_value;
    }
    return 0;
}

int applyDisplayParameters(struct display_parm_params *params, 
                struct display_parm_params *orig_params){

    struct ov_param_io ov;
    char displayconfig[255];
	int fd=0;
    HAL *hp = params->userptr;
    if (hp == NULL) return -1;
    hal_get_overlay_params(hp, &ov);
    ov.setmask = 0;

    // Apply only position and transparency changes to driver and only if
    // source and scale did'nt change otherwise wait for hald to apply it
    if (((params->video_source_value == orig_params->video_source_value) &&
         (params->horiz_scheme_value == orig_params->horiz_scheme_value) &&
         (params->vert_scheme_value == orig_params->vert_scheme_value) &&
         (params->horiz_scale_value == orig_params->horiz_scale_value) &&
         (params->vert_scale_value == orig_params->vert_scale_value) &&
         (params->horiz_res_value == orig_params->horiz_res_value) &&
         (params->vert_res_value == orig_params->vert_res_value) &&
         (params->keep_aspect_value == orig_params->keep_aspect_value)) ||
        (!params->video_source_set && !params->horiz_scheme_set &&
         !params->vert_scheme_set && !params->horiz_scale_set &&
         !params->vert_scale_set && !params->horiz_res_set &&
         !params->vert_res_set && !params->keep_aspect_set)) {

        if (params->horiz_position_set) { 
            ov.flags &= ~OV_FLAG_HPOSMASK;
            switch (params->horiz_position_value) { 
                case DISPLAY_PARM_HORIZ_POSITION_LOCATION: 
                    ov.flags |= OV_FLAG_HPOSLOCATION;
                    break;
                case DISPLAY_PARM_HORIZ_POSITION_OFFSET:
                    ov.flags |= OV_FLAG_HPOSOFFSET;
                    break;
            }
            ov.setmask |= OV_SET_PARAM_POSITION;
        }
        if (params->vert_position_set) { 
            ov.flags &= ~OV_FLAG_VPOSMASK;
            switch (params->vert_position_value) { 
                case DISPLAY_PARM_VERT_POSITION_LOCATION: 
                    ov.flags |= OV_FLAG_VPOSLOCATION;
                    break;
                case DISPLAY_PARM_VERT_POSITION_OFFSET:
                    ov.flags |= OV_FLAG_VPOSOFFSET;
                    break;
            }
            ov.setmask |= OV_SET_PARAM_POSITION;
        }
        if  (params->horiz_location_set) { 
            switch (params->horiz_location_value) {
                case DISPLAY_PARM_HORIZ_LOCATION_LEFT:
                    ov.hlocation = OV_LEFT_ADJUSTED;
                    break;
                case DISPLAY_PARM_HORIZ_LOCATION_CENTER:
                    ov.hlocation = OV_CENTER_ADJUSTED;
                    break;
                case DISPLAY_PARM_HORIZ_LOCATION_RIGHT:
                    ov.hlocation = OV_RIGHT_ADJUSTED;
                    break;
            }
            ov.setmask |= OV_SET_PARAM_POSITION;
        }
        if  (params->horiz_offset_set) { 
            ov.hoffset = params->horiz_offset_value;
            ov.setmask |= OV_SET_PARAM_POSITION;
        }
        if  (params->vert_location_set) { 
            switch (params->vert_location_value) {
                case DISPLAY_PARM_VERT_LOCATION_TOP:
                    ov.vlocation = OV_TOP_ADJUSTED;
                    break;
                case DISPLAY_PARM_VERT_LOCATION_MIDDLE:
                    ov.vlocation = OV_CENTER_ADJUSTED;
                    break;
                case DISPLAY_PARM_VERT_LOCATION_BOTTOM:
                    ov.vlocation = OV_BOTTOM_ADJUSTED;
                    break;
            }
            ov.setmask |= OV_SET_PARAM_POSITION;
        }
        if  (params->vert_offset_set) { 
            ov.voffset = params->vert_offset_value;
            ov.setmask |= OV_SET_PARAM_POSITION;
        }
        if (params->transparency_set) {
            ov.oalpha = params->transparency_value;
            ov.setmask |= OV_SET_PARAM_ALPHA;
        }
        if (ov.setmask) {
            hal_set_overlay_params(hp, &ov); 
        }
    }

    if (params->video_source_set || params->horiz_scheme_set ||
            params->vert_scheme_set || params->keep_aspect_set ||
            params->horiz_location_set || params->vert_location_set ||
            params->horiz_offset_set || params->vert_offset_set ||
            params->horiz_scale_set || params->vert_scale_set) {
        struct strmsrv_rx_clnt_params clnt_params;
        memset(&clnt_params, 0x00, sizeof(clnt_params));
        clnt_params.rfb_refresh = 1;
        clnt_params.rfb_mousecursor = 1;
        strmsrv_set_rx_clnt_params(hal_get_channel_number(hp), &clnt_params);
    }
    // Touching the file that is used by HALD to figure out that the config has changed
	sprintf (displayconfig, "/tmp/displayconfig_%d", hal_get_board_number(hp));
    fd = open (displayconfig, O_CREAT|O_WRONLY);
    if (fd > 0)	close(fd);

    return RC_NORMAL;
}
