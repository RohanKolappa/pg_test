#ifndef RAWTOUSB_H_
#define RAWTOUSB_H_

#pragma once 

#include "command.hh"
#include "strmsrv_compatibility.h"
#include "common.hh"
#include "vlog.h"
#include "v2d.h"
#include "v2o_lib.h"
#include "USBHid.h"
//#include "USB.h"

typedef struct _RawToUsbStruct
{
    unsigned int RawCode;
    unsigned int UsbCode;
    bool IsPrintable;
    const char *Name; //Keycap on standard USB keyboard
}RawToUsbStruct;

static const RawToUsbStruct RawToUsbKeyCodes [] = {
    {0x00,    0x0000,     0,    "None"},        /* 0 */
    {0x76,    0x0029,     0,    "Esc"},         /* 1 */
    {0x16,    0x001E,     1,    "1!"},          /* 2 */ 
    {0x1e,    0x001F,     1,    "2@"},          /* 3 */
    {0x26,    0x0020,     1,    "3#"},          /* 4 */
    {0x25,    0x0021,     1,    "4$"},          /* 5 */
    {0x2e,    0x0022,     1,    "5\"%\""},      /* 6 */
    {0x36,    0x0023,     1,    "6^"},          /* 7 */
    {0x3d,    0x0024,     1,    "7&"},          /* 8 */
    {0x3e,    0x0025,     1,    "8*"},          /* 9 */
    {0x46,    0x0026,     1,    "9("},          /* 10 */
    {0x45,    0x0027,     1,    "0)"},          /* 11 */
    {0x4e,    0x002D,     1,    "-_"},          /* 12 */
    {0x55,    0x002E,     1,    "=+"},          /* 13 */
    {0x66,    0x002A,     0,    "Backspace"},   /* 14 */
    {0x0d,    0x002B,     0,    "Tab"},         /* 15 */
    {0x15,    0x0014,     1,    "Q"},           /* 16 */
    {0x1d,    0x001A,     1,    "W"},           /* 17 */
    {0x24,    0x0008,     1,    "E"},           /* 18 */
    {0x2d,    0x0015,     1,    "R"},           /* 19 */
    {0x2c,    0x0017,     1,    "T"},           /* 20 */
    {0x35,    0x001C,     1,    "Y"},           /* 21 */
    {0x3c,    0x0018,     1,    "U"},           /* 22 */
    {0x43,    0x000C,     1,    "I"},           /* 23 */
    {0x44,    0x0012,     1,    "O"},           /* 24 */
    {0x4d,    0x0013,     1,    "P"},           /* 25 */
    {0x54,    0x002F,     1,    "[{"},          /* 26 */
    {0x5b,    0x0030,     1,    "]}"},          /* 27 */
    {0x5a,    0x0028,     0,    "Enter"},       /* 28 */
    {0x14,    0x0100,     0,    "LCtrl"},       /* 29 */
    {0x1c,    0x0004,     1,    "A"},           /* 30 */
    {0x1b,    0x0016,     1,    "S"},           /* 31 */
    {0x23,    0x0007,     1,    "D"},           /* 32 */
    {0x2b,    0x0009,     1,    "F"},           /* 33 */
    {0x34,    0x000A,     1,    "G"},           /* 34 */
    {0x33,    0x000B,     1,    "H"},           /* 35 */
    {0x3b,    0x000D,     1,    "J"},           /* 36 */
    {0x42,    0x000E,     1,    "K"},           /* 37 */
    {0x4b,    0x000F,     1,    "L"},           /* 38 */
    {0x4c,    0x0033,     1,    ";:"},          /* 39 */
    {0x52,    0x0034,     1,    "'\""},         /* 40 */
    {0x0e,    0x0035,     1,    "`~"},          /* 41 */
    {0x12,    0x0200,     0,    "LShift"},      /* 42 */
    {0x5d,    0x0031,     1,    "\\|"},         /* 43 */
    {0x1a,    0x001D,     1,    "Z"},           /* 44 */
    {0x22,    0x001B,     1,    "X"},           /* 45 */
    {0x21,    0x0006,     1,    "C"},           /* 46 */
    {0x2a,    0x0019,     1,    "V"},           /* 47 */
    {0x32,    0x0005,     1,    "B"},           /* 48 */
    {0x31,    0x0011,     1,    "N"},           /* 49 */
    {0x3a,    0x0010,     1,    "M"},           /* 50 */
    {0x41,    0x0036,     1,    ",<"},          /* 51 */
    {0x49,    0x0037,     1,    ".>"},          /* 52 */
    {0x4a,    0x0038,     1,    "/?"},          /* 53 */
    {0x59,    0x2000,     0,    "RShift"},      /* 54 */
    {0x7c,    0x0055,     1,    "KP-*"},        /* 55 */
    {0x11,    0x0400,     0,    "LAlt"},        /* 56 */
    {0x29,    0x8002C,    1,    "space"},       /* 57 */
    {0x58,    0x0039,     0,    "CapsLock"},    /* 58 */
    {0x05,    0x003A,     0,    "F1"},          /* 59 */
    {0x06,    0x003B,     0,    "F2"},          /* 60 */
    {0x04,    0x003C,     0,    "F3"},          /* 61 */
    {0x0c,    0x003D,     0,    "F4"},          /* 62 */
    {0x03,    0x003E,     0,    "F5"},          /* 63 */
    {0x0b,    0x003F,     0,    "F6"},          /* 64 */
    {0x83,    0x0040,     0,    "F7"},          /* 65 */
    {0x0a,    0x0041,     0,    "F8"},          /* 66 */
    {0x01,    0x0042,     0,    "F9"},          /* 67 */
    {0x09,    0x0043,     0,    "F10"},         /* 68 */
    {0x77,    0x0053,     0,    "NumLock"},     /* 69 */
    {0x7e,    0x0047,     0,    "ScrollLock"},  /* 70 */
    {0x6c,    0x2005F,    1,    "KP-7 / Home"}, /* 71 */
    {0x75,    0x20060,    1,    "KP-8 / Up"},   /* 72 */
    {0x7d,    0x20061,    1,    "KP-9 / PgUp"}, /* 73 */
    {0x7b,    0x0056,     1,    "KP--"},        /* 74 */
    {0x6b,    0x2005C,    1,    "KP-4 / Left"}, /* 75 */
    {0x73,    0x2005D,    1,    "KP-5"},        /* 76 */
    {0x74,    0x2005E,    1,    "KP-6 / Right"},/* 77 */
    {0x79,    0x0057,     1,    "KP-+"},        /* 78 */
    {0x69,    0x20059,    1,    "KP-1 / End"},  /* 79 */
    {0x72,    0x2005A,    1,    "KP-2"},        /* 80 */
    {0x7a,    0x2005B,    1,    "KP-3 / PgDn"}, /* 81 */
    {0x70,    0x20062,    1,    "KP-0 / Ins"},  /* 82 */
    {0x71,    0x20063,    1,    "KP-. / Del"},  /* 83 */
    {0x00,    0x0000,     0,    "103RD"},       /* 84 */
    {0x00,    0x0068,     0,    "F13"},         /* 85 */
    {0x00,    0x0000,     0,    "102ND"},       /* 86 */
    {0x78,    0x0044,     0,    "F11"},         /* 87 */
    {0x07,    0x0045,     0,    "F12"},         /* 88 */
    {0x00,    0x0069,     0,    "F14"},         /* 89 */
    {0x00,    0x006A,     0,    "F15"},         /* 90 */
    {0x00,    0x006B,     0,    "F16"},         /* 91 */
    {0x00,    0x006C,     0,    "F17"},         /* 92 */
    {0x00,    0x006D,     0,    "F18"},         /* 93 */
    {0x00,    0x006E,     0,    "F19"},         /* 94 */
    {0x00,    0x006F,     0,    "F20"},         /* 95 */
    {0xe05a,  0x0058,     0,    "KP-Enter"},    /* 96 */
    {0xe014,  0x1000,     0,    "RCtrl"},       /* 97 */
    {0xe04a,  0x0054,     1,    "KP-/"},        /* 98 */
    {0xff15,  0x0046,     0,    "PrtScr"},      /* 99 */
    {0xe011,  0x4000,     0,    "RAlt"},        /* 100 */
    {0x00,    0x0000,     0,    "None"},        /* 101 */
    {0xe06c,  0x004A,     0,    "Home"},        /* 102 */
    {0xe075,  0x10060,    0,    "Up"},          /* 103 */
    {0xe07d,  0x004B,     0,    "PgUp"},        /* 104 */
    {0xe06b,  0x0050,     0,    "Left"},        /* 105 */
    {0xe074,  0x004F,     0,    "Right"},       /* 106 */
    {0xe069,  0x004D,     0,    "End"},         /* 107 */
    {0xe072,  0x0051,     0,    "Down"},        /* 108 */
    {0xe07a,  0x004E,     0,    "PgDn"},        /* 109 */
    {0xe070,  0x0049,     0,    "Insert"},      /* 110 */
    {0xe071,  0x004C,     0,    "Delete"},      /* 111 */
    {0x00,    0x0000,     0,    "Macro"},       /* 112 */
    {0x00,    0x0000,     0,    "Mute"},        /* 113 */
    {0x00,    0x0000,     0,    "VolumeDown"},  /* 114 */
    {0x00,    0x0000,     0,    "VolumeUp"},    /* 115 */
    {0x00,    0x0000,     0,    "Power"},       /* 116 */
    {0x00,    0x0000,     1,    "KP-="},        /* 117 */
    {0x00,    0x0000,     1,    "KP-+/-"},      /* 118 */
    {0xff13,  0x0048,     0,    "Pause"},       /* 119 */
    {0x00,    0x0000,     0,    "F21"},         /* 120 */
    {0x00,    0x0000,     0,    "F22"},         /* 121 */
    {0x00,    0x0000,     0,    "F23"},         /* 122 */
    {0x00,    0x0000,     0,    "F24"},         /* 123 */
    {0x00,    0x0000,     1,    "KP-,"},        /* 124 */
    {0xe01f,  0x0800,     0,    "LMeta"},       /* 125 */
    {0xe027,  0x8000,     0,    "RMeta"},       /* 126 */
    {0xe02f,  0x0065,     0,    "Compose"}     /* 127 */
};

