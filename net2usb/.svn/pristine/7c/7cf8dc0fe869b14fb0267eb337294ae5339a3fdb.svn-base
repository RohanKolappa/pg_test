/*************************************************************************
 * Unless otherwise specified, Copyright (C) 2004 Barco Control Rooms
 * $Id: ppc2usb.c,v 1.1 2005/01/06 15:41:26 kboyadzh Exp $
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common.h"

#include "vnc2usb.h"

#include "vncrfb.h"
#include "vnckeysym.h"
#include <linux/hidraw.h>
#include "dev-hidraw.h"


#define DEFAULTXSIZE 800
#define DEFAULTYSIZE 600

#define MAX_X_IN_USB_REPORT 0x7FFF // must be the same as value defined in USB HID report
#define MAX_Y_IN_USB_REPORT 0x7FFF

#define SPY_CODES 0


static volatile uint32_t KeyboardLEDs;

static char const * const devname[] = { "/dev/hidg0", "/dev/hidg1", "/dev/hidg2" };
#define NUM_GADGET_DEV (sizeof(devname)/sizeof(*devname))
#define DEV_KEYB 0
#define DEV_MOUS 1
#define DEV_TABLET 2

static int openGadgetDev(uint32_t i);

#define NUMDEV NUM_GADGET_DEV
#define KEYBOARD_FIFO "/tmp/keyboardfifofd"
#define MOUSE_FIFO "/tmp/mouseabsfd"

static int fd_otg[NUM_GADGET_DEV];

static rfbScreenInfo *server;
int m_nKeyboardfd;
int m_nMousefd;


// command line options
bool relativevnc;
bool mouse;
bool reserve_dev_hidraw;


static int maxx = DEFAULTXSIZE, maxy = DEFAULTYSIZE, bpp = 1; //1- 8 bits per pixel
/* odd maxx doesn't work (vncviewer bug) */

static void init_threads_attributes(bool begin);
pthread_attr_t DfltThreadAttr; // used as default attribute for all new threads
static void vnc2usb_restartmessage(void);
static void * thread_poll_LEDs(void *dummy);
static void write_keyb_LED(int fdev, uint32_t val);


static void * thread_poll_new_dev_hidraw(void *prm);
static void * thread_read_dev_hidraw(void *prm);


static void on_vnc_mouse_event (int buttonMask, int XvncAbsol, int YvncAbsol, rfbClientPtr cl);
static void on_vnc_key_press (rfbBool down, rfbKeySym key, rfbClientPtr cl);

// http://www.linuxjournal.com/article/6429
//

/**#DEFINEs *******************************************************************************************/
#define NUM_RECORDS_IN_MOUS_FIFO 16
#define NUM_RECORDS_IN_KEYB_FIFO 8
 /* 2 words = 8 bytes, a record length of an USB Keyb/Mouse report */
#define RECORD_LENGTH 2

/* the record was filled in with new data from Source*/
#define NEW_DATA_AVAIL BIT0

/* from HID USB spec, 8.3 p.56. This is byte 0 in an USB keyb report sent to USB Host, but shifted << 8*/
#define USB_LEFT_CTRL BIT8
#define USB_LEFT_SHIFT BIT9
#define USB_LEFT_ALT BIT10
#define USB_LEFT_GUI BIT11
#define USB_RIGHT_CTRL BIT12
#define USB_RIGHT_SHIFT BIT13
#define USB_RIGHT_ALT BIT14
#define USB_RIGHT_GUI BIT15

#define NO_NUMLOCK_REQ BIT16
#define NUMLOCK_REQ BIT17
#define NO_SHIFT_REQ BIT18
#define NO_ALT_REQ BIT19

#define USB_CAPSLOCK_CODE 0x39
#define USB_NUMLOCK_CODE 0x53
/* wait 20msec after sending of NumLock from USB gadget interface*/
#define NUMLOCK_TOGGLE_PAUSE 20


/* global variables ***********************************************************************************/
struct mFIFO{
    volatile uint32_t BufHead;  /* the position where the next payload in FIFO will be written */
    volatile uint32_t BufTail;  /* the next payload record in FIFO which will be fetched by ReadFIFO */
    uint32_t MaxFifoSize;   /* the number of records in a FIFO, 3..32 */
    uint32_t MaxFifoRecSize;    /* keeps the selected Record size, up to MAX_FIFO_RECORD_SIZE */
    volatile uint32_t *pFIFOBuf;
    volatile uint32_t *pRecSize;    /* a pointer to an array where the length of the data in every Record is stored */
    volatile uint32_t *pBufContent; /* a pointer to an array of uint32_t, if 1 - the record is still not read, 0 - empty */
    volatile uint32_t ReturnValue;
    volatile uint32_t wi;
    volatile uint32_t RetVal;
    volatile uint32_t i;
    MUTEX(FIFObusy);
};

struct mFIFO *pMousFIFO, *pKeybFIFO, *pTabletFIFO;



/** function declarations *****************************************************************************/

static uint32_t InitFIFO(uint32_t FIFOSize, uint32_t RecSize,struct mFIFO * p);
static uint32_t WriteFIFO(uint32_t NumWords, uint32_t * pSrcBuf,struct mFIFO * p);
static uint32_t ReadFIFO(uint32_t * NumWordsInRec, uint32_t * pDestBuf,struct mFIFO * p);
static void ClearFIFO(struct mFIFO * p);
#if 0 // netviz2 - currently not used, kill with Ctrl+C
static void RemoveFIFO(struct mFIFO * p);
#endif

static uint32_t usbKeybWrite(rfbBool down, uint32_t VNCkey, rfbClientPtr cl);

/* handles incoming VNC mouse events */
static void on_vnc_mouse_event(int32_t buttonMask, int32_t XvncAbsol, int32_t YvncAbsol, rfbClientPtr cl);
/* handles VNC keyboard events */
static void on_vnc_key_press(rfbBool down, rfbKeySym key, rfbClientPtr cl);


/* *INDENT-OFF* */
/* BIT8 = 1 - emulate Ctrl at USB  */
/* BIT9 = 1 - emulate Shift at USB */
/* BIT10 = 1 - Left Alt at USB ... */
/* ...                             */
/* BIT14 = 1 - Right Alt at USB ...*/

/* BIT16 = 1 - NUM_LOCK must NOT be active   (= NUMLOCK led)       BIT16 and 17 = 0 - the key doesn`t care for NUMLOCK status*/
/* BIT17 = 1 - NUM LOCK must be activated at USB Host*/
/* BIT18 = 1 - NO shift (L/R) in report -- AZERTY*/
/* BIT19 = 1 - NO alt in report -- AZERTY */
/* used information from page  http://www.usna.edu/LangStudy/US-InternationalLayout.html */
// http://www.fileformat.info/info/unicode/char/00b9/index.htm

#if 1 // no more fake Shift or fake Alt. Remote Pointer must send the Shift or Alt codes

