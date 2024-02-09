#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syslog.h>

#include "strmsrv_compatibility.h"
#include "common.hh"
#include "vlog.h"
#include "hidutils.hh"
#include "v2d.h"

#define MAX_X_IN_USB_REPORT 0x7FFF
#define MAX_Y_IN_USB_REPORT 0x7FFF

#define GERMANY //Comment it out for English

//#if 1 // no more fake Shift or fake Alt. Remote Pointer must send the Shift or Alt codes

// keep the structure in sorted form for less iterations 
struct _KeySymToUSBStruct
{
	struct _KeySymToUSB KSTU;
};
static const struct _KeySymToUSBStruct KeySymToUSBStruct[] = {
	{{0x05e,0x0035,0x8002C,true}},//XK_asciicircum ^
	{{0x060,0x022E,0x8002C,true}},//XK_grave `
	{{0x0b4,0x002E,0x8002C,true}},//XK_acute ´
	{{0x0c0,0x022E,0x0204,true}},//XK_Agrave À
	{{0x0c1,0x002E,0x0204,true}},//XK_Aacute Á
	{{0x0c2,0x0035,0x0204,true}},//XK_Acircumflex Â
	{{0x0c8,0x022E,0x0208,true}},//XK_Egrave È
	{{0x0c9,0x002E,0x0208,true}},//XK_Eacute É
	{{0x0ca,0x0035,0x0208,true}},//XK_Ecircumflex Ê
	{{0x0cc,0x022E,0x020C,true}},//XK_Igrave Ì
	{{0x0cd,0x002E,0x020C,true}},//XK_Iacute Í
	{{0x0ce,0x0035,0x020C,true}},//XK_Icircumflex Î
	{{0x0d2,0x022E,0x0212,true}},//XK_Ograve Ò
	{{0x0d3,0x002E,0x0212,true}},//XK_Oacute Ó
	{{0x0d4,0x0035,0x0212,true}},//XK_Ocircumflex Ô
	{{0x0d9,0x022E,0x0218,true}},//XK_Ugrave Ù
	{{0x0da,0x002E,0x0218,true}},//XK_Uacute Ú
	{{0x0db,0x0035,0x0218,true}},//XK_Ucircumflex Û
	{{0x0e0,0x022E,0x0004,true}},//XK_agrave à
	{{0x0e1,0x002E,0x0004,true}},//XK_aacute á
	{{0x0e2,0x0035,0x0004,true}},//XK_acircumflex â
	{{0x0e8,0x022E,0x0008,true}},//XK_egrave è
	{{0x0e9,0x002E,0x0008,true}},//XK_eacute é
	{{0x0ea,0x0035,0x0008,true}},//XK_ecircumflex ê
	{{0x0ec,0x022E,0x000C,true}},//XK_igrave ì
	{{0x0ed,0x002E,0x000C,true}},//XK_iacute í
	{{0x0ee,0x0035,0x000C,true}},//XK_icircumflex î
	{{0x0f2,0x022E,0x0012,true}},//XK_ograve ò
	{{0x0f3,0x002E,0x0012,true}},//XK_oacute ó
	{{0x0f4,0x0035,0x0012,true}},//XK_ocircumflex ô
	{{0x0f9,0x022E,0x0018,true}},//XK_ugrave ù
	{{0x0fa,0x002E,0x0018,true}},//XK_uacute ú
	{{0x0fb,0x0035,0x0018,true}},//XK_ucircumflex û
	{{0x20ac,0x0508,0x0000,false}}//XK_EuroSign €
};

