This is a fake VNC server, never sending picture to connected VNC viewer,
so the viewer shows garbage, but the keystrokes/mouse coordinates are
accepted and converted to USB HID reports.

The vnc2usb opens 3 devices
    /dev/hidg0 - keyboard
    /dev/hidg1 - mouse classic (relative)
    /dev/hidg2 - tablet (x and y are absolute in range 0..7FFF)

These nodes are created by HID gadget module so it must be inserted first:

insmod g_hid.ko.

The vnc2usb must be started by root. Stop it with Ctrl+C




Example script to compile

#!/bin/sh

# set -x

export PATH=/home/kboyadzh/obj-buildroot/host/bin:/home/kboyadzh/obj-buildroot/host/usr/bin:/home/kboyadzh/obj-buildroot/host/usr/sbin/:$PATH

cd usb_test_vnc

make -r

# cp to nfs dir
cp vnc2usb /srv/nfs/netviz/usr/bin/



Add stuff for g_hid.ko:

The vnc2usb.c expects a composite HID device with 3 interfaces. Example descriptors, implemeneted in
linux kernel  ... 3.0rc7/arch/arm/mach-mmp/platform-netviz2.c

...
#include <linux/platform_device.h>
#include <linux/usb/g_hid.h>

// ============= /dev/hidg0  keyboard ==================================
static struct hidg_func_descriptor keyb_hid_data = { //length 63
    .subclass       = 0, /* No subclass */
    .protocol       = 1, /* BootProtocol Keyboard */
    .report_length  = 8,
    .report_desc_length = 63,
    .report_desc = {
        0x05, 0x01, /* USAGE_PAGE (Generic Desktop)           */
        0x09, 0x06, /* USAGE (Keyboard)                       */
        0xa1, 0x01, /* COLLECTION (Application)               */
        0x05, 0x07, /*   USAGE_PAGE (Keyboard)                */
        0x19, 0xe0, /*   USAGE_MINIMUM (Keyboard LeftControl) */
        0x29, 0xe7, /*   USAGE_MAXIMUM (Keyboard Right GUI)   */
        0x15, 0x00, /*   LOGICAL_MINIMUM (0)                  */
        0x25, 0x01, /*   LOGICAL_MAXIMUM (1)                  */
        0x75, 0x01, /*   REPORT_SIZE (1)                      */
        0x95, 0x08, /*   REPORT_COUNT (8)                     */
        0x81, 0x02, /*   INPUT (Data,Var,Abs)                 */
        0x95, 0x01, /*   REPORT_COUNT (1)                     */
        0x75, 0x08, /*   REPORT_SIZE (8)                      */
        0x81, 0x03, /*   INPUT (Cnst,Var,Abs)                 */
        0x95, 0x05, /*   REPORT_COUNT (5)                     */
        0x75, 0x01, /*   REPORT_SIZE (1)                      */
        0x05, 0x08, /*   USAGE_PAGE (LEDs)                    */
        0x19, 0x01, /*   USAGE_MINIMUM (Num Lock)             */
        0x29, 0x05, /*   USAGE_MAXIMUM (Kana)                 */
        0x91, 0x02, /*   OUTPUT (Data,Var,Abs)                */
        0x95, 0x01, /*   REPORT_COUNT (1)                     */
        0x75, 0x03, /*   REPORT_SIZE (3)                      */
        0x91, 0x03, /*   OUTPUT (Cnst,Var,Abs)                */
        0x95, 0x06, /*   REPORT_COUNT (6)                     */
        0x75, 0x08, /*   REPORT_SIZE (8)                      */
        0x15, 0x00, /*   LOGICAL_MINIMUM (0)                  */
        0x25, 0x65, /*   LOGICAL_MAXIMUM (101)                */
        0x05, 0x07, /*   USAGE_PAGE (Keyboard)                */
        0x19, 0x00, /*   USAGE_MINIMUM (Reserved)             */
        0x29, 0x65, /*   USAGE_MAXIMUM (Keyboard Application) */
        0x81, 0x00, /*   INPUT (Data,Ary,Abs)                 */
        0xc0        /* END_COLLECTION                         */
    }
};
static struct platform_device keyb_hid = {
    .name       = "hidg",   .id         = 0, // as /dev/hidg0
    .num_resources  = 0,    .resource   = 0,
    .dev.platform_data  = &keyb_hid_data,
};


