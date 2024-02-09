#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "vncrfb.h" // for MUTEX stuff
#include "dev-hidraw.h"

#define OPERATING_SYSTEM linux
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>


//#define LOGG

static char *hidraw_path = "/sys/class/hidraw/";

int fdev[HIDRAW_MAX_DEVICES];
MUTEX(fdevBusy);

int keybdev[ARRSZ(fdev)]; // keeps which /dev/hidrawXX devices are keyboards. Needed for LEDs
MUTEX(kdevBusy); // locks access to keybdev[]

// 'standard' USB Boot protocol keyboard report descriptor, first bytes
static char const keyboardSignature[] = {0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07};
static char const mouseSignature[] = {0x05, 0x01, 0x09, 0x02, 0xA1, 0x01};
static char const netvizSignature[] = {0x00, 0x01, 0x02}; // fake, TODO


struct signat knownSignat[] = {
    { "Keyboard", keyboardSignature, ARRSZ(keyboardSignature), 8, 8, NULL }, // NULL - there comes mFIFO, kFIFO etc
    { "Mouse",    mouseSignature,    ARRSZ(mouseSignature),    3, 5, NULL },
    { "MouseAbs", netvizSignature,   ARRSZ(netvizSignature),   4, 4,  NULL } //TODO
    // TODO add Netviz2 / tablet signature
};

struct signat *pSignat;



void init_pFIFOs(void){
    int i;
    for(i = 0; i < ARRSZ(fdev); i++)
        fdev[i] = keybdev[i] = -1;

    INIT_MUTEX(fdevBusy);
    INIT_MUTEX(kdevBusy);
    knownSignat[0].signFIFO = pKeybFIFO;
    knownSignat[1].signFIFO = pMousFIFO;
    knownSignat[2].signFIFO = pTabletFIFO;
}


// search /dev/hidrawXX, return NULL if nothing new was found, or the device is detected/attached already

struct hidpipe * scan_dev_hidraw(int hidrawNo)
{
    int r, fd = -1, len, found = 0;

    struct hidpipe *hid = NULL;
    struct stat devstat;
    struct hidraw_devinfo info;
    struct hidraw_report_descriptor *desc;
    char bufDesc[1024], bufLog[256], *t = bufLog;

    if(hidrawNo >= HIDRAW_MAX_DEVICES)
        return hid;

    // 1. create a name:
    t += sprintf(t, "/dev/hidraw%u", hidrawNo);

    // 3. exists device /dev/hidrawXX ?
    r = stat(bufLog, &devstat);
    if (r < 0){
        *bufLog = '\0';
        goto deviceDetectionFailed;
    }

    // 4. open /dev/hidrawXX in r/w mode
    LOCK(fdevBusy);         // TODO: it is possible that fdev[hidrawNo] >=0, but /dev/hidrawXX is not available;
    fd = fdev[hidrawNo];    // This could happen when a thread is dead without correct close() + free()
    if(fd >= 0){
        UNLOCK(fdevBusy);
        fd = -1;
       #ifdef LOGG
        t += sprintf(t, " processed already");
       #else
        *bufLog = '\0';
       #endif
        goto deviceDetectionFailed;
    }

    fd = open(bufLog, O_RDWR);
    if (fd < 0){
        UNLOCK(fdevBusy);
        t += sprintf(t, " could not be opened");
        perror(NULL); //using errno
        goto deviceDetectionFailed;
    }

    fdev[hidrawNo] = fd;
    UNLOCK(fdevBusy);

    // 5. Retrieve vendor/product All ioctl are handled in .../drivers/hid/hidraw.c, hidraw_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
    r = ioctl(fd, HIDIOCGRAWINFO, &info);  //struct hidraw_devinfo info;   linux/hidraw.h:#define HIDIOCGRAWINFO
    if (r < 0)
        goto deviceDetectionFailed;

    info.vendor &= 0xFFFF;
    info.product &= 0xFFFF;
    t += sprintf(t, " %04x:%04x", info.vendor, info.product & 0xFFFF);

    // 6. read HID descriptor size
    r = ioctl(fd, HIDIOCGRDESCSIZE, &len);
    if (r < 0 || len < 1)
        goto deviceDetectionFailed;
    //printf(" len=%u\n", len);

    // 7. Reading HID descriptor up to (buf - sizeof(int)) bytes
    desc = (struct hidraw_report_descriptor *)bufDesc;
    if (len > (sizeof(bufDesc) - sizeof(int)) )
        len = sizeof(bufDesc) - sizeof(int);
    desc->size = len;

    // 8. read the HID descriptor up to desc->size bytes
    r = ioctl(fd, HIDIOCGRDESC, desc); //struct hidraw_report_descriptor *desc;
    if (r < 0)
        goto deviceDetectionFailed;

    // 8a. Dump the Report descriptor
    //char dd[1024], *t = dd; int v; for(v = 0; v < desc->size; v++){ t += sprintf(t, " %02X", *(char *)(desc->value + v)); } printf("Descriptor:%s\n", dd);

    int cnt, x;
    for (cnt = 0; cnt < ARRSZ(knownSignat); cnt++) {
        pSignat = knownSignat + cnt;
        if(len > pSignat->signlen) {
            found = 1;
            for(x = 0; x < pSignat->signlen; x++) {
                if(pSignat->sign[x] != desc->value[x]){
                    found = 0;
                    break;
            }   }

            if(found){

                if((hid = (struct hidpipe *)malloc(sizeof(struct hidpipe)))){
                    hid->fd = fd;       // success
                    hid->hidrawN = hidrawNo;
                    hid->my_signat = pSignat;
                    hid->targetFIFO = pSignat->signFIFO;
                    hid->fdevBusy = &fdevBusy;
                    hid->pfdev = fdev + hidrawNo;
                    t += sprintf(t, " attached %s", pSignat->name);
                    if(pSignat->signFIFO == pKeybFIFO) { // a USB keyboard is attached
                        hid->keybBusy = &kdevBusy;       // then add this /dev/hidrawXX to array
                        hid->pKeyb = keybdev + hidrawNo; // with keyboards - where LED byte must be written to.
                        LOCK(*hid->keybBusy);
                        *hid->pKeyb = hidrawNo;
                        UNLOCK(*hid->keybBusy);

                    } else {
                        hid->pKeyb = NULL; // HID device, but not keyboard - no LEDs
                    }
					// Avoid keyboard/mouse USB reports going to the kernel input subsystem
					sprintf(bufDesc, "echo \'1\' > %shidraw%u/isolate", hidraw_path, hidrawNo); // TODO - in libsysfs
					system(bufDesc); //printf("Running %s\n", bufDesc);
				}
                break;
            }
        }
    }

deviceDetectionFailed:
    if ((NULL == hid) && (fd > 0)){
        close(fd);
        LOCK(fdevBusy);
        fdev[hidrawNo] = -1;
        UNLOCK(fdevBusy);
       #ifdef LOGG
        t += sprintf(t, " unknown HID device, Ignored");
       #else
        return NULL;
       #endif

    }
    if(*bufLog)
        printf("%s\n", bufLog);
    return hid;
}

