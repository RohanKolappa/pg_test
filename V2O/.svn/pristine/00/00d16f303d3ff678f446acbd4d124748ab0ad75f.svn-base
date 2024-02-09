#include <string.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "ad9888vif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_console.h"
#include "strmsrv_comm.h"
#include "rx_display_io_info_status_menu_gen.h"

static struct kbm_param_io kbmpio;

int getRxDisplayIOInfoStatus(struct rx_display_io_info_status_params *params) 
{

    HAL *hp = params->userptr;
    hal_get_kbm_params((HAL *) params->userptr, &kbmpio);
    if (hp == NULL)
        return RC_ERROR;

    params->ps2_keyboard_in_value =  kbmpio.kb_rbytes;    
    params->ps2_keyboard_dropped_value = kbmpio.kb_rdropped;
    params->ps2_mouse_in_value = kbmpio.ms_rbytes;
    params->ps2_mouse_dropped_value = kbmpio.ms_rdropped;
    params->keyboard_out_value =  kbmpio.kb_wbytes;    
    params->mouse_out_value = kbmpio.ms_wbytes;
    sprintf(params->kb_mode_value, "???");
    sprintf(params->local_cursor_value, "???");

    struct strmsrv_rx_io_params rx_io_params;
    int channel_number = hal_get_channel_number(hp);
    int rc = strmsrv_get_rx_io_params(channel_number, &rx_io_params);
    if (rc != -1) {
        params->control_in_value = rx_io_params.control_in;
        params->control_out_value = rx_io_params.control_out;
        params->serial_in_value = rx_io_params.serial_in;
        params->serial_out_value = rx_io_params.serial_out;
        params->usbkb_in_value = rx_io_params.usbkb_in_bytes;
        params->usbms_in_value = rx_io_params.usbms_in_bytes;
        hal_update_virtual_console(hp);
        if (hp->console_info.kb_active) 
            strcpy(params->kb_mode_value, rx_io_params.kb_mode);
        else
            strcpy(params->kb_mode_value, "Console");
        if (rx_io_params.local_cursor) {
            sprintf(params->local_cursor_value, "Enabled");
            if (strstr(params->kb_mode_value, "Remote") != NULL)
                strcat(params->local_cursor_value, " (Active)");
            else
                strcat(params->local_cursor_value, " (Not Active)");
        }
        else
            sprintf(params->local_cursor_value, "Disabled");
    }

    return RC_NORMAL;

}


