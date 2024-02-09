#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "hal.h"
#include "hal_LED.h"
#include "commonutils.h"
#include "kbms_utils.h"
#include "vlog.h"

#ifdef __XPI__
#include "xpi_gpio_control.h"
#else
#include "xp_i2c_control.h"
#include "led_control.h"
#endif

#ifdef __XPI__
#define EXPANSION_LED(x) \
    (((x) >= ENCODER_LED_GREEN) && ((x) <= EXPRJ45_YELLOW))
#define MAIN_LED(x) \
    (((x) == MAINRJ45_GREEN) || \
     ((x) == MAINRJ45_YELLOW) || \
     ((x) == MAINRJ45_ORANGE))
#define BLINKING_LED(x) \
    (((x) >= ENCODER_LED_GREEN) && ((x) <= VIDEOIN_LED_RED))
#endif

int hal_LED_status (HAL *hp, int lednumb) 
{
    int retval = LED_OFF;
#ifdef __XPI__
    /* Expansion RJ45 LEDs are turned on when the corresponding */
    /* bit in the expansion FPGA register S3_LED_REG is 0 while */
    /* all others are turned on when the corresponding bit is 1 */
    if (EXPANSION_LED(lednumb)) {
        if (lednumb < EXPRJ45_ORANGE) {
            if (hal_get_s3fpga_val32_k(hp, S3_LED_BLINK_REG) & (1<<lednumb))
                retval = LED_BLINK;
            else if (hal_get_s3fpga_val32_k(hp, S3_LED_REG) & (1<<lednumb)) 
                retval = LED_ON;
            else
                retval = LED_OFF;
        }
        else {
            if (hal_get_s3fpga_val32_k(hp, S3_LED_BLINK_REG) & (1<<lednumb))
                retval = LED_BLINK;
            else if (!(hal_get_s3fpga_val32_k(hp, S3_LED_REG) & (1<<lednumb))) 
                retval = LED_ON;
            else
                retval = LED_OFF;
        }
    } 
    else if (MAIN_LED(lednumb)) {
        int val;
        /* Main RJ45 LEDs are on when GPIO pin value is 0 */
        if (!xpi_get_gpio_pin_value(lednumb, &val))
            retval = (!val) ? LED_ON : LED_OFF;
        else
            retval = LED_OFF;
    }
#endif
    return retval;
}

void hal_LED_on(HAL *hp, int lednumb)
{
#ifdef __XPI__
    /* Expansion RJ45 LEDs are turned on when the corresponding */
    /* bit in the expansion FPGA register S3_LED_REG is 0 while */
    /* all others are turned on when the corresponding bit is 1 */
    if (EXPANSION_LED(lednumb)) {
        if (lednumb >= EXPRJ45_ORANGE && lednumb <= EXPRJ45_YELLOW) {
            hal_and_in_s3fpga_val32_k(hp, S3_LED_REG, ~(1<<lednumb));
        }
        else {
            hal_or_in_s3fpga_val32_k(hp, S3_LED_REG, 1<<lednumb);
        }
        hal_and_in_s3fpga_val32_k(hp, S3_LED_BLINK_REG, ~(1<<lednumb));
    }
    else if (MAIN_LED(lednumb))
        xpi_set_gpio_pin_value(lednumb, 0);
#else
    int hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == V2DI50_DEVICE) {
        struct led_io ledio;
        ledio.lednumb = lednumb;
        ledio.leddata = LED_ON;
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LEDSTATE, &ledio);
    }
    else if (hardware_type == PLX_DEVICE) {
        int avflag = hal_getAudioVideoFlag(hp);
        if (lednumb == CONNECTION_LED) {
            hal_setAudioVideoFlag(hp, avflag | AVFLAG_CONNECTIONLED);
        }
        else if (lednumb == VIDEO_LED) {
            unsigned char regval = hal_get_pca9555_clk_val_k(hp, 0x03);
            hal_setAudioVideoFlag(hp, avflag | AVFLAG_VIDEOLED);
            /* Set port 1, pin 0 to 1 */
            hal_set_pca9555_clk_val_k(hp, 0x03, regval | 0x40);
        }
    }
#endif
}

