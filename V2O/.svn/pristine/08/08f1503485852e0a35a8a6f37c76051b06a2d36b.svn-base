#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "kbms_utils.h"
#include "override_config.h"
#include "tx_kblocal_menu_gen.h"
#include "kbmode_config.h"
#include "strmsrv_comm.h"

int get_tx_kblocal_settings(HAL *hp, struct kbmode_config *kbmode_cfg) {

    struct txkeyboard_local_mode_settings_params params;

    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveTxKeyboardLocalModeSettingsValues(&params);

    memset(kbmode_cfg, 0, sizeof(struct kbmode_config));
    kbmode_cfg->esc[0] = 
        keycode_from_name(tx_local_esc_1_name_from_code(params.esc_char_1_value));
    kbmode_cfg->esc[1] = 
        keycode_from_name(tx_local_esc_2_name_from_code(params.esc_char_2_value));
    kbmode_cfg->esc[2] = 
        keycode_from_name(tx_local_esc_3_name_from_code(params.esc_char_3_value));
    kbmode_cfg->esc[3] = 
        keycode_from_name(tx_local_esc_4_name_from_code(params.esc_char_4_value));
    return 0;
}

int applyTxKeyboardLocalModeSettings(
        struct txkeyboard_local_mode_settings_params *params,
        struct txkeyboard_local_mode_settings_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_tx_kbm_params kbm_params;
    int set_kbm_params = 0;

    memset(&kbm_params, 0x00, sizeof(kbm_params));

    if (params->esc_char_1_set == 1) {
        kbm_params.local_icon.esc_char_set[0] = 1;
        kbm_params.local_icon.esc_char[0] = 
            keycode_from_name(tx_local_esc_1_name_from_code(params->esc_char_1_value));
        set_kbm_params = 1;
    }
    if (params->esc_char_2_set == 1) {
        kbm_params.local_icon.esc_char_set[1] = 1;
        kbm_params.local_icon.esc_char[1] = 
            keycode_from_name(tx_local_esc_2_name_from_code(params->esc_char_2_value));
        set_kbm_params = 1;
    }
    if (params->esc_char_3_set == 1) {
        kbm_params.local_icon.esc_char_set[2] = 1;
        kbm_params.local_icon.esc_char[2] = 
            keycode_from_name(tx_local_esc_3_name_from_code(params->esc_char_3_value));
        set_kbm_params = 1;
    }
    if (params->esc_char_4_set == 1) {
        kbm_params.local_icon.esc_char_set[3] = 1;
        kbm_params.local_icon.esc_char[3] = 
            keycode_from_name(tx_local_esc_4_name_from_code(params->esc_char_4_value));
        set_kbm_params = 1;
    }

    if (set_kbm_params) {
        strmsrv_set_tx_kbm_params(hal_get_channel_number(hp), &kbm_params);
    }
    return RC_NORMAL;
}

