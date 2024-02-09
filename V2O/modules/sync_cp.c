
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "vutils.h"



/********************************************
 * Routine for synchronized copy to file    *
 ********************************************/
static void print_sync_cp_help(void)
{
    printf("\nUsage: sync_cp src_file dst_file\n");
    printf("Synchronized copy of src_file to dst_file\"\n");
};

int sync_cp_main(int argc, char **argv)
{
    char *src, *dst;
    int sfd, dfd;
    int len;
    unsigned char buf[65536];

    if (argc != 3) {
        print_sync_cp_help();
        return -1;
    }

    src = argv[1];
    dst = argv[2];

    sfd = open(src, O_RDONLY);
    if (sfd < 0) {
        if (errno == ENOENT)
            return 0;
        printf("Failed to open source file %s. %m.\n", src);
        return -1;
    }
    dfd =
        open(dst, O_CREAT | O_WRONLY | O_TRUNC,
             S_IRWXU | S_IRGRP | S_IROTH);
    if (dfd < 0) {
        printf("Failed to open destination file %s. %m.\n", dst);
        close(sfd);
        return -1;
    }
    while (1) {
        if ((len = read(sfd, buf, sizeof(buf))) < 0) {
            printf("sync_cp: Failed to read from file %s. %m.\n", src);
            close(sfd);
            close(dfd);
            return -1;
        }
        else if (len == 0)
            break;
        if (write(dfd, buf, len) < 0) {
            printf("Failed to write to file %s. %m.\n", dst);
            close(sfd);
            close(dfd);
            return -1;
        }
    }
    fsync(dfd);
    close(sfd);
    close(dfd);

    return 0;
}
