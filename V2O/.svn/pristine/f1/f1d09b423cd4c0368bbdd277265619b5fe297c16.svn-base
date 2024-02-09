

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "if_list_menu_gen.h"
#include "host_list_menu_gen.h"
#include "linuxha_menu_gen.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"

char * constructHeaderIFList(char * buffer) {
    sprintf(buffer, " %-4s %-4s %-4s %-4s %-4s %-15s %-15s",
            "LAN", "EN", "Pri", "Mul", "DHCP", "Address", "NetMask");
    return buffer;
}

char * constructRowIFList(char *buffer,
                                 struct if_list_params *params) {

    sprintf(buffer, " %-4s %-4s %-4s %-4s %-4s %-15s %-15s",
            params->port,
            params->enable,
            params->primary,
            params->multicast,
            params->usedhcp,
            params->ipaddress_value,
            params->netmask_value);


    return tempbuf;
}

static int check_interface_in_use(struct if_list_params *paramsp)
{

    struct ha_params haparams;
    retrieveHAValues(&haparams);

    int in_use = 0;

    foreach_if_list_entry(paramsp) {
        if (haparams.enable_value) {
            if (paramsp->enable_value) {
                if (haparams.interface_value == 
                        (enum ha_interface) paramsp->port_value)
                    in_use = 1;
            }
        }
    }
    return in_use;
}

int validateIFList(int req_code,
                          struct if_list_params *params,
                          struct if_list_params *orig_params,
                          char * error_string) {

    struct if_list_params *paramsp;

    // Validate delete request
    // Also check if we are trying to delete a list used by HA
    if (req_code == DEL_REQUEST) {
        if (check_interface_in_use(params)) {
            sprintf (error_string, "Cannot delete interface,it is being used by HA");
            return RC_ERROR;
        }
        return RC_NORMAL;
    }

    //Validate if modify request a list which is being used by HA
    if (req_code == MOD_REQUEST) {
        if (check_interface_in_use(params)) {
            sprintf (error_string, "Cannot modify interface,it is being used by HA");
            return RC_ERROR;
        }
    }

    // Check for duplicate
    foreach_if_list_entry(paramsp) {
        if (req_code == MOD_REQUEST &&
           paramsp->if_list_id == params->if_list_id) {
           // if modify request, skip self
           continue;
        }
        if (params->port_set == 1) {
            if (paramsp->port_value == params->port_value) {
                sprintf(error_string, "Duplicate: LAN \'%s\' conflicts.",
                    params->port);
                return RC_DUPLICATE;
            }
        }
        if (params->primary_set == 1) {
            if ((params->primary_value) &&
                (paramsp->primary_value == params->primary_value)) {
                sprintf(error_string, "Duplicate: Primary \'%s\' conflicts.",
                    params->primary);
                return RC_DUPLICATE;
            }
        }
        if (params->ipaddress_set == 1) {
            if ((strcmp(paramsp->ipaddress_value , params->ipaddress_value)) == 0 ){
                sprintf(error_string, "Duplicate :IP Address \'%s\' conflicts",
                    params->ipaddress_value);
                return RC_DUPLICATE;
            }
        }

    }

    // Validate add request
    if (req_code == ADD_REQUEST) {
    }

    // Validate add, modify request
    /* Not currently supporting DHCP */
    /*
    if (params->usedhcp_value) {
        sprintf(error_string, "DHCP Client functionality not yet supported");
        return RC_INVALID_VALUE;
    }
    */

    return RC_NORMAL;
}