// ============= /dev/hidg1  mouse ==================================
static struct hidg_func_descriptor mous_hid_data = {
    .subclass       = 0, /* No subclass */
    .protocol       = 2, /* BootProtocol Mouse */
    .report_length  = 4, /* 4 bytes */
    .report_desc_length = 52, //ARRAY_SIZE(report_desc_keyb), // 63,
    .report_desc = {
        0x05, 0x01,             /* Usage Page (Generic Desktop),     */
        0x09, 0x02,             /* Usage (Mouse),                    */
        0xA1, 0x01,             /* Collection (Application),         */
        0x09, 0x01,             /* Usage (Pointer),                  */
        0xA1, 0x00,             /* Collection (Physical),            */

        0x05, 0x09,             /* Usage Page (Buttons),             */     // Byte 0  - bits 0..2 buttons
        0x19, 0x01,             /* Usage Minimum (01),               */
        0x29, 0x03,             /* Usage Maximum (03),               */
        0x15, 0x00,             /* Logical Minimum (0),              */
        0x25, 0x01,             /* Logical Maximum (1),              */
        0x75, 0x01,             /* Report Size (1),                  */
        0x95, 0x03,             /* Report Count (3),                 */
        0x81, 0x02,             /* Input (Data, Variable, Absolute), */
        0x75, 0x05,             /* Report Size (5),                  */
        0x95, 0x01,             /* Report Count (1),                 */
        0x81, 0x01,             /* Input (Constant),                 */

        0x05, 0x01,             /* Usage Page (Generic Desktop),     */
        0x09, 0x30,             /* Usage (X),                        */     // Byte 1 - X relative 0..FF
        0x09, 0x31,             /* Usage (Y),                        */     // Byte 2 - Y relative 0..FF
        0x09, 0x38,             /* Usage (Wheel)                     */     // Byte 3 - ScrollWheel only 0xFE-0x00-0x02
        0x15, 0x81,             /* Logical Minimum (-127),           */
        0x25, 0x7F,             /* Logical Maximum (127),            */
        0x75, 0x08,             /* Report Size (8),                  */
        0x95, 0x03,             /* Report Count (3),                 */
        0x81, 0x06,             /* Input (Data, Variable, Relative), */
        0xC0,                   /* End Collection,                   */
        0xC0                    /* End Collection                    */
    }
};
static struct platform_device mous_hid = {
    .name       = "hidg",   .id         = 1,        // to /dev/hidg1
    .num_resources  = 0,    .resource   = 0,
    .dev.platform_data  = &mous_hid_data,
};


