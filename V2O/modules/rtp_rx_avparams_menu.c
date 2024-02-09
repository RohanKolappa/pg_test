

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
#include "rtp_rx_avparams_menu_gen.h"

static int validateRTPRxAVParameters(struct rtp_rx_avparm_params *params,
                      struct rtp_rx_avparm_params *orig_params,
                      char *error_string) 
{
    if (params->port_from_value > params->port_to_value) {
        
        sprintf(error_string, "Start port cannot be greater than End port");
        return RC_INVALID_VALUE;
    }
    return RC_NORMAL;
}


int applyRTPRxAVParameters(struct rtp_rx_avparm_params *params,
                   struct rtp_rx_avparm_params *orig_params)
{

    return RC_NORMAL;
}


void RTPRxAVParametersInit()
{
    registerRTPRxAVParametersValidateFunction(validateRTPRxAVParameters);
}
