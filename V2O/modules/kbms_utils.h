#ifndef __KBMS_UTILS_H__
#define __KBMS_UTILS_H__

#include "hal.h"
#define KB_EVENT(t,c)   ((t == EV_KEY) && \
                         (c >= KEY_ESC && c <= KEY_COMPOSE))
                                                                                
#define MS_EVENT(t,c)   ((t == EV_REL && \
                         (c == REL_X || c == REL_Y || c == REL_WHEEL)) || \
                         (t == EV_KEY && c >= BTN_0 && c < KEY_MAX))

#define KEY_SYSRQ               99
#define KEY_PAUSE               119
#define KEY_NONE                -1

#define MSMODE_BTN_LEFT         0x0001
#define MSMODE_BTN_RIGHT        0x0002
#define MSMODE_BTN_MIDDLE       0x0004

#define KBM_TYPE_KEYBOARD       1
#define KBM_TYPE_MOUSE          2

#define EVENT_BUFFER_SIZE       (1024 * sizeof(struct input_event))

typedef struct keycode_s {
    unsigned int set2, set3;
    char *name;     /* keycap on a standard US keyboard */
} KEYCODE;

typedef struct rfbkey_s {
    unsigned int rfbkey, rfbkeyshift;
} RFBKEY;

typedef struct e0_rfbkey_s {
    unsigned char set2code;
    unsigned int rfbkey;
} E0RFBKEY;

#ifndef _INPUT_H
#include <linux/input.h>
#endif

#define KBM_MODE_REMOTE     1
#define KBM_MODE_CONSOLE    2 
#define KBM_MODE_LOCAL      3

#define kbm_mode_name_from_code(x)   ((x == KBM_MODE_LOCAL) ? "Local" : \
                                     ((x == KBM_MODE_REMOTE) ? "Remote" : \
                                     "Console"))

#define MAX_ESC_CHARS           4
#define MAX_BYTES_PER_ESC_CHAR  10
#define ESC_REMOTE              1
#define ESC_CONSOLE             2
#define ESC_LOCAL               3
#define ESC_NONE                -1
#define ESC_PARTIAL_MATCH        0
#define NEED_MORE_CHARS         -2
                                                                                
#define is_escape_code(x)   ((x) == ESC_CONSOLE || \
                            (x) == ESC_LOCAL || \
                            (x) == ESC_REMOTE)
                                                                                
#define kbmode_from_escape_code(x)  \
        (((x) == ESC_LOCAL) ? KBM_MODE_LOCAL : \
        (((x) == ESC_REMOTE) ? KBM_MODE_REMOTE : KBM_MODE_CONSOLE))

#define is_user_kbmode(x) \
        (((x) == KBM_MODE_LOCAL) || ((x) == KBM_MODE_REMOTE))

#define is_console_kbmode(x) ((x) == KBM_MODE_CONSOLE)

#ifdef __cplusplus
extern "C" {
#endif
    extern int keycode_from_name(char *name);
    extern int keycode_match(unsigned char *buf, 
                             int len, 
                             int kbtype, 
                             int *is_break); 
    extern int get_keyboard_escape(unsigned char *keyboard_escape, 
                                   int esc[], 
                                   int kbtype);
    extern int get_break_code(int keycode, int kbtype, unsigned char *brkcode);
    extern int fill_kb_buffer(unsigned char *buff, 
                              int maxbufflen,
                              struct input_event *evbuf,
                              int evbuflen, 
                              int kbtype,
                              int autorepeat);
    extern int fill_ms_buffer(unsigned char *buff, 
                              int maxbufflen,
                              struct input_event *evbuf,
                              int evbuflen, 
                              int mstype,
                              int msmode,
                              int *newmode);
    extern int xlate_set3_to_rfb(unsigned char *kbbuf,
                                 unsigned char *rfbbuf,
                                 unsigned char *numlock,
                                 unsigned char *capslock,
                                 unsigned char *shift,
                                 int kbbuflen);
    extern int xlate_set2_to_rfb(unsigned char *kbbuf,
                                 unsigned char *rfbbuf,
                                 unsigned char *numlock,
                                 unsigned char *capslock,
                                 unsigned char *shift,
                                 int kbbuflen);
    extern int get_set2_scancode(unsigned char *kbbuf,
                                 unsigned char *rfbbuf,
                                 unsigned char *numlock,
                                 unsigned char *capslock,
                                 unsigned char *shift,
                                 int kbbuflen,
                                 unsigned int *scancode,
                                 unsigned char *keydown);
    extern int write_to_kb_fpga(HAL *hp, 
                                unsigned char *buf, 
                                int buflen);
    extern int write_to_ms_fpga(HAL *hp, 
                                unsigned char *buf, 
                                int buflen, 
                                int mouse_type);
    extern void set_serial_parameters(int fd,
                                      int baud,
                                      int bits,
                                      int stops,
                                      unsigned char parity,
                                      unsigned char fctl);
#ifdef __cplusplus
}
#endif

#endif // __COMMONUTILS_H__
