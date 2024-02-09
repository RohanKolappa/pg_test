#include <getopt.h>


#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "saveVideoParamsToLUT.h"
#include "tx_video_params_menu_gen.h"
#include "xp_tx_video_params_menu_gen.h"
#include "rx_input_video_params_menu_gen.h"
#include "lut_utils.h"
#include "commonutils.h"
#include "cli_utils.h"
#include "system_utils.h"
#include "vlog.h"
#include "vutils.h"

#define VLOG_CATEGORY_NAME  "save_video_params_to_lut"

#define tx_cpmp_name_from_code(x) \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_50) ? "50"  : \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_100) ? "100" : \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_150) ? "150" : \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_250) ? "250" : \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_350) ? "350" : \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_500) ? "500" : \
        (((x) == TX_VIDEO_PARM_LUT_CPMP_750) ? "750" : \
        "Auto")))))))

#define xp_tx_cpmp_name_from_code(x) \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_50) ? "50"  : \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_100) ? "100" : \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_150) ? "150" : \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_250) ? "250" : \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_350) ? "350" : \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_500) ? "500" : \
        (((x) == XP_TX_VIDEO_PARM_LUT_CPMP_750) ? "750" : \
        "Auto")))))))

#define rx_input_cpmp_name_from_code(x) \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_50) ? "50"  : \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_100) ? "100" : \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_150) ? "150" : \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_250) ? "250" : \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_350) ? "350" : \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_500) ? "500" : \
        (((x) == RX_INPUT_VIDEO_PARM_LUT_CPMP_750) ? "750" : \
        "Auto")))))))

static int board_number = 0;

static struct option save_options[] = {
    {"channelNumber", 1, 0, 0},
    {0, 0, 0, 0}
};

static int save_current_video_params_to_LUT(HAL *hp) {
    struct LUT_struct lut;
    struct lut_list_params lutlist;
    char cmdbuf[256], buf[32];
    int hardware_type = hal_get_hardware_type(hp);
    int board_type = hal_get_board_type(hp);

    // Initialize LUT list parameters
    if (retrieveLUTListValues(hp) <= 0)
        return RC_ERROR;
    
    // Get current addside LUT from driver 
    if (hal_get_addside_lut(hp, &lut) < 0) 
        return RC_ERROR;

    // Get LUT list entry for addside LUT
    if (get_lut_list_params_by_lutid(&lutlist, lut.a_alutreg) == 0) {
        return RC_ERROR;
    }

    // Create the command for LUT change 
    sprintf(cmdbuf, "mod_lut_list -channelNumber %d", board_number + 1);

    if (hardware_type == PLX_DEVICE && board_type == BOARDTYPE_TX) {
        struct xp_tx_video_parm_params params;

        params.userptr = hp;
        retrieveXpTxVideoParametersValues(&params);

        if (params.phase_shift_set) {
            sprintf(buf, " -phase %d", params.phase_shift_value);
            strcat(cmdbuf, buf);
        }
        if (params.noise_fine_set) {
            sprintf(buf, " -fnoise %d", params.noise_fine_value);
            strcat(cmdbuf, buf);
        }
        if (params.noise_fine_set) {
            sprintf(buf, " -cnoise %d", params.noise_coarse_value);
            strcat(cmdbuf, buf);
        }
        if (params.horizontal_position_set) {
            sprintf(buf, " -hshift %d", params.horizontal_position_value);
            strcat(cmdbuf, buf);
        }
        if (params.vertical_position_set) {
            sprintf(buf, " -vshift %d", params.vertical_position_value);
            strcat(cmdbuf, buf);
        }
        if (params.lut_cpmp_set) {
            sprintf(buf, " -cpmp %s", 
                    xp_tx_cpmp_name_from_code(params.lut_cpmp_value));
            strcat(cmdbuf, buf);
        }
    }
    else if (hardware_type == V2DI50_DEVICE && board_type == BOARDTYPE_TX) {
        struct tx_video_parm_params params;

        params.userptr = hp;
        retrieveTxVideoParametersValues(&params);

        if (params.phase_shift_set) {
            sprintf(buf, " -phase %d", params.phase_shift_value);
            strcat(cmdbuf, buf);
        }
        if (params.noise_fine_set) {
            sprintf(buf, " -fnoise %d", params.noise_fine_value);
            strcat(cmdbuf, buf);
        }
        if (params.noise_fine_set) {
            sprintf(buf, " -cnoise %d", params.noise_coarse_value);
            strcat(cmdbuf, buf);
        }
        if (params.horizontal_position_set) {
            sprintf(buf, " -hshift %d", params.horizontal_position_value);
            strcat(cmdbuf, buf);
        }
        if (params.vertical_position_set) {
            sprintf(buf, " -vshift %d", params.vertical_position_value);
            strcat(cmdbuf, buf);
        }
        if (params.lut_cpmp_set) {
            sprintf(buf, " -cpmp %s", 
                    tx_cpmp_name_from_code(params.lut_cpmp_value));
            strcat(cmdbuf, buf);
        }
    }
    else if (hardware_type == PLX_DEVICE && board_type == BOARDTYPE_RX) {
        struct rx_input_video_parm_params params;

        params.userptr = hp;
        retrieveRxInputVideoParametersValues(&params);

        if (params.phase_shift_set) {
            sprintf(buf, " -phase %d", params.phase_shift_value);
            strcat(cmdbuf, buf);
        }
        if (params.horizontal_position_set) {
            sprintf(buf, " -hshift %d", params.horizontal_position_value);
            strcat(cmdbuf, buf);
        }
        if (params.vertical_position_set) {
            sprintf(buf, " -vshift %d", params.vertical_position_value);
            strcat(cmdbuf, buf);
        }
        if (params.lut_cpmp_set) {
            sprintf(buf, " -cpmp %s", 
                    rx_input_cpmp_name_from_code(params.lut_cpmp_value));
            strcat(cmdbuf, buf);
        }
    }
    else 
        return RC_NORMAL;

    sprintf(buf, " %d", lutlist.lut_list_id);
    strcat(cmdbuf, buf);

    // Run the command for LUT change 
    VLOG_DEBUG_CAT(VLOG_CATEGORY_NAME, "Executing command %s", cmdbuf);
    return system_command(cmdbuf);
}

