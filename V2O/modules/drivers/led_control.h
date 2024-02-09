#ifndef __LEDCONTROL_H__
#define __LEDCONTROL_H__

//#define NUMB_LEDS               9

#define LED_OFF                 0
#define LED_ON                  1
#define LED_BLINK_SUPERFAST     2  /* Four times a second */
#define LED_BLINK_FAST          3  /* Twice a second */
#define LED_BLINK_NORMAL        4  /* Once a second */
#define LED_BLINK_SLOW          5  /* Once in two seconds */
#define LED_BLINK_SUPERSLOW     6  /* Once in three seconds */

#define TWINKLE_LED             0
#define HVSYNC_LED              1
#define STEREO_LED              2
#define AUDIO_LED               3
#define MOUSE_LED               4
#define ALARM_LED               7
#define LOS_LED                 6
#define FUNC_LED                8

#define CONNECTION_LED          5
#define VIDEO_LED               6
#define SYSTEM_LED              7

#ifdef __KERNEL__
extern void LEDon (struct v2d_board *board, int lednumb);
extern void LEDoff (struct v2d_board *board,int lednumb);
extern void LEDblink (struct v2d_board *board,int lednumb, int blinkrate);
extern void LEDAllOn(struct v2d_board *board);
extern void LEDAllOff(struct v2d_board *board);
extern void LEDAllNormal(struct v2d_board *board);
extern void LEDkeepalive (struct v2d_board *board, int lednumb, int keepalive);

extern int led_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data);
int led_control_init(struct v2d_board *board);
void led_control_exit(struct v2d_board *board);
#endif /* __KERNEL__ */

#endif  /* __LEDCONTROL_H__ */
