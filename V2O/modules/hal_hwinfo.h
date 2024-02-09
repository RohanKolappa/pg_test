#ifndef __HAL_HWINFO_H__
#define __HAL_HWINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

    extern int hal_get_board_number(HAL * hp);
    extern int hal_get_channel_number(HAL * hp);
    extern int hal_get_number_of_boards(HAL * hp);
    extern int hal_get_hardware_type(HAL * hp);
    extern int hal_get_board_type(HAL * hp);
    extern int hal_get_hardware_version(HAL * hp);
    extern int hal_get_hardware_revision(HAL * hp);
    extern int hal_get_hardware_detected(HAL * hp);
    extern int hal_check_main_lm63(HAL * hp);
    extern int hal_check_expansion_lm63(HAL * hp);
    extern int hal_check_max6650(HAL * hp);
    extern unsigned char hal_get_fpga_version(HAL * hp);
    extern int hal_get_hardware_description(HAL * hp, char *buf,
                                            int maxlen);
    extern char * hal_utils_port_name(int board_type);
    extern int hal_get_xpi_info(HAL *hp, char * info, int maxinfo);
#ifdef __cplusplus
}
#endif

#endif              
