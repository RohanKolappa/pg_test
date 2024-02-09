#ifndef __I2C_CONTROL_H__
#define __I2C_CONTROL_H__

/* XP200 I2C device addresses */
#define PCA9564_I2C_ADDR        0x55
#define PM_PCA9545A_I2C_ADDR    0x71
#define AK4683AD_I2C_ADDR       0x10
#define AK4683AA_I2C_ADDR       0x12
#define PM_PCA9555_I2C_ADDR     0x20
#define EDID_EEPROM_I2C_ADDR    0x50
#define MON_EDID_I2C_ADDR       0x50
#define USB_EEPROM_I2C_ADDR     0x50
#define PCA9545A_I2C_ADDR       0x73
#define PCA9555_CLK_I2C_ADDR    0x21
#define PCA9555_DVI_I2C_ADDR    0x22
#define AD9888_I2C_ADDR         0x4d
#define SII1178_1_I2C_ADDR      0x38
#define SII1178_2_I2C_ADDR      0x39
#define DDRAM_1_I2C_ADDR        0x52
#define DDRAP_1_I2C_ADDR        0x32
#define DDRAM_2_I2C_ADDR        0x53
#define DDRAP_2_I2C_ADDR        0x33
#define LM63_I2C_ADDR           0x4c
#define CDCE_I2C_ADDR_MIN       0x68
#define CDCE_I2C_ADDR_MAX       0x6b
#define CDCE706_I2C_ADDR        0x69
#define CDCE906_I2C_ADDR        0x69
#define CDCE906_I2C_ADDR        0x69
#define CDCE906_I2C_ADDR        0x69
#define BAD_I2C_ADDR            0x77

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


/* Valid XP200 I2C device addresses */
#define VALID_I2C_ADDR(a)    \
        (((a) == PM_PCA9545A_I2C_ADDR) || \
         ((a) == AK4683AD_I2C_ADDR) || \
         ((a) == AK4683AA_I2C_ADDR) || \
         ((a) == PM_PCA9555_I2C_ADDR) || \
         ((a) == USB_EEPROM_I2C_ADDR) || \
         ((a) == EDID_EEPROM_I2C_ADDR) || \
         ((a) == MON_EDID_I2C_ADDR) || \
         ((a) == PCA9545A_I2C_ADDR) || \
         ((a) == PCA9555_CLK_I2C_ADDR) || \
         ((a) == PCA9555_DVI_I2C_ADDR) || \
         ((a) >= CDCE_I2C_ADDR_MIN && (a) <= CDCE_I2C_ADDR_MAX) || \
         ((a) == AD9888_I2C_ADDR) || \
         ((a) == SII1178_1_I2C_ADDR) || \
         ((a) == SII1178_2_I2C_ADDR) || \
         ((a) == DDRAM_1_I2C_ADDR) || \
         ((a) == DDRAP_1_I2C_ADDR) || \
         ((a) == DDRAM_2_I2C_ADDR) || \
         ((a) == DDRAP_2_I2C_ADDR) || \
         ((a) == LM63_I2C_ADDR))

#define VALID_MAIN_I2C_ADDR(a)    \
        (((a) == EDID_EEPROM_I2C_ADDR) || \
         ((a) == PCA9545A_I2C_ADDR) || \
         ((a) == PCA9555_CLK_I2C_ADDR) || \
         ((a) == PCA9555_DVI_I2C_ADDR) || \
         ((a) >= CDCE_I2C_ADDR_MIN && (a) <= CDCE_I2C_ADDR_MAX) || \
         ((a) == AD9888_I2C_ADDR) || \
         ((a) == SII1178_1_I2C_ADDR) || \
         ((a) == SII1178_2_I2C_ADDR) || \
         ((a) == DDRAM_1_I2C_ADDR) || \
         ((a) == DDRAP_1_I2C_ADDR) || \
         ((a) == DDRAM_2_I2C_ADDR) || \
         ((a) == DDRAP_2_I2C_ADDR) || \
         ((a) == LM63_I2C_ADDR))

#define VALID_PM_I2C_ADDR(a)    \
        (((a) == PM_PCA9545A_I2C_ADDR) || \
         ((a) == AK4683AD_I2C_ADDR) || \
         ((a) == AK4683AA_I2C_ADDR) || \
         ((a) == PM_PCA9555_I2C_ADDR) || \
         ((a) == USB_EEPROM_I2C_ADDR) || \
         ((a) >= CDCE_I2C_ADDR_MIN && (a) <= CDCE_I2C_ADDR_MAX) || \
         ((a) == LM63_I2C_ADDR))