void hal_LED_off(HAL *hp, int lednumb)
{
#ifdef __XPI__
    if (EXPANSION_LED(lednumb)) {
        if (lednumb >= EXPRJ45_ORANGE && lednumb <= EXPRJ45_YELLOW) {
            hal_or_in_s3fpga_val32_k(hp, S3_LED_REG, (1<<lednumb));
        }
        else {
            hal_and_in_s3fpga_val32_k(hp, S3_LED_REG, ~(1<<lednumb));
        }
        hal_and_in_s3fpga_val32_k(hp, S3_LED_BLINK_REG, ~(1<<lednumb));
    }
    else if (MAIN_LED(lednumb))
        xpi_set_gpio_pin_value(lednumb, 1);
#else
    int hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == V2DI50_DEVICE) {
        struct led_io ledio;
        ledio.lednumb = lednumb;
        ledio.leddata = LED_OFF;
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LEDSTATE, &ledio);
    }
    else if (hardware_type == PLX_DEVICE) {
        int avflag = hal_getAudioVideoFlag(hp);
        if (lednumb == CONNECTION_LED) {
            hal_setAudioVideoFlag(hp, avflag & ~AVFLAG_CONNECTIONLED);
        }
        else if (lednumb == VIDEO_LED) {
            unsigned char regval = hal_get_pca9555_clk_val_k(hp, 0x03);
            hal_setAudioVideoFlag(hp, avflag & ~AVFLAG_VIDEOLED);
            /* Set port 1, pin 0 to 0 */
            hal_set_pca9555_clk_val_k(hp, 0x03, regval & 0xbf);
        }
    }

#endif
}

void hal_LED_blink(HAL *hp, int lednumb, int blinkrate)
{
#ifdef __XPI__
    /* blink using HW assist in S6 for non RJ45 LEDs only */
    if (BLINKING_LED(lednumb)) 
        hal_or_in_s3fpga_val32_k(hp, S3_LED_BLINK_REG, 1<<lednumb);
#else
    struct led_io ledio;
    int hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == V2DI50_DEVICE) {
        ledio.lednumb = lednumb;
        ledio.leddata = blinkrate;
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LEDSTATE, &ledio);
    }
    else if (hardware_type == PLX_DEVICE) {
        if (lednumb == CONNECTION_LED) {
            hal_LED_off(hp, CONNECTION_LED);
        }
    }
#endif
}


void hal_LED_all_on(HAL * hp)
{
    int hardware_type = hal_get_hardware_type(hp);
#ifdef __XPI__
    if (hardware_type == XPI_DEVICE) {
        unsigned long s3regval = hal_get_s3fpga_val32_k(hp, S3_LED_REG);
        unsigned long s3blinkregval = 
            hal_get_s3fpga_val32_k(hp, S3_LED_BLINK_REG);
        s3regval |= EXPANSION_LED_MASK;
        s3regval &= ~EXPRJ45_LED_MASK;
        hal_set_s3fpga_val32_k(hp, S3_LED_REG, s3regval);
        s3blinkregval |= ~EXPANSION_LED_MASK;
        s3blinkregval &= ~EXPRJ45_LED_MASK;
        hal_set_s3fpga_val32_k(hp, S3_LED_BLINK_REG, s3blinkregval);
        hal_LED_on(hp, MAINRJ45_GREEN);
        hal_LED_on(hp, MAINRJ45_YELLOW);
        hal_LED_on(hp, MAINRJ45_ORANGE);
    }
#else
    if (hardware_type == V2DI50_DEVICE) {
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LED, LED_TYPE_ALLON);
    }
    else if (hardware_type == PLX_DEVICE) {
        hal_LED_on(hp, CONNECTION_LED);
        hal_LED_on(hp, VIDEO_LED);
    }
#endif
}


void hal_LED_all_off(HAL * hp)
{
    int hardware_type = hal_get_hardware_type(hp);
#ifdef __XPI__
    if (hardware_type == XPI_DEVICE) {
        unsigned long s3regval = hal_get_s3fpga_val32_k(hp, S3_LED_REG);
        unsigned long s3blinkregval = 
            hal_get_s3fpga_val32_k(hp, S3_LED_BLINK_REG);
        s3regval &= ~EXPANSION_LED_MASK;
        s3regval |= EXPRJ45_LED_MASK;
        hal_set_s3fpga_val32_k(hp, S3_LED_REG, s3regval);
        s3blinkregval &= ~EXPANSION_LED_MASK;
        s3blinkregval |= EXPRJ45_LED_MASK;
        hal_set_s3fpga_val32_k(hp, S3_LED_BLINK_REG, s3blinkregval);
        hal_LED_off(hp, MAINRJ45_GREEN);
        hal_LED_off(hp, MAINRJ45_YELLOW);
        hal_LED_off(hp, MAINRJ45_ORANGE);
    }
#else
    if (hardware_type == V2DI50_DEVICE)
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LED, LED_TYPE_ALLOFF);
    else if (hardware_type == PLX_DEVICE) {
        hal_LED_off(hp, CONNECTION_LED);
        hal_LED_off(hp, VIDEO_LED);
    }
#endif
}


void hal_LED_all_normal(HAL * hp)
{
#ifndef __XPI__
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE)
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LED, LED_TYPE_NORMAL);
#endif
}

