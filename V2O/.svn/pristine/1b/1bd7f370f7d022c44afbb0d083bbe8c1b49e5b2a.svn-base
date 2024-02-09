

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "host_list_menu_gen.h"
#include "if_list_menu_gen.h"
#include "dns_menu_gen.h"
#include "loadnet.h"

char * constructHeaderHostList(char * buffer) {
    sprintf(buffer, "%-40s %-16s",
            "Name", "IP Address");
    return buffer;
}

char * constructRowHostList(char *buffer, 
                                 struct host_list_params *params) {

    sprintf(buffer, "%-40.40s %-16.16s",
            params->hostname_value,
            params->hostaddress_value);
 

    return tempbuf;
}


int validateHostList(int req_code,
                          struct host_list_params *params,
                          struct host_list_params *orig_params,
                          char * error_string) {

    struct host_list_params *paramsp;
    
    // Validate delete request
    if (req_code == DEL_REQUEST) {

        return RC_NORMAL;
    }

    // Validate add request
    if (req_code == ADD_REQUEST) {
    }

    // Validate modify request
    if (req_code == MOD_REQUEST) {
    }
    
    // Check for duplicate
    foreach_host_list_entry(paramsp) {
        VLOG_INFO("Checking for %s %s %d",
                   paramsp->hostname_value, params->hostname_value, params->hostname_set);
        if (params->hostname_set == 1) {
            // if modify request, skip self
            if (req_code == MOD_REQUEST && 
                paramsp->host_list_id == params->host_list_id) {
                continue; 
            }
            if (strcmp(paramsp->hostname_value, params->hostname_value) == 0) {
                sprintf(error_string, "Duplicate: Name \'%s\' conflicts.",
                    params->hostname_value);
                return RC_DUPLICATE;
            }
        }

    }

    return RC_NORMAL;
}


// Apply for Host lists
int applyHostList(int req_code,
                       struct host_list_params *params,
                       struct host_list_params *orig_params) {
    return RC_NORMAL;
}



void HostListInit(){
    registerHostListSavedFunction(WriteHostFile);
}


