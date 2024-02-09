#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include  "hal.h"
#include "vutils.h"
#include "commonutils.h" 
#include "text_utils.h"

static void print_dump_help(void) {
    printf("\t-b board number\n");
    printf("\t-v Video Only\n");
    printf("\t-f <filename> Save to file\n");
    printf("\t-s Use select mechanism\n");
    printf("\t-h print this message\n");
} 

int dump_main (int argc, char **argv) {
    int rfd = 0;
    int wfd = 0;
    int rc = 0;
    int opt, bno = 0;
    HAL *hp;
    char buf[2048];
    struct timeval tvlast, tvnow;
    int nBytes =0;
    unsigned long nTotalBytes = 0;
    unsigned long nTotalPackets = 0;
    unsigned long delta;
    int dovideo = 0, doselect = 0;
    int dosave = 0;
    char sFileName[64];
    int nfds = 0;
    fd_set read_fdset, write_fdset;
    struct timeval timeout;
    double bandwidth;
    int stream_ok = 1;

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "hb:f:vs")) > 0) {
        switch (opt) {
        case 'f':
            strcpy(sFileName, optarg);
            dosave = 1;
            break;
        case 'b':
            bno = atoi(optarg);
            break;
        case 'v':
            dovideo = 1;
            break;
        case 's':
            doselect = 1;
            break;
        case 'h':
        case '?':
            print_dump_help();
            return rc;
        default:
            break;
        }
    }
    hp = hal_acquire (bno);
    if (hp == NULL) {
        printf("Invalid board number %d.\n", bno );
        return rc;
    }
    if (dovideo) {
        //PSCW -- FIXME
        //rfd = hal_get_video_fd(hp);
        rfd = hp->kernel_fd;
    }
    if (dosave) {
        wfd = open (sFileName, O_WRONLY);
        if (wfd < 0) {
            printf ("Could not open file %s\n", sFileName);
            return rc;
        }
    }

    int buflen=2048;
    gettimeofday (&tvlast, NULL);
    
    while (1) {
        if (doselect) {
            FD_ZERO(&read_fdset);
            FD_ZERO(&write_fdset);
            timeout.tv_sec = 0;
            timeout.tv_usec = 1000000;
            FD_SET(rfd, &read_fdset);
            nfds = rfd + 1;
            nfds = select(nfds, &read_fdset, NULL, NULL, &timeout);
            if (nfds == 0) { // timed out
                printf("Select timed out. Aborting...\n");
                printf("Read %lu bytes, %lu packets.\n",
                       nTotalBytes, nTotalPackets);
                break;
            }
            if (nfds < 0) {
                break;
            }
            if (!FD_ISSET(rfd, &read_fdset)) continue;
        }
        nBytes = read(rfd, (void *)buf, buflen);
        if (nBytes < 0) {
            printf("Read Error. Aborting...\n");
            break;
        };
        if (nBytes == 0) {
            if (stream_ok) {
                printf("Video Changed Signal...\n");
                stream_ok = 0; // Next valid data read will reset this flag
                continue;
            }
            else {
                printf("Video Changed Signal Error. Aborting...\n");
                break;
            }
        };

        //break; // Quit after one packet

        stream_ok = 1; // Valid data read so stream still ok

        if (wfd  > 0) {
            write (wfd, buf, nBytes);
        }
        nTotalBytes += nBytes;
        nTotalPackets++;
        gettimeofday (&tvnow, NULL);
        delta = (tvnow.tv_sec - tvlast.tv_sec) *1000000 +
                (tvnow.tv_usec - tvlast.tv_usec);
        bandwidth = (nTotalBytes * 1000000.0/(delta));
        if (delta > 1000000) {
            printf ("%lu ->  %s\n", 
                    delta, bytes_to_bits(bandwidth, buf, 32));
            nTotalBytes = 0;
            nTotalPackets = 0;
            tvlast.tv_sec = tvnow.tv_sec;
            tvlast.tv_usec = tvnow.tv_usec;
        }
    }
    return rc;
}

