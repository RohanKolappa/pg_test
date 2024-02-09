/*
 * tx_connection_config.h
 *
 *  Created on: Oct 13, 2009
 *      Author: rkale
 */

#ifndef TX_CONNECTION_CONFIG_H_
#define TX_CONNECTION_CONFIG_H_


struct tx_connection_config {
    int bandwidth;
    double burst_ratio;
    int burst_duration;
    int low_compression;
    int high_compression;
    int av_option;
    int framerate_divider;
    int min_refresh;
    int max_refresh;
    int min_slice;
    int max_slice;
    int block_threshold;
    int enable_444;
    int kbm_switch;
    int kbm_idle_limit;
    int audio_type;
};

#ifdef __cplusplus
extern "C" {
#endif

extern int get_tx_connection_config(HAL *hp, struct tx_connection_config *connection_cfg);

#ifdef __cplusplus
}
#endif

#endif /* TX_CONNECTION_CONFIG_H_ */
