
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
#include "tx_video_params_menu_gen.h"

static void paramsInitTxVideoParameters(struct tx_video_parm_params
                                        *params)
{
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

}

int applyTxVideoParameters(struct tx_video_parm_params *params,
                           struct tx_video_parm_params *orig_params)
{
    struct LUT_struct lut;
    HAL *hp = params->userptr;

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
        case TX_VIDEO_PARM_LUT_CPMP_AUTO:
            lut_cpmp_index = 0xFF;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_50:
            lut_cpmp_index = 0;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_100:
            lut_cpmp_index = 1;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_150:
            lut_cpmp_index = 2;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_250:
            lut_cpmp_index = 3;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_350:
            lut_cpmp_index = 4;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_500:
            lut_cpmp_index = 5;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_750:
            lut_cpmp_index = 6;
            break;
        case TX_VIDEO_PARM_LUT_CPMP_1500:
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


    return RC_NORMAL;
}

void TxVideoParametersInit()
{

    registerTxVideoParametersParamsInitFunction
        (paramsInitTxVideoParameters);
}
