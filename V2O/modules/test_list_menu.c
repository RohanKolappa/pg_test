#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "test_list_menu_gen.h"


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

char * constructHeaderTestListConfig(char * buffer) {
    sprintf(buffer, "%-12s %-12s %8s %8s %8s",
            "first_name", "last_name", "height", "weight", "samid");
    return buffer;
}

char * constructRowTestListConfig(char *buffer, 
                                    struct test_list_params *params) {

    check_hal(params->userptr, "Validate Construct Row");
    
 //   char buf[40];
    sprintf(buffer, "%-14s %-14s %-6.1f %-6.1f %5d ",
            params->first_name_value,
            params->last_name_value,
            params->height_value, 
            params->weight_value,
            params->samid_value);
 

    return buffer;
}

int validateTestListConfig(int req_code,
                          struct test_list_params *params,
                          struct test_list_params *orig_params,
                          char * error_string) {

  
    struct test_list_params *paramsp;

        // Validate add request
    if (req_code == ADD_REQUEST) {
        foreach_test_list_entry(paramsp) {
           if (paramsp->samid_value == params->samid_value) {
               sprintf(error_string, "Duplicate: SAMID \'%d\' conflicts.",
                        params->samid_value);
                return RC_DUPLICATE;
            }
        }
    }
 
      // Should not allow change of SAMID during modify
    if (req_code == MOD_REQUEST) {
        if ((params->samid_set == 1) &&
            (params->samid_value != orig_params->samid_value)) {
             sprintf(error_string, "Cannot modify ID");
             return RC_ERROR;
        }
    }


    // Validate delete request
    if (req_code == DEL_REQUEST) {

        if ((strcmp(orig_params->first_name_value, "pqr") == 0)) {
            sprintf(error_string, "Be careful about deleting entry named \"pqr\".");
            return RC_WARNING;
        }

        check_hal(orig_params->userptr, "Validate Delete");
        return RC_NORMAL;
    }


    // Validate add request
    if (req_code == ADD_REQUEST) {
        check_hal(params->userptr, "Validate Add");
        if ((params->first_name_set == 0) || 
            (strcmp(params->first_name_value, "") == 0)) {
            sprintf(error_string, "Need to specify first name.");
            return RC_ERROR;
        }
    }

    if (req_code == MOD_REQUEST) {
        check_hal(params->userptr, "Validate Mod");
        check_hal(orig_params->userptr, "Validate Mod Orig");
    } 

    // Check for duplicate
    foreach_test_list_entry(paramsp) {
        check_hal(paramsp->userptr, "Validate Foreach");
        if (params->first_name_set == 1 && params->last_name_set ==1) {
            // if modify request, skip self
            if (req_code == MOD_REQUEST && 
                paramsp->test_list_id == params->test_list_id) {
                continue; 
            }
            if (strcmp(paramsp->first_name_value, params->first_name_value) == 0) {
                if (strcmp(paramsp->last_name_value, params->last_name_value) == 0 ) {
                sprintf(error_string, "Duplicate: Name \'%s\' \'%s\' conflicts.",
                params->first_name_value, params->last_name_value);
                return RC_DUPLICATE;
                }
            }
        }
    }
    return RC_NORMAL;
}


// Apply for Test list
int applyTestListConfig(int req_code,
                       struct test_list_params *params,
                       struct test_list_params *orig_params) {


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
static void TestDefaultsInitFunction(struct test_list_params
                                                  *params) 
{

    HAL *hp = params->userptr;

    if (hp == NULL)
        return;
    retrieveTestListConfigValues(hp);
 
    check_hal(hp, "Function Call");
 
    strcpy(params->full_name_value, "");
    params->full_name_set = 1;
    strcpy(params->full_name, params->first_name);
    strcat(params->full_name, " ");
    strcat(params->full_name, params->last_name);
    strcpy(params->full_name_value, params->full_name);
    params->full_name_set = 1;
}

void TestListConfigInit()
{
    registerTestListConfigParamsDefaultsInitFunction(TestDefaultsInitFunction);
}


