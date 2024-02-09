

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef HAVE_VALGRIND
#include <valgrind/valgrind.h>
#else
#define RUNNING_ON_VALGRIND 0
#endif

#include "hal.h"
#include "hal_hwinfo.h"
#include "vutils.h"

/**************************************
 * Server keyboard/mouse test utility *
 **************************************/
static void print_kmtest_help(void)
{
    printf("\nUsage: kmtest [OPTIONS] [ARGUMENTS]\n\n");
    printf("Start server keyboard or mouse test\n");
    printf("Options:\n");
    printf("\t-c [1|2]\tTest channel 1 or 2\n");
    printf("\t-m\t\tTest mouse (instead of keyboard)\n");
    printf("\t-b\t\tTest both\n");
    printf("\t-B delay\tSet inter-byte delay to \"delay\" msecs\n");
    printf("\t-P delay\tSet inter-packet delay to \"delay\" msecs\n");
    printf("\n");
}

static unsigned char kbbuf[] = {
    0xba, 0x1c, 0x9a, 0xf0, 0x9a, 0x1c,
    0xba, 0x32, 0x9a, 0xf0, 0xaa, 0x32,
    0xba, 0x21, 0x9a, 0xf0, 0xaa, 0x21,
    0xba, 0x23, 0x9a, 0xf0, 0xaa, 0x23,
    0xba, 0x5a, 0x9a, 0xf0, 0xaa, 0x5a,
    0xba, 0x1c, 0x9a, 0xf0, 0xaa, 0x1c,
    0xba, 0x32, 0x9a, 0xf0, 0xaa, 0x32,
    0xba, 0x21, 0x9a, 0xf0, 0xaa, 0x21,
    0xba, 0x23, 0x9a, 0xf0, 0xaa, 0x23,
    0xba, 0x5a, 0x9a, 0xf0, 0xaa, 0x5a,
    0xba, 0x1c, 0x9a, 0xf0, 0xaa, 0x1c,
    0xba, 0x32, 0x9a, 0xf0, 0xaa, 0x32,
    0xba, 0x21, 0x9a, 0xf0, 0xaa, 0x21,
    0xba, 0x23, 0x9a, 0xf0, 0xaa, 0x23,
    0xba, 0x5a, 0x9a, 0xf0, 0xaa, 0x5a,
    0xba, 0x1c, 0x9a, 0xf0, 0xaa, 0x1c,
    0xba, 0x32, 0x9a, 0xf0, 0xaa, 0x32,
    0xba, 0x21, 0x9a, 0xf0, 0xaa, 0x21,
    0xba, 0x23, 0x9a, 0xf0, 0xaa, 0x23,
    0xba, 0x5a, 0x9a, 0xf0, 0xaa, 0x5a
};
static unsigned char msbuf[] = {
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x08, 0x00, 0x05, 0x20, 0x05,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb,
    0x10, 0x38, 0x00, 0xfb, 0x20, 0xfb
};

int kmtest_main(int argc, char **argv)
{
    char test = 'k';
    int bdelay = 0;
    int pdelay = 0;
    int pktnum = 0;
    int opt, i, is_i50 = 0, cno = -1;
    HAL *hp;
    int keyboardfd, mousefd;

    hp = hal_acquire(0);
    if (hp != NULL && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }

    while ((opt = getopt(argc, argv, "mbB:P:c:h")) > 0) {

        switch (opt) {
        case 'c':
            cno = atoi(optarg);
            break;
        case 'm':
            if (test == 'k') {
                test = 'm';
                break;
            }
            else {
                print_kmtest_help();
                return -1;
            }
        case 'b':
            if (test == 'k') {
                test = 'b';
                break;
            }
            else {
                print_kmtest_help();
                return -1;
            }
            break;
        case 'B':
            bdelay = atoi(optarg);
            break;
        case 'P':
            pdelay = atoi(optarg);
            break;
        case '?':
        case 'h':
            print_kmtest_help();
            return -1;
        }
    }
    if (is_i50) {
        if (cno != 1) {
            if (hp) hal_release(hp);
            print_kmtest_help();
            return -1;
        }
    }
    else if (cno <= 0 || cno > MAX_FPGA_BOARDS) {
        if (hp) hal_release(hp);
        print_kmtest_help();
        return -1;
    }
    else if (cno != 1) {
        if (hp) hal_release(hp);
        hp = hal_acquire(cno - 1);
    }

    if (hp == NULL) {
        printf("Failed. Could not access hardware.\n");
        return -1;
    }

    mousefd = hal_get_mouse_fd(hp);
    keyboardfd = hal_get_keyboard_fd(hp);

    printf("Writing to %s: %d msec inter-byte, %d msec inter-packet\n",
           (test == 'k') ? "KB" : ((test == 'm') ? "MS" : "KB & MS"),
           bdelay, pdelay);

    while (1) {
        if (test == 'm' || test == 'b') {
            if (write(mousefd, &msbuf[pktnum * 6], 6) < 0) {
                printf("Failed to write MS data to FPGA\n");
                hal_release(hp);
                return -1;
            }
        }

        if (test == 'b')
            usleep(pdelay * 1000);

        if (test == 'k' || test == 'b') {
            for (i = 0; i < 3; i++) {
                if (write(keyboardfd, &kbbuf[(pktnum * 3 + i) * 2], 2) <
                    0) {
                    printf("Failed to write KB data to FPGA\n");
                    hal_release(hp);
                    return -1;
                }
                usleep(bdelay * 1000);
            }
        }
        pktnum = (pktnum + 1) % 20;
        usleep(pdelay * 1000);
    }
    hal_release(0);
    return 0;
}
