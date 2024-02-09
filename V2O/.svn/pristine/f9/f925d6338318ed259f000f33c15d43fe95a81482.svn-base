#include <stdlib.h>
#include <sys/time.h>
#include <syslog.h>

#define CUSTOM_MENU_DEFS
#define NO_LUT_DETECTED   "No LUT Detected"
#include "custmenu.h"

#include "hal.h"
#include "vlog.h"
#include "vlib.h"
#include "commonutils.h"
#include "text_utils.h"
#include "strmsrv_comm.h"
#include "tx_connection_status_menu_gen.h"

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

static void compute_compression(struct LUT_struct *current_lut,
                                int current_nobpslice,
                                unsigned long byte_diff,
                                unsigned long frame_diff,
                                unsigned long slice_diff,
                                int timdiff,
                                int read_percent,
                                char *line1, char *line2)
{
    double xval, wval, tval, fval, pval, sval, cval;

    xval = (double) ((current_lut->a_orig_hres +
                      current_lut->a_hfp +
                      current_lut->a_hbp +
                      current_lut->a_hsync) *
                     (current_lut->a_orig_vres +
                      current_lut->a_vfp +
                      current_lut->a_vbp + current_lut->a_vsync) * 3);

    wval =
        (double) (current_lut->a_orig_hres * current_lut->a_orig_vres * 3);

    if (byte_diff > 0 && wval > 0) {
        pval = xval / wval;
        fval =
            current_lut->a_refresh / ((((double) frame_diff) / timdiff) *
                                      1000000.0);
        sval =
            ((wval / (double) (64 * current_nobpslice * 3)) * frame_diff) /
            slice_diff;
        cval =
            (((double) (64 * current_nobpslice * 3)) / byte_diff) *
            slice_diff;

        tval =
            (xval / byte_diff) * ((double) timdiff / 1000000.0) *
            current_lut->a_refresh;

        if (read_percent) {
            pval = ((double) 1.0 - (double) 1.0 / pval) * 100.0;
            fval = ((double) 1.0 - (double) 1.0 / fval) * 100.0;
            sval = ((double) 1.0 - (double) 1.0 / sval) * 100.0;
            cval = ((double) 1.0 - (double) 1.0 / cval) * 100.0;
            tval = ((double) 1.0 - (double) 1.0 / tval) * 100.0;
            sprintf(line1, "Pch %5.2f%% Frm %5.2f%% Slc %5.2f%%", pval, fval,
                    sval);
            sprintf(line2, "Real %5.2f%%     Total %.2f %%", cval, tval);

        }
        else {
            sprintf(line1, "Pch %5.1f  Frm %5.1f  Slc %5.1f", pval, fval,
                    sval);
            sprintf(line2, "Real %5.1f     Total %.1f", cval, tval);
        }
    }
    else {
        sprintf(line1, " ");
        sprintf(line2, " ");
    }

}

