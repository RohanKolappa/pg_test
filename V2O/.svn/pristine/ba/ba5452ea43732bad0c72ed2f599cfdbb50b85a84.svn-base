#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "tx_fpga_reg_status_menu_gen.h"



int getTxFPGARegStatus(struct tx_fpga_reg_status_params *params) 
{

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    params->FPGA_CSREG_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_CSREG);

    params->FPGA_CDCMRREG_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_CDCMRREG);

    params->FPGA_REG250_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG250);

    params->FPGA_REG254_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG254);

    return RC_NORMAL;

}


