#ifndef __EDID_UTILS_H__
#define __EDID_UTILS_H__

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#ifndef __EDIDLIST_MENU_H_
#include "edid_list_menu.h"
#endif

#define EDID_TEXT_SIZE  4096
#define EDID_PAGER      "--PAGE BREAK--"

#define EDID_LUT_TYPE_NONE      0
#define EDID_LUT_TYPE_LUTID     1
#define EDID_LUT_TYPE_NEWLUT    2

struct edid_lut_map {
    int type;
    int value;
};

/***********************
 * Function prototpyes *
 ***********************/
int get_edid_list_params_by_lutid(struct edid_list_params *paramsp, int lutid);
void get_edid_product_id(unsigned char *edidbuf, char *prodid);
int print_edid_to_buffer(unsigned char *buf, 
                         char *edidtxt, 
                         char *pager, 
                         int print_all);
int print_edid_extension_to_buffer(unsigned char *buf, 
                                   char *edidtxt, 
                                   char *pager,
                                   int block);
int get_lut_from_edid_descriptor(unsigned char *buf,
                                 int block,
                                 struct LUT_struct *lutp);
int get_lut_from_edid_descriptor_extension(unsigned char *buf,
                                           int block,
                                           char *monname,
                                           struct LUT_struct *lutp);
int get_edid_lut_map(HAL *hp, 
                    unsigned char *edidbuf, 
                    int extension,
                    struct LUT_struct *lutp, 
                    struct edid_lut_map *edid_lut,
                    int mapsize);
int update_lut_from_buffer(HAL *hp, unsigned char *edidbuf);
int update_edid_list_from_buffer(HAL *hp, unsigned char *edidbuf);
int load_edid_eeprom_with_descriptor(HAL *hp, unsigned char videoinput);
int load_edid_eeprom_with_default(HAL *hp, unsigned char videoinput);
int load_edid_eeprom_from_monitor(HAL *hp);

#endif          /* __EDID_UTILS_H__ */