static const uint32_t Encode00xx[256] = {  /* used for convertion ASCII -> USB usage codes*/
/*           0       1       2       3       4       5       6       7        8       9       A       B       C       D       E       F*/
/* 0*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*                                                                                                                                     */
/* 1*/  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*      space      s+1!   s+'"    s+3#    s+4$    s+5%    s+7&       '     s+9(    s+0)    s+8*    s+=+       ,       -       .       /*/
/* 2*/  0x8002C, 0x021E, 0x0234, 0x80220, 0x0221, 0x0222, 0x0224, 0x0034,  0x0226, 0x0227, 0x0225, 0x022E, 0x0036, 0x002D, 0x40037, 0x40038,
/*           0       1       2       3       4       5       6       7        8       9    s+;:       ;    s+,<       =    s+.>    s+/?*/
/* 3*/  0x40027, 0x4001E, 0x4001F, 0x40020, 0x40021, 0x40022, 0x40023, 0x40024,  0x40025, 0x40026, 0x0233, 0x0033, 0x0236, 0x002E, 0x0237, 0x0238,
/*        s+1@     s+a,    s+b,    s+c,    s+d,    s+e,    s+f,    s+g,     s+h,    s+i,    s+j,    s+k,    s+l,    s+m,    s+n,    s+o,*/
/* 4*/  0x8021F, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020A,  0x020B, 0x020C, 0x020D, 0x020E, 0x020F, 0x0210, 0x0211, 0x0212,
/*         s+p     s+q,    s+r,    s+s,    s+t,    s+u,    s+v,    s+w,     s+x,    s+y,    s+z       [       \       ]    s+6^    s+-_*/
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

#else
// removed shift + key, now shift is received before the key
static const uint32_t Encode00xx[256] = {  /* used for convertion ASCII -> USB usage codes*/
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
#endif


static const uint32_t EncodeFFxx[256] = {  /* used for convertion XK_ ...  -> USB usage codes*/
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


/********************************************************************************************************/
/* this function converts VNC keycodes coming from network and stores them as USB codes in keybFIFO     */
/* all VNC-to-USB codes conversion is done here. If another protocol must be used through Ethernet then */
/* this function must be changed only                                                                   */
static uint32_t usbKeybWrite(rfbBool down, uint32_t VNCkey, rfbClientPtr cl)
{
    uint32_t ModifKeys = 0, Found, Value, i;
    static uint32_t ModifKeysLatched;
    static uint32_t repBuf[2];
    static uint32_t LeftCtrlHold, LeftShftHold, LeftAltHold, LeftGUIHold;
    static uint32_t RightCtrlHold, RightShftHold, RightAltHold, RightGUIHold;
    static uint8_t but[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t newbut;
	uint8_t noshift = 0;
	uint8_t noalt = 0;
	uint32_t EmulateModifPress = 0;
    static uint16_t VNCpr[16];
	VNCkey &= 0x0000FFFF;


    /* if CAPS LOCK is lit at Slave, try to clear it */
    if(down && (KeyboardLEDs & KEYB_CAPS_LOCK)) {
        repBuf[0] = (KEYB_TOKEN | (USB_CAPSLOCK_CODE << 16) | ModifKeysLatched);
        repBuf[1] = 0;
        repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
        if (WriteFIFO(RECORD_LENGTH, repBuf, pKeybFIFO))
            return 1;
    }

    Found = 0;
    Value = 0;
	switch (VNCkey & 0xFF00) {
    case (0x0000):  /* The state of NumLock is not relevant here */
        Value = Encode00xx[VNCkey];
        Found = 1;
		// next, check whether R_Shift is not already pressed, and in this case ignore the L_Shift from Encode00xx[] array
        if((ModifKeysLatched) && (Value & 0xFF00))
        { // an Alt or Shift key must be pressed by SCN
            if(LeftCtrlHold && (Value & USB_RIGHT_CTRL))
                Value &= ~USB_RIGHT_CTRL;
            else if(RightCtrlHold && (Value & USB_LEFT_CTRL))
                Value &= ~USB_LEFT_CTRL;
            if(LeftShftHold && (Value & USB_RIGHT_SHIFT))
                Value &= ~USB_RIGHT_SHIFT;
            else if(RightShftHold && (Value & USB_LEFT_SHIFT))
                Value &= ~USB_LEFT_SHIFT;
           if(LeftAltHold && (Value & USB_RIGHT_ALT))
                Value &= ~USB_RIGHT_ALT;
            else if(RightAltHold && (Value & USB_LEFT_ALT))
                Value &= ~USB_LEFT_ALT;
           if(LeftGUIHold && (Value & USB_RIGHT_GUI))
                Value &= ~USB_RIGHT_GUI;
            else if(RightGUIHold && (Value & USB_LEFT_GUI))
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
	   break;

   case (0xFF00):
		ModifKeys = EncodeFFxx[VNCkey & 0x00FF] & ~0xFF;    /* only bits 31..8 are interesting */
        Value = EncodeFFxx[VNCkey & 0x00FF];    /* all bits */
        Found = 1;
        if(ModifKeys && !(Value & 0x000F00FF)) {   /* this case is to handle specificly Ctrl, Alt, Shift, GUI keys pressed alone. *//* These keys are implemented as Sticky keys, since VNC protocol does not use separated codes */
            static uint32_t PrevModifKeysLatched;   /* for Alt+x, Ctrl+s etc. bijv. VNC client sends first CTRL code, next 's' code to server */

            switch (ModifKeys) {    /* The state of NumLock is not relevant here */
            case (USB_LEFT_CTRL):   /* XK_Control_L */
                LeftCtrlHold = (down) ? USB_LEFT_CTRL : 0;
                break;
            case (USB_LEFT_SHIFT):  /* XK_Shift_L arrived from client */
                LeftShftHold = (down) ? USB_LEFT_SHIFT : 0;
                break;
            case (USB_LEFT_ALT):    /* XK_Alt_L */
                LeftAltHold = (down) ? USB_LEFT_ALT : 0;
                break;
            case (USB_LEFT_GUI):    /* XK_Hyper_L or XK_Super_L Windows key */
                LeftGUIHold = (down) ? USB_LEFT_GUI : 0;
                break;
            case (USB_RIGHT_CTRL):  /* XK_Control_R */
                RightCtrlHold = (down) ? USB_RIGHT_CTRL : 0;
                break;
            case (USB_RIGHT_SHIFT): /* XK_Shift_R */
                RightShftHold = (down) ? USB_RIGHT_SHIFT : 0;
                break;
            case (USB_RIGHT_ALT):   /* XK_Alt_R */
                RightAltHold = (down) ? USB_RIGHT_ALT : 0;
                break;
            case (USB_RIGHT_GUI):   /* XK_Hyper_R or XK_Super_R Windows key */
                RightGUIHold = (down) ? USB_RIGHT_GUI : 0;
                break;
            default:
                Found = 0;
                break;
            }
            ModifKeysLatched = (LeftCtrlHold | LeftShftHold | LeftAltHold | LeftGUIHold | RightCtrlHold | RightShftHold | RightAltHold | RightGUIHold);
            /* do not repeat pressed Shift/Ctrl/Alt/GUI key, otherwise Slave is going nuts */
            if (ModifKeysLatched == PrevModifKeysLatched)
                Found = 0;
            PrevModifKeysLatched = ModifKeysLatched;
        } else {    /* handle here only the cases where NumLock is required/banned */
            if((ModifKeys & 0xF0000) && (Value & 0xFF) && down) {  /* All this fun below was added because NumLock does not exist for VNC protocol. */
                static uint32_t tempKey[2];
                switch (ModifKeys) {
                case (NO_NUMLOCK_REQ):  /* NumLock LED must be off the VNC key to be interpretted correctly by Slave */
                    if (KeyboardLEDs & KEYB_NUM_LOCK) {  /* try to turn off NumLock at Slave */
                        tempKey[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16); /* send NumLock keystroke to USB itf */
                        tempKey[1] = 0;
                        tempKey[0] = (tempKey[0] & ~0xFFFF) | ((tempKey[0] >> 8) & 0xFF);
                        if (WriteFIFO(RECORD_LENGTH, tempKey, pKeybFIFO))
                            return 1;
                        usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
                        /*rfbLog(" NO_NUM_REQ "); */
                    }/*else the NUMLOCK is OFF already */
                    break;
				case (NUMLOCK_REQ): /* NumLock LED must be on */
					if(!(KeyboardLEDs & KEYB_NUM_LOCK)) {
						tempKey[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16);
						tempKey[1] = 0;
						tempKey[0] = (tempKey[0] & ~0xFFFF) | ((tempKey[0] >> 8) & 0xFF);
						if (WriteFIFO(RECORD_LENGTH, tempKey, pKeybFIFO))
							return 1;
						usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
					}/*else the NUMLOCK is ON already */
					break;
						/*add here next special cases */
				default:
                    Found = 0;
                    break;
        }   }   }
		/* the Keycode is still Value = EncodeFFxx[VNCkey & 0x00FF] and Found = 1; */
        break;
    default:
        Found = 0;
        Value = 0;
        break;
    }

    if (!Value)
        rfbLog("\r\n--Undefined VNCkey=%04X--\r\n", VNCkey);
    if (!Found)
        return 0;   /* sorry, still not supported code, or repeated Special Key (Shift, Ctrl...) */

	newbut = Value & 0xFF;


    for (i = 2; i < 8; i++) {
        if((but[i] == 0) || (but[i] == newbut)) {
            if(down) {
                but[i] = newbut;    /* the place is free, or contains the same butt value, write the new button */
                break;
            } else {    /* a button was released, scan but array and clean it     */
                while (i < 7){ /* move all records left since a button was released */
                    but[i] = but[i + 1];
                    i++;
                }
                but[i] = 0;
                break;
    }   }   }

    // this is to handle shift or AltGr states. 'A' can come as 'SHIFT down', 'A down' (Windows VNC client) or 'A' only (JAVA client)

    /* add alt,shift, ctrl... to ordinary keys, bits 15..8 of values from table Encode00xx */
    if( ((VNCkey & 0xFF00) == 0) && (EmulateModifPress = Value & 0xFF00)) {
        if(down == false)
            EmulateModifPress = 0;

        //printf("ModifKeysLatched=%04X, EmulateModifPress=%04X, down=%u\n", ModifKeysLatched, EmulateModifPress,((down)? 1:0));
        repBuf[0] = (((uint32_t) but[3] << 24) | ((uint32_t) but[2] << 16) | ( ModifKeysLatched | EmulateModifPress) | KEYB_TOKEN);

    } else {
        repBuf[0] = (((uint32_t) but[3] << 24) | ((uint32_t) but[2] << 16) | ModifKeysLatched | KEYB_TOKEN);
    }

    repBuf[1] = (((uint32_t) but[7] << 24) | ((uint32_t) but[6] << 16) | ((uint32_t) but[5] << 8) | ((uint32_t) but[4] << 0));

    // a check for spurious release code from RemotePointer applet. When a release code arrives, but press code
    // was not issued before, send 'release all' command to USB keyb module.
    // more than one press code is acceptable.
    Found = 0;
    for( i = 0; i < 15; i++)   // keep the last 14 pressed but not released 14 codes . code 15 is always 0
        if(down){
            if((VNCpr[i] == 0) || (VNCpr[i] == VNCkey)) { // ok, new pressed VNC key or repeated press key
                VNCpr[i] = VNCkey;
                Found = 1;
                break;
            }
        } else {    // a release code arrived, check for available press code
            if(VNCpr[i] == VNCkey) {
                uint16_t j;
                VNCpr[15] = 0;
                for(j = i; j < 15; j++) // free the position, move all records left
                    VNCpr[j] = VNCpr[j + 1];
                Found = 1;
                break;
        }   }

    if(!Found) {
        for( i = 0; i < 15; i++)
            VNCpr[i] = 0;
       #ifdef SPY_CODES
        printf("Spurious release code %04X, releasing pressed keys.\n", VNCkey);
       #endif
    }

    if((cl->IsClientRinning == false) || !Found) { // the client has just disappeared, but the clients have a bad habit to leave keys or mouse buttons pressed ....
        repBuf[0] = KEYB_TOKEN;
        repBuf[1] = 0;
        ModifKeysLatched = 0;
        LeftCtrlHold = LeftShftHold = LeftAltHold = LeftGUIHold = 0;
        RightCtrlHold = RightShftHold = RightAltHold = RightGUIHold = 0;
        for (i = 0; i < 8; but[i++] = 0);
    }

    // Netviz2: move bits 15..8 to 7..0, and 15..8 are 00 now
    uint32_t cc = repBuf[0] >> 8  & 0xFF;
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


#ifdef SPY_CODES
  printf("rep= [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]\n",
												   (repBuf[0] >> 0)  & 0xFF,
												   (repBuf[0] >> 8)  & 0xFF,
												   (repBuf[0] >> 16) & 0xFF,
                                                   (repBuf[0] >> 24) & 0xFF,
                                                   (repBuf[1] >> 0)  & 0xFF,
                                                   (repBuf[1] >> 8)  & 0xFF,
                                                   (repBuf[1] >> 16) & 0xFF,
                                                   (repBuf[1] >> 24) & 0xFF);
 #endif
    /*rfbLog(" %02X %02X %02X %02X %02X %02X\r\n", but[2], but[3], but[4], but[5], but[6], but[7]); */
    if (WriteFIFO(RECORD_LENGTH, repBuf, pKeybFIFO))
        return 1;
    return 0;
}


/******************************************************************************************************/
static int TransferFIFO(int num)
{
    static uint32_t repBuf[2];
    char *buf = (char*)repBuf;
    uint32_t z;
    int writtenBytes, buflen;
    static uint16_t Written[NUM_GADGET_DEV];

    buflen = 0;

    if (Written[num]) /* a keyb or mouse report was written to /dev/hidgXX */
        usleep(1000);
    Written[num] = 0;

    switch(num) {
        case DEV_KEYB:
            if (!(ReadFIFO(&z, repBuf, pKeybFIFO))) {
                rfbLog("DEV_KEYB %08X %08X\n", repBuf[0], repBuf[1]);
                buflen = 8;
            }
            break;
        case DEV_MOUS:
            if (!(ReadFIFO(&z, repBuf, pMousFIFO))) {
                //rfbLog("DEV_MOUS %08X %08X", repBuf[0], repBuf[1]);
                buflen = 4;
            }
            break;
        case DEV_TABLET:
            if (!(ReadFIFO(&z, repBuf, pTabletFIFO))) {
                rfbLog("DEV_TABLET %08X %08X", repBuf[0], repBuf[1]);
                buflen = 6;
            }
            break;
        default:
            break;
    }

    if(buflen) {
        //int j; char bufZ[256], *p = bufZ; for(j = 0; j < buflen; j++) p += sprintf(p, " %02X", *(buf + j)); printf("Report%s\n", bufZ);
        if(fd_otg[num]){
            bool err;
            do{
                err = false;
                writtenBytes = write(fd_otg[num], buf, buflen); // block here if /dev/hidXX is busy, not writable
                if((-1 == writtenBytes) && (errno == EINTR)) {
                    printf("ERR: cannot write to \'%s\', retry in 2 sec...\n", devname[num]); //probably USB cable is disconnected
                    err = true;
                    sleep(2);
                } else if((writtenBytes >= 0) && (writtenBytes != buflen)) { //weird, kernel error, check HID device definition in kernel, report size
                    printf("-%u- could not write %u bytes to \'%s\', only %u ?! Pause 1 sec ...\n", __LINE__, buflen, devname[num], writtenBytes);
                    perror(devname[num]);
                    sleep(1);
                } else
                    Written[num] = 1; //printf("writtenOK\n");

            }while(true == err);
    }    }
    return Written[num];
}

/******************************************************************************************************/
/* Create a US International keyboard sequence for special keys starting from a VNC code
   Source: http://www.forlang.wsu.edu/help/keyboards1.asp
*/
void createUsInternationalKeySequence(uint32_t key, int down, rfbClientPtr cl)
{
	switch (key)
	{
		case XK_sterling:	// £
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_dollar, cl);
			break;

		case XK_mu:	// µ
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_m, cl);
			break;

		case XK_degree: // °
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_colon, cl);
			break;

		case XK_section: // §
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_Shift_L, cl);
			usbKeybWrite(down, XK_s, cl);
			break;

		case XK_eacute:	// É
		case XK_Eacute:
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_e, cl);
			break;

		case XK_agrave: // À
		case XK_Agrave:
			if (down)
			{
				usbKeybWrite(1, XK_grave, cl);
				usbKeybWrite(0, XK_grave, cl);
			}
			usbKeybWrite(down, XK_a, cl);
			break;

		case XK_igrave: // Ì
		case XK_Igrave:
			if (down)
			{
				usbKeybWrite(1, XK_grave, cl);
				usbKeybWrite(0, XK_grave, cl);
			}
			usbKeybWrite(down, XK_i, cl);
			break;

		case XK_ograve: // Ò
		case XK_Ograve:
			if (down)
			{
				usbKeybWrite(1, XK_grave, cl);
				usbKeybWrite(0, XK_grave, cl);
			}
			usbKeybWrite(down, XK_o, cl);
			break;

		case XK_egrave:	// È
		case XK_Egrave:
			if (down)
			{
				usbKeybWrite(1, XK_grave, cl);
				usbKeybWrite(0, XK_grave, cl);
			}
			usbKeybWrite(down, XK_e, cl);
			break;

		case XK_ugrave:	//ù
		case XK_Ugrave:
			if (down)
			{
				usbKeybWrite(1, XK_grave, cl);
				usbKeybWrite(0, XK_grave, cl);
			}
			usbKeybWrite(down, XK_u, cl);
			break;


		case XK_ccedilla: // Ç
		case XK_Ccedilla:
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_comma, cl);
			break;

		case XK_asciicircum: //  ^
			if (down)
			{
				usbKeybWrite(1, XK_asciicircum, cl);
				usbKeybWrite(0, XK_asciicircum, cl);
			}
			usbKeybWrite(down, XK_space, cl);
			break;

		case XK_asciitilde: // ~
			if (down)
			{
				usbKeybWrite(1, XK_asciitilde, cl);
				usbKeybWrite(0, XK_asciitilde, cl);
			}
			usbKeybWrite(down, XK_space, cl);
			break;

		case XK_copyright:  // ©
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_c, cl);
			break;

		case XK_ntilde:		// ñ
		case XK_Ntilde:
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_n, cl);
			break;

		case XK_atilde:		// ã
		case XK_Atilde:
			if (down)
			{
				usbKeybWrite(1, XK_asciitilde, cl);
				usbKeybWrite(0, XK_asciitilde, cl);
			}
			usbKeybWrite(down, XK_a, cl);
			break;

		case XK_otilde:		// õ
		case XK_Otilde:
			if (down)
			{
				usbKeybWrite(1, XK_asciitilde, cl);
				usbKeybWrite(0, XK_asciitilde, cl);
			}
			usbKeybWrite(down, XK_o, cl);
			break;

		case XK_diaeresis: // ¨
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_quotedbl, cl);
			break;

		case XK_Adiaeresis:		// Ä
		case XK_adiaeresis:
			if (down)
			{
				usbKeybWrite(1, XK_quotedbl, cl);
				usbKeybWrite(0, XK_quotedbl, cl);
			}
			usbKeybWrite(down, XK_a, cl);
			break;

		case XK_Ediaeresis:		// Ë
		case XK_ediaeresis:
			if (down)
			{
				usbKeybWrite(1, XK_quotedbl, cl);
				usbKeybWrite(0, XK_quotedbl, cl);
			}
			usbKeybWrite(down, XK_e, cl);
			break;

		case XK_Idiaeresis:		// ï
		case XK_idiaeresis:
			if (down)
			{
				usbKeybWrite(1, XK_quotedbl, cl);
				usbKeybWrite(0, XK_quotedbl, cl);
			}
			usbKeybWrite(down, XK_i, cl);
			break;

		case XK_Odiaeresis:		// ö
		case XK_odiaeresis:
			if (down)
			{
				usbKeybWrite(1, XK_quotedbl, cl);
				usbKeybWrite(0, XK_quotedbl, cl);
			}
			usbKeybWrite(down, XK_o, cl);
			break;

		case XK_Udiaeresis:		// Ü
		case XK_udiaeresis:
			if (down)
			{
				usbKeybWrite(1, XK_quotedbl, cl);
				usbKeybWrite(0, XK_quotedbl, cl);
			}
			usbKeybWrite(down, XK_u, cl);
			break;

		case XK_AE:	 // Æ
		case XK_ae:
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_z, cl);
			break;

		case XK_ETH:    // Ð
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_d, cl);
			break;

		case XK_oslash:	// Ø
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_l, cl);
			break;

		case XK_twosuperior: //	²
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_2, cl);
			break;

		case XK_threesuperior: //	³
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_3, cl);
			break;

		case XK_bar: //	¦
			usbKeybWrite(down, XK_Alt_R, cl);
			usbKeybWrite(down, XK_bar, cl);
			break;

		default:
			usbKeybWrite(down, key, cl);
			break;
	}
}