static const unsigned int Encode00xx_German_Germany_Layout[ARRAY_SIZE] = {  /* used for convertion ASCII -> USB usage codes*/
/*           0       1       2       3       4       5       6       7        8       9       A       B       C       D       E       F*/
/* 0*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                       */
/* 1*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*      space      !        "       #       $       %       &       '       (        )       *      +       ,       -       .       /    */
/* 2*/  0x8002C, 0x021E, 0x021F, 0x0031, 0x0221, 0x0222, 0x0223, 0x0231,  0x0225, 0x0226, 0x0230, 0x0030, 0x0036, 0x0038, 0x40037, 0x0224,
/*           0       1       2       3       4          5        6         7         8         9       :      ;        <       =       >       ?   */
/* 3*/  0x40027, 0x4001E, 0x4001F, 0x40020, 0x40021, 0x40022, 0x40023, 0x40024,  0x40025, 0x40026, 0x0237, 0x0236, 0x0064, 0x0227, 0x0264, 0x022D,
/*         @       s+a,    s+b,    s+c,    s+d,    s+e,    s+f,    s+g,     s+h,    s+i,    s+j,    s+k,    s+l,    s+m,    s+n,    s+o, */
/* 4*/  0x0514, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020A,  0x020B, 0x020C, 0x020D, 0x020E, 0x020F, 0x0210, 0x0211, 0x0212,
/*         s+p     s+q,    s+r,    s+s,    s+t,    s+u,    s+v,    s+w,     s+x,    s+y,    s+z     [        \       ]       ^       _   */
/* 5*/  0x0213, 0x0214, 0x0215, 0x0216, 0x0217, 0x0218, 0x0219, 0x021A,  0x021B, 0x021D, 0x021C, 0x0525, 0x052D, 0x0526, 0x0000, 0x0238,
/*           `       a       b       c       d       e       f       g        h       i       j       k       l      m       n       o  */
/* 6*/  0x0000, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012,
/*           p       q       r       s       t       u       v       w        x       y       z    {       |       }        ~                */
/* 7*/  0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,  0x001B, 0x001D, 0x001C, 0x0524, 0x0564, 0x0527, 0x0530, 0x0000,
/**/
/* 8*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* 9*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*																   §																		*/
/* A*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0220,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*		  ° 			   ²      ³        ´       µ																						 */
/* B*/  0x0235, 0x0000, 0x051F, 0x0520, 0x0000, 0x0510, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*			À		Á		Â			  s+ä								È		É		Ê				Ì		Í		Î			*/
/* C*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0234, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*							Ò	 	Ó		Ô			  s+Ö						Ù		Ú		Û	   s+ü						ß    */
/* D*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0233, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x022F, 0x0000, 0x0000, 0x002D,
/*			à		á		â			   ä								è		é		ê				ì		í		î				*/
/* E*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0034, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*							ò		ó		ô			  Ö							ù		ú		û		ü					*/
/* F*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0033, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x002F, 0x0000, 0x0000, 0x0000
 };

static const unsigned int Encode00xx_EnglishLayout[ARRAY_SIZE] = {  /* used for convertion ASCII -> USB usage codes*/
/*           0       1       2       3       4       5       6       7        8       9       A       B       C       D       E       F*/
/* 0*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                     */
/* 1*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*      space      s+1!   s+'"    s+3#    s+4$    s+5%    s+7&       '     s+9(    s+0)    s+8*    s+=+       ,       -       .       /*/
/* 2*/  0x8002C, 0x021E, 0x0234, 0x80220, 0x0221, 0x0222, 0x0224, 0x0034,  0x0226, 0x0227, 0x0225, 0x022E, 0x0036, 0x002D, 0x40037, 0x40038,
/*           0       1       2       3         4       5        6        7         8        9       s+;:     ;      s+,<     =      s+.>    s+/?*/
/* 3*/  0x40027, 0x4001E, 0x4001F, 0x40020, 0x40021, 0x40022, 0x40023, 0x40024,  0x40025, 0x40026, 0x0233, 0x0033, 0x0236, 0x002E, 0x0237, 0x0238,
/*        s+1@     s+a,    s+b,    s+c,    s+d,    s+e,    s+f,    s+g,     s+h,    s+i,    s+j,    s+k,    s+l,    s+m,    s+n,    s+o,*/
/* 4*/  0x8021F, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020A,  0x020B, 0x020C, 0x020D, 0x020E, 0x020F, 0x0210, 0x0211, 0x0212,
/*         s+p     s+q,    s+r,    s+s,    s+t,    s+u,    s+v,    s+w,     s+x,    s+y,    s+z       [       \       ]      s+6^    s+-_ */
/* 5*/  0x0213, 0x0214, 0x0215, 0x0216, 0x0217, 0x0218, 0x0219, 0x021A,  0x021B, 0x021C, 0x021D, 0x8002F, 0x80031, 0x80030, 0x0223, 0x022D,
/*           `       a       b       c       d       e       f       g        h       i       j       k       l       m       n       o*/
/* 6*/  0x0035, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012,
/*           p       q       r       s       t       u       v       w        x       y       z    s+[{    s+\|    s+]}    s+`~*/
/* 7*/  0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,  0x001B, 0x001C, 0x001D, 0x8022F, 0x0231, 0x80230, 0x80235, 0x0000,
/**/
/* 8*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* 9*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* A*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*            SUBSCRIPT   TWO    THREE                                             ONE   - the keys generated by VK_OEM_102  <\> */
/* B*/  0x0000, 0x0000, 0x0264, 0x4064, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0064, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* C*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* D*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* E*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* F*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 };