int saveVideoParametersToLUT(MENUINFO * minfo)
{

    DIALOG *dialog;
    int retval = RC_NORMAL;
    int rc;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);
    
    // Get board number    
    HAL *hp;
    hp = minfo->minfo_userptr;
    if (hp != NULL) {
        board_number = hal_get_board_number(hp);
    } else {
        show_info(minfo,
                "Cannot Save: Wrong Hardware.\n"
                PRESSANYKEY_STR);
                    
        get_anykey(minfo);
        return RC_ERROR;
    }


    /**********************************
     * Don't let non-root users do it *
     **********************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 0,
                     "You must be root to save the current video parameters\n"
                     "to LUT.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /************************
     * Show initial message *
     ************************/
    dialog = show_dialog(minfo, 4, 0,
                         "This utility allows you to save the current video\n"
                         "parameters to LUT.\n");
    show_info(minfo, "Save current video parameters to LUT?" YESNO_STR);

    if (get_yesno(minfo)) {

        remove_dialog(dialog);

        dialog = show_dialog(minfo, 4, 0,
                     "</K></U>Warning:<!U><!K> The LUT is being updated...\n"
                     "Please do not interrupt the procedure.\n");

        rc = save_current_video_params_to_LUT(hp);

        remove_dialog(dialog);

        if (rc == 0)
            show_info(minfo,
                "Saved current video parameters to LUT.\n" PRESSANYKEY_STR);
        else 
            show_info(minfo, 
                "Error saving current video parameters to LUT.\n"
                PRESSANYKEY_STR);
        get_anykey(minfo);
    }
    else {
        remove_dialog(dialog);
    }
    return retval;
}

/*************************
 * Command Line Routines *
 *************************/
static char *save_video_params = "Save Video Parameters";

int save_video_params_to_lut_main(int argc, char **argv)
{
    int retval = RC_NORMAL;
    int opt_idx = 1;
    HAL *hp = NULL;
    int c, opt_index = 0;
    
    while (1) {
        c = getopt_long_only(argc, argv, "h", save_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            switch (opt_index) {
                case 0:
                    board_number = atoi(optarg) - 1;
                    break;
            }
        }
        else {
            retval = RC_ERROR;
            break;
        }
    }

    opt_idx = optind;

    hp = hal_acquire(board_number);

    if (hp == NULL) {
        print_cli_failure(save_video_params, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    if (view_only() != 0) {
        printf("You must be root to save video parameters to LUT\n");
        return RC_ERROR;
    }
    
    if (opt_idx == argc) {

        retval = save_current_video_params_to_LUT(hp);

        print_cli_success(save_video_params);
    }
    else {
        retval = RC_ERROR;
        print_cli_failure(save_video_params, CLI_REASON_BAD_USAGE);
        printf("\nUsage: save_video_params_to_lut -channelNumber Number\n");
        printf("\nSave current video parameters to LUT\n");
    }

    return retval;
}
