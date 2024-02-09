

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "static_route_list_menu_gen.h"



char * constructRowStaticRouteList(char *buffer,
                                   struct route_list_params *routeentryp) {
    if(routeentryp != NULL) {
        sprintf(buffer, "%-16s %-16s %-16s",
                routeentryp->destination_value,
                routeentryp->gateway_value,
                routeentryp->netmask_value);
        }
    return buffer;
}

char * constructHeaderStaticRouteList(char *buffer) {
    sprintf(buffer, "%-16s %-16s %-16s",
            "Destination", "Netmask", "Gateway");
    return buffer;
}

int validateStaticRouteList(int req_code,
                            struct route_list_params *params,
                            struct route_list_params *orig_params,
                            char * error_string) {

    return RC_NORMAL;
}

/* This generated menu is not used yet. Route list is configured using legacy
 * manual code. */
int applyStaticRouteList(int req_code,
                         struct route_list_params *params,
                         struct route_list_params *orig_params) {

    return RC_NORMAL;
}
