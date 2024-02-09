#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "ntp_menu_gen.h"

#define NTP_TEMPLATE_FILE "/home/onair/templates/ntp.conf"
#define NTP_CONF_FILE "/home/onair/ntp.conf"

static  int validateNTPClient(struct ntp_params *params,
                             struct ntp_params *orig_params,
                             char *error_string)
{

    if (params->enable_value && params->ipaddress_set && 
        (strlen(params->ipaddress_value) == 0)) {
        sprintf(error_string, "Server IP address not defined");
        return RC_INVALID_VALUE;
    }
    return RC_NORMAL;
}

int applyNTPClient(struct ntp_params *params, 
                   struct ntp_params *orig_params)
{
    char buff[128];
    int retcode;

    if (params->enable_value) {
        if (verbose_errors) {
            printf("Configuring NTP client with Server %s\n", 
                   params->ipaddress_value);
        }
        sprintf(buff, 
                "/bin/sed -e 's/NTPSERVERIP/%s/' %s > %s",
                params->ipaddress_value,
                NTP_TEMPLATE_FILE,
                NTP_CONF_FILE);
                
        retcode = system_direct_command(buff);
        if (retcode != 0 && verbose_errors) {
            printf("Could not setup NTP configuration\n");
        }
                   
    } else {
        sprintf(buff, "/bin/rm -f %s", NTP_CONF_FILE);
        retcode = system_command(buff);
    }

    return RC_NORMAL;
}


void NTPClientInit(void)
{

    registerNTPClientValidateFunction(validateNTPClient);

}
