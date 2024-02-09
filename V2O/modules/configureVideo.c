
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <getopt.h>
#include <strings.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureVideo.h"
#include "kbm_control.h"
#include "loadnet.h"
#include "log_control.h"
#include "vlog.h"
#include "commonutils.h"
#include "serverutils.h"
#include "clientutils.h"
#include "vutils.h"
#include "remoteConfig.h"
#include "strmsrv_comm.h"


// TODO: move these to a better place
char *yesnolist[] = { "Yes", "yes", "No", "no", NULL };

static int board_number = 0;

/******************************
 * Disconnect All Clients     *
 ******************************/
int disconnectClients(MENUINFO * minfo)
{
    DIALOG *dialog;

    HAL *hp;
    hp = minfo->minfo_userptr;
    if (hp != NULL) {
        board_number = hal_get_board_number(hp);
    }

    /********************
     * Print menu title *
     ********************/
    show_title(minfo, NULL);


    dialog = show_dialog(minfo, 4, 0,
                         "</K></U>Warning:<!K><!U> This will disconnect all currently\n"
                         "connected clients.");
    show_info(minfo, "Proceed to disconnect?" YESNO_STR);
    if (get_yesno(minfo)) {
        strmsrv_tx_disconnect(hal_get_channel_number(hp));
        show_info(minfo, "Disconnecting clients. Please wait...");
        sleep(1);
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
    }
    remove_dialog(dialog);

    return RC_NORMAL;
}


