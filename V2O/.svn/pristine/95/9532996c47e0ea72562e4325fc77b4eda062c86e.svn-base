#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "commonutils.h"
#include "vlog.h"
#include "vutils.h"
#include "v2d.h"


/*********************************************************************
 * Test utility to dump content of frame buffer and dslice SRAM      *
 *                                                                   *
 * dumpsram [-f|d] -n 2048 -a addr -H 1024 -V 768 -B bitmap.bmp      *
 *                                                                   *
 * will dump 2048 locations of frame buffer or dslice SRAM data and  *
 * write to a 1024x768 bit map file named 'bitmap.bmp'.              *
 *********************************************************************/
#define SRAM_SNCTLREG   0x104

#define FB_TSTREG       0x114
#define FB_ADRREG       0x118
#define FB_DATREG_H     0x11c
#define FB_DATREG_L     0x120
#define FB_TSTENREG     0x124
#define FB_STSREG       0x128

#define DS_TSTREG       0x130
#define DS_ADRREG       0x134
#define DS_DATREG_H     0x138
#define DS_DATREG_L     0x13c
#define DS_TSTENREG     0x12c
#define DS_STSREG       0x140


static void print_dump_sram_help(void)
{
    printf("Usage: dumpsram [OPTIONS] \n");
    printf("Dumps the content of frame buffer SRAM.\n");
    printf("Options: -n num\t\tDump 'num' SRAM locations\n");
    printf
        ("\t -d       \tDumps dslice SRAM (frame buffer SRAM is used if -d is not present)\n");
    printf("\t -N       \tGet slice number as data instead of pixels\n");
    printf("\t -a addr  \tSet start address to 'addr'\n");
    printf("\t -b bank  \tSet SRAM bank to 'bank'\n");
    printf
        ("\t -s usecs \tSleep usecs between setting addr & reading SRAM\n");
    printf
        ("\t -w usecs \tBusy wait usecs between setting addr & reading SRAM\n");
    printf
        ("\t          \tNote -s is used if both -s and -w are specified, and\n");
    printf
        ("\t          \tpolling for data valid will be used if neither is present\n");
    printf("\t -B bitmap\tConvert and write data to BMP file 'bitmap'\n");
    printf("\t -H hres\tSet HRES for BMP file to 'hres''\n");
    printf("\t -V hres\tSet VRES for BMP file to 'vres'\n");
    printf("\t -h Print this message\n");
}

static int write_bmp_file(char *bmpfilename, struct splash_frame *splash)
{
    FILE *fs;
    unsigned char buff[54], *sp;
    int i, j, hres, vres;

    if (bmpfilename == NULL)
        return -1;

    hres = splash->sf_hres;
    vres = splash->sf_vres;

    if ((fs = fopen(bmpfilename, "w+")) != NULL) {
        sp = buff;
        bzero(buff, sizeof(buff));

        /* BITMAPFILEHEADER */
        *(unsigned short *) (sp + 0) = 19778;
        *(unsigned long *) (sp + 2) = 54 + splash->sf_len;
        *(unsigned long *) (sp + 10) = 54;
        /* BITMAPINFOHEADER */
        *(unsigned long *) (sp + 14) = 40;
        *(unsigned long *) (sp + 18) = hres;
        *(unsigned long *) (sp + 22) = vres;
        *(unsigned short *) (sp + 26) = 1;
        *(unsigned short *) (sp + 28) = 24;
        *(unsigned long *) (sp + 34) = splash->sf_len;
        fwrite(buff, 54, 1, fs);
        for (sp = splash->sf_buffer, i = 0; i < vres; i++) {
            for (j = 0; j < hres; j++) {
                /* Write blue first, then green, then red */
                fwrite(&sp[2], 1, 1, fs);
                fwrite(&sp[1], 1, 1, fs);
                fwrite(&sp[0], 1, 1, fs);
                sp = &sp[3];
            }
        }
    }
    fclose(fs);
    return 0;
}


static void convert_sram_dump(unsigned char *splashbuf,
                              int splashbufsize,
                              unsigned long *srambuf, int sramsize)
{
    unsigned char *cp = splashbuf;
    int i;

    for (i = 0; i < sramsize && i < (splashbufsize / 4); i++) {
        *cp++ = (unsigned char) ((srambuf[i] & 0x000000ff));
        *cp++ = (unsigned char) ((srambuf[i] & 0x0000ff00) >> 8);
        *cp++ = (unsigned char) ((srambuf[i] & 0x00ff0000) >> 16);
        *cp++ = (unsigned char) ((srambuf[i] & 0xff000000) >> 24);
    }
}

