#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "rx_fpga_reg_status_menu_gen.h"



int getRxFPGARegStatus(struct rx_fpga_reg_status_params *params) 
{

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;
    
    params->FPGA_CSREG_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_CSREG);

    params->FPGA_CDCMRREG_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_CDCMRREG);

    params->FPGA_REG258_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG258);

    params->FPGA_REG25C_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG25C);

    params->FPGA_REG260_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG260);

    params->FPGA_REG264_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG264);

    params->FPGA_REG268_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_REG268);

    params->FPGA_DGLKSTATUS_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_DGLKSTATUS);

    params->FPGA_DGLKSTATUS2_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_DGLKSTATUS2);

    return RC_NORMAL;

}


