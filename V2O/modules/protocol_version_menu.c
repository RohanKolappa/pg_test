/*
 * protocol_version_menu.c
 *
 *  Created on: May 5, 2011
 *      Author: rkale
 */

#include <stdlib.h>
#include <string.h>
//#include <crypt.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "protocol_version_menu_gen.h"
#include "standalone.h"

extern int applyProtocolVersionParameters(struct protocol_version_params
                                          *params,
                                          struct protocol_version_params
                                          *orig_params)
{
    return RC_NORMAL;
}


static void protocolVersionParametersSaved(void *userptr) {
    HAL *hp = userptr;
    if (hp == NULL)
        return;
    standalone_restart_tx(hal_get_channel_number(hp));
}

void ProtocolVersionParametersInit() {
    registerProtocolVersionParametersSavedFunction(protocolVersionParametersSaved);
}