// ============= /dev/hidg2 tablet (absolute mouse) ==================================
static struct hidg_func_descriptor tablet_hid_data = {  // As mouse, but uses ABSOLUTE coordinates
    .subclass       = 0, /* No subclass */
    .protocol       = 0,
    .report_length  = 6,
    .report_desc_length = 58,
    .report_desc = {
        0x05, 0x01,             /* Usage Page (Generic Desktop),     */
        0x09, 0x01,             /* Usage (Pointer),                  */
        0xA1, 0x00,             /* Collection (Physical),            */

        0x05, 0x09,             /* Usage Page (Buttons),             */     // Byte 0  - bits 0..2 buttons
        0x19, 0x01,             /* Usage Minimum (01),               */
        0x29, 0x03,             /* Usage Maximum (03),               */
        0x15, 0x00,             /* Logical Minimum (0),              */
        0x25, 0x01,             /* Logical Maximum (1),              */
        0x75, 0x01,             /* Report Size (1),                  */
        0x95, 0x03,             /* Report Count (3),                 */
        0x81, 0x02,             /* Input (Data, Variable, Absolute), */
        0x75, 0x05,             /* Report Size (5),                  */
        0x95, 0x01,             /* Report Count (1),                 */
        0x81, 0x01,             /* Input (Constant),                 */

        0x05, 0x01,             /* Usage Page (Generic Desktop),     */     // Byte 1 - LSB X absolute
        0x09, 0x30,             /* Usage (X),                        */     // Byte 2 - MSB X absolute, X in range 0000..7FFF
        0x09, 0x31,             /* Usage (Y),                        */     // Byte3,4 - LSB/MSB Y, 0000..7FFF
        0x15, 0x00,             /* Logical Minimum (0),              */
        0x26, 0xFF, 0x7F,       /* Logical Maximum (32767),          */
        0x75, 0x10,             /* Report Size (16),                 */
        0x95, 0x02,             /* Report Count (2),                 */
        0x81, 0x02,             /* Input (Data, Variable, Absolute), */

        0x09, 0x38,             /* Usage (Wheel)                     */     // Byte 5 - ScrollWheel only 0xFE-0x00-0x02
        0x15, 0x81,             /* Logical Minimum (-127),           */
        0x25, 0x7F,             /* Logical Maximum (127),            */
        0x75, 0x08,             /* Report Size (8),                  */
        0x95, 0x01,             /* Report Count (1),                 */
        0x81, 0x06,             /* Input (Data, Variable, Relative), */

        0xC0,                   /* End Collection (Physical),        */
    }
};
static struct platform_device tablet_hid = {
    .name       = "hidg",   .id         = 2,        //thus /dev/hidg2
    .num_resources  = 0,    .resource   = 0,
    .dev.platform_data  = &tablet_hid_data,
};


static void __init common_init(void)
{
...
   #ifdef CONFIG_USB_GADGET_PXA_U2O
    pxa168_add_u2o(&aspenite_u2o_info); <---- add USB device (OTG) controller

     platform_device_register(&keyb_hid);  <---- add descriptors
     platform_device_register(&mous_hid);
     platform_device_register(&tablet_hid);
   #endif
...






http://www.kernel.org/doc/pending/hotplug.txt

replug USB device:   http://www.roman10.net/how-to-reset-usb-device-in-linux/



Hotplug events in kernel:

Invoked program name written in
/proc/sys/kernel/hotplug
for nvz2:
cat hotplug -> /sbin/mdev -> busybox




USB devices are anywhere nowadays, even many embedded devices replace the traditional serial devices with usb devices. However, I experienced that USB devices hang from time to time. In most cases, a manual unplug and replug will solve the issue. Actually, usb reset can simulate the unplug and replug operation.

First, get the device path for your usb device. Enter the command lsusb will give you something similar as below,

Bus 008 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
Bus 007 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
Bus 006 Device 002: ID 04b3:310c IBM Corp. Wheel Mouse
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 004 Device 002: ID 0a5c:2145 Broadcom Corp.
Bus 005 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub

Use the IBM Wheel Mouse as an example, the device node for it is /dev/bus/usb/006/002, where 006 is the bus number, and 002 is the device number.

Second, apply ioctl operation to reset the device. This is done in C code,

#include <stdio.h>

#include <fcntl.h>

#include <errno.h>

#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>

void main(int argc, char **argv)

{

	const char *filename;

	int fd;

	filename = argv[1];

	fd = open(filename, O_WRONLY);

	ioctl(fd, USBDEVFS_RESET, 0);

	close(fd);

	return;

}

Save the code above as reset.c, then compile the code using

gcc -o reset reset.c

This will produce the a binary named reset. Again, using the wheel mouse as an example, execute the following commands,

sudo ./reset /dev/bus/usb/006/002

You can take a look at the message by,

tail -f /var/log/messages

On my Ubuntu desktop, the last line reads,

May 4 16:09:17 roman10 kernel: [ 1663.013118] usb 6-2:
reset low speed USB device using uhci_hcd and address 2

This reset operation is effectively the same as you unplug and replug a usb device.

For another method of reset usb using libusb, please refer here

Reference: http://forums.x-plane.org/index.php?app=downloads&showfile=9485.

