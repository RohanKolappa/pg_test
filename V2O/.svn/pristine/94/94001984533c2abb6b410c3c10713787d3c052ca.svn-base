#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "v2d.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "vutils.h"
#include "vlog.h"
#include "kbm_control.h"
#include "kbms_utils.h"
#include "commonutils.h"
#include "version.h"

/*********************************************
 * Untranslated keyboard scancodes - See     *
 * kernel include file include/linux/input.h *
 *********************************************/
KEYCODE keycodes[] = {
    {0,       0,       "None"},        /* 0 */
    {0x76,    0x08,    "Esc"},         /* 1 */
    {0x16,    0x16,    "1!"},          /* 2 */ 
    {0x1e,    0x1e,    "2@"},          /* 3 */
    {0x26,    0x26,    "3#"},          /* 4 */
    {0x25,    0x25,    "4$"},          /* 5 */
    {0x2e,    0x2e,    "5%"},          /* 6 */
    {0x36,    0x36,    "6^"},          /* 7 */
    {0x3d,    0x3d,    "7&"},          /* 8 */
    {0x3e,    0x3e,    "8*"},          /* 9 */
    {0x46,    0x46,    "9("},          /* 10 */
    {0x45,    0x45,    "0)"},          /* 11 */
    {0x4e,    0x4e,    "-_"},          /* 12 */
    {0x55,    0x55,    "=+"},          /* 13 */
    {0x66,    0x66,    "Backspace"},   /* 14 */
    {0x0d,    0x0d,    "Tab"},         /* 15 */
    {0x15,    0x15,    "Q"},           /* 16 */
    {0x1d,    0x1d,    "W"},           /* 17 */
    {0x24,    0x24,    "E"},           /* 18 */
    {0x2d,    0x2d,    "R"},           /* 19 */
    {0x2c,    0x2c,    "T"},           /* 20 */
    {0x35,    0x35,    "Y"},           /* 21 */
    {0x3c,    0x3c,    "U"},           /* 22 */
    {0x43,    0x43,    "I"},           /* 23 */
    {0x44,    0x44,    "O"},           /* 24 */
    {0x4d,    0x4d,    "P"},           /* 25 */
    {0x54,    0x54,    "[{"},          /* 26 */
    {0x5b,    0x5b,    "]}"},          /* 27 */
    {0x5a,    0x5a,    "Enter"},       /* 28 */
    {0x14,    0x11,    "LCtrl"},       /* 29 */
    {0x1c,    0x1c,    "A"},           /* 30 */
    {0x1b,    0x1b,    "S"},           /* 31 */
    {0x23,    0x23,    "D"},           /* 32 */
    {0x2b,    0x2b,    "F"},           /* 33 */
    {0x34,    0x34,    "G"},           /* 34 */
    {0x33,    0x33,    "H"},           /* 35 */
    {0x3b,    0x3b,    "J"},           /* 36 */
    {0x42,    0x42,    "K"},           /* 37 */
    {0x4b,    0x4b,    "L"},           /* 38 */
    {0x4c,    0x4c,    ";:"},          /* 39 */
    {0x52,    0x52,    "'\""},         /* 40 */
    {0x0e,    0x0e,    "`~"},          /* 41 */
    {0x12,    0x12,    "LShift"},      /* 42 */
    {0x5d,    0x5c,    "\\|"},         /* 43 */
    {0x1a,    0x1a,    "Z"},           /* 44 */
    {0x22,    0x22,    "X"},           /* 45 */
    {0x21,    0x21,    "C"},           /* 46 */
    {0x2a,    0x2a,    "V"},           /* 47 */
    {0x32,    0x32,    "B"},           /* 48 */
    {0x31,    0x31,    "N"},           /* 49 */
    {0x3a,    0x3a,    "M"},           /* 50 */
    {0x41,    0x41,    ",<"},          /* 51 */
    {0x49,    0x49,    ".>"},          /* 52 */
    {0x4a,    0x4a,    "/?"},          /* 53 */
    {0x59,    0x59,    "RShift"},      /* 54 */
    {0x7c,    0x7e,    "KP-*"},        /* 55 */
    {0x11,    0x19,    "LAlt"},        /* 56 */
    {0x29,    0x29,    "space"},       /* 57 */
    {0x58,    0x14,    "CapsLock"},    /* 58 */
    {0x05,    0x07,    "F1"},          /* 59 */
    {0x06,    0x0f,    "F2"},          /* 60 */
    {0x04,    0x17,    "F3"},          /* 61 */
    {0x0c,    0x1f,    "F4"},          /* 62 */
    {0x03,    0x27,    "F5"},          /* 63 */
    {0x0b,    0x2f,    "F6"},          /* 64 */
    {0x83,    0x37,    "F7"},          /* 65 */
    {0x0a,    0x3f,    "F8"},          /* 66 */
    {0x01,    0x47,    "F9"},          /* 67 */
    {0x09,    0x4f,    "F10"},         /* 68 */
    {0x77,    0x76,    "NumLock"},     /* 69 */
    {0x7e,    0x5f,    "ScrollLock"},  /* 70 */
    {0x6c,    0x6c,    "KP-7 / Home"}, /* 71 */
    {0x75,    0x75,    "KP-8 / Up"},   /* 72 */
    {0x7d,    0x7d,    "KP-9 / PgUp"}, /* 73 */
    {0x7b,    0x84,    "KP--"},        /* 74 */
    {0x6b,    0x6b,    "KP-4 / Left"}, /* 75 */
    {0x73,    0x73,    "KP-5"},        /* 76 */
    {0x74,    0x74,    "KP-6 / Right"},/* 77 */
    {0x79,    0x7c,    "KP-+"},        /* 78 */
    {0x69,    0x69,    "KP-1 / End"},  /* 79 */
    {0x72,    0x72,    "KP-2"},        /* 80 */
    {0x7a,    0x7a,    "KP-3 / PgDn"}, /* 81 */
    {0x70,    0x70,    "KP-0 / Ins"},  /* 82 */
    {0x71,    0x71,    "KP-. / Del"},  /* 83 */
    {0,       0,       "103RD"},       /* 84 */
    {0,       0,       "F13"},         /* 85 */
    {0,       0,       "102ND"},       /* 86 */
    {0x78,    0x56,    "F11"},         /* 87 */
    {0x07,    0x5e,    "F12"},         /* 88 */
    {0,       0,       "F14"},         /* 89 */
    {0,       0,       "F15"},         /* 90 */
    {0,       0,       "F16"},         /* 91 */
    {0,       0,       "F17"},         /* 92 */
    {0,       0,       "F18"},         /* 93 */
    {0,       0,       "F19"},         /* 94 */
    {0,       0,       "F20"},         /* 95 */
    {0xe05a,  0x79,    "KP-Enter"},    /* 96 */
    {0xe014,  0x58,    "RCtrl"},       /* 97 */
    {0xe04a,  0x77,    "KP-/"},        /* 98 */
    {0xff,    0x57,    "PrtScr"},      /* 99 */
    {0xe011,  0x39,    "RAlt"},        /* 100 */
    {0,       0,       "None"},        /* 101 */
    {0xe06c,  0x6e,    "Home"},        /* 102 */
    {0xe075,  0x63,    "Up"},          /* 103 */
    {0xe07d,  0x6f,    "PgUp"},        /* 104 */
    {0xe06b,  0x61,    "Left"},        /* 105 */
    {0xe074,  0x6a,    "Right"},       /* 106 */
    {0xe069,  0x65,    "End"},         /* 107 */
    {0xe072,  0x60,    "Down"},        /* 108 */
    {0xe07a,  0x6d,    "PgDn"},        /* 109 */
    {0xe070,  0x67,    "Insert"},      /* 110 */
    {0xe071,  0x64,    "Delete"},      /* 111 */
    {0,       0,       "Macro"},       /* 112 */
    {0,       0,       "Mute"},        /* 113 */
    {0,       0,       "VolumeDown"},  /* 114 */
    {0,       0,       "VolumeUp"},    /* 115 */
    {0,       0,       "Power"},       /* 116 */
    {0,       0,       "KP-="},        /* 117 */
    {0,       0,       "KP-+/-"},      /* 118 */
    {0xff,    0x62,    "Pause"},       /* 119 */
    {0,       0,       "F21"},         /* 120 */
    {0,       0,       "F22"},         /* 121 */
    {0,       0,       "F23"},         /* 122 */
    {0,       0,       "F24"},         /* 123 */
    {0,       0,       "KP-,"},        /* 124 */
    {0xe01f,  0x8b,    "LMeta"},       /* 125 */
    {0xe027,  0x8c,    "RMeta"},       /* 126 */
    {0xe02f,  0x8d,    "Compose"},     /* 127 */
};
int keycodesize = sizeof(keycodes) / sizeof(KEYCODE);

/*********************************************
 * Mapping from keyboard scancode to RFB key *
 * See /usr/include/X11/keysymdef.h          *
 *********************************************/
#define XK_Sys_Req      0xff15
#define XK_Pause        0xff13      

