/*************************************************************************
 *
 * Unless otherwise specified, Copyright (C) 2004 Barco Control Rooms
 *
 * $Source: /home/services/cvs/firmware/ppc/userspace-utils/ppc2usb.h,v $
 * $Revision: 1.1 $
 * $Author: kboyadzh $
 * $Date: 2005/01/06 15:41:26 $
 *
 * Last ChangeLog Entry:
 * $Log: ppc2usb.h,v $
 * Revision 1.1  2005/01/06 15:41:26  kboyadzh
 * ppc2usb files
 *
 * Revision 1.0  2004/11/17 09:54:42 kboyadzh
 * - file created
 *
 ************************************************************************/
#ifndef _VNC2USB_H_
#define _VNC2USB_H_

#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT2 (1<<2)
#define BIT3 (1<<3)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)
#define BIT8 (1<<8)
#define BIT9 (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)


#define KEYB_NUM_LOCK BIT0
#define KEYB_CAPS_LOCK BIT1
#define KEYB_SCROLL_LOCK BIT2
#define KEYB_COMPOSE BIT3
#define KEYB_KANA BIT4

// some usb constants, byte 0 of
// K (keyboard)
#define KEYB_TOKEN 0x4B

// Max number of key strokes in a sequence to generate a special character from US international keyboard
#define MAX_US_INT_KEYS 8

#define VNC_SERVER_PORT 5900 // should be >= 5900
#endif

