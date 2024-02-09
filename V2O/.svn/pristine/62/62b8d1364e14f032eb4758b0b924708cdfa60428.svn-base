#ifndef __TEMPERATURE_CONTROL_H__
#define __TEMPERATURE_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif
    int hal_get_board_temperature(HAL *hp, float *ltemp, float *rtemp);
    void hal_init_fan_speed(HAL *hp, int speed);
    int hal_get_fan_speed(HAL *hp);
    void hal_set_fan_speed(HAL *hp, int speed);
    int hal_get_fan_rpm(HAL *hp);
    int hal_monitor_temperature(HAL *hp, int reset_samples);
    int hal_get_xpi_fpga_temperature(HAL *hp, float *temp); 
    int hal_get_xpi_cpu_temperature(HAL *hp, float *temp); 
    int hal_get_xpi_phy_temperature(HAL *hp, float *temp);
#ifdef __cplusplus
}
#endif

#endif /* __TEMPERATURE_CONTROL_H_INCLUDED__ */