static int OpenKbFiFo()
{
    int fifofile;

    m_nKeyboardfd = open(KEYBOARD_FIFO,  O_RDWR);

    if (m_nKeyboardfd < 1) {
        return 0 ;
    }

   return 1;
}

static int OpenMouseFiFo()
{

    m_nMousefd =  open(MOUSE_FIFO, O_RDWR);
    if (m_nMousefd < 0) {
       return 0;
    }

   
   return 1;

}

/******************************************************************************************************/
// this function is invoked from libvnc
static void on_vnc_key_press(rfbBool down, rfbKeySym key, rfbClientPtr cl)
{
    unsigned int keybuf[2];
    int rc;
    int j;
    unsigned  int downkey = down;

    if (m_nKeyboardfd <= 0)
        return;
#if 0
    keybuf[0] = key & 0xff;
    keybuf[1] = (key >> 8) & 0xff;
    keybuf[2] = (key >> 16) & 0xff;
    keybuf[3] =( key >> 24) & 0xff;
#endif
    keybuf[0] = key;
    keybuf[1] = down;

    rc =  write(m_nKeyboardfd, &keybuf, 2 * sizeof(int)); 

    return;
       
#ifdef SPY_CODES
    printf("-----------------------\n");
	printf("-%u- %s(down=%u key=%04X)\n", __LINE__, __FUNCTION__, down, key);
    {   
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
        if(down)
            printf("%s (0x%04X)", nm, key);
        else
            printf("."); //printf("       --up-- %s (0x%04X)\n",  nm, key);
    }
#endif

//	uint32_t keyboardSequence[MAX_US_INT_KEYS];
//	int keyCnt, nrOfKeys = 0;
	/* Filter out special VNC key codes: we emulate a US-international keyboard.
	   Some characters can only be created by simulating a sequence of key presses.
	   e.g. Ü = <Right Alt> + Y
	*/

//	createUsInternationalKeySequence(key, down, cl);
   
//	return;
}


