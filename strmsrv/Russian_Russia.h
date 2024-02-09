#ifndef RUSSIAN_RUSSIA_LAYOUT_H_
#define RUSSIAN_RUSSIA_LAYOUT_H_

#pragma once 

#include <stdio.h>
#include "ControlKeyCodes.h"

// keep the structure in sorted form for less iterations 
struct _KeySymToUSBStruct_Russian_Russia
{
	struct _KeySymToUSB KSTU;
};

static const struct _KeySymToUSBStruct_Russian_Russia KeySymToUSBStruct_Russian_Russia[] = {
	{{0x06a1,0x0000,0x0000,false}},//XK_Serbian_dje ђ CYRILLIC SMALL LETTER DJE
    {{0x06a2,0x0000,0x0000,false}},//XK_Macedonia_gje ѓ CYRILLIC SMALL LETTER GJE
    {{0x06a3,0x0035,0x0000,false}},//XK_Cyrillic_io ё CYRILLIC SMALL LETTER IO
    {{0x06a4,0x0000,0x0000,false}},//XK_Ukrainian_ie є CYRILLIC SMALL LETTER UKRAINIAN IE
    {{0x06a5,0x0000,0x0000,false}},//XK_Macedonia_dse ѕ CYRILLIC SMALL LETTER DZE
    {{0x06a6,0x0000,0x0000,false}},//XK_Ukrainian_i і CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
    {{0x06a7,0x0000,0x0000,false}},//XK_Ukrainian_yi ї CYRILLIC SMALL LETTER YI
    {{0x06a8,0x0000,0x0000,false}},//XK_Cyrillic_je ј CYRILLIC SMALL LETTER JE
    {{0x06a9,0x0000,0x0000,false}},//XK_Cyrillic_lje љ CYRILLIC SMALL LETTER LJE
    {{0x06aa,0x0000,0x0000,false}},//XK_Cyrillic_nje њ CYRILLIC SMALL LETTER NJE
    {{0x06ab,0x0000,0x0000,false}},//XK_Serbian_tshe ћ CYRILLIC SMALL LETTER TSHE
    {{0x06ac,0x0000,0x0000,false}},//XK_Macedonia_kje ќ CYRILLIC SMALL LETTER KJE
    {{0x06ae,0x0000,0x0000,false}},//XK_Byelorussian_shortu ў CYRILLIC SMALL LETTER SHORT U
    {{0x06af,0x0000,0x0000,false}},//XK_Cyrillic_dzhe џ CYRILLIC SMALL LETTER DZHE
    {{0x06b0,0x0220,0x0000,false}},//XK_numerosign № NUMERO SIGN
    {{0x06b1,0x0000,0x0000,false}},//XK_Serbian_DJE Ђ CYRILLIC CAPITAL LETTER DJE
    {{0x06b2,0x0000,0x0000,false}},//XK_Macedonia_GJE Ѓ CYRILLIC CAPITAL LETTER GJE
    {{0x06b3,0x0235,0x0000,false}},//XK_Cyrillic_IO Ё CYRILLIC CAPITAL LETTER IO
    {{0x06b4,0x0000,0x0000,false}},//XK_Ukrainian_IE Є CYRILLIC CAPITAL LETTER UKRAINIAN IE
    {{0x06b5,0x0000,0x0000,false}},//XK_Macedonia_DSE Ѕ CYRILLIC CAPITAL LETTER DZE
    {{0x06b6,0x0000,0x0000,false}},//XK_Ukrainian_I І CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
    {{0x06b7,0x0000,0x0000,false}},//XK_Ukrainian_YI Ї CYRILLIC CAPITAL LETTER YI
    {{0x06b8,0x0000,0x0000,false}},//XK_Cyrillic_JE Ј CYRILLIC CAPITAL LETTER JE
    {{0x06b9,0x0000,0x0000,false}},//XK_Cyrillic_LJE Љ CYRILLIC CAPITAL LETTER LJE
    {{0x06ba,0x0000,0x0000,false}},//XK_Cyrillic_NJE Њ CYRILLIC CAPITAL LETTER NJE 
    {{0x06bb,0x0000,0x0000,false}},//XK_Serbian_TSHE Ћ CYRILLIC CAPITAL LETTER TSHE 
    {{0x06bc,0x0000,0x0000,false}},//XK_Macedonia_KJE Ќ CYRILLIC CAPITAL LETTER KJE
    {{0x06be,0x0000,0x0000,false}},//XK_Byelorussian_SHORTU Ў CYRILLIC CAPITAL LETTER SHORT U
    {{0x06bf,0x0000,0x0000,false}},//XK_Cyrillic_DZHE Џ CYRILLIC CAPITAL LETTER DZHE
    {{0x06c0,0x0037,0x0000,false}},//XK_Cyrillic_yu ю CYRILLIC SMALL LETTER YU
    {{0x06c1,0x0009,0x0000,false}},//XK_Cyrillic_a а CYRILLIC SMALL LETTER A
    {{0x06c2,0x0036,0x0000,false}},//XK_Cyrillic_be б CYRILLIC SMALL LETTER BE
    {{0x06c3,0x001A,0x0000,false}},//XK_Cyrillic_tse ц CYRILLIC SMALL LETTER TSE
    {{0x06c4,0x000F,0x0000,false}},//XK_Cyrillic_de д CYRILLIC SMALL LETTER DE
    {{0x06c5,0x0017,0x0000,false}},//XK_Cyrillic_ie е CYRILLIC SMALL LETTER IE
    {{0x06c6,0x0004,0x0000,false}},//XK_Cyrillic_ef ф CYRILLIC SMALL LETTER EF
    {{0x06c7,0x0018,0x0000,false}},//XK_Cyrillic_ghe г CYRILLIC SMALL LETTER GHE
    {{0x06c8,0x002F,0x0000,false}},//XK_Cyrillic_ha х CYRILLIC SMALL LETTER HA
    {{0x06c9,0x0005,0x0000,false}},//XK_Cyrillic_i и CYRILLIC SMALL LETTER I
    {{0x06ca,0x0014,0x0000,false}},//XK_Cyrillic_shorti й CYRILLIC SMALL LETTER SHORT I
    {{0x06cb,0x0015,0x0000,false}},//XK_Cyrillic_ka к CYRILLIC SMALL LETTER KA
    {{0x06cc,0x000E,0x0000,false}},//XK_Cyrillic_el л CYRILLIC SMALL LETTER EL
    {{0x06cd,0x0019,0x0000,false}},//XK_Cyrillic_em м CYRILLIC SMALL LETTER EM
    {{0x06ce,0x001C,0x0000,false}},//XK_Cyrillic_en н CYRILLIC SMALL LETTER EN 
    {{0x06cf,0x000D,0x0000,false}},//XK_Cyrillic_o о CYRILLIC SMALL LETTER O
    {{0x06d0,0x000A,0x0000,false}},//XK_Cyrillic_pe п CYRILLIC SMALL LETTER PE
    {{0x06d1,0x001D,0x0000,false}},//XK_Cyrillic_ya я CYRILLIC SMALL LETTER YA
    {{0x06d2,0x000B,0x0000,false}},//XK_Cyrillic_er р CYRILLIC SMALL LETTER ER
    {{0x06d3,0x0006,0x0000,false}},//XK_Cyrillic_es с CYRILLIC SMALL LETTER ES 
    {{0x06d4,0x0011,0x0000,false}},//XK_Cyrillic_te т CYRILLIC SMALL LETTER TE
    {{0x06d5,0x0008,0x0000,false}},//XK_Cyrillic_u у CYRILLIC SMALL LETTER U
    {{0x06d6,0x0033,0x0000,false}},//XK_Cyrillic_zhe ж CYRILLIC SMALL LETTER ZHE
    {{0x06d7,0x0007,0x0000,false}},//XK_Cyrillic_ve в CYRILLIC SMALL LETTER VE
    {{0x06d8,0x0010,0x0000,false}},//XK_Cyrillic_softsign ь CYRILLIC SMALL LETTER SOFT SIGN
    {{0x06d9,0x0016,0x0000,false}},//XK_Cyrillic_yeru ы CYRILLIC SMALL LETTER YERU
    {{0x06da,0x0013,0x0000,false}},//XK_Cyrillic_ze з CYRILLIC SMALL LETTER ZE
    {{0x06db,0x000C,0x0000,false}},//XK_Cyrillic_sha ш CYRILLIC SMALL LETTER SHA
    {{0x06dc,0x0034,0x0000,false}},//XK_Cyrillic_e э CYRILLIC SMALL LETTER E
    {{0x06dd,0x0012,0x0000,false}},//XK_Cyrillic_shcha щ CYRILLIC SMALL LETTER SHCHA
    {{0x06de,0x001B,0x0000,false}},//XK_Cyrillic_che ч CYRILLIC SMALL LETTER CHE
    {{0x06df,0x0030,0x0000,false}},//XK_Cyrillic_hardsign ъ CYRILLIC SMALL LETTER HARD SIGN
    {{0x06e0,0x0237,0x0000,false}},//XK_Cyrillic_YU Ю CYRILLIC CAPITAL LETTER YU
    {{0x06e1,0x0209,0x0000,false}},//XK_Cyrillic_A А CYRILLIC CAPITAL LETTER A
    {{0x06e2,0x0236,0x0000,false}},//XK_Cyrillic_BE Б CYRILLIC CAPITAL LETTER BE
    {{0x06e3,0x021A,0x0000,false}},//XK_Cyrillic_TSE Ц CYRILLIC CAPITAL LETTER TSE
    {{0x06e4,0x020F,0x0000,false}},//XK_Cyrillic_DE Д CYRILLIC CAPITAL LETTER DE
    {{0x06e5,0x0217,0x0000,false}},//XK_Cyrillic_IE Е CYRILLIC CAPITAL LETTER IE
    {{0x06e6,0x0204,0x0000,false}},//XK_Cyrillic_EF Ф CYRILLIC CAPITAL LETTER EF
    {{0x06e7,0x0218,0x0000,false}},//XK_Cyrillic_GHE Г CYRILLIC CAPITAL LETTER GHE
    {{0x06e8,0x022F,0x0000,false}},//XK_Cyrillic_HA Х CYRILLIC CAPITAL LETTER HA
    {{0x06e9,0x0205,0x0000,false}},//XK_Cyrillic_I И CYRILLIC CAPITAL LETTER I
    {{0x06ea,0x0214,0x0000,false}},//XK_Cyrillic_SHORTI Й CYRILLIC CAPITAL LETTER SHORT I
    {{0x06eb,0x0215,0x0000,false}},//XK_Cyrillic_KA К CYRILLIC CAPITAL LETTER KA
    {{0x06ec,0x020E,0x0000,false}},//XK_Cyrillic_EL Л CYRILLIC CAPITAL LETTER EL
    {{0x06ed,0x0219,0x0000,false}},//XK_Cyrillic_EM М CYRILLIC CAPITAL LETTER EM
    {{0x06ee,0x021C,0x0000,false}},//XK_Cyrillic_EN Н CYRILLIC CAPITAL LETTER EN
    {{0x06ef,0x020D,0x0000,false}},//XK_Cyrillic_O О CYRILLIC CAPITAL LETTER O
    {{0x06f0,0x020A,0x0000,false}},//XK_Cyrillic_PE П CYRILLIC CAPITAL LETTER PE
    {{0x06f1,0x021D,0x0000,false}},//XK_Cyrillic_YA Я CYRILLIC CAPITAL LETTER YA
    {{0x06f2,0x020B,0x0000,false}},//XK_Cyrillic_ER Р CYRILLIC CAPITAL LETTER ER 
    {{0x06f3,0x0206,0x0000,false}},//XK_Cyrillic_ES С CYRILLIC CAPITAL LETTER ES
    {{0x06f4,0x0211,0x0000,false}},//XK_Cyrillic_TE Т CYRILLIC CAPITAL LETTER TE 
    {{0x06f5,0x0208,0x0000,false}},//XK_Cyrillic_U У CYRILLIC CAPITAL LETTER U
    {{0x06f6,0x0233,0x0000,false}},//XK_Cyrillic_ZHE Ж CYRILLIC CAPITAL LETTER ZHE
    {{0x06f7,0x0207,0x0000,false}},//XK_Cyrillic_VE В CYRILLIC CAPITAL LETTER VE
    {{0x06f8,0x0210,0x0000,false}},//XK_Cyrillic_SOFTSIGN Ь CYRILLIC CAPITAL LETTER SOFT SIGN
    {{0x06f9,0x0216,0x0000,false}},//XK_Cyrillic_YERU Ы CYRILLIC CAPITAL LETTER YERU 
    {{0x06fa,0x0213,0x0000,false}},//XK_Cyrillic_ZE З CYRILLIC CAPITAL LETTER ZE
    {{0x06fb,0x020C,0x0000,false}},//XK_Cyrillic_SHA Ш CYRILLIC CAPITAL LETTER SHA
    {{0x06fc,0x0234,0x0000,false}},//XK_Cyrillic_E Э CYRILLIC CAPITAL LETTER E
    {{0x06fd,0x0212,0x0000,false}},//XK_Cyrillic_SHCHA Щ CYRILLIC CAPITAL LETTER SHCHA
    {{0x06fe,0x021B,0x0000,false}},//XK_Cyrillic_CHE Ч CYRILLIC CAPITAL LETTER CHE
    {{0x06ff,0x0230,0x0000,false}},//XK_Cyrillic_HARDSIGN Ъ CYRILLIC CAPITAL LETTER HARD SIGN
	{{0x20ac,0x0508,0x0000,false}}//XK_EuroSign €

};

