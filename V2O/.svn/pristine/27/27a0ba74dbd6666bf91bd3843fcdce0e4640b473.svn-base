
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#ifndef __XPI__
#include "lut_table.h"
#endif
#include "lut_utils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlib.h"
#include "xp_tx_video_params_menu_gen.h"

static void paramsInitXpTxVideoParameters(struct xp_tx_video_parm_params 
                                          *params)
{
    struct LUT_struct current_alut;
    int lut_cpmp_index, avflag;

    HAL *hp = params->userptr;
    int value, level;
    
    if (hp == NULL)
        return;


    // Get phase adjust
    params->phase_shift_set = 1;
    value = hal_get_phase_adjust_k(hp);
    level = sampling_phase_value_to_level(value);
    params->phase_shift_value = level;

    // Get horizontal position
    params->horizontal_position_set = 1;
    params->horizontal_position_value = hal_get_hd_k(hp);

    // Get vertical position
    params->vertical_position_set = 1;
    params->vertical_position_value = hal_get_vd_k(hp);

    // Get fine noise elimination
    params->noise_fine_set = 1;
    value = 0;
    hal_get_ac_noise_threshold(hp, &value);
    level = ac_noise_value_to_level(value);
    params->noise_fine_value = level;

    // Get coarse noise elimination
    params->noise_coarse_set = 1;
    value = 0;
    hal_get_dc_noise_threshold(hp, &value);
    level = dc_noise_value_to_level(value);
    params->noise_coarse_value = level;

    // Get Charge Pump Current
    params->lut_cpmp_set = 1;
    if (hal_get_addside_lut(hp, &current_alut) == 0)
        lut_cpmp_index = current_alut.a_cpmp;
    else
        lut_cpmp_index = hal_get_default_charge_pump_curr_k(hp);


    switch (lut_cpmp_index) {
    case 0:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_50;
        break;
    case 1:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_100;
        break;
    case 2:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_150;
        break;
    case 3:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_250;
        break;
    case 4:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_350;
        break;
    case 5:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_500;
        break;
    case 6:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_750;
        break;
    case 7:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_1500;
        break;
    default:
        params->lut_cpmp_value = XP_TX_VIDEO_PARM_LUT_CPMP_AUTO;
        break;
    }

    avflag = hal_getAudioVideoFlag(hp);

    // Get DVI mode
    params->dvi_mode_set = 1;
    if (avflag & AVFLAG_DVIAUTO)
        params->dvi_mode_value = XP_TX_VIDEO_PARM_DVI_MODE_AUTO;
    else
        params->dvi_mode_value = XP_TX_VIDEO_PARM_DVI_MODE_LUT;

    // Get Optimize Latency 
    params->optimize_latency_set = 1;
    if (avflag & AVFLAG_LATENCY)
        params->optimize_latency_value = XP_TX_VIDEO_PARM_OPTIMIZE_LATENCY_YES;
    else
        params->optimize_latency_value = XP_TX_VIDEO_PARM_OPTIMIZE_LATENCY_NO;

}

int get_configured_tx_video_output(HAL *hp) {
    struct xp_tx_video_parm_params params;
    params.userptr = hp;
    retrieveXpTxVideoParametersValues(&params);
    return params.video_output_value;
}

int get_configured_dvimode_auto(HAL *hp) {
    struct xp_tx_video_parm_params params;
    params.userptr = hp;
    retrieveXpTxVideoParametersValues(&params);
    return (params.dvi_mode_value == XP_TX_VIDEO_PARM_DVI_MODE_AUTO) ? 1 : 0;
}

int get_configured_optimize_latency(HAL *hp) {
    struct xp_tx_video_parm_params params;
    params.userptr = hp;
    retrieveXpTxVideoParametersValues(&params);
    return (params.optimize_latency_value == XP_TX_VIDEO_PARM_OPTIMIZE_LATENCY_YES) ? 1 : 0;
}


unsigned long  get_configured_iframe_interval(HAL *hp) {
    struct xp_tx_video_parm_params params;
    params.userptr = hp;
    retrieveXpTxVideoParametersValues(&params);
    return (unsigned long) params.iframe_interval_value;
}

int applyXpTxVideoParameters(struct xp_tx_video_parm_params *params,
                             struct xp_tx_video_parm_params *orig_params)
{
    struct LUT_struct lut;
    HAL *hp = params->userptr;
    int avflag;

    if (hp == NULL)
        return RC_ERROR;

