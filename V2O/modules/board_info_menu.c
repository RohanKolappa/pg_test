#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "vlib.h"
#include "vlog.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "board_info_menu_gen.h"
#include "strmsrv_comm.h"
#include "standalone.h"

#define HALFILE   "/tmp/hald"


void kill_hald_current_channel(HAL *hp, int stop_strmsrv)
{
    char cmdbuf[256];
    struct stat statbuf;
    char halfile[20];
    sprintf(halfile, "%s.%d", HALFILE, hal_get_board_number(hp));

    if (stat(halfile, &statbuf) == 0) {
        // first kill the hald on that channel
        VLOG_INFO("Terminating hald on channel %d %s\n", hal_get_channel_number(hp), halfile);
        sprintf(cmdbuf, "kill `cat %s`", halfile);
        system_command(cmdbuf);

        if (stop_strmsrv) {
            if (is_standalone_enabled()) {
                standalone_stop(hal_get_channel_number(hp));
            }
            else {
                strmsrv_stop_fpga(hal_get_channel_number(hp));
            }
        }
    }
}

static int validateBoardInfo(struct board_info_params *params,
                struct board_info_params *orig_params,
                char *error_string) {
#ifndef __XPI__
    sprintf(error_string,"Changes will terminate existing connections.");
#else
    sprintf(error_string, "User needs to Save to Flash and reboot \n"
            "for change board type to take effect.");
#endif
    return RC_WARNING;
}

int applyBoardInfo(struct board_info_params *params, 
        struct board_info_params *orig_params){
    HAL *hp = params->userptr;
    char cmdbuf[256];
    if (hp == NULL)
        return RC_ERROR;

#ifndef __XPI__
    kill_hald_current_channel(hp, 1);

    hal_deactivate_board(hp);

    // Apply needed values
    if (params->boardtype_set) {
        switch (params->boardtype_value) {
            case BOARD_INFO_BOARDTYPE_STREAMDST:
                sprintf(cmdbuf, "/V2O/bin/loadfpga -b %d -t %d",
                        hal_get_board_number(hp), HARDWARE_V2D_RX);
                break;
            case BOARD_INFO_BOARDTYPE_STREAMSRC:
                sprintf(cmdbuf, "/V2O/bin/loadfpga -b %d -t %d",
                        hal_get_board_number(hp), HARDWARE_V2D_TX);
            default:
                break;
        }
    }
    system_command(cmdbuf);
    if (hal_get_hardware_type(hp) == XPI_DEVICE) 
        hal_activate_board(hp);
    else {
        // Start the hal daemon
        sprintf ( cmdbuf, "/V2O/bin/hald -b %d", hal_get_board_number(hp));
        system_command(cmdbuf);

        if (is_standalone_enabled()) {
            standalone_start(hal_get_channel_number(hp));
        }
    }
#else
    // Apply needed values
    if (params->boardtype_set) {
        sprintf(cmdbuf,"/bin/rm -f /etc/config/sys_type_xpi_*");
        system_command(cmdbuf);

        switch (params->boardtype_value) {
            case BOARD_INFO_BOARDTYPE_STREAMDST:
                sprintf(cmdbuf, "/bin/touch /etc/config/sys_type_xpi_decoder") ;
                break;
            case BOARD_INFO_BOARDTYPE_STREAMSRC:
                sprintf(cmdbuf, "/bin/touch /etc/config/sys_type_xpi_encoder") ;
                break;
            default:
                break;
        }
        system_command(cmdbuf);
    }

#endif

    return RC_NORMAL;
}

void BoardInfoInit()
{
    registerBoardInfoValidateFunction(validateBoardInfo);
}
