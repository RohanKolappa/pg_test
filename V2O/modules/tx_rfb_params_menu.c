#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "tx_rfb_params_menu_gen.h"
#include "vlog.h"
#include "strmsrv_comm.h"
#include "tx_rfb_config.h"

// Retrieves current RFB configuration for the given board.
int get_tx_rfb_config(HAL *hp, struct tx_rfb_config *rfb_cfg)
{

    struct tx_rfb_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveTxRFBParametersValues(&params);

    rfb_cfg->port = params.port_value;
    rfb_cfg->enable = params.enable_value;
    strcpy(rfb_cfg->ipaddress, params.ipaddress_value);
    strcpy(rfb_cfg->password, params.password_value);
    rfb_cfg->mouse_offset_x = params.offset_x_value;
    rfb_cfg->mouse_offset_y = params.offset_y_value;

    return 0;
}

int applyTxRFBParameters(struct tx_rfb_parm_params *params,
        struct tx_rfb_parm_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    /* Reconfigure RRB Server parameters on streaming server */
    struct strmsrv_tx_rfb_params rfb_params;
    memset(&rfb_params, 0x00, sizeof(struct strmsrv_tx_rfb_params));

    /* Collect orig params */
    rfb_params.enable = orig_params->enable_value;
    strcpy(rfb_params.ipaddress, orig_params->ipaddress_value);
    strcpy(rfb_params.password, orig_params->password_value);
    rfb_params.port = orig_params->port_value;
    rfb_params.mouse_offset_x = orig_params->offset_x_value;
    rfb_params.mouse_offset_y = orig_params->offset_y_value;

    /* Override with what changed */
    if (params->enable_set == 1)
        rfb_params.enable = params->enable_value;
    if (params->ipaddress_set == 1)
        strcpy(rfb_params.ipaddress, params->ipaddress_value);
    if (params->password_set == 1)
        strcpy(rfb_params.password, params->password_value);
    if (params->port_set == 1)
        rfb_params.port = params->port_value;
    if (params->offset_x_set == 1)
        rfb_params.mouse_offset_x = params->offset_x_value;
    if (params->offset_y_set == 1)
        rfb_params.mouse_offset_y = params->offset_y_value;

    strmsrv_set_tx_rfb_params(hal_get_channel_number(hp), &rfb_params);

    return RC_NORMAL;
}

