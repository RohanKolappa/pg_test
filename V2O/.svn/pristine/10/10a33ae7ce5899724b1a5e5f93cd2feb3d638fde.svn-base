
#include <getopt.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "adjustVideoParameters.h"
#ifndef __XPI__
#include "lut_table.h"
#endif
#include "lut_utils.h"
#include "vutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "cli_utils.h"

static char *phase_adjust = "Phase Adjust";
static char *phase_adjust_stage1 = "Phase Adjust Stage1";
static char *phase_adjust_stage2 = "Phase Adjust Stage2";
static char *phase_adjust_data = "Phase Adjust Data";
static char *phase_adjust_min = "Minimum Slice Rate";
static char *phase_adjust_lobound = "Lower Bound";
static char *phase_adjust_upbound = "Upper Bound";
static char *phase_adjust_mid = "Median";
static char *phase_name = "Phase";
static char *spf_name = "Slice Per Frame";

static struct option adjust_options[] = {
    {"coarseNoise", 1, 0, 0},
    {"fineNoise", 1, 0, 0},
    {"channelNumber", 1, 0, 0},
    {0, 0, 0, 0}
};

/*****************************************
 * Adjust Sampling Phase Video Parameter *
 *****************************************/
#define S_PHASE_SHIFT           "PHASE_SHIFT"

static int write_phase_shift_handler(char *name, 
                                     char *value, 
                                     void *ptr, 
                                     char flag) {

    int *shift = ptr;
    
    if (flag) {
        if (strcasecmp(name, S_PHASE_SHIFT) == 0) {
            sprintf(value, "%d", *shift);
            return 1;
        }
    }
    else {
        strcpy(name, S_PHASE_SHIFT);
        sprintf(value, "%d", *shift);
    }

    return 0;
}

static void set_sampling_phase(HAL *hp, int phase) {

    hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(phase));

    if (rewrite_config_file(VIDEO_CONFIG_FILE, 
                            write_phase_shift_handler, &phase) < 0) {
        VLOG_WARNING("Could not rewrite configuration file %s", 
                      VIDEO_CONFIG_FILE);
    }
}