//#else
// removed shift + key, now shift is received before the key
static const unsigned int Encode00xx_NotInUse[ARRAY_SIZE] = {  /* used for convertion ASCII -> USB usage codes*/
/*           0       1       2       3       4       5       6       7        8       9       A       B       C       D       E       F*/
/* 0*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                     */
/* 1*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*      space      s+1!   s+'"    s+3#    s+4$    s+5%    s+7&       '     s+9(    s+0)    s+8*    s+=+       ,       -       .       /*/
/* 2*/  0x002C, 0x001E, 0x0034, 0x0020, 0x0021, 0x0022, 0x0024, 0x0034,  0x0026, 0x0027, 0x0025, 0x002E, 0x0036, 0x002D, 0x0037, 0x0038,
/*           0       1       2       3       4       5       6       7        8       9    s+;:       ;    s+,<       =    s+.>    s+/?*/
/* 3*/  0x0027, 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024,  0x0025, 0x0026, 0x0033, 0x0033, 0x0036, 0x002E, 0x0037, 0x0038,
/*        s+1@     s+a,    s+b,    s+c,    s+d,    s+e,    s+f,    s+g,     s+h,    s+i,    s+j,    s+k,    s+l,    s+m,    s+n,    s+o,*/
/* 4*/  0x001F, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012,
/*         s+p     s+q,    s+r,    s+s,    s+t,    s+u,    s+v,    s+w,     s+x,    s+y,    s+z       [       \       ]    s+6^    s+-_*/
/* 5*/  0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,  0x001B, 0x001C, 0x001D, 0x002F, 0x0031, 0x0030, 0x0023, 0x002D,
/*           `       a       b       c       d       e       f       g        h       i       j       k       l       m       n       o*/
/* 6*/  0x0035, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012,
/*           p       q       r       s       t       u       v       w        x       y       z    s+[{    s+\|    s+]}    s+`~*/
/* 7*/  0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,  0x001B, 0x001C, 0x001D, 0x002F, 0x0031, 0x0030, 0x0035, 0x0000,
/**/
/* 8*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* 9*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* A*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*            SUBSCRIPT   TWO    THREE                                             ONE   - the keys generated by VK_OEM_102  <\> */
/* B*/  0x0000, 0x0000, 0x0064, 0x0064, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0064, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* C*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* D*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* E*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/**/
/* F*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 };
//#endif


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



CHIDutils::CHIDutils()
{   
    InitializeVariables();
    OpenGadgets();
    SetKeyboardLayout();
}

CHIDutils::~CHIDutils()
{
    CloseGadgets();

	if(m_pEncode00xx)
	{
		delete [] m_pEncode00xx;
		m_pEncode00xx = NULL;
	}

}

void CHIDutils::InitializeVariables()
{
    m_nModifKeysLatched = 0;
    m_nLeftCtrlHold = 0;
    m_nLeftShftHold = 0;
    m_nLeftAltHold = 0;
    m_nLeftGUIHold = 0;
    m_nRightCtrlHold = 0;
    m_nRightShftHold = 0;
    m_nRightAltHold = 0;
    m_nRightGUIHold = 0;
    m_nPrevModifKeysLatched = 0;
    m_nKeyboardfd = -1;
    m_nMouseRelfd = -1;
    m_nMouseAbsfd = -1;
    m_nKeyboardLEDs = 0;
    m_nWidth = 0;
    m_nHeight = 0;
	m_prevrepBuf[0] = m_prevrepBuf[1] = 0;
	m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;
	m_pEncode00xx = new unsigned int[ARRAY_SIZE];
	memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
    m_sButton[0] = m_sButton[1] = m_sButton[2] = m_sButton[3] = 0;
    m_sButton[4] = m_sButton[5] = m_sButton[6] = m_sButton[7] = 0;

}

void CHIDutils::OpenGadgets()
{
    int nCnt;

    for (nCnt = 0; nCnt < NUMBEROFGADGETS; nCnt++) {
        switch (nCnt) {
          case 0:
               m_nKeyboardfd = open(HIDKEYBOARDPATH, O_RDWR);
               if (m_nKeyboardfd <= 0)
                  SYSLOG_DEBUG("HIDUTILS: Could not open Keyboard gadget device path %s:",
                  HIDKEYBOARDPATH);
               break;

          case 1:
              m_nMouseRelfd = open(HIDRELMOUSEPATH, O_RDWR);
              if (m_nMouseRelfd <= 0)
                  SYSLOG_DEBUG("HIDUTILS: Could not open Relative Mouse Gadget device path %s:",
                        HIDRELMOUSEPATH);
              break;
          case 2:
              m_nMouseAbsfd = open(HIDABSMOUSEPATH, O_RDWR);
              if (m_nMouseAbsfd <= 0)
                  SYSLOG_DEBUG("HIDUTILS: Could not open Absolute Mouse Gaget device path %s:",
                          HIDABSMOUSEPATH);
              break;
          default:
              SYSLOG_DEBUG("HIDUTILS:Unknown Gadget device open request\n");
              break;

        }
    }

    return;
}

void CHIDutils::CloseGadgets()
{

    if (m_nKeyboardfd)
       close(m_nKeyboardfd);

    if (m_nMouseRelfd)
       close(m_nMouseRelfd);

    if (m_nMouseAbsfd)
       close(m_nMouseAbsfd);

       m_nKeyboardfd = m_nMouseRelfd = m_nMouseAbsfd = -1;

   return;
}

int CHIDutils::SetKeyboardLayout()
{
    FILE *fp;
    char buf[64];
    char *cp;
    char fpath[64] =  "/etc/config/txkblayout_0.txt";
     
     fp = fopen(fpath,"r");
     if (fp == NULL)
	 {
		 m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;
		 return 0;    /* return default English layout */
	 }

     fgets(buf, 64, fp);
     if ((cp = strchr(buf, '\n')) != 0)
         *cp = '\0';
     fclose(fp);


    if (!strcmp(buf, "US"))
	{
		strcpy(m_sKbLayout,"US");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;

		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_EnglishLayout,ARRAY_SIZE*sizeof(unsigned int));
	}
	else if (!strcmp(buf, "Belgian French"))
	{
		
	}
	else if (!strcmp(buf, "French"))
	{

	}
	else if (!strcmp(buf, "Spanish"))
	{

	}
	else if (!strcmp(buf, "German"))
	{
		strcpy(m_sKbLayout,"German");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_GERMAN_GERMANY;

		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_German_Germany_Layout,ARRAY_SIZE*sizeof(unsigned int));
	}
	else
	{
		strcpy(m_sKbLayout,"US");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;

		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_EnglishLayout,ARRAY_SIZE*sizeof(unsigned int));
	}

    return 1;

}