/* I2C device register range */
#define MAX_I2C_REG(a)    \
        (((a) == PM_PCA9545A_I2C_ADDR) ? 0x00 : \
        (((a) == AK4683AD_I2C_ADDR) ? 0x1f : \
        (((a) == AK4683AA_I2C_ADDR) ? 0x13 : \
        (((a) == PM_PCA9555_I2C_ADDR) ? 0x07 : \
        (((a) == USB_EEPROM_I2C_ADDR) ? 0x7f : \
        (((a) == EDID_EEPROM_I2C_ADDR) ? 0x7f : \
        (((a) == PCA9545A_I2C_ADDR) ? 0x00 : \
        (((a) == PCA9555_CLK_I2C_ADDR) ? 0x07 : \
        (((a) == PCA9555_DVI_I2C_ADDR) ? 0x07 : \
        (((a) >= CDCE_I2C_ADDR_MIN && (a) <= CDCE_I2C_ADDR_MAX) ? 0x1a : \
        (((a) == AD9888_I2C_ADDR) ? 0x19 : \
        (((a) == SII1178_1_I2C_ADDR) ? 0x0f : \
        (((a) == SII1178_2_I2C_ADDR) ? 0x0f : \
        (((a) == LM63_I2C_ADDR) ? 0xff : 0xffff))))))))))))))

#define I2C_REG_NONE                -1

#define I2CIO_TYPE_READ             1
#define I2CIO_TYPE_WRITE            2
#define I2CIO_TYPE_MAIN_READ        3
#define I2CIO_TYPE_MAIN_WRITE       4
#define I2CIO_TYPE_PM_READ          5
#define I2CIO_TYPE_PM_WRITE         6
#define I2CIO_TYPE_EDIDEEPROM_READ  7
#define I2CIO_TYPE_EDIDEEPROM_WRITE 8
#define I2CIO_TYPE_MONEDID_READ     9
#define I2CIO_TYPE_MONEDID_WRITE    10

#ifdef __cplusplus
extern "C" {
#endif
    void hal_set_i2c_debug(HAL *hp, int val);
    void hal_initialize_i2c(HAL *hp);
    int hal_read_i2c_val_k(HAL *hp, int type, unsigned short addr,
                           unsigned short reg, int numbytes,
                           unsigned char *dat);
    int hal_write_i2c_val_k(HAL *hp, int type, unsigned short addr,
                            unsigned short reg, int numbytes,
                            unsigned char *dat);
    void hal_set_main_i2c_val_k(HAL *hp,
                                unsigned short addr,
                                unsigned short reg,
                                unsigned char value);
    unsigned char hal_get_main_i2c_val_k(HAL *hp,
                                         unsigned short addr,
                                         unsigned short reg);
    void hal_set_pm_i2c_val_k(HAL *hp,
                              unsigned short addr,
                              unsigned short reg,
                              unsigned char value);
    unsigned char hal_get_pm_i2c_val_k(HAL *hp,
                                       unsigned short addr,
                                       unsigned short reg);

    void hal_set_pca9555_clk_val_k(HAL *hp, 
                                   unsigned short reg, 
                                   unsigned char value);
    unsigned char hal_get_pca9555_clk_val_k(HAL *hp, unsigned short reg);

    void hal_set_ak4683ad_val_k(HAL *hp, 
                                unsigned short reg, 
                                unsigned char value);
    void hal_set_ak4683aa_val_k(HAL *hp, 
                                unsigned short reg, 
                                unsigned char value);

    void hal_set_pm_pca9555_val_k(HAL *hp, 
                                  unsigned short reg, 
                                  unsigned char value);
    unsigned char hal_get_pm_pca9555_val_k(HAL *hp, unsigned short reg);

    void hal_set_pca9545a_val_k(HAL *hp, 
                                unsigned short reg, 
                                unsigned char value);
    unsigned char hal_get_pca9545a_val_k(HAL *hp, unsigned short reg);

    void hal_set_pca9555_dvi_val_k(HAL *hp, 
                                   unsigned short reg, 
                                   unsigned char value);
    unsigned char hal_get_pca9555_dvi_val_k(HAL *hp, unsigned short reg);

    void hal_set_v4_ad9888_val_k(HAL *hp, 
                                 unsigned short reg, 
                                 unsigned char value);
    unsigned char hal_get_v4_ad9888_val_k(HAL *hp, unsigned short reg);

    void hal_set_sii1178_1_val_k(HAL *hp, 
                                 unsigned short reg, 
                                 unsigned char value);
    unsigned char hal_get_sii1178_1_val_k(HAL *hp, unsigned short reg);

    void hal_set_sii1178_2_val_k(HAL *hp, 
                                 unsigned short reg, 
                                 unsigned char value);
    unsigned char hal_get_sii1178_2_val_k(HAL *hp, unsigned short reg);

    int xp_read_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize);
    int xp_load_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize);
    int xp_read_mon_edid(HAL *hp, unsigned char *buf, int bufsize);
    int xp_read_mon_edid_extension(HAL *hp, 
                                   int block,
                                   unsigned char *buf, 
                                   int bufsize);

#ifdef __cplusplus
}
#endif

#endif          /* __I2C_CONTROL_H__ */