int adjustVideoParameters(MENUINFO *minfo) {

    DIALOG *dialog;
    int board_type, hardware_type;
    struct fc_param_io fcpio;
    char strbuf[256];
    int old_phase, curr_phase, old_divider = 1;
    int old_coarse, old_fine, curr_coarse = 16, curr_fine = 16;
    int i, j, nl, lcnt;
    unsigned long base_frame_cnt, base_slice_cnt;
    double slice_per_frame, min_slice_per_frame;
    int mini, upboundi, loboundi, midi;
    double spfarray[32];
    char *cc;
    struct LUT_struct lut;
    struct LUT_struct current_alut;
    struct lut_list_params lutlist;
    unsigned char fpga_version;
    HAL *hp;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    hp = minfo->minfo_userptr;
    if (hp == NULL) {
        return show_fault_dialog(minfo, "Missing Hardware!\n");
    }

    board_type = hal_get_board_type(hp);

    if (board_type != BOARDTYPE_TX) {
        show_info(minfo, "Error: Not a Server. " PRESSANYKEY_STR);
        get_anykey(minfo);
        return RC_NORMAL;
    }

    /***********************
     * Show warning dialog *
     ***********************/
    dialog = show_dialog(minfo, 4, 0,
        "Running this test will disable server (disconnect all \n"
        "clients) and run a local test to estimate appropriate\n"
        "value for Sampling Phase.\n"
        "</K></U>Warning:<!U><!K> Make sure that you have a static frame\n"
        "(no movement on input video) before proceeding.");

    show_info(minfo, "Proceed with Auto Adjust?" YESNO_STR);

    if (!get_yesno(minfo)) {
        remove_dialog(dialog);
        return RC_NORMAL;
    }

    show_info(minfo, " ");

    remove_dialog(dialog);

    /**************************
     * Make window scrollable * 
     **************************/
    scrollok(minfo->minfo_menuscreen->window, TRUE);
    idlok(minfo->minfo_menuscreen->window, TRUE);

    if (board_type != BOARDTYPE_TX) {
        show_info(minfo, "Error: Not a Server. " PRESSANYKEY_STR);
        get_anykey(minfo);
        return RC_NORMAL;
    }

    /*********************************************
     * Make sure we have some input analog video * 
     *********************************************/
    if (!(hal_getActiveIntf(hp) & 
                (INTERFACE_HV|INTERFACE_SOG|INTERFACE_COMPOSITE))) {
        show_info(minfo, "Error: No input analog video. " PRESSANYKEY_STR);
        get_anykey(minfo);
        return RC_NORMAL;
    }

    current_alut.a_hres = 0;
    hal_get_addside_lut(hp, &current_alut);
    if (current_alut.a_hres == 0) {
        show_info(minfo, "Error: No input video detected. " PRESSANYKEY_STR);
        get_anykey(minfo);
        return RC_NORMAL;
    }
    
    /******************************************
     * Disable video freeze check. (There's   * 
     * no need to re-enable since the dserver *
     * will enable it when a client connects.)*
     ******************************************/
    hal_setVideoCheck(hp, 0);

    hardware_type = hal_get_hardware_type(hp);
    fpga_version = hal_get_fpga_val_k(hp, FPGA_IDREG) & 0x00ff;

    if (hardware_type != V2DI50_DEVICE || fpga_version >= 9) {
        old_divider = hal_get_framerate_divider(hp);
    }

    /* Turn off progressive compression */
    hal_set_progressive_compression(hp, 0);

    if (hardware_type != V2DI50_DEVICE) {
        hal_set_framerate_divider(hp, 2);
        /* Enable FPGA to count frames w/o reading */
        hal_set_framecnt_enable(hp, 1);
    }
    else {
        /* Disable server - "send_server_stopall doesn't seem to do */
        /* anything at the moment so we use "standalone" for now.   */
        system_command("/V2O/bin/standalone -r");
        if (hardware_type == XPI_DEVICE || fpga_version >= 9)
            hal_set_framerate_divider(hp, 2);

        /* Start tester to read video so we can count frames */
        scroll_print(minfo, "Starting Data Transfer.\n");
        if (hardware_type != XPI_DEVICE)
            system_command("/V2O/bin/tester -i 1000 &");
    }

    nl = hal_get_phase_adjust_k(hp);
    old_phase = sampling_phase_value_to_level(nl);;
    scroll_print(minfo, "Retrieved Sampling Phase %d\n", old_phase);

    hal_get_dc_noise_threshold(hp, &nl);
    old_coarse = dc_noise_value_to_level(nl);
    scroll_print(minfo, "Retrieved Coarse Noise Elimination %d\n", old_coarse);

    hal_get_ac_noise_threshold(hp, &nl);
    old_fine = ac_noise_value_to_level(nl);
    scroll_print(minfo, "Retrieved Fine Noise Elimination %d\n", old_fine);
    scroll_print(minfo, "Setting Coarse Noise Elimination to %d\n", 
            curr_coarse);
    hal_set_dc_noise_threshold(hp, dc_noise_level_to_value(curr_coarse));
    scroll_print(minfo, "Setting Fine Noise Elimination to %d\n", curr_fine);
    hal_set_ac_noise_threshold(hp, ac_noise_level_to_value(curr_fine));

    /***********************************************************
     * Start sampling slice rates at different sampling phases *
     ***********************************************************/
    memset(spfarray, 0x00, sizeof(spfarray));
    for (i = -16; i <= 15; i++) { 
        curr_phase = i; 
        scroll_print(minfo, "Setting Phase to %d\n", curr_phase);
        hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(curr_phase));
        usleep(100000);  /* pause for moment to let the hardware settle down */

        lcnt = 0;
        hal_get_fc_params(hp, &fcpio);
        base_frame_cnt = fcpio.frame_cnt;
        base_slice_cnt = fcpio.slice_cnt;
        while (lcnt++ < 30) {
            usleep(100000);
            hal_get_fc_params(hp, &fcpio);
            if ((fcpio.frame_cnt - base_frame_cnt) > 5)
                break;
        }

        if ((fcpio.frame_cnt - base_frame_cnt) > 0) {
            slice_per_frame = (double)(fcpio.slice_cnt - base_slice_cnt)/
                (fcpio.frame_cnt - base_frame_cnt);
            scroll_print(minfo, "Measured Slice Rate = %.2f in %.1f seconds\n",
                         slice_per_frame, (float)lcnt * 0.1);
            spfarray[i+16] = slice_per_frame;
        }
        else {
            scroll_print(minfo, "Unable to measure slice rate. Restarting.\n");
            spfarray[i+16] = (double) 1e+201;
            hal_resetAddsideVideo(hp);
        }
    }

    /******************************
     * Find Minima for slice rate *
     ******************************/
    mini = 16;
    min_slice_per_frame = spfarray[mini];
    for (i = 0; i < 32; i++) {
        if (spfarray[i] < min_slice_per_frame) {
            mini = i;
            min_slice_per_frame = spfarray[i];
        }
    }
    scroll_print(minfo, "Minimum Slice Rate %0.2f at phase %d\n", 
                 min_slice_per_frame, mini - 16);

    /***************************************************************
     * Sample slice rates again for greater duration over narrower *
     * range around the above minima.                              *
     ***************************************************************/
    for (i = mini - 16 - 3 ; i <= mini - 16 + 3; i++) {
        curr_phase = (i + 16 + 32)%32 - 16 ;
        scroll_print(minfo, "Setting Phase to %d\n", curr_phase);
        hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(curr_phase));
        usleep(100000);  /* pause for moment to let the hardware settle down */
        lcnt = 0;
        hal_get_fc_params(hp, &fcpio);
        base_frame_cnt = fcpio.frame_cnt;
        base_slice_cnt = fcpio.slice_cnt;
        while (lcnt++ < 30) {
            usleep(100000);
            hal_get_fc_params(hp, &fcpio);
            if ((fcpio.frame_cnt - base_frame_cnt) > 40)
                break;
        }
        if ((fcpio.frame_cnt - base_frame_cnt) > 0) {
            slice_per_frame = (double)(fcpio.slice_cnt - base_slice_cnt)/
                (fcpio.frame_cnt - base_frame_cnt);
            scroll_print(minfo, "Measured Slice Rate = %.2f in %.1f seconds\n",
                         slice_per_frame, (float)lcnt * 0.1);
            spfarray[(i+16)%32] = slice_per_frame;
        }
        else {
            scroll_print(minfo, 
                    "Unable to measure slice rate. Restarting...\n");
            spfarray[(i+16)%32] = (double) 1e+201;
            hal_resetAddsideVideo(hp);
        }
    }

    scroll_print(minfo, "Stopping Data Transfer\n");
    if (hardware_type != V2DI50_DEVICE) {
        hal_set_framecnt_enable(hp, 0);
        scroll_print(minfo, "Restoring Frame Rate Divider to %d\n", 
                old_divider);
        hal_set_framerate_divider(hp, old_divider);
    }
    else {
        if (hardware_type != XPI_DEVICE)
            system_command("/usr/bin/killall tester");
    }

    /******************************************
     * Restore original values for parameters *
     ******************************************/
    scroll_print(minfo, "Restoring Coarse Noise Elimination to %d\n", 
            old_coarse);
    hal_set_dc_noise_threshold(hp, dc_noise_level_to_value(old_coarse));
    scroll_print(minfo, "Restoring Fine Noise Elimination to %d\n", old_fine);
    hal_set_ac_noise_threshold(hp, ac_noise_level_to_value(old_fine));
    scroll_print(minfo, "Restoring Sampling Phase to %d\n", old_phase);
    hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(old_phase));
    hal_set_progressive_compression(hp, 1);

    if (hardware_type == V2DI50_DEVICE) {
        scroll_print(minfo, "Enabling Server.\n");
        hal_resetAddsideVideo(hp);
        /* Enable server - "send_server_startall doesn't seem to do */
        /* anything at the moment so we use "standalone" for now.   */
        system_command("/V2O/bin/standalone");
    }

    /****************
     * Print values *
     ****************/
    for (i = 0; i < 32; i+=4) {
        for (nl = 0, j = 0; j < 4; j++) {
            if (spfarray[i+j] > 1e+200)
                nl += sprintf(strbuf + nl, "%3d=INF ", i+j-16);
            else
                nl += sprintf(strbuf + nl, "%3d=%-6.2f ", i+j-16, spfarray[i+j]);
        }
        nl += sprintf(strbuf + nl, "\n");
        cc = strdup(strbuf);
        scroll_print(minfo, cc);
        free(cc);
    }

