/*
 * if_list_config.c
 *
 *  Created on: Oct 6, 2009
 *      Author: rkale
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "string.h"

#include "if_list_menu_gen.h"
#include "if_list_config.h"
#include "commonutils.h"
#include "system_utils.h"

// Retrieves current network interface params
int get_if_list_config(struct if_list_config *if_list_cfg)
{
    struct if_list_params *paramsp;
    int count = 0;
    retrieveIFListValues();

    foreach_if_list_entry(paramsp) {
        if_list_cfg->interface[count].enable = paramsp->enable_value;
        if_list_cfg->interface[count].primary = paramsp->primary_value;
        if_list_cfg->interface[count].multicast = paramsp->multicast_value;
        if_list_cfg->interface[count].usedhcp = paramsp->usedhcp_value;
        strcpy(if_list_cfg->interface[count].ipaddress,
                paramsp->ipaddress_value);
        strcpy(if_list_cfg->interface[count].netmask, paramsp->netmask_value);
        switch (paramsp->port_value) {
        case IF_LIST_PORT_1:
            if_list_cfg->interface[count].port = 1;
            break;
        case IF_LIST_PORT_2:
            if_list_cfg->interface[count].port = 2;
            break;
        case IF_LIST_PORT_3:
            if_list_cfg->interface[count].port = 3;
            break;
        default:
            if_list_cfg->interface[count].port = 1;
        }

        // If using DHCP, find the discovered ip adressses and netmasks
        if (paramsp->enable_value && paramsp->usedhcp_value == 1) {
            char ifname[32];
            sprintf(ifname, "eth%d", if_list_cfg->interface[count].port - 1);
            get_interface_info(ifname,
                    if_list_cfg->interface[count].ipaddress,
                    if_list_cfg->interface[count].netmask);
        }
        if (++count >= MAX_NUMBER_OF_INTERFACES)
            break;
    }
    if_list_cfg->number_of_interfaces = count;
    return 0;
}

// Find out if DHCP is enabled on the primary interface
int is_dhcp_enabled(void) {
    struct if_list_config if_list_cfg;
    int i;
    get_if_list_config(&if_list_cfg);
    for (i = 0; i < if_list_cfg.number_of_interfaces; i++) {
        if (if_list_cfg.interface[i].enable == 1 &&
                if_list_cfg.interface[i].primary == 1 &&
                if_list_cfg.interface[i].usedhcp == 1) {
            return 1;
        }
    }
    return 0;
}

int is_primary_interface(char * interface) {

    struct stat statbuff;
    char filename[128];
    sprintf(filename, "/var/status/primary_if_%s", interface);
    if  (stat(filename, &statbuff) == 0) {
        return 1;
    }
    return 0;
}
