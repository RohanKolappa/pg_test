

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "natzone_list_menu_gen.h"

char * constructHeaderNATZoneList(char * buffer) {
    sprintf(buffer, "%-10s %-16s",
            "Name", "IP Address");
    return buffer;
}

char * constructRowNATZoneList(char *buffer, 
                                 struct natzone_list_params *params) {

    sprintf(buffer, "%-10.10s %-16.16s",
            params->natzonename_value,
            params->natzonevalue_value);
 

    return tempbuf;
}


int validateNATZoneList(int req_code,
                          struct natzone_list_params *params,
                          struct natzone_list_params *orig_params,
                          char * error_string) {

    struct natzone_list_params *paramsp;
    
    // Validate delete request
    if (req_code == DEL_REQUEST) {

        return RC_NORMAL;
    }

    // Validate add request
    if (req_code == ADD_REQUEST) {
    }

    // Should not allow change of name during modify
    if (req_code == MOD_REQUEST) {
    }
    
    // Check for duplicate
    foreach_natzone_list_entry(paramsp) {
        VLOG_INFO("Checking for %s %s %d",
                   paramsp->natzonename_value, params->natzonename_value, params->natzonename_set);
        if (params->natzonename_set == 1) {
            // if modify request, skip self
            if (req_code == MOD_REQUEST && 
                paramsp->natzone_list_id == params->natzone_list_id) {
                continue; 
            }
            if (strcmp(paramsp->natzonename_value, params->natzonename_value) == 0) {
                sprintf(error_string, "Duplicate: Name \'%s\' conflicts.",
                    params->natzonename_value);
                return RC_DUPLICATE;
            }
        }

    }

    return RC_NORMAL;
}


// Apply for NAS lists
int applyNATZoneList(int req_code,
                       struct natzone_list_params *params,
                       struct natzone_list_params *orig_params) {

    return RC_NORMAL;
}

