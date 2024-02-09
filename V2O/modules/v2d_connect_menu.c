
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#ifndef __XPI__
#include "lut_table.h"
#endif
#include "lut_utils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlib.h"
#include "v2d_connect_menu_gen.h"
#include "standalone.h"
#include "configfile_parser.h"

static int board_number;
static char *get_config_file_name(void)
{
    static char buf[128];
    sprintf(buf, "/V2O/config/clientconnconf_%d.txt", board_number);
    return buf;
}
static int client_serveraddress_handler(char *name, char *value, void *ptr,
                                        char flag)
{

    struct v2d_connect_params *params= (struct v2d_connect_params *)ptr;

    if (flag) {
        if (strcasecmp(name, S_SERVER_ADDRESS) == 0) {
            strcpy(value, params->ipaddress_value);
            return 1;
        }
    }
    else {
        strcpy(name, S_SERVER_ADDRESS);
        strcpy(value, params->ipaddress_value);
    }
    return 0;
}

static int client_serverport_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    struct v2d_connect_params *params= (struct v2d_connect_params *)ptr;

    if (flag) {
        if (strcasecmp(name, S_SERVER_PORT) == 0) {
            sprintf(value, "%d", params->port_value);
            return 1;
        }
    }
    else {
        strcpy(name, S_SERVER_PORT);
        sprintf(value, "%d", params->port_value);
    }

    return 0;
}

static int client_bandwidth_handler(char *name, char *value, void *ptr,
                                    char flag)
{
    struct v2d_connect_params *params= (struct v2d_connect_params *)ptr;



    if (flag) {
        if (strcasecmp(name, S_MAX_BW) == 0) {
            sprintf(value, "%d", params->bandwidth_value);
            return 1;
        }
    }
    else {
        strcpy(name, S_MAX_BW);
        sprintf(value, "%d", params->bandwidth_value);
    }
    return 0;
}

static int client_multicast_handler(char *name, char *value, void *ptr,
                                    char flag)
{

    struct v2d_connect_params *params= (struct v2d_connect_params *)ptr;

    if (flag) {
        if (strcasecmp(name, S_MULTICAST) == 0) {
            sprintf(value, "%s", 
                    params->multicast_value ? "Yes" : "No");
            return 1;
        }
    }
    else {
        strcpy(name, S_MULTICAST);
        sprintf(value, "%s",
                    params->multicast_value ? "Yes" : "No");
    }
    return 0;
}

static int client_avoption_handler(char *name, char *value, void *ptr,
                                   char flag)
{
    struct v2d_connect_params *params= (struct v2d_connect_params *)ptr;


    if (flag) {
        if (strcasecmp(name, S_AVOPTION) == 0) {
            sprintf(value, "%s",params->avoption);
            return 1;
        }
    }
    else {
        strcpy(name, S_AVOPTION);
        sprintf(value, "%s",params->avoption);
    }
    return 0;
}

static int client_bidir_handler(char *name, char *value, void *ptr,
                                char flag)
{
    struct v2d_connect_params *params= (struct v2d_connect_params *)ptr;

    if (flag) {
        if (strcasecmp(name, S_AUDIO_BIDIR) == 0) {
            sprintf(value, "%s", 
                    params->bidir_value ? "Yes" : "No");
            return 1;
        }
    }
    else {
        strcpy(name, S_AUDIO_BIDIR);
        sprintf(value, "%s", 
                    params->bidir_value ? "Yes" : "No");
    }
    return 0;
}


static int client_rfb_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    if (flag) {
        if (strcasecmp(name, S_RFBCONNECTION) == 0) {
            sprintf(value, "No");
            return 1;
        }
    }
    else {
        strcpy(name, S_RFBCONNECTION);
        sprintf(value, "No");
    }
    return 0;
}

int applyV2DConnect(struct v2d_connect_params *params,
                             struct v2d_connect_params *orig_params)
{
    HAL *hp = params->userptr;
    board_number = hal_get_board_number(hp);

    if (hp == NULL)
        return RC_ERROR;
    if (rewrite_config_file(get_config_file_name(),
                            client_serveraddress_handler,
                            params) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_serverport_handler,
                               params) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_multicast_handler,
                               params) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_bandwidth_handler,
                               params) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_avoption_handler,
                               params) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_bidir_handler,
                               params) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_rfb_handler,
                               params) < 0 )
    {
        VLOG_WARNING("Could not rewrite configuration file %s",
                      get_config_file_name());
    }
    standalone_client_reconnect(board_number);



    return RC_NORMAL;
}
