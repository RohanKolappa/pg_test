
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/timer.h>  
#include <linux/sched.h>  
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "fpga.h"
#include "fpga_ioctl.h"
#include "lut_table.h"
#include "led_control.h"

#define TIMER_FREQUENCY         (HZ/4)   /* 4 times a second */




/*********************************************
 * Turns On or Off a specific hardware LED.  *
 *********************************************/
static void switchLED(struct v2d_board *board, int lednumb, int direction) {
    unsigned short  ledval;
    volatile unsigned short *datap = (unsigned short *) board->v_ledregister;

    if (direction == TRUE)
        board->v_leds[lednumb].l_state = LED_ON;
    else
        board->v_leds[lednumb].l_state = LED_OFF;

    if (!board->v_hardware_detected)
        return;

    if (board->v_pci_dev_type != V2DI50_DEVICE)
        return;

    if (lednumb >=0 && lednumb < 5) {
        ledval = *datap;
        if (direction == TRUE)
            ledval |= (0x01 << lednumb);
        else
            ledval &= ~(0x01 << lednumb);
        *datap = ledval;
    }
    else if (lednumb >= 5 && lednumb < 8) {
        ledval = *datap;
        if (direction != TRUE)
            ledval |= (0x01 << (lednumb+1));
        else
            ledval &= ~(0x01 << (lednumb+1));
        *datap = ledval;
    }

}

/**************************************************
 * LED task used to control various blink rates.  *
 **************************************************/
static void LEDtask(struct v2d_board *board) {
    int         i;
    int         blinkrate;
   
    board->v_ledcnt++;

    if (board->v_normal_operation == FALSE)
        return;
   
    for (i = 0; i < V2_NUM_LEDS; i++) {

        if (board->v_leds[i].l_keepalive < 0)  
            /* If keepalive timed out turn off led */
            switchLED(board,i, FALSE);

        blinkrate = 0;
        switch (board->v_leds[i].l_setting) {
        case LED_OFF:
            if (board->v_leds[i].l_state == LED_ON) {
                switchLED(board,i, FALSE);
            }
            break;
        case LED_ON:
            if (board->v_leds[i].l_state == LED_OFF && 
                           board->v_leds[i].l_keepalive >= 0) {
                switchLED(board,i, TRUE);
            }
            break;
        case LED_BLINK_NORMAL:  /* 1 sec on, 1 sec off = Blink once in 2 sec */
            blinkrate = 4;
            break;
        case LED_BLINK_SUPERFAST: /* .25 sec on, .25 sec off = Blink once in .5 sec */
            blinkrate = 1;
            break;
        case LED_BLINK_FAST: /* .5 sec on, .5 sec off = Blink once in 1 sec */
            blinkrate = 2;
            break;
        case LED_BLINK_SLOW: /* 2 sec on, 2 sec off = Blink once in 4 sec */
            blinkrate = 8;
            break;
        case LED_BLINK_SUPERSLOW: /* 3 sec on, 3 sec off = Blink once in 6 sec */
            blinkrate = 12;
            break;
        }
       
        if (blinkrate > 0 && board->v_leds[i].l_keepalive >= 0) {
            if ((board->v_ledcnt % blinkrate) == 0) {
                if (board->v_leds[i].l_state == LED_ON)
                    switchLED(board,i, FALSE);
                else 
                    switchLED(board,i, TRUE);
            }
        }

        /************************************************************
         * Decrement keepalive flag and make if -1 if it reaches    *
         * zero. In other words dont timeout keepalive if it is set *
         * to 0. Positive numbers timeout to -1.                    *
         ************************************************************/
        if ((board->v_ledcnt % 4) == 0) {
            if (board->v_leds[i].l_keepalive > 1)
                board->v_leds[i].l_keepalive--;
            else if (board->v_leds[i].l_keepalive == 1)
                board->v_leds[i].l_keepalive = -1;
        }
    }
}

/**************************
 * Turn on a specific LED *
 **************************/
void LEDon (struct v2d_board *board, int lednumb) {
    board->v_leds[lednumb].l_setting = LED_ON;
    if (board->v_normal_operation == TRUE)
        switchLED(board,lednumb, TRUE);
}

/***************************
 * Turn off a specific LED *
 ***************************/
void LEDoff (struct v2d_board *board,int lednumb) {
    board->v_leds[lednumb].l_setting = LED_OFF;
    if (board->v_normal_operation == TRUE)
        switchLED(board,lednumb, FALSE);
}

/**************************************
 * Blink given LED at a specific rate *
 **************************************/
void LEDblink (struct v2d_board *board, int lednumb, int blinkrate) {
    switch (blinkrate) {
    case LED_BLINK_NORMAL:
    case LED_BLINK_SUPERFAST:
    case LED_BLINK_FAST:
    case LED_BLINK_SLOW:
    case LED_BLINK_SUPERSLOW:
        board->v_leds[lednumb].l_setting = blinkrate;
        break;
    default:
        break; /* ignore other rates */
    }
}

