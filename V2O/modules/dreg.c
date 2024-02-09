#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include "plx9056pciif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_vpd.h"
#include "commonutils.h"
#include "device_detection.h"
#include "vlog.h"
#include "vutils.h"
#include "vlib.h"
#include "license_verifier.h"

static void print_xpi_version(HAL *hp) {
    char info[80];

    if (hal_get_xpi_info(hp, info, sizeof(info)) < 0) {
        printf("Failed to get XPI information\n");
    }
    else if (hp->hw_info.hardware_type != XPI_DEVICE) {
        printf("Not an XPI device.\n");
    }
    else if (!hp->hw_info.hardware_detected) {
        printf("No XPI device detected.\n");
    }
    else {
        printf("Configured as: %s. FPGA Date %s", 
            (hal_get_board_type(hp) == BOARDTYPE_TX) ? "Encoder" :
            "Decoder", info); 
    } 
}

static void print_xpi_features(void) {
#ifdef __XPI__
    printf("Features: ");
    if (!have_basic_license())
        printf("(basic)");
    if (have_basic_license())
        printf("basic");
    if (have_duallink_license())
        printf(", dual-link");
    if (have_stereo_license())
        printf(", stereo");
    printf("\n");
#endif
}

static void print_i50_version(HAL *hp) {
    int board_type = hal_get_board_type(hp);
    int slicesize, compmode, motioncompmode, mquant, dcquant, acnoise, dcnoise;
    char version[80], timestamp[80];

    hal_get_firmware_version(hp, version, 80);
    hal_get_firmware_date(hp, timestamp, 80);

    switch(board_type) {
    case BOARDTYPE_UNKNOWN:
        printf("Configured as: Unknown Hardware\n");
        break;
    case BOARDTYPE_RX:
        printf("V2D Reciever\n");
        printf("\tFPGA Version %s Date %s\n", version, timestamp);
        hal_get_dropside_nobpslice(hp, &slicesize);
        hal_get_dropside_compmode(hp, &compmode, &motioncompmode);
        /* On I50 Rx, motion compmode is not implmented */
        printf("\tSlice Size = %d, Compression Mode = %s\n",
            slicesize, (compmode) ? "444" : "422");
        break;
    case BOARDTYPE_TX:
        printf("V2D Transmitter\n");
        printf("\tFPGA Version %s Date %s\n", version, timestamp);
        hal_get_addside_mquant(hp, &mquant);
        hal_get_addside_dcquant(hp, &dcquant);
        hal_get_addside_nobpslice(hp, &slicesize);
        hal_get_ac_noise_threshold(hp, &acnoise);
        hal_get_dc_noise_threshold(hp, &dcnoise);
        printf("\tAC Noise Threshold = %d, DC Noise Threshold = %d\n",
            acnoise, dcnoise);
        printf("\tMQUANT = %d, DCQUANT = %d, Slice Size = %d\n",
            mquant, dcquant, slicesize);
        break;
    default:
        printf("Invalid board type %d (FPGA not initialized?)\n", board_type);
        break;
    }
}

