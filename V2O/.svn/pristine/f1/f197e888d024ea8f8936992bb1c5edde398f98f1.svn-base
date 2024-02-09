/*
 * tx_rfb_config.h
 *
 *  Created on: Sep 30, 2009
 *      Author: rkale
 */

#ifndef TX_RFB_CONFIG_H_
#define TX_RFB_CONFIG_H_

struct tx_rfb_config {
    int enable;
    int port;
    char ipaddress[32 + 1];
    char password[32 + 1];
    int mouse_offset_x;
    int mouse_offset_y;
};

#ifdef __cplusplus
extern "C" {
#endif

extern int get_tx_rfb_config(HAL *hp, struct tx_rfb_config *rfb_cfg);

#ifdef __cplusplus
}
#endif

#endif /* TX_RFB_CONFIG_H_ */
