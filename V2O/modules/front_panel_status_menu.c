

#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#include <sys/wait.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "front_panel_status_menu_gen.h"


int getFrontPanelStatus(struct front_panel_status_params *params) 
{
    if (get_hardware_category() != HARDWARE_CATEGORY_DMS)
        return RC_ERROR;

    if (WEXITSTATUS(system_command("killall -0 frontpanel")) == 0) {
        params->status_value = 1;
    }
    else {
        params->status_value = 0;
    }
    return RC_NORMAL;
}
