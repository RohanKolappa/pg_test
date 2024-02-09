/*
 * av_buffers_menu.c
 *
 *  Created on: Nov 9, 2009
 *      Author: rkale
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "vlog.h"
#include "vlib.h"
#include "commonutils.h"
#include "tx_av_buffers_menu_gen.h"

static void paramsInitTxAVBuffers(struct tx_av_buffers_params *params)
{
    struct fc_queue_limits_io fcqlimitio;

    HAL *hp = params->userptr;

    if (hp == NULL)
        return;

    if (hal_get_fc_queue_limits(hp, &fcqlimitio) == -1)
        return;

    params->read_video_queue_limit_set = 1;
    params->read_video_queue_limit_value = fcqlimitio.video_read_queue_limit;

    params->read_audio_queue_limit_set = 1;
    params->read_audio_queue_limit_value = fcqlimitio.audio_read_queue_limit;

    params->write_audio_queue_limit_set = 1;
    params->write_audio_queue_limit_value = fcqlimitio.audio_write_queue_limit;
}

int get_configured_tx_queue_limits(HAL *hp, 
                                   int *readvideolimit, 
                                   int *readaudiolimit,
                                   int *writeaudiolimit) {
    struct tx_av_buffers_params params;
    params.userptr = hp;
    retrieveTxAVBuffersValues(&params);
    *readvideolimit = params.read_video_queue_limit_value;
    *readaudiolimit = params.read_audio_queue_limit_value;
    *writeaudiolimit = params.write_audio_queue_limit_value;
    return 0;
}

int applyTxAVBuffers(struct tx_av_buffers_params *params,
        struct tx_av_buffers_params *orig_params)
{

    struct fc_queue_limits_io fcqlimitio;
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    if (hal_get_fc_queue_limits(hp, &fcqlimitio) == -1)
        return RC_ERROR;

    if (params->read_video_queue_limit_set == 1) {
        fcqlimitio.video_read_queue_limit = params->read_video_queue_limit_value;
    }
    if (params->read_audio_queue_limit_set == 1) {
        fcqlimitio.audio_read_queue_limit = params->read_audio_queue_limit_value;
    }
    if (params->write_audio_queue_limit_set == 1) {
         fcqlimitio.audio_write_queue_limit = params->write_audio_queue_limit_value;
    }
    if (hal_set_fc_queue_limits(hp, &fcqlimitio) == -1)
        return RC_ERROR;

    return RC_NORMAL;
}

void TxAVBuffersInit()
{

    registerTxAVBuffersParamsInitFunction
        (paramsInitTxAVBuffers);
}
