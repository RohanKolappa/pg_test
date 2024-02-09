#define CUSTOM_MENU_DEFS
#define MAX_DEBUG_REG 14
#include "custmenu.h"

#include "hal.h"
#include "debug_regII_status_menu_gen.h"
#include "debug_reg_parms_menu_gen.h"


int getDebugRegIIStatus(struct debug_regII_status_params *debug_cfg) 
{

    HAL *hp = debug_cfg->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct debug_reg_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveDebugRegParametersValues(&params);

    long debugvalue = 0;
    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_15_value);
    sprintf(debug_cfg->reg15_value, "REG 0x%03x: 0x%08lx", params.debug_reg_15_value, debugvalue);
    debug_cfg->avgreg15_value = debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_16_value);
    sprintf(debug_cfg->reg16_value, "REG 0x%03x: 0x%08lx", params.debug_reg_16_value, debugvalue);
    debug_cfg->avgreg16_value = debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_17_value);
    sprintf(debug_cfg->reg17_value, "REG 0x%03x: 0x%08lx", params.debug_reg_17_value, debugvalue);
    debug_cfg->avgreg17_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_18_value);
    sprintf(debug_cfg->reg18_value, "REG 0x%03x: 0x%08lx", params.debug_reg_18_value, debugvalue);
    debug_cfg->avgreg18_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_19_value);
    sprintf(debug_cfg->reg19_value, "REG 0x%03x: 0x%08lx", params.debug_reg_19_value, debugvalue);
    debug_cfg->avgreg19_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_20_value);
    sprintf(debug_cfg->reg20_value, "REG 0x%03x: 0x%08lx", params.debug_reg_20_value, debugvalue);
    debug_cfg->avgreg20_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_21_value);
    sprintf(debug_cfg->reg21_value, "REG 0x%03x: 0x%08lx", params.debug_reg_21_value, debugvalue);
    debug_cfg->avgreg21_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_22_value);
    sprintf(debug_cfg->reg22_value, "REG 0x%03x: 0x%08lx", params.debug_reg_22_value, debugvalue);
    debug_cfg->avgreg22_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_23_value);
    sprintf(debug_cfg->reg23_value, "REG 0x%03x: 0x%08lx", params.debug_reg_23_value, debugvalue);
    debug_cfg->avgreg23_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_24_value);
    sprintf(debug_cfg->reg24_value, "REG 0x%03x: 0x%08lx", params.debug_reg_24_value, debugvalue);
    debug_cfg->avgreg24_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_25_value);
    sprintf(debug_cfg->reg25_value, "REG 0x%03x: 0x%08lx", params.debug_reg_25_value, debugvalue);
    debug_cfg->avgreg25_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_26_value);
    sprintf(debug_cfg->reg26_value, "REG 0x%03x: 0x%08lx", params.debug_reg_26_value, debugvalue);
    debug_cfg->avgreg26_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_27_value);
    sprintf(debug_cfg->reg27_value, "REG 0x%03x: 0x%08lx", params.debug_reg_27_value, debugvalue);
    debug_cfg->avgreg27_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_28_value);
    sprintf(debug_cfg->reg28_value, "REG 0x%03x: 0x%08lx", params.debug_reg_28_value, debugvalue);
    debug_cfg->avgreg28_value =  debugvalue;

    return RC_NORMAL;
}
