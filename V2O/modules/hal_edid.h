#ifndef __HAL_EDID_H__
#define __HAL_EDID_H__

#ifdef __cplusplus
extern "C" {
#endif
    int hal_read_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize);
    int hal_load_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize);
    int hal_read_mon_edid(HAL *hp, unsigned char *buf, int bufsize);
    int hal_read_mon_edid_extension(HAL *hp, 
                                    int block,
                                    unsigned char *buf, 
                                    int bufsize);
    int hal_check_mon_edid(HAL *hp);
#ifdef __cplusplus
}
#endif

#endif          /* __HAL_EDID_H__ */
