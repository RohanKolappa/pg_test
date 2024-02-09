#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "vlog.h"
#include "commonutils.h"
#include "strmsrv_comm.h"
#include "clientutils.h"
#include "osd_control.h"
#include "rx_local_mouse_pointer_gen.h"

void get_rx_local_mouse_pointer_config(HAL *hp, int *MouseInput, int *timeout)
{
   struct rx_local_mouse_pointer_params params;
   memset(&params, 0x00, sizeof(params));

   params.userptr = hp;
   retrieveLocalMousePointerValues(&params);
   *MouseInput = params.enable_mouse_pointer_value;
   *timeout = params.timeout_value;

}

int applyLocalMousePointer(struct rx_local_mouse_pointer_params *params,
                           struct rx_local_mouse_pointer_params  *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_rx_mouse_input_params  rx_mouse_pointer;
    memset(&rx_mouse_pointer, 0x00, sizeof(rx_mouse_pointer));


    if (hp->hw_info.hardware_type != PLX_DEVICE &&
            hp->hw_info.hardware_type != XPI_DEVICE)
        return RC_ERROR;

    if (params->enable_mouse_pointer_set == 1) 
        rx_mouse_pointer.mouse_input_enable = params->enable_mouse_pointer_value;

    if (params->timeout_set == 1)
        rx_mouse_pointer.timeout_value = params->timeout_value;

    if (params->enable_mouse_pointer_value == 0)
        hal_set_mouse_cursor_status(hp, 0);


    strmsrv_set_rx_mouse_input_params(hal_get_channel_number(hp), &rx_mouse_pointer);

    return RC_NORMAL;
}


