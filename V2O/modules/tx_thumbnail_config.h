/*
 * tx_thumbnail_config.h
 *
 *  Created on: Jun 13, 2013
 *      Author: rkale
 */

#ifndef TX_THUMBNAIL_CONFIG_H_
#define TX_THUMBNAIL_CONFIG_H_

struct tx_thumbnail_config {
    int enable;
    int enable_native;
    int enable_large_thumbnail;
    int enable_small_thumbnail;
    int large_thumbnail_width;
    int large_thumbnail_height;
    int small_thumbnail_width;
    int small_thumbnail_height;
    int thumbnail_interval;
};

#ifdef __cplusplus
extern "C" {
#endif

extern int get_tx_thumbnail_config(int board_number, struct tx_thumbnail_config *thumbnail_cfg);

#ifdef __cplusplus
}
#endif


#endif /* TX_THUMBNAIL_CONFIG_H_ */
