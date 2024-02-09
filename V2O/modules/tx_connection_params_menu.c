
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "tx_connection_params_menu_gen.h"
#include "strmsrv_comm.h"
#include "tx_connection_config.h"
#include "v2d.h"

static int compute_av_option(int av_option_enum)
{

    int av_option = 0;
    switch (av_option_enum) {
    case TX_CONNECTION_PARM_AV_OPTION_VIDEO:
        av_option = AVOPTION_VIDEO;
        break;
    case TX_CONNECTION_PARM_AV_OPTION_AUDIO:
        av_option = AVOPTION_AUDIO;
        break;
    case TX_CONNECTION_PARM_AV_OPTION_BOTH:
        av_option = AVOPTION_AUDIO_VIDEO;
        break;
    }
    return av_option;
}

int get_tx_connection_config(HAL *hp, struct tx_connection_config *cfg) {

    struct tx_connection_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;

    retrieveConfiguredTxConnectionParametersValues(&params);

    cfg->bandwidth = params.bandwidth_value;
    cfg->burst_ratio = params.burst_ratio_value;
    cfg->burst_duration = params.burst_duration_value;
    cfg->low_compression = params.low_compression_value;
    cfg->high_compression = params.high_compression_value;
    cfg->av_option = params.av_option_value;
    cfg->framerate_divider = params.framerate_divider_value;
    cfg->min_refresh = params.min_refresh_value;
    cfg->max_refresh = params.max_refresh_value;
    cfg->min_slice = params.min_slice_value;
    cfg->max_slice = params.max_slice_value;
    cfg->block_threshold = params.block_threshold_value;
    if (params.color_sampling_value ==
            TX_CONNECTION_PARM_COLOR_SAMPLING_DYNAMIC_4_COLON_4_COLON_4)
        cfg->enable_444 =
            (hal_get_hardware_type(hp) == V2DI50_DEVICE) ?  COMPMODE_NONE :
            COMPMODE_MOTION_444;
    else if (params.color_sampling_value ==
            TX_CONNECTION_PARM_COLOR_SAMPLING_STATIC_4_COLON_4_COLON_4)
        cfg->enable_444 = COMPMODE_444;
    else
        cfg->enable_444 = COMPMODE_NONE;
    cfg->kbm_idle_limit = params.kbm_idle_limit_value;
    cfg->kbm_switch = params.kbm_switch_value;

    cfg->av_option = compute_av_option(params.av_option_value);

    // Legacy audio types not supported. Place holder for future audio types
    cfg->audio_type = 0;

    return 0;
}

static void initializeTxConnectionParameters(struct tx_connection_parm_params
                                             *params)
{
    HAL *hp = params->userptr;
    struct pc_param_io pcpio;

    if (hp == NULL)
        return;

    struct strmsrv_tx_params tx_params;
    memset(&tx_params, 0x00, sizeof(tx_params));

    strmsrv_get_tx_params(hal_get_channel_number(hp), &tx_params);

    if (tx_params.bandwidth_set) {
        params->bandwidth_set = 1;
        params->bandwidth_value = tx_params.bandwidth;
    }

    if (tx_params.burst_ratio_set) {
        params->burst_ratio_set = 1;
        params->burst_ratio_value = tx_params.burst_ratio;
    }

    if (tx_params.burst_duration_set) {
        params->burst_duration_set = 1;
        params->burst_duration_value = tx_params.burst_duration;
    }

    hal_get_pc_params(hp, &pcpio);

    params->min_refresh_set = 1;
    params->min_refresh_value = pcpio.pc_rlow;

    params->max_refresh_set = 1;
    params->max_refresh_value = pcpio.pc_rhigh;

    params->min_slice_set = 1;
    params->min_slice_value = pcpio.pc_ylow;

    params->max_slice_set = 1;
    params->max_slice_value = pcpio.pc_yhigh;

    params->block_threshold_set = 1;
    params->block_threshold_value = pcpio.pc_xblocks;

    params->low_compression_set = 1;
    hal_get_addside_low_compression(hp, &params->low_compression_value);

    params->high_compression_set = 1;
    hal_get_addside_high_compression(hp, &params->high_compression_value);

    params->color_sampling_set = 1;

    int compmode, motioncompmode;
    hal_get_addside_compmode(hp, &compmode, &motioncompmode);

    if (motioncompmode)
        params->color_sampling_value =
            TX_CONNECTION_PARM_COLOR_SAMPLING_DYNAMIC_4_COLON_4_COLON_4;
    else if (compmode == COMPMODE_444)
        params->color_sampling_value =
            TX_CONNECTION_PARM_COLOR_SAMPLING_STATIC_4_COLON_4_COLON_4;
    else
        params->color_sampling_value =
            TX_CONNECTION_PARM_COLOR_SAMPLING_4_COLON_2_COLON_2;

    params->framerate_divider_set = 1;
    params->framerate_divider_value = hal_get_framerate_divider(hp);
}