int CHIDutils::Update(CCommand *a_pCmd)
//int CHIDutils::Update()
{
   int nLayout;
   char kblayout[60];

   if (a_pCmd->GetIntVar("kblayout", &nLayout) != -1) {
       if (a_pCmd->GetVar("kblanguage", kblayout, 60) != -1) {
           if (strcmp(m_sKbLayout, kblayout) != 0)
               SetKeyboardLayout();
       }
   }

   return 1;

}


int CHIDutils::WriteAbsoluteMouse(char *pInbuf, int buflen)
{
    int nBytes;
    char *buf = (char *) pInbuf;

    if (buflen == 0 || pInbuf == NULL || m_nMouseAbsfd <= 0)
        return 0;

    nBytes = write(m_nMouseAbsfd, buf, buflen);

    if (nBytes <= 0) {
       return 0;
    }

    return nBytes;

}

int CHIDutils::WriteRelativeMouse(char *pInBuf, int buflen)
{
    int nBytes;

    if (pInBuf == NULL || buflen <= 0 || m_nMouseRelfd <= 0)
       return 0;

    nBytes = write(m_nMouseRelfd, pInBuf, buflen);

    if (nBytes <= 0)
        return 0;

    return nBytes;

}

int CHIDutils::WriteKeyboard(char *pInBuf, int buflen)
{
    int nBytes;

    if (pInBuf == NULL || buflen <= 0 || m_nKeyboardfd <= 0)
        return 0;
	
    nBytes =  write(m_nKeyboardfd, pInBuf, buflen);

    if (nBytes <= 0) {
        return 0;
    }

    return nBytes;
}


int CHIDutils::SendMouseEvents(char buttonMask, unsigned short XvncAbsol, unsigned short YvncAbsol )
{
    unsigned int  repBuf[2];
    unsigned int buttonbyte, x, y;
    char  wheel;


    //Comparison is always false due to limited range of data type
    //if ((XvncAbsol < 0) || ((XvncAbsol > 0) && (XvncAbsol >= (m_nWidth)))){
    //     return 0;
    //}

    //Comparison is always false due to limited range of data type
    //if ((YvncAbsol < 0) || ((YvncAbsol > 0) && (YvncAbsol >= (m_nHeight)))){
    //     return 0;
    //}

    buttonbyte = buttonMask & BIT0;
    if (buttonMask & BIT1)
        buttonbyte |= BIT2;

    if (buttonMask & BIT2)
        buttonbyte |= BIT1;
     
    wheel = 0;
    if (buttonMask & BIT4)
        wheel = -1;
    if (buttonMask & BIT3)
        wheel = 1;


    x = (XvncAbsol * (MAX_X_IN_USB_REPORT + 1)) / (m_nWidth );
    y = (YvncAbsol * (MAX_Y_IN_USB_REPORT + 1)) / (m_nHeight);


    repBuf[0] = (buttonbyte & 0xFF) | // Buttons to bits 7..0,  byte 1 HID report
               ((x & 0xFF) << 8)   | // X LSB to bits 15..0,   byte 2
               ((x & 0xFF00) << 8) | // X MSB to bits 23..16,  byte 3
               ((y & 0xFF) << 24)  ; // Y LSB to bits 31..24,  byte 4
    

     repBuf[1] = ((y >> 8) & 0xFF)   |   // Y MSB to bits 7..0,  byte 5
                    ((unsigned int)wheel << 8); // Wheel to bits 15..8  byte 6

     char *buf = (char*)repBuf;
     int nlen=  WriteAbsoluteMouse(buf, MSABSREPORTLEN);

     return nlen;

}


int CHIDutils::SendRelMouseEvents( char *pInBuf)
{
    int nLen = 0;
    
    if (pInBuf == NULL)
        return 0;

    nLen = WriteRelativeMouse(pInBuf, MSRELREPORTLEN);

    return nLen;
}

int CHIDutils::ProcessRemoteKeyboard(char *pPayload)
{
    unsigned int key = 0;
    unsigned char down = 0;
    unsigned int sessionId = 0;

    if (pPayload == NULL)
        return -1;
     
    int rc = v2dLibParseAbsoluteKeyboard((uint8_t *)pPayload, &key,
                                          &down, &sessionId);
    // TO DO Need to check SessionId here
    if (rc > 0) {
       if (!(SendKeyboardEvents(key, down)))
           SYSLOG_ERROR("HIDUTILS: Could not send keyboard events\n");
    }

    return 1;

}

