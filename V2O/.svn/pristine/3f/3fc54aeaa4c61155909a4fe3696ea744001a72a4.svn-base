#ifndef __HAL_VPD_H__
#define __HAL_VPD_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    int hal_plx_vpd_read_k(HAL *hp, unsigned long offset, unsigned long *val);
    int hal_plx_vpd_write_k(HAL *hp, unsigned long offset, unsigned long val);

    int hal_get_board_serial_number(HAL * hp, char *buf, int maxlen);
    unsigned long hal_get_board_vpd_date(HAL * hp);
    int hal_get_board_manufacture_date(HAL * hp, char *buf, int maxlen);
    int hal_get_board_revision(HAL * hp, char *buf, int maxlen);
    int hal_get_board_vpd(HAL *hp, unsigned long *mdate, unsigned long *serial,
            unsigned long *revision);
    int hal_set_board_vpd(HAL *hp, unsigned long mdate, unsigned long *serial,
            unsigned long revision);
                                 
#ifdef __cplusplus
}
#endif

#endif /* __HAL_VPD_H__ */