static RFBKEY set2_rfbkey[] = {
    {0x0000, 0x0000},         /* 0x00 */    
    {0xffc6, 0xffc6},         /* 0x01 "F9" */            
    {0x0000, 0x0000},         /* 0x02 */           
    {0xffc2, 0xffc2},         /* 0x03 "F5" */    
    {0xffc0, 0xffc0},         /* 0x04 "F3" */    
    {0xffbe, 0xffbe},         /* 0x05 "F1" */    
    {0xffbf, 0xffbf},         /* 0x06 "F2" */    
    {0xffc9, 0xffc9},         /* 0x07 "F12" */    
    {0x0000, 0x0000},         /* 0x08 */    
    {0xffc7, 0xffc7},         /* 0x09 "F10" */    
    {0xffc5, 0xffc5},         /* 0x0a "F8" */    
    {0xffc3, 0xffc3},         /* 0x0b "F6" */    
    {0xffc1, 0xffc1},         /* 0x0c "F4" */    
    {0xff09, 0xff09},         /* 0x0d "Tab" */    
    {0x0060, 0x007e},         /* 0x0e "`~" */    
    {0x0000, 0x0000},         /* 0x0f */    
    {0x0000, 0x0000},         /* 0x10 */    
    {0xffe9, 0xffe9},         /* 0x11 "LAlt" */    
    {0xffe1, 0xffe1},         /* 0x12 "LShift" */    
    {0x0000, 0x0000},         /* 0x13 */    
    {0xffe3, 0xffe3},         /* 0x14 "LCtrl" */    
    {0x0071, 0x0051},         /* 0x15 "Q" */    
    {0x0031, 0x0021},         /* 0x16 "1!" */    
    {0x0000, 0x0000},         /* 0x17 */    
    {0x0000, 0x0000},         /* 0x18 */    
    {0x0000, 0x0000},         /* 0x19 */    
    {0x007a, 0x005a},         /* 0x1a "Z" */    
    {0x0073, 0x0053},         /* 0x1b "S" */    
    {0x0061, 0x0041},         /* 0x1c "A" */    
    {0x0077, 0x0057},         /* 0x1d "W" */    
    {0x0032, 0x0040},         /* 0x1e "2@" */    
    {0x0000, 0x0000},         /* 0x1f */    
    {0x0000, 0x0000},         /* 0x20 */    
    {0x0063, 0x0043},         /* 0x21 "C" */    
    {0x0078, 0x0058},         /* 0x22 "X" */    
    {0x0064, 0x0044},         /* 0x23 "D" */    
    {0x0065, 0x0045},         /* 0x24 "E" */    
    {0x0034, 0x0024},         /* 0x25 "4$" */    
    {0x0033, 0x0023},         /* 0x26 "3#" */    
    {0x0000, 0x0000},         /* 0x27 */    
    {0x0000, 0x0000},         /* 0x28 */    
    {0x0020, 0x0020},         /* 0x29 "space" */    
    {0x0076, 0x0056},         /* 0x2a "V" */    
    {0x0066, 0x0046},         /* 0x2b "F" */    
    {0x0074, 0x0054},         /* 0x2c "T" */    
    {0x0072, 0x0052},         /* 0x2d "R" */    
    {0x0035, 0x0025},         /* 0x2e "5%" */    
    {0x0000, 0x0000},         /* 0x2f */    
    {0x0000, 0x0000},         /* 0x30 */    
    {0x006e, 0x004e},         /* 0x31 "N" */    
    {0x0062, 0x0042},         /* 0x32 "B" */    
    {0x0068, 0x0048},         /* 0x33 "H" */    
    {0x0067, 0x0047},         /* 0x34 "G" */    
    {0x0079, 0x0059},         /* 0x35 "Y" */    
    {0x0036, 0x005e},         /* 0x36 "6^" */    
    {0x0000, 0x0000},         /* 0x37 */    
    {0x0000, 0x0000},         /* 0x38 */    
    {0x0000, 0x0000},         /* 0x39 */    
    {0x006d, 0x004d},         /* 0x3a "M" */    
    {0x006a, 0x004a},         /* 0x3b "J" */    
    {0x0075, 0x0055},         /* 0x3c "U" */    
    {0x0037, 0x0026},         /* 0x3d "7&" */    
    {0x0038, 0x002a},         /* 0x3e "8*" */    
    {0x0000, 0x0000},         /* 0x3f */    
    {0x0000, 0x0000},         /* 0x40 */    
    {0x002c, 0x003c},         /* 0x41 ",<" */    
    {0x006b, 0x004b},         /* 0x42 "K" */    
    {0x0069, 0x0049},         /* 0x43 "I" */    
    {0x006f, 0x004f},         /* 0x44 "O" */    
    {0x0030, 0x0029},         /* 0x45 "0)" */    
    {0x0039, 0x0028},         /* 0x46 "9(" */    
    {0x0000, 0x0000},         /* 0x47 */    
    {0x0000, 0x0000},         /* 0x48 */    
    {0x002e, 0x003e},         /* 0x49 ".>" */    
    {0x002f, 0x003f},         /* 0x4a "/?" */    
    {0x006c, 0x004c},         /* 0x4b "L" */    
    {0x003b, 0x003a},         /* 0x4c ";:" */    
    {0x0070, 0x0050},         /* 0x4d "P" */    
    {0x002d, 0x005f},         /* 0x4e "-_" */    
    {0x0000, 0x0000},         /* 0x4f */    
    {0x0000, 0x0000},         /* 0x50 */    
    {0x0000, 0x0000},         /* 0x51 */    
    {0x0027, 0x0022},         /* 0x52 "'\""*/    
    {0x0000, 0x0000},         /* 0x53 */    
    {0x005b, 0x007b},         /* 0x54 "[{" */    
    {0x003d, 0x002b},         /* 0x55 "=+" */    
    {0x0000, 0x0000},         /* 0x56 */    
    {0x0000, 0x0000},         /* 0x57 */    
    {0x0000, 0x0000},         /* 0x58 "CapsLock" */    
    {0xffe2, 0xffe2},         /* 0x59 "RShift" */    
    {0xff0d, 0xff0d},         /* 0x5a "Enter" */    
    {0x005d, 0x007d},         /* 0x5b "]}" */    
    {0x0000, 0x0000},         /* 0x5c */    
    {0x005c, 0x007c},         /* 0x5d "\\|" */    
    {0x0000, 0x0000},         /* 0x5e */    
    {0x0000, 0x0000},         /* 0x5f */    
    {0x0000, 0x0000},         /* 0x60 */    
    {0x0000, 0x0000},         /* 0x61 */    
    {0x0000, 0x0000},         /* 0x62 */    
    {0x0000, 0x0000},         /* 0x63 */    
    {0x0000, 0x0000},         /* 0x64 */    
    {0x0000, 0x0000},         /* 0x65 */    
    {0xff08, 0xff08},         /* 0x66 "Backspace" */    
    {0x0000, 0x0000},         /* 0x67 */    
    {0x0000, 0x0000},         /* 0x68 */    
    {0xff9c, 0xffb1},         /* 0x69 "KP-1/End" */    
    {0x0000, 0x0000},         /* 0x6a */    
    {0xff96, 0xffb4},         /* 0x6b "KP-4/Left" */    
    {0xff95, 0xffb7},         /* 0x6c "KP-7/Home" */    
    {0x0000, 0x0000},         /* 0x6d */    
    {0x0000, 0x0000},         /* 0x6e */    
    {0x0000, 0x0000},         /* 0x6f */    
    {0xff9e, 0xffb0},         /* 0x70 "KP-0/Ins" */    
    {0xff9f, 0xffae},         /* 0x71 "KP-./Del" */    
    {0xff99, 0xffb2},         /* 0x72 "KP-2/Down" */    
    {0x0000, 0xffb5},         /* 0x73 "KP-5" */    
    {0xff98, 0xffb6},         /* 0x74 "KP-6/Right" */    
    {0xff97, 0xffb8},         /* 0x75 "KP-8/Up" */    
    {0xff1b, 0xff1b},         /* 0x76 "Esc" */    
    {0xff7f, 0xff7f},         /* 0x77 "NumLock" */    
    {0xffc8, 0xffc8},         /* 0x78 "F11" */    
    {0xffab, 0xffab},         /* 0x79 "KP-+" */    
    {0xff9b, 0xffb3},         /* 0x7a "KP-3/PgDn" */    
    {0xffad, 0x002d},         /* 0x7b "KP--" */    
    {0xffaa, 0xffaa},         /* 0x7c "KP-*" */    
    {0xff9a, 0xffb9},         /* 0x7d "KP-9/PgUp" */    
    {0xff14, 0xff14},         /* 0x7e "ScrollLock" */    
    {0x0000, 0x0000},         /* 0x7f */    
    {0x0000, 0x0000},         /* 0x80 */    
    {0x0000, 0x0000},         /* 0x81 */    
    {0x0000, 0x0000},         /* 0x82 */    
    {0xffc4, 0xffc4},         /* 0x83 "F7" */    
};

static E0RFBKEY set2_e0_rfbkey[] = {
    {0x5a, 0xff8d},           /* XP_KP_Enter */
    {0x14, 0xffe4},           /* XK_Control_R */
    {0x4a, 0xffaf},           /* XK_KP_Divide */
    {0x11, 0xffea},           /* XK_Alt_R */
    {0x6c, 0xff50},           /* XK_Home */
    {0x75, 0xff52},           /* XK_Up */
    {0x7d, 0xff55},           /* XK_Page_Up */
    {0x6b, 0xff51},           /* XK_Left */
    {0x74, 0xff53},           /* XK_Right */
    {0x69, 0xff57},           /* XK_End */
    {0x72, 0xff54},           /* XK_Down */
    {0x7a, 0xff56},           /* XK_Page_Down */
    {0x70, 0xff63},           /* XK_Insert */
    {0x71, 0xffff},           /* XK_Delete */
    {0x1f, 0xffeb},           /* XK_Super_L, LMeta */
    {0x27, 0xffec},           /* XK_Super_R, RMeta */
    {0x2f, 0xff67},           /* XK_Menu, Compose */
};
static int set2_e0_rfbkeysize = sizeof(set2_e0_rfbkey) / sizeof(E0RFBKEY);

