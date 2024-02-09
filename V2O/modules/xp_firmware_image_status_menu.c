
#include "hal.h"
#include "hal_vpd.h"
#include "vlib.h"
#include "vlog.h"
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "xp_firmware_image_status_menu_gen.h"

int getXPFirmwareImageStatus(struct xp_firmware_image_status_params *params) 
{
    HAL *hp;
    hp = hal_acquire(0);
    hal_get_board_serial_number(hp, params->board_0_serial_value, 80);
    hal_get_firmware_version(hp, params->board_0_firmware_value, 80);
    hal_get_firmware_date(hp, params->firmware_0_date_value, 80);
    hal_release(hp);
    hp = hal_acquire(1);
    hal_get_board_serial_number(hp, params->board_1_serial_value, 80);
    hal_get_firmware_version(hp, params->board_1_firmware_value, 80);
    hal_get_firmware_date(hp, params->firmware_1_date_value, 80);
    hal_release(hp);
    return RC_NORMAL;
}
