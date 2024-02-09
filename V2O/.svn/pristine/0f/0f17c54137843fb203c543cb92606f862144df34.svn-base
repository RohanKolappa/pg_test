#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "hal_hwinfo.h"
#include "ad9852dds.h"
#include "rx_dropside_reg_status_menu_gen.h"



int getRxDropSideRegStatus(struct rx_dropside_reg_status_params *params) 
{

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    params->FPGA_DCREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DCREG);

    params->FPGA_DSREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DSREG);

    params->FPGA_DVHAREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVHAREG);

    params->FPGA_DVVAREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVVAREG);

    params->FPGA_DVHSREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVHSREG);

    params->FPGA_DVVSREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVVSREG);

    params->FPGA_DVHFPREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVHFPREG);

    params->FPGA_DVVFPREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVVFPREG);

    params->FPGA_DVHBPREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVHBPREG);

    params->FPGA_DVVBPREG_value = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DVVBPREG);
    if(hal_get_hardware_type(hp) == PLX_DEVICE) {
        unsigned long fifolev;
        fifolev = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_DLFLREG);
        params->FPGA_DLFLREG_AUDIO_value = (unsigned short) ((fifolev & 0xffff0000) >> 16);
        params->FPGA_DLFLREG_value = (unsigned short) (fifolev & 0x0000ffff);
    }
    else {
        params->FPGA_DLFLREG_AUDIO_value = 0;
        params->FPGA_DLFLREG_value = hal_get_fpga_val_k((HAL *)params->userptr, FPGA_DLFLREG);
    }

    params->DDS_FREQ_WORD1_01_value = hal_get_ad9852_val_k((HAL *)params->userptr, DDS_FREQ_WORD1_01);

    params->DDS_FREQ_WORD1_02_value = hal_get_ad9852_val_k((HAL *)params->userptr, DDS_FREQ_WORD1_02);

    params->DDS_FREQ_WORD1_03_value = hal_get_ad9852_val_k((HAL *)params->userptr, DDS_FREQ_WORD1_03);

    params->DDS_FREQ_WORD1_04_value = hal_get_ad9852_val_k((HAL *)params->userptr, DDS_FREQ_WORD1_04);

    params->DDS_FREQ_WORD1_05_value = hal_get_ad9852_val_k((HAL *)params->userptr, DDS_FREQ_WORD1_05);

    params->DDS_FREQ_WORD1_06_value = hal_get_ad9852_val_k((HAL *)params->userptr, DDS_FREQ_WORD1_06);

    return RC_NORMAL;

}


