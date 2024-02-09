#ifndef __LUTLIST_LIB_H__
#define __LUTLIST_LIB_H__

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#ifndef __HAL_HWINFO_H__
#include "hal_hwinfo.h"
#endif

#ifndef __LUT_LIST_MENU_GEN_H_
#include "lut_list_menu_gen.h"
#endif
#define RESERVED_MIN_STD_LUTID  0x80
#define RESERVED_TX_LUTID       0xaa
#define RESERVED_RX_LUTID       0xab
#define RESERVED_OVERRIDE_LUTID 0xac
#define RESERVED_INVALID_LUTID  0xff

/***********************
 * Function prototpyes *
 ***********************/
int get_lut_list_values(HAL *hp);
int get_lut_list_params_by_index(struct lut_list_params *params, int index);
int get_lut_list_params_by_lutid(struct lut_list_params *params, int lutid);
int parse_lut(HAL *hp, struct list_head *lut_head);
void free_lut(struct list_head *lut_head);
int compare_luts(struct LUT_struct *lut1p, struct LUT_struct *lut2p);
int compare_luts_without_polarity(struct LUT_struct *lut1p, 
                                  struct LUT_struct *lut2p);
int get_lut_entry_by_lutid(HAL *hp, struct LUT_struct *lutp, int lutid);
int get_matching_lut_entry(HAL *hp, struct LUT_struct *lutp);
int find_matching_lut_entry(HAL *hp, 
                            unsigned short hsyncc, 
                            unsigned short amvreg, 
                            unsigned short amhreg, 
                            unsigned short amtreg, 
                            int duallink, 
                            struct LUT_struct *lutp);
int get_unique_lutid(HAL *hp);
int add_lut_entry(HAL *hp, struct LUT_struct *lutp);
int build_lut_entry(struct LUT_struct *lp, int device_type);

#endif          /* __LUTLIST_LIB_H__ */
