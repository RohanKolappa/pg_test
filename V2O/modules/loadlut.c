#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>

#include "hal.h"
#include "hal_hwinfo.h"
#ifndef __XPI__
#include "lut_table.h"
#endif
#include "lut_utils.h"
#include "vutils.h"
#include "vlog.h"

#define XPFLAG_BIT	0x01
#define XPIFLAG_BIT	0x02

static void kernel_luts(HAL *hp, int full_print);
static void print_current_luts(HAL *hp, int full_print);
static void print_lut(struct LUT_struct *lutp, int count, int full_print);

static void print_loadlut_help(int flag)
{
    if (flag & XPIFLAG_BIT) {
        printf("\nUsage: loadlut [OPTIONS]\n\n");
        printf("Loads or Displays LUT configuration.\n\n");
        printf("Options:\n");
        printf("\t-p\t Display only (do not Load)\n");
        printf("\t-k\t Display all LUTs in kernel (not from config file)\n");
        printf("\t-C\t Displays current add/drop/display LUT in the kernel,\n");        
        printf("\t-Q\t Displays current addside LUT in the ADV7604.\n");
    }
    else {
        printf("\nUsage: loadlut [OPTIONS]\n\n");
        printf("Loads or Displays LUT configuration.\n\n");
        printf("Options:\n");
        printf("\t-p\t Display only (do not Load)\n");
        printf("\t-k\t Display all LUTs in kernel (not from config file)\n");
        printf("\t-C\t Display current add/drop/display LUTs in kernel\n");
        printf("\t-F\t Print more detailed info about LUTs\n");
    }
    if (flag & XPFLAG_BIT) printf("\t-c\t Channel Nmber [1,2]\n");
}

/*************************************
 * Main entry point load lut utility *
 *************************************/
int loadlut_main(int argc, char **argv)
{
    int opt, cno = -1, is_i50 = 0, is_xpi = 0;
    int currentidx = -1;
    int parse_only = 0;
    int full_print = 0;
    int kernel_print = 0;
    int current_luts = 0;
    struct list_head *tmp, lut_list_head;
    struct LUT_struct *lutp;
    HAL *hp = NULL;
    int flag = 0;

    /*********************
     * Check system type *
     *********************/
    hp = hal_acquire(0);
    if (hp != NULL && hal_get_hardware_type(hp) == V2DI50_DEVICE) {
        is_i50 = 1;
        cno = 1;
    }
    else if (hp != NULL && hal_get_hardware_type(hp) == XPI_DEVICE) {
        is_xpi = 1;
        cno = 1;
        flag = XPIFLAG_BIT;
    }
    else {
        flag = XPFLAG_BIT;
    }

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "kCpFc:h")) > 0) {
        switch (opt) {
        case 'k':
            kernel_print = 1;
            break;
        case 'C':
            current_luts = 1;
            break;
        case 'p':
            parse_only = 1;
            break;
        case 'F':
            full_print = 1;
            break;
        case 'c':
            cno = atoi(optarg);
            break;
        case '?':
        case 'h':
            print_loadlut_help(flag);
            return -1;
        }
    }

    if (is_xpi) {
        cno = 1;
    }
    else if (is_i50) {
        if (cno != 1) {
            print_loadlut_help(flag);
            if (hp) hal_release(hp);
            return -1;
        }
    }
    else if (cno <= 0 || cno > MAX_FPGA_BOARDS) {
        print_loadlut_help(flag);
        if (hp) hal_release(hp);
        return -1;
    }
    else if (cno != 1) {
        if (hp) hal_release(hp);
        hp = hal_acquire(cno - 1);
    }

    if (hp == NULL) {
        printf("Failed. Could not access hardware\n");
        return -1;
    }

    if (kernel_print) {
        kernel_luts(hp, full_print);
        hal_release(hp);
        return 0;
    }

    if (current_luts) {
        print_current_luts(hp, full_print);
        hal_release(hp);
        return 0;
    }

    /**************************************
     * Read and parse the lut config file *
     **************************************/
    (void) parse_lut(hp, &lut_list_head);

    if (!parse_only) {
        hal_reset_lut(hp);
    }

    currentidx = 1;

    list_for_each(tmp, &lut_list_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        if (build_lut_entry(lutp, (is_i50) ? V2DI50_DEVICE : 
                    ((is_xpi) ? XPI_DEVICE : PLX_DEVICE)) < 0) {
            printf("Failed: Could not build LUT ID %d.\n", lutp->a_alutreg);
            break;
        }
        if (!parse_only) {
            if (hal_insert_lut(hp, lutp) < 0) {
                printf("Failed: Could not load LUT ID %d\n", lutp->a_alutreg);
                break;
            }
        }
        else {
            print_lut(lutp, currentidx, full_print);
        }
        currentidx++;
    }

    if (is_xpi) {
        hal_refresh_lut(hp);
    }

    if (!parse_only)
        printf("Loaded %d LUT entries from config file.\n", currentidx - 1);

    free_lut(&lut_list_head);
    hal_release(hp);
    return 0;
}

