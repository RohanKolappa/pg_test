#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "kbms_utils.h"
#include "override_config.h"
#include "kbmode_config.h"
#include "strmsrv_comm.h"
#include "vlog.h"

int get_kbcons_settings(HAL *hp, struct kbmode_config *kbmode_cfg) {

    struct keyboard_console_mode_settings_params params;

    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveKeyboardConsoleModeSettingsValues(&params);

    memset(kbmode_cfg, 0, sizeof(struct kbmode_config));
    kbmode_cfg->esc[0] = 
        keycode_from_name(cons_esc_1_name_from_code(params.esc_char_1_value));
    kbmode_cfg->esc[1] = 
        keycode_from_name(cons_esc_2_name_from_code(params.esc_char_2_value));
    kbmode_cfg->esc[2] = 
        keycode_from_name(cons_esc_3_name_from_code(params.esc_char_3_value));
    kbmode_cfg->esc[3] = 
        keycode_from_name(cons_esc_4_name_from_code(params.esc_char_4_value));
    kbmode_cfg->icon_color =
        color_code_from_name(cons_icon_color_from_code(params.icon_color_value));
    kbmode_cfg->icon_position = params.icon_position_value;
    kbmode_cfg->icon_duration = params.icon_duration_value;

    return 0;
}

int applyKeyboardConsoleModeSettings(
        struct keyboard_console_mode_settings_params *params,
        struct keyboard_console_mode_settings_params *orig_params)
{
    int bno;
    HAL *hp;
    struct strmsrv_rx_clnt_params clnt_params;
    struct strmsrv_tx_kbm_params kbm_params;
    int set_params = 0;

    memset(&kbm_params, 0x00, sizeof(kbm_params));
    memset(&clnt_params, 0x00, sizeof(clnt_params));

    if (params->esc_char_1_set == 1) {
        kbm_params.console_icon.esc_char_set[0] =
            clnt_params.console_icon.esc_char_set[0] = 1;
        kbm_params.console_icon.esc_char[0] =
            clnt_params.console_icon.esc_char[0] = 
            keycode_from_name(cons_esc_1_name_from_code(params->esc_char_1_value));
        set_params = 1;
    }
    if (params->esc_char_2_set == 1) {
        kbm_params.console_icon.esc_char_set[1] =
            clnt_params.console_icon.esc_char_set[1] = 1;
        kbm_params.console_icon.esc_char[1] =
            clnt_params.console_icon.esc_char[1] = 
            keycode_from_name(cons_esc_2_name_from_code(params->esc_char_2_value));
        set_params = 1;
    }
    if (params->esc_char_3_set == 1) {
        kbm_params.console_icon.esc_char_set[2] =
            clnt_params.console_icon.esc_char_set[2] = 1;
        kbm_params.console_icon.esc_char[2] =
            clnt_params.console_icon.esc_char[2] = 
            keycode_from_name(cons_esc_3_name_from_code(params->esc_char_3_value));
        set_params = 1;
    }
    if (params->esc_char_4_set == 1) {
        kbm_params.console_icon.esc_char_set[3] =
            clnt_params.console_icon.esc_char_set[3] = 1;
        kbm_params.console_icon.esc_char[3] =
            clnt_params.console_icon.esc_char[3] = 
            keycode_from_name(cons_esc_4_name_from_code(params->esc_char_4_value));
        set_params = 1;
    }

    if (set_params) {
        for (bno = 0; bno < MAX_FPGA_BOARDS; bno++) {
            if ((hp = hal_acquire(bno)) == NULL) {
                continue;
            }
            if (hal_get_board_type(hp) == BOARDTYPE_TX) {
                strmsrv_set_tx_kbm_params(bno+1, &kbm_params);
            }
            else {
                strmsrv_set_rx_clnt_params(bno+1, &clnt_params);
            }
            hal_release(hp);
        }
    }
    return RC_NORMAL;
}