const int RawToUsbKeyCodeSize = sizeof(RawToUsbKeyCodes) / sizeof(RawToUsbStruct);

class CRawToUsb
{

public:
	CRawToUsb(CUSBHID *usbHid,SRVR *srvr);
	~CRawToUsb();

	int		ProcessRawKeyboardData(unsigned int scancode, unsigned char down);
	
private :
	CUSBHID				*m_pUSBHID;
    SRVR                *m_pSrvr;
	unsigned int		m_nModifKeysLatched;
	unsigned int		repBuf[2];
	unsigned int		m_prevrepBuf[2];
	unsigned int		m_nLeftCtrlHold;
	unsigned int		m_nLeftShftHold;
	unsigned int		m_nLeftAltHold;
	unsigned int		m_nLeftGUIHold;
	unsigned int		m_nRightCtrlHold;
	unsigned int		m_nRightShftHold;
	unsigned int		m_nRightAltHold;
	unsigned int		m_nRightGUIHold;
	unsigned int		m_nPrevModifKeysLatched;
	unsigned char		m_sButton[8];
	unsigned short		m_sRAWpr[16];
	unsigned int*		m_pEncode00xx;
	unsigned int		m_nTempKey[2];
	int					ConvertRawToUsb(unsigned int scancode, unsigned char down);
	int					GetKeyboardLEDStatus();
    int                 GetUsbCode(unsigned int RawCode,unsigned int *UsbCode,unsigned int *IsPrintable);

};
#endif /* RAWTOUSB_H_ */