#define CREATE_PRINT_STATEMENT(membername, type)                        \
    len = printf("%s.%d", #membername, count);                      \
    printf("%*s " type "\n", 25 - len, " ", lutp->membername);

static void print_lut(struct LUT_struct *lutp, int count, int full_print)
{
    int len;

    if (lutp == NULL)
        return;

    printf("##################################################\n");
    CREATE_PRINT_STATEMENT(a_alutreg, "0x%x");
    CREATE_PRINT_STATEMENT(comment, "%s");
    CREATE_PRINT_STATEMENT(a_stereo, "0x%x");
    CREATE_PRINT_STATEMENT(a_hres, "0x%x");
    CREATE_PRINT_STATEMENT(a_vres, "0x%x");
    CREATE_PRINT_STATEMENT(a_orig_hres, "0x%x");
    CREATE_PRINT_STATEMENT(a_orig_vres, "0x%x");
    CREATE_PRINT_STATEMENT(a_refresh, "%f");
    CREATE_PRINT_STATEMENT(a_hfp, "0x%x");
    CREATE_PRINT_STATEMENT(a_hsync, "0x%x");
    CREATE_PRINT_STATEMENT(a_hbp, "0x%x");
    CREATE_PRINT_STATEMENT(a_vfp, "0x%x");
    CREATE_PRINT_STATEMENT(a_vsync, "0x%x");
    CREATE_PRINT_STATEMENT(a_vbp, "0x%x");
    CREATE_PRINT_STATEMENT(a_pcr, "%f");
    CREATE_PRINT_STATEMENT(a_lpcr, "%ld");
    CREATE_PRINT_STATEMENT(a_cspace, "%d");
    CREATE_PRINT_STATEMENT(a_rgain, "%d");
    CREATE_PRINT_STATEMENT(a_ggain, "%d");
    CREATE_PRINT_STATEMENT(a_bgain, "%d");
    CREATE_PRINT_STATEMENT(a_roffset, "%d");
    CREATE_PRINT_STATEMENT(a_goffset, "%d");
    CREATE_PRINT_STATEMENT(a_boffset, "%d");
    CREATE_PRINT_STATEMENT(a_hshift, "%d");
    CREATE_PRINT_STATEMENT(a_vshift, "%d");
    CREATE_PRINT_STATEMENT(a_phase, "%d");
    CREATE_PRINT_STATEMENT(a_noise, "0x%x");
    CREATE_PRINT_STATEMENT(a_vmargin, "0x%x");
    if (full_print) {
        CREATE_PRINT_STATEMENT(a_adcvco73, "0x%x");
        CREATE_PRINT_STATEMENT(a_cpmp, "0x%x");
        CREATE_PRINT_STATEMENT(a_amhreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_amvreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_hsyncc, "0x%x");
        CREATE_PRINT_STATEMENT(a_alutreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_fflwm, "0x%x");
        CREATE_PRINT_STATEMENT(a_ffhwm, "0x%x");
        CREATE_PRINT_STATEMENT(a_acreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_aafdreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avhareg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avvareg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avhdreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avvdreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcpll01, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcpll02, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcvco03, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcmc1, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcvco16, "0x%x");
        CREATE_PRINT_STATEMENT(d_dafdreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dcreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhareg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvareg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhfpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhsreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhbpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvfpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvsreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvbpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvbwreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds5, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds4, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds3, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds2, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds1, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds0, "0x%x");
        CREATE_PRINT_STATEMENT(d_ddsclk, "%f");
        CREATE_PRINT_STATEMENT(d_pixclck, "0x%x");
        CREATE_PRINT_STATEMENT(d_chgperbit, "0x%x");
    }
}

/*********************************************
 * Retrieve and print all LUTs in the kernel *
 *********************************************/
static void kernel_luts(HAL *hp, int full_print)
{
    struct LUT_struct lut;
    int count;

    lut.lut_list.next = NULL;

    count = 1;
    do {
        if (hal_retrieve_lut(hp, &lut) < 0)
            break;
       
        print_lut(&lut, count, full_print);

        count++;
    } while (lut.lut_list.next != NULL);
}

/******************************************************************
 * Retrieve and print current add/drop/display LUTs in the kernel *
 ******************************************************************/
static void print_current_luts(HAL *hp, int full_print)
{
    struct LUT_struct lut;
    int board_type = hal_get_board_type(hp);
    int hardware_type = hal_get_hardware_type(hp);

    memset(&lut, 0, sizeof(struct LUT_struct));

    if (hardware_type == V2DI50_DEVICE) {
        if (board_type == BOARDTYPE_TX) {
            if (hal_get_addside_lut(hp, &lut) == 0) {
                printf("\nCurrent Addside LUT:\n");
                print_lut(&lut, 1, full_print);
            }
            else 
                printf("Null Current Addside LUT\n");
        }
        else {
            if (hal_get_dropside_lut(hp, &lut) == 0) {
                printf("\nCurrent Dropside LUT:\n");
                print_lut(&lut, 1, full_print);
            }
            else
                printf("Null Current Dropside LUT\n");
        }
    }
    else {
        if (hal_get_addside_lut(hp, &lut) == 0) {
            printf("\nCurrent Addside LUT:\n");
            print_lut(&lut, 1, full_print);
        }
        else 
            printf("Null Current Addside LUT\n");

        if (board_type == BOARDTYPE_RX) {
            if (hal_get_dropside_lut(hp, &lut) == 0) {
                printf("\nCurrent Dropside LUT:\n");
                print_lut(&lut, 1, full_print);
            }
            else
                printf("Null Current Dropside LUT\n");
            if (hal_get_display_lut(hp, &lut) == 0) {
                printf("\nCurrent Display LUT:\n");
                print_lut(&lut, 1, full_print);
            }
            else
                printf("Null Current Display LUT\n");
        }
    }
}

/****************************************************************
 * To support genlock we need to force the RX to use a modified * 
 * LUT from what's sent from the TX. This command modifies the  *
 * current display LUT by the delta amounts specified.          *
 ****************************************************************/
static struct option modlut_options[] = {
    {"c", 1, 0, 0},
    {"hres", 1, 0, 0},
    {"vres", 1, 0, 0},
    {"hfp", 1, 0, 0},
    {"hsync", 1, 0, 0},
    {"hbp", 1, 0, 0},
    {"vfp", 1, 0, 0},
    {"vsync", 1, 0, 0},
    {"vbp", 1, 0, 0},
    {0, 0, 0, 0}
};

static void print_modlut_help(void)
{
    printf("\nUsage: modlut [OPTIONS]\n\n");
    printf("Modifies LUT configuration used for display.\n\n");
    printf("Options:\n");
    printf("\t-c cno\t Modify display LUT on channel 'cno'\n");
    printf("\t-hres delta\t Modify HRES by 'delta' pixels\n");
    printf("\t-vres delta\t Modify VRES by 'delta' lines\n");
    printf("\t-hfp delta\t Modify HFP by 'delta' pixels\n");
    printf("\t-hsync delta\t Modify HSYNC by 'delta' pixels\n");
    printf("\t-hbp delta\t Modify HBP by 'delta' pixels\n");
    printf("\t-vfp delta\t Modify VFP by 'delta' lines\n");
    printf("\t-vsync delta\t Modify VSYNC by 'delta' lines\n");
    printf("\t-vbp delta\t Modify VBP by 'delta' lines\n");
}

int modlut_main(int argc, char **argv)
{
    int c, cno = -1, opt_index = 0;
    int hres = 0, vres = 0, hfp = 0, hsync = 0, hbp = 0;
    int vfp = 0, vsync = 0, vbp = 0;
    HAL *hp = NULL;
    struct LUT_struct lut;

    while (1) {
        c = getopt_long_only(argc, argv, "h", modlut_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            switch (opt_index) {
            case 0:
                cno = atoi(optarg);
                break;
            case 1:
                hres = atoi(optarg);
                break;
            case 2:
                vres = atoi(optarg);
                break;
            case 3:
                hfp = atoi(optarg);
                break;
            case 4:
                hsync = atoi(optarg);
                break;
            case 5:
                hbp = atoi(optarg);
                break;
            case 6:
                vfp = atoi(optarg);
                break;
            case 7:
                vsync = atoi(optarg);
                break;
            case 8:
                vbp = atoi(optarg);
                break;
            }
        }
        else {
            print_modlut_help();
            return 0;
        }
    }

    /*************************
     * Check if system is XP *
     *************************/
    if (cno <= 0 || cno > MAX_FPGA_BOARDS) {
        printf("Invalid channel number %d\n", cno);
        return 0;
    }

    hp = hal_acquire(cno - 1);
    if (hp == NULL || hal_get_hardware_type(hp) != PLX_DEVICE) {
        printf("Channel %d not an XP board\n", cno);
        return 0;
    }

    if (hal_get_display_lut(hp, &lut) == 0) {

        printf("\nCurrent Display LUT:\n");
        print_lut(&lut, 1, 0);

        lut.a_hres += hres;
        lut.a_vres += vres;
        lut.a_hfp += hfp;
        lut.a_hsync += hsync;
        lut.a_hbp += hbp;
        lut.a_vfp += vfp;
        lut.a_vsync += vsync;
        lut.a_vbp += vbp;
        if (hal_set_display_lut(hp, &lut) < 0) {
            printf("Channel %d. Failed to modify display LUT\n", cno);
            return 0;
        }

        if (hal_get_display_lut(hp, &lut) == 0) {
            printf("\nModified Display LUT:\n");
            print_lut(&lut, 1, 0);
        }
        else {
            printf("Channel %d. Failed to get display LUT\n", cno);
        }
    }
    else {
        printf("Channel %d. Failed to get display LUT\n", cno);
    }

    hal_release(hp);

    return 0;
}
