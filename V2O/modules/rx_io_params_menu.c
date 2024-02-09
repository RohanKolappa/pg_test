
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "serial_config.h"
#include "rx_io_params_menu_gen.h"



int get_rx_serial_config(HAL *hp, struct serial_config *serial_cfg) {

    struct rx_io_parm_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveRxIOParametersValues(&params);

    serial_cfg->serial_enable = params.serial_enable_value;

    switch (params.serial_baudrate_value) {
    case RX_IO_PARM_SERIAL_BAUDRATE_300:
        serial_cfg->baudrate = 300;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_600:
        serial_cfg->baudrate = 600;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_1200:
        serial_cfg->baudrate = 1200;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_2400:
        serial_cfg->baudrate = 2400;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_4800:
        serial_cfg->baudrate = 4800;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_9600:
        serial_cfg->baudrate = 9600;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_19200:
        serial_cfg->baudrate = 19200;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_38400:
        serial_cfg->baudrate = 38400;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_57600:
        serial_cfg->baudrate = 57600;
        break;
    case RX_IO_PARM_SERIAL_BAUDRATE_115200:
        serial_cfg->baudrate = 115200;
        break;
    }
    
    serial_cfg->databits = params.serial_databits_value;
    serial_cfg->stopbits = params.serial_stopbits_value;

    switch (params.serial_parity_value) {
    case RX_IO_PARM_SERIAL_PARITY_EVEN:
        serial_cfg->parity = 'e';
        break;
    case RX_IO_PARM_SERIAL_PARITY_ODD:
        serial_cfg->parity = 'o';
        break;
    case RX_IO_PARM_SERIAL_PARITY_NONE:
        serial_cfg->parity = 'n';
        break;
    }

    switch (params.serial_flowcontrol_value) {
    case RX_IO_PARM_SERIAL_FLOWCONTROL_HW:
        serial_cfg->flowcontrol = 'h';
        break;
    case RX_IO_PARM_SERIAL_FLOWCONTROL_SW:
        serial_cfg->flowcontrol = 's';
        break;
    case RX_IO_PARM_SERIAL_FLOWCONTROL_NONE:
        serial_cfg->flowcontrol = 'n';
        break;
    }

    serial_cfg->serialwait = params.serial_wait_value;

    return 0;
}


int applyRxIOParameters(struct rx_io_parm_params *params,
                        struct rx_io_parm_params
                        *orig_params)
{

    if (params->serial_enable_set == 1) {

        if (get_hardware_profile() == HARDWARE_PROFILE_V2D_TX ||
            get_hardware_profile() == HARDWARE_PROFILE_V2D_RX) {
            
            if (params->serial_enable_value == 1) {
                system_command("/bin/touch /tmp/.rs232");
                system_command("/usr/bin/killall getty");
            }
            else {
                system_command("/bin/rm /tmp/.rs232");
                system_command("/usr/bin/killall sleep");
            }
        }

        // TODO: Do appropriate action on XP200 if any
    }
    
    return RC_NORMAL;
}



