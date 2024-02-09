#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "stand_alone_mode_menu_gen.h"
#include "standalone.h"


/* Prototype for applyStandAlone function */
int applyStandAlone(struct adminop_params *params, 
                             struct adminop_params *orig_params) 
{
    /* We start/stop the running instance of device app */
    /* Also create/delete Rx/Tx objects in StreamingServer as needed */
    if (params->standalone_value) {
        system_command("/bin/devappctl stop");
        standalone_startall();
    }
    else {
        standalone_stopall();
        // Since standalone mode config file is not written yet, we use
        // "forcestart" option that makes the script ignore the current standalone mode
        system_command("/bin/devappctl forcestart");
    }


    return RC_NORMAL;
}

