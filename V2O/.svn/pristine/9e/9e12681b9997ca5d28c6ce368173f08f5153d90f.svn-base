
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "vlib.h"
#include "rx_video_params_menu_gen.h"
#include "vlog.h"

static void paramsInitRxVideoParameters(struct rx_video_parm_params *params)
{
    unsigned short dcreg;

    HAL *hp = params->userptr;

    if (hp == NULL)
        return;

    // Get pixel adjust
    // Nothing to do here... We currently just reflect 
    // the config file value

    // Get GenLock mode
    // Nothing to do here... We currently just reflect 
    // the config file value

    // Get Video Ouptut
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE) { 
        params->video_output_value = 1;
        params->video_output_set = 1;
    }

    // Get LR sync
    dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
    if (dcreg & (DCREG_LRSYNC_POL))
        params->stereo_lr_sync_value = RX_VIDEO_PARM_STEREO_LR_SYNC_REV;
    else
        params->stereo_lr_sync_value = RX_VIDEO_PARM_STEREO_LR_SYNC_NORM;
    params->stereo_lr_sync_set = 1;

    // Get horizontal sync
    if (dcreg & DCREG_HSYNC_POL)
        params->horizontal_sync_pol_value = RX_VIDEO_PARM_HORIZONTAL_SYNC_POL_NEG;
    else
        params->horizontal_sync_pol_value = RX_VIDEO_PARM_HORIZONTAL_SYNC_POL_POS;
    params->horizontal_sync_pol_set = 1;

    // Get vertical sync
    if (dcreg & DCREG_VSYNC_POL)
        params->vertical_sync_pol_value = RX_VIDEO_PARM_VERTICAL_SYNC_POL_NEG;
    else
        params->vertical_sync_pol_value = RX_VIDEO_PARM_VERTICAL_SYNC_POL_POS;
    params->vertical_sync_pol_set = 1;

}

int get_configured_rx_video_output(HAL *hp) {
    struct rx_video_parm_params params;
    params.userptr = hp;
    retrieveRxVideoParametersValues(&params);
    return params.video_output_value;
}

void get_configured_rx_sync_polarities(HAL *hp, 
                                       int *lrsync, 
                                       int *hsync, 
                                       int *vsync) {
    struct rx_video_parm_params params;
    params.userptr = hp;
    retrieveRxVideoParametersValues(&params);
    if (params.stereo_lr_sync_value == RX_VIDEO_PARM_STEREO_LR_SYNC_REV)
        *lrsync = 1;
    else
        *lrsync = 0;
    if (params.horizontal_sync_pol_value == RX_VIDEO_PARM_HORIZONTAL_SYNC_POL_NEG)
        *hsync = 1;
    else
        *hsync = 0;
    if (params.horizontal_sync_pol_value == RX_VIDEO_PARM_HORIZONTAL_SYNC_POL_NEG)
        *vsync = 1;
    else
        *vsync = 0;
}

int get_configured_rx_genlock_params(HAL *hp, int *mode, int *delta) {
    struct rx_video_parm_params params;
    params.userptr = hp;
    retrieveRxVideoParametersValues(&params);
    if (params.genlock_mode_value == RX_VIDEO_PARM_GENLOCK_MODE_MASTER)
        *mode = GENLOCK_MODE_MASTER;
    else if (params.genlock_mode_value == RX_VIDEO_PARM_GENLOCK_MODE_SLAVE)
        *mode = GENLOCK_MODE_SLAVE;
    else
        *mode = GENLOCK_MODE_NONE;
    *delta = params.master_delay_value;
    return 0;
}

int applyRxVideoParameters(struct rx_video_parm_params *params,
                           struct rx_video_parm_params
                           *orig_params)
{
    unsigned short dcreg;
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    // Set pixel adjust
    if (params->pixel_adjust_set == 1) {
        hal_set_pixel_adjust_k(hp, params->pixel_adjust_value);
    }

    // Set video output
    if (hal_get_hardware_type(hp) == PLX_DEVICE && 
            params->video_output_set == 1) {
            hal_set_video_output(hp,params->video_output_value);
    }

    // Set GenLock mode only for I50 (hald will do it for XP)
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE && 
            params->genlock_mode_set == 1) {
        if (params->genlock_mode_value == RX_VIDEO_PARM_GENLOCK_MODE_MASTER)
            hal_setGenlockMode(hp, GENLOCK_MODE_MASTER);
        else if (params->genlock_mode_value == 
                RX_VIDEO_PARM_GENLOCK_MODE_SLAVE)
            hal_setGenlockMode(hp, GENLOCK_MODE_SLAVE);
        else 
            return RC_ERROR;
    }

    // Set LR sync
    if (params->stereo_lr_sync_set == 1) {
        dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        if (params->stereo_lr_sync_value == 
            RX_VIDEO_PARM_STEREO_LR_SYNC_NORM) {
            dcreg &= ~(DCREG_LRSYNC_POL);
        } else {
            dcreg |= (DCREG_LRSYNC_POL);
        }
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
    }

    // Set horizontal sync
    if (params->horizontal_sync_pol_set == 1) {
        dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        if (params->horizontal_sync_pol_value == 
            RX_VIDEO_PARM_HORIZONTAL_SYNC_POL_POS) {
            dcreg &= ~(DCREG_HSYNC_POL);
        } else {
            dcreg |= (DCREG_HSYNC_POL);
        }
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
    }

    // Set vertical sync
    if (params->vertical_sync_pol_set == 1) {
        dcreg = hal_get_fpga_val_k(hp, FPGA_DCREG);
        if (params->vertical_sync_pol_value == 
            RX_VIDEO_PARM_VERTICAL_SYNC_POL_POS) {
            dcreg &= ~(DCREG_VSYNC_POL);
        } else {
            dcreg |= (DCREG_VSYNC_POL);
        }
        hal_set_fpga_val_k(hp, FPGA_DCREG, dcreg);
    }

    return RC_NORMAL;
}

void RxVideoParametersInit()
{
    registerRxVideoParametersParamsInitFunction(paramsInitRxVideoParameters);
}


