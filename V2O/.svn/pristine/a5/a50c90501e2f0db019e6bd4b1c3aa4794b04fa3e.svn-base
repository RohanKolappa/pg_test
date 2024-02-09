#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <linux/vt.h>
#include <errno.h>

#include "setconsole.h"

#define VT100_CLRSCRN   "\033[2J"
#define VT100_HOME      "\033[H"
#define KBD_MESSAGE     "Switching Keyboard/Mouse to User Mode... "

int setconsole_main(int argc, char **argv) {
    int console_mode = 0, ttyfd, ttyno;
 
    /*****************
     * Parse options *
     *****************/
    if (argc != 2) {
        printf("Usage: setconsole [yes|no]\n");
        return -1;
    }
    if (strcasecmp(argv[1], "yes") == 0) {
        console_mode = 1;
    }
    ttyno = 13;
    ttyfd = open("/dev/tty13", O_RDWR);
    if (ttyfd < 0) {
        ttyfd = open("/dev/tty14", O_RDWR);
        ttyno = 14;
    }
    if (ttyfd < 0) {
        printf("Failed to open console TTY. %m.\n");
        return -1;
    }
    /* Change virtual terminal */
    if (console_mode) {
        if (ioctl(ttyfd, VT_ACTIVATE, 1) < 0) {
            printf("VT_ACTIVATE failed ttyno 1\n");
            close(ttyfd);
            return -1;
        }
        if (ioctl(ttyfd, VT_WAITACTIVE, 1) < 0) {
            printf("VT_WAITACTIVE failed ttyno 1\n");
            close(ttyfd);
            return -1;
        }
    }
    else {
        /* Display message on virtual terminal */
        write(ttyfd, VT100_CLRSCRN, sizeof VT100_CLRSCRN);
        write(ttyfd, VT100_HOME, sizeof VT100_HOME);
        write(ttyfd, KBD_MESSAGE, sizeof KBD_MESSAGE);

        if (ioctl(ttyfd, VT_ACTIVATE, ttyno) < 0) {
            printf("VT_ACTIVATE failed ttyno %d\n", ttyno);
            close(ttyfd);
            return -1;
        }
        if (ioctl(ttyfd, VT_WAITACTIVE, ttyno) < 0) {
            printf("VT_WAITACTIVE failed ttyno %d\n", ttyno);
            close(ttyfd);
            return -1;
        }
    }
    close(ttyfd);
    return 0;
}