int CHIDutils::ProcessRemoteMouse(char *pPayload)
{
    uint16_t x = 0, y = 0;
    uint8_t buttonmask = 0;
    uint32_t sessionId = 0;

    if (pPayload == NULL)
        return -1;

    int rc = v2dLibParseAbsoluteMouse((uint8_t *)pPayload, &x, &y, &buttonmask,&sessionId);

   // TO DO Need to check SessionId here 
    if (rc > 0 ) {
       if (!( SendMouseEvents(buttonmask, x, y)))
           SYSLOG_ERROR("HIDUTILS:could not send Abs mouse Events\n");

    }

    return 1;
}
int CHIDutils:: SendKeyboardEvents(unsigned int key, unsigned char down)
{
    
    //SYSLOG_INFO("-%u- %s(down=%u key=%04X)\n", __LINE__, __FUNCTION__, down, key);
    
#if 0
    char nm[64];

    switch(key) {
       case 0x0020: sprintf(nm, "Space");   break;   case 0xFF08: sprintf(nm, "Backspace"); break;   case 0xFF09: sprintf(nm, "Tab");      break;
       case 0xFF0D: sprintf(nm, "Enter");   break;   case 0xFF13: sprintf(nm, "Pause");     break;   case 0xFF14: sprintf(nm, "ScrollLck");break;
       case 0xFF1B: sprintf(nm, "Esc");     break;   case 0xFF50: sprintf(nm, "Home");      break;   case 0xFF51: sprintf(nm, "LeftArrow");break;
       case 0xFF52: sprintf(nm, "UpArrow"); break;   case 0xFF53: sprintf(nm, "RightArrow");break;   case 0xFF54: sprintf(nm, "DownArrow");break;
       case 0xFF55: sprintf(nm, "PgUp");    break;   case 0xFF56: sprintf(nm, "PgDown");    break;   case 0xFF57: sprintf(nm, "End");      break;
       case 0xFF61: sprintf(nm, "PrintScr");break;   case 0xFF63: sprintf(nm, "Insert");    break;   case 0xFF67: sprintf(nm, "R_Menu");   break;
       case 0xFFBE: sprintf(nm, "F1");      break;   case 0xFFBF: sprintf(nm, "F2");        break;   case 0xFFC0: sprintf(nm, "F3");       break;
       case 0xFFC1: sprintf(nm, "F4");      break;   case 0xFFC2: sprintf(nm, "F5");        break;   case 0xFFC3: sprintf(nm, "F6");       break;
       case 0xFFC4: sprintf(nm, "F7");      break;   case 0xFFC5: sprintf(nm, "F8");        break;   case 0xFFC6: sprintf(nm, "F9");       break;
       case 0xFFC7: sprintf(nm, "F10");     break;   case 0xFFC8: sprintf(nm, "F11");       break;   case 0xFFC9: sprintf(nm, "f12");      break;
       case 0xFFE1: sprintf(nm, "L_Shift"); break;   case 0xFFE2: sprintf(nm, "R_Shift");   break;   case 0xFFE3: sprintf(nm, "L_Ctrl");   break;
       case 0xFFE4: sprintf(nm, "R_Ctrl");  break;   case 0xFFE5: sprintf(nm, "CapsLock");  break;   case 0xFFE6: sprintf(nm, "ShiftLock");break;
       case 0xFFE9: sprintf(nm, "L_Alt");   break;   case 0xFFEA: sprintf(nm, "R_Alt");     break;   case 0xFFEB: sprintf(nm, "L_Winkey"); break;
       case 0xFFEC: sprintf(nm, "R_Winkey");break;   case 0xFFFF: sprintf(nm, "Delete");    break;   default: sprintf(nm, "%c", (char)key);break;
     }
	/*
      if(down)
	  {
          //SYSLOG_INFO("%s (0x%04X)", nm, key);
		  SYSLOG_INFO("-%u- %s(%s (0x%04X))\n", __LINE__, __FUNCTION__, nm, key);
	  }
      else
          SYSLOG_INFO("."); //printf("       --up-- %s (0x%04X)\n",  nm, key);
	  */
		
	SYSLOG_INFO("-%u- %s( %s (0x%04X), down=%u )\n", __LINE__, __FUNCTION__, nm, key,((down)? 1:0));
    
#endif

    if (key == 0xFFE5) 
          return 8;

    ConvertVNCKeysToUSBKeys(key, down);
	return 8;

}

