#define CUSTOM_MENU_DEFS
#define MAX_DEBUG_REG 14
#include "custmenu.h"

#include "hal.h"
#include "omnitek_reg_status_menu_gen.h"
#include "omnitek_reg_parms_menu_gen.h"


int getOmniTekRegStatus(struct omnitek_reg_status_params *omnitek_cfg) 
{

    HAL *hp = omnitek_cfg->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct omnitek_reg_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveOmniTekRegParametersValues(&params);

    long omnitekvalue = 0;
    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_1_value);
    sprintf(omnitek_cfg->reg1_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_1_value, omnitekvalue);
    omnitek_cfg->avgreg1_value = omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_2_value);
    sprintf(omnitek_cfg->reg2_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_2_value, omnitekvalue);
    omnitek_cfg->avgreg2_value = omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_3_value);
    sprintf(omnitek_cfg->reg3_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_3_value, omnitekvalue);
    omnitek_cfg->avgreg3_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_4_value);
    sprintf(omnitek_cfg->reg4_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_4_value, omnitekvalue);
    omnitek_cfg->avgreg4_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_5_value);
    sprintf(omnitek_cfg->reg5_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_5_value, omnitekvalue);
    omnitek_cfg->avgreg5_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_6_value);
    sprintf(omnitek_cfg->reg6_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_6_value, omnitekvalue);
    omnitek_cfg->avgreg6_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_7_value);
    sprintf(omnitek_cfg->reg7_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_7_value, omnitekvalue);
    omnitek_cfg->avgreg7_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_8_value);
    sprintf(omnitek_cfg->reg8_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_8_value, omnitekvalue);
    omnitek_cfg->avgreg8_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_9_value);
    sprintf(omnitek_cfg->reg9_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_9_value, omnitekvalue);
    omnitek_cfg->avgreg9_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_10_value);
    sprintf(omnitek_cfg->reg10_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_10_value, omnitekvalue);
    omnitek_cfg->avgreg10_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_11_value);
    sprintf(omnitek_cfg->reg11_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_11_value, omnitekvalue);
    omnitek_cfg->avgreg11_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_12_value);
    sprintf(omnitek_cfg->reg12_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_12_value, omnitekvalue);
    omnitek_cfg->avgreg12_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_13_value);
    sprintf(omnitek_cfg->reg13_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_13_value, omnitekvalue);
    omnitek_cfg->avgreg13_value =  omnitekvalue;

    omnitekvalue = hal_get_omnitek_val_k(hp, params.omnitek_reg_14_value);
    sprintf(omnitek_cfg->reg14_value, "REG 0x%03x: 0x%08lx", params.omnitek_reg_14_value, omnitekvalue);
    omnitek_cfg->avgreg14_value =  omnitekvalue;

    return RC_NORMAL;
}
