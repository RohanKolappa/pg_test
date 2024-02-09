#ifndef PORTUGUESE_PORTUGAL_LAYOUT_H_
#define PORTUGUESE_PORTUGAL_LAYOUT_H_

#pragma once 

#include <stdio.h>
#include "ControlKeyCodes.h"

// keep the structure in sorted form for less iterations 
struct _KeySymToUSBStruct_Portuguese_Portugal
{
	struct _KeySymToUSB KSTU;
};

static const struct _KeySymToUSBStruct_Portuguese_Portugal KeySymToUSBStruct_Portuguese_Portugal[] = {
	{{0x05e,0x0231,0x8002C,true}},//XK_asciicircum ^ 
	{{0x060,0x0230,0x8002C,true}},//XK_grave ` 
	{{0x07e,0x0031,0x8002C,true}},//XK_asciitilde ~
	{{0x0a8,0x052F,0x8002C,true}},//XK_diaeresis � 
	{{0x0b4,0x0030,0x8002C,true}},//XK_acute � 
    {{0x0b8,0x052E,0x8002C,true}},//XK_cedilla � 
	{{0x0c0,0x0230,0x0204,true}},//XK_Agrave � 
	{{0x0c1,0x0030,0x0204,true}},//XK_Aacute � 
	{{0x0c2,0x0231,0x0204,true}},//XK_Acircumflex � 
	{{0x0c3,0x0031,0x0204,true}},//XK_Atilde �
	{{0x0c4,0x052F,0x0204,true}},//XK_Adiaeresis � 
    {{0x0c7,0x052E,0x0206,true}},//XK_Ccedilla �
	{{0x0c8,0x0230,0x0208,true}},//XK_Egrave � 
	{{0x0c9,0x0030,0x0208,true}},//XK_Eacute � 
	{{0x0ca,0x0231,0x0208,true}},//XK_Ecircumflex � 
	{{0x0cb,0x052F,0x0208,true}},//XK_Ediaeresis � 
	{{0x0cc,0x0230,0x020C,true}},//XK_Igrave � 
	{{0x0cd,0x0030,0x020C,true}},//XK_Iacute � 
	{{0x0ce,0x0231,0x020C,true}},//XK_Icircumflex � 
	{{0x0cf,0x052F,0x020C,true}},//XK_Idiaeresis � 
	{{0x0d1,0x0031,0x0211,true}},//XK_Ntilde �
	{{0x0d2,0x0230,0x0212,true}},//XK_Ograve � 
	{{0x0d3,0x0030,0x0212,true}},//XK_Oacute � 
	{{0x0d4,0x0231,0x0212,true}},//XK_Ocircumflex � 
	{{0x0d5,0x0031,0x0212,true}},//XK_Otilde �
	{{0x0d6,0x052F,0x0212,true}},//XK_Odiaeresis � 
	{{0x0d9,0x0230,0x0218,true}},//XK_Ugrave � 
	{{0x0da,0x0030,0x0218,true}},//XK_Uacute � 
	{{0x0db,0x0231,0x0218,true}},//XK_Ucircumflex � 
	{{0x0dc,0x052F,0x0218,true}},//XK_Udiaeresis � 
	{{0x0dd,0x0030,0x021C,true}},//XK_Yacute � 
	{{0x0e0,0x0230,0x0004,true}},//XK_agrave � 
	{{0x0e1,0x0030,0x0004,true}},//XK_aacute � 
	{{0x0e2,0x0231,0x0004,true}},//XK_acircumflex � 
	{{0x0e3,0x0031,0x0004,true}},//XK_atilde �
	{{0x0e4,0x052F,0x0004,true}},//XK_adiaeresis � 
    {{0x0e7,0x052E,0x0006,true}},//XK_ccedilla � 
	{{0x0e8,0x0230,0x0008,true}},//XK_egrave � 
	{{0x0e9,0x0030,0x0008,true}},//XK_eacute � 
	{{0x0ea,0x0231,0x0008,true}},//XK_ecircumflex � 
	{{0x0eb,0x052F,0x0008,true}},//XK_ediaeresis � 
	{{0x0ec,0x0230,0x000C,true}},//XK_igrave � 
	{{0x0ed,0x0030,0x000C,true}},//XK_iacute � 
	{{0x0ee,0x0231,0x000C,true}},//XK_icircumflex � 
	{{0x0ef,0x052F,0x000C,true}},//XK_idiaeresis � 
	{{0x0f1,0x0031,0x0011,true}},//XK_ntilde �
	{{0x0f2,0x0230,0x0012,true}},//XK_ograve � 
	{{0x0f3,0x0030,0x0012,true}},//XK_oacute � 
	{{0x0f4,0x0231,0x0012,true}},//XK_ocircumflex � 
	{{0x0f5,0x0031,0x0012,true}},//XK_otilde �
	{{0x0f6,0x052F,0x0012,true}},//XK_odiaeresis � 
	{{0x0f9,0x0230,0x0018,true}},//XK_ugrave � 
	{{0x0fa,0x0030,0x0018,true}},//XK_uacute � 
	{{0x0fb,0x0231,0x0018,true}},//XK_ucircumflex � 
	{{0x0fc,0x052F,0x0018,true}},//XK_udiaeresis � 
	{{0x00fd,0x0030,0x001C,true}},//XK_yacute � 
	{{0x00ff,0x052F,0x001C,true}},//XK_ydiaeresis � 
    {{0x07ec,0x0510,0x0000,false}},//XK_Greek_mu �
	{{0x20ac,0x0508,0x0000,false}}//XK_EuroSign �
};