void hal_LED_keepalive(HAL *hp, int lednumb, int sec)
{
#ifndef __XPI__
    struct led_io ledio;
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        ledio.lednumb = lednumb;
        ledio.leddata = sec;
        ioctl(hp->kernel_fd, FPGA_IOC_SET_LEDALIVE, &ledio);
    }
#endif
}

void hal_all_LED_status(HAL *hp, int *led_on, int *led_blink) 
{
#ifdef __XPI__
    int  val;

    *led_on = hal_get_s3fpga_val32_k(hp, S3_LED_REG) & EXPANSION_LED_MASK;

    if (*led_on & (1 << EXPRJ45_GREEN))
        *led_on &= ~(1 << EXPRJ45_GREEN);
    else
        *led_on |= (1 << EXPRJ45_GREEN);
    if (*led_on & (1 << EXPRJ45_ORANGE))
        *led_on &= ~(1 << EXPRJ45_ORANGE);
    else
        *led_on |= (1 << EXPRJ45_ORANGE);
    if (*led_on & (1 << EXPRJ45_YELLOW))
        *led_on &= ~(1 << EXPRJ45_YELLOW);
    else
        *led_on |= (1 << EXPRJ45_YELLOW);

    *led_blink = hal_get_s3fpga_val32_k(hp, S3_LED_BLINK_REG) & 
         EXPANSION_LED_MASK;

    if (xpi_get_gpio_pin_value(MAINRJ45_GREEN, &val) && val == LED_ON)
        *led_on |= MAINRJ45_GREEN_BIT;

    if (xpi_get_gpio_pin_value(MAINRJ45_YELLOW, &val) && val == LED_ON)
        *led_on |= MAINRJ45_YELLOW_BIT;

    if (xpi_get_gpio_pin_value(MAINRJ45_ORANGE, &val) && val == LED_ON)
        *led_on |= MAINRJ45_ORANGE_BIT;

#endif
}

void hal_set_all_LEDs(HAL *hp, int on, int blink)
{
#ifdef __XPI__
    unsigned long s3regval = hal_get_s3fpga_val32_k(hp, S3_LED_REG);
    unsigned long s3blinkregval = hal_get_s3fpga_val32_k(hp, S3_LED_BLINK_REG);
    int mainrj45_green, mainrj45_yellow, mainrj45_orange;

    if (on & (1 << EXPRJ45_GREEN))
        on &= ~(1 << EXPRJ45_GREEN);
    else
        on |= (1 << EXPRJ45_GREEN);
    if (on & (1 << EXPRJ45_ORANGE))
        on &= ~(1 << EXPRJ45_ORANGE);
    else
        on |= (1 << EXPRJ45_ORANGE);
    if (on & (1 << EXPRJ45_YELLOW))
        on &= ~(1 << EXPRJ45_YELLOW);
    else
        on |= (1 << EXPRJ45_YELLOW);

    /* Figure out what the S3 registers should be then set the   */
    /* register to that value the LED flickers if it's set twice */
    s3regval = (s3regval & ~EXPANSION_LED_MASK) | (on & EXPANSION_LED_MASK);
    s3blinkregval = (s3blinkregval & ~BLINKING_LED_MASK) | 
        (blink & BLINKING_LED_MASK);
    hal_set_s3fpga_val32_k(hp, S3_LED_REG, s3regval);
    hal_set_s3fpga_val32_k(hp, S3_LED_BLINK_REG, s3blinkregval);

    mainrj45_green = (on & (1 << MAINRJ45_GREEN_BIT));
    mainrj45_yellow = (on & (1 << MAINRJ45_YELLOW_BIT));
    mainrj45_orange = (on & (1 << MAINRJ45_ORANGE_BIT));
    
    /* Don't turn off MAINRJ45 LEDs if they're already off the   */
    /* LED will flicker if you turn it off when it's off.        */
    if (mainrj45_green != hal_LED_status(hp, MAIN_RJ45_GREEN)) {
        if (mainrj45_green)
            hal_LED_on(hp, MAINRJ45_GREEN);
        else
            hal_LED_off(hp, MAINRJ45_GREEN);
    }

    if (mainrj45_yellow != hal_LED_status(hp, MAIN_RJ45_YELLOW)) {
        if (mainrj45_yellow)
            hal_LED_on(hp, MAINRJ45_YELLOW);
        else
            hal_LED_off(hp, MAINRJ45_YELLOW);
    }

    if (mainrj45_orange != hal_LED_status(hp, MAIN_RJ45_ORANGE)) {
        if (mainrj45_orange)
            hal_LED_on(hp, MAINRJ45_ORANGE);
        else
            hal_LED_off(hp, MAINRJ45_ORANGE);
    }
#endif
}