static const unsigned int Encode00xx_Russian_Russia_Layout[ARRAY_SIZE] = {  /* used for convertion ASCII -> USB usage codes*/
/*           0       1       2       3       4       5       6       7        8       9       A       B       C       D       E       F*/
/* 0*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                      */
/* 1*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*      space      !        "       #       $       %       &       '       (        )       *      +       ,       -       .       /   */
/* 2*/  0x8002C, 0x021E, 0x021F, 0x0220, 0x0221, 0x02222, 0x0223, 0x002D,  0x0226, 0x0227, 0x0225, 0x022E, 0x0238, 0x002D, 0x0038, 0x0231,
/*          0       1       2       3       4      5        6       7        8       9       :      ;       <       =       >       ?  */
/* 3*/  0x0027, 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024,  0x0025, 0x0026, 0x0223, 0x0221, 0x0031, 0x002E, 0x0231, 0x0224,
/*         @      s+a,    s+b,    s+c,    s+d,    s+e,    s+f,    s+g,     s+h,    s+i,    s+j,   s+k,    s+l,    s+m,    s+n,    s+o,  */
/* 4*/  0x051F, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020A,  0x020B, 0x020C, 0x020D, 0x020E, 0x020F, 0x0210, 0x0211, 0x0212,
/*        s+p     s+q,    s+r,    s+s,    s+t,    s+u,    s+v,    s+w,     s+x,    s+y,    s+z     [        \       ]       ^       _   */
/* 5*/  0x0213, 0x0214, 0x0215, 0x0216, 0x0217, 0x0218, 0x0219, 0x021A,  0x021B, 0x021C, 0x021D, 0x0525, 0x0031, 0x0526, 0x0000, 0x022D,
/*           `     a       b       c       d       e       f       g        h       i       j       k       l      m       n       o    */
/* 6*/  0x0000, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012,
/*         p       q       r       s       t       u       v       w        x       y      z       {       |       }        ~           */
/* 7*/  0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,  0x001B, 0x001C, 0x001D, 0x0524, 0x0235, 0x0527, 0x0000, 0x0000,
/*                                                                                                                                      */
/* 8*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                      */
/* 9*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*				   ¡			  £		  ¤					      §		    ¨				ª		«	   ¬						    */
/* A*/  0x0000, 0x002E, 0x0506, 0x0520, 0x0530, 0x0000, 0x0000, 0x0521,  0x0000, 0x0000, 0x0234, 0x002E, 0x0535, 0x0000, 0x0000, 0x0000,
/*		   °	   ±	  ²       ³        ´       µ	   ¶	  ·			¸			   º        »		¼		½		¾	    ¿   */
/* B*/  0x002E, 0x0233, 0x051F, 0x0520, 0x0000, 0x0510, 0x0515, 0x0537,  0x0000, 0x051E, 0x0034, 0x022E, 0x0521, 0x0522, 0x0523, 0x022E,
/*			À		Á		Â	  Ã 	   Ä	  				  Ç	       È		É		Ê		Ë		Ì		Í		Î		Ï	*/
/* C*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0233,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*					Ñ		Ò	 	Ó	   Ô	  Õ	      Ö						   Ù		Ú		Û	   Ü		Ý				ß   */
/* D*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0516,
/*			à		á		â	  ã	       ä					  ç		   è		é		ê		ë		ì		í		î		ï	*/
/* E*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0033,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*					ñ		ò		ó		ô	   õ	  Ö							ù		ú		û		ü		ý				ÿ	*/
/* F*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 };


#endif /* RUSSIAN_RUSSIA_LAYOUT_H_ */