static RFBKEY set3_rfbkey[] = {
    {0x0000, 0x0000},         /* 0x00 */    
    {0x0000, 0x0000},         /* 0x01 */    
    {0x0000, 0x0000},         /* 0x02 */    
    {0x0000, 0x0000},         /* 0x03 */    
    {0x0000, 0x0000},         /* 0x04 */    
    {0x0000, 0x0000},         /* 0x05 */    
    {0x0000, 0x0000},         /* 0x06 */    
    {0xffbe, 0xffbe},         /* 0x07 "F1" */    
    {0xff1b, 0xff1b},         /* 0x08 "Esc" */    
    {0x0000, 0x0000},         /* 0x09 */    
    {0x0000, 0x0000},         /* 0x0a */    
    {0x0000, 0x0000},         /* 0x0b */    
    {0x0000, 0x0000},         /* 0x0c */    
    {0xff09, 0xff09},         /* 0x0d "Tab" */    
    {0x0060, 0x007e},         /* 0x0e "`~" */    
    {0xffbf, 0xffbf},         /* 0x0f "F2" */    
    {0x0000, 0x0000},         /* 0x10 */    
    {0xffe3, 0xffe3},         /* 0x11 "LCtrl" */    
    {0xffe1, 0xffe1},         /* 0x12 "LShift" */    
    {0x0000, 0x0000},         /* 0x13 */    
    {0x0000, 0x0000},         /* 0x14 "CapsLock" */    
    {0x0071, 0x0051},         /* 0x15 "Q" */    
    {0x0031, 0x0021},         /* 0x16 "1!" */    
    {0xffc0, 0xffc0},         /* 0x17 "F3" */    
    {0x0000, 0x0000},         /* 0x18 */    
    {0xffe9, 0xffe9},         /* 0x19 "LAlt" */    
    {0x007a, 0x005a},         /* 0x1a "Z" */    
    {0x0073, 0x0053},         /* 0x1b "S" */    
    {0x0061, 0x0041},         /* 0x1c "A" */    
    {0x0077, 0x0057},         /* 0x1d "W" */    
    {0x0032, 0x0040},         /* 0x1e "2@" */    
    {0xffc1, 0xffc1},         /* 0x1f "F4" */    
    {0xffc1, 0xffc1},         /* 0x20 */    
    {0x0063, 0x0043},         /* 0x21 "C" */    
    {0x0078, 0x0058},         /* 0x22 "X" */    
    {0x0064, 0x0044},         /* 0x23 "D" */    
    {0x0065, 0x0045},         /* 0x24 "E" */    
    {0x0034, 0x0024},         /* 0x25 "4$" */    
    {0x0033, 0x0023},         /* 0x26 "3#" */    
    {0xffc2, 0xffc2},         /* 0x27 "F5" */    
    {0x0000, 0x0000},         /* 0x28 */    
    {0x0020, 0x0020},         /* 0x29 "space" */    
    {0x0076, 0x0056},         /* 0x2a "V" */    
    {0x0066, 0x0046},         /* 0x2b "F" */    
    {0x0074, 0x0054},         /* 0x2c "T" */    
    {0x0072, 0x0052},         /* 0x2d "R" */    
    {0x0035, 0x0025},         /* 0x2e "5%" */    
    {0xffc3, 0xffc3},         /* 0x2f "F6" */    
    {0x0000, 0x0000},         /* 0x30 */    
    {0x006e, 0x004e},         /* 0x31 "N" */    
    {0x0062, 0x0042},         /* 0x32 "B" */    
    {0x0068, 0x0048},         /* 0x33 "H" */    
    {0x0067, 0x0047},         /* 0x34 "G" */    
    {0x0079, 0x0059},         /* 0x35 "Y" */    
    {0x0036, 0x005e},         /* 0x36 "6^" */    
    {0xffc4, 0xffc4},         /* 0x37 "F7" */    
    {0x0000, 0x0000},         /* 0x38 */    
    {0xffea, 0xffea},         /* 0x39 "RAlt" */    
    {0x006d, 0x004d},         /* 0x3a "M" */    
    {0x006a, 0x004a},         /* 0x3b "J" */    
    {0x0075, 0x0055},         /* 0x3c "U" */    
    {0x0037, 0x0026},         /* 0x3d "7&" */    
    {0x0038, 0x002a},         /* 0x3e "8*" */    
    {0xffc5, 0xffc5},         /* 0x3f "F8" */    
    {0x0000, 0x0000},         /* 0x40 */    
    {0x002c, 0x003c},         /* 0x41 ",<" */    
    {0x006b, 0x004b},         /* 0x42 "K" */    
    {0x0069, 0x0049},         /* 0x43 "I" */    
    {0x006f, 0x004f},         /* 0x44 "O" */    
    {0x0030, 0x0029},         /* 0x45 "0)" */    
    {0x0039, 0x0028},         /* 0x46 "9(" */    
    {0xffc6, 0xffc6},         /* 0x47 "F9" */    
    {0x0000, 0x0000},         /* 0x48 */    
    {0x002e, 0x003e},         /* 0x49 ".>" */    
    {0x002f, 0x003f},         /* 0x4a "/?" */    
    {0x006c, 0x004c},         /* 0x4b "L" */    
    {0x003b, 0x003a},         /* 0x4c ";:" */    
    {0x003b, 0x003a},         /* 0x4d "P" */    
    {0x002d, 0x005f},         /* 0x4e "-_" */    
    {0xffc7, 0xffc7},         /* 0x4f "F10" */    
    {0x0000, 0x0000},         /* 0x50 */    
    {0x0000, 0x0000},         /* 0x51 */    
    {0x0027, 0x005c},         /* 0x52 "'\""*/    
    {0x0000, 0x0000},         /* 0x53 */    
    {0x005b, 0x007b},         /* 0x54 "[{" */    
    {0x003d, 0x002b},         /* 0x55 "=+" */    
    {0xffc8, 0xffc8},         /* 0x56 "F11"  */    
    {0x0000, 0x0000},         /* 0x57 "PrtScr" */    
    {0xffe4, 0xffe4},         /* 0x58 "RCtrl" */    
    {0xffe2, 0xffe2},         /* 0x59 "RShift" */    
    {0xff0d, 0xff0d},         /* 0x5a "Enter" */    
    {0x005d, 0x007d},         /* 0x5b "]}" */    
    {0x005c, 0x007c},         /* 0x5c "\\|" */    
    {0x0000, 0x0000},         /* 0x5d */    
    {0xffc9, 0xffc9},         /* 0x5e "F12" */    
    {0xff14, 0xff14},         /* 0x5f "ScrollLock" */    
    {0xff54, 0xff54},         /* 0x60 "Down" */    
    {0xff51, 0xff51},         /* 0x61 "Left" */    
    {0x0000, 0x0000},         /* 0x62 "Pause" */    
    {0xff52, 0xff52},         /* 0x63 "Up" */    
    {0xffff, 0xffff},         /* 0x64 "Delete" */
    {0xff57, 0xff57},         /* 0x65 "End" */
    {0xff08, 0xff08},         /* 0x66 "Backspace" */    
    {0xff63, 0xff63},         /* 0x67 "Insert" */
    {0x0000, 0x0000},         /* 0x68 */
    {0xff9c, 0xffb1},         /* 0x69 "KP-1/End" */
    {0xff53, 0xff53},         /* 0x6a "Right" */
    {0xff96, 0xffb4},         /* 0x6b "KP-4/Left" */
    {0xff95, 0xffb7},         /* 0x6c "KP-7/Home" */
    {0xff56, 0xff56},         /* 0x6d "PgDn" */
    {0xff50, 0xff50},         /* 0x6e "Home" */
    {0xff55, 0xff55},         /* 0x6f "PgUp" */
    {0xff9e, 0xffb0},         /* 0x70 "KP-0/Ins" */
    {0xff9f, 0xffae},         /* 0x70 "KP-./Del" */
    {0xff99, 0xffb2},         /* 0x72 "KP-2/Down" */
    {0x0000, 0xffb5},         /* 0x73 "KP-5" */
    {0xff98, 0xffb6},         /* 0x74 "KP-6/Right" */
    {0xff97, 0xffb8},         /* 0x75 "KP-8/Up" */
    {0xff7f, 0xff7f},         /* 0x76 "NumLock" */    
    {0xffad, 0xffad},         /* 0x77 "KP-/" */
    {0x0000, 0x0000},         /* 0x78 */
    {0xff8d, 0xff8d},         /* 0x79 "KP-Enter" */
    {0x0000, 0x0000},         /* 0x7a */
    {0xffad, 0xffad},         /* 0x7b "KP--" */
    {0xffab, 0xffab},         /* 0x7c "KP-+" */
    {0xff9a, 0xffb9},         /* 0x7d "KP-9/PgUp" */
    {0xffaa, 0xffaa},         /* 0x7e "KP-*" */
    {0x0000, 0x0000},         /* 0x7f */
};

