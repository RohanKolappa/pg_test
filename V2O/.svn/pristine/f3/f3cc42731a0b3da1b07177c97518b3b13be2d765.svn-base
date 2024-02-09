#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "clientutils.h"
#include "kbm_control.h"
#include "rx_kbm_params_menu_gen.h"
#include "rx_kbm_config.h"
#include "strmsrv_comm.h"
#include "vlog.h"

int get_rx_kbm_config(HAL *hp, struct rx_kbm_config *kbm_cfg) {

    struct rx_kbm_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveRxKBMParametersValues(&params);

    kbm_cfg->kbm_mode = KBM_MODE_REMOTE;
    kbm_cfg->local_keyboard_type = KB_TYPE_PC;
    kbm_cfg->local_mouse_type = MS_TYPE_IMPS2;

    if (params.kbm_mode_value == RX_KBM_PARM_KBM_MODE_LOCAL)
        kbm_cfg->kbm_mode = KBM_MODE_LOCAL;

    if (params.local_keyboard_type_value == RX_KBM_PARM_LOCAL_KEYBOARD_TYPE_SGI)
        kbm_cfg->local_keyboard_type = KB_TYPE_SGI;

    if (params.local_mouse_type_value == RX_KBM_PARM_LOCAL_MOUSE_TYPE_PS2)
        kbm_cfg->local_mouse_type = MS_TYPE_PS2;

    return 0;
}

int applyRxKBMParameters(struct rx_kbm_parm_params *params,
                         struct rx_kbm_parm_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    if (hp->hw_info.hardware_type != PLX_DEVICE)
        return RC_NORMAL;

    struct strmsrv_rx_clnt_params clnt_params;
    int set_clnt_params = 0;

    memset(&clnt_params, 0x00, sizeof(clnt_params));

    if (params->local_keyboard_type_set == 1) {
        clnt_params.local_kbtype_set = 1;
        if (params->local_keyboard_type_value == RX_KBM_PARM_LOCAL_KEYBOARD_TYPE_SGI)
            clnt_params.local_kbtype = KB_TYPE_SGI;
        else
            clnt_params.local_kbtype = KB_TYPE_PC;
        set_clnt_params = 1;
    }
    if (params->local_mouse_type_set == 1) {
        clnt_params.local_mstype_set = 1;
        if (params->local_mouse_type_value == RX_KBM_PARM_LOCAL_MOUSE_TYPE_PS2)
            clnt_params.local_mstype = MS_TYPE_PS2;
        else
            clnt_params.local_mstype = MS_TYPE_IMPS2;
        set_clnt_params = 1;
    }
    if (params->kbm_mode_set == 1) {
        clnt_params.kbmode_set = 1;
        if (params->kbm_mode_value == RX_KBM_PARM_KBM_MODE_LOCAL)
            clnt_params.kbmode = KBM_MODE_LOCAL;
        else
            clnt_params.kbmode = KBM_MODE_REMOTE;
        set_clnt_params = 1;
    }

    if (set_clnt_params) {
        strmsrv_set_rx_clnt_params(hal_get_channel_number(hp), &clnt_params);
    }

    return RC_NORMAL;
}


