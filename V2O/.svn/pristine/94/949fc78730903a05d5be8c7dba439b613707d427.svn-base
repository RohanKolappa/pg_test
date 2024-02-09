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
#include "kbms_utils.h"
#include "kbconsole_config.h"
#include "kbconsole_menu_gen.h"
#include "vlog.h"

int is_keyboard_in_console_mode(void) 
{
    struct kbconsoleconf_params params;

    retrieveKBConsoleValues(&params);
    return params.kbconsole_value;
}

int set_keyboard_in_console_mode(int yesno) 
{
    char buff[128];
    sprintf(buff, "set_kbconsole -kbconsole %s", (yesno) ? "Yes" : "No");
    return system_command(buff);
}

/* Prototype for applyBootKBConsole function */
int applyKBConsole(struct kbconsoleconf_params *params, 
                struct kbconsoleconf_params *orig_params) 
{
    int bno;
    HAL *hp;

    for (bno = 0; bno < MAX_FPGA_BOARDS; bno++) {
        if ((hp = hal_acquire(bno)) != NULL)
            break;
    }
    if (hp == NULL) {
        VLOG_INFO("Failed to apply KB mode. Can't communicate with hardware");
        return RC_ERROR;
    }
    if (params->kbconsole_value) { 
        hal_kbm_switchout(hp);
    }
    else {
        hal_kbm_switchin(hp);
    }

    return RC_NORMAL;
}