int getTxConnectionStatus(struct tx_connection_status_params *params)
{
    static unsigned long last_slice_cnt = 0;
    static unsigned long last_frame_cnt = 0;
    static uint64_t last_audio_bytes = 0;
    static uint64_t last_reverse_audio_bytes = 0;
    static uint64_t last_video_bytes = 0;

    // Get HAL pointer
    HAL *hp = params->userptr;
    if (hp == NULL) {
        return RC_ERROR;
    }

    int timdiff = get_time_difference();
    int hardware_type = hal_get_hardware_type(hp);

    // Get flow control params from driver
    struct fc_param_io fcpio;
    hal_get_fc_params(hp, &fcpio);

    if (hardware_type == XPI_DEVICE) {
        if (fcpio.frame_cnt < (last_frame_cnt % 0x10000)) 
            fcpio.frame_cnt += 0x10000;
        fcpio.frame_cnt += (last_frame_cnt / 0x10000) * 0x10000; 
        if (fcpio.slice_cnt < (last_slice_cnt % 0x10000)) 
            fcpio.slice_cnt += 0x10000;
        fcpio.slice_cnt += (last_slice_cnt / 0x10000) * 0x10000; 
    }

    // Compute Frame and Slice rates
    unsigned long frame_diff = fcpio.frame_cnt - last_frame_cnt;
    unsigned long slice_diff = fcpio.slice_cnt - last_slice_cnt;
    last_slice_cnt = fcpio.slice_cnt;
    last_frame_cnt = fcpio.frame_cnt;

    params->frame_rate_value = (((double) frame_diff) / timdiff) * 1000000.0;
    params->slice_rate_value = (((double) slice_diff) / timdiff) * 1000000.0;

    // Frame/Slice Counts
    params->frames_value = fcpio.frame_cnt;
    params->slices_value = fcpio.slice_cnt;

    // Get Bandwidth info from StreamingServer
    struct strmsrv_tx_connection_params tx_connection_params;
    int channel_number = hal_get_channel_number(hp);
    unsigned long long video_bytes_diff = 0;
    unsigned long long audio_bytes_diff = 0;
    unsigned long long reverse_audio_bytes_diff = 0;

    int rc = strmsrv_get_tx_connection_params(channel_number, 
                &tx_connection_params);

    if (rc == -1) {
        sprintf(params->client_value, " ");
        sprintf(params->status_value, "Idle");
        sprintf(params->max_bandwidth_value, "0.00");
        sprintf(params->video_bandwidth_value, "0.00");
        sprintf(params->audio_bandwidth_value, "0.00");
        sprintf(params->lut_value, NO_LUT_DETECTED);
    }
    else {
        // Compute bandwidth
        uint64_t video_words = 0, audio_words = 0, reverse_audio_words = 0;

        bytes_to_bits(tx_connection_params.bandwidth, 
                      params->max_bandwidth_value,
                      FIELD_TX_CONNECTION_STATUS_MAX_BANDWIDTH_LENGTH);

        if (hal_get_hardware_type(hp) == XPI_DEVICE) {
            video_words = (uint64_t) hal_get_xpi_videowords(hp);
            audio_words = (uint64_t) hal_get_xpi_audiowords(hp, 1);
            reverse_audio_words = (uint64_t) hal_get_xpi_audiowords(hp, 0);
            if (video_words < (last_video_bytes / 4)) {
                video_words += 0xffffffff;
            }
            if (audio_words < (last_audio_bytes / 4)) {
                audio_words += 0xffffffff;
            }
            if (reverse_audio_words < (last_reverse_audio_bytes / 4)) {
                reverse_audio_words += 0xffffffff;
            }
            video_bytes_diff = (video_words * 4) - last_video_bytes;
            last_video_bytes = video_words * 4; 
            audio_bytes_diff = (audio_words * 4) - last_audio_bytes;
            last_audio_bytes = audio_words * 4; 
            reverse_audio_bytes_diff = (reverse_audio_words * 4) - 
                last_reverse_audio_bytes;
            last_reverse_audio_bytes = reverse_audio_words * 4; 

        }
        else {
            video_bytes_diff = tx_connection_params.video_bytes - 
                last_video_bytes;
            audio_bytes_diff = tx_connection_params.audio_bytes - 
                last_audio_bytes;
            reverse_audio_bytes_diff = 
                tx_connection_params.reverse_audio_bytes - 
                last_reverse_audio_bytes;
            last_video_bytes = tx_connection_params.video_bytes;
            last_audio_bytes = tx_connection_params.audio_bytes;
            last_reverse_audio_bytes = tx_connection_params.reverse_audio_bytes;
        }

        double video_rate = ((double) video_bytes_diff * 1000000) / (timdiff);
        
        bytes_to_bits((int) video_rate, 
                      params->video_bandwidth_value,
                      FIELD_TX_CONNECTION_STATUS_VIDEO_BANDWIDTH_LENGTH);

        double audio_rate = ((double) audio_bytes_diff * 1000000) / (timdiff);
        bytes_to_bits((int) audio_rate,
                      params->audio_bandwidth_value,
                      FIELD_TX_CONNECTION_STATUS_AUDIO_BANDWIDTH_LENGTH);

        double reverse_audio_rate = ((double) reverse_audio_bytes_diff * 
                1000000) / (timdiff);
        bytes_to_bits((int) reverse_audio_rate,
                      params->reverse_audio_bandwidth_value,
                      FIELD_TX_CONNECTION_STATUS_REVERSE_AUDIO_BANDWIDTH_LENGTH);

        if (hal_get_hardware_type(hp) == XPI_DEVICE) {
            params->video_bytes_value = video_words * 4;
            params->audio_bytes_value = audio_words * 4;
            params->reverse_audio_bytes_value = reverse_audio_words * 4;
        }
        else {
            params->video_bytes_value = tx_connection_params.video_bytes;
            params->audio_bytes_value = tx_connection_params.audio_bytes;
            params->reverse_audio_bytes_value = 
                tx_connection_params.reverse_audio_bytes;
        }
        if (tx_connection_params.connected) {
            strncpy(params->client_value, 
                tx_connection_params.client,
                FIELD_TX_CONNECTION_STATUS_CLIENT_LENGTH);
            strcpy(params->status_value, "Connected");
            if (tx_connection_params.multicast)
                strcat(params->status_value, " [Multicast]");
        }
        else {
            strncpy(params->client_value, "Not Connected", 
                    FIELD_TX_CONNECTION_STATUS_CLIENT_LENGTH);
            strcpy(params->status_value, "Idle");
            sprintf(params->max_bandwidth_value, "0.00");
        }

        // Current LUT from StreamingServer
        if (strcmp(tx_connection_params.lut, "") == 0)
            sprintf(params->lut_value, NO_LUT_DETECTED);
        else            
            strncpy(params->lut_value, tx_connection_params.lut, 
                    FIELD_TX_CONNECTION_STATUS_LUT_LENGTH);
    }

    // Current addside LUT 
    struct LUT_struct current_alut;
    memset(&current_alut, 0x00, sizeof(current_alut));
    hal_get_addside_lut(hp, &current_alut);

    if (strlen(current_alut.comment) > 0)
        snprintf(params->lut_value,
                 FIELD_TX_CONNECTION_STATUS_LUT_LENGTH,
                 "%d [%s]", current_alut.a_alutreg, current_alut.comment);
    else
        strcpy(params->lut_value, "");

    // Interface info
    int active = hal_getActiveIntf(hp);
    if (active & INTERFACE_DIGITAL) {
        if (active  & INTERFACE_DUALLINK) {
            strcpy(params->interface_value, "DVI-D");
        }
        else {
            strcpy(params->interface_value, "DVI-S");
        }
    }
    else if (active & INTERFACE_HV) {
        strcpy(params->interface_value, "Analog HV");
    }
    else if (active & INTERFACE_SOG) {
        strcpy(params->interface_value, "Analog SOG");
    }
    else if (active & INTERFACE_COMPOSITE) {
        strcpy(params->interface_value, "Composite");
    }
    else {
        strcpy(params->interface_value, "???");
    }

    // Frequency info
    double freq;
    freq = hal_getHorizontalFrequency(hp);
    if (freq > 0.01 && freq < 1000.0)
        params->horizontal_frequency_value = freq;

    freq = hal_getVerticalFrequency(hp);
    if (freq > 0.01 && freq < 1000.0)
        params->vertical_frequency_value = freq; 

    // Horizontal lines
    int lines = hal_getHorizontalLines(hp);
    if (lines > 0 && lines < 10000)
        params->horizontal_lines_value = lines;

    // Stereo?
    params->stereo_value = hal_getStereo(hp);

    // Compute compression info
    int current_nobpslice = 128;
    hal_get_addside_nobpslice(hp, &current_nobpslice);

    compute_compression(&current_alut,
                        current_nobpslice,
                        video_bytes_diff,
                        frame_diff,
                        slice_diff,
                        timdiff,
                        0,
                        params->compression_line_1_value, 
                        params->compression_line_2_value);


    if (hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        params->video_output_value = 1;
    }
    else { 
        params->video_output_value = hal_get_video_output(hp);
    }
    return RC_NORMAL;
}


