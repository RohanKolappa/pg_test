#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_vpd.h"
#include "commonutils.h"
#include "vlog.h"
#include "loadnet.h"
#include "vutils.h"
#include "ddcedid.h"
#include "generatedHeaders.h"
#include "plx9056pciif.h"
#include "vlib.h"
#include "loadfpga.h"
#include "codec_version_menu_gen.h"

#define  FIX_CODEC_VERSION(codecversion)   (codecversion) == 2 ? 2 : 1

int write_to_fpga(HAL *hp, char *fname, int flags, unsigned char cval, 
        int sval);

/*****************************************************************
 * Routines to load the V2D FPGA from the file 'fname'           *
 *****************************************************************/
static int do_write_to_fpga(HAL *hp,
                            FILE *file, 
                            int flags, 
                            unsigned char cval,
                            int sval) {
#ifndef __XPI__
    struct reg_io regio;
    unsigned long lval, tmplval, numbytes = 0;
    int rc, n, fsize, kfd;
    unsigned char tmpcval; 

    fsize = (flags & LOADFPGA_FLAG_TYPELX80) ? TYPELX80_BITFILE_SIZE : 
        FPGA_BITFILE_SIZE;

    if ((kfd = hp->kernel_fd) < 0) {
        VLOG_INFO("Error: board %d cannot communicate with driver. %m.\n", 
        hp->board_number);
        return -1;
    }

    lval = hal_get_plx_val_k(hp, PCI9056_EEPROM_CTRL_STAT) | 0x00090000;
    usleep(1000);
    hal_set_plx_val_k(hp, PCI9056_EEPROM_CTRL_STAT, lval & 0xfffeffff);
    usleep(1000);
    hal_set_plx_val_k(hp, PCI9056_EEPROM_CTRL_STAT, lval);
    lval = hal_get_plx_val_k(hp, PCI9056_SPACE0_ROM_DESC);
    usleep(1000);

    if (!(flags & LOADFPGA_FLAG_WBYTE)) {
        hal_set_plx_val_k(hp, PCI9056_SPACE0_ROM_DESC, (unsigned long) 0x43430000);
        usleep(1000);
    }
 
    if (flags & LOADFPGA_FLAG_SLEEP) usleep(sval);
    
    rewind(file);
    regio.type = REGIO_TYPE_BAR2_SET;
    regio.reg = 0;
    for (n = 0; n <= fsize / sizeof(unsigned long); n++) {
        if ((rc = fread(&tmplval, sizeof(unsigned long), 1, file)) < 0) {
            VLOG_INFO("Error: failed reading data from file. %m.");
            return -1;
        }
        else if (rc == 0) 
            break;
        if (flags & LOADFPGA_FLAG_WBYTE) {
            int k;
            for (k = 0; k < 4; k++) {
                tmpcval = (tmplval & (0x000000ff << (8 * k))) >> (8 * k);
                regio.value = (tmpcval << 24) | (tmpcval << 16) | 
                        (tmpcval << 8) | tmpcval;
                if (ioctl(kfd, FPGA_IOC_MOD_REG, &regio) < 0) {
                    VLOG_INFO("Error: driver failed to process command. %m.\n");
                    return -1;
                }
            }
        } 
        else {
            regio.value = tmplval; 
            if (ioctl(kfd, FPGA_IOC_MOD_REG, &regio) < 0) {
                VLOG_INFO("Error: driver failed to process command. %m.\n");
                return -1;
            }
        }
        if (flags & LOADFPGA_FLAG_CHECK) {
            tmpcval = (unsigned char) (hal_get_bar2_val_k(hp, 0x100000) & 0xff); 
            if (tmpcval != cval) {
                printf("loadfpga: n = %d Bar 2 Reg 0x100000 = 0x%x (!= 0x%x)\n",
              n, tmpcval, cval);
                VLOG_INFO("loadfpga: n = %d Bar 2 Reg 0x100000 = 0x%x (!= 0x%x)\n", 
            n, tmpcval, cval); 
            }
        } 
        numbytes += sizeof(unsigned long);
    }

    hal_set_plx_val_k(hp, PCI9056_SPACE0_ROM_DESC, lval);
    sleep(1);
#endif
    return 0;
}

int write_to_fpga(HAL *hp,
                  char *fname, 
                  int flags, 
                  unsigned char cval,
                  int sval) {
    FILE *ffile;
    int rc = -1, fsize;
    struct stat stat;

    fsize = (flags & LOADFPGA_FLAG_TYPELX80) ? TYPELX80_BITFILE_SIZE : 
        FPGA_BITFILE_SIZE;
    
    if ((ffile = fopen(fname, "rb")) == NULL) {
        VLOG_INFO("Can't open input file %s. Board %d not loaded.", 
                fname, hp->board_number); 
        return rc;
    }

    if (lstat(fname, &stat) < 0) {
        VLOG_INFO("Can't stat input file %s. %m.", fname);
        fclose(ffile);
        return rc;
    }

    if ((unsigned long) stat.st_size != fsize) {
        VLOG_INFO("Wrong input file %s size %ld (should be %ld).", 
        fname, (unsigned long) stat.st_size, (long) fsize);
        fclose(ffile);
        return rc;
    }


    rc = do_write_to_fpga(hp, ffile, flags, cval, sval);
    fclose(ffile);

    return rc;
}

