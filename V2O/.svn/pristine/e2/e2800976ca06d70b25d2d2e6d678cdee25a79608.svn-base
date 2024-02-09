#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "sync_edid_eeprom_menu_gen.h"
#include "vlog.h"
#include "sync_edid_eeprom_config.h"

int get_sync_edid_eeprom_config(HAL *hp)
{
    struct sync_edid_eeprom_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveSyncEdidEepromValues(&params);
    return params.enable_value;
}

int applySyncEdidEeprom(struct sync_edid_eeprom_parm_params *params,
        struct sync_edid_eeprom_parm_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    return RC_NORMAL;
}

