#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "ad9888vif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "strmsrv_comm.h"
#include "kbm_control.h"
#include "tx_kbm_config.h"
#include "tx_display_io_info_status_menu_gen.h"

static struct kbm_param_io kbmpio;

int getTxDisplayIOInfoStatus(struct tx_display_io_info_status_params *params) 
{

    HAL *hp = params->userptr;
    hal_get_kbm_params((HAL *) params->userptr, &kbmpio);
    if (hp == NULL)
        return RC_ERROR;

    params->keyboard_out_value =  kbmpio.kb_wbytes;    
    params->keyboard_dropped_value = kbmpio.kb_wdropped;
    params->mouse_out_value = kbmpio.ms_wbytes;
    params->mouse_dropped_value = kbmpio.ms_wdropped;

    // Put default values
    sprintf(params->kb_type_value, "%s", "PC");
    sprintf(params->ms_type_value, "%s", "IMPS2");
    strcpy(params->kb_mode_value, "???");
    strcpy(params->rfb_server_value, "???");

    struct strmsrv_tx_io_params tx_io_params;
    int channel_number = hal_get_channel_number(hp);
    int rc = strmsrv_get_tx_io_params(channel_number, &tx_io_params);
    if (rc != -1) {
        params->control_in_value = tx_io_params.control_in;
        params->control_out_value = tx_io_params.control_out;
        params->keyboard_in_value = tx_io_params.keyboard_in;
        params->mouse_in_value = tx_io_params.mouse_in;
        params->usbkb_in_bytes_value = tx_io_params.usbkb_in_bytes;
        params->usbms_in_bytes_value = tx_io_params.usbms_in_bytes;
        params->serial_in_value = tx_io_params.serial_in;
        params->serial_out_value = tx_io_params.serial_out;

        if(tx_io_params.kb_type == KB_TYPE_SGI)
            sprintf(params->kb_type_value, "%s", "SGI");
        else
            sprintf(params->kb_type_value, "%s", "PC");

        if(tx_io_params.ms_type == MS_TYPE_PS2) 
            sprintf(params->ms_type_value, "%s", "PS2");
        else 
            sprintf(params->ms_type_value, "%s", "IMPS2");

        strcpy(params->kb_mode_value, tx_io_params.kb_mode);
    }

    struct strmsrv_tx_rfb_connection_params tx_rfb_connection_params;
    rc = strmsrv_get_tx_rfb_connection_params(channel_number, 
            &tx_rfb_connection_params);
    if (rc != -1 && tx_rfb_connection_params.rfb_server_set) {
        strcpy(params->rfb_server_value, tx_rfb_connection_params.rfb_server);
        strcat(params->rfb_server_value, " ");
        if (tx_rfb_connection_params.rfb_ready_set) {
            if (tx_rfb_connection_params.rfb_ready) 
                strcat(params->rfb_server_value, "(Ready)");
            else
                strcat(params->rfb_server_value, "(Not Ready)");
        }
        else {
            strcat(params->rfb_server_value, "(Disabled)");
        }
        params->rfb_keyboard_out_value = tx_rfb_connection_params.rfb_keyboard_out;
        params->rfb_mouse_out_value = tx_rfb_connection_params.rfb_mouse_out;
    }
    else {
        strcpy(params->rfb_server_value, "???");
    }

    return RC_NORMAL;

}


