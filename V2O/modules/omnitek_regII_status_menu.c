#define CUSTOM_MENU_DEFS
#define MAX_DEBUG_REG 14
#include "custmenu.h"

#include "hal.h"
#include "omnitek_regII_status_menu_gen.h"
#include "omnitek_reg_parms_menu_gen.h"


int getOmniTekRegIIStatus(struct omnitek_regII_status_params *omnitek_cfg) 
{

    HAL *hp = omnitek_cfg->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct omnitek_reg_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveOmniTekRegParametersValues(&params);

    long omnitekvalue = 0;
    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_15_value);
    sprintf(omnitek_cfg->reg15_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_15_value, omnitekvalue);
    omnitek_cfg->avgreg15_value = omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_16_value);
    sprintf(omnitek_cfg->reg16_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_16_value, omnitekvalue);
    omnitek_cfg->avgreg16_value = omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_17_value);
    sprintf(omnitek_cfg->reg17_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_17_value, omnitekvalue);
    omnitek_cfg->avgreg17_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_18_value);
    sprintf(omnitek_cfg->reg18_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_18_value, omnitekvalue);
    omnitek_cfg->avgreg18_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_19_value);
    sprintf(omnitek_cfg->reg19_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_19_value, omnitekvalue);
    omnitek_cfg->avgreg19_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_20_value);
    sprintf(omnitek_cfg->reg20_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_20_value, omnitekvalue);
    omnitek_cfg->avgreg20_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_21_value);
    sprintf(omnitek_cfg->reg21_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_21_value, omnitekvalue);
    omnitek_cfg->avgreg21_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_22_value);
    sprintf(omnitek_cfg->reg22_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_22_value, omnitekvalue);
    omnitek_cfg->avgreg22_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_23_value);
    sprintf(omnitek_cfg->reg23_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_23_value, omnitekvalue);
    omnitek_cfg->avgreg23_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_24_value);
    sprintf(omnitek_cfg->reg24_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_24_value, omnitekvalue);
    omnitek_cfg->avgreg24_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_25_value);
    sprintf(omnitek_cfg->reg25_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_25_value, omnitekvalue);
    omnitek_cfg->avgreg25_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_26_value);
    sprintf(omnitek_cfg->reg26_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_26_value, omnitekvalue);
    omnitek_cfg->avgreg26_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_27_value);
    sprintf(omnitek_cfg->reg27_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_27_value, omnitekvalue);
    omnitek_cfg->avgreg27_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_28_value);
    sprintf(omnitek_cfg->reg28_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_28_value, omnitekvalue);
    omnitek_cfg->avgreg28_value =  omnitekvalue;

    return RC_NORMAL;
}