/**************************************
 * Set Keep Alive bit for a given led *
 **************************************/
void LEDkeepalive (struct v2d_board *board, int lednumb, int keepalive) {
    board->v_leds[lednumb].l_keepalive= keepalive;
    
}

/*************************************************************
 * Turn Off all LEDs without loosing the current LED setting *
 *************************************************************/
void LEDAllOff(struct v2d_board *board) {
    int i;
    board->v_normal_operation = FALSE;
    for (i = 0; i < V2_NUM_LEDS; i++)
        switchLED(board, i, FALSE);
}

/************************************************************
 * Turn On all LEDs without loosing the current LED setting *
 ************************************************************/
void LEDAllOn(struct v2d_board *board) {
    int i;
    board->v_normal_operation = FALSE;
    for (i = 0; i < V2_NUM_LEDS; i++)
        switchLED(board, i, TRUE);
}

/************************************************************
 * Turn On all LEDs without loosing the current LED setting *
 ************************************************************/
void LEDAllNormal(struct v2d_board *board) {
    board->v_normal_operation = TRUE;
}


/*********************
 * LED timer process *
 *********************/
static void led_process(unsigned long data) {
    struct v2d_board *board = (struct v2d_board *) data;


    LEDtask(board);
        
    LEDkeepalive(board,SYSTEM_LED, 1);
    
    /* Reschedule timer  for next tick */
    mod_timer(&board->v_led_timer, jiffies + TIMER_FREQUENCY);

}

/***************************************
 * LED control initialization function *
 ***************************************/
int led_control_init(struct v2d_board *board) {

    int i;
    if (!board->v_hardware_detected) {
        return 0;
    }
    if (board->v_pci_dev_type != V2DI50_DEVICE)
        return 0;



    board->v_ledregister = board->v_ConfigMemBase+ FPGA_LEDREG;
    board->v_normal_operation=TRUE;

    /******************************************
     * Turn Off Keep Alive flags for all LEDs *
     ******************************************/
    for (i = 0; i < V2_NUM_LEDS; i++)
        LEDkeepalive(board, i, 0);


    /***************************
     * First turn off all LEDs *
     ***************************/
    for (i = 0; i < V2_NUM_LEDS; i++)
        LEDoff(board, i);

    /**************************************
     * Set up first LED to blink steadily *
     **************************************/
    LEDblink(board, TWINKLE_LED, LED_BLINK_FAST);

    LEDkeepalive(board, SYSTEM_LED, 1);
    LEDon(board, SYSTEM_LED);


    /******************************************************
     * Create a timer to periodically call timer function *
     ******************************************************/
    init_timer(&board->v_led_timer);
    board->v_led_timer.function = led_process;
    board->v_led_timer.data = (unsigned long) board;
    board->v_led_timer.expires = jiffies + TIMER_FREQUENCY;
    add_timer(&board->v_led_timer);

    return 0;
}


void led_control_exit(struct v2d_board *board) {
    if (!board->v_hardware_detected) {
        return ;
    }
    if (board->v_pci_dev_type != V2DI50_DEVICE)
        return;


    LEDoff(board,SYSTEM_LED);

    del_timer_sync(&board->v_led_timer);
}

int led_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data) {

    int type;
    struct led_io ledio;;
    if (!board->v_hardware_detected) {
        return 0;
    }
    if (board->v_pci_dev_type != V2DI50_DEVICE)
        return 0;


    switch (cmd) {
    case FPGA_IOC_SET_LED:
        type = data;
        switch (type) {
        case LED_TYPE_ALLON:
            LEDAllOn(board);
            break;
        case LED_TYPE_ALLOFF:
            LEDAllOff(board);
            break;
        case LED_TYPE_NORMAL:
            LEDAllNormal(board);
            break;
        }
        return 0;

    case FPGA_IOC_SET_LEDSTATE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ledio, (unsigned char *)data, sizeof(ledio)) != 0)
            return -EFAULT;

        if (ledio.lednumb >= 0 && ledio.lednumb < V2_NUM_LEDS) {
            switch(ledio.leddata) {
            case LED_ON:
                LEDon(board,ledio.lednumb);
                break;
            case LED_OFF:
                LEDoff(board,ledio.lednumb);
                break;
                
            case LED_BLINK_NORMAL:
            case LED_BLINK_SUPERFAST:
            case LED_BLINK_FAST:
            case LED_BLINK_SLOW:
            case LED_BLINK_SUPERSLOW:
                LEDblink(board,ledio.lednumb, ledio.leddata);
                break;
                
            }
        }
        return 0;

    case FPGA_IOC_SET_LEDALIVE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&ledio, (unsigned char *)data, sizeof(ledio)) != 0)
            return -EFAULT;

        if (ledio.lednumb >= 0 && ledio.lednumb < V2_NUM_LEDS) {
            LEDkeepalive(board, ledio.lednumb, ledio.leddata);
        }
        return 0;


    }
    
    return -EINVAL;
}
