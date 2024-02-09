#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "read_audio_statistics_status_menu_gen.h"

int getReadAudioStatisticsStatus(
        struct read_audio_statistics_status_params *params)
{
    struct fc_stat_io statio;

    HAL *hp = params->userptr;
    if (hp == NULL)
        return RC_ERROR;

    hal_get_fc_read_stats(hp, &statio);

    params->dma_requests_value = statio.audio_stats.dma_requests;
    params->dma_busy_value = statio.audio_stats.dma_busy;
    params->dma_lock_busy_value = statio.audio_stats.dma_lock_busy;
    params->dma_errors_value = statio.audio_stats.dma_errors;
    params->dma_grants_value = statio.audio_stats.dma_grants;
    params->fifo_empty_value = statio.audio_stats.fifo_empty;
    params->fifo_full_value = statio.audio_stats.fifo_full;
    params->fifo_reset_value = statio.audio_stats.fifo_reset;
    params->fops_requests_value = statio.audio_stats.fops_requests;
    params->fops_success_value = statio.audio_stats.fops_success;
    params->fops_errors_value = statio.audio_stats.fops_errors;
    params->fops_bytes_value = statio.audio_stats.fops_bytes;
    params->sequence_value = statio.audio_stats.sequence;
    params->queue_length_value = statio.audio_stats.queue_length;
    params->queue_full_value = statio.audio_stats.queue_full;
    params->queue_empty_value = statio.audio_stats.queue_empty;
    params->queue_limit_value = statio.audio_stats.queue_limit;
    params->dropped_value = statio.audio_stats.buffer_drops;
    params->buffer_pool_value = statio.audio_stats.buffer_pool;

    return RC_NORMAL;
}
