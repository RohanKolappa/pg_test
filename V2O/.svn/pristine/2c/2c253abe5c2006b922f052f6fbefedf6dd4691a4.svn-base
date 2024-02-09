#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "demos_menu_gen.h"


#define DEMO_SETUP_SCRIPT "/etc/scripts/setup_demos.sh"

int applyDemos(struct demos_mode_params *params, 
                          struct demos_mode_params *orig_params)
{
    char buff[128];
    int retcode;
    struct stat statbuf;

    // Ignore apply if demo setup script not found
    if (stat(DEMO_SETUP_SCRIPT, &statbuf) != 0) {
        return RC_NORMAL;
    }

    if (params->enable_value) {
        sprintf(buff, "%s setup", DEMO_SETUP_SCRIPT);
        retcode = system_command(buff);
    } else { 
        sprintf(buff, "%s teardown", DEMO_SETUP_SCRIPT);
        retcode = system_command(buff);
    }

    return RC_NORMAL;
}
