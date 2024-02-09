#ifndef __XPI_I2C_CONTROL_H__
#define __XPI_I2C_CONTROL_H__

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#ifdef __XPI__
#ifndef __PLATFORM_NETVIZXPI_H__
#include "platform-netvizxpi.h"
#endif

#ifndef __NETVIZXPI_H_INCLUDE__
#include "netvizxpi/netvizxpi.h"
#endif
#endif

/* LM63 temperature sensor registers */
#define LM63REG_LT                  0x00
#define LM63REG_RTHB                0x01
#define LM63REG_CFG                 0x03
#define LM63REG_RTLB                0x10
#define LM63REG_RMTCRI_SETPOINT     0x19
#define LM63REG_TACHCNT_LOW         0x46
#define LM63REG_TACHCNT_HIGH        0x47
#define LM63REG_PWM_RPM             0x4a
#define LM63REG_SPINUP_CFG          0x4b
#define LM63REG_PWM_VAL             0x4c
#define LM63REG_PWM_FREQ            0x4d
#define LM63REG_MFG_ID              0xfe
#define LM63REG_REV_ID              0xff

/* MAX6650 Registers */
#define MAX6650REG_SPEED            0x00
#define MAX6650REG_CONFIG           0x02
#define MAX6650REG_GPIO_DEF         0x04
#define MAX6650REG_DAC              0x06
#define MAX6650REG_ALARM_EN         0x08
#define MAX6650REG_ALARM            0x0a
#define MAX6650REG_TACH0            0x0c
#define MAX6650REG_TACH1            0x0e
#define MAX6650REG_TACH2            0x10
#define MAX6650REG_TACH3            0x12
#define MAX6650REG_GPIO_STAT        0x14
#define MAX6650REG_COUNT            0x16
#define MAX6650REG_TACH1            0x0e
#define MAX6650REG_ALARM            0x0a

#ifdef __cplusplus
extern "C" {
#endif
    int xpi_get_main_i2c_val(HAL *hp,  
                             unsigned char port,
                             unsigned char addr,
                             unsigned short reg,
                             unsigned char *val);
    int xpi_set_main_i2c_val(HAL *hp,  
                             unsigned char port,
                             unsigned char addr,
                             unsigned short reg,
                             unsigned char val);
    int xpi_get_expansion_i2c_val(HAL *hp,  
                                  unsigned char port,
                                  unsigned char addr,
                                  unsigned short reg,
                                  unsigned char *val);
    int xpi_set_expansion_i2c_val(HAL *hp,  
                                  unsigned char port,
                                  unsigned char addr,
                                  unsigned short reg,
                                  unsigned char val);
    int xpi_get_monedid_val(HAL *hp,  
                            unsigned short reg,
                            unsigned char *val);
    int xpi_set_monedid_val(HAL *hp,  
                            unsigned short reg,
                            unsigned char val);
    int xpi_read_mon_edid(HAL *hp, unsigned char *buf, int bufsize);
    int xpi_read_edid_eeprom(HAL *hp, 
                             int subdev, 
                             unsigned char *buf, 
                             int bufsize);
    int xpi_load_edid_eeprom(HAL *hp, 
                             int subdev, 
                             unsigned char *buf, 
                             int bufsize);
#ifdef __cplusplus
}
#endif

#endif          /* __XPI_I2C_CONTROL_H__ */