unsigned char sysrq_makecode2[] = {
    0xe0, 0x12, 0xe0, 0x7c
};
int sysrq_makecode2size = sizeof(sysrq_makecode2) / sizeof(unsigned char);

unsigned char sysrq_breakcode2[] = {
    0xe0, 0xf0, 0x7c, 0xe0, 0xf0, 0x12
};
int sysrq_breakcode2size = sizeof(sysrq_breakcode2) / sizeof(unsigned char);
                                                                                
unsigned char pause_makecode2[] = {
    0xe1, 0x14, 0x77, 0xe1, 0xf0, 0x14, 0xf0, 0x77
};
int pause_makecode2size = sizeof(pause_makecode2) / sizeof(unsigned char);

static unsigned char v2d_sysrq_makecode2[] = {
    0xaa, 0xe0, 0xaa, 0x12, 0xaa, 0xe0, 0xaa, 0x7c 
};
int v2d_sysrq_makecode2size = sizeof(v2d_sysrq_makecode2) / sizeof(unsigned char);

static unsigned char v2d_sysrq_breakcode2[] = {
    0xaa, 0xe0, 0xaa, 0xf0, 0xaa, 0x7c, 0xaa, 0xe0, 0xaa, 0xf0, 0xaa, 0x12 
};
int v2d_sysrq_breakcode2size = sizeof(v2d_sysrq_breakcode2) / sizeof(unsigned char);

static unsigned char v2d_pause_makecode2[] = {
    0xaa, 0xe1, 0xaa, 0x14, 0xaa, 0x77, 0xaa, 0xe1, 
    0xaa, 0xf0, 0xaa, 0x14, 0xaa, 0xf0, 0xaa, 0x77
};
int v2d_pause_makecode2size = sizeof(v2d_pause_makecode2) / sizeof(unsigned char);

/*********************************************
 * Common TX and RX Keyboard/Mouse routines  * 
 *********************************************/
int keycode_from_name(char *name) {
    int i;
    for (i = 0; i < keycodesize; i++)
        if (strcmp(keycodes[i].name, name) == 0)
            return i;
    return 0;
}

/*********************************************************************
 * Returns length of break code for key 'keycode', KB type 'kbtype'  *
 * in location starting at 'brkcode' or 0 if there's no break code.  *
 *********************************************************************/
int get_break_code(int keycode, int kbtype, unsigned char *brkcode) {
    int i, brklen = 0;

    if (keycode == 0 || keycode == KEY_PAUSE)
        return 0;

    if (keycode == KEY_SYSRQ) {
        brklen = sysrq_breakcode2size;
        for (i = 0; i < brklen; i++) {
            brkcode[2*i] = 0xaa;
            brkcode[2*i + 1] = sysrq_breakcode2[i];
        }
    }
    else if (kbtype == KB_TYPE_PC && keycode >= 96) {
        brklen = 6;
        brkcode[0] = brkcode[2] = brkcode[4] = 0xaa;
        brkcode[1] = 0xe0;
        brkcode[3] = 0xf0;
        brkcode[5] = (keycodes[keycode].set2 & 0xff);
    }
    else {
        brklen = 4;
        brkcode[0] = brkcode[2] = 0xaa;
        brkcode[1] = 0xf0;
        if (kbtype == KB_TYPE_PC)
            brkcode[3] = (keycodes[keycode].set2 & 0xff);
        else
            brkcode[3] = (keycodes[keycode].set3 & 0xff);
    }

    return brklen;
}

/***********************************************************************
 * Searches "keycodes" table to determine if data in buffer 'buf' is a *
 * make or break for KB type 'kbtype'. Returns NEED_MORE_CHARS if need *
 * more data for a complete scan code, otherwise returns KEY_NONE or   *
 * the matching keycode from the "keycodes" table.                     *
 ***********************************************************************/
int keycode_match(unsigned char *buf, int len, int kbtype, int *is_break) {
    int i, rc = KEY_NONE;
    unsigned int code;

    *is_break = 0;

    if (len < 2 || (buf[1] == 0xf0 && len < 4)) {
        return NEED_MORE_CHARS;
    }

    if (kbtype == KB_TYPE_PC && ((buf[1] == 0xe0 && len < 4) ||
            (buf[1] == 0xe0 && buf[3] == 0xf0 && len < 6) ||
            (buf[1] == 0xe0 && buf[3] == 0x12 && len < 8) ||
            (buf[1] == 0xe0 && buf[3] == 0xf0 && buf[5] == 0x7c && len < 12) ||
            (buf[1] == 0xe1 && len < 16))) {
        return NEED_MORE_CHARS;
    }

    if (kbtype == KB_TYPE_PC) {
        if (len >= 8 &&
                buf[1] == sysrq_makecode2[0] &&
                buf[3] == sysrq_makecode2[1] &&
                buf[5] == sysrq_makecode2[2] &&
                buf[7] == sysrq_makecode2[3]) {
            return KEY_SYSRQ;
        }
        if (len >= 12 &&
                buf[1] == sysrq_breakcode2[0] &&
                buf[3] == sysrq_breakcode2[1] &&
                buf[5] == sysrq_breakcode2[2] &&
                buf[7] == sysrq_breakcode2[3] &&
                buf[9] == sysrq_breakcode2[4] &&
                buf[11] == sysrq_breakcode2[5]) {
            *is_break = 1;
            return KEY_SYSRQ;
        }
        if (len >= 16 &&
                buf[1] == pause_makecode2[0] &&
                buf[3] == pause_makecode2[1] &&
                buf[5] == pause_makecode2[2] &&
                buf[7] == pause_makecode2[3] &&
                buf[9] == pause_makecode2[4] &&
                buf[11] == pause_makecode2[5] &&
                buf[13] == pause_makecode2[6] &&
                buf[15] == pause_makecode2[7]) {
             return KEY_PAUSE;
        }
        if ((len >= 4 && buf[1] == 0xf0) ||
                (len >= 6 && buf[1] == 0xe0 && buf[3] == 0xf0)) {
            *is_break = 1;
        }
    }

    for (code = 0, i = 0; i < len && i < 8; i += 2) {
        if (buf[i + 1] == 0xf0)
            continue;
        code <<= 8;
        code |= buf[i + 1];
        if (buf[i + 1] != 0xe0)
            break;
    }

    for (i = 0; i <= 127; i++) {
        if ((kbtype == KB_TYPE_PC && code == keycodes[i].set2) ||
            (kbtype == KB_TYPE_SGI && code == keycodes[i].set3)) {
             rc = i; break;
        }
    }

    return rc;
}

/***********************************************************
 * Computes keyboard escape sequence (array of scan codes) *
 * based on an input array of escape codes.                *
 ***********************************************************/
int get_keyboard_escape(unsigned char *keyboard_escape, int esc[], int kbtype) 
{
    int i, j, keyboard_escape_len;
    int last_key_remaining_len = 0;
    int max_byte_len = MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS;

    if (keyboard_escape == NULL) 
        return -1;

    memset(keyboard_escape, 0, sizeof(keyboard_escape));
    keyboard_escape_len = 0;

    for (i = j = 0; i < MAX_ESC_CHARS; i++) {
        int code, kc;
        if ((code = esc[i]) == 0) continue;
        else {
            kc = (kbtype == KB_TYPE_PC) ?
                keycodes[code].set2 : keycodes[code].set3;
            if ((kc & 0xff00) == 0xe000) {
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = 0xe0;
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = (unsigned char) (kc & 0xff);
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = 0xe0;
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = 0xf0;
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = (unsigned char) (kc & 0x00ff);
                keyboard_escape_len += 10;
                last_key_remaining_len = 6;
            }
            else {
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = (unsigned char) kc;
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = 0xf0;
                keyboard_escape[j++] = 0xaa;
                keyboard_escape[j++] = (unsigned char) (kc & 0x00ff);
                keyboard_escape_len += 6;
                last_key_remaining_len = 4;
            }
        }
    }
    /* the byte after keyboard_escape_len is written with last key break code
     * length , this is to indicate where to break the keycode, when to hold 
     * the last key and not to sent for further processing */

    if (keyboard_escape_len < max_byte_len && last_key_remaining_len > 0)
        keyboard_escape[keyboard_escape_len] = keyboard_escape_len - last_key_remaining_len;
    
    
    return keyboard_escape_len;
}

/*******************************************************************
 * Converts keyboard events in 'evbuf' to V2D-style keyboard input *
 * Only keyboard events in 'evbuf' are processed, everything else  * 
 * will be ignored (i.e. we assume keyboard/mouse events are never *
 * mixed in the same USB event packet).                            *
 *******************************************************************/
