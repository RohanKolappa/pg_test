#include <stdio.h>
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
#include "vlog.h"
#include "tx_thumbnail_params_menu_gen.h"
#include "strmsrv_comm.h"
#include "tx_thumbnail_config.h"

int get_tx_thumbnail_config(int board_number, struct tx_thumbnail_config *thumbnail_cfg) {

    struct tx_thumbnail_parm_params params;
    HAL * hp;

    hp = hal_acquire(board_number);
    if (hp == NULL)
        return -1;

    params.userptr = hp;
    retrieveTxThumbnailParametersValues(&params);
    hal_release(hp);

    memset(thumbnail_cfg, 0x00, sizeof(struct tx_thumbnail_config));

    thumbnail_cfg->enable = params.enable_value;
    thumbnail_cfg->enable_native = params.enable_native_value;
    thumbnail_cfg->enable_large_thumbnail = params.enable_large_thumbnail_value;
    thumbnail_cfg->enable_small_thumbnail = params.enable_small_thumbnail_value;
    thumbnail_cfg->large_thumbnail_width = params.large_thumbnail_width_value;
    thumbnail_cfg->large_thumbnail_height = params.large_thumbnail_height_value;
    thumbnail_cfg->small_thumbnail_width = params.small_thumbnail_width_value;
    thumbnail_cfg->small_thumbnail_height = params.small_thumbnail_height_value;
    thumbnail_cfg->thumbnail_interval = params.thumbnail_interval_value;

    return 0;
}

int applyTxThumbnailParameters(struct tx_thumbnail_parm_params *params,
                        struct tx_thumbnail_parm_params
                        *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_tx_thumbnail_params thumbnail_params;
    memset(&thumbnail_params, 0x00, sizeof(struct strmsrv_tx_thumbnail_params));

    thumbnail_params.enable_thumbnails = params->enable_value;
    thumbnail_params.enable_native = params->enable_native_value;
    thumbnail_params.enable_large_thumbnail = params->enable_large_thumbnail_value;
    thumbnail_params.enable_small_thumbnail = params->enable_small_thumbnail_value;
    thumbnail_params.large_thumbnail_width = params->large_thumbnail_width_value;
    thumbnail_params.large_thumbnail_height = params->large_thumbnail_height_value;
    thumbnail_params.small_thumbnail_width = params->small_thumbnail_width_value;
    thumbnail_params.small_thumbnail_height = params->small_thumbnail_height_value;
    thumbnail_params.thumbnail_interval = params->thumbnail_interval_value;

    if (!is_standalone_enabled())
        strmsrv_set_tx_thumbnail_params(hal_get_channel_number(hp), &thumbnail_params);

    return RC_NORMAL;
}

static void txThumbnailParametersSaved(void *userptr)
{
    HAL *hp = userptr;

    if (hp == NULL)
        return;
}

void TxThumbnailParametersInit()
{
    registerTxThumbnailParametersSavedFunction(txThumbnailParametersSaved);
}
