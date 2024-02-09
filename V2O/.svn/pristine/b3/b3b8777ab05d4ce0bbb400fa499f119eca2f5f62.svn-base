#ifndef __RX_KBM_CONFIG_H_
#define __RX_KBM_CONFIG_H_

struct rx_kbm_config {
    int kbm_mode;               /* Remote or Local */
    int local_keyboard_type;    /* keyboard type for Local or Control mode */ 
    int local_mouse_type;       /* mouse type for Local or Control mode */ 
};

#ifdef __cplusplus
extern "C" {
#endif
extern int get_rx_kbm_config(HAL *hp, struct rx_kbm_config *kbm_cfg);
#ifdef __cplusplus
}
#endif
#endif                          /* __RX_KBM_CONFIG_H_ */
