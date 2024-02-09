#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "temperature_control.h"
#include "device_detection.h"
#include "temperature_status_menu_gen.h"

int getTemperatureStatus(struct temperature_status_params *params) 
{
    HAL *hp;
    float local, remote;
    int hardware_profile = get_hardware_profile();

    params->channel_1_local_value = 0;
    params->channel_1_remote_value = 0;
    params->channel_2_local_value = 0;
    params->channel_2_remote_value = 0;

    if ((hp = hal_acquire(0)) != NULL 
            && hal_get_hardware_type(hp) == PLX_DEVICE) {
        if (hal_get_board_temperature(hp, &local, &remote) < 0)
            VLOG_WARNING("Channel 1. Failed to read board temperature.");
        else {
            params->channel_1_local_value = local;
            params->channel_1_remote_value = remote;
        }
    }
    if (hp) hal_release(hp);

    if ((hardware_profile == HARDWARE_PROFILE_XP200 || 
            hardware_profile == HARDWARE_PROFILE_XP220) &&
            (hp = hal_acquire(1)) != NULL) {
        if (hal_get_board_temperature(hp, &local, &remote) < 0)
            VLOG_WARNING("Channel 2. Failed to read board temperature.");
        else {
            params->channel_2_local_value = local;
            params->channel_2_remote_value = remote;
        }
        if (hp) hal_release(hp);
    }

    /*******************************
     * Initialize SBC temperatures *
     *******************************/
    if ((params->cputemperature_value = 
                get_sysfs_attribute(TEMPERATURE_PROC_FILE, 
                    "temperature: %d C\n")) < 0) 
        params->cputemperature_value = 0;

    return RC_NORMAL;
}