int fill_kb_buffer(unsigned char *buff, 
                   int maxbufflen,
                   struct input_event *evbuf,
                   int evbuflen,
                   int kbtype,
                   int autorepeat) {

    int i, kblen = 0;
                                                                                
    if (kbtype != KB_TYPE_PC && kbtype != KB_TYPE_SGI) 
        return 0;

    for (i = 0; i < evbuflen / sizeof(struct input_event); i++) {
        unsigned char sc[8];
        unsigned int kc;
        int type, code, value, j, len = 0;
        struct input_event *ev = &evbuf[i];

        type = ev->type; code = ev->code; value = ev->value;

        if (!KB_EVENT(type, code) || (value < 0 && value > 2))
            continue;

        kc = (kbtype == KB_TYPE_PC) ? keycodes[code].set2 : keycodes[code].set3;

        if (kc == 0) 
            break;

        if (value == 2) {                    /* repeat */
            if (!autorepeat || code == KEY_SYSRQ || code == KEY_PAUSE || 
                    code == KEY_LEFTCTRL || code == KEY_RIGHTCTRL ||
                    code == KEY_LEFTALT || code == KEY_RIGHTALT || 
                    code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT || 
                    code == KEY_LEFTMETA || code == KEY_RIGHTMETA || 
                    code == KEY_F1 || code == KEY_F2 || code == KEY_F3 ||
                    code == KEY_F4 || code == KEY_F5 || code == KEY_F6 ||
                    code == KEY_F7 || code == KEY_F8 || code == KEY_F9 ||
                    code == KEY_F10 || code == KEY_F11 || code == KEY_F12 ||
                    code == KEY_CAPSLOCK || code == KEY_NUMLOCK ||
                    code == KEY_SCROLLLOCK ) {
                continue;
            }
            else {
                len = 3;
                sc[0] = 0xf0;
                sc[1] = (unsigned char) (kc & 0x7f);
                sc[2] = (unsigned char) (kc & 0x7f);
            }
        }
        else if (value == 1) {               /* Key press */
            if (code == KEY_SYSRQ) {
                len = sysrq_makecode2size;
                memcpy(sc, sysrq_makecode2, len);
            }
            else if (code == KEY_PAUSE) {
                len = pause_makecode2size;
                memcpy(sc, pause_makecode2, len);
            }
            else if ((kc & 0xff00) == 0xe000) {
                len = 2;
                sc[0] = 0xe0;
                sc[1] = (unsigned char) (kc & 0x7f);
            }
            else {
                len = 1;
                sc[0] = (unsigned char) kc;
            }
        }
        else {                              /* Key release */
            switch (kbtype) {
            case 2:
                if (code == KEY_SYSRQ) {
                    len = sysrq_breakcode2size;
                    memcpy(sc, sysrq_breakcode2, len);
                }
                else if (code == KEY_PAUSE) {
                    /* No break code */
                    continue;
                }
                else if ((kc & 0xff00) == 0xe000) {
                    len = 3;
                    sc[0] = 0xe0;
                    sc[1] = 0xf0;
                    sc[2] = (unsigned char) (kc & 0x7f);
                }
                else {
                    len = 2;
                    sc[0] = 0xf0; 
                    sc[1] = (unsigned char) (kc & 0x7f);
                }
                break;
            case 3:
                len = 2;
                sc[0] = 0xf0;
                sc[1] = (unsigned char) (kc & 0x7f);
                break;
            }
        }

        if (kblen + 2 * len >= maxbufflen) 
            break;

        /**************************************************
         * Add 0xaa in front of each KB byte to make data *
         * into the FPGA PS/2 format for the V2D Tx       *
         **************************************************/
        for (j = 0; j < len; j++) {
            buff[kblen + 2 * j] = 0xaa;
            buff[kblen + 2 * j + 1] = sc[j];
        }
        kblen += 2 * len;
    }

    return kblen;
}

/************************************************************
 * Converts mouse events in 'evbuf' to V2D-style mouse data *
 ************************************************************/
int fill_ms_buffer(unsigned char *buff,
                   int maxbufflen,
                   struct input_event *evbuf,
                   int evbuflen,
                   int mstype,
                   int msmode,
                   int *newmode) {

    signed char dat[4];
    int i, len, mslen = 0, index;
    int dx = 0, dy = 0, dz = 0, buttons = 0;
    
    *newmode = msmode;

    if (mstype != MS_TYPE_PS2 && mstype != MS_TYPE_IMPS2) 
        return 0;

    if (msmode & MSMODE_BTN_LEFT)
        buttons |= (0x01 << 0); 
    if (msmode & MSMODE_BTN_RIGHT)
        buttons |= (0x01 << 1);
    if (msmode & MSMODE_BTN_MIDDLE)
        buttons |= (0x01 << 2);

    for (i = 0; i < evbuflen / sizeof(struct input_event); i++) {
        int type, code, value;
        struct input_event *ev = &evbuf[i];
       
        type = ev->type, code = ev->code, value = ev->value;

        if (type == EV_REL) {
            if (code == REL_X) 
                dx += value;
            else if (code == REL_Y)
                dy -= value;
            else if (code == REL_WHEEL && mstype == MS_TYPE_IMPS2)
                dz -= value;
        }
        else if (type == EV_KEY) {
            if (code == BTN_0 || code == BTN_LEFT)
                index = 0;
            else if (code == BTN_1 || code == BTN_RIGHT)
                index = 1;
            else if (code == BTN_2 || code == BTN_MIDDLE)
                index = 2;
            else
                continue;
            if (value == 0) {
                buttons &= ~(0x01 << index); 
                if (code == BTN_0 || code == BTN_LEFT)
                    *newmode &= ~MSMODE_BTN_LEFT;
                else if (code == BTN_1 || code == BTN_RIGHT)
                    *newmode &= ~MSMODE_BTN_RIGHT;
                else
                    *newmode &= ~MSMODE_BTN_MIDDLE;
            }
            else if (value == 1) {
                buttons |= (0x01 << index);
                if (code == BTN_0 || code == BTN_LEFT)
                    *newmode |= MSMODE_BTN_LEFT;
                else if (code == BTN_1 || code == BTN_RIGHT)
                    *newmode |= MSMODE_BTN_RIGHT;
                else
                    *newmode |= MSMODE_BTN_MIDDLE;
            }
        }
    }

    len =  (mstype == MS_TYPE_PS2) ? (MS_PS2_PKTSIZE/2) : (MS_IMPS2_PKTSIZE/2);
    do {
        dat[0] = 0x08 | ((dx < 0) << 4) | ((dy < 0) << 5) | (buttons & 0x07);
        dat[1] = (dx > 127) ? 127 : ((dx < -127) ? -127 : dx);
        dat[2] = (dy > 127) ? 127 : ((dy < -127) ? -127 : dy);
        dx -= dat[1];
        dy -= dat[2];
        if (mstype == MS_TYPE_IMPS2) {
            dat[3] = (dz > 127) ? 127 : ((dz < -127) ? -127 : dz);
            dz -= dat[3];
        }
        else 
            dat[3] = 0;

        if (mslen + 2 * len >= maxbufflen) 
            break;

        /*****************************************************
         * Add 0x00 or 0x03 in front of each MS byte to make *
         * data into the FPGA PS/2 format for the V2D Tx     *
         *****************************************************/
        for (i = 0; i < len; i++) {
            buff[mslen + 2 * i] = mstype;
            buff[mslen + 2 * i + 1] = dat[i];
        }
        mslen += 2 * len;
    } while (dx != 0 || dy != 0 || (mstype == MS_TYPE_IMPS2 && dz != 0)); 

    return mslen;
}

/****************************************************************************
 * Translate V2D-style PS2 keyboard input to RFB key events. Called with    *
 * 'kbbuf' pointing to the PS2 keyboard buffer, 'kbbuflen' has the number   *
 * of bytes in the keyboard buffer, '*numlock', '*capslock' and '*shift'    *
 * have the states of the numlock, capslock and shift keys, resp., 'rfbbuf' *
 * pointing to the buffer for the translated RFB events (4-byte key followed*
 * by 1 byte down flag. Returns number of bytes in RFB buffer with *numlock,*
 * *capslock and *shift containing the new numlock, capslock & shift states.*
 ****************************************************************************/
#define LSHIFT          0x12
#define RSHIFT          0x59
#define SET2CAPSLOCK    0x58
#define SET3CAPSLOCK    0x14
#define SET2NUMLOCK     0x77
#define SET3NUMLOCK     0x76
#define KEYPAD_KEY(x)   (((x) == 0x70) || \
                         ((x) == 0x69) || \
                         ((x) == 0x72) || \
                         ((x) == 0x7a) || \
                         ((x) == 0x6b) || \
                         ((x) == 0x73) || \
                         ((x) == 0x74) || \
                         ((x) == 0x6c) || \
                         ((x) == 0x75) || \
                         ((x) == 0x7d) || \
                         ((x) == 0x71))

int xlate_set3_to_rfb(unsigned char *kbbuf, 
                      unsigned char *rfbbuf,
                      unsigned char *numlock,
                      unsigned char *capslock,
                      unsigned char *shift,
                      int kbbuflen) {
    int i = 0, j = 0;
    unsigned long lval;

    VLOG_DEBUG_CAT(VLOG_KBM_RFB, 
            "%s. numlock %d, capslock %d, shift %d, KB buffer len %d:", 
            __FUNCTION__, *numlock, *capslock, *shift, kbbuflen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, kbbuf, kbbuflen);

    while (i < kbbuflen - 1) {
        unsigned char sc1 = kbbuf[i+1];
        if ((sc1 = kbbuf[i+1]) == 0xf0) {
            if (i > kbbuflen - 4) 
                break;
            else {
                unsigned char sc2 = kbbuf[i+3];
                if (sc2 == LSHIFT || sc2 == RSHIFT)
                    *shift = (*capslock) ? 1 : 0;
                else if (sc2 != SET3CAPSLOCK && sc2 != SET3NUMLOCK) {
                    if (KEYPAD_KEY(sc2)) {
                        if (*numlock || *shift) 
                            lval = htonl(set3_rfbkey[sc2].rfbkeyshift);
                        else
                            lval = htonl(set3_rfbkey[sc2].rfbkey);
                    }
                    else {
                        if (*shift || *capslock) 
                            lval = htonl(set3_rfbkey[sc2].rfbkeyshift);
                        else
                            lval = htonl(set3_rfbkey[sc2].rfbkey);
                    }
                    memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                    rfbbuf[j+4] = 0; 
                }
                j += 5;
            }
            i += 4;
        }
        else {
            if (sc1 == LSHIFT || sc1 == RSHIFT) 
                *shift = 1;
            else if (sc1 == SET3CAPSLOCK) 
                *capslock = (*capslock) ? 0 : 1;
            else if (sc1 == SET3NUMLOCK) 
                *numlock = (*numlock) ? 0 : 1;
            else {
                if (KEYPAD_KEY(sc1)) {
                    if (*numlock || *shift) 
                        lval = htonl(set3_rfbkey[sc1].rfbkey);
                    else 
                        lval = htonl(set3_rfbkey[sc1].rfbkeyshift);
                }
                else {
                    if (*shift || *capslock) 
                        lval = htonl(set3_rfbkey[sc1].rfbkeyshift);
                    else 
                        lval = htonl(set3_rfbkey[sc1].rfbkey);
                }
                memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                rfbbuf[j+4] = 1; 
                j += 5;
            }
            i += 2;
        }
    }
    VLOG_DEBUG_CAT(VLOG_KBM_RFB, "%s returning %d bytes:", __FUNCTION__, j);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, j);

    return j;
}

