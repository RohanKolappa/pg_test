
#include <ctype.h>
#include <sys/stat.h>

#include "hal.h"
#include "commonutils.h"
#include "vlog.h"
#include "vutils.h"
#include "generatedHeaders.h"
#include "v2d.h"


/****************************************
 * Tester utility for Auto Phase Adjust *
 ****************************************/
static void print_help(void)
{
    printf("\nUsage: tester  [OPTIONS]\n\n");
    printf("Run a V2D test.\n\n");
    printf("Options:\n");
    printf("\t-a Type\tDo audio write test for audio type \"Type\".\n");
    printf("\t-A Type\tDo audio write test with 1024 byte DMA.\n");
    printf("\t-w Do Write test.\n");
    printf("\t-r Do Write/Read test.\n");
    printf("\t-s Check Sequence numbers.\n");
    printf("\t-i Count\t Do test \"Count\" times.\n");
    printf("\t-l Length\t Get \"Length\" bytes in each test.\n");
    printf
        ("\t-f File\t\t Write read data to \"File\" (1 Meg or FILESIZE).\n");
    printf("\t-S Filesize \t Set FILESIZE to \"Filesize\".\n");
    printf("\n");
}


int tester_main(int argc, char **argv)
{

#define READ_TEST               1
#define WRITE_TEST              2
#define READ_WRITE_TEST         3
#define AUDIO_WRITE_TEST1       4
#define AUDIO_WRITE_TEST2       5

#define CHECK_SEQUENCE
#define WRITE_SIZE      1024
#define READ_SIZE       10240
#define RW_LEN          0x80000000
#define AUDIOPACKFILE   "/tmp/rxapack"

    int which_test = READ_TEST;
    int opt;
    int rw_len = 0x800000;
    int check_sequence = 0;
    int iterations = 1;
    char *filename = NULL;
    int filesize = 1024 * 1024;
    int audiotype = AUDIOTYPE_DEFAULT;
    int data_sequence = 0;
    HAL *hp;
    int audiofd, videofd;

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Error: Could not open device. Aborting...\n");
        return -1;
    }


    while ((opt = getopt(argc, argv, "wrsl:i:f:S:da:A:D")) > 0) {

        switch (opt) {
        case 'a':
            which_test = AUDIO_WRITE_TEST1;
            audiotype = atoi(optarg);
            break;
        case 'A':
            which_test = AUDIO_WRITE_TEST2;
            audiotype = atoi(optarg);
            break;
        case 'w':
            which_test = WRITE_TEST;
            break;
        case 'r':
            which_test = READ_WRITE_TEST;
            break;
        case 's':
            check_sequence = 1;
            break;
        case 'D':
            data_sequence = 1;
            break;
        case 'S':
            filesize = atoi(optarg);
            break;
        case 'i':
            iterations = atoi(optarg);
            break;
        case 'f':
            filename = optarg;
            break;
        case 'l':
            rw_len = atoi(optarg);
            rw_len &= 0xfffffffc;
            if (rw_len == 0)
                rw_len = 0x800000;
            break;
        case '?':
        case 'h':
            print_help();;
            return -1;
        }
    }

    /* Get hold of video/audio fds */
    //PSCW -- FIXME
    //videofd = hal_get_video_fd(hp);
    videofd = hp->kernel_fd;
    audiofd = hal_get_audio_fd(hp);

    if (audiofd < 0 || videofd < 0) {
        printf("Error: Could not get audio/video feeds.\n");
        goto ERROR_LABEL;
    }



    if (which_test == AUDIO_WRITE_TEST1 || which_test == AUDIO_WRITE_TEST2) {
        FILE *ifile = NULL, *ofile = NULL;
        int rc, len1, len2;
        unsigned char buf1[1024], buf2[1024];
        static int framelen[] =
            { 1024, 520, 520, 268, 520, 268, 268, 142 };

        if (filename == NULL)
            filename = AUDIOPACKFILE;

        if ((ifile = fopen(filename, "rb")) == NULL) {
            printf("Error opening file %s. (%m).\n", filename);
            return -1;
        }

        if ((ofile = fopen("/tmp/tester.dump", "w+b")) == NULL) {
            printf("Error opening file /tmp/tester.dump. (%m).\n");
            return -1;
        }

        while (1) {
            len1 = fread(buf1, 1, framelen[audiotype], ifile);
            if (len1 <= 0 || len1 != framelen[audiotype]) {
                if (len1 == 0)
                    printf("EOF on %s. Exiting.\n", filename);
                else
                    printf
                        ("Failed to read %s (return code %d). Exiting.\n",
                         filename, len1);
                return 0;
            }
            if (audiotype == AUDIOTYPE_DEFAULT ||
                which_test == AUDIO_WRITE_TEST1) {
                if ((rc = write(audiofd, buf1, len1)) != len1) {
                    printf("Failed to write to device. return code %d.\n",
                           rc);
                    return -1;
                }
                if ((rc = fwrite(buf1, 1, len1, ofile)) != len1) {
                    printf
                        ("Failed to write to dump file. return code %d.\n",
                         rc);
                    return -1;
                }
            }
            else {
                len2 = fread(buf2, 1, framelen[audiotype], ifile);
                if (len2 <= 0 || len2 != framelen[audiotype]) {
                    if (len2 == 0)
                        printf("EOF on %s. Exiting.\n", filename);
                    else
                        printf
                            ("Failed to read %s. return code %d. Exiting.\n",
                             filename, len2);
                    return 0;
                }
                memcpy(&buf1[len1 - 8], &buf2[8], len2 - 8);
                if ((rc =
                     write(audiofd, buf1,
                           len1 + len2 - 16)) != len1 + len2 - 16) {
                    printf("Failed to write to device. return code %d.\n",
                           rc);
                    return -1;
                }
                if ((rc =
                     fwrite(buf1, 1, len1 + len2 - 16,
                            ofile)) != len1 + len2 - 16) {
                    printf
                        ("Failed to write to dump file. return code %d.\n",
                         rc);
                    return -1;
                }
            }
        }
        fclose(ifile);
        fclose(ofile);
        return 0;
    }
    else {
        int rc = 0;
        char buff[READ_SIZE];
        char writebuff[WRITE_SIZE];
        int i = 0, j;
        int filefd = -1, frc, frlen = 0;
        unsigned long len;
        struct timeval start, elapsed;
        float bitrate, total_bitrate = 0.0;
        int snapshots = 0;
        unsigned long expseq = 0, currseq = 0;
        unsigned long expseqp = 0, currseqp = 0;
        unsigned long seqlen = 0;
        int err = 0, k;
        unsigned long wseqnumb = 0;
        int max_fd = -1;
        fd_set read_fdset;
        struct timeval timeout;
        int nd;
        int got_video;


        printf("Running for %d iterations.\n", iterations);

        if (which_test == WRITE_TEST) {
            printf("Performing WRITE test.\n");
            filename = NULL;
        }
        else if (which_test == READ_WRITE_TEST)
            printf("Performing WRITE/READ test.\n");

        if (filename != NULL) {

            if ((filefd = creat(filename, S_IRWXU)) < 0) {
                printf("Client: Warning could not open %s\n", filename);
                filename = NULL;
            }
        }

        if (check_sequence) {
            if (which_test == WRITE_TEST) {
                printf("Cannot check sequence number for this test.\n");
                check_sequence = 0;
            }
            else {
                printf("Checking sequence numbers.\n");
            }
        }

        for (j = 0; j < iterations; j++) {

            len = 0;
            i = 0;
            got_video = 0;

            gettimeofday(&start, (struct timezone *) 0);
            while (len < rw_len) {

                if ((which_test == WRITE_TEST) ||
                    (which_test == READ_WRITE_TEST)) {
                    for (i = 0; i < WRITE_SIZE; i += 4)
                        *((unsigned long *) (writebuff + i)) = (wseqnumb +=
                                                                3);
                  try_again:
                    rc = write(videofd, writebuff, WRITE_SIZE);
                    if (rc < 0 && errno == ENOSPC) {
                        usleep(100000);
                        goto try_again;
                    }
                    else if (rc < 0) {
                        perror("Write:");
                        printf("Write error %d\n", rc);
                        break;
                    }
                    else if (rc < WRITE_SIZE) {
                        printf("Error writing full packet %d\n", rc);
                    }
                }
                if ((which_test == READ_TEST) ||
                    (which_test == READ_WRITE_TEST)) {
                    /**************************************************
                     * PSCW - we use select to decide whether to read *
                     * audio or video. We have to read audio even     *
                     * though we only want to read video since if we  *
                     * don't, the audio flow control queue will       *
                     * fill up and we won't get video any more.       *
                     **************************************************/
                    FD_ZERO(&read_fdset);
                    max_fd = -1;
                    FD_SET(videofd, &read_fdset);
                    FD_SET(audiofd, &read_fdset);
                    max_fd = (audiofd > videofd) ? audiofd : videofd;
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;

                    nd = select(max_fd + 1, &read_fdset, NULL, NULL,
                                &timeout);
                    if (nd < 0) {
                        if (errno == EINTR)
                            continue;
                        printf("Select returned with error %m\n");
                        break;
                    }
                    else if (nd == 0)
                        continue;

                    if (FD_ISSET(audiofd, &read_fdset)) {
                        rc = read(audiofd, buff, READ_SIZE);
                        if (rc < 0) {
                            perror("Read audio:");
                            printf("Audio read error %d\n", rc);
                            break;
                        }
                    }
                    if (FD_ISSET(videofd, &read_fdset)) {
                        rc = read(videofd, buff, READ_SIZE);
                        if (rc < 0) {
                            perror("Read video:");
                            printf("Video read error %d\n", rc);
                            break;
                        }
                        got_video = 1;
                    }
                }

                if (!got_video)
                    continue;

                if (check_sequence) {
                    if ((currseqp =
                         *((unsigned long *) (buff + 8))) != expseqp) {
                        if (expseqp != 0)
                            printf
                                ("Packet Sequence error: expected %lu got %lu at %ld\n",
                                 expseqp, currseqp, len);

                        if (err++ > 8)
                            break;
                    }
                    expseqp = currseqp + 1;

                    if (data_sequence) {
                        for (k = 0; k < (rc - 8); k += 4) {

                            if ((currseq =
                                 *((unsigned long *) (buff + 8 + k))) !=
                                expseq) {
                                printf
                                    ("Sequence error: expected 0x%lx got 0x%lx at packet %d byte %lu\n",
                                     expseq, currseq, i, seqlen + k);
                                if (err++ > 8) {
                                    goto ERROR_LABEL;
                                }
                            }
                            if (which_test == READ_WRITE_TEST)
                                expseq = currseq + 3;
                            else
                                expseq = currseq + 1;
                        }

                        seqlen += rc - 8;
                    }

                }               /* Check sequence */

                if (filefd > 0 && frlen < filesize) {
                    frc = write(filefd, buff + 8, rc - 8);
                    if (frc < rc - 8) {
                        printf("Write Error to file %s at %d packet\n",
                               filename, i + 1);
                        filefd = -1;
                    }
                    frlen += frc;
                }

                len += rc;
                i++;

#ifdef LIMIT_BANDWIDTH
                {
                    slotlen += rc;
                    if (slotlen > 8192) {
                        struct timespec req;
                        req.tv_sec = 0;
                        req.tv_nsec = 1000;
                        slotlen = 0;
                        nanosleep(&req, 0);
                    }
                }
#endif
            }

            gettimeofday(&elapsed, (struct timezone *) 0);

            if (elapsed.tv_usec < start.tv_usec) {
                elapsed.tv_usec += 1000000;
                --elapsed.tv_sec;
            }
            elapsed.tv_usec = (elapsed.tv_sec - start.tv_sec) * 1000000 +
                (elapsed.tv_usec - start.tv_usec);
            bitrate = (len * 7.62939453) / elapsed.tv_usec;

            printf
                ("Read %lu bytes using %d iterations in %d microseconds [%d]\n",
                 len, i, (int) elapsed.tv_usec, j);
            printf("Bit Rate = %f Mbps.\n", bitrate);
            printf("Bytes per Iteration = %f.\n", (double) len / i);
            total_bitrate += bitrate;
            snapshots++;
        }

        printf("Average bit rate = %f Mbps.\n", total_bitrate / snapshots);

    }

    hal_release(hp);
    return 0;

  ERROR_LABEL:
    hal_release(hp);
    return -1;
}
