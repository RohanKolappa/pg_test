#ifndef __VLIB_H_INCLUDED__
#define __VLIB_H_INCLUDED__

#include <stdio.h>

#ifndef __FPGA_IOCTL_H_INCLUDED__
#ifdef __XPI__
#include "netvizxpi/netvizxpi.h"
#else
#include "fpga_ioctl.h"
#endif
#endif

#ifndef __FRAME_COUNTER_UTILS_H__
#include "frame_counter_utils.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    int hal_plx_read_k(HAL *hp, unsigned long offset, unsigned long *value);
    void hal_set_plx_val_k(HAL *hp, unsigned short reg, unsigned long value);
    unsigned long hal_get_plx_val_k(HAL *hp, unsigned short reg);

    void hal_set_plx_config_val_k(HAL *hp, 
                                  unsigned short reg, 
                                  unsigned long value);
    unsigned long hal_get_plx_config_val_k(HAL *hp, unsigned short reg);

    void hal_set_bar2_val_k(HAL *hp, unsigned long reg, unsigned long value);
    unsigned long hal_get_bar2_val_k(HAL *hp, unsigned long reg);

    void hal_set_bar3_val_k(HAL *hp, unsigned long reg, unsigned long value);
    unsigned long hal_get_bar3_val_k(HAL *hp, unsigned long reg);

    int hal_plx_eeprom_present(HAL *hp);
    void hal_set_plx_eeprom_val_k(HAL *hp, 
                                  unsigned short reg, 
                                  unsigned long value);
    unsigned long hal_get_plx_eeprom_val_k(HAL *hp, unsigned short reg);


    int hal_set_driver_enable(HAL *hp, int mode);
    int hal_get_board_status(HAL *hp);
                                 
    void hal_soft_reset_fpga(HAL *hp);
    int hal_activate_board(HAL *hp);
    int hal_deactivate_board(HAL *hp);

    int get_configured_rx_video_output(HAL *hp);
    void get_configured_rx_sync_polarities(HAL *hp, 
                                           int *lrsync, 
                                           int *hsync, 
                                           int *vsync);
    int get_configured_rx_genlock_params(HAL *hp, int *mode, int *delta);
    int get_configured_tx_video_output(HAL *hp);
    int get_configured_dvimode_auto(HAL *hp);
    int get_configured_optimize_latency(HAL *hp);
    unsigned long get_configured_iframe_interval(HAL *hp);
    int get_configured_tx_queue_limits(HAL *hp, 
                                       int *readvideolimit, 
                                       int *readaudiolimit,
                                       int *writeaudiolimit);
    int get_configured_rx_queue_limits(HAL *hp, 
                                       int *readaudiolimit, 
                                       int *writevideolimit, 
                                       int *writeaudiolimit);
#ifdef __cplusplus
}
#endif

#endif /* __VLIB_H_INCLUDED__ */