static int get_set2_e0_rfbkey(unsigned char set2code) {
    int i;
    for (i = 0; i < set2_e0_rfbkeysize; i++) {
        if (set2code == set2_e0_rfbkey[i].set2code) {
            return set2_e0_rfbkey[i].rfbkey;
        }
    }
    return 0;
}
 
int xlate_set2_to_rfb(unsigned char *kbbuf, 
                      unsigned char *rfbbuf,
                      unsigned char *numlock,
                      unsigned char *capslock,
                      unsigned char *shift,
                      int kbbuflen) {
    int i = 0, j = 0;
    unsigned long lval;

    VLOG_DEBUG_CAT(VLOG_KBM_RFB, 
            "%s. numlock %d, capslock %d, shift %d, KB buffer len %d:", 
            __FUNCTION__, *numlock, *capslock, *shift, kbbuflen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, kbbuf, kbbuflen);

    while (i < kbbuflen - 1) {
        unsigned char sc1 = kbbuf[i+1];

        if (sc1 == 0xe1) {
            if (i > kbbuflen - v2d_pause_makecode2size)
               break;
            if (!memcmp(&kbbuf[i], v2d_pause_makecode2, 
                        v2d_pause_makecode2size)) {
                lval = htonl(XK_Pause);
                memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                rfbbuf[j+4] = 0;
                i += v2d_pause_makecode2size;
                j += 5;
            }
            else break;
        }
        else if (sc1 == 0xe0) {
            if (i > kbbuflen - 4) 
                break;
            else {
                unsigned char sc2 = kbbuf[i+3];
                if (sc2 == 0xf0) {
                    if (i <= kbbuflen - v2d_sysrq_breakcode2size) {
                        if (!memcmp(&kbbuf[i], v2d_sysrq_breakcode2, 
                                v2d_sysrq_breakcode2size)) {
                           lval = htonl(XK_Pause);
                           memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                           rfbbuf[j+4] = 0;
                           i += v2d_sysrq_breakcode2size;
                           j += 5;
                           continue;
                        }
                    }
                    if (i > kbbuflen - 6) 
                        break;
                    else {
                        int sc3 = get_set2_e0_rfbkey(kbbuf[i+5]);
                        lval = htonl(sc3);
                        memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                        rfbbuf[j+4] = 0;
                        i += 6;
                        j += 5;
                    }
                }
                else {
                    if (i <= kbbuflen - v2d_sysrq_makecode2size) {
                        if (!memcmp(&kbbuf[i], v2d_sysrq_makecode2, 
                                v2d_sysrq_makecode2size)) {
                            lval = htonl(XK_Sys_Req);
                            memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                            rfbbuf[j+4] = 1;
                            i += v2d_sysrq_makecode2size;
                            j += 5;
                            continue;
                        }
                    }
                    lval = htonl(get_set2_e0_rfbkey(sc2));
                    memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                    rfbbuf[j+4] = 1;
                    i += 4;
                    j += 5;
                }
            }
        }
        else if (sc1 == 0xf0) {
            if (i > kbbuflen - 4) 
                break;
            else {
                unsigned char sc2 = kbbuf[i+3];
                if (sc2 == LSHIFT || sc2 == RSHIFT) {
                    *shift =  0;
                    lval = htonl(set2_rfbkey[sc2].rfbkey);
                    memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                    rfbbuf[j+4] = 0;
                    j += 5;
                }
                else if (sc2 != SET2CAPSLOCK && sc2 != SET2NUMLOCK) {
                    if (KEYPAD_KEY(sc2)) {
                        if (*numlock) 
                            lval = htonl(set2_rfbkey[sc2].rfbkeyshift);
                        else 
                            lval = htonl(set2_rfbkey[sc2].rfbkey);
                    }
                    else {
                        if (*shift || *capslock)  
                            lval = htonl(set2_rfbkey[sc2].rfbkeyshift);
                        else
                            lval = htonl(set2_rfbkey[sc2].rfbkey);
                    }
                    memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                    rfbbuf[j+4] = 0;
                    j += 5;
                }
                i += 4;
            }
        }
        else {
            if (sc1 == LSHIFT || sc1 == RSHIFT) {
                *shift = 1;
                lval  = htonl(set2_rfbkey[sc1].rfbkey);
                memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                rfbbuf[j+4] = 1;
                j += 5;
            }
            else if (sc1 == SET2CAPSLOCK) 
                *capslock = (*capslock) ? 0 : 1;
            else if (sc1 == SET2NUMLOCK) 
                *numlock = (*numlock) ? 0 : 1;
            else {
                if (KEYPAD_KEY(sc1)) {
                    if (*numlock ) 
                        lval = htonl(set2_rfbkey[sc1].rfbkeyshift);
                    else 
                        lval = htonl(set2_rfbkey[sc1].rfbkey);
                }
                else {
                    if (*shift || *capslock)  
                        lval = htonl(set2_rfbkey[sc1].rfbkeyshift);
                    else
                        lval = htonl(set2_rfbkey[sc1].rfbkey);
                }
                memcpy(&rfbbuf[j], &lval, sizeof(unsigned long));
                rfbbuf[j+4] = 1;
                j += 5;
            }
            i += 2;
        }
    }
    VLOG_DEBUG_CAT(VLOG_KBM_RFB, "%s returning %d bytes:", __FUNCTION__, j);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, j);

    return j;
}

/***********************************
 * Write keyboard data to the FPGA *
 ***********************************/
int write_to_kb_fpga(HAL *hp, 
                     unsigned char *buf, 
                     int buflen) 
{

    int i, rc = 0, kbfd = hal_get_keyboard_fd(hp);
    int is_plx_device = (hal_get_hardware_type(hp) == PLX_DEVICE) ? 1 : 0;
    int cno = hal_get_channel_number(hp);

    if (buflen < 2) return rc;

    /**********************************************************
     * For XP or I50 FPGA version > 1, set the byte preceding *
     * KB data to indicate whether it's a "Start of Chunk" or *
     * "End of Chunk" so FPGA can retransmit if necessary on  *
     * the TX-Host PS/2 interface.                            *  
     **********************************************************/
    for (i = 0; i < buflen; i += 2) {
        if (i > KB_BUFFER_SIZE) i -= KB_BUFFER_SIZE;
        if ((is_plx_device || hal_get_fpga_version(hp) >= 2) && 
            (buf[i + 1] == 0xe0 || buf[i + 1] == 0xe1 || buf[i + 1]==0xf0)) {
            buf[i] = 0x9a;
            i = (i + 2) % KB_BUFFER_SIZE;
        }
        else
            buf[i] = 0xaa; 
    }

    VLOG_DEBUG_CAT(VLOG_KBM_KB, "Channel %d. Writing %d bytes KB Data", 
            cno, buflen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_KB, buf, buflen);

    if ((rc = write(kbfd, buf, buflen)) < 0) 
        VLOG_WARNING_CAT(VLOG_KBM_KB,
                "Channel %d. Unable to write KB data to FPGA.", cno);
    return rc;
}

/********************************
 * Write mouse data to the FPGA *
 ********************************/