/******************************************************************************************************/
// TODO
static void on_vnc_mouse_event (int buttonMask, int XvncAbsol, int YvncAbsol, rfbClientPtr cl)
{
    static uint32_t  repBuf[2];
    int tmp;
    int8_t xrel, yrel, wheel;
    uint8_t buttonbyte;

    
    buttonbyte = buttonMask & BIT0;
    if(buttonMask & BIT1) buttonbyte |= BIT2;
    if(buttonMask & BIT2) buttonbyte |= BIT1;
    wheel = 0;
    if(buttonMask & BIT4) wheel = -1;
    if(buttonMask & BIT3) wheel = 1;

    if(false == relativevnc){   // XvncAbsol, XvncAbsol are absolute
        static int32_t prevx = 0, prevy = 0, just_connected = 1;

         // first mouse report arrived from Master
        if(just_connected){
            just_connected = 0;
            prevx = XvncAbsol;
            prevy = YvncAbsol;
            xrel = 0;
            yrel = 0;
            return;
        }

        tmp = XvncAbsol - prevx;
        if(tmp < -128)
            tmp = -128;
        else if (tmp > 127)
            tmp = 127;
        xrel= tmp;

        tmp = YvncAbsol - prevy;
        if(tmp < -128)
            tmp = -128;
        else if (tmp > 127)
            tmp = 127;
        yrel= tmp;

        // keep the previous absolute coordinates
        prevx = XvncAbsol;
        prevy = YvncAbsol;

    } else {    // VNC viewer sends relative X/Y
        tmp = XvncAbsol;
        if(tmp < -128)
            tmp = -128;
        else if (tmp > 127)
            tmp = 127;
        xrel = tmp;

        tmp = YvncAbsol;
        if(tmp < -128)
            tmp = -128;
        else if (tmp > 127)
            tmp = 127;
        yrel = tmp;
    }

    repBuf[0] = (((uint32_t)wheel) << 24) | ((((uint32_t)yrel) & 0xFF) << 16) | ((((uint32_t)xrel) & 0xFF) << 8) | (buttonbyte & 0xFF);
    repBuf[1] = 0;

    if (WriteFIFO(RECORD_LENGTH, repBuf, pMousFIFO)){
        ClearFIFO(pMousFIFO);
        WriteFIFO(RECORD_LENGTH, repBuf, pMousFIFO);
    }
    //printf("butt:%i  x:%d(%d)  y:%d(%d)\n", buttonMask, XvncAbsol, xrel, YvncAbsol, yrel);
    return;
}


