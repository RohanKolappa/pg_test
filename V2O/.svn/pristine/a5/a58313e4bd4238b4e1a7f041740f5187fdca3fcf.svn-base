#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "ntp_server_menu_gen.h"

#define NTP_SERVER_TEMPLATE_FILE "/home/onair/templates/ntp_server.conf"
#define NTP_SERVER_CONF_FILE "/home/onair/ntp_server.conf"


int applyNTPServer(struct ntp_server_params *params, 
                          struct ntp_server_params *orig_params)
{
    char buff[128];
    int retcode;

    if (params->enable_value) {
        if (verbose_errors) {
            printf("Configuring NTP server with Server %s\n", 
                   params->ipaddress_value);
        }

        if (strlen(params->ipaddress_value) > 0) {
            /* Server is configured */
            sprintf(buff, 
                    "/bin/sed -e 's/NTPSERVERIP/%s/' %s > %s",
                    params->ipaddress_value,
                    NTP_SERVER_TEMPLATE_FILE,
                    NTP_SERVER_CONF_FILE);
                
            retcode = system_direct_command(buff);
            if (retcode != 0 && verbose_errors) {
                printf("Could not setup NTP configuration\n");
            }
        }
        else {
            /* Server not configured */
            sprintf(buff, 
                    "/bin/sed -e 's/server NTPSERVERIP/#server NTPSERVERIP/' %s > %s",
                    NTP_SERVER_TEMPLATE_FILE,
                    NTP_SERVER_CONF_FILE);
                
            retcode = system_direct_command(buff);
            if (retcode != 0 && verbose_errors) {
                printf("Could not setup NTP configuration\n");
            }
        }
    } else {
        sprintf(buff, "/bin/rm -f %s", NTP_SERVER_CONF_FILE);
        retcode = system_command(buff);
    }

    return RC_NORMAL;
}