static void print_xp_board_version(int cno) {
    int board_type, slicesize, compmode, motioncompmode;
    int mq, dcq, acn, dcn, serial = -1;
    char version[80], timestamp[80], revision[80], str[80];
    unsigned long plxconfig, plxrevision;
    HAL *hp;
   
    hp = hal_acquire(cno - 1);

    if (hp == NULL) { 
        printf("Channel %d: No Hardware detected.\n", cno);
        return;
    }

    plxconfig = hal_get_plx_config_val_k(hp, PCI9056_VENDOR_ID);
    plxrevision = hal_get_plx_config_val_k(hp, PCI9056_REVISION_ID);
    board_type = hal_get_board_type(hp);

    switch (board_type) {
    case HARDWARE_UNKNOWN:
        printf("Channel %d: Unknown Hardware.\n", cno);
        printf("\tVendor 0x%04x Chip 0x%04x Rev 0x%02x\n",
            (unsigned short) (plxconfig & 0xffff), 
            (unsigned short) ((plxconfig & 0xffff0000) >> 16), 
            (unsigned char) (plxrevision & 0xff));
        break;

    case HARDWARE_V2D_RX: 
        printf("Channel %d: Configured as: V2D RX\n", cno);
        hal_get_firmware_version(hp, version, 80);
        hal_get_firmware_date(hp, timestamp, 80);
        printf("\tFPGA Version %s Date %s\n", version, timestamp);

        hal_get_dropside_nobpslice(hp, &slicesize);
        hal_get_dropside_compmode(hp, &compmode, &motioncompmode);
        /* On XP RX, motioncompmode and compmode are always the same */
        printf("\tSlice Size = %d, Compression Mode = %d\n",
                slicesize, compmode);
        break;

    case HARDWARE_V2D_TX: 
        printf("Channel %d: Configured as: V2D TX\n", cno);
        hal_get_firmware_version(hp, version, sizeof version);
        hal_get_firmware_date(hp, timestamp, sizeof timestamp);
        printf("\tFPGA Version %s Date %s\n", version, timestamp);

        hal_get_addside_mquant(hp, &mq);
        hal_get_addside_dcquant(hp, &dcq);
        hal_get_addside_nobpslice(hp, &slicesize);
        hal_get_ac_noise_threshold(hp, &acn);
        hal_get_dc_noise_threshold(hp, &dcn);
        printf("\tAC Noise Threshold = %d, DC Noise Threshold = %d\n", 
                    acn, dcn);
        printf("\tMQUANT = %d, DCQUANT = %d, Slice Size = %d\n", 
                    mq, dcq, slicesize);
        break;

    default:
        printf("Invalid board type %d (FPGA not initialized?)\n", board_type);
        break;
    }

    if (hal_get_board_revision(hp, revision, sizeof revision) < 0)
        printf("\tBoard Revision: ???      ");
    else
        printf("\tBoard Revision: %s       ", revision);

    if (hal_get_board_manufacture_date(hp, str, sizeof str) < 0)
        printf("Manufactured on ???\n");
    else
        printf("Manufactured on %s\n", str);

    if (hal_get_board_serial_number(hp, str, sizeof str) < 0) 
        printf("\tSerial Number: ???        ");
    else
        printf("\tSerial Number: %s    ", str);

    sscanf(str, "IPV%d", &serial);

    printf("FPGA Type: %s\n", 
        (serial >= LX100_SERIAL_MIN && serial <= LX100_SERIAL_MAX) ? "LX100" :
        ((serial >= LX80_SERIAL_MIN && serial <= LX80_SERIAL_MAX) ? "LX80" :
        "Unknown"));

    if (hal_get_hardware_description(hp, str, sizeof str) < 0)
        printf("\tHardware Description: ???\n");
    else
        printf("\tHardware Description: %s\n", str);

    hal_release(hp);
}

static void print_help(void)
{
    printf("\nUsage: dreg [options]\n\n");
    printf ("Without no option or the -R option, display or modify FPGA interactively\n");
    printf("Else the following options do specific tasks and exit.\n");
    printf("\n");
    printf("Options:\n");
    printf("\t-0 [-R reg]\t Interactively display or modify Bar 0 registers.\n");
    printf("\t-2 [-R reg]\t Interactively display or modify Bar 2 registers.\n");
    printf("\t-3 [-R reg]\t Interactively display or modify Bar 3 registers.\n");
    printf("\t-A\t\t Restart add side.\n");
    printf("\t-C [-R reg]\t Interactively display or modify PCI config registers.\n");
    printf("\t-D\t\t Restart drop side.\n");
    printf("\t-d [-f first] [-l last]\t Dump registers \"first\" to \"last\"\n");
    printf("\t-I number\t Fire interrupt \"number\".\n");
    printf("\t-o [-R reg]\t Interactively display or modify OmniTek Bar 1 registers\n");
    printf("\t-p pcr\t\t Set PCR value above which to use dual link\n");
    printf("\t-S number\t Set addside slice size to \"number\".\n"); 
    printf("\t-s\t\t Run command silently.\n"); 
    printf("\t-v\t\t Print version information\n");
    printf("\t-X [0|1]\t Set external sync to 0 (use ADC sync) or 1.\n");
    printf("\t-Z num\t\t Request 'num' Full Screen refresh frames(on Tx).\n");
    printf("\n");
    printf("\t-0, -2, -3, -C, -c, -p, -r options are for XP200 only\n");
    printf("\t-o option is for XPI only\n");
}

