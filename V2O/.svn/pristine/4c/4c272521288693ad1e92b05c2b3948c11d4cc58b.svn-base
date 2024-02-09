
#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#include <sys/wait.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "xmpp_server_status_menu_gen.h"



int getXMPPServerStatus(struct xmpp_server_status_params *params) {


    struct stat statbuf;

    if (stat("/var/status/mserver.up", &statbuf) == 0) {
        params->status_value = 1;
    }
    else {
        params->status_value = 0;
    }

    return RC_NORMAL;
}