    // Set phase adjust
    if (params->phase_shift_set == 1) {
        int level = params->phase_shift_value;
        int value = sampling_phase_level_to_value(level);
        hal_set_phase_adjust_k(hp, value);
    }

    // Set horizontal position
    if (params->horizontal_position_set == 1) {
        hal_adjust_hd_k(hp, params->horizontal_position_value);
    }

    // Set vertical position
    if (params->vertical_position_set == 1) {
        hal_adjust_vd_k(hp, params->vertical_position_value);
    }

    // Set fine noise elimination
    if (params->noise_fine_set == 1) {
        int level = params->noise_fine_value;
        int value = ac_noise_level_to_value(level);
        hal_set_ac_noise_threshold(hp, value);
    }

    // Set coarse noise elimination
    if (params->noise_coarse_set == 1) {
        int level = params->noise_coarse_value;
        int value = dc_noise_level_to_value(level);
        hal_set_dc_noise_threshold(hp, value);
    }

    // Apply charge_pump_current
    if (params->lut_cpmp_set == 1) {


        int lut_cpmp_index = 0xFF;
        switch (params->lut_cpmp_value) {
        case XP_TX_VIDEO_PARM_LUT_CPMP_AUTO:
            lut_cpmp_index = 0xFF;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_50:
            lut_cpmp_index = 0;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_100:
            lut_cpmp_index = 1;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_150:
            lut_cpmp_index = 2;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_250:
            lut_cpmp_index = 3;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_350:
            lut_cpmp_index = 4;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_500:
            lut_cpmp_index = 5;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_750:
            lut_cpmp_index = 6;
            break;
        case XP_TX_VIDEO_PARM_LUT_CPMP_1500:
            lut_cpmp_index = 7;
            break;
        }

        // Do not apply charge_pump_current unless we have to. Applying
        // this triggers an lut change due to which all other values get reset
        if (params->lut_cpmp_value != orig_params->lut_cpmp_value) {
            struct lut_list_params lutlist;
            
            memset(&lut, 0x00, sizeof(lut));
            if (hal_get_addside_lut(hp, &lut) < 0 || lut.a_hres == 0) {
                hal_set_default_charge_pump_curr_k(hp, lut_cpmp_index);
            }
            else if (get_lut_list_params_by_lutid(&lutlist, lut.a_alutreg)) {
                char buff[128];
                if (lut_cpmp_index == 0xFF)
                    sprintf(buff, 
                        "mod_lut_list -alutreg %d -cpmp Auto -channelNumber %d %d", 
                        lut.a_alutreg, 
                        hal_get_board_number(hp)+1, 
                        lutlist.lut_list_id);
                else
                    sprintf(buff, 
                        "mod_lut_list -alutreg %d -cpmp %d -channelNumber %d %d", 
                        lut.a_alutreg, lut_cpmp_index, 
                        hal_get_board_number(hp)+1,
                        lutlist.lut_list_id);
                system_command(buff);
            }
        }
    }

    // Set DVI mode
    if (params->dvi_mode_set == 1) {
       if ((avflag = hal_getAudioVideoFlag(hp)) >= 0) {
           if (params->dvi_mode_value == XP_TX_VIDEO_PARM_DVI_MODE_LUT) {
               hal_setAudioVideoFlag(hp, avflag & ~AVFLAG_DVIAUTO);
           }
           else {
               hal_setAudioVideoFlag(hp, avflag | AVFLAG_DVIAUTO);
           }
       }
       hal_resetAddsideVideo(hp);
    }

    // Set Optimize Latency
    if (params->optimize_latency_set == 1) {
        if ((avflag = hal_getAudioVideoFlag(hp)) >= 0) {
            if (params->optimize_latency_value == 
                    XP_TX_VIDEO_PARM_OPTIMIZE_LATENCY_NO) {
                hal_set_optimize_latency(hp, 0);
            }
            else {
                hal_set_optimize_latency(hp, 1);
            }
        }
    }

    // Set I-Frame Interval 
    if (params->iframe_interval_set) {
        hal_set_iframe_interval(hp, 
                (unsigned long) params->iframe_interval_value) ;
    }
    
    // Set Loopback Video Outupt 
    if (params->video_output_set) {
        hal_set_video_output(hp, params->video_output_value);
    }

    return RC_NORMAL;
}

void XpTxVideoParametersInit()
{

    registerXpTxVideoParametersParamsInitFunction
        (paramsInitXpTxVideoParameters);
}