int write_to_ms_fpga(HAL *hp, 
                     unsigned char *buf, 
                     int buflen, 
                     int mouse_type) 
{

    int len, sz, id, rc = 0, msfd = hal_get_mouse_fd(hp);
    unsigned char tmpbuf[MS_BUFFER_SIZE * MS_IMPS2_PKTSIZE / MS_PS2_PKTSIZE];
    unsigned char sop, eop, mop;
    int i, j;
    unsigned char *bp;
    unsigned short fpga_version = hal_get_fpga_version(hp);
    int is_plx_device = (hal_get_hardware_type(hp) == PLX_DEVICE) ? 1 : 0;
    int cno = hal_get_channel_number(hp);
#ifdef DEBUG_MOUSE
    struct timeval now;
    unsigned int days, hrs, mins, secs, millisecs;

    gettimeofday(&now, (struct timezone *) 0);
#endif

    if (mouse_type == MS_TYPE_IMPS2) {
        sz = MS_IMPS2_PKTSIZE;
        id = MS_ID_IMPS2;
    }
    else { 
        sz = MS_PS2_PKTSIZE;
        id = MS_ID_PS2;
    }

    if (buflen != MS_IMPS2_PKTSIZE && buflen != MS_PS2_PKTSIZE) 
        return rc;

    sop = eop = mop = id;
    if (is_plx_device || fpga_version >= 10) {
        sop = (0x10 | id);
        eop = (0x20 | id);
    }
    else if (fpga_version >= 2) {
        sop = (0x20 | id);
        eop = (0x10 | id);
    }

    /**********************************************************
     * Pad or delete bytes in mouse packet to make it match   *
     * the confgured mouse type and set the byte preceding    *
     * MS data to indicate "Start of Packet", "End of Packet" *
     * and also the mouse id of the packet for the FPGA.      *
     **********************************************************/
    if (buflen % sz != 0) {
        i = len = 0;
        while (i < buflen) {
            memcpy(&tmpbuf[len], &buf[i], MS_PS2_PKTSIZE);
            if (sz == MS_IMPS2_PKTSIZE) {
                memset(&tmpbuf[len + MS_PS2_PKTSIZE], 0, 2);
            }
            tmpbuf[len] = sop;
            tmpbuf[len + sz - 2] = eop;
            for (j = 2; j < sz - 2; j += 2)
                tmpbuf[len + j] = mop;
            len += sz; 
            if (sz == MS_PS2_PKTSIZE) 
                i += MS_IMPS2_PKTSIZE;
            else 
                i += MS_PS2_PKTSIZE;
        }
        bp = tmpbuf;
    }
    else {
        i = 0;
        while (i < buflen) {
            buf[i] = sop;
            buf[i + sz - 2] = eop;
            for (j = 2; j < sz - 2; j += 2)
                buf[i + j] = mop;
            i += sz;
        }
        bp = buf; len = buflen;
    }

#ifdef  DEBUG_MOUSE 
    millisecs = now.tv_usec / 1000;
    secs = now.tv_sec;
    mins = secs / 60;
    secs -= mins * 60;
    hrs = mins / 60;
    mins -= hrs * 60;
    days = hrs / 24;
    hrs -= days * 24;
    VLOG_INFO_CAT(VLOG_KBM_MS, 
            "Channel %d. Writing %d bytes MS data[%02d:%02d:%02d:%03d]", 
    VLOG_INFO_CAT(KBMMS,
            "Channel %d. Writing %d bytes MS data[%02d:%02d:%02d:%03d]", 
            cno, len, hrs, mins, secs, millisecs);
    VLOG_INFO_HEXDATA_CAT(VLOG_KBM_MS, bp, len);
#else
    VLOG_DEBUG_CAT(VLOG_KBM_MS, 
            "Channel %d. Writing %d bytes MS data", cno, len);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_MS, bp, len);
#endif

    if ((rc = write(msfd, bp, len)) < 0) {
        VLOG_WARNING_CAT(VLOG_KBM_MS, 
                "Channel %d. Unable to write MS data to FPGA.", cno);
        return rc;
    }
    else
        return buflen;
}

/*****************************************
 * Routine to set serial port parameters *
 *****************************************/
void set_serial_parameters(int fd,
                           int baudrate,
                           int databits,
                           int stopbits,
                           unsigned char parity, 
                           unsigned char flowcontrol)
{
    struct termios tty;
    int speed = B9600;
    int bits = 8;
    int stops = 1;
    unsigned char par = 'n';
    unsigned char fctl = 'n';

    if (baudrate == 300)
        speed = B300;
    else if (baudrate == 600)
        speed = B600;
    else if (baudrate == 1200)
        speed = B1200;
    else if (baudrate == 2400)
        speed = B2400;
    else if (baudrate == 4800)
        speed = B4800;
    else if (baudrate == 9600)
        speed = B9600;
    else if (baudrate == 19200)
        speed = B19200;
    else if (baudrate == 38400)
        speed = B38400;
    else if (baudrate == 57600)
        speed = B57600;
    else if (baudrate == 115200)
        speed = B115200;

    if (databits >= 5 && databits <= 8)
        bits = databits;

    if (stopbits >= 1 && stopbits <= 2)
        stops = stopbits;

    if (parity == 'e' || parity == 'E' || parity == 'o' || parity == 'O')
        par = parity;

    if (flowcontrol == 'x' || flowcontrol == 'X' ||
        flowcontrol == 'h' || flowcontrol == 'H')
        fctl = flowcontrol;

    /* get current terminal attribute */
    tcgetattr(fd, &tty);

    /* set baudrate */
    tty.c_cflag &= ~0x100f;
    tty.c_cflag |= speed;
    tty.c_ispeed = (speed_t) speed;
    tty.c_ospeed = (speed_t) speed;

    /* set data bits */
    switch (bits) {
    case 5:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
        break;
    case 6:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
        break;
    case 7:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
        break;
    case 8:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        break;
    }

    /* set stop bits */
    if (stops == 2)
        tty.c_cflag |= CSTOPB;
    else
        tty.c_cflag &= ~CSTOPB;

    /* set line to raw, noecho mode */
    tty.c_iflag = IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;

    /* ignore modem lines, enable receiver */
    tty.c_cflag |= CLOCAL | CREAD;

    /* set min. # chars for read to 1, read timeout to 0.5 sec */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    /* set flow control */
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF);
    if (fctl == 'x' || fctl == 'X')
        tty.c_iflag |= IXON | IXOFF;
    else if (fctl == 'h' || fctl == 'H')
        tty.c_cflag |= CRTSCTS;

    /* set parity */
    tty.c_cflag &= ~(PARENB | PARODD);
    if (par == 'e' || par == 'E')
        tty.c_cflag |= PARENB;
    else if (par == 'o' || par == 'O')
        tty.c_cflag |= PARODD;

    /* turn on hang up on close */
    tty.c_cflag |= HUPCL;

    /* set new terminal attribute */
    tcsetattr(fd, TCSANOW, &tty);

    /* flush both input & output */
    tcflush(fd, TCIOFLUSH);
}


