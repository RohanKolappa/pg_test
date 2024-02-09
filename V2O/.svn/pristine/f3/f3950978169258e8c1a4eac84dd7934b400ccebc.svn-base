#define CUSTOM_MENU_DEFS
#define MAX_DEBUG_REG 14
#include "custmenu.h"

#include "hal.h"
#include "s6_regII_status_menu_gen.h"
#include "s6_reg_parms_menu_gen.h"


int getS6RegIIStatus(struct s6_regII_status_params *s6_cfg) 
{

    HAL *hp = s6_cfg->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    struct s6_reg_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveS6RegParametersValues(&params);

    long s6value = 0;
    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_15_value);
    sprintf(s6_cfg->reg15_value, "REG 0x%03x: 0x%08lx", params.s6_reg_15_value, s6value);
    s6_cfg->avgreg15_value = s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_16_value);
    sprintf(s6_cfg->reg16_value, "REG 0x%03x: 0x%08lx", params.s6_reg_16_value, s6value);
    s6_cfg->avgreg16_value = s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_17_value);
    sprintf(s6_cfg->reg17_value, "REG 0x%03x: 0x%08lx", params.s6_reg_17_value, s6value);
    s6_cfg->avgreg17_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_18_value);
    sprintf(s6_cfg->reg18_value, "REG 0x%03x: 0x%08lx", params.s6_reg_18_value, s6value);
    s6_cfg->avgreg18_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_19_value);
    sprintf(s6_cfg->reg19_value, "REG 0x%03x: 0x%08lx", params.s6_reg_19_value, s6value);
    s6_cfg->avgreg19_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_20_value);
    sprintf(s6_cfg->reg20_value, "REG 0x%03x: 0x%08lx", params.s6_reg_20_value, s6value);
    s6_cfg->avgreg20_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_21_value);
    sprintf(s6_cfg->reg21_value, "REG 0x%03x: 0x%08lx", params.s6_reg_21_value, s6value);
    s6_cfg->avgreg21_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_22_value);
    sprintf(s6_cfg->reg22_value, "REG 0x%03x: 0x%08lx", params.s6_reg_22_value, s6value);
    s6_cfg->avgreg22_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_23_value);
    sprintf(s6_cfg->reg23_value, "REG 0x%03x: 0x%08lx", params.s6_reg_23_value, s6value);
    s6_cfg->avgreg23_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_24_value);
    sprintf(s6_cfg->reg24_value, "REG 0x%03x: 0x%08lx", params.s6_reg_24_value, s6value);
    s6_cfg->avgreg24_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_25_value);
    sprintf(s6_cfg->reg25_value, "REG 0x%03x: 0x%08lx", params.s6_reg_25_value, s6value);
    s6_cfg->avgreg25_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_26_value);
    sprintf(s6_cfg->reg26_value, "REG 0x%03x: 0x%08lx", params.s6_reg_26_value, s6value);
    s6_cfg->avgreg26_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_27_value);
    sprintf(s6_cfg->reg27_value, "REG 0x%03x: 0x%08lx", params.s6_reg_27_value, s6value);
    s6_cfg->avgreg27_value =  s6value;

    s6value = hal_get_s6fpga_val32_k(hp, params.s6_reg_28_value);
    sprintf(s6_cfg->reg28_value, "REG 0x%03x: 0x%08lx", params.s6_reg_28_value, s6value);
    s6_cfg->avgreg28_value =  s6value;

    return RC_NORMAL;
}
