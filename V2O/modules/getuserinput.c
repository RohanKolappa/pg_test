#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <linux/input.h>

#include "vutils.h"

#define ESC         0x1b
#define TIMEOUT     2

int getuserinput_main(int argc, char **argv) {
    fd_set read_fdset;
    int nd, max_fd = -1, rc = 1, secs = 0, waittime = TIMEOUT;
    struct timeval timeout;
    unsigned char buf[1];

    if (argc > 2) {
        printf("Usage: %s [waittime]\n", argv[0]);
        return -1;
    }

    if (argc > 1) 
        waittime = atoi(argv[1]);

    if (waittime <= 0) waittime = TIMEOUT;

    system("/bin/stty -echo -cooked > /dev/null 2>&1");
    
    while (1) {
        FD_ZERO(&read_fdset);
        max_fd = -1;
        FD_SET(0, &read_fdset);
        max_fd = 0;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        nd = select(max_fd + 1, &read_fdset, NULL, NULL, &timeout);
        if (nd < 0) {
            if (errno != EINTR)
                break;
        }
        else if (nd == 0) {
            if (++secs > waittime) {
                rc = 0; break;
            }
        }
        else {
            fread(buf, 1, 1, stdin);
            fflush(stdin);
            if (buf[0] == ESC)
                break;
        }
    } 

    system("/bin/stty echo cooked > /dev/null 2>&1");

    printf("%d", rc);

    fflush(stdout);

    return rc;
} 
