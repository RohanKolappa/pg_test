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
#include "xpi_fan_status_menu_gen.h"
#include "temperature_control.h"

int getXPiFanStatus(struct xpi_fan_status_params *params) 
{
    float local, remote, cputemp, fpgatemp, phytemp;
    int profile = get_hardware_profile(), speed, rpm;

#ifdef __XPI__
    if (profile == HARDWARE_PROFILE_XPI) {
        HAL *hp = hal_acquire(0);
        if (hp == NULL)
            return RC_ERROR;
        if (hal_get_xpi_cpu_temperature(hp, &cputemp) < 0) {
            VLOG_WARNING("Failed to read CPU internal temperature.");
        }
        params->cputemperature_value = cputemp;

        if (hal_get_board_temperature(hp, &local, &remote) < 0) {
            VLOG_WARNING("Failed to read board temperature.");
        }
        else {
            params->board_local_value = local;
            params->board_remote_value = remote;
        }

        if (hal_get_xpi_fpga_temperature(hp, &fpgatemp) < 0) {
            VLOG_WARNING("Failed to read FPGA temperature.");
        }
        params->fpgatemperature_value = fpgatemp;

        if (hal_get_xpi_phy_temperature(hp, &phytemp) < 0) {
            VLOG_WARNING("Failed to read PHY temperature.");
        }
        params->phytemperature_value = phytemp;

        speed = get_fan_speed_config(hp);
        if (speed == FAN_SPEED_LOW)
            strcpy(params->fan_speed_value, "Low");
        else if (speed == FAN_SPEED_MEDIUM)
            strcpy(params->fan_speed_value, "Medium");
        else if (speed == FAN_SPEED_HIGH)
            strcpy(params->fan_speed_value, "High");
        else
            strcpy(params->fan_speed_value, "Auto");

        if ((rpm = hal_get_fan_rpm(hp)) < 0)
            strcpy(params->fan_rpm_value, "???");
        else
            sprintf(params->fan_rpm_value, "%d", rpm);

        hal_release(hp);
    }
#endif

    return RC_NORMAL;
}

