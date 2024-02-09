#ifndef _DEV_HIDRAW_H__
#define _DEV_HIDRAW_H__

struct mFIFO;

#undef ARRSZ
#define ARRSZ(x) (sizeof(x)/sizeof(*x))

// known signatures + target FIFO functions
struct signat {
    const char * name; // Keyboard or Mouse
    const char * sign; // first xx bytes of HID 'Report descriptor'.
    const int signlen;
    int lenMin;
    int lenMax;
    struct mFIFO *signFIFO;
 };


// This struct describes thread/pipe for reading /dev/hidrawXX and writing to Keyb/Mous/tablet FIFO's
struct hidpipe {
    int fd;
    int hidrawN;
    struct signat * my_signat;
    struct mFIFO *targetFIFO;
    MUTEX(*fdevBusy);
    int *pfdev; // target of *pfdev (element of array fdev[] in dev-hidraw.c must be accessed always with locked fdevBusy
    MUTEX(*keybBusy);
    int *pKeyb;
};


struct hidpipe * scan_dev_hidraw(int hidrawNo);
void init_pFIFOs(void);


//TODO - from vnc2usb
extern struct mFIFO *pMousFIFO, *pKeybFIFO, *pTabletFIFO;

//TODO from dev-hidraw.c
extern int fdev[];
extern MUTEX(fdevBusy);
extern int keybdev[];
extern MUTEX(kdevBusy);

#endif
