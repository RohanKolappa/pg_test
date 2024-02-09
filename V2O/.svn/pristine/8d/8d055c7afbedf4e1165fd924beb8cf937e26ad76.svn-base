#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "ad9888vif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "tx_addside_reg_status_menu_gen.h"
#include "vlog.h"


int getTxAddSideRegStatus(struct tx_addside_reg_status_params *params) 
{

    HAL *hp = params->userptr;
    int hardware_type;

    if (hp == NULL)
        return RC_ERROR;

    hardware_type = hal_get_hardware_type(hp);

    params->FPGA_ACREG_value = 
	hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_ACREG);

    params->FPGA_ASREG_value = 
	hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_ASREG);

    if (hardware_type == XPI_DEVICE) {
        params->FPGA_AMTREG_value = 
                hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AMDVITREG);
    }
    else if ((hal_get_fpga_val32_k(hp, FPGA_AMTREG) & AMTREG_HVSYNC) == 
					AMTREG_HVSYNC)
        params->FPGA_AMTREG_value = 
		hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AMTREG);
    else if (hardware_type == PLX_DEVICE)
        params->FPGA_AMTREG_value = 
                hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AMDVITREG);

    params->FPGA_AMLREG_value = 
	    hal_getHorizontalLinesVal((HAL *)params->userptr);

    params->FPGA_AMHREG_value = 
	    hal_getHorizontalFrequency((HAL *)params->userptr);

    params->FPGA_AMVREG_value = 
	    hal_getVerticalFrequencyVal((HAL *)params->userptr);

    params->FPGA_AVHAREG_value = 
        hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AVHAREG);

    params->FPGA_AVVAREG_value = 
	    hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AVVAREG);

    params->FPGA_AVHDREG_value = 
	hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AVHDREG);

    params->FPGA_AVVDREG_value = 
	hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AVVDREG);

    if (hardware_type == XPI_DEVICE || hardware_type == PLX_DEVICE ) {
        unsigned long fifolev;
        fifolev = hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_ALFLREG);
        params->FPGA_AAFDREG_value = 
		(unsigned short) ((fifolev & 0xffff0000) >> 16);
        params->FPGA_ALFLREG_value = 
		(unsigned short) (fifolev & 0x0000ffff);
    }
    else {
        params->FPGA_AAFDREG_value = 
		hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_AAFDREG);
        params->FPGA_ALFLREG_value = 
		hal_get_fpga_val32_k((HAL *)params->userptr, FPGA_ALFLREG);
    }

    params->VIF_SYNC_DETECT_value = 
		hal_get_ad9888_val_k((HAL *)params->userptr, VIF_SYNC_DETECT);
    
    params->VIF_SYNC_CNTL_value = 
		hal_get_ad9888_val_k((HAL *)params->userptr, VIF_SYNC_CNTL);

    params->VIF_PLL_MSB_value = 
		hal_get_ad9888_val_k((HAL *)params->userptr, VIF_PLL_MSB);

    params->VIF_PLL_LSB_value = 
		hal_get_ad9888_val_k((HAL *)params->userptr, VIF_PLL_LSB);

    params->VIF_VCO_CPMP_value = 
		hal_get_ad9888_val_k((HAL *)params->userptr, VIF_VCO_CPMP);

    params->VIF_PHASE_ADJ_value = 
		hal_get_ad9888_val_k((HAL *)params->userptr, VIF_PHASE_ADJ);

    return RC_NORMAL;
}


