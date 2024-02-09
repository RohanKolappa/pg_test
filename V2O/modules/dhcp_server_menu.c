

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "dhcp_server_menu_gen.h"
#include "if_list_menu_gen.h"

#define DHCPD_TEMPLATE_FILE "/home/onair/templates/udhcpd.conf"
#define DHCPD_CONF_FILE "/home/onair/udhcpd.conf"


static int validateDHCPServer(struct dhcp_server_params *params,
                              struct dhcp_server_params *orig_params,
                              char *error_string)
{
    /* TODO Validate Start and End IP are in the same network */
    /* TODO Validate Start IP is less than End IP*/
    return RC_NORMAL;
}


int applyDHCPServer(struct dhcp_server_params *params, 
                         struct dhcp_server_params *orig_params)
{

    char buff[128];
    struct if_list_params *ifparamsp;


    if (params->enable_value) {
        if (verbose_errors) {
            printf("Configuring DHCP Server\n");
        }

        /* Need to retrieve eth1 Interface parameters */
        retrieveIFListValues();
        // Check for ETH1 interface
        foreach_if_list_entry(ifparamsp) {
            if (ifparamsp->port_value == IF_LIST_PORT_2) break;
        }

        if (ifparamsp == NULL) {
            if (verbose_errors) {
                printf("Invalid Interface\n");
            }
            sprintf(buff, "/bin/rm -f %s", DHCPD_CONF_FILE);
            system_command(buff);
            sprintf(buff, "/usr/bin/killall udhcpd");
            system_command(buff);
            return RC_INVALID_VALUE;
        }

        sprintf(buff, "/bin/cp %s %s", 
                DHCPD_TEMPLATE_FILE, DHCPD_CONF_FILE);
        system_command(buff);

        sprintf(buff, "/bin/sed -i -e 's/STARTIP/%s/' %s", 
                params->start_ip_value, DHCPD_CONF_FILE);
        system_command(buff);
        sprintf(buff, "/bin/sed -i -e 's/ENDIP/%s/' %s", 
                params->end_ip_value, DHCPD_CONF_FILE);
        system_command(buff);
        sprintf(buff, "/bin/sed -i -e 's/DNSSERVERS/%s/' %s", 
                params->dns_server_value, DHCPD_CONF_FILE);
        system_command(buff);
        sprintf(buff, "/bin/sed -i -e 's/GATEWAY/%s/' %s", 
                ifparamsp->ipaddress, DHCPD_CONF_FILE);
        system_command(buff);
        sprintf(buff, "/bin/sed -i -e 's/SUBNET/%s/' %s", 
                ifparamsp->netmask, DHCPD_CONF_FILE);
        system_command(buff);

        sprintf(buff, "/usr/sbin/udhcpd %s", DHCPD_CONF_FILE);
        system_command(buff);
                   
    } else {
        if (verbose_errors) {
            printf("Disabling DHCP Server\n");
        }
        sprintf(buff, "/bin/rm -f %s", DHCPD_CONF_FILE);
        system_command(buff);
        sprintf(buff, "/usr/bin/killall udhcpd");
        system_command(buff);
    }

    return RC_NORMAL;
}

void DHCPServerInit() {
    registerDHCPServerValidateFunction(validateDHCPServer);
}