static const unsigned int Encode00xx_Portuguese_Portugal_Layout[ARRAY_SIZE] = {  /* used for convertion ASCII -> USB usage codes*/
/*           0       1       2       3       4       5       6       7        8       9       A       B       C       D       E       F*/
/* 0*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                      */
/* 1*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*      space      !        "       #       $       %       &       '       (        )       *      +       ,       -       .       /   */
/* 2*/  0x8002C, 0x021E, 0x021F, 0x0220, 0x0221, 0x02222, 0x0223, 0x002D,  0x0225, 0x0226, 0x022F, 0x002F, 0x0036, 0x0038, 0x0037, 0x0224,
/*          0       1       2       3       4      5        6       7        8       9       :      ;       <       =       >       ?  */
/* 3*/  0x0027, 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024,  0x0025, 0x0026, 0x0237, 0x0236, 0x0031, 0x0227, 0x0231, 0x022D,
/*         @      s+a,    s+b,    s+c,    s+d,    s+e,    s+f,    s+g,     s+h,    s+i,    s+j,   s+k,    s+l,    s+m,    s+n,    s+o,  */
/* 4*/  0x051F, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020A,  0x020B, 0x020C, 0x020D, 0x020E, 0x020F, 0x0210, 0x0211, 0x0212,
/*        s+p     s+q,    s+r,    s+s,    s+t,    s+u,    s+v,    s+w,     s+x,    s+y,    s+z     [        \       ]       ^       _   */
/* 5*/  0x0213, 0x0214, 0x0215, 0x0216, 0x0217, 0x0218, 0x0219, 0x021A,  0x021B, 0x021C, 0x021D, 0x0525, 0x0035, 0x0526, 0x0000, 0x0238,
/*           `     a       b       c       d       e       f       g        h       i       j       k       l      m       n       o    */
/* 6*/  0x0000, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012,
/*         p       q       r       s       t       u       v       w        x       y      z       {       |       }        ~           */
/* 7*/  0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,  0x001B, 0x001C, 0x001D, 0x0524, 0x0235, 0x0527, 0x0000, 0x0000,
/*                                                                                                                                      */
/* 8*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                      */
/* 9*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*				   �			  �		  �					      �		    �				�		�	   �						    */
/* A*/  0x0000, 0x002E, 0x0506, 0x0520, 0x0530, 0x0000, 0x0000, 0x0521,  0x0000, 0x0000, 0x0234, 0x002E, 0x0535, 0x0000, 0x0000, 0x0000,
/*		   �	   �	  �       �        �       �	   �	  �			�			   �        �		�		�		�	    �   */
/* B*/  0x002E, 0x0233, 0x051F, 0x0520, 0x0000, 0x0510, 0x0515, 0x0537,  0x0000, 0x051E, 0x0034, 0x022E, 0x0521, 0x0522, 0x0523, 0x022E,
/*			�		�		�	  � 	   �	  				  �	       �		�		�		�		�		�		�		�	*/
/* C*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0233,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*					�		�	 	�	   �	  �	      �						   �		�		�	   �		�				�   */
/* D*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0516,
/*			�		�		�	  �	       �					  �		   �		�		�		�		�		�		�		�	*/
/* E*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0033,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*					�		�		�		�	   �	  �							�		�		�		�		�				�	*/
/* F*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 };

#endif /* PORTUGUESE_PORTUGAL_LAYOUT_H_ */