#ifdef WRITE_TO_FILE
    {
        FILE *fp = NULL;

        if (fp == NULL) {
            if ((fp = fopen("/tmp/datafile", "w+")) == NULL) {
                VLOG_WARNING("Unable to open data file for writing");
            }
        }
        if (fp != NULL) {
            for (i = 0; i < 32; i++) {
                if (spfarray[i] > 1e+200)
                    fprintf(fp, "%3d INF \n", i-16);
                else
                    fprintf(fp, "%3d %-6.2f \n", i-16, spfarray[i]);
            }
            fclose(fp);
            system_command("ftpput 192.168.1.215 -u rahul -p ____ . /tmp/datafile");
        }
    }
#endif

    /************************************
     * Find Minima for slice rate again *
     ************************************/
    nl = mini;
    mini = 16;
    min_slice_per_frame = spfarray[mini];
    for (i = 0; i < 32; i++) {
        if (spfarray[i] < min_slice_per_frame) {
            mini = i;
            min_slice_per_frame = spfarray[i];
        }
    }
    scroll_print(minfo, "Minimum Slice Rate %0.2f at phase %d\n", 
                 min_slice_per_frame, mini - 16);

    if (nl != mini) 
        scroll_print(minfo, "Earlier minimum was at %d\n", nl - 16);

    /*****************************************************************
     * Find Median for slice rate. Search upwands and downwards from *
     * Minima and see where the average of the next three value      *
     * changes more than 4 times the minima.                         *
     *****************************************************************/
    upboundi = (mini + 2)%32;
    for (i = 1; i < 32; i++) {
        j = mini + i;
        slice_per_frame = (double)(spfarray[j%32] + spfarray[(j+1)%32] + 
                                   spfarray[(j+2)%32])/3.0;
        if (slice_per_frame > 4 * min_slice_per_frame) {
            upboundi = j%32;
            break;
        }
    }    

    loboundi = (mini + 32 - 2)%32;
    for (i = 1; i < 32; i++) {
        j = mini + 64 - i;
        slice_per_frame = (double)(spfarray[j%32] + spfarray[(j-1)%32] + 
                                   spfarray[(j-2)%32])/3.0;
        if (slice_per_frame > 4 * min_slice_per_frame) {
            loboundi = j%32;
            break;
        }
    }    

    /*****************************************************************
     * Compute median as average of upper and lower bound. Since the *
     * range is cyclic we have to handle case where lower bound is   *
     * more than upper bound.                                        *
     *****************************************************************/
    if (loboundi < upboundi)
        midi = (loboundi + upboundi)/2;
    else
        midi = ((loboundi + upboundi + 32)/2) % 32;

    scroll_print(minfo, "Lower Bound = %d, Upper Bound = %d, Median = %d\n", 
                 loboundi - 16, upboundi - 16, midi - 16);

    sprintf(strbuf, "Recommended Sampling Phase is %d.  Do you want to\n"
            "switch to this value from current value of %d? "YESNO_STR , 
            mini - 16, old_phase);
    show_info(minfo, strbuf);

    if (get_yesno(minfo)) {
        set_sampling_phase(hp, mini - 16);
        sprintf(strbuf, "Configured Sampling Phase to %d. \n" PRESSANYKEY_STR, 
                mini - 16);
        memset(&lut, 0x00, sizeof(lut));

        hal_get_addside_lut(hp, &lut);
        if (lut.a_hres == 0) {
            show_info(minfo, strbuf);
            get_anykey(minfo);
            return RC_NORMAL;
        }

        if (get_lut_list_params_by_lutid(&lutlist, lut.a_alutreg)) {
            sprintf(strbuf, 
                "Save video parameters to current LUT:\n%s ?" YESNO_STR,
                lut.comment);
            show_info(minfo, strbuf);
            if (get_yesno(minfo)) {
                char buff[128];
                sprintf(buff, 
                    "mod_lut_list -alutreg %d -phase %d -channelNumber %d %d",
                    lut.a_alutreg, mini - 16, hal_get_board_number(hp)+1,
                    lutlist.lut_list_id);
                system_command(buff);
            }
        }
    }
    return RC_NORMAL;
}