int CHIDutils::ConvertVNCKeysToUSBKeys(unsigned int VNCkey, unsigned char down)
{
    unsigned int  ModifKeys = 0, Found, Value, i;
    unsigned char newbut;
    unsigned char noshift = 0;
    unsigned char noalt = 0;
    unsigned int EmulateModifPress = 0;
	_KeySymToUSB kstu = {0,0,0,0};

    VNCkey &= 0x0000FFFF;
    
	//For XPI virtual keyboard will follow convetion to always keep CAPS-LOCK On 

	//Send CAPS-LOCK USB Code
    if (down && (GetKeyboardLEDStatus()  & KEYB_CAPS_LOCK)) {
        repBuf[0] = (KEYB_TOKEN | (USB_CAPSLOCK_CODE << 16) | m_nModifKeysLatched);
        repBuf[1] = 0;
        repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
        if (!WriteKeyboard((char *)repBuf, 8))
            return 1;
    }

    Found = 0;
    Value = 0;
	switch (VNCkey & 0xFF00) {
	case (0x0000):  /* The state of NumLock is not relevant here */
		{
			//This is a printable character key.
			//Find the USB code for the char from table.
			Value = m_pEncode00xx[VNCkey];

			if(!Value)// not found in the Encode00xx , now look into KeySymToUSBStruct , may be a dead key
			{

				if(XKKeySymToUSB(VNCkey,kstu))
				{
					Value = kstu.Button2;
					Found = 1;
				}
			}
			else
			{
				Found = 1;
				// check if modifier keys already pressed
				// next, check whether R_Shift is not already pressed, and in this case ignore the L_Shift from Encode00xx[] array
				if ((m_nModifKeysLatched) && (Value & 0xFF00))
				{
					// Set/Unset right modifier bits to simulate char
					//TO DO : Analyze. Why to set/unset if already set either left or right?

					// an Alt or Shift key must be pressed by SCN
					if (m_nLeftCtrlHold && (Value & USB_RIGHT_CTRL))
						Value &= ~USB_RIGHT_CTRL;
					else if (m_nRightCtrlHold && (Value & USB_LEFT_CTRL))
						Value &= ~USB_LEFT_CTRL;
					if (m_nLeftShftHold && (Value & USB_RIGHT_SHIFT))
						Value &= ~USB_RIGHT_SHIFT;
					else if (m_nRightShftHold && (Value & USB_LEFT_SHIFT))
						Value &= ~USB_LEFT_SHIFT;
					if (m_nLeftAltHold && (Value & USB_RIGHT_ALT))
						Value &= ~USB_RIGHT_ALT;
					else if (m_nRightAltHold && (Value & USB_LEFT_ALT))
						Value &= ~USB_LEFT_ALT;
					if (m_nLeftGUIHold && (Value & USB_RIGHT_GUI))
						Value &= ~USB_RIGHT_GUI;
					else if (m_nRightGUIHold && (Value & USB_LEFT_GUI))
						Value &= ~USB_LEFT_GUI;
				}
				// AZERTY shift/alt + (number) key
				if (Value & NO_SHIFT_REQ) //NO_ALT_REQ
				{
					noshift =1;
				}
				if (Value & NO_ALT_REQ)
				{
					noalt =1;
				}
			}

			break;
		}
	case (0xFF00):
		{
			//This is a non-printable control key e.g. CTRL, ALT, F1, ENTER etc.
			ModifKeys = EncodeFFxx[VNCkey & 0x00FF] & ~0xFF;    /* only bits 31..8 are interesting */
			Value = EncodeFFxx[VNCkey & 0x00FF];    /* all bits */
			Found = 1;
			if (ModifKeys && !(Value & 0x000F00FF)) {   /* this case is to handle specificly Ctrl, Alt, Shift, GUI keys pressed alone. *//* These keys are implemented as Sticky keys, since VNC protocol does not use separated codes */
				// static unsigned int m_nPrevModifKeysLatched;   /* for Alt+x, Ctrl+s etc. bijv. VNC client sends first CTRL code, next 's' code to server */

				switch (ModifKeys) {    /* The state of NumLock is not relevant here */
				case (USB_LEFT_CTRL):   /* XK_Control_L */
					m_nLeftCtrlHold = (down) ? USB_LEFT_CTRL : 0;
					break;
				case (USB_LEFT_SHIFT):  /* XK_Shift_L arrived from client */
					m_nLeftShftHold = (down) ? USB_LEFT_SHIFT : 0;
					break;
				case (USB_LEFT_ALT):    /* XK_Alt_L */
					m_nLeftAltHold = (down) ? USB_LEFT_ALT : 0;
					break;
				case (USB_LEFT_GUI):    /* XK_Hyper_L or XK_Super_L Windows key */
					m_nLeftGUIHold = (down) ? USB_LEFT_GUI : 0;
					break;
				case (USB_RIGHT_CTRL):  /* XK_Control_R */
					m_nRightCtrlHold = (down) ? USB_RIGHT_CTRL : 0;
					break;
				case (USB_RIGHT_SHIFT): /* XK_Shift_R */
					m_nRightShftHold = (down) ? USB_RIGHT_SHIFT : 0;
					break;
				case (USB_RIGHT_ALT):   /* XK_Alt_R */
					m_nRightAltHold = (down) ? USB_RIGHT_ALT : 0;
					break;
				case (USB_RIGHT_GUI):   /* XK_Hyper_R or XK_Super_R Windows key */
					m_nRightGUIHold = (down) ? USB_RIGHT_GUI : 0;
					break;
				default:
					Found = 0;
					break;
				}
				m_nModifKeysLatched = (m_nLeftCtrlHold | m_nLeftShftHold | m_nLeftAltHold | m_nLeftGUIHold | m_nRightCtrlHold | m_nRightShftHold | m_nRightAltHold | m_nRightGUIHold);
				/* do not repeat pressed Shift/Ctrl/Alt/GUI key, otherwise Slave is going nuts */
				if (m_nModifKeysLatched == m_nPrevModifKeysLatched)
					Found = 0;
				m_nPrevModifKeysLatched = m_nModifKeysLatched;
			} else {    /* handle here only the cases where NumLock is required/banned */
				if ((ModifKeys & 0xF0000) && (Value & 0xFF) && down) {  /* All this fun below was added because NumLock does not exist for VNC protocol. */

					switch (ModifKeys) {
					case (NO_NUMLOCK_REQ):  /* NumLock LED must be off the VNC key to be interpretted correctly by Slave */
						if (GetKeyboardLEDStatus() & KEYB_NUM_LOCK) {  /* try to turn off NumLock at Slave */
							m_nTempKey[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16); /* send NumLock keystroke to USB itf */
							m_nTempKey[1] = 0;
							m_nTempKey[0] = (m_nTempKey[0] & ~0xFFFF) | ((m_nTempKey[0] >> 8) & 0xFF);
							if (!WriteKeyboard(( char *)m_nTempKey, 8)) {
								return 1;
							}
							usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
							/*rfbLog(" NO_NUM_REQ "); */
						}/*else the NUMLOCK is OFF already */
						break;
					case (NUMLOCK_REQ): /* NumLock LED must be on */
						if (!(GetKeyboardLEDStatus()  & KEYB_NUM_LOCK)) {
							m_nTempKey[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16);
							m_nTempKey[1] = 0;
							m_nTempKey[0] = (m_nTempKey[0] & ~0xFFFF) | ((m_nTempKey[0] >> 8) & 0xFF);
							if (!WriteKeyboard(( char *)m_nTempKey, 8)) {
								return 1;
							}
							usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
						}/*else the NUMLOCK is ON already */
						break;
						/*add here next special cases */
					default:
						Found = 0;
						break;
					}   
				}   
			}
			/* the Keycode is still Value = EncodeFFxx[VNCkey & 0x00FF] and Found = 1; */
			break;
		}
	default:
		{
			if(XKKeySymToUSB(VNCkey,kstu))
			{
				if(!kstu.IsDeadKey)
				{
					Value = kstu.Button1;
					Found = 1;
					if ((m_nModifKeysLatched) && (Value & 0xFF00))
					{
						// Set/Unset right modifier bits to simulate char
						//TO DO : Analyze. Why to set/unset if already set either left or right?

						// an Alt or Shift key must be pressed by SCN
						if (m_nLeftCtrlHold && (Value & USB_RIGHT_CTRL))
							Value &= ~USB_RIGHT_CTRL;
						else if (m_nRightCtrlHold && (Value & USB_LEFT_CTRL))
							Value &= ~USB_LEFT_CTRL;
						if (m_nLeftShftHold && (Value & USB_RIGHT_SHIFT))
							Value &= ~USB_RIGHT_SHIFT;
						else if (m_nRightShftHold && (Value & USB_LEFT_SHIFT))
							Value &= ~USB_LEFT_SHIFT;
						if (m_nLeftAltHold && (Value & USB_RIGHT_ALT))
							Value &= ~USB_RIGHT_ALT;
						else if (m_nRightAltHold && (Value & USB_LEFT_ALT))
							Value &= ~USB_LEFT_ALT;
						if (m_nLeftGUIHold && (Value & USB_RIGHT_GUI))
							Value &= ~USB_RIGHT_GUI;
						else if (m_nRightGUIHold && (Value & USB_LEFT_GUI))
							Value &= ~USB_LEFT_GUI;
					}
				}

				noshift = 0;
				noalt = 0;
			}
			else
			{
				Found = 0;
				Value = 0;
			}
			break;
		}
	}// endof Switch

    if (!Value)
	{
        SYSLOG_INFO("\r\n--Undefined VNCkey=%04X--\r\n", VNCkey);
	}

    if (!Found)
	{
        return 0;   /* sorry, still not supported code, or repeated Special Key (Shift, Ctrl...) */
	}

    newbut = Value & 0xFF;


    for (i = 2; i < 8; i++) {
        if ((m_sButton[i] == 0) || (m_sButton[i] == newbut)) {
            if (down) {
                m_sButton[i] = newbut;    /* the place is free, or contains the same butt value, write the new button */
                break;
            } else {    /* a button was released, scan m_sButton array and clean it     */
                while (i < 7){ /* move all records left since a button was released */
                    m_sButton[i] = m_sButton[i + 1];
                    i++;
                }
                m_sButton[i] = 0;
                break;
    
            }   
        }   
    }

    // this is to handle shift or AltGr states. 'A' can come as 'SHIFT down', 'A down' (Windows VNC client) or 'A' only (JAVA client)

    /* add alt,shift, ctrl... to ordinary keys, bits 15..8 of values from table Encode00xx */
	
	EmulateModifPress = Value & 0xFF00;
	if( ((VNCkey & 0xFF00) == 0) && (EmulateModifPress)) {
        if (down == false)
            EmulateModifPress = 0;
	}
	
	repBuf[0] = (((unsigned int) m_sButton[3] << 24) | ((unsigned int) m_sButton[2] << 16) | ( m_nModifKeysLatched | EmulateModifPress) | KEYB_TOKEN);
    repBuf[1] = (((unsigned int) m_sButton[7] << 24) | ((unsigned int) m_sButton[6] << 16) | ((unsigned int) m_sButton[5] << 8) | ((unsigned int) m_sButton[4] << 0));
	

    // a check for spurious release code from RemotePointer applet. When a release code arrives, m_sButton press code
    // was not issued before, send 'release all' command to USB keyb module.
    // more than one press code is acceptable.
    Found = 0;
    for ( i = 0; i < 15; i++)   // keep the last 14 pressed m_sButton not released 14 codes . code 15 is always 0
        if (down){
            if ((m_sVNCpr[i] == 0) || (m_sVNCpr[i] == VNCkey)) { // ok, new pressed VNC key or repeated press key
                m_sVNCpr[i] = VNCkey;
                Found = 1;
                break;
            }
        } else {    // a release code arrived, check for available press code
            if (m_sVNCpr[i] == VNCkey) {
                uint16_t j;
                m_sVNCpr[15] = 0;
                for (j = i; j < 15; j++) // free the position, move all records left
                    m_sVNCpr[j] = m_sVNCpr[j + 1];
                    Found = 1;
                break;
            }   
        }

    if (!Found) {
        for ( i = 0; i < 15; i++)
            m_sVNCpr[i] = 0;
            SYSLOG_INFO("HIDUTILC::Spurious release code %04X, releasing pressed keys.\n", VNCkey);
    }

    if (!Found) { // the client has just disappeared, m_sButton the clients have a bad habit to leave keys or mouse buttons pressed ....
        repBuf[0] = KEYB_TOKEN;
        repBuf[1] = 0;
        m_nModifKeysLatched = 0;
        m_nLeftCtrlHold = m_nLeftShftHold = m_nLeftAltHold = m_nLeftGUIHold = 0;
        m_nRightCtrlHold = m_nRightShftHold = m_nRightAltHold = m_nRightGUIHold = 0;
        for (i = 0; i < 8; m_sButton[i++] = 0);
    }

    //  move bits 15..8 to 7..0, and 15..8 are 00 now
    unsigned int cc = repBuf[0] >> 8  & 0xFF;
    repBuf[0] &= ~0xFFFF;
    repBuf[0] |= cc;

    // Remove shift bits from modifiers report byte --- numbers on AZERTY...
    if(noshift)
    {
        repBuf[0] &= ~0x22;
    }
    if(noalt)
    {
        repBuf[0] &= ~0x44;
    }

	if(kstu.KeySym && kstu.IsDeadKey)
	{
		
		// store the previous buffer state first 
		m_prevrepBuf[0] = repBuf[0];
		m_prevrepBuf[1] = repBuf[1];
	
		//simulate the dead key first and then the remaining ones
		EmulateModifPress = kstu.Button1 & 0xFF00;

		unsigned char deadKey = kstu.Button1 & 0xFF;
		repBuf[0] = (((unsigned int) 0 << 24) | ((unsigned int) deadKey << 16) | EmulateModifPress | KEYB_TOKEN);
		repBuf[1] = (((unsigned int) 0 << 24) | ((unsigned int) 0 << 16) | ((unsigned int) 0 << 8) | ((unsigned int) 0 << 0));

		//  move bits 15..8 to 7..0, and 15..8 are 00 now
		unsigned int cc = repBuf[0] >> 8  & 0xFF;
		repBuf[0] &= ~0xFFFF;
		repBuf[0] |= cc;

		if(down)
		{
			if (!WriteKeyboard((char *)repBuf, 8)) {
				return 1;
			}
		}

		if (!WriteKeyboard((char *)m_prevrepBuf, 8)) {
			return 1;
		}


	}
	else
	{
		if (!WriteKeyboard((char *)repBuf, 8)) {
			return 1;
		}
	}
       return 8;
}

