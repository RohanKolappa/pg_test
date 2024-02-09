#include <stdlib.h>
#include <sys/time.h>
#include <syslog.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "vlog.h"
#include "commonutils.h"
#include "text_utils.h"
#include "vlib.h"
#include "strmsrv_comm.h"
#include "lut_utils.h"
#include "rx_connection_status_menu_gen.h"


static int get_time_difference(void) 
{
   static struct timeval last_time = {0, 0};
   struct timeval now;
   gettimeofday(&now, (struct timezone *) 0);
   int timdiff = 1;
   int timediff = TIMEVAL_DIFF(now, last_time);
   if (timediff > 1000)
       timdiff = timediff;
   last_time = now;

   return timdiff;
}

int getRxConnectionStatus(struct rx_connection_status_params *params)
{
    static unsigned long last_slice_cnt = 0;
    static unsigned long last_frame_cnt = 0;
    static unsigned long last_audio_bytes = 0;
    static uint64_t last_reverse_audio_bytes = 0;
    static unsigned long last_video_bytes = 0;
    static unsigned long last_video_bits = 0;

    // Get HAL pointer
    HAL *hp = params->userptr;
    if (hp == NULL) {
        return RC_ERROR;
    }

    int timdiff = get_time_difference();

    // Current Dropside (i.e. remote) LUT Discription
    struct LUT_struct lut;
    memset(&lut, 0x00, sizeof(lut));
    if (hal_get_dropside_lut(hp, &lut) != -1) {
        if (lut.a_alutreg != 0 && lut.a_alutreg < RESERVED_RX_LUTID)
            snprintf(params->rlut_value,
                     FIELD_RX_CONNECTION_STATUS_RLUT_LENGTH,
                     "%d [%s]", lut.a_alutreg, lut.comment);
        else
            snprintf(params->rlut_value,
                     FIELD_RX_CONNECTION_STATUS_RLUT_LENGTH,
                     "%s", lut.comment);
    }
    else {
        sprintf(params->rlut_value, "????");
    }
    
    // Current Display (i.e. override) LUT Discription
    memset(&lut, 0x00, sizeof(lut));
    if (hal_get_display_lut(hp, &lut) != -1) {
        if (lut.a_alutreg != 0 && lut.a_alutreg < RESERVED_RX_LUTID)
            snprintf(params->dlut_value,
                     FIELD_RX_CONNECTION_STATUS_RLUT_LENGTH,
                     "%d [%s]", lut.a_alutreg, lut.comment);
        else
            snprintf(params->dlut_value,
                     FIELD_RX_CONNECTION_STATUS_RLUT_LENGTH,
                     "%s", lut.comment);
    }
    else {
        sprintf(params->dlut_value, "????");
    }

    // Current Display (i.e. override) LUT Discription
    memset(&lut, 0x00, sizeof(lut));
    if (hal_get_display_lut(hp, &lut) != -1) {
        if (lut.a_alutreg != 0 && lut.a_alutreg < RESERVED_RX_LUTID)
            snprintf(params->dlut_value,
                     FIELD_RX_CONNECTION_STATUS_DLUT_LENGTH,
                     "%d [%s]", lut.a_alutreg, lut.comment);
        else
            snprintf(params->dlut_value,
                     FIELD_RX_CONNECTION_STATUS_DLUT_LENGTH,
                     "%s", lut.comment);
    }
    else {
        sprintf(params->dlut_value, "????");
    }

    // Genlock mode
    int genlockmode = hal_getGenlockMode(hp);
    if (genlockmode == GENLOCK_MODE_MASTER)
        sprintf(params->genlock_status_value, "Master");
    else if (genlockmode == GENLOCK_MODE_SLAVE)
        sprintf(params->genlock_status_value, "Slave");
    else
        sprintf(params->genlock_status_value, "None");

    struct fc_wparam_io fcwpio;
    hal_get_fc_wparams(hp, &fcwpio);

    if (hal_get_hardware_type(hp) == XPI_DEVICE) {
        if (fcwpio.wframe_cnt < (last_frame_cnt % 0x10000))
            fcwpio.wframe_cnt += 0x10000;
        fcwpio.wframe_cnt += (last_frame_cnt / 0x10000) * 0x10000;
        if (fcwpio.wslice_cnt < (last_slice_cnt % 0x10000))
            fcwpio.wslice_cnt += 0x10000;
        fcwpio.wslice_cnt += (last_slice_cnt / 0x10000) * 0x10000;
    }

    // Frames/Slices
    params->video_frames_value = fcwpio.wframe_cnt;
    params->video_slices_value = fcwpio.wslice_cnt;

    // Compute Frame and Slice rates
    unsigned long frame_diff = fcwpio.wframe_cnt - last_frame_cnt;
    unsigned long slice_diff = fcwpio.wslice_cnt - last_slice_cnt;
    last_slice_cnt = fcwpio.wslice_cnt;
    last_frame_cnt = fcwpio.wframe_cnt;

    params->frame_rate_value = (((double) frame_diff) / timdiff) * 1000000.0;
    params->slice_rate_value = (((double) slice_diff) / timdiff) * 1000000.0;


    struct strmsrv_rx_connection_params rx_connection_params;
    int channel_number = hal_get_channel_number(hp);
    int rc = strmsrv_get_rx_connection_params(channel_number, &rx_connection_params);

    unsigned long video_bytes_diff = 0;
    unsigned long audio_bytes_diff = 0;
    uint64_t reverse_audio_bytes_diff = 0;
    if (rc == -1) {
        sprintf(params->server_value, "????");
        sprintf(params->status_value, "????");
        sprintf(params->current_bandwidth_value, "????");
        sprintf(params->video_bandwidth_value, "????");
        sprintf(params->audio_bandwidth_value, "????");
        sprintf(params->reverse_audio_bandwidth_value, "????");
        last_video_bytes = 0;
        last_audio_bytes = 0;
        last_reverse_audio_bytes = 0;
    } 
    else if (rx_connection_params.client_unavailable == 1) {
        last_video_bytes = 0;
        last_audio_bytes = 0;

        bytes_to_bits(0, 
                      params->video_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_VIDEO_BANDWIDTH_LENGTH);

        bytes_to_bits(0,
                      params->audio_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_AUDIO_BANDWIDTH_LENGTH);


        bytes_to_bits(0,
                      params->reverse_audio_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_REVERSE_AUDIO_BANDWIDTH_LENGTH);

        bytes_to_bits(0, 
                      params->current_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_CURRENT_BANDWIDTH_LENGTH);

        strncpy(params->server_value, 
               "Not Connected",
               FIELD_RX_CONNECTION_STATUS_SERVER_LENGTH);

        strcpy(params->status_value, "Idle");
    }
    else {
        uint64_t video_words = 0, audio_words = 0, reverse_audio_words = 0;
        if (hal_get_hardware_type(hp) == XPI_DEVICE) {
            video_words = (uint64_t) hal_get_xpi_videowords(hp);
            if (video_words < (last_video_bytes / 4)) {
                video_words += 0xffffffff;
            }
            video_bytes_diff = (video_words * 4) - last_video_bytes;
            last_video_bytes = video_words * 4;
            audio_words = (uint64_t) hal_get_xpi_audiowords(hp, 0);
            if (audio_words < (last_audio_bytes / 4)) {
                audio_words += 0xffffffff;
            }
            audio_bytes_diff = (audio_words * 4) - last_audio_bytes;
            last_audio_bytes = audio_words * 4;
            reverse_audio_words = (uint64_t) hal_get_xpi_audiowords(hp, 1);
            if (reverse_audio_words < (last_reverse_audio_bytes / 4)) {
                reverse_audio_words += 0xffffffff;
            }
            reverse_audio_bytes_diff = (reverse_audio_words * 4) - 
                last_reverse_audio_bytes;
            last_reverse_audio_bytes = reverse_audio_words * 4;

            rx_connection_params.video_bytes = last_video_bytes;
            rx_connection_params.video_packets = hal_get_xpi_videopkts(hp);
            rx_connection_params.video_loss = hal_get_xpi_videoloss(hp);
            rx_connection_params.audio_bytes = last_audio_bytes;
            rx_connection_params.audio_packets = hal_get_xpi_audiopkts(hp);
            rx_connection_params.reverse_audio_bytes = 
                last_reverse_audio_bytes; 
            rx_connection_params.audio_loss = hal_get_xpi_audioloss(hp);
            rx_connection_params.video_outoforder = 
                rx_connection_params.audio_outoforder = 
                rx_connection_params.queue_drops = 0;
        }
        else {
            video_bytes_diff = rx_connection_params.video_bytes - 
                last_video_bytes;
            audio_bytes_diff = rx_connection_params.audio_bytes - 
                last_audio_bytes;
            reverse_audio_bytes_diff = 
                rx_connection_params.reverse_audio_bytes - 
                last_reverse_audio_bytes;
        }

        last_video_bytes = rx_connection_params.video_bytes;
        last_audio_bytes = rx_connection_params.audio_bytes;
        last_reverse_audio_bytes = rx_connection_params.reverse_audio_bytes;

        double video_rate = ((double) video_bytes_diff * 1000000) / (timdiff);
        
        bytes_to_bits((int) video_rate, 
                      params->video_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_VIDEO_BANDWIDTH_LENGTH);

        double audio_rate = ((double) audio_bytes_diff * 1000000) / (timdiff);
        bytes_to_bits((int) audio_rate,
                      params->audio_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_AUDIO_BANDWIDTH_LENGTH);

        double total_rate = video_rate + audio_rate;
        
        bytes_to_bits((int) total_rate, 
                      params->current_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_CURRENT_BANDWIDTH_LENGTH);

        if (hal_get_hardware_type(hp) == XPI_DEVICE)
            params->current_bytes_value = video_words + audio_words;
        else
            params->current_bytes_value = rx_connection_params.video_bytes +
                rx_connection_params.audio_bytes;

        double reverse_audio_rate = 
            ((double) reverse_audio_bytes_diff * 1000000) / (timdiff);

        bytes_to_bits((int) reverse_audio_rate,
                      params->reverse_audio_bandwidth_value,
                      FIELD_RX_CONNECTION_STATUS_REVERSE_AUDIO_BANDWIDTH_LENGTH);

        if (hal_get_hardware_type(hp) == XPI_DEVICE)
            params->reverse_audio_bytes_value = reverse_audio_words;
        else
            params->reverse_audio_bytes_value = 
                rx_connection_params.reverse_audio_bytes;

        strncpy(params->server_value, 
               rx_connection_params.server,
               FIELD_RX_CONNECTION_STATUS_SERVER_LENGTH);

        strncpy(params->status_value, 
               rx_connection_params.status,
               FIELD_RX_CONNECTION_STATUS_STATUS_LENGTH);
        
        params->video_bytes_value = rx_connection_params.video_bytes;
        params->video_packets_value = rx_connection_params.video_packets;
        params->video_loss_value = rx_connection_params.video_loss;
        params->video_outoforder_value = rx_connection_params.video_outoforder;
        params->audio_bytes_value = rx_connection_params.audio_bytes;
        params->audio_packets_value = rx_connection_params.audio_packets;
        params->audio_loss_value = rx_connection_params.audio_loss +
                rx_connection_params.audio_outoforder;
        // params->audio_outoforder_value = 
        //      rx_connection_params.audio_outoforder;

        if (hal_get_hardware_type(hp) == XPI_DEVICE) {
            params->video_dropped_value = hal_get_xpi_videodrop(hp); 
            params->audio_dropped_value = hal_get_xpi_audiodrop(hp);
        }
        else {
            struct fc_stat_io fwstat;
            hal_get_fc_write_stats(hp, &fwstat);
            params->video_dropped_value = fwstat.video_stats.buffer_drops + 
                fwstat.video_stats.queue_full;
            params->audio_dropped_value = fwstat.audio_stats.buffer_drops +
                fwstat.audio_stats.queue_full;
        }
        params->queue_drops_value = rx_connection_params.queue_drops;
    }

    if (hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        params->video_output_value=1;
    }
    else {
        params->video_output_value=get_configured_rx_video_output(hp);
    }
    

    return RC_NORMAL;
}
