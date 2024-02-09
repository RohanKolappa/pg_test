#define CUSTOM_MENU_DEFS
#define MAX_DEBUG_REG 14
#include "custmenu.h"

#include "hal.h"
#include "s6_reg_status_menu_gen.h"
#include "s6_reg_parms_menu_gen.h"


int getS6RegStatus(struct s6_reg_status_params *s6_cfg) 
{

    HAL *hp = s6_cfg->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct s6_reg_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveS6RegParametersValues(&params);

    long s6value = 0;
    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_1_value);
    sprintf(s6_cfg->reg1_value, "REG 0x%03x: 0x%08lx", params.s6_reg_1_value, s6value);
    s6_cfg->avgreg1_value = s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_2_value);
    sprintf(s6_cfg->reg2_value, "REG 0x%03x: 0x%08lx", params.s6_reg_2_value, s6value);
    s6_cfg->avgreg2_value = s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_3_value);
    sprintf(s6_cfg->reg3_value, "REG 0x%03x: 0x%08lx", params.s6_reg_3_value, s6value);
    s6_cfg->avgreg3_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_4_value);
    sprintf(s6_cfg->reg4_value, "REG 0x%03x: 0x%08lx", params.s6_reg_4_value, s6value);
    s6_cfg->avgreg4_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_5_value);
    sprintf(s6_cfg->reg5_value, "REG 0x%03x: 0x%08lx", params.s6_reg_5_value, s6value);
    s6_cfg->avgreg5_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_6_value);
    sprintf(s6_cfg->reg6_value, "REG 0x%03x: 0x%08lx", params.s6_reg_6_value, s6value);
    s6_cfg->avgreg6_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_7_value);
    sprintf(s6_cfg->reg7_value, "REG 0x%03x: 0x%08lx", params.s6_reg_7_value, s6value);
    s6_cfg->avgreg7_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_8_value);
    sprintf(s6_cfg->reg8_value, "REG 0x%03x: 0x%08lx", params.s6_reg_8_value, s6value);
    s6_cfg->avgreg8_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_9_value);
    sprintf(s6_cfg->reg9_value, "REG 0x%03x: 0x%08lx", params.s6_reg_9_value, s6value);
    s6_cfg->avgreg9_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_10_value);
    sprintf(s6_cfg->reg10_value, "REG 0x%03x: 0x%08lx", params.s6_reg_10_value, s6value);
    s6_cfg->avgreg10_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_11_value);
    sprintf(s6_cfg->reg11_value, "REG 0x%03x: 0x%08lx", params.s6_reg_11_value, s6value);
    s6_cfg->avgreg11_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_12_value);
    sprintf(s6_cfg->reg12_value, "REG 0x%03x: 0x%08lx", params.s6_reg_12_value, s6value);
    s6_cfg->avgreg12_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_13_value);
    sprintf(s6_cfg->reg13_value, "REG 0x%03x: 0x%08lx", params.s6_reg_13_value, s6value);
    s6_cfg->avgreg13_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_14_value);
    sprintf(s6_cfg->reg14_value, "REG 0x%03x: 0x%08lx", params.s6_reg_14_value, s6value);
    s6_cfg->avgreg14_value =  s6value;

    return RC_NORMAL;
}
