#ifndef __HAL_LED_H__
#define __HAL_LED_H__

#ifdef __cplusplus
extern "C" {
#endif
    extern int hal_LED_status(HAL * hp, int lednumb);
    extern void hal_LED_on(HAL * hp, int lednumb);
    extern void hal_LED_off(HAL * hp, int lednumb);
    extern void hal_LED_blink(HAL * hp, int lednumb, int blinkrate);
    extern void hal_LED_all_on(HAL * hp);
    extern void hal_LED_all_off(HAL * hp);
    extern void hal_LED_all_normal(HAL * hp);
    extern void hal_LED_keepalive(HAL * hp, int lednumb, int sec);
    extern void hal_all_LED_status(HAL * hp, int *on, int *blink);
    extern void hal_set_all_LEDs(HAL * hp, int on, int blink);
#ifdef __cplusplus
}
#endif

//define LEDs for XPi
//Controlled by Processor GPIO
#define MAIN_RJ45_GREEN         101
#define MAIN_RJ45_YELLOW        113
#define MAIN_RJ45_ORANGE        100

//Controlled by Expansion FPGA / S3 FPGA
#define ENCODER_LED_GREEN       0
#define DECODER_LED_GREEN       1
#define VIDEOOUT_LED_GREEN      2
#define VIDEOOUT_LED_RED        3
#define VIDEOIN_LED_GREEN       4
#define VIDEOIN_LED_RED         5
#define EXPRJ45_ORANGE          6
#define EXPRJ45_GREEN           7
#define EXPRJ45_YELLOW          8
#define MIN_EXPANSION_LED       ENCODER_LED_GREEN
#define MAX_EXPANSION_LED       EXPRJ45_YELLOW
#define EXPANSION_LED_MASK      0x000001ff
#define BLINKING_LED_MASK       0x0000003f
#define EXPRJ45_LED_MASK        0x000001c0

#define MAINRJ45_GREEN_BIT      9
#define MAINRJ45_YELLOW_BIT     10
#define MAINRJ45_ORANGE_BIT     11
#define MAIN_LED_MASK           0x00000e00

#define S3_LED_REG              0x10
#define S3_LED_BLINK_REG        0x14

//Controlled by Processor GPIO
#define MAINRJ45_GREEN          101
#define MAINRJ45_YELLOW         113
#define MAINRJ45_ORANGE         100

#define LED_OFF                 0
#define LED_ON                  1
#define LED_BLINK               2

#endif                   
