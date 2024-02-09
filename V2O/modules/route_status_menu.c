#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "route_status_menu_gen.h"

char *constructHeaderRouteStatus(char *buffer)
{

    sprintf(buffer, "%-16s %-16s %-16s",
            "Destination", "Gateway", "Netmask");

    return buffer;
}

char *constructRowRouteStatus(char *buffer,
                              struct route_status_params *params) 
{
    sprintf(buffer, "%-16s %-16s %-16s",
            params->destination_value,
            params->gateway_value,
            params->netmask_value);
            

    return buffer;
}


int getRouteStatus(struct route_status_params *params) 
{
    FILE *inpipe;
    char inbuf[256];
    int lineno = 0;
    char *cp;

    char *command = "/sbin/route -n";

    inpipe = popen(command, "r");
    if (inpipe == NULL) {
        return RC_ERROR;
    }

    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        lineno++;
        if (lineno > 2) {
            params = getRouteStatusParamsStruct(params);

            cp = strtok(inbuf, " \t");
            if (cp == NULL)
                continue;
            strncpy(params->destination_value, cp, 
                    FIELD_ROUTE_STATUS_DESTINATION_LENGTH);

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy(params->gateway_value, cp, 
                    FIELD_ROUTE_STATUS_GATEWAY_LENGTH);

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy(params->netmask_value, cp, 
                    FIELD_ROUTE_STATUS_NETMASK_LENGTH);

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy(params->flags_value, cp, 
                    FIELD_ROUTE_STATUS_FLAGS_LENGTH);

            /* skip metric, ref, use TODO??*/
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;

            cp = strtok(NULL, " \t\n");
            if (cp == NULL)
                continue;
            strncpy(params->interface_value, cp, 
                    FIELD_ROUTE_STATUS_INTERFACE_LENGTH);

        }
    }

    pclose(inpipe);



    return RC_NORMAL;
}
