#ifndef __USBUTILS_H_INCLUDED__
#define __USBUTILS_H_INCLUDED__

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#define USB_UPDATE_FLAG_ADD       1
#define USB_UPDATE_FLAG_REMOVE    2

int usb_initialize_event_devices(HAL *hp);
void usb_release_event_devices(HAL *hp);
int usb_update_event_device(HAL *hp, char *device, int flag);

int usb_find_tty_device_name_for_board(int bno, 
                                       char name[], 
                                       int len,
                                       int builtin);
int usb_get_tty_device_board_number(const char *name);

#endif /* __LOADFPGA_H_INCLUDED__ */
