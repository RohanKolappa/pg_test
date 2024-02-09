#ifndef CONTROLKEYCODES_H_
#define CONTROLKEYCODES_H_

#pragma once 

#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


//#define ARRAY_SIZE 256
#define KEYBOARD_LAYOUT_ENGLISH_US 1
#define KEYBOARD_LAYOUT_GERMAN_GERMANY 2
#define KEYBOARD_LAYOUT_FRENCH_BELGIUM 3
#define KEYBOARD_LAYOUT_FRENCH_FRANCE 4
#define KEYBOARD_LAYOUT_SPANISH_SPAIN 5
#define KEYBOARD_LAYOUT_DUTCH_NETHERLANDS 6
#define KEYBOARD_LAYOUT_ITALIAN_ITALY 7
#define KEYBOARD_LAYOUT_PORTUGUESE_PORTUGAL 8
#define KEYBOARD_LAYOUT_RUSSIAN_RUSSIA 9

struct _KeySymToUSB
{
	unsigned int	KeySym;// XK_Key
	unsigned int	Button1;// USB key code for dead key if KeySym is a dead key else USB key code for KeySym if KeySym is not a dead key
	unsigned int	Button2;// USB key code for generating the dead key if KeySym is a dead key
	bool			IsDeadKey;// whether KeySym is a dead key or not

};


struct _KeySymToUSBStruct
{
	struct _KeySymToUSB KSTU;
};


static const unsigned int EncodeFFxx[ARRAY_SIZE] = {  /* used for convertion XK_ ...  -> USB usage codes*/
/*            0           1           2           3              4           5           6           7*/
/*               8           9           A           B              C           D           E           F*/

/*FF00*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/*        _BackSpace      XK_Tab   _Linefeed      _Clear                    _Return                      */
/*FF08*/    0x002A,     0x002B,     0x0000,     0x009C,        0x0000,     0x0028,     0x0000,     0x0000,

/*                                              XK_Pause   _Scroll_Lock    _Sys_Req                      */
/*FF10*/    0x0000,     0x0000,     0x0000,     0x0048,        0x0047,     0x0246,     0x0000,     0x0000,
/*                                                _Escape                                                */
/*FF18*/    0x0000,     0x0000,     0x0000,     0x0029,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/**/
/*FF20*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*FF28*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/**/
/*FF30*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*FF38*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/**/
/*FF40*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*FF48*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*           XK_Home     XK_Left      XK_Up     XK_Right        XK_Down  XK_Page_Up  _Page_Down      XK_End*/
/*FF50*/    0x004A,     0x0050,     0x0052,     0x004F,        0x0051,     0x004B,     0x004E,     0x004D,
/**/
/*FF58*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*           _Select    XK_Print               XK_Insert                                           XK_Menu*/
/*FF60*/    0x0077,     0x0046,     0x0000,     0x0049,        0x0000,     0x0000,     0x0000,     0x0065,
/*                                             XK_Break,Ctrl+Pause!*/
/*FF68*/    0x0000,     0x0000,     0x0000,     0x0048,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/**/
/*FF70*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*FF78*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/**/
/*FF80*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/*                                                                       XK_KP_Enter*/
/*FF88*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0058,     0x0000,     0x0000,
/**/
/*                                                                       XK_KP_Home  XK_KP_Left    XK_KP_Up*/
/*FF90*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,    0x1005F,    0x1005C,    0x10060,
/*       XK_KP_Right  XK_KP_Down _KP_Page_Up _KP_Page_Down    XK_KP_End             XK_KP_Insert XK_KP_Delete*/
/*FF98*/   0x1005E,    0x1005A,    0x10061,    0x1005B,       0x10059,     0x0000,    0x10062,    0x10063,
/**/
/**/
/*FFA0*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/*                               _KP_Multiply  XK_KP_Add  _KP_Separator _KP_Subtract _KP_Decimal XK_KP_Divide*/
/*FFA8*/    0x0000,     0x0000,     0x0055,     0x0057,        0x0085,     0x0056,    0x20063,     0x0054,
/**/
/*          XK_KP_0     XK_KP_1     XK_KP_2     XK_KP_3        XK_KP_4     XK_KP_5     XK_KP_6     XK_KP_7*/
/*FFB0*/   0x20062,    0x20059,    0x2005A,    0x2005B,       0x2005C,    0x2005D,    0x2005E,    0x2005F,
/*          XK_KP_8     XK_KP_9                                          XK_KP_Equal    XK_F1        XK_F2*/
/*FFB8*/   0x20060,    0x20061,     0x0000,     0x0000,        0x0000,     0x0067,     0x003A,     0x003B,
/**/
/*           XK_F3       XK_F4       XK_F5       XK_F6          XK_F7       XK_F8       XK_F9      XK_F10*/
/*FFC0*/    0x003C,     0x003D,     0x003E,     0x003F,        0x0040,     0x0041,     0x0042,     0x0043,
/*          XK_F11      XK_F12      XK_F13      XK_F14         XK_F15      XK_F16      XK_F17      XK_F18*/
/*FFC8*/    0x0044,     0x0045,     0x0068,     0x0069,        0x006A,     0x006B,     0x006C,     0x006D,
/**/
/*          XK_F19      XK_F20      XK_F21      XK_F22         XK_F23      XK_F24*/
/*FFD0*/    0x006E,     0x006F,     0x0070,     0x0071,        0x0072,     0x0073,     0x0000,     0x0000,
/**/
/*FFD8*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/**/
/*                    XK_Shift_L  XK_Shift_R XK_Control_L  XK_Control_R XK_Caps_Lock _Shift_Lock  XK_Meta_L*/
/*FFE0*/    0x0000,     0x0200,     0x2000,     0x0100,        0x1000,     0x0039,     0x0039,     0x0800,
/*         XK_Meta_R    XK_Alt_L    XK_Alt_R   XK_Super_L    XK_Super_R   XK_Hyper_L  XK_Hyper_R*/
/*FFE8*/    0x8000,     0x0400,     0x4000,     0x0800,        0x8000,     0x0800,     0x8000,     0x0000,
/**/
/**/ 
/*FFF0*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x0000,
/*                                                                                                  _Delete*/
/*FFF8*/    0x0000,     0x0000,     0x0000,     0x0000,        0x0000,     0x0000,     0x0000,     0x004C
/**/
 };
/* *INDENT-ON* */

#endif /* CONTROLKEYCODES_H_ */