int get_set2_scancode(unsigned char *kbbuf,
                      unsigned char *rfbbuf,
                      unsigned char *numlock,
                      unsigned char *capslock,
                      unsigned char *shift,
                      int kbbuflen,
                      unsigned int *scancode,
                      unsigned char *keydown)
{
    int i = 0, j = 0;
    int nworder;
    *scancode =0;
    *keydown = 0;

    VLOG_DEBUG_CAT(VLOG_KBM_RFB,
        "%s. numlock %d, capslock %d, shift %d, KB buffer len %d:", 
        __FUNCTION__, *numlock, *capslock, *shift, kbbuflen);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, kbbuf, kbbuflen);

    while (i < kbbuflen - 1) {
        unsigned char sc1 = kbbuf[i+1];
        //VLOG_INFO_CAT(VLOG_KBM_KB,"get_set2_scancode : Sc1= [%02X] i=%d \n",sc1,i);	

        if (sc1 == 0xe1) 
        {
            if (i > kbbuflen - v2d_pause_makecode2size)
                break;
            if (!memcmp(&kbbuf[i], v2d_pause_makecode2, 
                v2d_pause_makecode2size)) 
            {
                // *((unsigned int *) &rfbbuf[j]) = htonl(XK_Pause); 
                *scancode = XK_Pause;
                *keydown = 1;
                nworder = htonl(XK_Pause);
                rfbbuf[j+0] =  nworder & 0xff;
                rfbbuf[j+1] = (nworder >>8) & 0xff;
                rfbbuf[j+2] = (nworder >> 16) & 0xff;
                rfbbuf[j+3] = (nworder >> 24) & 0xff;
                rfbbuf[j+4] = 0;
                i += v2d_pause_makecode2size;
                j += 5;
            }
            else break;
        }
        else if (sc1 == 0xe0) 
        {
            if (i > kbbuflen - 4) 
                break;
            else {
                unsigned char sc2 = kbbuf[i+3];
                //VLOG_INFO_CAT(VLOG_KBM_KB,"get_set2_scancode : Sc2= [%02X] i=%d\n",sc2,i);
                if (sc2 == 0xf0) 
                {
                    if (i <= kbbuflen - v2d_sysrq_breakcode2size) 
                    {
                        if (!memcmp(&kbbuf[i], v2d_sysrq_breakcode2, 
                            v2d_sysrq_breakcode2size)) 
                        {
                            //   *((unsigned int *) &rfbbuf[j]) = htonl(XK_Sys_Req); 
                            *scancode = XK_Sys_Req;
                            *keydown = 0;
                            nworder = htonl(XK_Sys_Req);
                            rfbbuf[j+0] =  nworder & 0xff;
                            rfbbuf[j+1] = (nworder >>8) & 0xff;
                            rfbbuf[j+2] = (nworder >> 16) & 0xff;
                            rfbbuf[j+3] = (nworder >> 24) & 0xff;
                            rfbbuf[j+4] = 0;
                            i += v2d_sysrq_breakcode2size;
                            j += 5;
                            continue;
                        }
                    }
                    if (i > kbbuflen - 6) 
                        break;
                    else {
                        //VLOG_INFO_CAT(VLOG_KBM_KB,"get_set2_scancode : Sc3= [%02X] i=%d\n",kbbuf[i+5],i);
                        int sc3 = get_set2_e0_rfbkey(kbbuf[i+5]);
                        //*scancode = sc3;
                        *scancode = (unsigned int ) sc1 << 8 | (unsigned int ) kbbuf[i+5];
                        *keydown = 0;
                        //   *((unsigned int *) &rfbbuf[j]) = htonl(sc3);

                        nworder = htonl(sc3);
                        rfbbuf[j+0] =  nworder & 0xff;
                        rfbbuf[j+1] = (nworder >>8) & 0xff;
                        rfbbuf[j+2] = (nworder >> 16) & 0xff;
                        rfbbuf[j+3] = (nworder >> 24) & 0xff;
                        rfbbuf[j+4] = 0;
                        i += 6;
                        j += 5;
                    }
                }
                else 
                {
                    if (i <= kbbuflen - v2d_sysrq_makecode2size) 
                    {
                        if (!memcmp(&kbbuf[i], v2d_sysrq_makecode2, 
                            v2d_sysrq_makecode2size)) 
                        {
                            //  *((unsigned int *) &rfbbuf[j]) = htonl(XK_Sys_Req); 
                            *scancode = XK_Sys_Req;
                            *keydown = 1;
                            nworder = htonl(XK_Sys_Req);
                            rfbbuf[j+0] =  nworder & 0xff;
                            rfbbuf[j+1] = (nworder >>8) & 0xff;
                            rfbbuf[j+2] = (nworder >> 16) & 0xff;
                            rfbbuf[j+3] = (nworder >> 24) & 0xff;
                            rfbbuf[j+4] = 1;
                            i += v2d_sysrq_makecode2size;
                            j += 5;
                            continue;
                        }
                    }
                    // *((unsigned int *) &rfbbuf[j]) = 
                    //    htonl(get_set2_e0_rfbkey(sc2));
                    //*scancode = get_set2_e0_rfbkey(sc2);
                    *scancode = (unsigned int ) sc1 << 8 | (unsigned int ) sc2;
                    *keydown = 1;
                    nworder = htonl(get_set2_e0_rfbkey(sc2));
                    rfbbuf[j+0] =  nworder & 0xff;
                    rfbbuf[j+1] = (nworder >>8) & 0xff;
                    rfbbuf[j+2] = (nworder >> 16) & 0xff;
                    rfbbuf[j+3] = (nworder >> 24) & 0xff;
                    rfbbuf[j+4] = 1;
                    i += 4;
                    j += 5;
                }
            }
        }
        else if (sc1 == 0xf0) 
        {
            if (i > kbbuflen - 4) 
                break;
            else 
            {
                unsigned char sc2 = kbbuf[i+3];
                //VLOG_INFO_CAT(VLOG_KBM_KB,"get_set2_scancode : Sc2= [%02X] i=%d\n",sc2,i);

                if (sc2 == LSHIFT || sc2 == RSHIFT) 
                {
                    *shift =  0;
                    //*scancode = set2_rfbkey[sc2].rfbkey;
                    *scancode = sc2;
                    *keydown = 0;
                    nworder  = htonl(set2_rfbkey[sc2].rfbkey);
                    rfbbuf[j+0] =  nworder & 0xff;
                    rfbbuf[j+1] = (nworder >>8) & 0xff;
                    rfbbuf[j+2] = (nworder >> 16) & 0xff;
                    rfbbuf[j+3] = (nworder >> 24) & 0xff;
                    rfbbuf[j+4] = 0;
                    j += 5;
                }
                else if (sc2 != SET2CAPSLOCK && sc2 != SET2NUMLOCK) 
                {
                    if (KEYPAD_KEY(sc2)) 
                    {
                        if (*numlock )
                        {
                            //  *((unsigned int *) &rfbbuf[j]) = 
                            //     htonl(set2_rfbkey[sc2].rfbkeyshift);
                            //*scancode = set2_rfbkey[sc2].rfbkeyshift;
                            *scancode = sc2;
                            *keydown = 0;
                            nworder = htonl(set2_rfbkey[sc2].rfbkeyshift);
                            rfbbuf[j+0] =  nworder & 0xff;
                            rfbbuf[j+1] = (nworder >>8) & 0xff;
                            rfbbuf[j+2] = (nworder >> 16) & 0xff;
                            rfbbuf[j+3] = (nworder >> 24) & 0xff;
                        }
                        else 
                        {
                            // *((unsigned int *) &rfbbuf[j]) = 
                            //  htonl(set2_rfbkey[sc2].rfbkey);
                            //*scancode = set2_rfbkey[sc2].rfbkey;
                            *scancode = sc2;
                            *keydown = 0;
                            nworder = htonl(set2_rfbkey[sc2].rfbkey);
                            rfbbuf[j+0] =  nworder & 0xff;
                            rfbbuf[j+1] = (nworder >>8) & 0xff;
                            rfbbuf[j+2] = (nworder >> 16) & 0xff;
                            rfbbuf[j+3] = (nworder >> 24) & 0xff;
                        }
                    }
                    else 
                    {
                        //  *((unsigned int *) &rfbbuf[j]) = 

                        //*scancode = (*shift || *capslock) ? set2_rfbkey[sc2].rfbkeyshift : set2_rfbkey[sc2].rfbkey;
                        *scancode = sc2;
                        *keydown = 0;

                        nworder = 
                            (*shift || *capslock) ? 
                            htonl(set2_rfbkey[sc2].rfbkeyshift) :
                            htonl(set2_rfbkey[sc2].rfbkey);


                        rfbbuf[j+0] =  nworder & 0xff;
                        rfbbuf[j+1] = (nworder >>8) & 0xff;
                        rfbbuf[j+2] = (nworder >> 16) & 0xff;
                        rfbbuf[j+3] = (nworder >> 24) & 0xff;

                    }
                    rfbbuf[j+4] = 0;
                    j += 5;
                }
                else if ( sc2 == SET2CAPSLOCK )
                {
                    j += 5;
                    *scancode = sc2;
                    *keydown = 0;
                }
                else if ( sc2 == SET2NUMLOCK )
                {
                    *scancode = sc2;
                    *keydown = 0;
                    j += 5;
                }
                i += 4;
            }
        }
        else 
        {
            if (sc1 == LSHIFT || sc1 == RSHIFT) 
            {
                *shift = 1;
                //*scancode = set2_rfbkey[sc1].rfbkey;
                *scancode = sc1;
                *keydown = 1;
                nworder  = htonl(set2_rfbkey[sc1].rfbkey);
                rfbbuf[j+0] =  nworder & 0xff;
                rfbbuf[j+1] = (nworder >>8) & 0xff;
                rfbbuf[j+2] = (nworder >> 16) & 0xff;
                rfbbuf[j+3] = (nworder >> 24) & 0xff;
                rfbbuf[j+4] = 1;
                j += 5;
            }
            else if (sc1 == SET2CAPSLOCK) 
            {
                *capslock = (*capslock) ? 0 : 1;
                j += 5;
                *scancode = sc1;
                *keydown = 1;

            }
            else if (sc1 == SET2NUMLOCK) 
            {
                *numlock = (*numlock) ? 0 : 1;
                j += 5;
                *scancode = sc1;
                *keydown = 1;

            }
            else 
            {
                if (KEYPAD_KEY(sc1)) 
                {
                    if (*numlock ) 
                    {
                        //     *((unsigned int *) &rfbbuf[j]) = 
                        //       htonl(set2_rfbkey[sc1].rfbkeyshift);
                        //*scancode = set2_rfbkey[sc1].rfbkeyshift;
                        *scancode = sc1;
                        *keydown = 1;
                        nworder = htonl(set2_rfbkey[sc1].rfbkeyshift);
                        rfbbuf[j+0] =  nworder & 0xff;
                        rfbbuf[j+1] = (nworder >>8) & 0xff;
                        rfbbuf[j+2] = (nworder >> 16) & 0xff;
                        rfbbuf[j+3] = (nworder >> 24) & 0xff;
                    }
                    else 
                    {
                        // *((unsigned int *) &rfbbuf[j]) = 
                        //*scancode = set2_rfbkey[sc1].rfbkey;
                        *scancode = sc1;
                        *keydown = 1;
                        nworder = htonl(set2_rfbkey[sc1].rfbkey);
                        rfbbuf[j+0] =  nworder & 0xff;
                        rfbbuf[j+1] = (nworder >>8) & 0xff;
                        rfbbuf[j+2] = (nworder >> 16) & 0xff;
                        rfbbuf[j+3] = (nworder >> 24) & 0xff;
                    }
                }
                else 
                {
                    //  *((unsigned int *) &rfbbuf[j]) = 
                    //*scancode =  (*shift || *capslock) ? set2_rfbkey[sc1].rfbkeyshift : set2_rfbkey[sc1].rfbkey;
                    *scancode =  sc1;
                    *keydown = 1;
                    nworder =
                        (*shift || *capslock) ? 
                        htonl(set2_rfbkey[sc1].rfbkeyshift) : 
                        htonl(set2_rfbkey[sc1].rfbkey);
                    rfbbuf[j+0] =  nworder & 0xff;
                    rfbbuf[j+1] = (nworder >>8) & 0xff;
                    rfbbuf[j+2] = (nworder >> 16) & 0xff;
                    rfbbuf[j+3] = (nworder >> 24) & 0xff;
                }
                rfbbuf[j+4] = 1;
                j += 5;
            }
            i += 2;
        }
    }
    VLOG_DEBUG_CAT(VLOG_KBM_RFB, "%s returning %d bytes:", __FUNCTION__, j);
    VLOG_DEBUG_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, j);

    return j;
}