/*************************************
 * Main entry point load lut utility *
 *************************************/
int dreg_main(int argc, char **argv)
{
    int opt, cno = -1, silent = 0;
    int do_bar0 = 0, do_bar3 = 0, do_pciconfig = 0, do_dump = 0;
    unsigned long first = 0, last = 0x7fc;
    int areset = -1, dreset = -1, interruptnum = -1;
    int pcr48bit = -1, reset_fpga = -1, slicenum = -1, do_omnitek = 0;
    int extsync = -1, print_version = -1;
    unsigned long lreg = 0, lval = 0;
    unsigned short val = 0;
    HAL *hp;
    unsigned long (*plx_get_func)(HAL *, unsigned short);
    void (*plx_set_func)(HAL *, unsigned short, unsigned long);
    unsigned long (*hal_get_func)(HAL *, unsigned long);
    void (*hal_set_func)(HAL *, unsigned long, unsigned long);
    int is_i50 = 0, is_xpi = 0, board_type, hardware_type, ival;
    char buff[80];
    int requestFS = -1;

    /*******************
     * Allow only root *
     *******************/
    if (getuid() != 0) {
        printf("You must be root to execute this command\n");
        return 0;
    }

    hp = hal_acquire(0);
    hardware_type = hal_get_hardware_type(hp);
    if (hp && hardware_type == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }
    else if (hp && hardware_type == XPI_DEVICE) {
        is_xpi = 1;
        cno = 1;
    }

    plx_get_func = hal_get_plx_val_k;
    plx_set_func = hal_set_plx_val_k;
    hal_get_func = hal_get_fpga_val32_k;
    hal_set_func = hal_set_fpga_val32_k;

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "023ACc:DE:I:op:R:rS:svX:Z:")) > 0) {
        switch (opt) {
        case '0': 
            if (is_i50 || is_xpi) { 
                printf("Invalid option -0.\n");
                hal_release(hp);
                return -1;
            }
            do_bar0 = 1;
            break; 
        case '2':
            if (is_i50 || is_xpi) {
                printf("Invalid option -2.\n");
                hal_release(hp);
                return -1;
            }
            hal_get_func = hal_get_bar2_val_k;
            hal_set_func = hal_set_bar2_val_k;
            break; 
        case '3':
            if (is_i50 || is_xpi) {
                printf("Invalid option -3.\n");
                hal_release(hp);
                return -1;
            }
            hal_get_func = hal_get_bar3_val_k;
            hal_set_func = hal_set_bar3_val_k;
            do_bar3 = 1;
            break; 
        case 'A': 
            areset = 1; 
            break;
        case 'C':
            if (is_i50 || is_xpi) {
                printf("Invalid option -C.\n");
                hal_release(hp);
                return -1;
            }
            plx_get_func = hal_get_plx_config_val_k;
            plx_set_func = hal_set_plx_config_val_k;
            do_pciconfig = 1; 
            break; 
        case 'c':
            ival = atoi(optarg);
            if ((is_i50 || is_xpi) && ival != 1) {
                printf("Invalid channel number %d\n", ival);
                hal_release(hp);
                return -1;
            }
            if (ival <= 0 || ival > MAX_FPGA_BOARDS) {
                printf("Invalid channel number %d\n", ival);
                if (hp) hal_release(hp);
                return -1;
            }
            cno = ival; 
            break;
        case 'D':
            dreset = 1;
            break;
        case 'd':
            do_dump = 1;
            break;
        case 'f':
            first = strtoul(optarg, NULL, 16);
            break;
        case 'I':
            if (is_xpi) {
                printf("Invalid option -I.\n");
                hal_release(hp);
                return -1;
            }
            ival = atoi(optarg);
            if ((is_i50 && (ival < 0 || ival > 14)) ||
                    (!is_i50 && (ival < 0 || ival > 29))) {
                printf("Invalid interrupt number %d\n", ival);
                if (hp) hal_release(hp);
                return -1;
            }
            interruptnum = ival;
            break;
        case 'l':
            last = strtoul(optarg, NULL, 16);
            break;
        case 'o':
            if (!is_xpi) {
                printf("Invalid option -o. Exiting.\n");
                if (hp) hal_release(hp);
                return -1;
            }
            do_omnitek = 1;
            hal_get_func = hal_get_omnitek_val_k;
            hal_set_func = hal_set_omnitek_val_k;
            break;
        case 'p':
            if (is_i50 || is_xpi) {
                printf("Invalid option -p\n");
                hal_release(hp);
                return -1;
            }
            ival = atoi(optarg);
            if (ival < 0)  {
                printf("Invalid PCR 48 threshold %d\n", ival);
                if (hp) hal_release(hp);
                return -1;
            }
            pcr48bit = ival;
            break;
        case 'R':
            lreg = strtoul(optarg, NULL, 16);
            break;
        case 'r':
            if (is_i50 || is_xpi) {
                printf("Invalid option -r\n");
                hal_release(hp);
                return -1;
            }
            reset_fpga = 1;
            break;
        case 'S':
            ival = atoi(optarg);
            if (ival < 0) {
                printf("Invalid slices per frame %d\n", ival);
                if (hp) hal_release(hp);
                return -1;
            }
            slicenum = atoi(optarg);
            break;
        case 's':
            silent = 1;
            break;
        case 'v':
            print_version = 1;
            break;
        case 'X':
            extsync = atoi(optarg) ? 1 : 0;
            break;
        case 'Z':
            requestFS = atoi(optarg);
            break;
        default:
            print_help();
            if (hp) hal_release(hp);
            return -1;
        }
    }

    if (optind != argc) {
        print_help();
        if (hp) hal_release(hp);
        return -1;
    }

    if (hp == NULL) {
        printf("Failed. Could not access hardware.\n");
        return -1;
    }

    /* Get hardware description */
    hal_get_hardware_description(hp, buff, sizeof(buff));

    /* Print version information */
    if (print_version != -1) {
        char version[80], timestamp[80];

        if (is_xpi) {
            printf("%s Software Date %s %s\n", buff, __DATE__, __TIME__);
            print_xpi_version(hp);
            print_xpi_features();
            hal_release(hp);
        }
        else {
            get_software_version(version, 80);
            get_software_date(timestamp, 80);
            printf("%s Software Version %s Date %s\n", 
					buff, version, timestamp);
            if (is_i50) {
                print_i50_version(hp);
                hal_release(hp);
            }
            else {
                if (hp) hal_release(hp);
                if (cno == -1) {
                    for (cno = 1; cno <= MAX_FPGA_BOARDS; cno++) 
                        print_xp_board_version(cno);
                }
                else 
                    print_xp_board_version(cno);
            }
        }
        return 0;
    }

    if (!is_i50 && !is_xpi) {
        if (cno == -1) {
            printf("Failed. Invalid channel number.\n");
            return -1;
        }
        if (cno != 1) {
            hal_release(hp);
            hp = hal_acquire(cno - 1);
            if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE) {
                printf ("Channel %d. Failed to open FPGA device\n", cno);
                if (hp) hal_release(hp);
                return -1;
            }
        }
    }

    board_type = hal_get_board_type(hp);

    /* Handle non-interactive specific tasks */
    if (areset != -1) {         
        if (board_type == BOARDTYPE_TX) {
            if (!silent) {
                if (is_i50)
                    printf("Resetting Addside Video...\n");
                else
                    printf("Channel %d. Resetting Addside Video...\n", cno);
            }
            hal_set_video_output(hp, 0);
            hal_resetAddsideVideo(hp);
            sleep(1);
            hal_set_video_output(hp, 1);
        }
        else if (!silent) {
            printf("The -A option is only valid for V2D Transmitters\n");
        }
        hal_release(hp);
        return 0;
    }
    else if (dreset != -1) {      
        if (board_type == BOARDTYPE_RX) {
            if (is_i50) {
                if (!silent) 
                    printf("Resetting Dropside Video...\n");
            }
            else {
                if (!silent)
                    printf("Channel %d. Resetting Dropside Video...\n", cno);
                if (hal_get_overlay_mode(hp) & OV_FLAG_SRC_REMOTE) {
                    hal_or_in_fpga_val_k(hp, FPGA_DCREG, DCREG_LRSYNC_DISABLE);
                    sleep(2);
                }
            }
            hal_soft_reset_fpga(hp);
            hal_resetDropsideVideo(hp);
        }
        else if (!silent) {
            printf("The -D option is only valid for V2D Receivers\n");
        }
        hal_release(hp);
        return 0;
    }
    else if (interruptnum != -1) {
        if (!silent) {
            if (is_i50)
                printf("Firing interrupt %d...\n", interruptnum); 
            else
                printf("Channel %d. Firing interrupt %d...\n", cno, interruptnum); 
        }
        hal_fire_interrupt(hp, interruptnum);
        hal_release(hp);
        return 0;
    }
    else if (pcr48bit != -1) {
        if (board_type != BOARDTYPE_TX) {
            if (!silent)
                printf("The -p option is only valid for V2D Transmitters\n");
            hal_release(hp);
            return 0;
        }
        /* TODO: implement configurable 48bit PCR threshold on TX */
        if (!silent)
            printf("Channel %d. Setting 48bit PCR threshold to %d"
                   " (not implemented yet)...\n", cno, pcr48bit);
        hal_release(hp);
        return 0;
    }
    else if (reset_fpga != -1) {
        int status;
        if (!silent) 
            printf("Resetting Channel %d...\n", cno);
        if ((status = hal_get_board_status(hp)) < 0) {
            if (!silent)
                printf("Failed to get status for Channel %d\n", cno);
            hal_release(hp);
            return 0;
        }
        if (status & FPGA_BOARD_INITIALIZED) {
            if (hal_deactivate_board(hp) < 0) {
                if (!silent) 
                    printf("Failed to de-activate Channel %d\n", cno);
                hal_release(hp);
                return 0;
            }
        }
        if (hal_activate_board(hp) < 0) {
            if (!silent) 
                printf("Failed to activate Channel %d\n", cno);
        }
        else if (!silent) {
            printf("Channel %d driver initialized\n", cno);
        }
        hal_release(hp);
        return 0;
    }
    else if (slicenum != -1) {
        int oldslice;
        if (board_type != BOARDTYPE_TX) {
            if (!silent)
                printf("The -S option is only valid for V2D Transmitters\n");
            hal_release(hp);
            return 0;
        }
        if (hal_get_addside_nobpslice(hp, &oldslice) < 0) {
            if (!silent)
                printf("Failed to get blocks per slice from device\n");
        }
        else if (hal_set_addside_nobpslice(hp, slicenum) < 0) {
            if (!silent)
                printf("Failed to set blocks per slice for device\n");
        }
        else if (!silent) {
            if (is_i50) {
                if (slicenum == 0) 
                    printf("Blocks per slice restored to automatic setting\n");
                else {
                    printf("Blocks per slice changed from %d to %d\n", 
                        oldslice, slicenum);
                    printf("Use \"dreg -S 0\" to change back to automatic"
                           " setting\n");
                }
            }
            else if (slicenum == 0) 
                printf("Channel %d blocks per slice restored to "
                        "automatic setting\n", cno);
            else {
                printf("Channel %d blocks per slice changed from %d "
                        "to %d\n", cno, oldslice, slicenum);
                printf("Use \"dreg -S 0 -c %d\" to change back to "
                        "automatic setting\n", cno);
            }
        }
        hal_release(hp);
        return 0;
    }
    else if (extsync != -1) {
        if (board_type != BOARDTYPE_TX) {
            if (!silent) 
                printf("The -X option is only valid for V2D Transmitters\n");
            hal_release(hp);
            return 0;
        }
        if (!silent) {
            if (is_i50)
                printf("Setting external sync to %s...\n", 
                        (extsync) ? "Enabled" : "Disabled"); 
            else
                printf("Channel %d. Setting external sync to %s...\n", cno,
                        (extsync) ? "Enabled" : "Disabled"); 
        }
        hal_set_ext_sync(hp, extsync);
        hal_release(hp);
        return 0;
    }
    else if (requestFS > 0) {
        if (board_type != BOARDTYPE_TX) {
            if (!silent)
                printf("The -Z option is only valid for V2D Transmitters\n");
            hal_release(hp);
            return 0;
        }
        if (!silent)
            printf("Channel %d: Requesting %d full screen frames.\n", 
                    cno, requestFS);
        if (hal_set_addside_noslicedrop(hp, requestFS) < 0 && !silent) {
            printf("Failed to issue Full screen request.\n");
        }
        return 0;
    }
    else if (do_dump > 0) {
        if (!do_omnitek) {	/* dump FPGA registers */
            first &= 0xfffffffc;
            last &= 0xfffffffc;
        }
        lreg = first; 
        do {
            lval = hal_get_func(hp, lreg);
            printf("0x%08lx\t0x%08lx\n", lreg, lval); 
            lreg += (do_omnitek) ? 1 : 4; 
        }
        while (lreg <= last);

        return 0;
    }

    /* Handle interactive register display/modify tasks */
    if (!is_i50 && do_bar3 && hal_get_hardware_revision(hp) < 2) {
        if (!silent)
            printf("The -3 option is only valid for XP200 rev 2 or later\n");
        hal_release(hp);
        return 0;
    }

    if (do_pciconfig) 
        lreg &= 0x1fc;
    else if (do_bar0) 
        lreg &= 0xffc;
    else if (!do_omnitek)	/* do_bar2, do_bar3 and dreg w/o options */
        lreg &= 0xfffffffc;

    strcpy(buff, "");
    do {
        if (buff[0] == '.') {
            break;
        }
        if (strlen(buff) > 1) {
            lreg -= (do_omnitek) ? 1 : 4; 
            lval = strtoul(buff, NULL, 16);
            if (do_pciconfig || do_bar0 || do_omnitek)
                printf("0x%04x -> 0x%08lx ? ", (unsigned short) lreg, lval);
            else if (is_i50)     /* I50 dreg w/o options */
                printf("0x%04x -> 0x%04x ? ", (unsigned short) lreg, 
                        (unsigned short) lval);
            else                /* do_bar2, do_bar3, XP dreg w/o options */ 
                printf("0x%08lx -> 0x%08lx ? ", lreg, lval);

            if (fgets(buff, 80, stdin) == NULL)
                break;

            if (buff[0] == '.')
                break;

            if (buff[0] != 'y' && buff[0] != 'Y') {
                lreg += 4; 
                continue;
            }

            if (do_pciconfig || do_bar0) 
                plx_set_func(hp, (unsigned short) lreg, lval);
            else if (is_i50) {   /* I50 dreg w/o options */
                hal_set_fpga_val_k(hp, (unsigned short) lreg, 
                        (unsigned short) lval); 
            }
            else	/* do_bar2, do_bar3, do_omnitek & XP dreg w/o options */
                hal_set_func(hp, lreg, lval);
        }

        if (do_pciconfig || do_bar0) {
            if (silent)
                printf("0x%04x -> ", (unsigned short) lreg); 
            else { 
                lval = plx_get_func(hp, (unsigned short) lreg);
                printf("0x%04x == 0x%08lx -> ", (unsigned short) lreg, lval); 
            }
        }
        else if (is_i50) {  /* I50 dreg w/o options */
            if (silent)
                printf("0x%04x -> ", (unsigned short) lreg);
            else {
                val = hal_get_fpga_val_k(hp, (unsigned short) lreg);
                printf("0x%04x == 0x%04x -> ", (unsigned short) lreg, val); 
            }
        }
        else {	/* do_omnitek, do_bar2, do_bar3, XP200 dreg w/o options */ 
            if (silent)
                printf("0x%08lx -> ", lreg);
            else {
                lval = hal_get_func(hp, lreg);
                printf("0x%08lx == 0x%08lx -> ", lreg, lval); 
            }
        }

        lreg += (do_omnitek) ? 1 : 4; 

    } while (fgets(buff, 80, stdin) != NULL);

    hal_release(hp);
    return 0;
}