static void print_load_fpga_help(void) {
    printf("\nUsage: loadfpga [-h] [-r] [-c cval] [-s sval] [-b] [-f fname] board\n");
    printf("Loads the content of 'fname' into the FPGA on board 'board'\n");
    printf("\t-r N retry up to 'N' times if FPGA ID not what it should be after loading.\n");
    printf("\t-c cval check if Bar 2 Reg 0x100000 is 'cval' (hex)\n");
    printf("\t-s sval sleep 'sval' usecs (dec) after setting PLX registers\n");
    printf("\t-l set bitfile name to lx80, size to 2911376\n"); 
    printf("\t-t boardtype\n"); 
    printf("\t-1 set bitfile name for Rev. 1\n"); 
    printf("\t-B do byte write to FPGA (instead of 32-bit write)\n"); 
    printf("\t-b board number \n"); 
    printf("\t-h print this message\n");
} 

int load_fpga_main(int argc, char **argv) {
    char *fname = NULL;
    int i, opt, bno = 0, rc = -1, flags = 0, typelx80 = 0;
    int typelx100 = 0;
    unsigned char cval = 0;
    int retries = 3, sval = 0;
    unsigned long idreg;
    int boardtype = HARDWARE_V2D_TX;
    int codecversion = 0; // by default load version1 codec
    int fpga_type = 0;

    HAL *hp;

    /*******************
     * Check root user *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to load file to the V2D FPGA\n");
        return rc;
    }


    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "hr:c:s:f:b:V:t:R:TB")) > 0) {
        switch (opt) {
        case 'r':
            retries = atoi(optarg);
            break;
        case 'V':
            codecversion = atoi(optarg);
            break;
        case 'b':
            bno = atoi(optarg);
            break;
        case 't':
            boardtype = atoi(optarg);
            break;
        case 'B':
            flags |= LOADFPGA_FLAG_WBYTE;
            break;
        case 'c':
            flags |= LOADFPGA_FLAG_CHECK;
            cval = (unsigned char) strtoul(optarg, NULL, 16);
            break;
        case 's':
            flags |= LOADFPGA_FLAG_SLEEP;
            sval = atoi(optarg);
            break;
        case 'f':
            fname = optarg;
            break;
        case 'h':
        case '?':
            print_load_fpga_help();
            return rc;
        default:
            break;
        }
    }

    hp = hal_acquire(bno);
    if (hp == NULL) {
        printf("Invalid board number %d.\n", bno);
        return rc;
    }

      /* check for correct codec version incase of change in Board and first time sys boot up*/
    if (codecversion == 0) {
        codecversion = 1;
        if (boardtype == HARDWARE_V2D_TX) {
          codecversion = 1;//get_configured_codec_version(hp);
          codecversion = FIX_CODEC_VERSION(codecversion);
        }
    }
     /* Don't load if board not a V2D board  */
    if (!hal_get_hardware_detected(hp)) {
        printf("Board %d not a real V2D board. FPGA not loaded.\n", bno + 1);
        return 0;
    }

    hal_deactivate_board(hp);

    /* Determine which file to load */
    fpga_type = hal_get_fpga_type(hp);

    switch (fpga_type) {
    case FPGA_TYPE_LX100:
        typelx100 = 1;
        flags |= LOADFPGA_FLAG_TYPELX100;
     break;
    case FPGA_TYPE_LX80:
        typelx80 = 1;
        flags |= LOADFPGA_FLAG_TYPELX80;
     break;
    default:
        printf("Unkown FPGA type  %d.\n", fpga_type);
     break;
    }
   
    if (fname == NULL) {
        if (boardtype == HARDWARE_V2D_RX) {
            if (codecversion == 1)
                fname = (typelx80) ? S_LX80_RX_FPGA_FILE : S_LX100_RX_FPGA_FILE;
            else if (codecversion == 2)
                fname = (typelx80) ? S_LX80_RX_SVC_FPGA_FILE
                    : S_LX100_RX_SVC_FPGA_FILE;
            VLOG_INFO("Loading file %s\n", fname);
        }
        else {
            if (codecversion == 1)
                fname = (typelx80) ? S_LX80_TX_FPGA_FILE : S_LX100_TX_FPGA_FILE;
            else if (codecversion == 2)
                fname = (typelx80) ? S_LX80_TX_SVC_FPGA_FILE
                    : S_LX100_TX_SVC_FPGA_FILE;
            VLOG_INFO("Loading file %s\n", fname);
        }
    }
   
    hal_set_driver_enable(hp, 0);
    idreg = hal_get_fpga_val_k(hp, FPGA_IDREG) & 0xf000;
    for (i = 0; i < retries; i++) {
        if (write_to_fpga(hp, fname, flags, cval, sval) < 0) {
            printf("Failed to load FPGA. See system log for errors.\n");
            hal_activate_board(hp);
            return 0;
        }
        idreg = hal_get_fpga_val_k(hp, FPGA_IDREG) & 0xf000; 
        if (idreg == 0xa000 || idreg == 0xd000) break; 
    }

    if (i < retries)
        printf("OK\n");
    else
        printf("Failed to load %s to board %d\n", fname, bno+1);

    hal_activate_board(hp);

    return 0;
}