/******************************************************************************************************/
    // Absolute coordinates to /dev/hidg2, 6 bytes HID report
//  XvncAbsol must come from VNCclient in range 0..(DEFAULTXSIZE-1), else the VNCclient is buggy
static void on_vnc_tablet_event (int buttonMask, int XvncAbsol, int YvncAbsol, rfbClientPtr cl)
{
    static uint32_t  repBuf[2];
    uint32_t buttonbyte, x, y;
    int8_t wheel;

    int buffer[5];
    int rc;

  //  printf( "b= %x xvnc = %x yvnc = %x cwidth = %x cheight = %x\n",
    //          buttonMask, XvncAbsol, YvncAbsol, cl->width,cl->height);

    if (m_nMousefd < 0)
        return ;

    buffer[0] = buttonMask;
    buffer[1] = XvncAbsol;
    buffer[2] = YvncAbsol;
    buffer[3] = cl->width;
    buffer[4] = cl->height;

    rc = write(m_nMousefd, &buffer, 5 * sizeof(int));
   // if (rc != (5 * sizeof(int)))
      //  printf( " Error in Abs mouse data to mouse FIFO \n");

    return;



    if(FALSE == cl->IsClientNetviz){
        if((XvncAbsol == MAX_X_IN_USB_REPORT) && (YvncAbsol == MAX_Y_IN_USB_REPORT)) {
            cl->width = MAX_X_IN_USB_REPORT + 1; // 0x8000
            cl->height = MAX_Y_IN_USB_REPORT + 1;
            cl->IsClientNetviz = TRUE;
            return;
        }
    }

    if((XvncAbsol < 0) || ((XvncAbsol > 0) && (XvncAbsol >= cl->width))){
        printf("ERR bad VNCviewver: got x=%d, expected values 0..%u !\n", XvncAbsol, cl->width - 1);
        return;
    }
    if((YvncAbsol < 0) || ((YvncAbsol > 0) && (YvncAbsol >= cl->height))){
        printf("ERR bad VNCviewver: got y=%d, expected values 0..%u !\n", YvncAbsol, cl->height - 1);
        return;
    }

    buttonbyte = buttonMask & BIT0;
    if(buttonMask & BIT1) buttonbyte |= BIT2;
    if(buttonMask & BIT2) buttonbyte |= BIT1;

    wheel = 0;
    if(buttonMask & BIT4) wheel = -1;
    if(buttonMask & BIT3) wheel = 1;

    if(FALSE == cl->IsClientNetviz){
		x = (XvncAbsol * (MAX_X_IN_USB_REPORT + 1)) / cl->width;
		y = (YvncAbsol * (MAX_Y_IN_USB_REPORT + 1)) / cl->height;
		/*printf("\t\txxx xVNC=%d yVNC=%d x=%u y=%u cl->width=%d cl->height=%d\n",XvncAbsol,YvncAbsol, x, y, cl->width, cl->height)	;*/

    } else {
        x = XvncAbsol;
        y = YvncAbsol;
    }

    repBuf[0] = (buttonbyte & 0xFF) | // Buttons to bits 7..0,  byte 1 HID report
                ((x & 0xFF) << 8)   | // X LSB to bits 15..0,   byte 2
                ((x & 0xFF00) << 8) | // X MSB to bits 23..16,  byte 3
                ((y & 0xFF) << 24)  ; // Y LSB to bits 31..24,  byte 4

    repBuf[1] = ((y >> 8) & 0xFF)   |   // Y MSB to bits 7..0,  byte 5
                ((uint32_t)wheel << 8); // Wheel to bits 15..8  byte 6

    if (WriteFIFO(RECORD_LENGTH, repBuf, pTabletFIFO)){
        ClearFIFO(pTabletFIFO);
        WriteFIFO(RECORD_LENGTH, repBuf, pTabletFIFO);
        //printf("MouseOverflow!\n");
    }
    //printf("butt:%i  x:%d(%u)  y:%d(%u)\n", buttonMask, XvncAbsol, x, YvncAbsol, y);
    return;
}