static void print_phase_adjust_data(int idx, int ph, double spf) {
    printf("    " XML_OBJINDEX,  idx, phase_adjust_data);
    printf("      " XML_ATTRFLOAT, spf_name, spf);
    printf("    " XML_OBJEND);
}

int auto_phase_adjust_main(int argc, char **argv) {
    int board_type, hardware_type, cno = -1;
    struct fc_param_io fcpio;
    int old_phase, curr_phase;
    int old_coarse, old_fine, old_divider = 1;
    int curr_coarse = 16, curr_fine = 16;
    int i, j, nl, mini = 0, midi, lcnt, loboundi, upboundi;
    unsigned long base_frame_cnt, base_slice_cnt;
    double slice_per_frame, min_slice_per_frame;
    double spfarray1[32], spfarray2[32];
    int rc = RC_NORMAL;
    int opt_index = 0, c;
    struct LUT_struct current_alut;
    unsigned char fpga_version;
    HAL *hp;

    hp = hal_acquire(0);
    if (hp == NULL) {
        print_cli_failure(phase_adjust, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    hardware_type = hal_get_hardware_type(hp);

    fpga_version = hal_get_fpga_version(hp);

    while (1) {
        c = getopt_long_only(argc, argv, "h", adjust_options, &opt_index);
        if (c == -1) 
            break;
        else if (c == 0) {
            int value;
            if (opt_index == 0) {
                value = atoi(optarg);
                if (value >= 1 && value <= 256)
                    curr_coarse = value;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 1) {
                value = atoi(optarg);
                if (value >= 1 && value <= 2048)
                    curr_fine = value;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 2) {
                value = atoi(optarg);
                if (value >= 1 && value <= MAX_FPGA_BOARDS )
                    cno = value;
                else
                    rc = RC_INVALID_VALUE;
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }
        
    if (rc != RC_NORMAL || 
            ((hardware_type != PLX_DEVICE) && (cno != -1) && (cno != 1)) || 
            ((hardware_type == PLX_DEVICE) && 
             (cno <= 0 || cno > MAX_FPGA_BOARDS))) {
        print_cli_failure(phase_adjust, CLI_REASON_BAD_USAGE);
        printf("\nUsage: auto_phase_adjust [OPTIONS]\n");
        printf("Run automatic video phase adjustment\n");
        printf("Options\n");
        printf("\t-coarseNoise Value\tSet coarse noise to a value between 1 and 256\n");
        printf("\t-fineNoise Value\tSet fine noise to a value between 1 and 2048\n");
        printf("\t-channelNumber Number\tRun auto phase adjust for channel 'Number'\n");
        hal_release(hp);
        return rc;
    }

    if ((hardware_type != PLX_DEVICE) && cno != 1) {
        hal_release(hp);
        hp = hal_acquire(cno - 1);
        if (hp == NULL) {
            print_cli_failure(phase_adjust, CLI_REASON_WRONG_HW);
            return RC_ERROR;
        }
    }

    board_type = hal_get_board_type(hp);
    if (board_type != BOARDTYPE_TX) {
        print_cli_failure(phase_adjust, CLI_REASON_WRONG_HW);
        hal_release(hp);
        return RC_ERROR;
    }

    fpga_version = hal_get_fpga_version(hp); 

    /* Make sure we have some input video */
    current_alut.a_hres = 0;

    hal_get_addside_lut(hp, &current_alut);
    if (current_alut.a_hres == 0) {
        print_cli_failure(phase_adjust, CLI_REASON_NO_INPUT);
        hal_release(hp);
        return RC_ERROR;
    }

    if ((hardware_type == V2DI50_DEVICE) || fpga_version >= 9) {
        old_divider = hal_get_framerate_divider(hp);
    }    

    /* Turn off progressive compression */
    hal_set_progressive_compression(hp, 0);

    if (hardware_type != V2DI50_DEVICE) {
        hal_set_framerate_divider(hp, 2);
        /* Enable FPGA to count frames w/o reading */
        hal_set_framecnt_enable(hp, 1);
    }
    else {
        /* Disable server - "send_server_stopall doesn't seem to do */
        /* anything at the moment so we use "standalone" for now.   */
        system_command("/V2O/bin/standalone -r");
        if (hardware_type == XPI_DEVICE || fpga_version >= 9) {
            hal_set_framerate_divider(hp, 2);
        } 

        /* Start tester on XP to read video so we can count frames */
        if (hardware_type != XPI_DEVICE)
            system_command("/V2O/bin/tester -i 1000 &");
    }

    nl = hal_get_phase_adjust_k(hp);
    old_phase = sampling_phase_value_to_level(nl);;

    hal_get_dc_noise_threshold(hp, &nl);
    old_coarse = dc_noise_value_to_level(nl);

    hal_get_ac_noise_threshold(hp, &nl);
    old_fine = ac_noise_value_to_level(nl);

    hal_set_dc_noise_threshold(hp, dc_noise_level_to_value(curr_coarse));
    hal_set_ac_noise_threshold(hp, ac_noise_level_to_value(curr_fine));

    /***********************************************************
     * Start sampling slice rates at different sampling phases *
     ***********************************************************/
    for (i = -16; i <= 15; i++) {
        curr_phase = i;
        hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(curr_phase));
        usleep(100000);  /* pause for moment to let the hardware settle down */

        lcnt = 0;
        hal_get_fc_params(hp, &fcpio);
        base_frame_cnt = fcpio.frame_cnt;
        base_slice_cnt = fcpio.slice_cnt;

        while (lcnt++ < 30) {
            usleep(100000);
            hal_get_fc_params(hp, &fcpio);
            if ((fcpio.frame_cnt - base_frame_cnt) > 5)
                break;
        } 

        if ((fcpio.frame_cnt - base_frame_cnt) > 0) {
            slice_per_frame = (double)(fcpio.slice_cnt - base_slice_cnt)/
                                        (fcpio.frame_cnt - base_frame_cnt);
            spfarray2[i+16] = spfarray1[i+16] = slice_per_frame;
        }
        else {
            print_cli_failure(phase_adjust, CLI_REASON_BAD_FRAMECNT);
            rc = RC_ERROR;
        }
    }

    /******************************
     * Find Minima for slice rate *
     ******************************/
    mini = 16;
    min_slice_per_frame = spfarray1[mini];
    for (i = 0; i < 32; i++) {
        if (spfarray1[i] < min_slice_per_frame) {
            mini = i;
            min_slice_per_frame = spfarray1[i];
        }
    }

    /***************************************************************
     * Sample slice rates again for greater duration over narrower *
     * range around the above minima.                              *
     ***************************************************************/
    for (i = mini - 16 - 3 ; i <= mini - 16 + 3; i++) {
        curr_phase = (i + 16 + 32)%32 - 16 ;
        hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(curr_phase));
        usleep(100000);  /* pause for moment to let the hw settle down */
        lcnt = 0;
        hal_get_fc_params(hp, &fcpio);
        base_frame_cnt = fcpio.frame_cnt;
        base_slice_cnt = fcpio.slice_cnt;
        while (lcnt++ < 30) {
            usleep(100000);
            hal_get_fc_params(hp, &fcpio);
            if ((fcpio.frame_cnt - base_frame_cnt) > 40)
                break;
        }

        if ((fcpio.frame_cnt - base_frame_cnt) > 0) {
            slice_per_frame = (double)(fcpio.slice_cnt - base_slice_cnt)/
                                        (fcpio.frame_cnt - base_frame_cnt);
            spfarray2[(i+16)%32] = slice_per_frame;
        }
        else {
            print_cli_failure(phase_adjust, CLI_REASON_BAD_FRAMECNT);
            rc = RC_ERROR;
        }
    }

    if (hardware_type != V2DI50_DEVICE) {
        hal_set_framecnt_enable(hp, 0);
        if (hardware_type == XPI_DEVICE || fpga_version >= 9)
            hal_set_framerate_divider(hp, old_divider);
    }
    else {
        if (hardware_type != XPI_DEVICE)
            system_command("/usr/bin/killall tester");
    }

    /******************************************
     * Restore original values for parameters *
     ******************************************/
    hal_set_dc_noise_threshold(hp, dc_noise_level_to_value(old_coarse));
    hal_set_ac_noise_threshold(hp, ac_noise_level_to_value(old_fine));
    hal_set_phase_adjust_k(hp, sampling_phase_level_to_value(old_phase));
    hal_set_progressive_compression(hp, 1);

    if (hardware_type == V2DI50_DEVICE) {
        hal_resetAddsideVideo(hp);
        /* Enable server - "send_server_startall doesn't seem to do */
        /* anything at the moment so we use "standalone" for now.   */
        system_command("/V2O/bin/standalone");
    }

    hal_release(hp);

    if (rc == RC_ERROR) {
        return rc;
    }

    /************************************
     * Find Minima for slice rate again *
     ************************************/
    nl = mini;
    mini = 16;
    min_slice_per_frame = spfarray2[mini];
    for (i = 0 ; i < 32; i++) {
        if (spfarray2[i] < min_slice_per_frame) {
            mini = i;
            min_slice_per_frame = spfarray2[i];
        }
    }

    /*****************************************************************
     * Find Median for slice rate. Search upwands and downwards from *
     * Minima and see where the average of the next three value      *
     * changes more than 4 times the minima.                         *
     *****************************************************************/
    upboundi = (mini + 2)%32;
    for (i = 1; i < 32; i++) {
        j = mini + i;
        slice_per_frame = (double)(spfarray2[j%32] + spfarray2[(j+1)%32] + 
                                   spfarray1[(j+2)%32])/3.0;
        if (slice_per_frame > 4 * min_slice_per_frame) {
            upboundi = j%32;
            break;
        }
    }    

    loboundi = (mini + 32 - 2)%32;
    for (i = 1; i < 32; i++) {
        j = mini + 64 - i;
        slice_per_frame = (double)(spfarray2[j%32] + spfarray2[(j-1)%32] + 
                                   spfarray2[(j-2)%32])/3.0;
        if (slice_per_frame > 4 * min_slice_per_frame) {
            loboundi = j%32;
            break;
        }
    }    

    /*****************************************************************
     * Compute median as average of upper and lower bound. Since the *
     * range is cyclic we have to handle case where lower bound is   *
     * more than upper bound.                                        *
     *****************************************************************/
    if (loboundi < upboundi)
        midi = (loboundi + upboundi)/2;
    else
        midi = ((loboundi + upboundi + 32)/2) % 32;

    printf(XML_GROUP, phase_adjust);
    printf("  " XML_RESPONSE);
    printf("    " XML_ATTRSTR, CLI_STATUS_NAME, CLI_STATUS_SUCCESS);
    printf("    " XML_ATTRSTR, CLI_REASON_NAME, CLI_REASON_NA);
    printf("  " XML_RESPONSEEND);
    printf("  " XML_OBJENTRY, phase_adjust_min);
    printf("      " XML_ATTRINT, phase_name, mini - 16);
    printf("      " XML_ATTRINT, phase_adjust_lobound, loboundi - 16);
    printf("      " XML_ATTRINT, phase_adjust_upbound, upboundi - 16);
    printf("      " XML_ATTRINT, phase_adjust_mid, midi - 16);
    printf("      " XML_ATTRFLOAT, spf_name, min_slice_per_frame);
    printf("  " XML_OBJEND);
    printf("  " XML_TABLE, phase_adjust_stage1);
    for (i = 0; i < 32; i++) {
        print_phase_adjust_data(i + 1, i - 16, spfarray1[i]); 
    } 
    printf("  " XML_TABEND);
    printf("  " XML_TABLE, phase_adjust_stage2);
    for (i = 0 ; i < 32; i++) {
        print_phase_adjust_data(i + 1, i - 16, spfarray2[i]); 
    } 
    printf("  " XML_TABEND);
    printf(XML_GROUPEND);
    return RC_NORMAL;
}