// Apply for IF lists
int applyIFList(int req_code,
                       struct if_list_params *params,
                       struct if_list_params *orig_params) {

    char buff[128];
    int retcode=0;
    struct stat dhcpcstat;
    int retries;
    int bDhcpDown=0;
    int bDhcpUp=0;
    int bIfUp=0;
    int bIfDown=0;
    int bIfMulticastDown=0;
    int bIfMulticastUp=0;
    int isLinkLocal = 0;

    switch (req_code) {
        case DEL_REQUEST:
            if (orig_params->enable_value) {
                if (orig_params->usedhcp_value) {
                    bDhcpDown=1;
                }
                bIfDown=1;
            }
            break;
        case MOD_REQUEST:
            if (params->enable_value) {
                bDhcpDown=1;
                if (params->usedhcp_value) {
                    if ((params->usedhcp_set) ||
                           (params->enable_set)) {
                        bDhcpUp=1;
                    }
                    else {
                        bDhcpDown=1;
                    }
                }
                else {
                    bIfUp=1;
                }
                if (params->multicast_value) {
                    bIfMulticastUp=1;
                }
                else {
                    bIfMulticastDown=1;
                }
            }
            else {
                bDhcpDown=1;
                bIfDown=1;
            }
            break;
        case ADD_REQUEST:
            if (params->enable_value) {
                if (params->usedhcp_value) {
                    bDhcpUp=1;
                }
                else {
                    bIfUp=1;
                }
                if (params->multicast_value) {
                    bIfMulticastUp=1;
                }
            }
            break;
        default:
            break;
    }

    // If delete request, only orig_parm is available,
    // we shadow params to orig_params to make rest of the code simpler
    if (req_code == DEL_REQUEST)
        params = orig_params;

    // Is the configured IP address a link local address?
    if (strncmp(params->ipaddress_value, "169.254.", 8) == 0)
        isLinkLocal = 1;

    // Mark primary interface in a status file
    if (params->enable_value && params->primary_value && req_code != DEL_REQUEST) {
        sprintf(buff, "/bin/touch /var/status/primary_if_eth%d", params->port_value);
        system_command(buff);
    }
    else {
        sprintf(buff, "/bin/rm -f /var/status/primary_if_eth%d", params->port_value);
        system_command(buff);
    }

    if (bDhcpDown) {
        sprintf(buff, "/tmp/udhcpc.eth%d", orig_params->port_value);
        if  (stat(buff, &dhcpcstat) == 0) {
            sprintf(buff, "kill `cat /tmp/udhcpc.eth%d`",
                    orig_params->port_value);
            retcode = system_command(buff);
            sprintf(buff, "/tmp/udhcpc.eth%d", orig_params->port_value);
            // Now wait for dhcpc to die with a retry count
            for (retries=5; retries >0; retries--) {
                if (stat(buff, &dhcpcstat)) break;
                    sleep (1);
            }
        }
    }
    if (bIfDown) {
        sprintf(buff, "/sbin/avahi-autoipd --kill eth%d", params->port_value);
        retcode = system_command(buff);

        sprintf(buff, "/sbin/ifconfig eth%d down",
                orig_params->port_value);
        retcode = system_command(buff);
    }
    if (bDhcpUp) {
        sprintf(buff, "/sbin/udhcpc -i eth%d -p /tmp/udhcpc.eth%d -R -b "
                "-s /V2O/bin/dhcpc_event",
                params->port_value, params->port_value);
        retcode = system_command(buff);
        if (retcode != 0 && verbose_errors) {
            printf("Could not set %s IP.\n", params->port);
        }
    }
    if (bIfUp) {
        if (!isLinkLocal) {
            sprintf(buff, "/sbin/avahi-autoipd --kill eth%d", params->port_value);
            retcode = system_command(buff);

            sprintf(buff, "/sbin/ifconfig eth%d %s netmask %s",
                    params->port_value,
                    params->ipaddress_value, params->netmask_value);
            retcode = system_command(buff);
            if (retcode != 0 && verbose_errors) {
                printf("Could not set %s IP.\n", params->port);
            }
        }
        else {
            sprintf(buff, "/sbin/ifconfig eth%d 0.0.0.0 down", params->port_value);
            retcode = system_command(buff);

            sprintf(buff, "/sbin/avahi-autoipd --kill eth%d", params->port_value);
            retcode = system_command(buff);

            sprintf(buff, "/sbin/avahi-autoipd --no-chroot -wD eth%d", params->port_value);
            retcode = system_command(buff);
        }
    }
    if (bIfUp || bDhcpUp) {
        /* Set tx queuelength on the interface */
        sprintf (buff, "/sbin/rh_ifconfig eth%d txqueuelen 10000 mtu %d",
                 params->port_value, params->mtu_value);
        retcode = system_command(buff);
        sprintf (buff,
                "/sbin/route add -net 224.0.0.0 netmask 240.0.0.0"
                " dev eth%d", params->port_value);
        retcode = system_command(buff);
        // This has to change to invoke the route list entry
        // loadStaticRouteList();
        system_command ("loadnet -r");
    }
    if (bIfMulticastUp) {
        sprintf (buff,
                "/sbin/route add -net 224.0.0.0 netmask 240.0.0.0"
                " dev eth%d", params->port_value);
        retcode = system_command(buff);
    }
    if (bIfMulticastDown) {
        sprintf (buff,
                "/sbin/route del -net 224.0.0.0 netmask 240.0.0.0"
                " dev eth%d", params->port_value);
        retcode = system_command(buff);
    }
    return RC_NORMAL;
}

void IFListInit(){
    registerIFListSavedFunction(WriteHostFile);
}


