#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlog.h"
#include "v2d.h"
#include "tx_splash_config.h"
#include "tx_splash_params_menu_gen.h"
#include "strmsrv_comm.h"
#include "lut_utils.h"
#include "slice_size.h"

int get_tx_splash_config(HAL *hp, struct tx_splash_config *splash_cfg) {

    struct tx_splash_parm_params params;
    int slice_size;
    int encoded_width;
    int encoded_height;
    int slice_width = 0;
    int slice_height = 0;

    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveTxSplashParametersValues(&params);

    switch (params.splash_screen_value) {
    case TX_SPLASH_PARM_SPLASH_SCREEN_BLUE_SCREEN:
        splash_cfg->tx_splash = SPLASH_TYPE_BLUE;
        break;
    case TX_SPLASH_PARM_SPLASH_SCREEN_GRID_LINES:
        splash_cfg->tx_splash = SPLASH_TYPE_GRID;
        break;
    }

    splash_cfg->splash_rate = params.splash_bw_value;
    splash_cfg->enable_splash = params.enable_splash_value;
    splash_cfg->splash_stereo = params.stereo_value;
    splash_cfg->splash_orig_hres = params.horiz_resolution_value;
    splash_cfg->splash_orig_vres = params.vert_resolution_value;
    splash_cfg->splash_refresh = params.refresh_rate_value;
    splash_cfg->splash_pcr = params.pixel_clock_rate_value;
    splash_cfg->splash_hfp = params.horiz_front_porch_value;
    splash_cfg->splash_vfp = params.vert_front_porch_value;
    splash_cfg->splash_hbp = params.horiz_back_porch_value;
    splash_cfg->splash_vbp = params.vert_back_porch_value;
    splash_cfg->splash_hsync = params.horiz_synch_value;
    splash_cfg->splash_vsync = params.vert_synch_value;
    splash_cfg->splash_mquant = SPLASH_MQUANT;
    splash_cfg->splash_dcquant = SPLASH_DCQUANT;
    splash_cfg->splash_slicesize = 32;
    splash_cfg->splash_slicewidth = 0;
    splash_cfg->splash_sliceheight = 0;
    //splash_cfg->splash_codecversion = hal_get_codec_version(hp);
    splash_cfg->splash_codecversion = CODEC_VERSION_STANDARD;

    if (splash_cfg->splash_codecversion == 1) {
      compute_slice_size(splash_cfg->splash_orig_hres, splash_cfg->splash_orig_vres,
            &slice_size, &encoded_width, &encoded_height);
    }
    else {
      compute_svc_slice_size(splash_cfg->splash_orig_hres,
                splash_cfg->splash_orig_vres, &slice_size, &slice_width,
                &slice_height, &encoded_width, &encoded_height);
    }
    splash_cfg->splash_hres = encoded_width;
    splash_cfg->splash_vres = encoded_height;
    splash_cfg->splash_slicesize = slice_size;
    splash_cfg->splash_slicewidth = slice_width;
    splash_cfg->splash_sliceheight = slice_height;

    return 0;
}

int applyTxSplashParameters(struct tx_splash_parm_params *params,
                        struct tx_splash_parm_params
                        *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    // We let the savedParameter callback handle the actual apply
    // since the parameters are not written to config till
    // this function returns normally

    return RC_NORMAL;
}

static void txSplashParametersSaved(void *userptr)
{
    HAL *hp = userptr;

    if (hp == NULL)
        return;
    strmsrv_set_tx_splash_params(hal_get_channel_number(hp));

}

void TxSplashParametersInit()
{

    registerTxSplashParametersSavedFunction(txSplashParametersSaved);
}

int updateTxSplashLutParameters(HAL *hp, int lutid)
{
    char buff[400];
    struct LUT_struct lut;
    int rc = -1;

    if (get_lut_entry_by_lutid(hp, &lut, lutid) == 1) {
        sprintf(buff,
            "set_tx_splash_parm -channelNumber %d -stereo %s "
            "-horiz_resolution %d -vert_resolution %d -refresh_rate %6.2f "
            "-pixel_clock_rate %6.2f -horiz_front_porch %d "
            "-vert_front_porch %d -horiz_back_porch %d -vert_back_porch %d "
            "-horiz_sync %d -vert_sync %d",
            hal_get_channel_number(hp), (lut.a_stereo) ? "Yes" : "No",
            lut.a_orig_hres, lut.a_orig_vres, lut.a_refresh, lut.a_pcr,
            lut.a_hfp, lut.a_vfp, lut.a_hbp, lut.a_vbp,
            lut.a_hsync, lut.a_vsync);
        rc = system_command(buff);
        if (rc < 0) {
            VLOG_INFO("Channel %d. Failed to set Splash LUT parameters",
                   hal_get_channel_number(hp));
        }
    }
    return rc;
}


