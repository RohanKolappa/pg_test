
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "device_detection.h"
#include "multicast_params_menu_gen.h"
#include "standalone.h"

int applyMulticastParameters(struct multicast_parm_params *params,
                             struct multicast_parm_params
                             *orig_params)
{
    // We let the savedParameter callback handle the actual apply
    // since the parameters are not written to config till
    // this function returns normally
    return RC_NORMAL;
}

static void multicastParametersSaved(void) 
{
   /* tearing down streaming server object for each individual
     board */
    standalone_restart_tx(1);
    standalone_restart_tx(2);
}

void MulticastParametersInit() {
    registerMulticastParametersSavedFunction(multicastParametersSaved);
}