int applyTxConnectionParameters(struct tx_connection_parm_params *params,
                                struct tx_connection_parm_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL) {
        return RC_ERROR;
    }

    struct strmsrv_tx_params tx_params;
    int set_tx_params = 0;
    int is_xpi = (hal_get_hardware_type(hp) == XPI_DEVICE) ? 1 : 0;

    memset(&tx_params, 0x00, sizeof(tx_params));
    if (params->bandwidth_set == 1) {
        tx_params.bandwidth_set = 1;
        tx_params.bandwidth = params->bandwidth_value;
        set_tx_params = 1;
    }

    if (params->burst_ratio_set == 1) {
        tx_params.burst_ratio_set = 1;
        tx_params.burst_ratio = params->burst_ratio_value;
        set_tx_params = 1;
    }

    if (params->burst_duration_set == 1) {
        tx_params.burst_duration_set = 1;
        tx_params.burst_duration = params->burst_duration_value;
        set_tx_params = 1;
    }

    if (params->kbm_switch_set == 1) {
        tx_params.kbm_switch_set = 1;
        tx_params.kbm_switch = params->kbm_switch_value;
        set_tx_params = 1;
    }

    if (params->kbm_idle_limit_set == 1) {
        tx_params.kbm_idle_limit_set = 1;
        tx_params.kbm_idle_limit = params->kbm_idle_limit_value;
        set_tx_params = 1;
    }

    if (params->av_option_set == 1) {
        tx_params.av_option_set = 1;
        tx_params.av_option = compute_av_option(params->av_option_value);
        set_tx_params = 1;
    }

    if (set_tx_params)
        strmsrv_set_tx_params(hal_get_channel_number(hp), &tx_params);

    if (params->low_compression_set == 1) {
        hal_set_addside_low_compression(hp, params->low_compression_value);
    }

    if (params->high_compression_set == 1) {
        hal_set_addside_high_compression(hp, params->high_compression_value);
    }

    struct pc_param_io pcpio;
    
    memset(&pcpio, 0x00, sizeof(pcpio));
    if (params->min_refresh_set == 1) {
        pcpio.setmask |= PC_SET_PARAM_RLOW;
        pcpio.pc_rlow = params->min_refresh_value;
    }
    if (params->max_refresh_set == 1) {
        pcpio.setmask |= PC_SET_PARAM_RHIGH;
        pcpio.pc_rhigh = params->max_refresh_value;
    }
    if (params->min_slice_set == 1) {
        pcpio.setmask |= PC_SET_PARAM_YLOW;
        pcpio.pc_ylow = params->min_slice_value;
    }
    if (params->max_slice_set == 1) {
        pcpio.setmask |= PC_SET_PARAM_YHIGH;
        pcpio.pc_yhigh = params->max_slice_value;
    }
    if (params->block_threshold_set) {
        pcpio.setmask |= PC_SET_PARAM_XBLOCKS;
        pcpio.pc_xblocks = params->block_threshold_value;
    }
    if (pcpio.setmask)
        hal_set_pc_params(hp, &pcpio);

    if (params->color_sampling_set) {
        int compmode = 0, motioncompmode = 0;
        if (params->color_sampling_value ==
                TX_CONNECTION_PARM_COLOR_SAMPLING_DYNAMIC_4_COLON_4_COLON_4) {
            /* I50 does not support motion 444 */
            if (hal_get_hardware_type(hp) == V2DI50_DEVICE)
                return RC_ERROR;
            motioncompmode = 1;
        }
        else if (params->color_sampling_value ==
                TX_CONNECTION_PARM_COLOR_SAMPLING_STATIC_4_COLON_4_COLON_4)
            compmode = 1;
        hal_set_addside_compmode(hp, compmode, motioncompmode);
    }

    if (params->framerate_divider_set == 1) {
        hal_set_framerate_divider(hp, params->framerate_divider_value);
    }

    return RC_NORMAL;

}


void TxConnectionParametersInit()
{

    registerTxConnectionParametersParamsInitFunction
        (initializeTxConnectionParameters);
}

