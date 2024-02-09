#define CUSTOM_MENU_DEFS
#define MAX_DEBUG_REG 14
#include "custmenu.h"

#include "hal.h"
#include "debug_reg_status_menu_gen.h"
#include "debug_reg_parms_menu_gen.h"


int getDebugRegStatus(struct debug_reg_status_params *debug_cfg) 
{

    HAL *hp = debug_cfg->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct debug_reg_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveDebugRegParametersValues(&params);

    long debugvalue = 0;
    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_1_value);
    sprintf(debug_cfg->reg1_value, "REG 0x%03x: 0x%08lx", params.debug_reg_1_value, debugvalue);
    debug_cfg->avgreg1_value = debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_2_value);
    sprintf(debug_cfg->reg2_value, "REG 0x%03x: 0x%08lx", params.debug_reg_2_value, debugvalue);
    debug_cfg->avgreg2_value = debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_3_value);
    sprintf(debug_cfg->reg3_value, "REG 0x%03x: 0x%08lx", params.debug_reg_3_value, debugvalue);
    debug_cfg->avgreg3_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_4_value);
    sprintf(debug_cfg->reg4_value, "REG 0x%03x: 0x%08lx", params.debug_reg_4_value, debugvalue);
    debug_cfg->avgreg4_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_5_value);
    sprintf(debug_cfg->reg5_value, "REG 0x%03x: 0x%08lx", params.debug_reg_5_value, debugvalue);
    debug_cfg->avgreg5_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_6_value);
    sprintf(debug_cfg->reg6_value, "REG 0x%03x: 0x%08lx", params.debug_reg_6_value, debugvalue);
    debug_cfg->avgreg6_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_7_value);
    sprintf(debug_cfg->reg7_value, "REG 0x%03x: 0x%08lx", params.debug_reg_7_value, debugvalue);
    debug_cfg->avgreg7_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_8_value);
    sprintf(debug_cfg->reg8_value, "REG 0x%03x: 0x%08lx", params.debug_reg_8_value, debugvalue);
    debug_cfg->avgreg8_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_9_value);
    sprintf(debug_cfg->reg9_value, "REG 0x%03x: 0x%08lx", params.debug_reg_9_value, debugvalue);
    debug_cfg->avgreg9_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_10_value);
    sprintf(debug_cfg->reg10_value, "REG 0x%03x: 0x%08lx", params.debug_reg_10_value, debugvalue);
    debug_cfg->avgreg10_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_11_value);
    sprintf(debug_cfg->reg11_value, "REG 0x%03x: 0x%08lx", params.debug_reg_11_value, debugvalue);
    debug_cfg->avgreg11_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_12_value);
    sprintf(debug_cfg->reg12_value, "REG 0x%03x: 0x%08lx", params.debug_reg_12_value, debugvalue);
    debug_cfg->avgreg12_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_13_value);
    sprintf(debug_cfg->reg13_value, "REG 0x%03x: 0x%08lx", params.debug_reg_13_value, debugvalue);
    debug_cfg->avgreg13_value =  debugvalue;

    debugvalue = hal_get_fpga_val32_k(hp, params.debug_reg_14_value);
    sprintf(debug_cfg->reg14_value, "REG 0x%03x: 0x%08lx", params.debug_reg_14_value, debugvalue);
    debug_cfg->avgreg14_value =  debugvalue;

    return RC_NORMAL;
}