static void GetIPaddress()
{
    char buf[255];
    struct ifaddrs *ipadd; 
    struct ifaddrs *ifa;
    memset(buf, 0, 255);

    getifaddrs(&ipadd);

    for (ifa = ipadd; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr->sa_family == AF_INET) {
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
                    buf, INET_ADDRSTRLEN);
      } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,buf,INET6_ADDRSTRLEN);
      }
    }

    strcat(buf, " (");
    strcat (buf, buf);
    strcat(buf, ")");
    strcat(buf, " KBM Controler");

}

/******************************************************************************************************/
/******************************************************************************************************/
//                                              vnc2usb main
/******************************************************************************************************/
int main(int argc, char **argv)
{
    uint32_t i;
    char OurName[255];

    memset(OurName, 0, 255);
    gethostname(OurName, 255);
    strcat(OurName, " KBM Controler");

    //printf("\t\targc = %d\n", argc); while(fd < argc){ printf("\t\t-%u- \'%s\'\n", fd, argv[fd]); fd++;} fd = 0;

    if(getuid()){
        printf("ERR you are not root, /dev/hidXX devices cannot be opened.Abort.\n");
        exit(5);
    }

    if (!(pMousFIFO = (struct mFIFO *) malloc(sizeof(struct mFIFO))))
        exit(2);
    if (InitFIFO(NUM_RECORDS_IN_MOUS_FIFO, RECORD_LENGTH, pMousFIFO))
        exit(2);
    INIT_MUTEX(pMousFIFO->FIFObusy);

    if (!(pTabletFIFO = (struct mFIFO *) malloc(sizeof(struct mFIFO))))
        exit(2);
    if (InitFIFO(NUM_RECORDS_IN_MOUS_FIFO, RECORD_LENGTH, pTabletFIFO))
        exit(2);
    INIT_MUTEX(pTabletFIFO->FIFObusy);

    if (!(pKeybFIFO = (struct mFIFO *) malloc(sizeof(struct mFIFO))))
        exit(2);
    if (InitFIFO(NUM_RECORDS_IN_KEYB_FIFO, RECORD_LENGTH, pKeybFIFO))
        exit(2);
    INIT_MUTEX(pKeybFIFO->FIFObusy);

    init_pFIFOs(); // init FIFO pointers in dev-hidraw.c


   // if(openGadgetDev(DEV_KEYB) || openGadgetDev(DEV_TABLET) || openGadgetDev(DEV_MOUS))

    if (!OpenKbFiFo() || !OpenMouseFiFo())
        exit(5);

    atexit(vnc2usb_restartmessage);
    pthread_t thr;
    init_threads_attributes(true);
    // pthread_create(&thr, &DfltThreadAttr, thread_poll_LEDs, NULL); // poll keyboard LEDs to detect NumLock or CapsLock
    // usleep(10000);

    relativevnc = false;
    mouse = false;
    reserve_dev_hidraw = false;

    if (argc > 2) {
        maxx = atoi(argv[1]);
        maxy = atoi(argv[2]);
        printf(" resolution changed  %d %d\n", maxx, maxy);
    }

    if (maxx < 100 || maxy < 100 || maxx > 3200 || maxy > 3200)
    {
        maxx = 800;
        maxy = 600;  
    }

    // handle the command line parameters here, update some parameters
    if(!rfbProcessArguments(server, &argc, argv))
        exit(1);

    //TODO - handle hotplug in mdev, /etc/udev.conf
    if(true == reserve_dev_hidraw) {
        pthread_create(&thr, &DfltThreadAttr, thread_poll_new_dev_hidraw, NULL); // poll for just attached /dev/hidraw
        usleep(10000);
    }

           //rfbGetScreen(int* argc,char** argv, int width,int height,int bitsPerSample,int samplesPerPixel, int bytesPerPixel)
    server = rfbGetScreen (&argc, argv, maxx, maxy, 8, 3, bpp); // instantiate a VNC server, rfbScreenInfo structure
    server->desktopName =  OurName; //"SCN\'s VNC server";
	server->port = VNC_SERVER_PORT;
    server->alwaysShared = TRUE;
    server->kbdAddEvent = on_vnc_key_press;    // hooks (pointers) to our functions
    server->ptrAddEvent = (mouse)? on_vnc_mouse_event : on_vnc_tablet_event;
    server->httpPort = 0;
    server->httpDir ="."; // if server->httpDir = NULL then http server is disabled in LibVNC

    server->ToEnd = FALSE;

    rfbInitServer(server);

    rfbLog("Running on %s\r\n", server->thisHost);

     // start background listening threads in vncmain.c
    rfbRunEventLoop(server, -1, TRUE);

    // add 2 times on/off of NumLock key, to read current LED status
    uint32_t tempKey[2], k;
    for(k = 0; k < 4; k++){
        tempKey[0] = (!(k & BIT0))? (USB_NUMLOCK_CODE << 16) : 0;
        tempKey[1] = 0;
        WriteFIFO(RECORD_LENGTH, tempKey, pKeybFIFO);
    }


    // main endless polling cycle
    i = 0;
    while(server->ToEnd == FALSE){  //while(server->ListenerRuns == TRUE)

        usleep(5000); // no new data from network / in FIFO's, sleep for a while
    //    if(!TransferFIFO(DEV_TABLET) && !TransferFIFO(DEV_KEYB) && !TransferFIFO(DEV_MOUS)){
        //    usleep(5000); // no new data from network / in FIFO's, sleep for a while
      //  }
    }

    exit(0); // never reached, but keep a good style :-)
    usleep(500000);
    printf("\r\n end_ppc2usbmain:\r\n");fflush(stderr);
    // wait listenig thread to die
    // while(server->ListenerRuns == TRUE);
    rfbScreenCleanup(server);
    printf("\r\n The End\r\n");fflush(stderr);
    return 0;
}// main()


