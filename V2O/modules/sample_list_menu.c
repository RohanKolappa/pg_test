#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "sample_list_menu_gen.h"


static void check_hal(HAL *hp, const char * who) {
    if (hp == NULL) {
        VLOG_INFO("%s Could not get HAL Ptr", who);
    }
    else {
        VLOG_INFO("%s Got HAL: %d, %d", 
                   who,
                   hal_get_board_number(hp),
                   hal_get_number_of_boards(hp));
        
        
    }
}

char * constructHeaderSampleList(char * buffer) {
    sprintf(buffer, "%-10s %-32s",
            "Name", "Info");
    return buffer;
}

char * constructRowSampleList(char *buffer, 
                                    struct sample_list_params *params) {

    check_hal(params->userptr, "Validate Construct Row");
    
    char buf[40];
    sprintf(buf, "%d/%d %s", 
            params->video_port_value,
            params->audio_port_value,
            params->enable_value ? "*" : " ");
    sprintf(buffer, "%-10.10s %-32.32s",
            params->name_value, buf);
 

    return tempbuf;
}



int validateSampleList(int req_code,
                          struct sample_list_params *params,
                          struct sample_list_params *orig_params,
                          char * error_string) {

  
    struct sample_list_params *paramsp;
    
    // Validate delete request
    if (req_code == DEL_REQUEST) {

        if ((strcmp(orig_params->name_value, "pqr") == 0)) {
            sprintf(error_string, "Be careful about deleting entry named \"pqr\".");
            return RC_WARNING;
        }

        check_hal(orig_params->userptr, "Validate Delete");
        return RC_NORMAL;
    }



    // Validate add request
    if (req_code == ADD_REQUEST) {
        check_hal(params->userptr, "Validate Add");
        if ((params->name_set == 0) || 
            (strcmp(params->name_value, "") == 0)) {
            sprintf(error_string, "Need to specify name.");
            return RC_ERROR;
        }

    }

    if (req_code == MOD_REQUEST) {
        check_hal(params->userptr, "Validate Mod");
        check_hal(orig_params->userptr, "Validate Mod Orig");
    } 

    // Check for duplicate
    foreach_sample_list_entry(paramsp) {
        check_hal(paramsp->userptr, "Validate Foreach");
        if (params->name_set == 1) {
            // if modify request, skip self
            if (req_code == MOD_REQUEST && 
                paramsp->sample_list_id == params->sample_list_id) {
                continue; 
            }
            if (strcmp(paramsp->name_value, params->name_value) == 0) {
                sprintf(error_string, "Duplicate: Name \'%s\' conflicts.",
                    params->name_value);
                return RC_DUPLICATE;
            }
        }
    }

    if (params->audio_port_set) {
        if (params->audio_port_value == 5678) {
            sprintf(error_string, "Dangerous to use this value for port.");
            return RC_WARNING;
        }
    }

    return RC_NORMAL;
}


// Apply for Sample list
int applySampleList(int req_code,
                       struct sample_list_params *params,
                       struct sample_list_params *orig_params) {


    if (req_code == DEL_REQUEST) {
        check_hal(orig_params->userptr, "Apply Delete");
    }

    if (req_code == ADD_REQUEST) {
        check_hal(params->userptr, "Apply Add");
    }

    if (req_code == MOD_REQUEST) {
        check_hal(params->userptr, "Apply Mod");
        check_hal(orig_params->userptr, "Apply Mod Orig");
    } 

    return RC_NORMAL;
}

static void myDefaultsInitFunction(struct sample_list_params
                                                  *params)
{
    struct sample_list_params *paramsp;
    HAL *hp = params->userptr;

    if (hp == NULL)
        return;

    params->audio_port_set = 1;
    params->audio_port_value = 6666;

    retrieveSampleListValues(hp);

  again:
    foreach_sample_list_entry(paramsp) {
        if (paramsp->audio_port_value == params->audio_port_value) {
            params->audio_port_value++;
            goto again;
        }
    }
}


void SampleListInit()
{
    registerSampleListParamsDefaultsInitFunction(myDefaultsInitFunction);
}