int CHIDutils::SetKeyboardLEDsStatus()
{
    int nBytes;
    char  CapslockStatus[2];

    if (m_nKeyboardfd < 0)
        return 0;

    nBytes =  read(m_nKeyboardfd, CapslockStatus, 2);

     if (nBytes < 0)
       return 0;

    m_nKeyboardLEDs = CapslockStatus[0];

    return m_nKeyboardLEDs;
}

/*
void CHIDutils::SendCapsEvent(char *key , int down)
{
    int pklen = 8;
    if (key == NULL || m_nKeyboardfd < 0)
        return;
  
    WriteKeyboard(key, pklen);

}
*/

void CHIDutils::SetScreenWidth(int Width)
{

    m_nWidth = Width;
}

void CHIDutils::SetScreenHeight(int Height)
{
    m_nHeight = Height;

}

bool CHIDutils::XKKeySymToUSB(unsigned int keysym,_KeySymToUSB &kstu)
{
	int min = 0;
	int max = sizeof(KeySymToUSBStruct) / sizeof(_KeySymToUSB) - 1;
	int mid;
	bool found = false;

	//binary search in table 
	  while (max >= min) {
		mid = (min + max) / 2;
		if (KeySymToUSBStruct[mid].KSTU.KeySym < keysym)
		  min = mid + 1;
		else if (KeySymToUSBStruct[mid].KSTU.KeySym > keysym)
		  max = mid - 1;
		else {
		  //found it 
		kstu = KeySymToUSBStruct[mid].KSTU;
		 found = true;
		 break;
		}
	  }
	  return found;
}

int CHIDutils::GetKeyboardFD()
{
	return m_nKeyboardfd;
}

int CHIDutils::GetAbsoluteMouseFD()
{
	return m_nMouseAbsfd;
}

int CHIDutils::GetRelativeMouseFD()
{
	return m_nMouseRelfd;
}

int CHIDutils::GetKeyboardLEDStatus()
{
	return m_nKeyboardLEDs;
}