/******************************************************************************************************/
// check every XX sec for new HID devices - local or remote
static void * thread_poll_new_dev_hidraw(void *prm)
{
    int i;
    pthread_t thr;
    struct hidpipe * hid;

    while(1) {
        for(i = 0; i < HIDRAW_MAX_DEVICES; i++){

            hid = scan_dev_hidraw(i);
            if(hid){
                //printf("i=%u, Detected %s\n", i, hid->my_signat->name);
                pthread_create(&thr, &DfltThreadAttr, thread_read_dev_hidraw, (void *)hid);
                usleep(10000);
            }

        }
        sleep(5);
    }

    return NULL; // never reached
}



/******************************************************************************************************/
void rfbUsage(void) // Obsolete, TODO
{
    fprintf(stderr, "-relativevnc use Mouse HID USB interface, X/Y are in range +-127. Default is absolute data (Tablet mode)\n");
    fprintf(stderr, "-mouse use Mouse HID USB interface, but X is 0..%u, Y is 0..%u\n", maxx, maxy);
    return;
}

/******************************************************************************************************/
/* purges COUNT arguments from ARGV at POSITION and decrements ARGC.
   POSITION points to the first non purged argument afterwards. */
void rfbPurgeArguments(int *argc, int *position, int count, char *argv[])
{
    int amount = *argc - *position - count;
    if(amount)
        memmove(argv + *position, argv + *position + count, sizeof(char*) * amount);
    *argc -= count;
}

/******************************************************************************************************/
rfbBool rfbProcessArguments(rfbScreenInfoPtr rfbScreen, int *argc, char *argv[])
{
   #if 1
    int i,i1;
    if(!argc)
        return TRUE;
    for(i = i1 = 1; i < *argc; ) {
        if ((strcmp(argv[i], "-help") == 0) || (strcmp(argv[i], "-?") == 0)) {
            rfbUsage();
            return FALSE;
        } else if (strcmp(argv[i], "-relativevnc") == 0) {
            relativevnc = true;
            mouse = true;
            rfbLog("\r\nExpect Relative mouse coordinates from the Client, using mouse HID reports\r\n");
        } else if (strcmp(argv[i], "-hidraw") == 0) {
            reserve_dev_hidraw = true;
            rfbLog("\r\nUsing /dev/hidrawXX\r\n");
        } else if (strcmp(argv[i], "-mouse") == 0) {
            mouse = true;
            rfbLog("\r\nExpect Absolute coordinates from the Client, conveted to relative and sent through mouse HID interface\r\n");
        } else if (strcmp(argv[i], "-name") == 0) {  /* -desktop desktop-name */
            if (i + 1 >= *argc){
                rfbUsage();
                return FALSE;
            }
            rfbScreen->desktopName = argv[++i];

        } else {
            i++;
            i1 = i;
            continue;
        }
        /* we just remove the processed arguments from the list */
        rfbPurgeArguments(argc, &i1, i - i1 + 1 , argv);
        i = i1;
    } // end of  for (i = i1 = 1; i < *argc;)
   #endif
    return TRUE;
}


/******************************************************************************************************/
static int openGadgetDev(uint32_t i)
{
    if(i >= NUM_GADGET_DEV){
        printf("-%u- ERR i=%u\n", __LINE__, i);
        return -1;
    }
    //printf("Opening device \'%s\'\n", devname[i]);

    if ((fd_otg[i] = open(devname[i], O_RDWR, 0666)) == -1) {
        printf("-%u- ERR cannot open %s, is module g_hid.ko inserted ?  insmod /drivers/usb/gadget/g_hid.ko\n", __LINE__, devname[i]);
        perror(devname[i]);
        fd_otg[i] = 0;
        return 3;
    }
    return 0;
}


/******************************************************************************************************/
int closeGadgetDev(uint32_t i)
{
    if(fd_otg[i]) close(fd_otg[i]);
    fd_otg[i] = 0;
    return 0;
}


/******************************************************************************************************/
 //read non-stop /dev/hidraw0( or hidraw1 ...) HID reports (8 bytes), and pump into dedicated Fifo
static void * thread_read_dev_hidraw(void *prm)
{
    int res, xx;
    // currently pr, v, hid - not used
    struct hidpipe *hid = (struct hidpipe *)prm;

    uint32_t repBuf[2]; // here is hidden a possible endianess problem for USB reports

    while(1){  // Run till USB device is accessible

        res = read(hid->fd, (void *)repBuf, sizeof(repBuf)); // and thread blocks here
        xx = errno;
        if((res >= hid->my_signat->lenMin) && (res <= hid->my_signat->lenMax)) {
            if (WriteFIFO(RECORD_LENGTH, repBuf, hid->targetFIFO)){
                ClearFIFO(hid->targetFIFO); //FIFO overflow, clear and write the same info again
                WriteFIFO(RECORD_LENGTH, repBuf, hid->targetFIFO);
            }
        } else if(-1 == res){

            if((xx == EINTR) || (xx == EAGAIN)){

                usleep(10000); // non-fatal error.

            } else { // device is disconnected ?

                char bf[128];
                sprintf(bf, "Stop reading /dev/hidraw%u (%s)", hid->hidrawN, hid->my_signat->name); perror(bf);

                // send a report full with 00 00 ...  release all pressed keys.
                repBuf[0] = repBuf[1] = 0;
                if (WriteFIFO(RECORD_LENGTH, repBuf, hid->targetFIFO)){
                    ClearFIFO(hid->targetFIFO);
                    WriteFIFO(RECORD_LENGTH, repBuf, hid->targetFIFO);
                }

                if(NULL != hid->pKeyb) {
                    LOCK(*hid->keybBusy);
                    *hid->pKeyb = -1;  // thus keybdev[hidrawNo] = -1, dev/hidrawXX is not keyboard, no LEDs to write to.
                    UNLOCK(*hid->keybBusy);
                }

                LOCK(*hid->fdevBusy);
                close(hid->fd);
                *hid->pfdev = -1; // thus fdev[hidrawNo] = -1, dev/hidrawXX is free for other threads
                UNLOCK(*hid->fdevBusy);
                free(hid);
                return NULL;
            }

        } else { // The report was not the expected "hid->my_signat->reportlen" bytes, ignore it
            usleep(10000);
        }
    }

    return NULL; // never reached
}


/******************************************************************************************************/
// write the LEDs of connected USB keyboard
static void write_keyb_LED(int fdev, uint32_t val)
{
    if(fdev > 0) {
        unsigned char buf[2]; // byte 0 - always 00 (ReportID), byte 1 - mask 4 bits for NumLock, CapsLOck, ScrollLock LEDs
        int writtenBytes;     // byte 1 only passes through USB cable
        buf[0] = 0;
        buf[1] = val & 0xFF;
        writtenBytes = write(fdev, buf, ARRSZ(buf)); // block here if /dev/hidrawXX is busy, not writable
        if((-1 == writtenBytes) && (errno == EINTR)) {
            printf("ERR: cannot write to \'/dev/hidrawXX\'\n"); //probably USB cable is disconnected
        }
    }
    return;
}


