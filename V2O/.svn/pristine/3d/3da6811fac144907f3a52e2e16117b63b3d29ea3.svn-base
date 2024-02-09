#include <syslog.h>

#define CUSTOM_MENU_DEFS 
#include "custmenu.h" 
#include "hal.h"
#include "hal_hwinfo.h"
#include "xpi_firmware_image_status_menu_gen.h"

int getXPiFirmwareImageStatus(struct xpi_firmware_image_status_params *params)
{
    HAL *hp = hal_acquire(0);
    char buf[34];

    if (hp == NULL) {
        return RC_ERROR;
    }

    memset(buf, 0, sizeof(buf));
    hal_get_firmware_version(hp, buf, sizeof(buf));
    strncpy(params->xpi_firmware_value, buf, 
            FIELD_XPI_FIRMWARE_IMAGE_STATUS_XPI_FIRMWARE_LENGTH);

    memset(buf, 0, sizeof(buf));
    hal_get_firmware_date(hp, buf, sizeof(buf));
    strncpy(params->xpi_firmware_date_value, buf, 
            FIELD_XPI_FIRMWARE_IMAGE_STATUS_XPI_FIRMWARE_DATE_LENGTH);

    memset(buf, 0, sizeof(buf));
    hal_get_s6fpga_version(hp, buf, sizeof(buf));
    strncpy(params->fanout_firmware_value, buf, 
            FIELD_XPI_FIRMWARE_IMAGE_STATUS_FANOUT_FIRMWARE_LENGTH);

    memset(buf, 0, sizeof(buf));
    hal_get_s6fpga_date(hp, buf, sizeof(buf));
    strncpy(params->fanout_date_value, buf, 
            FIELD_XPI_FIRMWARE_IMAGE_STATUS_FANOUT_DATE_LENGTH);

    memset(buf, 0, sizeof(buf));
    hal_get_s3fpga_version(hp, buf, sizeof(buf));
    strncpy(params->expansion_firmware_value, buf, 
            FIELD_XPI_FIRMWARE_IMAGE_STATUS_EXPANSION_FIRMWARE_LENGTH);

    memset(buf, 0, sizeof(buf));
    hal_get_s3fpga_date(hp, buf, sizeof(buf));
    strncpy(params->expansion_date_value, buf, 
            FIELD_XPI_FIRMWARE_IMAGE_STATUS_EXPANSION_DATE_LENGTH);

    return 0;
}


