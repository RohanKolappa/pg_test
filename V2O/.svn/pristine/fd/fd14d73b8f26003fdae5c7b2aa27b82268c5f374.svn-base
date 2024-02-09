#include <stdlib.h> 
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlog.h"
#include "lut_list_menu_gen.h"
#include "lut_utils.h"
#include "override_list_menu.h"
#include "override_utils.h"
#include "edid_list_menu.h"
#include "edid_utils.h"

void LoadLUTListToKernel(int req_code, HAL *hp);

char * constructHeaderLUTList(char * buffer) {
    sprintf(buffer, " %-12s %s  %s %s  %s %s  %s",
            "Name", "Hres", "Vres", "Rfrsh", "PixClk", "Str?", "LutId");
    return buffer;
}

char * constructRowLUTList(char *buffer, 
                           struct lut_list_params *params) {
    params->name_value[12] = 0;

    sprintf(buffer, "%-12s %-5d %-5d %-6.1f %-6.1f %-4s %4d",
        params->name_value,
        params->orig_hres_value,
        params->orig_vres_value,
        params->refresh_value,
        params->pcr_value,
        (params->stereo_value == LUT_LIST_STEREO_YES) ? "Yes" : 
        ((params->stereo_value == LUT_LIST_STEREO_NO) ? "No" : "Both"),
        params->alutreg_value);

    return buffer;
}

int validateLUTList(int req_code,
                    struct lut_list_params *params,
                    struct lut_list_params *orig_params,
                    char * error_string) {

    struct lut_list_params *paramsp;
    
    // Validate add request
    if (req_code == ADD_REQUEST) {
        foreach_lut_list_entry(paramsp) {
            if (paramsp->alutreg_value == params->alutreg_value) {
                sprintf(error_string, "Duplicate: LUTID \'%d\' conflicts.",
                        params->alutreg_value);
                return RC_DUPLICATE;
            }
        }
    }

    // Should not allow change of LUTID during modify
    if (req_code == MOD_REQUEST) {
        if ((params->alutreg_set == 1) &&
            (params->alutreg_value != orig_params->alutreg_value)) {
             sprintf(error_string, "Cannot modify LUTID");
             return RC_ERROR;
        }
    }
    
    // Validate request does not affect override or edid lists
    if (req_code == DEL_REQUEST || req_code == MOD_REQUEST) {
        HAL *hp = orig_params->userptr;
        struct override_list_params ovparams;
        struct edid_list_params edparams;
        int lutid = orig_params->alutreg_value;
        int ovcount, edcount;

        ovcount = retrieveOverrideListValues(hp);

        edcount = retrieveEdidDescriptorListValues(hp);

        if (ovcount > 0 && 
                get_override_list_params_by_lutid(&ovparams, lutid)) {
            strcpy(error_string, "This LUT is used in Override List"); 
            return RC_WARNING;
        }  
        if (edcount > 0 &&
                get_edid_list_params_by_lutid(&edparams, lutid)) {
            strcpy(error_string, "This LUT is used in EDID List"); 
            return RC_WARNING;
        }  
    } 

    return RC_NORMAL;
}

static void initLUTListDefaults(struct lut_list_params *params) {
    HAL *hp = params->userptr;

    params->alutreg_set = 1;
    params->alutreg_value = get_unique_lutid(hp);
}

// Apply request to LUT list
int applyLUTList(int req_code,
                 struct lut_list_params *params,
                 struct lut_list_params *orig_params) {
    HAL *hp;

    if (req_code == DEL_REQUEST || req_code == MOD_REQUEST) {
        hp = orig_params->userptr;
    }
    else {
        hp = params->userptr;
    }

    if (hp == NULL) {
        return RC_ERROR;
    }

    return RC_NORMAL;
}

void LoadLUTListToKernel(int req_code, HAL *hp) {
    char buff[128];

    if (hp) {
        if (hal_get_hardware_type(hp) == V2DI50_DEVICE) {
            strcpy(buff, "/V2O/bin/loadlut");
        }
        else {
            sprintf(buff, "/V2O/bin/loadlut -c %d", hal_get_board_number(hp)+1);
        }
        system_command(buff);
    }
}

void LUTListInit(void) {
    registerLUTListSavedFunction((LUTListSavedFunction *) LoadLUTListToKernel);
    registerLUTListParamsDefaultsInitFunction((LUTListParamsDefaultsInitFunction *) initLUTListDefaults);
}