/******************************************************************************************************/
static void vnc2usb_restartmessage(void)
{
    init_threads_attributes(false);
    printf("Restarting %s\n", __FILE__);
    return;
}


/******************************************************************************************************/
 //read non-stop keyb LEDs 'out report', coming from OTG cable -
static void * thread_poll_LEDs(void *dummy)
{
    char res;
    while(1){
        uint32_t ledWord[2];
        if(fd_otg[DEV_KEYB]){
            res = read(fd_otg[DEV_KEYB], (void *)ledWord, 2); // and thread blocks here
            if(res >= 1){
                int i, k[HIDRAW_MAX_DEVICES], j = 0;
                // printf("LEDmask:"); int i; for (i = 0; i < res; i++) printf(" %02X", ledWord[i]); printf("\n");
                KeyboardLEDs = ledWord[0] & 0xFF;

                LOCK(kdevBusy);
                for(i = 0; i < ARRSZ(k); i++) {
                    if((fdev[i] > 0) && (keybdev[i] >= 0))
                        k[j++] = keybdev[i];
                }
                UNLOCK(kdevBusy);

                for(i = 0; i < j; i++){
                    write_keyb_LED(fdev[k[i]], KeyboardLEDs);
                }
            }
        } else sleep(5); //OTG cable is not connected
    }

    return NULL; // never reached
}


/******************************************************************************************************/
static void init_threads_attributes(bool begin)
{   //Once an attribute is initialized and configured, it has process-wide scope.
    //pthread_attr_t DfltThreadAttr; - global variable
    pthread_attr_init(&DfltThreadAttr); // get the default attributest for the new threads
    if(begin){
        int res, detachstate;
        if((res = pthread_attr_getdetachstate(&DfltThreadAttr, &detachstate)))
            printf("Cannot run pthread_attr_getdetachstate %u, %s\n", __LINE__, __FUNCTION__);
        else if(detachstate != PTHREAD_CREATE_DETACHED){
            printf("NOT detached, set attribute %u, %s\n", __LINE__, __FUNCTION__);
            pthread_attr_setdetachstate(&DfltThreadAttr, PTHREAD_CREATE_DETACHED); // this also does malloc()
        }
    } else
        pthread_attr_destroy(&DfltThreadAttr);   // this does free()
    return;
}

/******************************************************************************************************/
/* allocate exact number of records in a FIFO (3..256) and number of 32bit words for a FIFO record(8..128)*/
static uint32_t InitFIFO(uint32_t FIFOSize, uint32_t RecSize,struct mFIFO * p)
{
    if(!(p->pFIFOBuf = (uint32_t *) malloc(sizeof (uint32_t) * FIFOSize * RecSize)))
        return 1;
    else if(!(p->pRecSize = (uint32_t *) malloc(sizeof (uint32_t) * FIFOSize))) /* number of bytes payload in a record. */
        return 1;
    else if(!(p->pBufContent = (uint32_t *) malloc(sizeof (uint32_t) * FIFOSize)))
        return 1;
    p->MaxFifoSize = FIFOSize;
    p->MaxFifoRecSize = RecSize;
    ClearFIFO(p);
    return 0;
}

/******************************************************************************************************/
   /* mark all records as empty, set Tail/Head to 0. */
static void ClearFIFO(struct mFIFO * p)
{
    LOCK(p->FIFObusy);
    p->BufHead = 0;     /* the position where the next payload in FIFO will be written */
    p->BufTail = 0;     /* the next payload from the FIFO which will be fetched */
    /*set all records empty */
    for (p->i = p->MaxFifoSize; p->i--;)
        *(p->pBufContent + p->i) = 0;
    UNLOCK(p->FIFObusy);
    return;
}

/******************************************************************************************************/
  /* clean up... */
#if 0 // currently not used, just Ctrl+C
static void RemoveFIFO(struct mFIFO * p)
{
    free((void *) p->pFIFOBuf);
    free((void *) p->pRecSize);
    free((void *) p->pBufContent);
    return;
}
#endif

/******************************************************************************************************/
 /* Arguments: number of 32bit words to be placed in a FIFO record, and a pointer to the source array */
 /* return: 0-the Source data was copied in the record pointed by BufHead, */
 /*         1-overflow. the previous record was not read by Destination, overwritten with Source data */
static uint32_t WriteFIFO(uint32_t NumWords, uint32_t * pSrcBuf, struct mFIFO * p)
{
    LOCK(p->FIFObusy);
    p->ReturnValue = 0;
    for (p->wi = NumWords; p->wi--;)
        *(p->pFIFOBuf + p->BufHead * p->MaxFifoRecSize + p->wi) = *(pSrcBuf + p->wi);
    /* store the length of SrcBuf in the pRecSize array */
    *(p->pRecSize + p->BufHead) = NumWords;
    *(p->pBufContent + p->BufHead) |= NEW_DATA_AVAIL;
    p->BufHead++;
    if(p->BufHead >= p->MaxFifoSize)
        p->BufHead = 0;
    if((*(p->pBufContent + p->BufHead) & NEW_DATA_AVAIL))
        p->ReturnValue = 1; /* the next record will be overwritten, an overflow flag must be asserted to PPC */
    UNLOCK(p->FIFObusy);
    return p->ReturnValue;
}

/******************************************************************************************************/
 /* Arguments: pointer to a variable where the number of 32bit words sitting in the FIFO Record */
 /* will be stored, and a pointer to the Destination Buffer */
 /* return: 0-the Buf data was copied to Dest.Buffer successfuly, */
 /*         1- No new data, FIFO is empty */
static uint32_t ReadFIFO(uint32_t * NumWordsInRec, uint32_t * pDestBuf,struct mFIFO * p)
{
    LOCK(p->FIFObusy);
    p->i = 0;
    p->RetVal = 0;
    /* if the record pointed by BufTail is still not read = contains new data */
    if ((*(p->pBufContent + p->BufTail) & NEW_DATA_AVAIL)) {
        /* copy the length of the Record */
        *NumWordsInRec = *(p->pRecSize + p->BufTail);
        for (p->i = *NumWordsInRec; (p->i)--;)
            *(pDestBuf + p->i) = *(p->pFIFOBuf + p->BufTail * p->MaxFifoRecSize + p->i);
        /* mark the current Record as read, now it could be filled in with new data by Write function */
        *(p->pBufContent + p->BufTail) &= ~NEW_DATA_AVAIL;
        p->BufTail++;
        /* step ahead and stay here until NewTail record is filled in by WriteFIFO */
        if (p->BufTail == p->MaxFifoSize)
            p->BufTail = 0;
    } else {
        /* the record is empty/ was read already. The dest buf is not changed */
        p->RetVal = 1;
        *NumWordsInRec = 0;
    }
    UNLOCK(p->FIFObusy);
    return p->RetVal;
}
