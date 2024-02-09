/*
 * rfb_connection_menu.c
 *
 *  Created on: Dec 9, 2009
 *      Author: rkale
 */

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "configfile_parser.h"
#include "vlog.h"
#include "rfb_connection_list_menu_gen.h"
#include "rx_connection_status_menu_gen.h"
#include "standalone.h"

char * constructHeaderRFBConnectionList(char * buffer) {
    sprintf(buffer, "%-16s %-8s",
            "IP Address", "Port");
    return buffer;
}

char * constructRowRFBConnectionList(char *buffer,
                                 struct rfb_connection_list_params *params) {

    if (strcmp(params->ipaddress_value, "0.0.0.0") == 0) {
        sprintf(buffer, "Disconnect");
    }
    else {
        sprintf(buffer, "%-16.40s %-8d",
                params->ipaddress_value,
                params->port_value);
    }

    return tempbuf;
}


int validateRFBConnectionList(int req_code,
                          struct rfb_connection_list_params *params,
                          struct rfb_connection_list_params *orig_params,
                          char * error_string) {

//    struct rfb_connection_list_params *paramsp;

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
//    foreach_rfb_connection_list_entry(paramsp) {
//
//    }

    return RC_NORMAL;
}


// Apply for RFB Connection list
int applyRFBConnectionList(int req_code,
                       struct rfb_connection_list_params *params,
                       struct rfb_connection_list_params *orig_params) {
    return RC_NORMAL;
}

static int rfb_ipaddress_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    struct rfb_connection_list_params *params =
        (struct rfb_connection_list_params *) ptr;

    if (flag) {
        if (strcasecmp(name, S_SERVER_ADDRESS) == 0) {
            sprintf(value, "%s", params->ipaddress_value);
            return 1;
        }
    }
    else {
        strcpy(name, S_SERVER_ADDRESS);
        sprintf(value, "%s", params->ipaddress_value);
    }
    return 0;
}

static int rfb_port_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    struct rfb_connection_list_params *params =
        (struct rfb_connection_list_params *) ptr;

    if (flag) {
        if (strcasecmp(name, S_RFB_PORT) == 0) {
            sprintf(value, "%d", params->port_value);
            return 1;
        }
    }
    else {
        strcpy(name, S_RFB_PORT);
        sprintf(value, "%d", params->port_value);
    }
    return 0;
}

static int rfb_password_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    struct rfb_connection_list_params *params =
        (struct rfb_connection_list_params *) ptr;

    if (flag) {
        if (strcasecmp(name, S_RFB_PASSWORD) == 0) {
            sprintf(value, "%s", params->password_value);
            return 1;
        }
    }
    else {
        strcpy(name, S_RFB_PASSWORD);
        sprintf(value, "%s", params->password_value);
    }
    return 0;
}

static int rfb_enable_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    if (flag) {
        if (strcasecmp(name, S_RFBCONNECTION) == 0) {
            sprintf(value, "Yes");
            return 1;
        }
    }
    else {
        strcpy(name, S_RFBCONNECTION);
        sprintf(value, "Yes");
    }
    return 0;
}

int selectRFBConnectionList(MENUINFO *minfo,
                            struct rfb_connection_list_params *params,
                            char *error_string) {

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    // Abort if not in standalone mode
    if (!is_standalone_enabled())
        return RC_ERROR;

    int board_number = hal_get_board_number(hp);
    static char config_file_name[128];
    sprintf(config_file_name, "/V2O/config/clientconnconf_%d.txt", board_number);

    if (rewrite_configuration_file(config_file_name,
                                   rfb_ipaddress_handler,
                                   params) < 0 ||
        rewrite_configuration_file(config_file_name,
                                   rfb_port_handler,
                                   params) < 0 ||
        rewrite_configuration_file(config_file_name,
                                   rfb_password_handler,
                                   params) < 0 ||
        rewrite_configuration_file(config_file_name,
                                   rfb_enable_handler,
                                   params) < 0 )
    {
        VLOG_WARNING("Could not rewrite configuration file %s",
                      config_file_name);
    }

    VLOG_INFO("Need to make a connection to %s:%d",
            params->ipaddress_value, params->port_value);
    standalone_client_reconnect(board_number);

    return displayRxConnectionStatus(minfo);
}

