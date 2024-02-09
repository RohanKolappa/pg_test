#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "vlog.h"
#include "commonutils.h"
#include "device_detection.h"
#include "system_utils.h"
#include "xpi_i2c_control.h"
#include "temperature_control.h"
#include "fan_status_menu_gen.h"

int getFanStatus(struct fan_status_params *params) 
{
    float local, remote;
    int speed, cputemp;
    int profile = get_hardware_profile();

    if (profile == HARDWARE_PROFILE_XP100) {
        HAL *hp = hal_acquire(0);
        if (hp == NULL)
            return RC_ERROR;
        speed = hal_get_fan_speed(hp);
        if (speed == FAN_SPEED_LOW) 
            strcpy(params->fan_speed_value, "Low");
        else if (speed == FAN_SPEED_MEDIUM)
            strcpy(params->fan_speed_value, "Medium");
        else if (speed == FAN_SPEED_HIGH)
            strcpy(params->fan_speed_value, "High");
        else
            strcpy(params->fan_speed_value, "Off");

        if ((cputemp = get_sysfs_attribute(TEMPERATURE_PROC_FILE,
                        "temperature: %d C\n")) < 0) {
            hal_release(hp);
            return RC_ERROR;
        }
        params->cputemperature_value = cputemp;

        if (hal_get_board_temperature(hp, &local, &remote) < 0) {
            VLOG_WARNING("Channel 1. Failed to read board temperature.");
            hal_release(hp);
            return RC_ERROR;
        }
        else {
            params->channel_1_local_value = local;
            params->channel_1_remote_value = remote;
        }
        params->fan_rpm_value = (double) hal_get_fan_rpm(hp);
        hal_release(hp);
    }

    return RC_NORMAL;
}

