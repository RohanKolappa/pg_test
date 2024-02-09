#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <netinet/in.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "hal_hwinfo.h"
#include "menuutils.h"
#include "configureSystem.h"
#include "configureVideo.h"
#include "loadnet.h"
#include "vlib.h"
#include "osd_control.h"
#include "vlog.h"
#include "remoteConfig.h"
#include "chromakey_params_menu_gen.h"
#include "chromakey_control.h"

int get_chromakey_config(HAL *hp, struct chromakey_config *cfg) {
    struct chromakey_parm_params params;

    if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE)
        return -1;

    memset(&params, 0, sizeof(struct chromakey_parm_params));
    params.userptr = hp;
    retrieveChromakeyParamValues(&params);

    if (params.enable_value == CHROMAKEY_PARM_ENABLE_YES)  
        cfg->enable = 1;
    else 
        cfg->enable = 0;
    if (params.colorref_value == CHROMAKEY_PARM_COLORREF_LOCATION) 
        cfg->colorref = COLORREF_LOCATION;
    else
        cfg->colorref = COLORREF_VALUE;
    cfg->transparency = params.transparency_value;
    cfg->redvalue = params.redvalue_value;
    cfg->redtolerance = params.redtolerance_value;
    cfg->greenvalue = params.greenvalue_value;
    cfg->greentolerance = params.greentolerance_value;
    cfg->bluevalue = params.bluevalue_value;
    cfg->bluetolerance = params.bluetolerance_value;
    cfg->pix1x = params.pix1x_value;
    cfg->pix1y = params.pix1y_value;
    cfg->pix2x = params.pix2x_value;
    cfg->pix2y = params.pix2y_value;
    cfg->pix3x = params.pix3x_value;
    cfg->pix3y = params.pix3y_value;

    return 0;
}

int set_chromakey_config(HAL *hp, struct chromakey_config *cfg) {
    int rc = 0;

    if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE)
        return -1;

    hal_set_chromakey_enable_k(hp, MATTE_LOCAL, cfg->enable);

    hal_set_chromakey_alpha_k(hp, MATTE_LOCAL, cfg->transparency);

    if (cfg->colorref == COLORREF_VALUE) 
        rc = set_color_registers_from_values(hp, MATTE_LOCAL, cfg);
    else
        rc = set_color_registers_from_pixels(hp, MATTE_LOCAL, cfg);

    return rc;
}

static int init_chromakey_cursors(HAL *hp, struct chromakey_config *cfg) {
    struct chromakey_config config, *ckcfg;
    struct LUT_struct lut;

    if (hal_get_display_lut(hp, &lut)) 
        return -1;
    if (cfg == NULL) {
        if (get_chromakey_config(hp, &config) < 0)
            return -1;
        ckcfg = &config;
    }
    else {
        ckcfg = cfg;
    }

    hal_init_pix_cursor(hp, ckcfg->pix1x, ckcfg->pix1y, lut.a_hres,
                            lut.a_vres, LOCALPIX1CURSOR);
    hal_init_pix_cursor(hp, ckcfg->pix2x, ckcfg->pix2y, lut.a_hres,
                            lut.a_vres, LOCALPIX2CURSOR);
    hal_init_pix_cursor(hp, ckcfg->pix3x, ckcfg->pix3y, lut.a_hres,
                            lut.a_vres, LOCALPIX3CURSOR);
    return 0;
}

static void disable_chromakey_cursors(HAL *hp) {
    hal_set_pix_cursor_status(hp, 0, LOCALPIX1CURSOR);
    hal_set_pix_cursor_status(hp, 0, LOCALPIX2CURSOR);
    hal_set_pix_cursor_status(hp, 0, LOCALPIX3CURSOR);
}

int applyChromakeyParam(struct chromakey_parm_params *params,
                           struct chromakey_parm_params *orig_params)
{
    HAL *hp = params->userptr;
    struct chromakey_config config;

    if (hp == NULL)
        return RC_ERROR;

    if (hal_get_hardware_type(hp) != PLX_DEVICE)
        return RC_NORMAL;

    get_chromakey_config(hp, &config);

    if (params->enable_set) 
        config.enable = 
            (params->enable_value == CHROMAKEY_PARM_ENABLE_YES) ? 1 : 0;

    if (params->transparency_set)
        config.transparency = params->transparency_value;

    if (params->colorref_set)
        config.colorref = (params->colorref_value == 
            CHROMAKEY_PARM_COLORREF_LOCATION) ? COLORREF_LOCATION :
            COLORREF_VALUE;

    if (params->redvalue_set)
        config.redvalue = params->redvalue_value;
    if (params->greenvalue_set)
        config.greenvalue = params->greenvalue_value;
    if (params->bluevalue_set)
        config.bluevalue = params->bluevalue_value;
    if (params->redtolerance_set)
        config.redtolerance = params->redtolerance_value;
    if (params->greentolerance_set)
        config.greentolerance = params->greentolerance_value;
    if (params->bluetolerance_set)
        config.bluetolerance = params->bluetolerance_value;
    if (params->pix1x_set)
        config.pix1x = params->pix1x_value;
    if (params->pix1y_set)
        config.pix1y = params->pix1y_value;
    if (params->pix2x_set)
        config.pix2x = params->pix2x_value;
    if (params->pix2y_set)
        config.pix2y = params->pix2y_value;
    if (params->pix3x_set)
        config.pix3x = params->pix3x_value;
    if (params->pix3y_set)
        config.pix3y = params->pix3y_value;

    if (params->pix1x_set || params->pix1y_set || params->pix2x_set ||
            params->pix2y_set || params->pix3x_set || params->pix3y_set) {
        init_chromakey_cursors(hp, &config);
    }

    if (set_chromakey_config(hp, &config) < 0)
        return RC_ERROR;

    return RC_NORMAL;
}

static int validateChromakeyParam(struct chromakey_parm_params *params,
                                         struct chromakey_parm_params *orig_params,
                                         char *error_string) {

    if (params->transparency_set) {
        if (params->transparency_value == 100) {
           sprintf(error_string,"Not good to have transparency as 100.");
           return RC_WARNING;
        }
    }
    return RC_NORMAL;
}

static void ChromakeyParamMenuInit(void *userptr) 
{
    HAL *hp = (HAL *) userptr ;
    init_chromakey_cursors(hp, NULL);

}

static void ChromakeyParamMenuExit(void *userptr)
{
    HAL *hp = (HAL *) userptr ;
    disable_chromakey_cursors(hp);
}

void ChromakeyParamInit()
{

    registerChromakeyParamMenuInitFunction(ChromakeyParamMenuInit);
    registerChromakeyParamMenuExitFunction(ChromakeyParamMenuExit);
    registerChromakeyParamValidateFunction(validateChromakeyParam); 
}

