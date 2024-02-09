#define CUSTOM_MENU_DEFS
#include "vlog.h"
#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "kbm_control.h"
#include "tx_kbm_params_menu_gen.h"
#include "tx_kbm_config.h"
#include "strmsrv_comm.h"
#include "kbms_utils.h"

int get_tx_kbm_config(HAL *hp, struct tx_kbm_config *kbm_cfg) {

    struct tx_kbm_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveTxKBMParametersValues(&params);

    kbm_cfg->keyboard_type = KB_TYPE_PC;
    kbm_cfg->mouse_type = MS_TYPE_IMPS2;

    if (params.keyboard_type_value == TX_KBM_PARM_KEYBOARD_TYPE_SGI)
        kbm_cfg->keyboard_type = KB_TYPE_SGI;

    if (params.mouse_type_value == TX_KBM_PARM_MOUSE_TYPE_PS2)
        kbm_cfg->mouse_type = MS_TYPE_PS2;

    return 0;
}

int applyTxKBMParameters(struct tx_kbm_parm_params *params,
                         struct tx_kbm_parm_params *orig_params)
{

    HAL *hp = params->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_tx_kbm_params tx_kbm_params;
    int set_tx_kbm_params = 0;
    
    memset(&tx_kbm_params, 0x00, sizeof(tx_kbm_params));

    if (params->keyboard_type_set == 1) {
        tx_kbm_params.kbtype_set = 1;
        if (params->keyboard_type_value == TX_KBM_PARM_KEYBOARD_TYPE_SGI)
            tx_kbm_params.kbtype = KB_TYPE_SGI;
        else
            tx_kbm_params.kbtype = KB_TYPE_PC;
        set_tx_kbm_params = 1;
    }

    if (params->mouse_type_set == 1) {
        tx_kbm_params.mstype_set = 1;
        if (params->mouse_type_value == TX_KBM_PARM_MOUSE_TYPE_PS2)
            tx_kbm_params.mstype = MS_TYPE_PS2;
        else
            tx_kbm_params.mstype = MS_TYPE_IMPS2;
        set_tx_kbm_params = 1;
    }

    if (set_tx_kbm_params)
        strmsrv_set_tx_kbm_params(hal_get_channel_number(hp), &tx_kbm_params);

    return RC_NORMAL;
}


