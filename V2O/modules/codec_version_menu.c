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
#include "codec_version_menu_gen.h"
#include "strmsrv_comm.h"
#include "standalone.h"

struct  codec_version_params *paramslist;


int get_configured_codec_version(HAL *hp) {
    int codecversion ;
    struct  codec_version_params codec_version_params;

    codec_version_params.userptr = hp;

    retrieveCodecVersionValues(&codec_version_params);
    switch( codec_version_params.codectype_value) {
      case CODEC_VERSION_CODECTYPE_STANDARD:
        codecversion = 1;
        break;
      case CODEC_VERSION_CODECTYPE_SCALABLE:
        codecversion = 2;
        break ;
      default:
        codecversion = 1;
        break;
    }
    return codecversion;
}

 static void CodecVersionSaved(void *userptr)
{
    HAL *hp = userptr;
    char cmdbuf[256];

    if (hp == NULL)
        return;

    kill_hald_current_channel(hp, 0);

    if (paramslist->codectype_set) {
        switch (paramslist->codectype_value) {
        case CODEC_VERSION_CODECTYPE_STANDARD:
            sprintf(cmdbuf, "/V2O/bin/loadfpga   -V %d -b %d -t %d ", 1,
                    hal_get_board_number(hp), HARDWARE_V2D_TX);

            hal_set_video_available(hp, 0);
            hal_resetAddsideVideo(hp);
            break;
        case CODEC_VERSION_CODECTYPE_SCALABLE:
            sprintf(cmdbuf, "/V2O/bin/loadfpga   -V %d -b %d -t %d", 2,
                    hal_get_board_number(hp), HARDWARE_V2D_TX);

            hal_set_video_available(hp, 0);
            hal_resetAddsideVideo(hp);
            break;
        }
    }

    system_command(cmdbuf);

    // Start the hal daemon
    sprintf(cmdbuf, "/V2O/bin/hald -b %d", hal_get_board_number(hp));
    system_command(cmdbuf);
    
}

int applyCodecVersion(struct codec_version_params *params,
        struct codec_version_params *orig_params) {

    paramslist = params;
    return RC_NORMAL;
}

void CodecVersionInit() {
       registerCodecVersionSavedFunction(CodecVersionSaved);
}