int dumpsram_main(int argc, char **argv)
{
    int i, j, opt, verbose = 0;
    int board_type, hres = 0, vres = 0, fsize, dsize = 1;
    char *bmpfile = NULL;
    unsigned long *buffer;
    unsigned short val1, val2, bank = 0;
    unsigned long start = 0, addr;
    int sleep = 0, wait = 0, do_dslice = 0, do_slicenumber = 0;
    unsigned short tstreg, adrreg, tstenreg, datreg_l, datreg_h, stsreg;
    HAL *hp;

    while ((opt = getopt(argc, argv, "dNn:b:a:s:w:B:H:V:hv")) > 0) {
        switch (opt) {
        case 'v':
            verbose = 1;
            break;
        case 'd':
            do_dslice = 1;
            break;
        case 'N':
            do_slicenumber = 1;
            break;
        case 'n':
            dsize = atoi(optarg);
            break;
        case 'b':
            bank = (short) atoi(optarg);
            break;
        case 'a':
            start = (long) atol(optarg);
            break;
        case 's':
            sleep = atoi(optarg);
            break;
        case 'w':
            wait = atoi(optarg);
            break;
        case 'B':
            bmpfile = optarg;
            break;
        case 'H':
            hres = atoi(optarg);
            break;
        case 'V':
            vres = atoi(optarg);
            break;
        case 'h':
        case '?':
            print_dump_sram_help();
            return -1;
        }
    }

    if (optind != argc || (bmpfile != NULL && (hres == 0 || vres == 0)) ||
        bank > 3) {
        print_dump_sram_help();
        return -1;
    }

    hp = hal_acquire(0);
    if (hp == NULL) {
        printf("Failed to retrieve V2D hardware type. Aborting...\n");
        return -1;
    }
    board_type = hal_get_board_type(hp);

    if (board_type != BOARDTYPE_RX) {
        printf("Not on a V2D-Rx\n");
        goto ERROR_LABEL;
    }

    if (sleep < 0)
        sleep = 0;
    if (wait < 0)
        wait = 0;

    if (do_dslice == 0) {
        tstreg = FB_TSTREG;
        tstenreg = FB_TSTENREG;
        adrreg = FB_ADRREG;
        datreg_l = FB_DATREG_L;
        datreg_h = FB_DATREG_H;
        stsreg = FB_STSREG;
    }
    else {
        tstreg = DS_TSTREG;
        tstenreg = DS_TSTENREG;
        adrreg = DS_ADRREG;
        datreg_l = DS_DATREG_L;
        datreg_h = DS_DATREG_H;
        stsreg = DS_STSREG;
    }

    if (do_slicenumber)
        hal_or_in_fpga_val_k(hp, SRAM_SNCTLREG, 0x01);
    else
        hal_and_in_fpga_val_k(hp, SRAM_SNCTLREG, 0xfe);

    /* Make sure SRAM test register is 0 */
    hal_set_fpga_val_k(hp, tstreg, 0x00);

    /* Stop FPGA r/w to dsramctl so software can access read data */
    hal_or_in_fpga_val_k(hp, tstenreg, 0x01);

    if (bmpfile != NULL) {
        struct splash_frame splash_frame;

        if (hres == 0 || vres == 0) {
            printf("Bad horiz or vert resolution\n");
            goto ERROR_LABEL;
        }

        fsize = (hres * vres * 3 + 3) / 4;      /* SRAM uses 4 longs for 3 pixels */
        if (dsize > (hres * vres * 3) / 4)
            dsize = (hres * vres * 3) / 4;

        /* Allocate memory for splash frame */
        splash_frame.sf_len = hres * vres * 3;
        splash_frame.sf_buffer = malloc(splash_frame.sf_len * 2);
        if (splash_frame.sf_buffer == NULL) {
            printf("Can't allocate memory for splash buffer. %m.\n");
            goto ERROR_LABEL;
        }
        memset(splash_frame.sf_buffer, 0, splash_frame.sf_len * 2);
        splash_frame.sf_hres = hres;
        splash_frame.sf_vres = vres;

        /* Allocate memory for buffer to read SRAM values */
        buffer = calloc(fsize, sizeof(long));
        if (buffer == NULL) {
            printf("Can't allocate memory for pixel data. %m.\n");
            goto ERROR_LABEL;
        }

        printf("Reading %d SRAM values ...\n", dsize);

        for (i = 0; i < dsize; i++) {

            /* Set SRAM read bank and address */
            addr = start + i;
            val1 =
                (bank << 4) | (unsigned short) ((addr & 0x000f0000) >> 16);
            hal_set_fpga_val_k(hp, tstreg, val1);
            hal_set_fpga_val_k(hp, adrreg, (unsigned short) (addr & 0x0000ffff));

            /* Toggle SRAM read enable bit from 0 to 1 */
            val1 |= (1 << 6);
            hal_set_fpga_val_k(hp, tstreg, val1);

            if (sleep == 0 && wait == 0) {
                /* Poll for data valid */
                while (!(hal_get_fpga_val_k(hp, stsreg) & 0x01)) {
                    for (j = 0; j < 10; j++);
                }
            }
            else if (sleep == 0) {
                /* Busy loop 'wait' usecs */
                for (j = 0; j < 100 * wait; j++);
            }
            else {
                /* Sleep 'sleep' usecs */
                usleep(sleep);
            }

            /* Read the SRAM value and convert to pixel data */
            val1 = hal_get_fpga_val_k(hp, datreg_h);
            val2 = hal_get_fpga_val_k(hp, datreg_l);
            buffer[i] =
                (unsigned long) (val1 << 16) | (unsigned long) val2;

            /* Reset SRAM read enble bit */
            val1 &= ~(1 << 6);
            hal_set_fpga_val_k(hp, tstreg, val1);
        }
        if (verbose) {
            for (i = 0; i < dsize / 8; i += 8) {
                printf("%08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
                       buffer[i], buffer[i + 1], buffer[i + 2],
                       buffer[i + 3], buffer[i + 4], buffer[i + 5],
                       buffer[i + 6], buffer[i + 7]);
            }
        }
        printf("Converting to pixels...\n");
        convert_sram_dump(splash_frame.sf_buffer, splash_frame.sf_len,
                          buffer, dsize);
        free(buffer);

        printf("Writing to BMP file...\n");
        if (write_bmp_file(bmpfile, &splash_frame) < 0) {
            printf("Failed to write BMP file. %m.\n");
            free(splash_frame.sf_buffer);
            goto ERROR_LABEL;
        }
        free(splash_frame.sf_buffer);
    }
    else {
        unsigned long lval;
        printf("Reading %d SRAM values ...\n", dsize);
        for (i = 0; i < dsize; i++) {
            /* Set SRAM read bank and address */
            addr = start + i;
            val1 =
                (bank << 4) | (unsigned short) ((addr & 0x000f0000) >> 16);
            hal_set_fpga_val_k(hp, tstreg, val1);
            hal_set_fpga_val_k(hp, adrreg, (unsigned short) (addr & 0x0000ffff));

            /* Toggle SRAM read enable bit from 0 to 1 */
            val1 |= (1 << 6);
            hal_set_fpga_val_k(hp, tstreg, val1);

            if (sleep == 0 && wait == 0) {
                /* Poll for data valid */
                while ((hal_get_fpga_val_k(hp, stsreg) & 0x01) == 0) {
                    for (j = 0; j < 10; j++);
                }
            }
            else if (sleep == 0) {
                /* Busy loop 'wait' usecs */
                for (j = 0; j < 100 * wait; j++);
            }
            else {
                /* Sleep 'sleep' usecs */
                usleep(sleep);
            }

            /* Read the SRAM value and convert to pixel data */
            val1 = hal_get_fpga_val_k(hp, datreg_h);
            val2 = hal_get_fpga_val_k(hp, datreg_l);
            lval = (unsigned long) (val1 << 16) | (unsigned long) val2;
            printf("%08lx\n", lval);

            /* Reset SRAM read enble bit */
            val1 &= ~(1 << 6);
            hal_set_fpga_val_k(hp, tstreg, val1);
        }
    }

    /* Restore SRAM access register */
    hal_set_fpga_val_k(hp, tstreg, 0x00);

    /* Reset SRAM test enable bit */
    hal_and_in_fpga_val_k(hp, tstenreg, 0xfe);

    /* Disable and enable video */
    hal_and_in_fpga_val_k(hp, FPGA_DCREG, ~DCREG_VID_ENABLE);
    usleep(1000);
    hal_or_in_fpga_val_k(hp, FPGA_DCREG, DCREG_VID_ENABLE);

    hal_release(hp);
    return 0;

ERROR_LABEL:
    hal_release(hp);
    return -1;
}
