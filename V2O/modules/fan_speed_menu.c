#include <stdlib.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "device_detection.h"
#include "system_utils.h"
#include "fan_speed_menu_gen.h"
#include "temperature_control.h"
#include "vlog.h"

int get_fan_speed_config(HAL *hp) {
    struct fan_speed_params params;
    params.userptr = hp;
    retrieveFanSpeedParametersValues(&params);
    if (params.fanspeed_value == FAN_SPEED_FANSPEED_LOW)
        return FAN_SPEED_LOW;
    else if (params.fanspeed_value == FAN_SPEED_FANSPEED_MEDIUM)
        return FAN_SPEED_MEDIUM;
    else if (params.fanspeed_value == FAN_SPEED_FANSPEED_HIGH)
        return FAN_SPEED_HIGH;
    else
        return FAN_SPEED_AUTO;
}

int applyFanSpeedParameters(struct fan_speed_params *params,
                             struct fan_speed_params *orig_params)
{
    HAL *hp = hal_acquire(0);

    if (hp == NULL) {
        return RC_ERROR;
    }
    int hwprofile = get_hardware_profile();
    if (hwprofile == HARDWARE_PROFILE_XP100 ||
           hwprofile == HARDWARE_PROFILE_XPI ) {
        if (params->fanspeed_value == FAN_SPEED_FANSPEED_LOW)
            hal_set_fan_speed(hp, FAN_SPEED_LOW);
        else if (params->fanspeed_value == FAN_SPEED_FANSPEED_MEDIUM)
            hal_set_fan_speed(hp, FAN_SPEED_MEDIUM);
        else if (params->fanspeed_value == FAN_SPEED_FANSPEED_HIGH)
            hal_set_fan_speed(hp, FAN_SPEED_HIGH);
    }
    hal_release(hp);
    return RC_NORMAL;
}
